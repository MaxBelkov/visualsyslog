//---------------------------------------------------------------------------
#ifndef messmatchH
#define messmatchH

#include "server.h"
#include "maxxml.h"
//---------------------------------------------------------------------------
class TMessMatch
{
public:
  int Priority;       // 0-7: syslog const LOG_, -1: all Priority
  bool MatchCase;     // default true
  bool TextContains1; // true-Contains Text1 false-NOT Contains Text1
  String Text1;       // text to find in message i:<ip column> h:<host column>
                      // f:<facility column>
  bool TextContains2;
  String Text2;

public:
  TMessMatch();
  // return true if match Priority AND Text1 AND Text2
  bool Match(TSyslogMessage * p);
  String GetDescription(void);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);

private:
  bool MatchTextAllFilds(TSyslogMessage * p, String & Text, bool Contains);
  bool MatchText(String & Field, String & Text);
};
//---------------------------------------------------------------------------
#endif
