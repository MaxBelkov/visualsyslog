//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "fileframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern String WorkDir;
TFileFr * FileFr;
//---------------------------------------------------------------------------
__fastcall TFileFr::TFileFr(TComponent* Owner)
    : TFrame(Owner)
{
  OpenDialog->InitialDir = WorkDir;
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

  bEnableValuesChange = true;
}
//---------------------------------------------------------------------------
void TFileFr::FromDialog(TStorageFile * p)
{
  p->SetFileName(FileEdit->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFileFr::Change(TObject *Sender)
{
  if( bEnableValuesChange )
  {
    if( OnValuesChange )
      OnValuesChange(this);
  }
}
//---------------------------------------------------------------------------
void TFileFr::CanEdit(bool b)
{
  if( b )
  {
    GroupBox->Caption = "File settings";
    //FileEdit->Font->Color = clWindowText;
  }
  else
  {
    GroupBox->Caption = "File settings for default file";// (read only)";
    //FileEdit->Font->Color = clGrayText;
  }
  //Enabled = b;
}
//---------------------------------------------------------------------------

