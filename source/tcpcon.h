//---------------------------------------------------------------------------
#ifndef tcpconH
#define tcpconH
#include "tcpserv.h"

//---------------------------------------------------------------------------
class TSyslogdTcpConn
{
public:
  MSocket * Socket;
  BYTE * Data;
  int Size;     // Total buffer size
  int DataSize; // Data size in buffer

public:
  TSyslogdTcpConn(MSocket * p);
  ~TSyslogdTcpConn();
  BYTE * GetBufferSize(int NeedFreeSize);
};
//---------------------------------------------------------------------------
extern MTCPServer * tcp;
extern TList * tcp_cons;

// server control
bool TcpServerCreate(void);
void TcpServerDestroy(void);
void TcpServerStart(void);
void TcpServerStop(void);
void TcpAccept(void);
void TcpPrintStatus(void);

// connection control
void TcpAddConnection(MSocket * p);
void TcpDeleteConnection(int index);
void TcpCloseAllConnections(void);
int TcpGetConnectionsCount(void);

void TcpReceiveMessage(void);
//---------------------------------------------------------------------------
#endif
