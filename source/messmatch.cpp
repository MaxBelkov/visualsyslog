//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
#include "messmatch.h"

//---------------------------------------------------------------------------
TMessMatch::TMessMatch()
{
  Text1 = new TStringList;
  Text2 = new TStringList;
  Clear();
}
//---------------------------------------------------------------------------
TMessMatch::~TMessMatch()
{
  delete Text1;
  Text1 = NULL;
  delete Text2;
  Text2 = NULL;
}
//---------------------------------------------------------------------------
void TMessMatch::Clear(void)
{
  bNot = false;
  PriorityMask = PriorityMaskAll; // any Priority (match all)
  FacilityMask = FacilityMaskAll; // any Facility (match all)

  Field1 = 0;
  Contains1 = true;
  Text1->Text = "";
  Field2 = 0;
  Contains2 = true;
  Text2->Text = "";

  MatchCase = true;
}
//---------------------------------------------------------------------------
bool TMessMatch::Match(TSyslogMessage * p)
{
  bool rv = LocalMatch(p);
  if( bNot )
    return ! rv;
  return rv;
}
//---------------------------------------------------------------------------
bool TMessMatch::LocalMatch(TSyslogMessage * p)
{
  if( p->PRI != -1 ) // priority and facility value exist in message
  {
    if( ((1<<LOG_PRI(p->PRI)) & PriorityMask) == 0 )
      return false;
    if( ((1<<LOG_FAC(p->PRI)) & FacilityMask) == 0 )
      return false;
  }

  // rule 1
  bool b = true;
  for(int i=0; i<Text1->Count; i++)
  {
    if( MatchAllFilds(p, Field1, Contains1, Text1->Strings[i]) )
    {
      b = true;
      break;
    }
    b = false;
  }
  // strings count > 0 and all strings not match
  if( ! b )
    return false;

  // rule 2
  b = true;
  for(int i=0; i<Text2->Count; i++)
  {
    if( MatchAllFilds(p, Field2, Contains2, Text2->Strings[i]) )
    {
      b = true;
      break;
    }
    b = false;
  }
  if( ! b )
    return false;

  return true;
}
//---------------------------------------------------------------------------
bool TMessMatch::MatchAllFilds(TSyslogMessage * p, int Field, bool Contains,
  String Text)
{
  int l = Text.Length();
  if( l > 0 )
  {
    switch( Field )
    {
      case 0: // Text contains
      {
        bool _d = FieldContains(p->DateStr, Text);
        bool _s = FieldContains(p->SourceAddr, Text);
        bool _h = FieldContains(p->HostName, Text);
        bool _f = FieldContains(p->Facility, Text);
        bool _p = FieldContains(p->Priority, Text);
        bool _t = FieldContains(p->Tag, Text);
        bool _m = FieldContains(p->Msg, Text);
        if( Contains )
          return _d || _s || _h || _f || _p || _t || _m;
        else // NOT contains
          return !_d && !_s && !_h && !_f && !_p && !_t && !_m;
      }
      case 1: // Message text contains
      {
        bool _m = FieldContains(p->Msg, Text);
        if( Contains )
          return _m;
        else // NOT contains
          return !_m;
      }
      case 2: // IP =
        if( MatchCase )
          return Contains == (Text == p->SourceAddr);
        else
          return Contains == SameText(Text, p->SourceAddr, loUserLocale);
      case 3: // Host =
        if( MatchCase )
          return Contains == (Text == p->HostName);
        else
          return Contains == SameText(Text, p->HostName, loUserLocale);
      case 4: // Facility =
        if( MatchCase )
          return Contains == (Text == p->Facility);
        else
          return Contains == SameText(Text, p->Facility, loUserLocale);
      case 5: // Tag =
      {
        // Compare without [PID]
        String _tag = p->Tag;
        int i = _tag.Pos('[');
        if( i > 0 )
          _tag.Delete(i, _tag.Length()-i+1);
        if( MatchCase )
          return Contains == (Text == _tag);
        else
          return Contains == SameText(Text, _tag, loUserLocale);
      }
    }
  }
  return true;
}
//---------------------------------------------------------------------------
bool TMessMatch::FieldContains(String & FieldText, String & Text)
{
  int r;
  if( MatchCase )
    r = FieldText.Pos(Text);
  else
    r = FieldText.UpperCase().Pos(Text.UpperCase());
  return r > 0;
}
//---------------------------------------------------------------------------
char * szAllMessMatch = "All messages match";

String TMessMatch::GetDescription(void)
{
  String rv;
  String t;

  if( PriorityMask != PriorityMaskAll ) // filter by priority enable
  {
    int c=0;
    t = "";
    for(int pri=0; pri<LOG_NPRIORITIES; pri++)
      if( (1<<pri) & PriorityMask )
      {
        if( c++ > 0 )
          t += " OR ";
        t += getcodetext(pri, prioritynames);
      }
    if( c > 1 )
      t = String("(") + t + ")";
    rv += String("Priority = ") + t;
  }

  if( FacilityMask != FacilityMaskAll ) // filter by priority enable
  {
    if( rv.Length() > 0 ) rv += " AND ";
    int c=0;
    t = "";
    for(int fac=0; fac<LOG_NFACILITIES; fac++)
      if( (1<<fac) & FacilityMask )
      {
        if( c++ > 0 )
          t += " OR ";
        t += getcodetext(fac<<3, facilitynames);
      }
    if( c > 1 )
      t = String("(") + t + ")";
    rv += String("Facility = ") + t;
  }

  if( Text1->Count > 0 )
  {
    t = GetDelimitedText(Text1, "\" OR \"");
    if( rv.Length() > 0 ) rv += " AND ";
    switch( Field1 )
    {
      case 0: // Text contains
        rv += String("Text ") + (Contains1 ? "" : "NOT ") + "contains \"" + t + "\"";
      break;
      case 1: // Message text contains
        rv += String("Message ") + (Contains1 ? "" : "NOT ") + "contains \"" + t + "\"";
      break;
      case 2: // IP =
        rv += String("IP") + (Contains1 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 3: // Host =
        rv += String("Host") + (Contains1 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 4: // Facility =
        rv += String("Facility") + (Contains1 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 5: // Tag =
        rv += String("Tag") + (Contains1 ? " = " : " <> ") + "\"" + t + "\"";
      break;
    }
  }

  if( Text2->Count > 0 )
  {
    t = GetDelimitedText(Text2, "\" OR \"");
    if( rv.Length() > 0 ) rv += " AND ";
    switch( Field2 )
    {
      case 0: // Text contains
        rv += String("Text ") + (Contains2 ? "" : "NOT ") + "contains \"" + t + "\"";
      break;
      case 1: // Message text contains
        rv += String("Message ") + (Contains2 ? "" : "NOT ") + "contains \"" + t + "\"";
      break;
      case 2: // IP =
        rv += String("IP") + (Contains2 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 3: // Host =
        rv += String("Host") + (Contains2 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 4: // Facility =
        rv += String("Facility") + (Contains2 ? " = " : " <> ") + "\"" + t + "\"";
      break;
      case 5: // Tag =
        rv += String("Tag") + (Contains2 ? " = " : " <> ") + "\"" + t + "\"";
      break;
    }
  }

  if( rv.Length() == 0 )
    rv = szAllMessMatch;

  if( bNot )
    rv = String("NOT (") + rv + ")";

  return rv;
}
//---------------------------------------------------------------------------
bool TMessMatch::IsAllMatch(void)
{
  return GetDescription() == szAllMessMatch;
}
//---------------------------------------------------------------------------
void TMessMatch::Save(XMLElementEx * p)
{
  p->wb("not", bNot);
  p->wi("prioritymask", PriorityMask);
  p->wi("facilitymask", FacilityMask);
  p->wb("matchcase", MatchCase);
  p->wi("field1", Field1);
  p->wb("contains1", Contains1);
  p->ws("text1", Text1);
  p->wi("field2", Field2);
  p->wb("contains2", Contains2);
  p->ws("text2", Text2);
}
//---------------------------------------------------------------------------
void TMessMatch::Load(XMLElementEx * p)
{
  if( p->exist("prioritymask") )
  {
    bNot = p->rb("not", false);
    PriorityMask = p->ri("prioritymask", 0);
    FacilityMask = p->ri("facilitymask", 0);
  }
  else
  {
    // convert from legacy format
    int OperationP; // 0 =, 1 <= (to emerg), 2 >= (to debug)
    int Priority;   // 0-7: syslog const LOG_, -1: any Priority (match all)

    OperationP = p->ri("operationp", 0);
    Priority = p->ri("priority", -1);
    if( Priority==-1) // any Priority (match all)
    {
      PriorityMask = PriorityMaskAll;
    }
    else
    {
      PriorityMask = 0;
      switch( OperationP )
      {
        case 0: // =
          PriorityMask = 1<<Priority;
        break;
        case 1: // <= (to emerg)
          for(int i=0; i<=Priority; i++)
            PriorityMask |= 1<<Priority;
        break;
        case 2: // >= (to debug)
          for(int i=Priority; i<LOG_NPRIORITIES; i++)
            PriorityMask |= 1<<Priority;
        break;
      }
    }
  }
  MatchCase = p->rb("matchcase", true);
  Field1 = p->ri("field1", 0);
  Contains1 = p->rb("contains1", true);
  p->rs("text1", Text1);
  Field2 = p->ri("field2", 0);
  Contains2 = p->rb("contains2", true);
  p->rs("text2", Text2);

  DeleteEmptyLines();
}
//---------------------------------------------------------------------------
static char * szFilter = "filter";

void TMessMatch::Save(String file, bool bShowMessage)
{
  tinyxml2::XMLDocument doc;
  doc.InsertEndChild( doc.NewDeclaration() );

  XMLElement * e = doc.NewElement(szFilter);
  doc.InsertEndChild( e );
  Save((XMLElementEx *)e);

  XMLError err = doc.SaveFile(file.c_str());
  if( err != XML_SUCCESS )
    if( bShowMessage )
      ReportError2("Error saving file: \"%s\" [%d]", file.c_str(), err);
}
//---------------------------------------------------------------------------
void TMessMatch::Load(String file, bool bShowMessage)
{
  XMLError err;
  try
  {
    Clear();
    tinyxml2::XMLDocument doc;
    err = doc.LoadFile(file.c_str());
    if( err == XML_SUCCESS )
    {
      XMLElement * hls = doc.RootElement();
      if( ! hls )
        throw 1;
      if( strcmpi(hls->Name(), szFilter) != 0 )
        throw 1;

      Load((XMLElementEx *)hls);
    }
    else
      throw 0;
  }
  catch(int i)
  {
    if( bShowMessage )
    {
      if( i==0 )
        ReportError2("Error reading file: \"%s\" [%d]", file.c_str(), err);
      else
        ReportError2("Incorrect format of file: \"%s\"", file.c_str());
    }
  }
}
//---------------------------------------------------------------------------
void TMessMatch::DeleteEmptyLines(void)
{
  for(int i=0; i<Text1->Count; i++)
    if( Text1->Strings[i].Trim().Length() == 0 )
      Text1->Delete(i--);
  for(int i=0; i<Text2->Count; i++)
    if( Text2->Strings[i].Trim().Length() == 0 )
      Text2->Delete(i--);
}
//---------------------------------------------------------------------------
String TMessMatch::GetDelimitedText(TStringList * p, String delimstr)
{
  String rv;
  for(int i=0; i<p->Count; i++)
  {
    if( i > 0 )
      rv += delimstr;
    rv += p->Strings[i];
  }
  return rv;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

