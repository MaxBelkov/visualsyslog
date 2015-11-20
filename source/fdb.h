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
  int number;   // file number to link from TMessProcess::SaveFile
  String file;  // log file name (if user specify the file name without path:
                // WorkDir will be added)

  // *** LogRotate ***
  int rotation_type;   // file rotation: 0-disable, 1-by size, 2-by date [0]
  // rotation by size
  int rotation_size;   // > 0 [10]
  int rotation_mult;   // 0,1,2 (KBs,MBs,GBs) [MBs]
  // rotation by date
  int rotation_moment; // 0,1,2,3 (Day,Week,Month,Year) [Week]
  int rotation_hour;   // 0..23 [0]
  // file name after rotation
  int rotation_renaming; // 0-file+number[rotation_count] 1-rename file to rotation_name
  String rotation_name;  // new file name (without path!) after rotation {dd-mm-yyyy hh}
  int rotation_count;    // total number of rotated files >= 1 [10]

private:
  TFile out;    // file write object

public:
  ULONGLONG file_size; // for log rotatition by size
  int last_date; // for log rotatition by date

public:
  TStorageFile();
  TStorageFile(TStorageFile * p);
  void operator = (TStorageFile * p);
  // save data to file
  bool Save(AnsiString s);

  String GetDescription(void);
  // get log file name with full path
  String GetFileName(void);
  void SetFileName(String f);

  void Save(XMLElementEx * p);
  void Load(XMLElementEx * p);

  // *** LogRotate ***
  void RotateFile(void);

  String GetFileNameToRotate(int num); // for rotation_renaming==0
  String GetNewRotationName(void);     // for rotation_renaming==1

  bool IsRotationEnable(void);
  bool IsNeedRenameAfterRotate(void);

  String GetRotationDescription(void);

  // *** Rotation names log ***
  // (It used to view file with buttons "View prev" "view next")
  String RotationLogFormatFileName(void);
  // fn - log file name
  void RotationLogAddItem(String fn, String & file_to_delete);
  // num: 0-GetFileName() >=1-read rotation name from file
  String RotationLogGetItem(int num);
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

  void CheckRotate(void);

private:
  virtual void __fastcall Clear(void);
};
//---------------------------------------------------------------------------
#endif
