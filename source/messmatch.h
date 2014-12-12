//---------------------------------------------------------------------------
#ifndef messmatchH
#define messmatchH

#include "server.h"
#include "maxxml.h"
//---------------------------------------------------------------------------
class TMessMatch
{
public:
  int OperationP; // 0 =, 1 <= (to emerg), 2 >= (to debug)
  int Priority;   // 0-7: syslog const LOG_, -1: any Priority (match all)
  // AND
  int Field1;     // 0-Text, 1-Message, 2-IP, 3-Host, 4-Facility, 5-Tag
  bool Contains1; // true(1) -  Contains or = Text1
                  // false(0) - NOT Contains or <> Text1
  TStringList * Text1; // text strings to find in message i:<ip column> h:<host column>
                       // f:<facility column>
  // AND
  int Field2;
  bool Contains2;
  TStringList * Text2;

  bool MatchCase; // default true
  
public:
  TMessMatch();
  ~TMessMatch();
  // return true if match Priority AND Text1 AND Text2
  bool Match(TSyslogMessage * p);
  String GetDescription(void);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);

  void DeleteEmptyLines(void);

private:
  bool MatchAllFilds(TSyslogMessage * p, int Field, bool Contains, String Text);
  bool FieldContains(String & FieldText, String & Text);
  String GetDelimitedText(TStringList * p, String delimstr);
};
//---------------------------------------------------------------------------
#endif
