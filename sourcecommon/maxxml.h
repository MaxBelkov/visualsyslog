//---------------------------------------------------------------------------
#ifndef maxxmlH
#define maxxmlH

#include "tinyxml2.h"
using namespace tinyxml2;

class XMLElementEx : public XMLElement
{
public:
  // Read value from element named name
  String rs(const String & name, String defval=String());
  int ri(const String & name, int defval=0);
  double rd(const String & name, double defval=0.0);
  bool rb(const String & name, bool defval=false);

  // Write value to element named name
  void ws(const String & name, String val);
  void wi(const String & name, int val);
  void wd(const String & name, double val);
  void wb(const String & name, bool val);
};
//---------------------------------------------------------------------------
#endif
