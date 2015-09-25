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

extern TStorageFileList * fdb;
extern String WorkDir;
extern TMainCfg MainCfg;
void RunSetup(int SelectedTabIndex);
TMessProcessFr * MessProcessFr;
//---------------------------------------------------------------------------
__fastcall TMessProcessFr::TMessProcessFr(TComponent* Owner)
    : TFrame(Owner)
{
  DefaultRecipient = MainCfg.Letter.recipient;
  OpenDialog->InitialDir = WorkDir;
  fdb->GetList(SaveFileCB->Items);

  TStorageFile * p = fdb->Get(0);
  if( p )
  {
    IgnoreCB->Caption = String("Ignore (do not save to the default file \"") +
      p->GetDescription() + "\")";
  }
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
  String f = SoundFileEdit->Text;

  if( f.Length() == 0 )
    return;

  if( f.Length() > 0 && ExtractFilePath(f).Length() == 0 )
    f = WorkDir + f;

  if( FileExists(f) )
  {
    if( ::PlaySound(f.c_str(), NULL, SND_FILENAME | SND_NODEFAULT |
            SND_NOWAIT | SND_ASYNC) == FALSE )
      ReportError2("Error play sound file: \"%s\"", FormatLastError2(GetLastError()).c_str());
  }
  else
  {
    ReportError2("File \"%s\" not found!", f.c_str());
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
  SaveFileCB->ItemIndex = SaveFileCB->Items->IndexOfObject((TObject *)p->SaveFile);

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
  int i = SaveFileCB->ItemIndex;
  if( i == -1 )
    p->SaveFile = i;
  else
    p->SaveFile = (int)SaveFileCB->Items->Objects[i];
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
void __fastcall TMessProcessFr::SpeedButton2Click(TObject *Sender)
{
  int number = -1;
  int i = SaveFileCB->ItemIndex;
  if( i >= 0 )
    number = (int)SaveFileCB->Items->Objects[i];

  RunSetup(1);
  fdb->GetList(SaveFileCB->Items);
  
  SaveFileCB->ItemIndex = SaveFileCB->Items->IndexOfObject((TObject *)number);
}
//---------------------------------------------------------------------------

