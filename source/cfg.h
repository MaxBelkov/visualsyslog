//---------------------------------------------------------------------------
#ifndef cfgH
#define cfgH

//---------------------------------------------------------------------------
class TMainCfg
{
public:
  bool UdpEnable;
  String UdpInterface;
  int UdpPort;

  bool TcpEnable;
  String TcpInterface;
  int TcpPort;

public:
  TMainCfg();
  bool Load(void);
  bool Save(void);
};
//---------------------------------------------------------------------------
#endif
