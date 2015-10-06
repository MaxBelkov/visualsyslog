//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "tcpcon.h"
#include "cfg.h"
#include "messageform.h"
#include "server.h"
#include "fdb.h"

//---------------------------------------------------------------------------
TSyslogdTcpConn::TSyslogdTcpConn(MSocket * p)
{
  Socket = p;
  Data = NULL;
  Size = 0;
  DataSize = 0;
}
//---------------------------------------------------------------------------
TSyslogdTcpConn::~TSyslogdTcpConn()
{
  if( Data )
    delete [] Data;
  if( Socket )
  {
    Socket->Close();
    delete Socket;
  }
}
//---------------------------------------------------------------------------
BYTE * TSyslogdTcpConn::GetBufferSize(int NeedFreeSize)
{
  if( Size - DataSize < NeedFreeSize )
  {
    Size = NeedFreeSize + DataSize;
    BYTE * p = new BYTE[Size];
    if( Data )
    {
      memmove(p, Data, DataSize);
      delete [] Data;
    }
    Data = p;
  }
  return Data + DataSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

extern TStorageFileList * fdb;
extern TMainCfg MainCfg;
bool WriteToLogError(String fmt, ...);
bool WriteToLogRawMessage(char * p);
void PrintSB(int i, String s);
bool ProcessMessageRules(TSyslogMessage * p);

MTCPServer * tcp = NULL;
TList * tcp_cons = NULL;
//---------------------------------------------------------------------------
bool TcpServerCreate(void)
{
  tcp_cons = new TList();
  tcp = new MTCPServer();
  if( tcp->GetError() )
  {
    String err = String("Error tcp: ") + tcp->GetErrorMessageEng();
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
    return false;
  }
  else
    return true;
}
//---------------------------------------------------------------------------
void TcpServerDestroy(void)
{
  delete tcp;
  tcp = NULL;
  delete tcp_cons;
  tcp_cons = NULL;
}
//---------------------------------------------------------------------------
void TcpServerStart(void)
{
  if( ! MainCfg.TcpEnable )
  {
    TcpPrintStatus();
    return;
  }

  if( tcp->Open(MainCfg.TcpInterface.c_str(), MainCfg.TcpPort) )
  {
    TcpPrintStatus();
  }
  else
  {
    TcpPrintStatus();
    String err = String("Error tcp: ") + tcp->GetErrorMessageEng() +
      " [tcp port " + IntToStr(MainCfg.TcpPort) + "]";
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
  }
}
//---------------------------------------------------------------------------
void TcpServerStop(void)
{
  if( tcp->IsOpen() )
  {
    TcpCloseAllConnections();
    tcp->Close();
    TcpPrintStatus();
  }
}
//---------------------------------------------------------------------------
void TcpAccept(void)
{
  // Have new TCP connection ?
  MSocket * p = tcp->GetClient(1); // 1 msec
  if( p )
  {
    // yes
    TcpAddConnection(p);
  }
  else
  {
    if( tcp->errcode )
      WriteToLogError("ERROR\tTCP connection: %s code=%ld",
        tcp->GetErrorMessageEng().c_str(), tcp->errcode);
  }
}
//---------------------------------------------------------------------------
void TcpAddConnection(MSocket * p)
{
  tcp_cons->Add( new TSyslogdTcpConn(p) );
  TcpPrintStatus();
}
//---------------------------------------------------------------------------
void TcpDeleteConnection(int index)
{
  if( index<0 || index>=tcp_cons->Count )
    return;
  delete (TSyslogdTcpConn *)tcp_cons->Items[index];
  tcp_cons->Delete(index);
  TcpPrintStatus();
}
//---------------------------------------------------------------------------
void TcpCloseAllConnections(void)
{
  // Close all existing connections
  for(int i=0; i<tcp_cons->Count; i++)
  {
    delete (TSyslogdTcpConn *)tcp_cons->Items[i];
  }
  tcp_cons->Clear();
}
//---------------------------------------------------------------------------
int TcpGetConnectionsCount(void)
{
  return tcp_cons->Count;
}
//---------------------------------------------------------------------------
void TcpPrintStatus(void)
{
  if( ! MainCfg.TcpEnable )
  {
    PrintSB(1, "TCP: server disabled");
  }
  else
  {
    if( tcp->IsOpen() )
      PrintSB(1, String("TCP ") + tcp->GetLocalAddrPort() +
        " [" + IntToStr(TcpGetConnectionsCount()) + "]");
    else
      PrintSB(1, "TCP: server not started");
  }
}
//---------------------------------------------------------------------------
void TcpReceiveMessage(void)
{
  int rl;
  TSyslogdTcpConn * c;
  MSocket * p;
  for(int i=0; i<tcp_cons->Count; i++)
  {
    c = (TSyslogdTcpConn *)tcp_cons->Items[i];
    p = c->Socket;

    if( p->Poll(true, false, false, 0) ) // wait 0 msec
    {
      rl = p->ReadLength();

      if( rl > 0 )
      {
        // Ready to receive rl bytes
        if( p->Read(c->GetBufferSize(rl), rl) )
        {
          c->DataSize += rl;
        }
        else
        {
          if( p->bytes > 0 && p->bytes < rl )
          {
            c->DataSize += p->bytes;
            WriteToLogError("WARNING\tTCP received %ld, wait %ld, error=%ld", p->bytes, rl, p->errcode);
          }
          else
          {
            if( p->errcode==WSAESHUTDOWN )
            {
              WriteToLogError("WARNING\tTCP shutdown from %s", p->GetRemoteAddrPort().c_str());
              TcpDeleteConnection(i--);
              continue;
            }
            else
            {
              WriteToLogError("ERROR\tTCP read(rl=%ld) from %s\tbytes=%ld error=%ld",
                p->GetRemoteAddrPort().c_str(), rl, p->bytes, p->errcode);
            }
          }
        }
      }
      else // Readed 0 bytes !
      {
        BYTE b;
        if( ! p->Read(&b, 1) )
        {
          if( p->errcode==0 && p->bytes==0 )
          {
            // Good disconnect
            TcpDeleteConnection(i--);
            continue;
          }
          else
          {
            WriteToLogError("ERROR\tTCP error from %s: %s [%d]",
              p->GetRemoteAddrPort().c_str(), p->GetErrorMessageEng().c_str(), p->errcode);
            TcpDeleteConnection(i--);
            continue;
          }
        }
      }
    } // Poll
    if( c->DataSize > 0 )
    {
      int i, start;
      for(i=start=0; i<c->DataSize; i++)
      {
        // find end of message
        if( c->Data[i]==0 || c->Data[i]=='\n' || c->Data[i]=='\r' )
        {
          if( start < i )
          {
            // process messages

            // NULL terminate syslog message
            c->Data[i] = 0;

            WriteToLogRawMessage((char *)(c->Data + start));

            TSyslogMessage sm;
            sm.FromStringSyslogd((char *)(c->Data + start), i - start, &c->Socket->destAddr);
            if( ProcessMessageRules(&sm) )
            {
              // option is not set: "Ignore (do not save to the default file "syslog")"
              TStorageFile * sf = fdb->Get(0);
              if( sf )
                if( ! sf->Save( sm.ToString() ) )
                  WriteToLogError("ERROR\tSave message to file: %s", sf->GetFileName().c_str());
            }
          }
          start = i + 1;
        }
      }
      // half of syslog message in c->Data ?
      if( start < i )
      {
        // Save to buffer start
        memmove(c->Data, c->Data + start, i - start);
        c->DataSize = i - start;
      }
      else
      {
        c->DataSize = 0;
      }
    }
  } // End for
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

