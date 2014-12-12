#include <vcl.h>
#pragma hdrstop

#include "base64.h"
#include "sendmail.h"

TSendmailThread * SendmailThread = NULL;

//---------------------------------------------------------------------------
//static
String smtpError(int code)
{
  switch(code)
  {
	case 211: return "System status, or system help reply";
	case 214: return "Help message";
	case 220: return "Service ready ";
	case 221: return "Service closing transmission channel ";
	case 250: return "Requested mail action okay, completed ";
	case 251: return "User not local; will forward to ";
	case 354: return "Start mail input; end with \".\"";
	case 421: return "Service not available, closing transmission channel";
	case 450: return "Requested mail action not taken: mailbox unavailable";
	case 451: return "Requested action aborted: local error in processing";
	case 452: return "Requested action not taken: insufficient system storage";
	case 500: return "Syntax error, command unrecognized";
	case 501: return "Syntax error in parameters or arguments";
	case 502: return "Command not implemented";
	case 503: return "Bad sequence of commands";
	case 504: return "Command parameter not implemented";
    case 512: return "The host server for the recipientТs domain name cannot be found (DNS error)";
    case 513: return "Address type is incorrect or authentication required";
    case 523: return "The RecipientТs mailbox cannot receive messages this big";
    case 530: return "Access denied";
    case 535: return "Username and Password not accepted";
    case 541: return "Recipient Address Rejected Ц Access denied";
	case 550: return "Requested action not taken: mailbox unavailable";
	case 551: return "User not local; please try <....>";
	case 552: return "Requested mail action aborted: exceeded storage allocation";
	case 553: return "Requested action not taken: mailbox name not allowed";
	case 554: return "Transaction failed";
  }
  return String().sprintf("Unknown(%d)", code);
}
//---------------------------------------------------------------------------
static AnsiString ToUft8Header(AnsiString s)
{
  // do not encode empty string
  if( s.Length() == 0 )
    return s;
  s = UTF8Encode(s);
  char * p = base64_encode(s.c_str(), s.Length());
  AnsiString rv("=?utf-8?B?");
  rv += p;
  rv += "?=";
  delete [] p;
  return rv;
}
//---------------------------------------------------------------------------
__fastcall TSendmailThread::TSendmailThread()
  : TThread(true)
{
  letters.EnableMultiTreading();
  smtp = new TIdSMTP(NULL);
  SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL(NULL);
  smtp->IOHandler = SSLIOHandler;
}
//---------------------------------------------------------------------------
__fastcall TSendmailThread::~TSendmailThread()
{
  delete smtp;
  smtp = NULL;
  delete SSLIOHandler;
  SSLIOHandler = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TSendmailThread::Execute()
{
  TLetter * p;
  while( ! Terminated )
  {
    p = letters.Extract(0);
    if( ! p )
    {
      Sleep(10);
      continue;
    }

    if( SendInternal(p) )
    {
    }
    else
    {
      //p->error
    }
    delete p;
  }
}
//---------------------------------------------------------------------------
void __fastcall TSendmailThread::Exit(void)
{
  if( SendmailThread )
  {
    SendmailThread->Terminate();
    SendmailThread->WaitFor();
    delete SendmailThread;
    SendmailThread = NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall TSendmailThread::Send(TLetter * p)
{
  if( ! SendmailThread )
  {
    SendmailThread = new TSendmailThread;
    SendmailThread->Resume();
  }
  TLetter * n = new TLetter;
  *n = *p;
  SendmailThread->letters.Add(n);
}
//---------------------------------------------------------------------------
bool TSendmailThread::SendInternal(TLetter * p)
{
  bool rv = false;
  TIdMessage * m = NULL;
  try
  {
    m = new TIdMessage(NULL);

    m->OnInitializeISO = iiso;
    //m->ContentTransferEncoding = "base64";
    m->ContentType = "text/plain";
    m->CharSet = "utf-8";

    // без Sender попадем в спам
    m->Sender->Name = ToUft8Header(p->sender_name);
    m->Sender->Address = p->sender;

    m->From->Name = p->sender_name;
    m->From->Address = p->sender;

    m->Recipients->EMailAddresses = p->recipient;

    m->Subject = p->subject;
    m->Body->Text = p->message;

    smtp->Host = p->server;
    smtp->Port = p->port;
    smtp->Username = p->username;
    smtp->Password = p->password;

    switch( p->ssl )
    {
      case 0: // нет (незащищенна€ авторизаци€)
        smtp->UseTLS = utNoTLSSupport;
      break;
      case 1: // использовать SSL сразу (обычно 465 порт)
        smtp->UseTLS = utUseImplicitTLS;
      break;
      case 2: // использовать TLS после того как сервер разрешил (обычно 587 порт)
        smtp->UseTLS = utUseRequireTLS;
      break;
    }

    if( smtp->Host.IsEmpty() )
      throw EIdException("smtp server address not defined");

    smtp->Connect();

	smtp->Send(m);

    p->error = "";
    rv = true;
  }
  catch(EIdSMTPReplyError &e)
  {
    p->error.printf("%s (%s [%d])", e.Message.c_str(),
      smtpError(e.ErrorCode).c_str(), e.ErrorCode);
  }
  catch(EIdException &e)
  {
    p->error.printf("%s", e.Message.c_str());
  }
  catch(...)
  {
    p->error = "Exception";
  }

  for(int i=1, l=p->error.Length(); i<=l; i++)
    if( p->error[i]=='\r' || p->error[i]=='\n' )
      p->error[i] = ' ';

  p->result = rv;

  try
  {
    smtp->Disconnect();
  }
  catch(...)
  {}

  delete m;

  if( p->callback || p->closure )
  {
    SyncLetter = p;
	Synchronize(sync);
  }

  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TSendmailThread::iiso(char &VHeaderEncoding, AnsiString &VCharSet)
{
  VHeaderEncoding = 'B';
  VCharSet = "utf-8";
}
//---------------------------------------------------------------------------
void __fastcall TSendmailThread::sync(void)
{
  try
  {
    if( SyncLetter->callback)
      SyncLetter->callback(SyncLetter);
    if( SyncLetter->closure )
      SyncLetter->closure(SyncLetter);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------

