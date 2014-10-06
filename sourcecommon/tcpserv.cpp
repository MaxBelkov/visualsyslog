#include <vcl.h>
#pragma hdrstop

#include "tcpserv.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
MSocket::MSocket(SOCKET h, bool enableExceptions)
{
  memset(this, 0, sizeof(MSocket));
  handle = h;
  Connected = h>0;   // Подключен ?
  Exceptions = enableExceptions;
  bNoDestroy = false;
}
//---------------------------------------------------------------------------
MSocket::~MSocket()
{
  if( !bNoDestroy ) Close();
}
//---------------------------------------------------------------------------
void MSocket::Initialize(MSocket * p)//SOCKET h, struct sockaddr_in * DestinationAddress)
{
  handle = p->handle;
  Connected = p->Connected;
  Error = p->Error;
  Exceptions = p->Exceptions;
  destAddr = p->destAddr;
  localAddr = p->localAddr;
  InBytes = p->InBytes;
  OutBytes = p->OutBytes;
  bytes = p->bytes;
  errcode = p->errcode;
}
//---------------------------------------------------------------------------
bool MSocket::Close(void)
{
  if( !handle || !Connected ) return true;
/*
  * WinSock 2 extension -- manifest constants for shutdown()
  #define SD_RECEIVE      0x00
  #define SD_SEND         0x01
  #define SD_BOTH         0x02
*/
  shutdown(handle, 0x02); // SD_SEND
  Error = closesocket(handle);
  handle = NULL;
  Connected = false;
  return ! Error;
}
//---------------------------------------------------------------------------
bool MSocket::Write(void * data, int size)
{
  Error = 0;
  if( !handle ) { Error = 1; if( Exceptions ) throw 0; return ! Error; }

  bytes = send(handle, (char *)data, size, 0);
  errcode = 0;
  if( bytes==SOCKET_ERROR ) { errcode = WSAGetLastError(); Error = 4; }
  if( bytes<=0 ) Error = 2;
  if( bytes<size ) Error = 3;
  if( Error )
  {
    if( Exceptions ) throw 0;
  }
  else
  {
    OutBytes += size;
  }
  return ! Error;
}
//---------------------------------------------------------------------------
bool MSocket::Read(void * data, int size)
{
  Error = 0;
  if( !handle ) { Error = 1; if( Exceptions ) throw 0; return ! Error; }

  bytes = recv(handle, (char *)data, size, 0);
  errcode = 0;
  if( bytes==SOCKET_ERROR ) { errcode = WSAGetLastError(); Error = 4; }
  if( bytes<=0 ) Error = 2;
  if( bytes<size ) Error = 3;
  if( Error )
  {
    if( Exceptions ) throw 0;
  }
  else
  {
    InBytes += bytes;
  }
  return ! Error;
}
//---------------------------------------------------------------------------
DWORD MSocket::ReadLength(void)
{
  if( !handle ) { Error = true; if( Exceptions ) throw 0; return !Error; }
  //
  DWORD bytes;
  Error = ioctlsocket(handle, FIONREAD, &bytes);
  errcode = 0;
  if( Error==SOCKET_ERROR ) errcode = WSAGetLastError();
  //
  if( Error ) { bytes = 0L; if( Exceptions ) throw 0; }
  return bytes;
}
//---------------------------------------------------------------------------
bool MSocket::Poll(bool bread, bool bwrite, bool berror, long msec)
{
  Error = 0;
  errcode = 0;
  if( !handle )
  {
    Error = 1;
    if( Exceptions ) throw 0;
    return !Error;
  }
  fd_set f;
  FD_ZERO(&f);
  FD_SET(handle, &f);
  timeval t;
  t.tv_sec = 0;
  t.tv_usec = msec * 1000; // 0 -> "POLL" SOCKET STATE and return immediately...
  int socknumber = select(FD_SETSIZE, bread ? &f : NULL, bwrite ? &f : NULL, berror ? &f : NULL, &t);
  if( socknumber==SOCKET_ERROR )
    errcode = WSAGetLastError();
  return socknumber > 0;
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetLocalAddr(void)
{
  return inet_ntoa(localAddr.sin_addr);
}
//---------------------------------------------------------------------------
WORD MSocket::GetLocalPort(void)
{
  return htons(localAddr.sin_port);
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetLocalAddrPort(void)
{
  AnsiString rv(inet_ntoa(localAddr.sin_addr));
  rv += ":" + IntToStr(htons(localAddr.sin_port));
  return rv;
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetRemoteAddr(void)
{
  return inet_ntoa(destAddr.sin_addr);
}
//---------------------------------------------------------------------------
WORD MSocket::GetRemotePort(void)
{
  return htons(destAddr.sin_port);
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetRemoteAddrPort(void)
{
  AnsiString rv(inet_ntoa(destAddr.sin_addr));
  rv += ":" + IntToStr(htons(destAddr.sin_port));
  return rv;
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetErrorMessage(void)
{
  switch( errcode )
  {
    case 0: return "";
    case WSAEADDRINUSE: return "Адрес или порт уже используется в системе";
    case WSAENETDOWN: return "Сеть умерла";
    case WSAENETUNREACH: return "Сеть недоступна";
    case WSAECONNABORTED: return "Соединение закрыто из-за ошибки";
    case WSAECONNRESET: return "Соединение закрыто удаленной стороной";
    case WSAENOBUFS: return "Сетевой буфер переполнен";
    case WSAESHUTDOWN: return "Обмен невозможен - соединение закрывается";
    case WSAETIMEDOUT: return "Таймаут подключения";
    case WSAECONNREFUSED: return "Удаленный узел отклонил соединение";
    case WSAEHOSTDOWN: return "Удаленный узел не существует";
    case WSAHOST_NOT_FOUND: return "Имя узла не найдено";
    case WSAENOTSOCK: return "Неверный идентификатор сокета";
    case WSAEWOULDBLOCK: return "Неблокирующая операция выполняется";
    case WSANO_DATA: return "Не удается получить IP адрес по имени узла";
  }
  return AnsiString("Код ошибки: ") + IntToStr(errcode);
}
//---------------------------------------------------------------------------
AnsiString MSocket::GetErrorMessageEng(void)
{
  switch( errcode )
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
    /* Local error codes
    case MYERROR_UNKNOWNPARAMS: return "Invalid parameters";
    case MYERROR_READCOUNT: return "to the receive buffer still have UDP packets";
    case MYERROR_NOREQSIZE: return "to the receive buffer data is less than requested";
    */
  }
  return AnsiString ("Error code: ") + IntToStr(errcode);
}
//---------------------------------------------------------------------------
/*
bool MSocket::WriteTo(void * data, int size, struct sockaddr * p)
{
  Error = 0;
  if( !handle ) { Error = 1; if( Exceptions ) throw 0; return !Error; }
  //
  int t = sendto(handle, (char *)data, size, 0, p, sizeof(struct sockaddr));
  if( t!=size ) Error = 2;
  if( Error ) { if( Exceptions ) throw 0; return !Error; }
  OutBytes += size;
  return ! Error;
}
//---------------------------------------------------------------------------
bool MSocket::ReadFrom(void * data, int size, struct sockaddr * p)
{
  Error = 0;
  if( !handle ) { Error = 1; if( Exceptions ) throw 0; return !Error; }
  //
  int fromlen = sizeof(struct sockaddr);
  bytes = recvfrom(handle, (char *)data, size, 0, p, &fromlen);
  if( bytes<=0 ) Error = 2;
  if( Error ) { if( Exceptions ) throw 0; return !Error; }
  InBytes += bytes;
  return ! Error;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//WORD MTCPServer::localport = 10000;
//---------------------------------------------------------------------------
MTCPServer::MTCPServer(bool enableExceptions)
           : MSocket(NULL, enableExceptions)
{
  WSADATA WsaData;
  Error = WSAStartup(MAKEWORD(2,0), &WsaData);
  errcode = WSAGetLastError();
  if( Exceptions && Error ) throw 0;
}
/*---------------------------------------------------------------------------
MTCPServer::MTCPServer(char * dest, WORD port, bool enableExceptions)
        : Exceptions( enableExceptions ) // handle(NULL), Connected(false), Error(0), 
{
  memset(this, 0, sizeof(MTCPServer));
  WSADATA WsaData;
  Error = WSAStartup(0x0101, &WsaData); // MAKEWORD(2,0)
  if( Exceptions && Error ) throw 0;

  Open("", 0, dest, port);
}*/
//---------------------------------------------------------------------------
MTCPServer::~MTCPServer()
{
  Close();
  WSACleanup();
}
//---------------------------------------------------------------------------
bool MTCPServer::Open(char * local, WORD lport)
{
  Error = 0;
  if( handle || !local ) { Error = 1; if( Exceptions ) throw 0; return !Error; }
  // По умолчанию будем нумеровать локальные порты с 10000
  //if( !lport ) lport = localport++;

  errcode = 0;
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
      errcode = WSAGetLastError();
      Error = 2;
      if( Exceptions ) throw 0; return !Error;
    }
  }
  SOCKET h;
  //
  // Open a TCP socket
  //    h = socket(AF_INET, SOCK_DGRAM, 0);
  h = socket(AF_INET, SOCK_STREAM, 0);
  if( h==INVALID_SOCKET)
  {
    errcode = WSAGetLastError();
    Error = 4;
  }
  if( Error ) { if( Exceptions ) throw 0; return !Error; }
  //
  // Bind our server to the agreed upon port number. See commdef.h for the actual port number
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(lport);
  localAddr.sin_addr = l;
  /*
  WriteToLogMT("S\tOpen(\"%s\" %u.%u.%u.%u:%u",
    local,
    l.S_un.S_un_b.s_b1,
    l.S_un.S_un_b.s_b2,
    l.S_un.S_un_b.s_b3,
    l.S_un.S_un_b.s_b4,
    lport);
  */
  if( bind(h, (struct sockaddr *)&localAddr, sizeof(localAddr)) )
  {
    errcode = WSAGetLastError();
    Error = 5;
  }
  if( Error ) { closesocket(h); if( Exceptions ) throw 0; return !Error; }
  //WriteToLogMT("S\tBind OK");

  if( listen(h, SOMAXCONN) )
  {
    errcode = WSAGetLastError();
    Error = 7;
  }
  if( Error ) { closesocket(h); if( Exceptions ) throw 0; return !Error; }
  //WriteToLogMT("S\tListen OK");
  //
  handle = h;
  Connected = true;
  //
  return ! Error;
}
//---------------------------------------------------------------------------
MSocket * MTCPServer::GetClient(long timetowait)
{
  MSocket * p = NULL;
  errcode = 0;
  if( Poll(true, false, false, timetowait) )
  {
    struct sockaddr_in dest;
    int addrlen = sizeof(dest);
    SOCKET s = accept(handle, (sockaddr *)&dest, &addrlen);
    errcode = 0;
    if( s==INVALID_SOCKET )
    {
      errcode = WSAGetLastError();
    }
    else
    {
      p = new MSocket(s);
      p->destAddr = dest;
    }
  }
  return p;
}
//---------------------------------------------------------------------------
bool MTCPServer::Close(void)
{
  if( !handle || !Connected ) return true;
  Error = closesocket(handle);
  handle = NULL;
  Connected = false;
  return ! Error;
}
//---------------------------------------------------------------------------
