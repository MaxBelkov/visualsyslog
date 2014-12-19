//---------------------------------------------------------------------------
#ifndef maxxmlH
#define maxxmlH

#include "tinyxml2.h"
using namespace tinyxml2;

// xml read / write helper class
class XMLElementEx : public XMLElement
{
private:
  // delete all carriage returns \r from string s
  // convert to utf8
  AnsiString ConvertToSave(const String s);
  // add carriage return to all line feed
  // convert from utf8
  String ConvertAfterLoad(const AnsiString s);

public:
  bool exist(const AnsiString name);

  // Read value from element named name
  String rs(const AnsiString name, String defval=String());
  void rs(const AnsiString name, TStrings * val);
  int ri(const AnsiString name, int defval=0);
  double rd(const AnsiString name, double defval=0.0);
  bool rb(const AnsiString name, bool defval=false);

  // Write value to element named name
  void ws(const AnsiString name, String val);
  void ws(const AnsiString name, TStrings * val);
  void wi(const AnsiString name, int val);
  void wd(const AnsiString name, double val);
  void wb(const AnsiString name, bool val);
};
//---------------------------------------------------------------------------
#endif
