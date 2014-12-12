#ifndef __SENDMAIL_H__
#define __SENDMAIL_H__

#include <IdSSLOpenSSL.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdSMTP.hpp>
#include <IdReplySMTP.hpp>

#include "unilist.h"
#include "letter.h"

/* Для отправки письма нужно создать экземляр объекта TLetter (см. letter.h)
 * и присвоить ему значения полей:
 *
 *   server - адрес сервера к котому подключаться по SMTP
 *   port   - порт сервера
 *   username - има пользователя, для аутентификации
 *   password - пароль, для аутентификации
 *   ssl - 0 нет (незащищенная авторизация),
 *         1 использовать SSL сразу (обычно 465 порт),
 *         2 использовать TLS после того как сервер разрешил (обычно 587 порт)
 *
 *   sender - e-mail адрес отправителя
 *   sender_name - имя отправителя
 *   recipient - e-mail адрес получателя
 *   subject - тема письма
 *   message - сообщение
 *
 *   callback - функция, которая вудет вызвана по отправке письма или ошибке.
 *              Аргументы:
 *                self - объект TLetter; в его поле error
 *                       будет текст с описанием ошибки если result == false
 *   closure - то же самое что и callback, но с добавлением указателя на объект.
 *             Позволяет вызывать функции, являющиеся методами классов.
 * 
 * После этого нужно вызвать статическую функцию TSendmailThread::Send()
 * Она создаст поток отправки писем, поставит письмо в очередь на отправку и
 * вернет управление мгновенно.
 * О результате отправки письма можно узнать с помощью callback / closure
 * При завершении приложения нужно вызвать статическую функцию TSendmailThread::Exit()
 * для завершения работы потока отправки писем.
 */

//---------------------------------------------------------------------------
class TSendmailThread : public TThread
{
private:
  TIdSMTP * smtp;             // indy
  TIdSSLIOHandlerSocketOpenSSL * SSLIOHandler;
  TUniList<TLetter> letters;  // list of letters
  TLetter * SyncLetter;       // Pointer for sync()

private:
  bool SendInternal(TLetter * p);
  void __fastcall sync(void);
  void __fastcall iiso(char &VHeaderEncoding, AnsiString &VCharSet);

protected:
  void __fastcall Execute();

public:
  __fastcall TSendmailThread(void);
  __fastcall ~TSendmailThread();
  static void __fastcall Send(TLetter * p);
  static void __fastcall Exit(void);
};
//---------------------------------------------------------------------------
#endif

