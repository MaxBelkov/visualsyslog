//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "utils.h"
#include "fdb.h"

const char * szStorageFiles = "files";
  const char * szStorageFile = "file";
extern String WorkDir;

bool WriteToLogError(String fmt, ...);
// sending a signal to main window: release file for rotation
void __fastcall PreRotateFile(String FileName);
void __fastcall PostRotateFile(void);

//---------------------------------------------------------------------------
TStorageFile::TStorageFile()
{
  number = 0;

  rotation_type = 0;
  rotation_size = 10;
  rotation_mult = 1;
  rotation_moment = 1;
  rotation_hour = 0;
  rotation_renaming = 0;
  rotation_count = 10;

  file_size = 0;
  last_date = 0;
}
//---------------------------------------------------------------------------
TStorageFile::TStorageFile(TStorageFile * p)
{
  *this = p;
}
//---------------------------------------------------------------------------
void TStorageFile::operator = (TStorageFile * p)
{
  number = p->number;
  file = p->file;

  rotation_type = p->rotation_type;
  rotation_size = p->rotation_size;
  rotation_mult = p->rotation_mult;
  rotation_moment = p->rotation_moment;
  rotation_hour = p->rotation_hour;
  rotation_renaming = p->rotation_renaming;
  rotation_name = p->rotation_name;
  rotation_count = p->rotation_count;

  file_size = p->file_size;
  last_date = p->last_date;
}
//---------------------------------------------------------------------------
bool TStorageFile::Save(AnsiString s)
{
  // check LogRotate...
  if( rotation_type == 1 ) // by size
  {
    int mult;
    switch( rotation_mult )
    {
      // 0,1,2 (KBs,MBs,GBs) [MBs]
      case 0: mult = 1024; break;
      case 1: mult = 1024*1024; break;
      case 2: mult = 1024*1024*1024; break;
      default: mult = 1024*1024; break;
    }
    if( file_size + s.Length() > rotation_size * mult )
    {
      RotateFile();
    }
  }

  if( ! out.IsOpen() )
  {
    out.Open(GetFileName(), GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                   OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL);
    if( ! out )
      return false;
    out.ToEnd();
    file_size = out.GetSize64();
  }
  out << s;
  file_size += out.Bytes;
  return ! out.GetError();
}
//---------------------------------------------------------------------------
String TStorageFile::GetFileNameToRotate(int num)
{
  String f = GetFileName();
  if( num == 0 )
  {
    return f;
  }
  else if( num > 0 && num <= rotation_count )
  {
    // file f have an extension ?
    String e = ExtractFileExt(f);
    if( e.Length() > 0 )
      f = ChangeFileExt(f, IntToStr(num) + e);
    else
      f = f + IntToStr(num);
    return f;
  }
  return String();
}
//---------------------------------------------------------------------------
String TStorageFile::GetNewRotationName(void)
{
  String f = rotation_name;
  try
  {
    for(int i=1, l=f.Length(); i<=l; i++)
    {
      if( f[i] == '{' )
      {
        // find '}'
        for(int j=i+1; j<=l; j++)
        {
          if( f[j] == '}' )
          {
            String mask = f.SubString(i+1, j-i-1);
            String r = FormatDateTime(mask, Now());
            f.Delete(i, j-i+1);
            f.Insert(r, i);
            l = f.Length();
            i += r.Length() - 1;
            break;
          }
        }
      }
    }
  }
  catch(...)
  {
    WriteToLogError("GetNewRotationName error");
  }
  return ExtractFilePath(GetFileName()) + f;
}
//---------------------------------------------------------------------------
bool TStorageFile::IsNeedRenameAfterRotate(void)
{
  return rotation_renaming==1;
}
//---------------------------------------------------------------------------
void TStorageFile::RotateFile(void)
{
  out.Close(); // close current writing file

  if( IsNeedRenameAfterRotate() )
  {
    // rename file to rotation_name

    String f = GetFileName();
    String n = GetNewRotationName();

    PreRotateFile(f); // close current viewed file
    PreRotateFile(n);

    if( ! ::MoveFileEx(f.c_str(), n.c_str(),
              MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) )
    {
      DWORD e = GetLastError();
      if( e!=2 ) // ignore error: "can't find file"
        WriteToLogError("ERROR\tRotate file from \"%s\" to \"%s\": %s [%lu]",
          f.c_str(), n.c_str(), FormatLastError2(e).c_str(), e);
    }
    else
    {
      String file_to_delete;
      RotationLogAddItem(n, file_to_delete);
      if( ! file_to_delete.IsEmpty() )
        if( ! DeleteFile(file_to_delete) )
        {
          DWORD e = GetLastError();
          WriteToLogError("ERROR\tDelete old rotated file \"%s\": %s [%lu]",
            file_to_delete.c_str(), FormatLastError2(e).c_str(), e);
        }
    }

    PostRotateFile(); // open current viewed file if been closed
  }
  else
  {
    // rename file to file+number[rotation_count]
    
    // rotation log open
    TStringList * p = new TStringList;
    String rlf = RotationLogFormatFileName();

    for(int i=rotation_count-1; i>=0; i--)
    {
      String f = GetFileNameToRotate(i);
      String n = GetFileNameToRotate(i+1);

      PreRotateFile(f); // close current viewed file
      PreRotateFile(n);

      if( ! ::MoveFileEx(f.c_str(), n.c_str(),
                MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) )
      {
        DWORD e = GetLastError();
        if( e!=2 ) // ignore error: "can't find file"
          WriteToLogError("ERROR\tRotate file from \"%s\" to \"%s\": %s [%lu]",
            f.c_str(), n.c_str(), FormatLastError2(e).c_str(), e);
      }
      else
      {
        // do not use RotationLogAddItem() due to the acceleration of work:
        // if the rotation_count==100, the file .rl is written 100 times !
        //RotationLogAddItem(n);

        // rotation log add
        try
        {
          p->Insert(0, n);
          //if( p->Count > rotation_count )
          //  p->Delete(p->Count - 1);
        }
        catch(...)
        {}
      }

      PostRotateFile(); // open current viewed file if been closed
    }

    // rotation log close
    try { p->SaveToFile(rlf); }
    catch(const Exception & E)
    { WriteToLogError("ERROR\tRotationLogAddItem2(%s): %s", rlf.c_str(), E.Message.c_str()); }
    catch(...) {}
    delete p;
  }
}
//---------------------------------------------------------------------------
bool TStorageFile::IsRotationEnable(void)
{
  return rotation_type > 0;
}
//---------------------------------------------------------------------------
String TStorageFile::GetFileName(void)
{
  // if user specify the file name without path: WorkDir will be added
  if( file.Length() > 0 && ExtractFilePath(file).Length() == 0 )
    return WorkDir + file;
  // full file name  
  return file;
}
//---------------------------------------------------------------------------
void TStorageFile::SetFileName(String f)
{
  if( MySameText(ExtractFilePath(f), WorkDir) )
    file = ExtractFileName(f);
  else
    file = f;
}
//---------------------------------------------------------------------------
String TStorageFile::GetDescription(void)
{
  return file;
}
//---------------------------------------------------------------------------
String TStorageFile::GetRotationDescription(void)
{
  String s;
  String m;
  switch( rotation_type )
  {
    case 0: s = "off"; break;
    case 1:
      switch( rotation_mult )
      {
        case 0: m = "KB"; break;
        case 1: m = "MB"; break;
        case 2: m = "GB"; break;
      }
      s = String("by size ") + IntToStr(rotation_size) + " " + m;
    break;
    case 2:
      switch( rotation_moment )
      {
        case 0: m = "day"; break;
        case 1: m = "week"; break;
        case 2: m = "month"; break;
        case 3: m = "year"; break;
      }
      s = String("by ") + m + " " + IntToStr(rotation_hour) + " hour";
    break;
  }
  if( rotation_type > 0 )
    switch( rotation_renaming )
    {
      case 0: s += String(", ") + ExtractFileName(file) + "[1.." +
                   IntToStr(rotation_count) + "]"; break;
      case 1: s += String(", ") + rotation_name +
                   " [" + IntToStr(rotation_count) + "]"; break;
    }
  return s;
}
//---------------------------------------------------------------------------
void TStorageFile::Save(XMLElementEx * p)
{
  p->ws("file", file);
  p->wi("number", number);

  p->wi("rtype", rotation_type);
  p->wi("rsize", rotation_size);
  p->wi("rmult", rotation_mult);
  p->wi("rmoment", rotation_moment);
  p->wi("rhour", rotation_hour);
  p->wi("rrenaming", rotation_renaming);
  p->ws("rname", rotation_name);
  p->wi("rcount", rotation_count);
}
//---------------------------------------------------------------------------
void TStorageFile::Load(XMLElementEx * p)
{
  file = p->rs("file");
  number = p->ri("number");

  rotation_type = p->ri("rtype", 0);
  rotation_size = p->ri("rsize", 10);
  rotation_mult = p->ri("rmult", 1);
  rotation_moment = p->ri("rmoment", 1);
  rotation_hour = p->ri("rhour", 0);
  rotation_renaming = p->ri("rrenaming", 0);
  rotation_name = p->rs("rname");
  rotation_count = p->ri("rcount", 10);
}
//---------------------------------------------------------------------------
String TStorageFile::RotationLogFormatFileName(void)
{
  return WorkDir + ExtractFileName(file) + ".rl";
}
//---------------------------------------------------------------------------
void TStorageFile::RotationLogAddItem(String fn, String & file_to_delete)
{
  TStringList * p = NULL;
  String rlf = RotationLogFormatFileName();
  try
  {
    p = new TStringList;
    try { p->LoadFromFile(rlf); } catch(...) {}
    p->Insert(0, fn);
    // delete last item if limit of files exceeded
    if( rotation_count > 0 )
      if( p->Count > rotation_count )
      {
        file_to_delete = p->Strings[p->Count - 1];
        p->Delete(p->Count - 1);
      }
    p->SaveToFile(rlf);
  }
  catch(const Exception & E)
  {
    WriteToLogError("ERROR\tRotationLogAddItem(%s): %s", rlf.c_str(), E.Message.c_str());
  }
  catch(...)
  {}
  delete p;
}
//---------------------------------------------------------------------------
String TStorageFile::RotationLogGetItem(int num)
{
  // 0 - current file
  if( num==0 )
    return GetFileName();

  String rv;
  TStringList * p = NULL;
  try
  {
    p = new TStringList;
    p->LoadFromFile( RotationLogFormatFileName() );
    if( num > 0 && num <= p->Count )
      rv = p->Strings[num-1];
  }
  catch(...)
  {}
  delete p;
  return rv;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TStorageFileList::TStorageFileList()
{
}
//---------------------------------------------------------------------------
__fastcall TStorageFileList::~TStorageFileList(void)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall TStorageFileList::Clear(void)
{
  for(int i=0; i<Count; i++)
    delete (TStorageFile *)Items[i];
  TList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall TStorageFileList::Add(TStorageFile * p)
{
  TList::Add( p );
}
//---------------------------------------------------------------------------
TStorageFile * __fastcall TStorageFileList::Get(int index)
{
  if( index >= 0 && index < Count )
    return (TStorageFile *)Items[index];
  return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TStorageFileList::Del(int index)
{
  if( index >= 0 && index < Count )
  {
    delete (TStorageFile *)Items[index];
    TList::Delete(index);
  }
}
//---------------------------------------------------------------------------
void TStorageFileList::Save(XMLNode * p)
{
  tinyxml2::XMLDocument * doc = p->GetDocument();
  XMLNode * hls = p->InsertEndChild( doc->NewElement(szStorageFiles) );
  XMLElement * e;
  for(int i=0; i<Count; i++)
  {
    e = doc->NewElement(szStorageFile);
    hls->InsertEndChild(e);
    Get(i)->Save((XMLElementEx *)e);
  }
}
//---------------------------------------------------------------------------
void TStorageFileList::Load(XMLNode * p)
{
  Clear();
  TStorageFile * sf;
  for(const XMLElement * pr = p->FirstChildElement(szStorageFile);
      pr;
      pr = pr->NextSiblingElement(szStorageFile))
  {
    sf = new TStorageFile;
    sf->Load((XMLElementEx *)pr);
    Add(sf);
  }
}
//---------------------------------------------------------------------------
void TStorageFileList::operator = (TStorageFileList * p)
{
  Clear();
  for(int i=0; i<p->Count; i++)
  {
    Add( new TStorageFile(p->Get(i)) );
  }
}
//---------------------------------------------------------------------------
void TStorageFileList::GetList(TStrings * s, bool getall)
{
  s->BeginUpdate();
  s->Clear();
  TStorageFile * p;
  for(int i=getall?0:1; i<Count; i++)
  {
    p = Get(i);
    s->AddObject(p->GetDescription(), (TObject *)p->number);
  }
  s->EndUpdate();
}
//---------------------------------------------------------------------------
int TStorageFileList::GetNewNumber(void)
{
  int rv = 0;
  for(int i=0; i<Count; i++)
    rv = MAX(Get(i)->number, rv);
  return rv + 1;
}
//---------------------------------------------------------------------------
TStorageFile * __fastcall TStorageFileList::GetByNumber(int _number)
{
  TStorageFile * p;
  for(int i=0; i<Count; i++)
  {
    p = Get(i);
    if( p->number == _number )
      return p;
  }
  return NULL;
}
//---------------------------------------------------------------------------
void TStorageFileList::CheckRotate(void)
{
  try
  {
    TDateTime t = Now();
    unsigned short year, month, day, hour, min, sec, msec;
    t.DecodeDate(&year, &month, &day);
    t.DecodeTime(&hour, &min, &sec, &msec);

    TStorageFile * p;
    for(int i=0; i<Count; i++)
    {
      p = Get(i);

      if( p->rotation_type != 2 ) // by date
        continue;
      if( p->last_date == 0 )
      {
        p->last_date = (int)t; // today
        if( hour < p->rotation_hour )
          p->last_date--; // perform rotation today
        continue;
      }
      if( (int)t > p->last_date )
      {
        // now is next (or another) day

        switch( p->rotation_moment )
        {
          case 0: // Day
          break;
          case 1: // Week
            if( t.DayOfWeek() != 2 ) // not Monday
            {
              p->last_date = (int)t;
              continue;
            }
          break;
          case 2: // Month
            if( day != 1 ) // not first day of monyh
            {
              p->last_date = (int)t;
              continue;
            }
          break;
          case 3: // Year
            if( month != 1 && day != 1 ) // not first day of year
            {
              p->last_date = (int)t;
              continue;
            }
          break;
          default:
            continue;
        }

        if( hour >= p->rotation_hour )
        {
          p->RotateFile();
          p->last_date = (int)t;
        }
      }
    }
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
