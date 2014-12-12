//---------------------------------------------------------------------------
#ifndef letterH
#define letterH

#include "maxxml.h"

// Predefined smtp server profiles:
#define SMTP_MAIL_RU 0
#define SMTP_YANDEX_RU 1
#define SMTP_ICLOUD  2
#define SMTP_GOOGLE  3
//---------------------------------------------------------------------------
class TLetter
{
public:
    // smtp server params
    String server;
    int port;
    String username;
    String password;
    int ssl;

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
    void operator = (TLetter & l);
    void SetPredefinedServer(int profile);
    void Save(XMLElementEx * p);
    void Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
#endif
