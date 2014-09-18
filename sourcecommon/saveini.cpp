//---------------------------------------------------------------------------
#pragma hdrstop

#include "saveini.h"

//---------------------------------------------------------------------------
TSaveParamsINI::TSaveParamsINI(AnsiString inFile, bool inCreateNewFile) :
                TSaveParams(inFile, inCreateNewFile)
{
  f = NULL;
  Load(inFile);
}
//---------------------------------------------------------------------------
__fastcall TSaveParamsINI::~TSaveParamsINI()
{
  try
  {
    delete f;
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
void TSaveParamsINI::Set(AnsiString Section, AnsiString name, Variant value)
{
  try
  {
    if( !f )
      Load(fFile);
    if( f )
    {
      AnsiString v = value;
      f->WriteStringNOCR(Section.c_str(), name.c_str(), v.c_str());
    }
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------
Variant TSaveParamsINI::Get(AnsiString Section, AnsiString name, Variant defval)
{
  Variant rv;
  try
  {
    if( !f )
      Load(fFile);
    if( f )
    {
      AnsiString v = defval;
      v = f->ReadStringNOCR(Section.c_str(), name.c_str(), v.c_str());
      // Вместо пустой строки возвращаем Empty вариант тип
      if( v.Length()>0 )
        rv = v;
    }
    /* Слишком долго: TFile::Find вызывается дважды !
    if( f->ValueExists(Section.c_str(), name.c_str()) )
      rv = f->ReadStringNOCR(Section.c_str(), name.c_str());
    else
      rv = defval;
    */
  }
  catch(...)
  {}
  return rv;
}
//---------------------------------------------------------------------------
bool TSaveParamsINI::SectionExists(const AnsiString Section)
{
  if( !f )
    Load(fFile);
  if( f )
    return f->SectionExists(Section.c_str());
  else
    return false;
}
//---------------------------------------------------------------------------
bool TSaveParamsINI::ValueExists(const AnsiString Section, const AnsiString name)
{
  if( !f )
    Load(fFile);
  if( f )
    return f->ValueExists(Section.c_str(), name.c_str());
  else
    return false;
}
//---------------------------------------------------------------------------
bool TSaveParamsINI::Load(AnsiString inFile)
{
  bool rv = false;
  try
  {
    if( f )
    {
      // Если есть изменения в файле - запишем его
      Save();
      delete f;
      f = NULL;
    }
    fFile = inFile;
    f = new TMYIniFile(inFile.c_str(), true, fCreateNewFile);
    rv = true;
  }
  catch(...)
  {}
  return rv;
}
//---------------------------------------------------------------------------
bool TSaveParamsINI::Save(void)
{
  if( !f )
    return false;
  bool rv = true;
  try
  {
    if( f->Change )
      rv = f->UpdateFile();
  }
  catch(...)
  { rv = false; }
  return rv;
}
//---------------------------------------------------------------------------
void __fastcall TSaveParamsINI::SetFile(AnsiString inFile)
{
  Load(inFile);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
