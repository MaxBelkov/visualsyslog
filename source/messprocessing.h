//---------------------------------------------------------------------------
#ifndef messprocessingH
#define messprocessingH

#include "maxxml.h"
#include "messmatch.h"
//---------------------------------------------------------------------------
// message process item
class TMessProcess
{
public:
  bool bIgnore;       // Ignore (do not save to the default file "syslog")
  bool bAlarm;        //
    String AlarmMess; //
  bool bSound;        //
    String SoundFile; //
    int PlayCount;    //
  bool bSendMail;     //
    String Recipient; //
  bool bRunProg;      //
    String ProgFile;  //
    bool bProgHide;   //
  bool bSaveToFile;   //
    int SaveFile;     // link to TStorageFile::number (-1 file not selected)

public:
  TMessProcess();
  String GetDescription(void);
  String GetSoundFileName(void);
  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
// message process rule item
class TMessProcessRule
{
public:
  bool bEnable;
  TMessMatch Match;
  TMessProcess Process;

public:
  TMessProcessRule();
  void Save(XMLElementEx * p);
  bool Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
// message process rule list
class TMessProcessRuleList : public TList
{
public:
  __fastcall TMessProcessRuleList();
  __fastcall virtual ~TMessProcessRuleList(void);
  void __fastcall Add(TMessProcessRule * p);
  TMessProcessRule * __fastcall Get(int index);
  void __fastcall Del(int index);

  void Save(String file);
  void Load(String file);

private:
  virtual void __fastcall Clear(void);
};
//---------------------------------------------------------------------------
#endif
