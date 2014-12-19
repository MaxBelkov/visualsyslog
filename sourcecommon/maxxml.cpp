//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "maxxml.h"

//---------------------------------------------------------------------------
AnsiString XMLElementEx::ConvertToSave(const String s)
{
  String rv;
  for(int i=1, l=s.Length(); i<=l; i++)
  {
    if( s[i] != '\r' )
      rv += s[i];
  }
  return UTF8Encode(rv);
}
//---------------------------------------------------------------------------
String XMLElementEx::ConvertAfterLoad(const AnsiString s)
{
  String rv = UTF8Decode(s);
  // add carriage return to all line feed
  String rv2;
  for(int i=1, l=rv.Length(); i<=l; i++)
  {
    if( rv[i]=='\n' )
      rv2 += '\r';
    rv2 += rv[i];
  }
  return rv2;
}
//---------------------------------------------------------------------------
bool XMLElementEx::exist(const AnsiString name)
{
  return FirstChildElement(name.c_str()) != NULL;
}
//---------------------------------------------------------------------------
String XMLElementEx::rs(const AnsiString name, String defval)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( p )
  {
    return ConvertAfterLoad(AnsiString(p->GetText()));
  }
  return defval;
}
//---------------------------------------------------------------------------
void XMLElementEx::rs(const AnsiString name, TStrings * val)
{
  val->Clear();
  for(XMLElement * p = FirstChildElement(name.c_str());
      p;
      p = p->NextSiblingElement(name.c_str()))
  {
    val->Add( ConvertAfterLoad(AnsiString(p->GetText())) );
  }
}
//---------------------------------------------------------------------------
int XMLElementEx::ri(const AnsiString name, int defval)
{
  int rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryIntText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
double XMLElementEx::rd(const AnsiString name, double defval)
{
  double rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryDoubleText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
bool XMLElementEx::rb(const AnsiString name, bool defval)
{
  bool rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryBoolText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
void XMLElementEx::ws(const AnsiString name, String val)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( ! p )
  {
    p = GetDocument()->NewElement(name.c_str());
    InsertEndChild(p);
  }
  p->SetText( ConvertToSave(val).c_str() );
}
//---------------------------------------------------------------------------
void XMLElementEx::ws(const AnsiString name, TStrings * val)
{
  XMLElement * p;
  while( (p = FirstChildElement(name.c_str())) != NULL )
    DeleteChild(p);

  for(int i=0, l=val->Count; i<l; i++)
  {
    p = GetDocument()->NewElement(name.c_str());
    InsertEndChild(p);
    p->SetText( ConvertToSave(val->Strings[i]).c_str() );
  }
}
//---------------------------------------------------------------------------
void XMLElementEx::wi(const AnsiString name, int val)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( ! p )
  {
    XMLElement * e = GetDocument()->NewElement(name.c_str());
    InsertEndChild(e);
    e->SetText(val);
  }
  else
  {
    p->SetText(val);
  }
}
//---------------------------------------------------------------------------
void XMLElementEx::wd(const AnsiString name, double val)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( ! p )
  {
    XMLElement * e = GetDocument()->NewElement(name.c_str());
    InsertEndChild(e);
    e->SetText(val);
  }
  else
  {
    p->SetText(val);
  }
}
//---------------------------------------------------------------------------
void XMLElementEx::wb(const AnsiString name, bool val)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( ! p )
  {
    XMLElement * e = GetDocument()->NewElement(name.c_str());
    InsertEndChild(e);
    e->SetText(val);
  }
  else
  {
    p->SetText(val);
  }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
