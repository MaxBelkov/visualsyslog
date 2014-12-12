//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <mmsystem.h>
#include "cfg.h"
#include "utils.h"
#include "messageform.h"
#include "messprocessframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern String WorkDir;
extern TMainCfg MainCfg;
TMessProcessFr * MessProcessFr;
//---------------------------------------------------------------------------
__fastcall TMessProcessFr::TMessProcessFr(TComponent* Owner)
    : TFrame(Owner)
{
  DefaultRecipient = MainCfg.Letter.recipient;
  OpenDialog->InitialDir = WorkDir;
}
//---------------------------------------------------------------------------
TForm * TMessProcessFr::GetForm(void)
{
  if( Parent )
  {
    if( dynamic_cast<TForm *>(Parent) )
      return (TForm *)Parent;
    if( dynamic_cast<TForm *>(Parent->Parent) )
      return (TForm *)(Parent->Parent);
  }
  return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::InsertAlarmMessButtonClick(TObject *Sender)
{
  POINT p = InsertAlarmMessButton->ClientOrigin;
  InsertPopupMenu->Popup(p.x, p.y + InsertAlarmMessButton->Height);
  if( GetForm() )
    GetForm()->ActiveControl = AlarmMessEdit;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::pmInsertClick(TObject *Sender)
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

  if( GetForm() )
  {
    if( dynamic_cast<TEdit *>(GetForm()->ActiveControl) )
    {
      TEdit * p = (TEdit *)(GetForm()->ActiveControl);
      p->Text = p->Text + s;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::SelWavCommonButtonClick(TObject *Sender)
{
  OpenDialog->Title = "Open sound file";
  OpenDialog->Filter = "Sound files (*.wav)|*.wav|All files (*.*)|*.*";
  OpenDialog->DefaultExt = "wav";
  if( OpenDialog->Execute() )
    SoundFileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::PlayButtonClick(TObject *Sender)
{
  AnsiString str;
  if( FileExists(SoundFileEdit->Text) )
  {
    if( ::PlaySound(str.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_NOWAIT | SND_ASYNC) == FALSE )
      ReportError2("Error play sound file: \"%s\"", FormatLastError2(GetLastError()).c_str());
  }
  else
  {
    ReportError2("File \"%s\" not found!", SoundFileEdit->Text.c_str());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::SpeedButton1Click(TObject *Sender)
{
  OpenDialog->Title = "Open program";
  OpenDialog->Filter = "Program (*.exe)|*.exe|All files (*.*)|*.*";
  OpenDialog->DefaultExt = "exe";
  if( OpenDialog->Execute() )
    ProgFileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::InsertProgramButtonClick(TObject *Sender)
{
  POINT p = InsertProgramButton->ClientOrigin;
  InsertPopupMenu->Popup(p.x, p.y + InsertProgramButton->Height);
  if( GetForm() )
    GetForm()->ActiveControl = ProgFileEdit;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::SpeedButton2Click(TObject *Sender)
{
  OpenDialog->Title = "Select file";
  OpenDialog->Filter = "All files (*.*)|*.*";
  OpenDialog->DefaultExt = "";
  if( OpenDialog->Execute() )
    SaveFileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void TMessProcessFr::ToDialog(TMessProcess * p)
{
  bEnableValuesChange = false;

  IgnoreCB->Checked = p->bIgnore;
  AlarmCB->Checked = p->bAlarm;
  AlarmMessEdit->Text = p->AlarmMess;
  SoundCB->Checked = p->bSound;
  SoundFileEdit->Text = p->SoundFile;
  PlayCountEdit->Text = IntToStr(p->PlayCount);
  SendMailCB->Checked = p->bSendMail;
  RecipientEdit->Text = p->Recipient;
  RecipientEditExit(RecipientEdit);
  RunProgCB->Checked = p->bRunProg;
  ProgFileEdit->Text = p->ProgFile;
  ProgHideCB->Checked = p->bProgHide;
  SaveToFileCB->Checked = p->bSaveToFile;
  SaveFileEdit->Text = p->SaveFile;

  bEnableValuesChange = true;
}
//---------------------------------------------------------------------------
void TMessProcessFr::FromDialog(TMessProcess * p)
{
  p->bIgnore = IgnoreCB->Checked;
  p->bAlarm = AlarmCB->Checked;
  p->AlarmMess = AlarmMessEdit->Text;
  p->bSound = SoundCB->Checked;
  p->SoundFile = SoundFileEdit->Text;
  p->PlayCount = StrToIntDef(PlayCountEdit->Text, 1);
  p->bSendMail = SendMailCB->Checked;
  if( RecipientEdit->Text == DefaultRecipient )
    p->Recipient = "";
  else
    p->Recipient = RecipientEdit->Text;
  p->bRunProg = RunProgCB->Checked;
  p->ProgFile = ProgFileEdit->Text;
  p->bProgHide = ProgHideCB->Checked;
  p->bSaveToFile = SaveToFileCB->Checked;
  p->SaveFile = SaveFileEdit->Text;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::Change(TObject *Sender)
{
  if( bEnableValuesChange )
  {
    if( OnValuesChange )
      OnValuesChange(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::RecipientEditEnter(TObject *Sender)
{
  TEdit * p = (TEdit *)Sender;
  if( p->Text == DefaultRecipient )
  {
    p->Text = "";
    p->Font->Color = clWindowText;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessFr::RecipientEditExit(TObject *Sender)
{
  TEdit * p = (TEdit *)Sender;
  if( p->Text.IsEmpty() )
  {
    p->Text = DefaultRecipient;
    p->Font->Color = clGrayText;
  }
  else if( p->Text == DefaultRecipient )
  {
    p->Font->Color = clGrayText;
  }
}
//---------------------------------------------------------------------------

