//---------------------------------------------------------------------------
#ifndef udpconH
#define udpconH

//---------------------------------------------------------------------------
bool UdpServerCreate(void);
void UdpServerDestroy(void);
void UdpServerStart(void);
void UdpServerStop(void);
void UdpReceiveMessage(void);
//---------------------------------------------------------------------------
#endif
