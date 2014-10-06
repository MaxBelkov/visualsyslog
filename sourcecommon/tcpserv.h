#ifndef MyServerTcpH
#define MyServerTcpH

#include <winsock.h>

//---------------------------------------------------------------------------
class MSocket
{
public:
  SOCKET handle;    //
  bool Connected;   // Подключен ?
  int  Error;       // Последняя операция завершилась с ошибкой ?
  bool Exceptions;  // Генерить иключение при ошибках ?
  bool bNoDestroy;

public:
  struct sockaddr_in destAddr, localAddr;
  int InBytes, OutBytes; // Счетчики байт
  int bytes;   // Байт считано или записано в последней операции
  int errcode; // ==WSAGetLastError() последней операции

public:
  MSocket(SOCKET h=NULL, bool enableExceptions = false);
  ~MSocket();

  void Initialize(MSocket * p);//SOCKET h, struct sockaddr_in * DestinationAddress);

public:
  // Закрыть соединение
  bool Close(void);
  // Проверить открытость соединения
  bool IsOpen(void) { return Connected; }
  //
  // Эти функции пользовать только в клиентском режиме
  // (т.е. Вы уже открыли соединение с уделенной стороной и
  // знаете на кокой порт и адрес работаете)
  bool Write(void * data, int size);
  bool Read(void * data, int size);
  // Эти функции пользовать когда угодно
  //bool WriteTo(void * data, int size, struct sockaddr * p);
  //bool ReadFrom(void * data, int size, struct sockaddr * p);
  // Какой объем данных нам пришел и можно его получить ?
  DWORD ReadLength(void);
  // Проверить состояние: есть данные для считывания,
  // есть возможность записи данных, есть ошибки, время ожидания
  bool Poll(bool bread, bool bwrite, bool berror, long msec=0);

  // Получить последнее состояние файла
  int    GetError(void)  { return Error; }
  SOCKET GetHandle(void) { return handle; }

public:
  bool operator ! () { return GetError()!=0; }

public:
  MSocket & operator << (char * string)
  {
    if( string )
    {
      int l = lstrlen(string);
      if( l > 0 )
        Write(string, l+1);
    }
    return *this;
  }
  MSocket & operator << (char c) { Write((BYTE *)&c, sizeof(c)); return *this; }
  MSocket & operator << (int i)  { Write((BYTE *)&i, sizeof(i)); return *this; }
  MSocket & operator >> (char & c) { Read((BYTE *)&c, sizeof(c)); return *this; }

public:
  // получить локальный адрес к которому мы привязаны
  AnsiString GetLocalAddr(void);
  WORD GetLocalPort(void);
  AnsiString GetLocalAddrPort(void);

  // получить удаленный адрес, с которым мы соединены
  AnsiString GetRemoteAddr(void);
  WORD GetRemotePort(void);
  AnsiString GetRemoteAddrPort(void);

  // получить сообщение об ошибке
  AnsiString GetErrorMessage(void);
  // получить сообщение об ошибке на инглише
  AnsiString GetErrorMessageEng(void);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class MTCPServer : public MSocket
{
public:
  MTCPServer(bool enableExceptions = false);
  ~MTCPServer();

public:
  // Ждать соединений TCP
  bool Open(char * local="", WORD lport=0);

  MSocket * GetClient(long timetowait=100); // in msec
  //  Закрыть соединение
  bool Close(void);
  // Проверить открытость соединения
  bool IsOpen(void) { return Connected; }
};
//---------------------------------------------------------------------------
#endif

