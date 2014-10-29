//---------------------------------------------------------------------------
#ifndef messhlH
#define messhlH

#include "messmatch.h"
#include "messstyle.h"
//---------------------------------------------------------------------------
// message highlight item
class TMessHighlight
{
public:
  bool bEnable;
  TMessMatch Match;
  TMessStyle Style;

public:
  TMessHighlight();
  void Save(XMLElementEx * p);
  bool Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
// message highlight item list (profile)
class TMessHighlightList : public TList
{
public:
  String ProfileName;

public:
  __fastcall TMessHighlightList();
  __fastcall virtual ~TMessHighlightList(void);
  void __fastcall Add(TMessHighlight * p);
  TMessHighlight * __fastcall Get(int index);
  void __fastcall Del(int index);

  // if not found - return NULL
  TMessStyle * FindStyleForMessage(TSyslogMessage * p);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);

private:
  virtual void __fastcall Clear(void);
};
//---------------------------------------------------------------------------
// message highlight profile list
class THighlightProfileList : public TList
{
public:
  int CurrentProfile; // index of current profile

public:
  __fastcall THighlightProfileList();
  __fastcall virtual ~THighlightProfileList(void);
  void __fastcall Add(TMessHighlightList * p);

  TMessHighlightList * GetCurrentProfile(void);
  void Save(String file);
  void Load(String file);

private:
  virtual void __fastcall Clear(void);
};
//---------------------------------------------------------------------------
#endif

