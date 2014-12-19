//---------------------------------------------------------------------------
#ifndef fdbH
#define fdbH

#include "maxxml.h"      // work with xml
#include "file.h"        // work with files

//---------------------------------------------------------------------------
// saving messages to file
class TStorageFile
{
public:
  int number;   // number to link from TMessProcess::SaveFile
  String file;  // file name
  TFile out;    // file write object
  //LogRotate

public:
  TStorageFile();
  TStorageFile(TStorageFile * p);
  void operator = (TStorageFile * p);
  // save data to file
  bool Save(AnsiString s);

  String GetDescription(void);
  String GetFileName(void);
  void SetFileName(String f);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);
};
//---------------------------------------------------------------------------
// file list
class TStorageFileList : public TList
{
public:
  __fastcall TStorageFileList();
  __fastcall virtual ~TStorageFileList(void);

  void __fastcall Add(TStorageFile * p);
  TStorageFile * __fastcall Get(int index);
  void __fastcall Del(int index);

  void Save(XMLNode * p);
  void Load(XMLNode * p);

  void operator = (TStorageFileList * p);
  // getall: true - return default StorageFile
  void GetList(TStrings * s, bool getall=false);

  TStorageFile * __fastcall GetByNumber(int _number);
  int GetNewNumber(void);

private:
  virtual void __fastcall Clear(void);
};
//---------------------------------------------------------------------------
#endif
