//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messstyleframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMessStyleFr *MessStyleFr;
//---------------------------------------------------------------------------
__fastcall TMessStyleFr::TMessStyleFr(TComponent* Owner)
    : TFrame(Owner)
{
  OnValuesChange = NULL;
}
//---------------------------------------------------------------------------
void TMessStyleFr::ToDialog(TMessStyle * p)
{
  EnableStyleChange = false;
  TextColorBox->Selected = p->TextColor;
  BackgroundColorBox->Selected = p->BackgroundColor;
  BoldCB->Checked = p->bBold;
  ItalicCB->Checked = p->bItalic;
  UnderlineCB->Checked = p->bUnderline;
  EnableStyleChange = true;
  //UpdatePreview();
}
//---------------------------------------------------------------------------
void TMessStyleFr::FromDialog(TMessStyle * p)
{
  p->TextColor = TextColorBox->Selected;
  p->BackgroundColor = BackgroundColorBox->Selected;
  p->bBold = BoldCB->Checked;
  p->bItalic = ItalicCB->Checked;
  p->bUnderline = UnderlineCB->Checked;
}
//---------------------------------------------------------------------------
/*
void TMessStyleFr::UpdatePreview(void)
{
  PreviewLabel->Font->Color = TextColorBox->Selected;
  PreviewLabel->Color = BackgroundColorBox->Selected;

  TFontStyles s = PreviewLabel->Font->Style;
  if( BoldCB->Checked ) s << fsBold;
  else s >> fsBold;
  if( ItalicCB->Checked ) s << fsItalic;
  else s >> fsItalic;
  if( UnderlineCB->Checked ) s << fsUnderline;
  else s >> fsUnderline;
  PreviewLabel->Font->Style = s;
}
*/
//---------------------------------------------------------------------------
void __fastcall TMessStyleFr::StyleChange(TObject *Sender)
{
  if( EnableStyleChange )
  {
    //UpdatePreview();
    if( OnValuesChange )
      OnValuesChange(this);
  }
}
//---------------------------------------------------------------------------

