//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messstyle.h"

//---------------------------------------------------------------------------
TMessStyle::TMessStyle()
{
  TextColor = clBlack;
  BackgroundColor = clWhite;
  bBold = false;
  bItalic = false;
  bUnderline = false;
}
//---------------------------------------------------------------------------
void TMessStyle::Save(XMLElementEx * p)
{
  p->wi("textColor", (int)TextColor);
  p->wi("backgroundColor", (int)BackgroundColor);
  p->wb("bold", bBold);
  p->wb("italic", bItalic);
  p->wb("underline", bUnderline);
}
//---------------------------------------------------------------------------
void TMessStyle::Load(XMLElementEx * p)
{
  TextColor = (TColor)p->ri("textColor", (int)clBlack);
  BackgroundColor = (TColor)p->ri("backgroundColor", (int)clWhite);
  bBold = p->rb("bold", false);
  bItalic = p->rb("italic", false);
  bUnderline = p->rb("underline", false);
}
//---------------------------------------------------------------------------
void TMessStyle::SetFontStyle(TFont * p)
{
  TFontStyles s = p->Style;
  if( bBold ) s << fsBold;
  else s >> fsBold;
  if( bItalic ) s << fsItalic;
  else s >> fsItalic;
  if( bUnderline ) s << fsUnderline;
  else s >> fsUnderline;
  p->Style = s;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

