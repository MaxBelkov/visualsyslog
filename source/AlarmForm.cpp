//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "sound.h"       // play sound thread
#include "saveini.h"     // TSaveParamsINI
#include "AlarmForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TShowAlarmForm * ShowAlarmForm = NULL;
extern TSaveParamsINI * AppParams;
//---------------------------------------------------------------------------
__fastcall TShowAlarmForm::TShowAlarmForm(TComponent* Owner)
    : TForm(Owner)
{
  ListBox->Color = (TColor)RGB(255,197,197);
  *AppParams >> this;
}
//---------------------------------------------------------------------------
void __fastcall TShowAlarmForm::FormDestroy(TObject *Sender)
{
  *AppParams << this;
}
//---------------------------------------------------------------------------
void __fastcall TShowAlarmForm::ClearButtonClick(TObject *Sender)
{
  ListBox->Items->Clear();
  OKButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TShowAlarmForm::OKButtonClick(TObject *Sender)
{
  if( TalkingThread )
    TalkingThread->Stop();
  Close();  
}
//---------------------------------------------------------------------------
