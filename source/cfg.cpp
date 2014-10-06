//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "inif.h"
#include "cfg.h"

extern String MainCfgFile;
char szSetup[] = "SETUP";
//---------------------------------------------------------------------------
TMainCfg::TMainCfg()
{
}
//---------------------------------------------------------------------------
bool TMainCfg::Load(void)
{
  TMYIniFile in(MainCfgFile.c_str());
  if( ! in )
    return false;

  UdpEnable = in.ReadBool(szSetup, "UdpEnable", true);
  UdpInterface = in.ReadString(szSetup, "UdpInterface", "0.0.0.0");
  UdpPort = in.ReadInteger(szSetup, "UdpPort", 514);

  TcpEnable = in.ReadBool(szSetup, "TcpEnable", false);
  TcpInterface = in.ReadString(szSetup, "TcpInterface", "0.0.0.0");
  TcpPort = in.ReadInteger(szSetup, "TcpPort", 514);

  return true;
}
//---------------------------------------------------------------------------
bool TMainCfg::Save(void)
{
  TMYIniFile out(MainCfgFile.c_str());
  // Read the existing file from disk
  if( ! out )
    return false;
  // If there is a file on disk, then continue anyway

  out.WriteBool(szSetup, "UdpEnable", UdpEnable);
  out.WriteString(szSetup, "UdpInterface", UdpInterface);
  out.WriteInteger(szSetup, "UdpPort", UdpPort);

  out.WriteBool(szSetup, "TcpEnable", TcpEnable);
  out.WriteString(szSetup, "TcpInterface", TcpInterface);
  out.WriteInteger(szSetup, "TcpPort", TcpPort);

  return true;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
