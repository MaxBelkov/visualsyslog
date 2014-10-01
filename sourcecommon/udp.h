#ifndef SaturnUDPSocketH
#define SaturnUDPSocketH
#include <winsock.h>

#define MYERROR_UNKNOWNPARAMS   50000
#define MYERROR_READCOUNT       50001
#define MYERROR_NOREQSIZE       50002

//---------------------------------------------------------------------------
class TUDP
{
private:
  SOCKET handle;        // Идентификатор сокета
  bool Connected;       // Подключен ?
  bool Exceptions;      // Генерить иключение при ошибках ?
  bool bBlockProtected; // Если = true, то все вызовы к блокирующим функциям чтения
                        // предворяются контролем количества данных во входящем буфере

public:
  struct sockaddr_in destAddr, localAddr;
  int  InBytes;   // Всего байт принято
  int  OutBytes;  // Всего байт послано
  int  bytes;     // Байт прочитано в последней операции чтения из сокета
  int  Error;     // if Error != 0 значит последняя операция завершилась с ошибкой
  int  WSAError;  // Код ошибки сокетов WSAGetLastError()
  int  FlushSize; // Общий размер пакетов, убитых функцией Flush()

public:
  TUDP(bool enableExceptions = false, bool _bBlockProtected = false);
  // Данный конструктор инициализирует соединение в режиме клиента
  TUDP(char * dest, WORD port, bool enableExceptions = false,
       bool _bBlockProtected = false);
  ~TUDP();

public:
  // Открыть соединение UDP.
  // Если local="" && lport=0 - они будут выбраны автоматически.
  // ПРИМЕР: if( !udp.Open("", 0, "www.microsoft.com", 80) ) return false;
  // Сокет будет привязан к адресу local:lport
  // Можно использовать функции Write, Read, WriteTo, ReadFrom
  bool Open(char * local, WORD lport, char * dest, WORD dport);
  // То-же что и Open, но привязка к адресу не осуществляется
  // Можно использовать функции только WriteTo, ReadFrom
  bool SimpleOpen(char * local, WORD lport, char * dest, WORD dport);

  bool BroadcastOpen(WORD port);
  
  //  Закрыть соединение
  bool Close(void);
  // Проверить открытость соединения
  bool IsOpen(void) { return Connected; }
  // Разрешить широковещательные пакеты
  bool __fastcall EnableBroadcast(BOOL enable);

  // Эти функции пользовать только в клиентском режиме
  // (т.е. Вы уже открыли соединение с удаленной стороной и
  // знаете на какой порт и адрес работаете)
  bool Write(void * data, int size);
  virtual bool Read(void * data, int size, bool bPeek=false);

  // Эти функции пользовать когда угодно
  bool WriteTo(void * data, int size, struct sockaddr * p);
  // Получение ОДНОГО пакета из приемного буфера
  bool ReadFrom(void * data, int size, struct sockaddr * p, bool bPeek=false);
  // Cуммарный размер ВСЕХ пакетов в приемном буфере
  virtual DWORD ReadLength(void);
  // Проверить состояние: есть данные для считывания,
  // есть возможность записи данных, есть ошитбки, время ожидания
  bool Poll(bool bread, bool bwrite, bool berror, long msec=0);
  // Очистить приемный буфер
  // Возвращает - сколько пакетов укучено
  virtual int Flush(void);

  // получить локальный адрес к которому мы привязаны
  AnsiString GetLocalAddr(void);
  WORD GetLocalPort(void);
  AnsiString GetLocalAddrPort(void);

  // получить удаленный адрес, с которым мы соединены
  AnsiString GetRemoteAddr(void);
  WORD GetRemotePort(void);
  AnsiString GetRemoteAddrPort(void);

  // Получить последнее состояние файла
  int    GetError(void)    { return Error; }
  int    GetWSAError(void) { return WSAError; }
  SOCKET GetHandle(void)   { return handle; }

  // получить сообщение об ошибке
  AnsiString GetErrorMessage(void);
  // получить сообщение об ошибке на инглише
  AnsiString GetErrorMessageEng(void);

public:
  bool operator ! () { return GetError()!=0; }
  __property BOOL Broadcast = {write=EnableBroadcast};
};
//---------------------------------------------------------------------------
#endif
