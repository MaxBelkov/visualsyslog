//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "utils.h"
#include "fdb.h"

const char * szStorageFiles = "files";
  const char * szStorageFile = "file";
extern String WorkDir;

//---------------------------------------------------------------------------
TStorageFile::TStorageFile()
{
  number = 0;
}
//---------------------------------------------------------------------------
TStorageFile::TStorageFile(TStorageFile * p)
{
  *this = p;
}
//---------------------------------------------------------------------------
void TStorageFile::operator = (TStorageFile * p)
{
  file = p->file;
  number = p->number;
}
//---------------------------------------------------------------------------
bool TStorageFile::Save(AnsiString s)
{
  if( ! out.IsOpen() )
  {
    out.Open(GetFileName(), GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                   OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL);
    if( ! out )
      return false;
    out.ToEnd();
  }
  out << s;
  return ! out.GetError();
}
//---------------------------------------------------------------------------
String TStorageFile::GetFileName(void)
{
  if( file.Length() > 0 && ExtractFilePath(file).Length() == 0 )
    return WorkDir + file;
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
void TStorageFile::Save(XMLElementEx * p)
{
  p->ws("file", file);
  p->wi("number", number);
}
//---------------------------------------------------------------------------
void TStorageFile::Load(XMLElementEx * p)
{
  file = p->rs("file");
  number = p->ri("number");
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
#pragma package(smart_init)
