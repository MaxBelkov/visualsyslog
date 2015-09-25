//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "udp.h"         // udp server
#include "cfg.h"
#include "messageform.h"
#include "server.h"
#include "udpcon.h"
#include "fdb.h"

TUDP * udp = NULL;

extern TStorageFileList * fdb;
extern TMainCfg MainCfg;
bool WriteToLogError(String fmt, ...);
bool WriteToLogRawMessage(char * p);
void PrintSB(int i, String s);
bool ProcessMessageRules(TSyslogMessage * p);

//---------------------------------------------------------------------------
bool UdpServerCreate(void)
{
  udp = new TUDP();
  if( udp->GetError() )
  {
    String err = String("Error udp: ") + udp->GetErrorMessageEng();
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
    return false;
  }
  else
    return true;
}
//---------------------------------------------------------------------------
void UdpServerDestroy(void)
{
  delete udp;
  udp = NULL;
}
//---------------------------------------------------------------------------
void UdpServerStart(void)
{
  if( ! MainCfg.UdpEnable )
  {
    PrintSB(0, "UDP: server disabled");
    return;
  }

  if( udp->Open(MainCfg.UdpInterface.c_str(), MainCfg.UdpPort, "0.0.0.0", 0) )
  {
    PrintSB(0, String("UDP ") + udp->GetLocalAddrPort());
  }
  else
  {
    PrintSB(0, "UDP: server not started");
    String err = String("Error udp: ") + udp->GetErrorMessageEng() +
      " [udp port " + IntToStr(MainCfg.UdpPort) + "]";
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
  }
}
//---------------------------------------------------------------------------
void UdpServerStop(void)
{
  if( udp->IsOpen() )
  {
    PrintSB(0, "UDP: server not started");
    udp->Close();
  }
}
//---------------------------------------------------------------------------
void UdpReceiveMessage(void)
{
  if( ! udp )
    return;
  if( ! udp->IsOpen() )
    return;

  struct sockaddr_in a;
  DWORD rl;
  while( (rl = udp->ReadLength()) > 0 )
  {
    BYTE * ReceiveBuffer = new BYTE[rl + 1];
    if( ! udp->ReadFrom(ReceiveBuffer, rl, (struct sockaddr *)&a) )
    {
      if( udp->GetWSAError() == MYERROR_READCOUNT )
        ; // Good error :) In the receive buffer still have UDP packets
      else
      {
        WriteToLogError(String("ERROR\tudp: ") + udp->GetErrorMessageEng());
      }
    }

    if( udp->bytes > 0 )
    {
      // NULL terminate syslog message
      ReceiveBuffer[udp->bytes] = 0;

      WriteToLogRawMessage((char *)ReceiveBuffer);

      TSyslogMessage sm;
      sm.FromStringSyslogd((char *)ReceiveBuffer, udp->bytes, &a);

      if( ProcessMessageRules(&sm) )
      {
        // option is not set: "Ignore (do not save to the default file "syslog")"
        TStorageFile * sf = fdb->Get(0);
        if( sf )
          if( ! sf->Save( sm.ToString() ) )
            WriteToLogError("ERROR\tSave message to file: %s", sf->GetFileName().c_str());
      }
    }
    delete [] ReceiveBuffer;
  }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
