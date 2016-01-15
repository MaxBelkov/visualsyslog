//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "utils.h"
#include "letter.h"

//---------------------------------------------------------------------------
TLetter::TLetter()
{
  port = 25;
  ssl = SMTP_SSL_OFF;
  callback = NULL;
  closure = NULL;
  data = NULL;
}
//---------------------------------------------------------------------------
TLetter::TLetter(const TLetter & l)
{
  *this = l;
}
//---------------------------------------------------------------------------
void TLetter::operator = (const TLetter & l)
{
  server = l.server;
  port = l.port;
  username = l.username;
  password = l.password;
  ssl = l.ssl;

  sender = l.sender;
  sender_name = l.sender_name;
  recipient = l.recipient;
  subject = l.subject;
  message = l.message;

  data = l.data;

  callback = l.callback;
  closure = l.closure;

  error = l.error;
  result = l.result;
}
//---------------------------------------------------------------------------
void TLetter::SetPredefinedServer(TSmtpServerType st)
{
  switch( st )
  {
    case SMTP_MAIL_RU:
      server = "smtp.mail.ru";
      port = 465;
      ssl = SMTP_SSL_ON;
    break;
    case SMTP_YANDEX_RU:
      server = "smtp.yandex.ru";
      port = 465;
      ssl = SMTP_SSL_ON;
    break;
    case SMTP_ICLOUD:
      server = "smtp.mail.me.com";
      port = 587;
      ssl = SMTP_SSL_TLS;
    break;
    case SMTP_GOOGLE:
      server = "smtp.gmail.com";
      port = 465;
      ssl = SMTP_SSL_ON;
    break;
  }
}
//---------------------------------------------------------------------------
void TLetter::Save(XMLElementEx * p)
{
  // smtp server params
  p->ws("server", server);
  p->wi("port", port);
  p->ws("username", username);
  p->ws("password", SecurePassword(password));
  p->wi("ssl", ssl);

  // message params
  p->ws("sender", sender);
  p->ws("sender_name", sender_name);
  p->ws("recipient", recipient);
  p->ws("subject", subject);
  p->ws("message", message);
}
//---------------------------------------------------------------------------
void TLetter::Load(XMLElementEx * p)
{
  // smtp server params
  server = p->rs("server");
  port = p->ri("port", 25);
  username = p->rs("username");
  password = UnsecurePassword(p->rs("password"));
  ssl = (TSmtpSSLType)p->ri("ssl", SMTP_SSL_OFF);

  // message params
  sender = p->rs("sender");
  sender_name = p->rs("sender_name");
  recipient = p->rs("recipient");
  subject = p->rs("subject");
  message = p->rs("message");
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
