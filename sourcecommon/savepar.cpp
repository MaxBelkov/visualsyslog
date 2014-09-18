//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "savepar.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
bool CorrectFormRect(int & l, int & t, int & w, int & h);

//---------------------------------------------------------------------------
TParamValue::TParamValue(AnsiString n, Variant v) :
                         name(n), value(v)
{}
TParamValue::TParamValue(void)
{}
bool TParamValue::Empty(void)
{ return value.IsEmpty(); }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TSaveParams::TSaveParams(AnsiString inFile, bool inCreateNewFile)
{
  fFile = inFile;
  fCreateNewFile = inCreateNewFile;
  CurrentSection = "default";
  fMinimumColumnWidth = 0;
}
/*---------------------------------------------------------------------------
TSaveParams::TSaveParams(void)
{
  fCreateNewFile = false;
  CurrentSection = "default";
  fMinimumColumnWidth = 0;
}
*/
//---------------------------------------------------------------------------
// Эти функции работают с текущей секцией fCurrentSection
void __fastcall TSaveParams::SetByName(AnsiString name, Variant value)
{
  Set(fCurrentSection, name, value);
}
//---------------------------------------------------------------------------
Variant __fastcall TSaveParams::GetByName(AnsiString name)
{
  return Get(fCurrentSection, name);
}
//---------------------------------------------------------------------------
Variant TSaveParams::GetByNameDef(AnsiString name, Variant defval)
{
  return Get(fCurrentSection, name, defval);
}
//---------------------------------------------------------------------------
void __fastcall TSaveParams::SetByIndex(int index, Variant value)
{
  Set(fCurrentSection, AnsiString("index") + IntToStr(index), value);
}
//---------------------------------------------------------------------------
Variant __fastcall TSaveParams::GetByIndex(int index)
{
  return Get(fCurrentSection, AnsiString("index") + IntToStr(index));
}
//---------------------------------------------------------------------------
Variant TSaveParams::GetByIndexDef(int index, Variant defval)
{
  return Get(fCurrentSection, AnsiString("index") + IntToStr(index), defval);
}
//---------------------------------------------------------------------------
// Сохранение положения и размеров формы
void TSaveParams::SetVcl(TForm * p)
{
  try
  {
    fCurrentSection = p->Name;
    Set(fCurrentSection, "Left", p->Left);
    Set(fCurrentSection, "Top", p->Top);
    Set(fCurrentSection, "Width", p->Width);
    Set(fCurrentSection, "Height", p->Height);
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TForm * p)
{
  try
  {
    int l,t,w,h;
    fCurrentSection = p->Name;
    Variant v;
    v = Get(fCurrentSection, "Left");
    if( v.IsEmpty() ) return;
    l = v;
    v = Get(fCurrentSection, "Top");
    if( v.IsEmpty() ) return;
    t = v;
    v = Get(fCurrentSection, "Width");
    if( v.IsEmpty() ) return;
    w = v;
    v = Get(fCurrentSection, "Height");
    if( v.IsEmpty() ) return;
    h = v;

    if( CorrectFormRect(l, t, w, h) )
      p->SetBounds(l, t, w, h);
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
// Сохранение положения формы
void TSaveParams::SetVclPos(TForm * p)
{
  try
  {
    fCurrentSection = p->Name;
    Set(fCurrentSection, "Left", p->Left);
    Set(fCurrentSection, "Top", p->Top);
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
void TSaveParams::GetVclPos(TForm * p)
{
  try
  {
    int l,t,w,h;
    fCurrentSection = p->Name;
    Variant v;

    v = Get(fCurrentSection, "Left");
    if( v.IsEmpty() ) return;
    l = v;

    v = Get(fCurrentSection, "Top");
    if( v.IsEmpty() ) return;
    t = v;

    w = p->Width;

    h = p->Height;

    if( CorrectFormRect(l, t, w, h) )
      p->SetBounds(l, t, w, h);
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
// Сохранение ширин колонок
void TSaveParams::SetVcl(TStringGrid * p)
{
  fCurrentSection = p->Owner->Name;
  for(int j=0; j<p->ColCount; j++)
    Set(fCurrentSection, p->Name + "_col" + IntToStr(j+1), p->ColWidths[j]);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TStringGrid * p)
{
  fCurrentSection = p->Owner->Name;
  Variant v;
  for(int j=0; j<p->ColCount; j++)
  {
    v = Get(fCurrentSection, p->Name + "_col" + IntToStr(j+1));
    if( ! v.IsEmpty() )
      p->ColWidths[j] = MAX(v,fMinimumColumnWidth);
  }
}
//---------------------------------------------------------------------------
// Сохранение ширин колонок
void TSaveParams::SetVcl(TDBGrid * p)
{
  fCurrentSection = p->Owner->Name;
  for(int j=0; j<p->Columns->Count; j++)
    Set(fCurrentSection, p->Name + "_col" + IntToStr(j+1), p->Columns->Items[j]->Width);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TDBGrid * p)
{
  fCurrentSection = p->Owner->Name;
  Variant v;
  for(int j=0; j<p->Columns->Count; j++)
  {
    v = Get(fCurrentSection, p->Name + "_col" + IntToStr(j+1));
    if( ! v.IsEmpty() )
      p->Columns->Items[j]->Width = MAX(v,fMinimumColumnWidth);
  }
}
//---------------------------------------------------------------------------
// Сохранение ширин колонок
void TSaveParams::SetVcl(TListView * p)
{
  fCurrentSection = p->Owner->Name;
  for(int j=0; j<p->Columns->Count; j++)
    Set(fCurrentSection, p->Name + "_col" + IntToStr(j+1), p->Columns->Items[j]->Width);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TListView * p)
{
  fCurrentSection = p->Owner->Name;
  Variant v;
  for(int j=0; j<p->Columns->Count; j++)
  {
    v = Get(fCurrentSection, p->Name + "_col" + IntToStr(j+1));
    if( ! v.IsEmpty() )
      p->Columns->Items[j]->Width = MAX(v,fMinimumColumnWidth);
  }
}
//---------------------------------------------------------------------------
void TSaveParams::SetVcl(TEdit * p)
{
  fCurrentSection = p->Owner->Name;
  Set(fCurrentSection, p->Name, p->Text);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TEdit * p)
{
  fCurrentSection = p->Owner->Name;
  p->Text = Get(fCurrentSection, p->Name);
}
//---------------------------------------------------------------------------
void TSaveParams::SetVcl(TCheckBox * p)
{
  fCurrentSection = p->Owner->Name;
  Set(fCurrentSection, p->Name, p->Checked);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TCheckBox * p)
{
  fCurrentSection = p->Owner->Name;
  p->Checked = Get(fCurrentSection, p->Name);
}
//---------------------------------------------------------------------------
void TSaveParams::SetVcl(TParamValue * p)
{
  Set(fCurrentSection, p->name, p->value);
}
//---------------------------------------------------------------------------
void TSaveParams::GetVcl(TParamValue * p)
{
  p->value = Get(fCurrentSection, p->name);
}
//---------------------------------------------------------------------------
void TSaveParams::SetMany(TParamValue * p, int count)
{
  for(int i=0; i<count; i++)
	SetVcl(&p[i]);
}
//---------------------------------------------------------------------------
void TSaveParams::GetMany(TParamValue * p, int count)
{
  for(int i=0; i<count; i++)
	GetVcl(&p[i]);
}
//---------------------------------------------------------------------------
void TSaveParams::SetFont(AnsiString Section, AnsiString Name, TFont* Font)
{
        int StInt;
        TFontStyles St;
	Set(Section, Name + "_Name", Font->Name);
	Set(Section, Name + "_Charset", Font->Charset);
	Set(Section, Name + "_Size", Font->Size);
	Set(Section, Name + "_Color", Font->Color);
        St = Font->Style;
        memcpy(&StInt, &St, 1);
	Set(Section, Name + "_Style", StInt);
}
//---------------------------------------------------------------------------
void TSaveParams::GetFont(AnsiString Section, AnsiString Name, TFont* Font)
{
	Variant v;
        int StInt;
        TFontStyles St;

	v = Get(Section, Name + "_Name");
	if(!v.IsEmpty()) Font->Name = v;

	v = Get(Section, Name + "_Charset");
	if(!v.IsEmpty()) Font->Charset = v;

	v = Get(Section, Name + "_Size");
	if(!v.IsEmpty()) Font->Size = v;

	v = Get(Section, Name + "_Color");
	if(!v.IsEmpty())
	{
		int cl = v;
		Font->Color = TColor(cl);
	}

	v = Get(Section, Name + "_Style");
	if(!v.IsEmpty())
        {
                try
                {
                        StInt = v;
                }
                catch(...)
                {
                        StInt = 0;
                }
                memcpy(&St, &StInt, 1);
                Font->Style = St;
        }
}
//---------------------------------------------------------------------------
TSaveParams & TSaveParams::operator << (TForm * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TForm * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator < (TForm * p) { SetVclPos(p); return *this; }
TSaveParams & TSaveParams::operator > (TForm * p) { GetVclPos(p); return *this; }
TSaveParams & TSaveParams::operator << (TStringGrid * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TStringGrid * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator << (TDBGrid * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TDBGrid * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator << (TListView * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TListView * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator << (TEdit * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TEdit * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator << (TCheckBox * p) { SetVcl(p); return *this; }
TSaveParams & TSaveParams::operator >> (TCheckBox * p) { GetVcl(p); return *this; }
TSaveParams & TSaveParams::operator << (TParamValue & p)  { SetVcl(&p); return *this; }
TSaveParams & TSaveParams::operator >> (TParamValue & p) { GetVcl(&p); return *this; }
//---------------------------------------------------------------------------
int TSaveParams::ReadInteger(const AnsiString Section, const AnsiString Ident, int Default)
{
  return Get(Section, Ident, Default);
}
AnsiString TSaveParams::ReadString(const AnsiString Section, const AnsiString Ident, const AnsiString Default)
{
  return Get(Section, Ident, Default);
}
double TSaveParams::ReadFloat(const AnsiString Section, const AnsiString Ident, double Default)
{
  return Get(Section, Ident, Default);
}
bool TSaveParams::ReadBool(const AnsiString Section, const AnsiString Ident, bool Default)
{
  return Get(Section, Ident, Default);
}
void TSaveParams::WriteInteger(const AnsiString Section, const AnsiString Ident, int Value)
{
  Set(Section, Ident, Value);
}
void TSaveParams::WriteString(const AnsiString Section, const AnsiString Ident, const AnsiString Value)
{
  Set(Section, Ident, Value);
}
void TSaveParams::WriteFloat(const AnsiString Section, const AnsiString Ident, double Value)
{
  Set(Section, Ident, Value);
}
void TSaveParams::WriteBool(const AnsiString Section, const AnsiString Ident, bool Value)
{
  Set(Section, Ident, Value);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
