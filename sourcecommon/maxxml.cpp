//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "maxxml.h"

//---------------------------------------------------------------------------
String XMLElementEx::rs(const String & name, String defval)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) return String(p->GetText());
  return defval;
}
//---------------------------------------------------------------------------
int XMLElementEx::ri(const String & name, int defval)
{
  int rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryIntText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
double XMLElementEx::rd(const String & name, double defval)
{
  double rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryDoubleText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
bool XMLElementEx::rb(const String & name, bool defval)
{
  bool rv;
  XMLElement * p = FirstChildElement(name.c_str());
  if( p ) if( p->QueryBoolText(&rv) == XML_SUCCESS ) return rv;
  return defval;
}
//---------------------------------------------------------------------------
void XMLElementEx::ws(const String & name, String val)
{
  XMLElement * p = FirstChildElement(name.c_str());
  if( ! p )
  {
    XMLElement * e = GetDocument()->NewElement(name.c_str());
    InsertEndChild(e);
    e->SetText(val.c_str());
  }
  else
  {
    p->SetText(val.c_str());
  }
}
//---------------------------------------------------------------------------
void XMLElementEx::wi(const String & name, int val)
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
void XMLElementEx::wd(const String & name, double val)
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
void XMLElementEx::wb(const String & name, bool val)
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
