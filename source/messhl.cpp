//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
#include "messhl.h"

const char * szHighlightsNode = "highlights";
  const char * szProfileNode = "profile";
    const char * szHighlightNode = "highlight";
      const char * szMatchNode = "match";
      const char * szStyleNode = "style";
//---------------------------------------------------------------------------
TMessHighlight::TMessHighlight()
{
  bEnable = true;
}
//---------------------------------------------------------------------------
void TMessHighlight::Save(XMLElementEx * p)
{
  tinyxml2::XMLDocument * d = p->GetDocument();
  XMLElement * e;

  p->wb("enable", bEnable);

  e = d->NewElement(szMatchNode);
  p->InsertEndChild(e);
  Match.Save((XMLElementEx *)e);

  e = d->NewElement(szStyleNode);
  p->InsertEndChild(e);
  Style.Save((XMLElementEx *)e);
}
//---------------------------------------------------------------------------
bool TMessHighlight::Load(XMLElementEx * p)
{
  bool rv = true;
  bEnable = p->rb("enable", true);
  const XMLElement * e;
  e = p->FirstChildElement(szMatchNode);
  if( e ) Match.Load((XMLElementEx *)e);
  else rv = false;
  e = p->FirstChildElement(szStyleNode);
  if( e ) Style.Load((XMLElementEx *)e);
  else rv = false;
  return rv;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TMessHighlightList::TMessHighlightList()
{

}
//---------------------------------------------------------------------------
__fastcall TMessHighlightList::~TMessHighlightList(void)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMessHighlightList::Clear(void)
{
  for(int i=0; i<Count; i++)
    delete (TMessHighlight *)Items[i];
  TList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMessHighlightList::Add(TMessHighlight * p)
{
  TList::Add( p );
}
//---------------------------------------------------------------------------
TMessHighlight * __fastcall TMessHighlightList::Get(int index)
{
  if( index >= 0 && index < Count )
    return (TMessHighlight *)Items[index];
  return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMessHighlightList::Del(int index)
{
  if( index >= 0 && index < Count )
  {
    delete (TMessHighlight *)Items[index];
    TList::Delete(index);
  }
}
//---------------------------------------------------------------------------
TMessStyle * TMessHighlightList::FindStyleForMessage(TSyslogMessage * p)
{
  TMessHighlight * mh;
  for(int i=0; i<Count; i++)
  {
    mh = (TMessHighlight *)Items[i];
    if( mh->bEnable )
      if( mh->Match.Match(p) )
        return &(mh->Style);
  }
  return NULL;
}
//---------------------------------------------------------------------------
void TMessHighlightList::Save(XMLElementEx * p)
{
  p->ws("name", ProfileName);

  tinyxml2::XMLDocument * d = p->GetDocument();
  XMLElement * e;

  TMessHighlight * mh;
  for(int i=0; i<Count; i++)
  {
    mh = (TMessHighlight *)Items[i];
    e = d->NewElement(szHighlightNode);
    p->InsertEndChild(e);
    mh->Save((XMLElementEx *)e);
  }
}
//---------------------------------------------------------------------------
void TMessHighlightList::Load(XMLElementEx * p)
{
  Clear();

  ProfileName = p->rs("name");

  TMessHighlight * mh;
  // highlight
  for(const XMLElement * hl = p->FirstChildElement(szHighlightNode);
      hl;
      hl = hl->NextSiblingElement(szHighlightNode))
  {
    mh = new TMessHighlight;
    mh->Load((XMLElementEx *)hl);
    Add(mh);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall THighlightProfileList::THighlightProfileList()
{

}
//---------------------------------------------------------------------------
__fastcall THighlightProfileList::~THighlightProfileList(void)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall THighlightProfileList::Clear(void)
{
  for(int i=0; i<Count; i++)
    delete (TMessHighlightList *)Items[i];
  TList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall THighlightProfileList::Add(TMessHighlightList * p)
{
  TList::Add( p );
}
//---------------------------------------------------------------------------
TMessHighlightList * THighlightProfileList::GetCurrentProfile(void)
{
  if( CurrentProfile >= 0 && CurrentProfile < Count )
    return (TMessHighlightList *)Items[CurrentProfile];
  return NULL;  
}
//---------------------------------------------------------------------------
void THighlightProfileList::Save(String file)
{
  tinyxml2::XMLDocument doc;
  doc.InsertEndChild( doc.NewDeclaration() );

  XMLNode * hls = doc.InsertEndChild( doc.NewElement(szHighlightsNode) );

  XMLElement * e;
  TMessHighlightList * mhl;
  for(int i=0; i<Count; i++)
  {
    mhl = (TMessHighlightList *)Items[i];
    e = doc.NewElement(szProfileNode);
    hls->InsertEndChild(e);
    mhl->Save((XMLElementEx *)e);
  }

  doc.SaveFile(file.c_str());
}
//---------------------------------------------------------------------------
void THighlightProfileList::Load(String file)
{
  try
  {
    Clear();
    tinyxml2::XMLDocument doc;
    XMLError err = doc.LoadFile(file.c_str());
    if( err == XML_SUCCESS )
    {
      XMLElement * hls;
      hls = doc.RootElement();
      if( ! hls )
        throw 1;
      if( strcmpi(hls->Name(), szHighlightsNode) != 0 )
        throw 1;

      TMessHighlightList * mhl;
      // profiles
      for(const XMLElement * pr = hls->FirstChildElement(szProfileNode);
          pr;
          pr = pr->NextSiblingElement(szProfileNode))
      {
        mhl = new TMessHighlightList;
        mhl->Load((XMLElementEx *)pr);
        Add(mhl);
      }
    }
    else
      throw 0;
  }
  catch(int i)
  {
    if( i==0 )
      ReportError2("Error reading file: \"%s\"", file.c_str());
    else
      ReportError2("Incorrect format of file: \"%s\"", file.c_str());
  }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
