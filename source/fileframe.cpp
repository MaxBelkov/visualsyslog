//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "fileframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern String WorkDir;
TFileFr * FileFr;

char szDateTimeFormatHint[] =
"Specify a filename to rename the file after rotation\n"
"Example: file name \"file_{yyyy-mm-dd}\" is converted to name \"file_2015-09-29\" using the current date\n"
"\nAllowable substitutions:\n"
"{d}   the day as a number without a leading zero (1-31)\n"
"{dd}   the day as a number with a leading zero (01-31)\n"
"{ddd}   the day as an abbreviation (Sun-Sat)\n"
"{dddd}   the day as a full name (Sunday-Saturday)\n"
"{m}   the month as a number without a leading zero (1-12). If the m specifier immediately follows an h or hh specifier, the minute rather than the month is using\n"
"{mm}   the month as a number with a leading zero (01-12). If the mm specifier immediately follows an h or hh specifier, the minute rather than the month is using\n"
"{mmm}   the month as an abbreviation (Jan-Dec)\n"
"{mmmm}   the month as a full name (January-December)\n"
"{yy}   the year as a two-digit number (00-99)\n"
"{yyyy}   the year as a four-digit number (0000-9999)\n"
"{h}   the hour without a leading zero (0-23)\n"
"{hh}   the hour with a leading zero (00-23)\n"
"{n}   the minute without a leading zero (0-59)\n"
"{nn}   the minute with a leading zero (00-59)\n"
"{s}   the second without a leading zero (0-59)\n"
"{ss}   the second with a leading zero (00-59)\n"
;
//---------------------------------------------------------------------------
__fastcall TFileFr::TFileFr(TComponent* Owner)
    : TFrame(Owner)
{
  OpenDialog->InitialDir = WorkDir;
  HelpLabel->Hint = szDateTimeFormatHint;
}
//---------------------------------------------------------------------------
void __fastcall TFileFr::SpeedButton2Click(TObject *Sender)
{
  OpenDialog->Title = "Select file";
  OpenDialog->Filter = "All files (*.*)|*.*";
  OpenDialog->DefaultExt = "";
  if( OpenDialog->Execute() )
    FileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void TFileFr::ToDialog(TStorageFile * p)
{
  bEnableValuesChange = false;

  FileEdit->Text = p->GetFileName();

  switch( p->rotation_type )
  {
    case 0: RotationRB1->Checked = true; break;
    case 1: RotationRB2->Checked = true; break;
    case 2: RotationRB3->Checked = true; break;
  }
  SizeEdit->Text = p->rotation_size;
  SizeEditUD->Position = p->rotation_size;
  MultCB->ItemIndex = p->rotation_mult;
  MomentCB->ItemIndex = p->rotation_moment;
  HourEdit->Text = p->rotation_hour;
  HourEditUD->Position = p->rotation_hour;

  switch( p->rotation_renaming )
  {
    case 0: RenamingRB0->Checked = true; break;
    case 1: RenamingRB1->Checked = true; break;
  }
  NewNameEdit->Text = p->rotation_name;
  CountEdit->Text = p->rotation_count;
  CountEditUD->Position = p->rotation_count;

  RotationEnable();

  bEnableValuesChange = true;
}
//---------------------------------------------------------------------------
void TFileFr::RotationEnable(void)
{
  bool b2 = RotationRB2->Checked;
  bool b3 = RotationRB3->Checked;

  SizeEdit->Enabled = b2;
  MultCB->Enabled = b2;
  MomentCB->Enabled = b3;
  HourEdit->Enabled = b3;

  RenamingRB0->Enabled = b2 || b3;
  RenamingRB1->Enabled = b2 || b3;
  CountEdit->Enabled = b2 || b3;
  NewNameEdit->Enabled = b2 || b3;
}
//---------------------------------------------------------------------------
void TFileFr::FromDialog(TStorageFile * p)
{
  p->SetFileName(FileEdit->Text);

  if( RotationRB2->Checked )
    p->rotation_type = 1;
  else if( RotationRB3->Checked )
    p->rotation_type = 2;
  else
    p->rotation_type = 0;

  p->rotation_size = StrToIntDef(SizeEdit->Text, 10);
  if( p->rotation_size < 1 )
    p->rotation_size = 1;

  if( MultCB->ItemIndex == -1 )
    p->rotation_mult = 1;
  else
    p->rotation_mult = MultCB->ItemIndex;

  if( MomentCB->ItemIndex == -1 )
    p->rotation_moment = 1;
  else
    p->rotation_moment = MomentCB->ItemIndex;

  p->rotation_hour = StrToIntDef(HourEdit->Text, 0);
  if( p->rotation_hour < 0 || p->rotation_hour > 23 )
    p->rotation_hour = 0;

  if( RenamingRB0->Checked )
    p->rotation_renaming = 0;
  else
    p->rotation_renaming = 1;
  p->rotation_name = NewNameEdit->Text.Trim();
  p->rotation_count = StrToIntDef(CountEdit->Text, 1);
  if( p->rotation_count < 1 )
    p->rotation_count = 1;
}
//---------------------------------------------------------------------------
void __fastcall TFileFr::Change(TObject *Sender)
{
  if( bEnableValuesChange )
  {
    if( OnValuesChange )
      OnValuesChange(this);
    RotationEnable();
  }
}
//---------------------------------------------------------------------------
void TFileFr::SetDefaultFile(bool b)
{
  if( b )
  {
    GroupBox->Caption = "File settings for default file";
  }
  else
  {
    GroupBox->Caption = "File settings";
  }
}
//---------------------------------------------------------------------------
void __fastcall TFileFr::HelpLabelClick(TObject *Sender)
{
  NewNameEdit->Text = ExtractFileName(FileEdit->Text) + "_{yyyy-mm-dd}";
}
//---------------------------------------------------------------------------

