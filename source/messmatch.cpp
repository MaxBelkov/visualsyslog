//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//#include "inif.h"
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
String TMessMatch::GetDescription(void)
{
  String rv;
  if( Priority != -1 ) // filter by priority enable
    rv = String("Priority = ") + getcodetext(LOG_PRI(Priority), prioritynames);

  if( Text1.Length() > 0 )
  {
    if( rv.Length() > 0 ) rv += " AND ";
    rv += String("Message ") + (TextContains1 ? "" : "NOT") +
      " contains \"" + Text1 + "\"";
  }

  if( Text2.Length() > 0 )
  {
    if( rv.Length() > 0 ) rv += " AND ";
    rv += String("Message ") + (TextContains2 ? "" : "NOT") +
      " contains \"" + Text2 + "\"";
  }

  return rv;
}
//---------------------------------------------------------------------------
void TMessMatch::Save(XMLElementEx * p)
{
  p->wi("Priority", Priority);
  p->wb("MatchCase", MatchCase);
  p->wb("Contains1", TextContains1);
  p->ws("Text1", Text1);
  p->wb("Contains2", TextContains2);
  p->ws("Text2", Text2);
}
//---------------------------------------------------------------------------
void TMessMatch::Load(XMLElementEx * p)
{
  Priority = p->ri("Priority", -1);
  MatchCase = p->rb("MatchCase", true);
  TextContains1 = p->rb("Contains1", true);
  Text1 = p->rs("Text1");
  TextContains2 = p->rb("Contains2", true);
  Text2 = p->rs("Text2");
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

