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
  // Read value from element named name
  String rs(const String name, String defval=String());
  void rs(const String name, TStrings * val);
  int ri(const String name, int defval=0);
  double rd(const String name, double defval=0.0);
  bool rb(const String name, bool defval=false);

  // Write value to element named name
  void ws(const String name, String val);
  void ws(const String name, TStrings * val);
  void wi(const String name, int val);
  void wd(const String name, double val);
  void wb(const String name, bool val);
};
//---------------------------------------------------------------------------
#endif
