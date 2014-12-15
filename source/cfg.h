//---------------------------------------------------------------------------
#ifndef cfgH
#define cfgH

#include "letter.h"
#include "fdb.h"
//---------------------------------------------------------------------------
class TMainCfg
{
public:
  // main
  bool UdpEnable;
  String UdpInterface;
  int UdpPort;
  bool TcpEnable;
  String TcpInterface;
  int TcpPort;

  // mail
  TLetter Letter;

public:
  TMainCfg();
  void Save(String file, TStorageFileList * sfl);
  void Load(String file, TStorageFileList * sfl);
};
//---------------------------------------------------------------------------
#endif
