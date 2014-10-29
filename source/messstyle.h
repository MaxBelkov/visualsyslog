//---------------------------------------------------------------------------
#ifndef messstyleH
#define messstyleH

#include "maxxml.h"
//---------------------------------------------------------------------------
class TMessStyle
{
public:
  TColor TextColor;       // font color
  TColor BackgroundColor;
  bool bBold;             // font style
  bool bItalic;
  bool bUnderline;

public:
  TMessStyle();
  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);
  void SetFontStyle(TFont * p);
};
//---------------------------------------------------------------------------
#endif

