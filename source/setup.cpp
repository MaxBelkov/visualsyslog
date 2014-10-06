//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MibAccess.h"
#include "utils.h"       // common functions
#include "cfg.h"
#include "messageform.h" // cool message box
#include "setup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern TMainCfg MainCfg;

TSetupForm * SetupForm;
//---------------------------------------------------------------------------
__fastcall TSetupForm::TSetupForm(TComponent* Owner)
    : TForm(Owner)
{
  bUdpRestart = false;
  bTcpRestart = false;
  
  MibII m;
  UINT IPArray[20];
  UINT IPArraySize = 20;
  m.Init();
  UdpInterfaceCB->Items->BeginUpdate();
  UdpInterfaceCB->Items->Add("0.0.0.0");
  if( m.GetIPAddress(IPArray, IPArraySize) )
  {
    for(UINT i=0; i<IPArraySize; i++)
    {
      // Ignore 0.0.0.0 & 127.0.0.1
      if( IPArray[i]==0 || IPArray[i]==16777343 )
        continue;
      UdpInterfaceCB->Items->Add(IPAddrToString(IPArray[i]));
    }
  }
  UdpInterfaceCB->Items->EndUpdate();

  TcpInterfaceCB->Items = UdpInterfaceCB->Items;

  EnableUdpCB->Checked = MainCfg.UdpEnable;
  UdpInterfaceCB->Text = MainCfg.UdpInterface;
  UdpPortEdit->Text = MainCfg.UdpPort;

  EnableTcpCB->Checked = MainCfg.TcpEnable;
  TcpInterfaceCB->Text = MainCfg.TcpInterface;
  TcpPortEdit->Text = MainCfg.TcpPort;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::OKButtonClick(TObject *Sender)
{
  int port;

  if( ! TryStrToInt(UdpPortEdit->Text, port) )
  {
    ReportMess2("Invalid udp port number");
    return;
  }
  if( port < 0 || port >= 65535 )
  {
    ReportMess2("Invalid udp port number");
    return;
  }
  if( UdpInterfaceCB->Text.Length() == 0 )
  {
    ReportMess2("Invalid udp listener interface");
    return;
  }
  if( port != MainCfg.UdpPort )
    bUdpRestart = true;
  if( MainCfg.UdpInterface != UdpInterfaceCB->Text )
    bUdpRestart = true;
  if( MainCfg.UdpEnable != EnableUdpCB->Checked )
    bUdpRestart = true;
  MainCfg.UdpEnable = EnableUdpCB->Checked;
  MainCfg.UdpInterface = UdpInterfaceCB->Text;
  MainCfg.UdpPort = port;

  if( ! TryStrToInt(TcpPortEdit->Text, port) )
  {
    ReportMess2("Invalid tcp port number");
    return;
  }
  if( port < 0 || port >= 65535 )
  {
    ReportMess2("Invalid tcp port number");
    return;
  }
  if( TcpInterfaceCB->Text.Length() == 0 )
  {
    ReportMess2("Invalid tcp listener interface");
    return;
  }
  if( port != MainCfg.TcpPort )
    bTcpRestart = true;
  if( MainCfg.TcpInterface != TcpInterfaceCB->Text )
    bTcpRestart = true;
  if( MainCfg.TcpEnable != EnableTcpCB->Checked )
    bTcpRestart = true;
  MainCfg.TcpEnable = EnableTcpCB->Checked;
  MainCfg.TcpInterface = TcpInterfaceCB->Text;
  MainCfg.TcpPort = port;

  ModalResult = mrOk;
}
//---------------------------------------------------------------------------

