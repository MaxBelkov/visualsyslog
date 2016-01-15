#ifndef __SENDMAIL_H__
#define __SENDMAIL_H__

#include <IdSSLOpenSSL.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdSMTP.hpp>
#include <IdReplySMTP.hpp>

#include "unilist.h"
#include "letter.h"

/* TSendmailThread - класс потока для отправки E-Mail (объект TLetter)
   Используется библиотека Indy.Sockets (VCL) version 10
   (http://www.indyproject.org/Sockets/index.EN.aspx)
   Автор: Бельков М.

   Для отправки E-Mail нужно создать экземляр объекта TLetter (см. letter.h)
   и присвоить ему значения полей:

     server - адрес сервера к котому подключаться по SMTP
     port   - порт сервера
     username - има пользователя, для аутентификации
     password - пароль, для аутентификации
     ssl - SMTP_SSL_OFF нет (незащищенная авторизация),
           SMTP_SSL_ON  использовать SSL сразу (обычно 465 порт),
           SMTP_SSL_TLS использовать TLS после того как сервер разрешил (обычно 587 порт)

     sender - e-mail адрес отправителя
     sender_name - имя отправителя
     recipient - e-mail адрес получателя
     subject - тема письма
     message - сообщение

     callback - функция, которая вудет вызвана по отправке письма или ошибке.
                Аргументы:
                  self - объект TLetter; в его поле error
                         будет текст с описанием ошибки если result == false
     closure - то же самое что и callback, но с добавлением указателя на объект.
               Позволяет вызывать функции, являющиеся методами классов.

   Определить указатель на поток:
   TSendmailThread * SendmailThread = NULL;

   После этого нужно вызвать статическую функцию
   TSendmailThread::Send(&SendmailThread, TLetter *)
   Она создаст поток отправки писем, поставит письмо в очередь на отправку и
   вернет управление мгновенно.
   Создание потока отправки писем выполняется только при отправке первого письма.
   О результате отправки письма можно узнать с помощью callback / closure
   При завершении приложения нужно вызвать статическую функцию
   TSendmailThread::Exit(&SendmailThread)
   для завершения работы потока отправки писем.
*/

//---------------------------------------------------------------------------
class TSendmailThread : public TThread
{
private:
  TIdSMTP * smtp;             // Поддержка SMTP (библиотека indy)
  TIdSSLIOHandlerSocketOpenSSL * SSLIOHandler; // Поддержка SSL
  TUniList<TLetter> letters;  // Очередь писем на отправку
  TLetter * SyncLetter;       // Указатель для функции sync()

private:
  // Выполняет синхронную отправку письма
  // Все исключения ловит сама
  bool SendInternal(TLetter * p);
  // Вызов TLetter::callback and/or TLetter::closure в контексте основного потока
  void __fastcall sync(void);
  // Функция устанавливает кодировку писем UTF8
  // Специфична для TIdMessage::OnInitializeISO
  void __fastcall iiso(char &VHeaderEncoding, AnsiString &VCharSet);

protected:
  // Основная функция потока
  void __fastcall Execute();

public:
  __fastcall TSendmailThread(void);
  __fastcall ~TSendmailThread();
  // Создает поток (если он еще не создан), помещает указатель на него в
  // *SendmailThread и отправляет письмо TLetter
  static void __fastcall Send(TSendmailThread ** SendmailThread, TLetter * p);
  // Удаляет поток по указателю *SendmailThread
  static void __fastcall Exit(TSendmailThread ** SendmailThread);
};
//---------------------------------------------------------------------------
#endif
