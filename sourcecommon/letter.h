//---------------------------------------------------------------------------
#ifndef letterH
#define letterH

#include "maxxml.h"

// Predefined smtp server profiles:
enum TSmtpServerType { SMTP_MAIL_RU, SMTP_YANDEX_RU, SMTP_ICLOUD, SMTP_GOOGLE };
// SMTP server type
enum TSmtpSSLType {
  SMTP_SSL_OFF, // do not use ssl
  SMTP_SSL_ON,  // use ssl immediately (usually port 465)
  SMTP_SSL_TLS  // use TLS after the server allowed (usually port 587)
                  };

//---------------------------------------------------------------------------
class TLetter
{
public:
  // smtp server params
  String server;
  int port;
  String username;
  String password;
  TSmtpSSLType ssl;

  // message params
  String sender;
  String sender_name;
  String recipient;
  String subject;
  String message;

  // user data
  void * data;
    
public:
  void (* callback)(TLetter * self);
  void __fastcall (__closure * closure)(TLetter * self);

  // The result of sending a letter
  bool result;    // true - sent false - NOT sent
  String error;   // description of the error

public:
  TLetter();
  TLetter(const TLetter & l);
  void operator = (const TLetter & l);
  void SetPredefinedServer(TSmtpServerType st);
  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
#endif
