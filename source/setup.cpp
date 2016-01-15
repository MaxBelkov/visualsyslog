//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "sendmail.h"
#include "MibAccess.h"
#include "utils.h"       // common functions
#include "cfg.h"
#include "messageform.h" // cool message box
#include "server.h"
#include "saveini.h"     // TSaveParamsINI
#include "setup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "fileframe"
#pragma resource "*.dfm"

extern TStorageFileList * fdb;
extern TMainCfg MainCfg;
extern TSaveParamsINI * AppParams;
extern TSendmailThread * SendmailThread;

int TSetupForm::LastTabIndex = 0;
int TSetupForm::LastFileIndex = 0;
TSetupForm * SetupForm = NULL;
//---------------------------------------------------------------------------
__fastcall TSetupForm::TSetupForm(TComponent* Owner)
    : TForm(Owner)
{
  // main
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

  D3CB->Checked = MainCfg.b3D;
  WriteRawCB->Checked = MainCfg.bWriteRaw;
  ReceiveUTF8CB->Checked = MainCfg.bReceiveUTF8;

  // files
  localSFL = new TStorageFileList;
  *localSFL = fdb; 
  FillRuleList(LastFileIndex);
  FileFr->OnValuesChange = OnFrameValuesChange;

  // mail
  ToInterface(&MainCfg.Letter);

  PageControl->TabIndex = LastTabIndex;

  *AppParams >> (TStringGrid *)DrawGrid;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::FormDestroy(TObject *Sender)
{
  SetupForm = NULL;
  
  LastTabIndex = PageControl->TabIndex;
  if( DrawGrid->Row > 0 )
    LastFileIndex = DrawGrid->Row - 1;
  delete localSFL;
  localSFL = NULL;
  *AppParams << (TStringGrid *)DrawGrid;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::FillRuleList(int SelectedIndex)
{
  DrawGrid->RowCount = 1 + localSFL->Count;
  if( DrawGrid->RowCount > 1 )
  {
    DrawGrid->FixedRows = 1;
    if( DrawGrid->RowCount > SelectedIndex + 1 )
      DrawGrid->Row = SelectedIndex + 1;
    else if( DrawGrid->RowCount >= 2 )
      DrawGrid->Row = 1;
  }
  DrawGrid->OnClick(this);
  DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::OKButtonClick(TObject *Sender)
{
  // main
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

  MainCfg.b3D = D3CB->Checked;
  MainCfg.bWriteRaw = WriteRawCB->Checked;
  MainCfg.bReceiveUTF8 = ReceiveUTF8CB->Checked;

  // files
  *fdb = localSFL;

  // mail
  if( ! FromInterface(&MainCfg.Letter) )
    return;

  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
bool TSetupForm::FromInterface(TLetter * p)
{
  p->server = serverEdit->Text;

  int port;
  if( ! TryStrToInt(portEdit->Text, port) )
  {
    ReportMess2("Invalid smtp server port number");
    return false;
  }
  if( port < 0 || port >= 65535 )
  {
    ReportMess2("Invalid smtp server port number");
    return false;
  }

  p->port = port;
  p->username = usernameEdit->Text;
  p->password = passwordEdit->Text;
  p->ssl = (TSmtpSSLType)sslComboBox->ItemIndex;

  p->sender = senderEdit->Text;
  p->sender_name = sender_nameEdit->Text;
  p->recipient = recipientEdit->Text;
  p->subject = subjectEdit->Text;
  p->message = messageMemo->Text;
  return true;
}
//---------------------------------------------------------------------------
void TSetupForm::ToInterface(TLetter * p)
{
  serverEdit->Text = p->server;
  portEdit->Text = p->port;
  usernameEdit->Text = p->username;
  passwordEdit->Text = p->password;
  sslComboBox->ItemIndex = p->ssl;

  senderEdit->Text = p->sender;
  sender_nameEdit->Text = p->sender_name;
  recipientEdit->Text = p->recipient;
  subjectEdit->Text = p->subject;
  messageMemo->Text = p->message;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::SmtpSelClick(TObject *Sender)
{
  TLetter l;
  l.SetPredefinedServer( (TSmtpServerType)((TSpeedButton *)Sender)->Tag );
  serverEdit->Text = l.server;
  portEdit->Text = l.port;
  sslComboBox->ItemIndex = l.ssl;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::pmInsertClick(TObject *Sender)
{
  String s;
  switch( ((TMenuItem *)Sender)->Tag )
  {
    case 0: s = "{time}"; break;
    case 1: s = "{ip}"; break;
    case 2: s = "{host}"; break;
    case 3: s = "{facility}"; break;
    case 4: s = "{priority}"; break;
    case 5: s = "{tag}"; break;
    case 6: s = "{message}"; break;
  }

  if( ActiveControl == messageMemo )
    messageMemo->Text = messageMemo->Text + s;

  if( ActiveControl == subjectEdit )
    subjectEdit->Text = subjectEdit->Text + s;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::InsertToMessageButtonClick(TObject *Sender)
{
  POINT p = InsertMemoButton->ClientOrigin;
  InsertPopupMenu->Popup(p.x, p.y + InsertMemoButton->Height);
  ActiveControl = messageMemo;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::InsertSubjectButtonClick(TObject *Sender)
{
  POINT p = InsertSubjectButton->ClientOrigin;
  InsertPopupMenu->Popup(p.x, p.y + InsertSubjectButton->Height);
  ActiveControl = subjectEdit;
}
//---------------------------------------------------------------------------
static void OnTestSendMail(TLetter * l)
{
  if( l->result )
    ReportMess2("Test message sent successfully");
  else
    ReportError2(String("Error sent test message:\n") + l->error);

  if( SetupForm )
  {
    SetupForm->TestButton->Enabled = true;
    SetupForm->TestButton->Caption = "Send test message";
  }
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::TestButtonClick(TObject *Sender)
{
  TSyslogMessage m;
  m.SourceAddr = "127.0.0.1";
  m.Facility = "daemon";
  m.Priority = "err";
  m.DateStr = "Dec  1 13:56:04";
  m.HostName = "host1";
  m.Tag = "proxy[6452]";
  m.Msg = "Host or domain name not found";

  TLetter l;
  if( ! FromInterface(&l) )
    return;
  l.subject = m.Format(l.subject);
  l.message = m.Format(l.message);
  l.callback = OnTestSendMail;
  TSendmailThread::Send(&SendmailThread, &l);

  SetupForm->TestButton->Enabled = false;
  SetupForm->TestButton->Caption = "waiting for smtp server answer...";
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = CancelButton->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::AddFileButtonClick(TObject *Sender)
{
  TStorageFile * p = new TStorageFile;
  p->number = localSFL->GetNewNumber();
  localSFL->Add(p);
  DrawGrid->RowCount = 1 + localSFL->Count;
  DrawGrid->Row = DrawGrid->RowCount - 1;
  DrawGrid->FixedRows = 1;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::DelFileButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 2 )
    return;
  int ARow = DrawGrid->Row - 1;
  // Deleting default file "syslog" not allowed
  if( ARow == 0 )
    return;
  localSFL->Del(ARow);
  DrawGrid->RowCount = 1 + localSFL->Count;
  DrawGrid->OnClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::DrawGridClick(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
  {
    FileFr->Visible = false;
  }
  else
  {
    FileFr->Visible = true;
    FileFr->SetDefaultFile(DrawGrid->Row == 1);

    int ARow = DrawGrid->Row - 1;
    TStorageFile * p = localSFL->Get(ARow);
    if( p )
      FileFr->ToDialog(p);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::DrawGridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  TCanvas * c = ((TStringGrid *)Sender)->Canvas;
  String s;

  if( ARow==0 ) // Grid title
  {
    switch( ACol )
    {
      case 0: s = " File"; break;
      case 1: s = " Rotation"; break;
    }
  }
  else
  {
    TStorageFile * p = localSFL->Get(ARow-1);
    if( p )
    {
      if( ACol == 0 )
        s = String(" ") + p->GetDescription();
      else if( ACol == 1 )
        s = String(" ") + p->GetRotationDescription();
    }
  }
  int x = Rect.Left + 2;
  int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
  c->TextRect(Rect, x, y, s);
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::OnFrameValuesChange(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
    return;
  int ARow = DrawGrid->Row - 1;
  TStorageFile * p = localSFL->Get(ARow);
  if( p )
  {
    FileFr->FromDialog(p);
    DrawGrid->Invalidate();
  }
}
//---------------------------------------------------------------------------

