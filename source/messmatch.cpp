//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "syslog.h"
#include "messmatch.h"

//---------------------------------------------------------------------------
TMessMatch::TMessMatch()
{
  Priority = -1;
  MatchCase = true;
  TextContains1 = true;
  TextContains2 = true;
}
//---------------------------------------------------------------------------
bool TMessMatch::Match(TSyslogMessage * p)
{
  if( Priority != -1 ) // filter by priority enable
    if( p->PRI != -1 ) // priority value exist in message
      if( LOG_PRI(p->PRI) != LOG_PRI(Priority) )
        return false;

  if( ! MatchTextAllFilds(p, Text1, TextContains1) )
    return false;
  if( ! MatchTextAllFilds(p, Text2, TextContains2) )
    return false;
    
  return true;
}
//---------------------------------------------------------------------------
bool TMessMatch::MatchTextAllFilds(TSyslogMessage * p, String & Text, bool Contains)
{
  int l = Text.Length();
  if( l > 0 )
  {
    if( l >= 3 )
    {
      // text to find in message i:<ip column> h:<host column> f:<facility column>
      char c = Text[1];
      if( Text[2] == ':' )
      {
        if( c == 'i' || c == 'I' )
          return Contains == (Text.SubString(3, l-2) == p->SourceAddr);
        if( c == 'h' || c == 'H' )
          return Contains == (Text.SubString(3, l-2) == p->HostName);
        if( c == 'f' || c == 'F' )
          return Contains == (Text.SubString(3, l-2) == p->Facility);
        if( c == 't' || c == 'T' )
          return Contains == (Text.SubString(3, l-2) == p->Tag);
      }
    }

    bool _d = MatchText(p->DateStr, Text);
    bool _s = MatchText(p->SourceAddr, Text);
    bool _h = MatchText(p->HostName, Text);
    bool _f = MatchText(p->Facility, Text);
    bool _p = MatchText(p->Priority, Text);
    bool _t = MatchText(p->Tag, Text);
    bool _m = MatchText(p->Msg, Text);

    if( Contains )
      return _d || _s || _h || _f || _p || _t || _m;
    else // NOT contains
      return !_d && !_s && !_h && !_f && !_p && !_t && !_m;
  }
  return true;
}
//---------------------------------------------------------------------------
bool TMessMatch::MatchText(String & Field, String & Text)
{
  int r;
  if( MatchCase )
    r = Field.Pos(Text);
  else
    r = Field.UpperCase().Pos(Text.UpperCase());
  return r > 0;
}
//---------------------------------------------------------------------------
bool TMessMatch::IsIPFilter(String & Text)
{
  if( Text.Length() >= 3 )
    if( Text[2] == ':' )
      if( Text[1] == 'i' || Text[1] == 'I' )
        return true;
  return false;
}
//---------------------------------------------------------------------------
bool TMessMatch::IsHostFilter(String & Text)
{
  if( Text.Length() >= 3 )
    if( Text[2] == ':' )
      if( Text[1] == 'h' || Text[1] == 'H' )
        return true;
  return false;
}
//---------------------------------------------------------------------------
bool TMessMatch::IsFacilityFilter(String & Text)
{
  if( Text.Length() >= 3 )
    if( Text[2] == ':' )
      if( Text[1] == 'f' || Text[1] == 'F' )
        return true;
  return false;
}
//---------------------------------------------------------------------------
bool TMessMatch::IsTagFilter(String & Text)
{
  if( Text.Length() >= 3 )
    if( Text[2] == ':' )
      if( Text[1] == 't' || Text[1] == 'T' )
        return true;
  return false;
}
//---------------------------------------------------------------------------
String TMessMatch::GetDescription(void)
{
  String rv;

  if( Priority != -1 ) // filter by priority enable
    rv = String("Priority = ") + getcodetext(LOG_PRI(Priority), prioritynames);

  int l = Text1.Length();
  if( l > 0 )
  {
    if( rv.Length() > 0 ) rv += " AND ";

    if( IsIPFilter(Text1) )
      rv += String("IP")+ (TextContains1 ? " = " : " <> ") +
        "\"" + Text1.SubString(3, l-2) + "\"";
    else if( IsHostFilter(Text1) )
      rv += String("Host")+ (TextContains1 ? " = " : " <> ") +
        "\"" + Text1.SubString(3, l-2) + "\"";
    else if( IsFacilityFilter(Text1) )
      rv += String("Facility")+ (TextContains1 ? " = " : " <> ") +
        "\"" + Text1.SubString(3, l-2) + "\"";
    else if( IsTagFilter(Text1) )
      rv += String("Tag")+ (TextContains1 ? " = " : " <> ") +
        "\"" + Text1.SubString(3, l-2) + "\"";
    else
      rv += String("Text ") + (TextContains1 ? "" : "NOT ") +
        "contains \"" + Text1 + "\"";
  }

  l = Text2.Length();
  if( l > 0 )
  {
    if( rv.Length() > 0 ) rv += " AND ";

    if( IsIPFilter(Text2) )
      rv += String("IP")+ (TextContains2 ? " = " : " <> ") +
        "\"" + Text2.SubString(3, l-2) + "\"";
    else if( IsHostFilter(Text2) )
      rv += String("Host")+ (TextContains2 ? " = " : " <> ") +
        "\"" + Text2.SubString(3, l-2) + "\"";
    else if( IsFacilityFilter(Text2) )
      rv += String("Facility")+ (TextContains2 ? " = " : " <> ") +
        "\"" + Text2.SubString(3, l-2) + "\"";
    else if( IsTagFilter(Text2) )
      rv += String("Tag")+ (TextContains2 ? " = " : " <> ") +
        "\"" + Text2.SubString(3, l-2) + "\"";
    else
      rv += String("Text ") + (TextContains2 ? "" : "NOT ") +
        "contains \"" + Text2 + "\"";
  }

  if( rv.Length() == 0 )
    rv = "All messages match";

  return rv;
}
//---------------------------------------------------------------------------
void TMessMatch::Save(XMLElementEx * p)
{
  p->wi("priority", Priority);
  p->wb("matchcase", MatchCase);
  p->wb("contains1", TextContains1);
  p->ws("text1", Text1);
  p->wb("contains2", TextContains2);
  p->ws("text2", Text2);
}
//---------------------------------------------------------------------------
void TMessMatch::Load(XMLElementEx * p)
{
  Priority = p->ri("priority", -1);
  MatchCase = p->rb("matchcase", true);
  TextContains1 = p->rb("contains1", true);
  Text1 = p->rs("text1");
  TextContains2 = p->rb("contains2", true);
  Text2 = p->rs("text2");
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

