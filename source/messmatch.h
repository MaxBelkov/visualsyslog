//---------------------------------------------------------------------------
#ifndef messmatchH
#define messmatchH

#include "server.h"
#include "maxxml.h"
#include "syslog.h"

#define PriorityMaskAll 0xFF
#define FacilityMaskAll 0x00FFFFFF // 24 facilities
//---------------------------------------------------------------------------
class TMessMatch
{
public:
  bool bNot;          // true --> Negate (apply logical NOT)
  BYTE PriorityMask;  // 8 priorities (LOG_)
  DWORD FacilityMask; // 24 facilities (LOG_)

  // rule 1
  int Field1;     // 0-Text, 1-Message, 2-IP, 3-Host, 4-Facility, 5-Tag
  bool Contains1; // true(1) -  Contains or = Text1
                  // false(0) - NOT Contains or <> Text1
  TStringList * Text1; // text strings to find in message

  // AND

  // rule 2
  int Field2;
  bool Contains2;
  TStringList * Text2;

  bool MatchCase; // default true
  
public:
  TMessMatch();
  ~TMessMatch();
  void Clear(void);
  // return true if match Priority AND Text1 AND Text2
  bool Match(TSyslogMessage * p);
  String GetDescription(void);
  bool IsAllMatch(void);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);
  void Save(String file, bool bShowMessage=true);
  void Load(String file, bool bShowMessage=true);

  void DeleteEmptyLines(void);

private:
  bool LocalMatch(TSyslogMessage * p);
  bool MatchAllFilds(TSyslogMessage * p, int Field, bool Contains, String Text);
  bool FieldContains(String & FieldText, String & Text);
  String GetDelimitedText(TStringList * p, String delimstr);
};
//---------------------------------------------------------------------------
#endif
