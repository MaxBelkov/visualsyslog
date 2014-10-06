#include <vcl.h>
#pragma hdrstop

#include "udp.h"

//---------------------------------------------------------------------------
TUDP::TUDP(bool enableExceptions, bool _bBlockProtected)
     : handle( NULL ),
       Connected( false ),
       Error( 0 ),
       WSAError( 0 ),
       Exceptions( enableExceptions ),
       bBlockProtected( _bBlockProtected )
{
  InBytes = OutBytes = bytes = FlushSize = 0;
  memset(&destAddr, 0, sizeof(destAddr));
  destAddr.sin_family = AF_INET;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;

  WSADATA WsaData;
  WORD wVersionRequested = MAKEWORD(2, 0);
  WSAError = WSAStartup(wVersionRequested, &WsaData);
  if( WSAError )
  {
    Error = 1;
    if( Exceptions ) throw 0;
  }
}
//---------------------------------------------------------------------------
TUDP::TUDP(char * dest, WORD port, bool enableExceptions, bool _bBlockProtected)
     : handle( NULL ),
       Connected( false ),
       Error( 0 ),
       WSAError( 0 ),
       Exceptions( enableExceptions ),
       bBlockProtected( _bBlockProtected )
{
  InBytes = OutBytes = bytes = FlushSize = 0;
  memset(&destAddr, 0, sizeof(destAddr));
  destAddr.sin_family = AF_INET;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;

  WSADATA WsaData;
  WSAError = WSAStartup(0x0101, &WsaData); // MAKEWORD(2,0)
  if( WSAError )
  {
    Error = 1;
    if( Exceptions ) throw 0;
  }

  Open("", 0, dest, port);
}
//---------------------------------------------------------------------------
TUDP::~TUDP()
{
  Close();
  WSACleanup();
}
//---------------------------------------------------------------------------
bool TUDP::Open(char * local, WORD lport, char * dest, WORD dport)
{
  Error = 0;
  WSAError = 0;

  if( handle || !local || !dest )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }

  in_addr l;
  l.S_un.S_addr = inet_addr(local);
  if( l.S_un.S_addr==4294967295 ) // host name OR error in IP address
  {
    // try to resolve host name and copy to l
    HOSTENT * hostent = gethostbyname(local);
    if( hostent )
    {
      // hostent->h_name ---> Full local name
      l.S_un = ((in_addr *)hostent->h_addr_list[0])->S_un;
    }
    else
    {
      //l.S_un.S_addr = INADDR_ANY; // Локальный адрес может быть нулевым
      WSAError = WSAGetLastError();
      Error = 2;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }

  in_addr d;
  d.S_un.S_addr = inet_addr(dest);
  if( d.S_un.S_addr==4294967295 ) // host name OR error in IP address
  {
    // try to resolve host name and copy to d
    HOSTENT * hostent = gethostbyname(dest);
    if( hostent )
    {
      // hostent->h_name ---> Full dest name
      d.S_un = ((in_addr *)hostent->h_addr_list[0])->S_un;
    }
    else
    {
      //d.S_un.S_addr = INADDR_ANY; // Удаленный адрес может быть нулевым
      WSAError = WSAGetLastError();
      Error = 3;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }

  SOCKET h;

  // open a TCP socket
  h = socket(AF_INET, SOCK_DGRAM, 0);
  if( h==INVALID_SOCKET )
  {
    WSAError = WSAGetLastError();
    Error = 4;
    if( Exceptions ) throw 0;
    return !Error;
  }

  // Bind our server to the agreed upon port number. See commdef.h for the actual port number
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(lport);
  localAddr.sin_addr = l;

  if( bind(h, (struct sockaddr *)&localAddr, sizeof(localAddr)) )
  {
    WSAError = WSAGetLastError();
    Error = 6;
    closesocket(h);
    if( Exceptions ) throw 0;
    return !Error;
  }

  memset(&destAddr, 0, sizeof(destAddr));
  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(dport);
  destAddr.sin_addr = d;

  if( dport > 0 )
  {
    if( connect(h, (sockaddr *)&destAddr, sizeof(destAddr)) )
    {
      WSAError = WSAGetLastError();
      Error = 7;
      closesocket(h);
      if( Exceptions ) throw 0;
      return !Error;
    }
  }
  // Получим локальный адрес и порт, к которому мы прибиндились
  int addr_len = sizeof(localAddr);
  if( getsockname(h, (struct sockaddr *)&localAddr, &addr_len) )
  {
    WSAError = WSAGetLastError();
    Error = 8;
    closesocket(h);
    if( Exceptions ) throw 0;
    return !Error;
  }

  handle = h;
  Connected = true;

  return ! Error;
}
//---------------------------------------------------------------------------
bool TUDP::SimpleOpen(char * local, WORD lport, char * dest, WORD dport)
{
  Error = 0;
  WSAError = 0;

  if( handle || !local || !dest )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }

  in_addr l;
  l.S_un.S_addr = inet_addr(local);
  if( l.S_un.S_addr==4294967295 ) // host name OR error in IP address
  {
    // try to resolve host name and copy to l
    HOSTENT * hostent = gethostbyname(local);
    if( hostent )
    {
      // hostent->h_name ---> Full local name
      l.S_un = ((in_addr *)hostent->h_addr_list[0])->S_un;
    }
    else
    {
      //l.S_un.S_addr = INADDR_ANY; // Локальный адрес может быть нулевым
      WSAError = WSAGetLastError();
      Error = 2;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }
  in_addr d;
  d.S_un.S_addr = inet_addr(dest);
  if( d.S_un.S_addr==4294967295 ) // host name OR error in IP address
  {
    // try to resolve host name and copy to d
    HOSTENT * hostent = gethostbyname(dest);
    if( hostent )
    {
      // hostent->h_name ---> Full dest name
      d.S_un = ((in_addr *)hostent->h_addr_list[0])->S_un;
    }
    else
    {
      //d.S_un.S_addr = INADDR_ANY; // Удаленный адрес может быть нулевым
      WSAError = WSAGetLastError();
      Error = 3;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }

  SOCKET h;

  // open a TCP socket
  h = socket(AF_INET, SOCK_DGRAM, 0);
  if( h==INVALID_SOCKET )
  {
    WSAError = WSAGetLastError();
    Error = 4;
    if( Exceptions ) throw 0;
    return !Error;
  }

  // Bind our server to the agreed upon port number. See commdef.h for the actual port number
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(lport);
  localAddr.sin_addr = l;

  memset(&destAddr, 0, sizeof(destAddr));
  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(dport);
  destAddr.sin_addr = d;

  handle = h;
  Connected = true;

  return ! Error;
}

//---------------------------------------------------------------------------
bool TUDP::Close(void)
{
  WSAError = 0;
  Error = 0;

  if( !handle || !Connected ) return true;
  if( closesocket(handle) )
  {
    WSAError = WSAGetLastError();
    Error = 1;
  }
  handle = NULL;
  Connected = false;
  return ! Error;
}
//---------------------------------------------------------------------------
// Разрешить широковещательные пакеты
bool __fastcall TUDP::EnableBroadcast(BOOL enable)
{
  WSAError = 0;
  Error = 0;

  if( !handle )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return false;
  }
  if( setsockopt(handle, SOL_SOCKET, SO_BROADCAST, (char *)&enable, sizeof(BOOL))==SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 2;
    if( Exceptions ) throw 0;
    return false;
  }
  return true;
}
//---------------------------------------------------------------------------
bool TUDP::Write(void * data, int size)
{
  WSAError = 0;
  Error = 0;

  if( !handle )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }

  int rv = send(handle, (char *)data, size, 0);
  if( rv == SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 2;
    if( Exceptions ) throw 0;
    return !Error;
  }
  if( rv != size )
  {
    WSAError = WSAGetLastError();
    Error = 3;
    if( Exceptions ) throw 0;
    return !Error;
  }
  OutBytes += size;
  return ! Error;
}
//---------------------------------------------------------------------------
bool TUDP::Read(void * data, int size, bool bPeek)
{
  WSAError = 0;
  Error = 0;
  bytes = 0;

  if( !handle || size<=0 )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }
  // Сделаем проверку входного буфера
  if( bBlockProtected )
  {
    DWORD bufsize;
    if( ioctlsocket(handle, FIONREAD, &bufsize) == SOCKET_ERROR )
    {
      WSAError = WSAGetLastError();
      Error = 2;
      if( Exceptions ) throw 0;
      return !Error;
    }
    if( bufsize < (DWORD)size )
    {
      Error = 3;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }

  bytes = recv(handle, (char *)data, size, bPeek ? MSG_PEEK : 0);
  if( bytes == SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 4;
    bytes = 0;
    if( Exceptions ) throw 0;
    return !Error;
  }
  if( bytes != size )
  {
    WSAError = WSAGetLastError();
    Error = 5;
    if( Exceptions ) throw 0;
    return !Error;
  }
  InBytes += bytes;
  return ! Error;
}
//---------------------------------------------------------------------------
bool TUDP::WriteTo(void * data, int size, struct sockaddr * p)
{
  WSAError = 0;
  Error = 0;

  if( !handle )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }

  int rv = sendto(handle, (char *)data, size, 0, p, sizeof(struct sockaddr));
  if( rv == SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 2;
    if( Exceptions ) throw 0;
    return !Error;
  }
  if( rv != size )
  {
    WSAError = WSAGetLastError();
    Error = 3;
    if( Exceptions ) throw 0;
    return !Error;
  }
  OutBytes += size;
  return ! Error;
}
//---------------------------------------------------------------------------
bool TUDP::ReadFrom(void * data, int size, struct sockaddr * p, bool bPeek)
{
  WSAError = 0;
  Error = 0;
  bytes = 0;

  if( !handle || size<=0 )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }
  // Сделаем проверку входного буфера
  if( bBlockProtected )
  {
    DWORD bufsize;
    if( ioctlsocket(handle, FIONREAD, &bufsize) == SOCKET_ERROR )
    {
      WSAError = WSAGetLastError();
      Error = 2;
      if( Exceptions ) throw 0;
      return !Error;
    }
    if( bufsize < (DWORD)size )
    {
      WSAError = MYERROR_NOREQSIZE;
      Error = 3;
      if( Exceptions ) throw 0;
      return !Error;
    }
  }

  int fromlen = sizeof(struct sockaddr);
  bytes = recvfrom(handle, (char *)data, size, bPeek ? MSG_PEEK : 0, p, &fromlen);
  if( bytes==SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 4;
    bytes = 0;
    if( Exceptions ) throw 0;
    return !Error;
  }
  if( bytes != size )
  {
    // Обычно происходит, когда в приемном буфере несколько UPD пакетов
    // Получать UDP пакеты можно только по одному
    // Поэтому после получения одного пакета bytes < size
    // ReadLength() дает суммарный размер ВСЕХ пакетов в приемном буфере
    WSAError = MYERROR_READCOUNT;
    Error = 5;
    if( Exceptions ) throw 0;
    return !Error;
  }
  InBytes += bytes;
  return ! Error;
}
//---------------------------------------------------------------------------
// Cуммарный размер ВСЕХ пакетов в приемном буфере
DWORD TUDP::ReadLength(void)
{
  WSAError = 0;
  Error = 0;

  if( !handle )
  {
    WSAError = MYERROR_UNKNOWNPARAMS;
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }

  DWORD bufsize;
  if( ioctlsocket(handle, FIONREAD, &bufsize) == SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 1;
    bufsize = 0;
    if( Exceptions ) throw 0;
    return !Error;
  }
  return bufsize;
}
//---------------------------------------------------------------------------
bool TUDP::Poll(bool bread, bool bwrite, bool berror, long msec)
{
  WSAError = 0;
  fd_set f;
  FD_ZERO(&f);
  FD_SET(handle, &f);
  timeval t;
  t.tv_sec = 0;
  t.tv_usec = msec * 1000; // "POLL" SOCKET STATE msec period and return immediately...
  int rv = select(FD_SETSIZE, bread ? &f : NULL, bwrite ? &f : NULL, berror ? &f : NULL, &t);
  if( rv == SOCKET_ERROR )
  {
    WSAError = WSAGetLastError();
    Error = 1;
    bytes = 0;
    if( Exceptions ) throw 0;
    return !Error;
  }
  return rv > 0;
}
//---------------------------------------------------------------------------
// Очистить приемный буфер
// Возвращает - сколько пакетов укучено
int TUDP::Flush(void)
{
  DWORD l;
  int count = 0;
  FlushSize = 0;
  while( (l = ReadLength()) > 0 )
  {
    BYTE * p = new BYTE[l];
    Read(p, l);
    delete [] p;
    count++;
    FlushSize += l;
  }
  return count;
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetLocalAddr(void)
{
  return inet_ntoa(localAddr.sin_addr);
}
//---------------------------------------------------------------------------
WORD TUDP::GetLocalPort(void)
{
  return htons(localAddr.sin_port);
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetLocalAddrPort(void)
{
  AnsiString rv(inet_ntoa(localAddr.sin_addr));
  rv += ":" + IntToStr(htons(localAddr.sin_port));
  return rv;
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetRemoteAddr(void)
{
  return inet_ntoa(destAddr.sin_addr);
}
//---------------------------------------------------------------------------
WORD TUDP::GetRemotePort(void)
{
  return htons(destAddr.sin_port);
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetRemoteAddrPort(void)
{
  AnsiString rv(inet_ntoa(destAddr.sin_addr));
  rv += ":" + IntToStr(htons(destAddr.sin_port));
  return rv;
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetErrorMessage(void)
{
  switch( WSAError )
  {
    case 0: return "";
    case WSAEADDRINUSE: return "Адрес или порт уже используется в системе";
    case WSAENETDOWN: return "Сеть умерла";
    case WSAENETUNREACH: return "Сеть недоступна";
    case WSAECONNABORTED: return "Соединение закрыто из-за ошибки";
    case WSAECONNRESET: return "Соединение закрыто удаленной стороной";
    case WSAENOBUFS: return "Сетевой буфер переполнен";
    case WSAESHUTDOWN: return "Обмен невозможен - соединение закрывается";
    case WSAETIMEDOUT: return "Таймаут подключения по TCP/IP";
    case WSAECONNREFUSED: return "Удаленный узел отклонил соединение";
    case WSAEHOSTDOWN: return "Удаленный узел не существует";
    case WSAHOST_NOT_FOUND: return "Имя узла не найдено";
    case WSAENOTSOCK: return "Неверный идентификатор сокета";
    case WSAEWOULDBLOCK: return "Неблокирующая операция выполняется";
    case WSANO_DATA: return "Не удается получить IP адрес по имени узла";
    // WSAEHOSTUNREACH возникает когда Ethernet кабель выключен и WiFi нет
    case WSAEHOSTUNREACH: return "Сеть недоступна"; // "Узел сети недоступен";
    case WSAEMSGSIZE: return "Сообщение слишком большое";
    // local error codes
    case MYERROR_UNKNOWNPARAMS: return "Неверные параметры";
    case MYERROR_READCOUNT: return "В приемном буфере еще остались UDP пакеты";
    case MYERROR_NOREQSIZE: return "В приемном буфере данных меньше чем запрошено";
  }
  return AnsiString("Код ошибки: ") + IntToStr(WSAError);
}
//---------------------------------------------------------------------------
AnsiString TUDP::GetErrorMessageEng(void)
{
  switch( WSAError )
  {
    case 0: return "";
    case WSAEADDRINUSE: return "Address or port is already in use";
    case WSAENETDOWN: return "Network died";
    case WSAENETUNREACH: return "Network is not available";
    case WSAECONNABORTED: return "Connection closed because of an error";
    case WSAECONNRESET: return "Connection closed by remote side";
    case WSAENOBUFS: return "Network buffer overflow";
    case WSAESHUTDOWN: return "The exchange is not possible - the connection is closed";
    case WSAETIMEDOUT: return "Timeout connecting over TCP/IP";
    case WSAECONNREFUSED: return "The remote host refused the connection";
    case WSAEHOSTDOWN: return "The remote host does not exist";
    case WSAHOST_NOT_FOUND: return "Host Name not found";
    case WSAENOTSOCK: return "Invalid socket identification";
    case WSAEWOULDBLOCK: return "Non-blocking operation is in progress";
    case WSANO_DATA: return "Can not get IP address for the host name";
    // WSAEHOSTUNREACH occurs when the Ethernet cable is off and there is no WiFi
    case WSAEHOSTUNREACH: return "Network not available"; // "Host not available";
    case WSAEMSGSIZE: return "Message too large";
    // Local error codes
    case MYERROR_UNKNOWNPARAMS: return "Invalid parameters";
    case MYERROR_READCOUNT: return "to the receive buffer still have UDP packets";
    case MYERROR_NOREQSIZE: return "to the receive buffer data is less than requested";
  }
  return AnsiString ("Error code: ") + IntToStr(WSAError);
}
//---------------------------------------------------------------------------

