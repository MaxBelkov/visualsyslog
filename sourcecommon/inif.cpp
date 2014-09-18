//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>

#include "utils.h"
#include "inif.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TMYIniFile::TMYIniFile(char * File, bool enableExceptions, bool bCreateNewFile) :
						TFile(enableExceptions),
						FileName(File)
{
	list = NULL;
	BYTE * p = NULL;
	try
	{
		IniError = 0;
        bAnyError = false;
		bChange = false;
		CurSectionIndex = -1;
        CurParamIndex = -1;
		list = new TStringList();

		if( ! bCreateNewFile )
		{
			// Откроем файл для чтения
			if( ! Open(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN) )
			{
                // Файла нет - ошибку не даем, может мы будем его потом записывать...
				//IniError = 1;
                bAnyError = true;
				return;
			}

			// Считаем содержимое файла
			DWORD l = GetSize();
			if( l > 0 )
			{
				p = new BYTE[l];
				AnsiString s;
				if( ! Read(p, l) )
                {
					IniError = 1;
                    bAnyError = true;
                }
				else
					for(DWORD i=0; i<l; i++)
					{
						if( p[i]=='\r' || p[i]=='\n' || !p[i] )
						{
							s = s.TrimLeft();
							if( s.Length()>0 )
							{
								list->Add(s);
								s = "";
							}
						}
						else
							s += (char)p[i];
					}
				delete [] p;
				p = NULL;
			}
		}
	}
	catch(...)
	{
        IniError = 1;
        bAnyError = true;
		delete [] p;
	}
    // Закрываем считанный файл
    Close();
}
//---------------------------------------------------------------------------
TMYIniFile::~TMYIniFile()
{
  try
  {
    if( bChange )
      UpdateFile();
  }
  catch(...)
  {
    IniError = 1;
    bAnyError = true;
  }
  delete list;
  list = NULL;
}
//---------------------------------------------------------------------------
bool TMYIniFile::UpdateFile(void)
{
  // Откроем файл для записи
  if( ! Open(FileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS,
             FILE_ATTRIBUTE_NORMAL) )
  {
    IniError = 1;
    bAnyError = true;
    return false;
  }

  for(int i=0; i<list->Count; i++)
	TFile::WriteString(AnsiString(list->Strings[i]).c_str());
  Close();
  IniError = 0;
  bAnyError = false;
  bChange = false;
  return true;
}
//---------------------------------------------------------------------------
bool TMYIniFile::IsSectionName(String Section)
{
  String s = Section.TrimLeft();
  if( s.Length() > 3 )
	if( s[1]=='[' )
	  return true;
  return false;
  //return *SkipStartCntlChars(Section)=='[';
}
//---------------------------------------------------------------------------
// В строке s заменяем "\" на "\r\n", а "\\" на "\"
AnsiString TMYIniFile::SpecialReplace(AnsiString s)
{
  AnsiString result;
  for(int i=1,l=s.Length(); i<=l; i++)
  {
    if( s[i]=='\\' )
    {
      if( (i+1)<=l && s[i+1]=='\\' ) { result += '\\'; i++; }
      else result += "\r\n";
    }
    else
      result += s[i];
  }
  return result;
}
//---------------------------------------------------------------------------
// Заменяем "\r\n" на "\", а "\" на "\\"
AnsiString TMYIniFile::SpecialReverseReplace(AnsiString s)
{
  AnsiString result;
  for(int i=1,l=s.Length(); i<=l; i++)
  {
    if( s[i]=='\r' && (i+1)<=l && s[i+1]=='\n' )
    {
      result += '\\';
      i++;
      // Проверим что не идут подряд два ентера - разделим их пробелом
      if( (i+2)<=l && s[i+1]=='\r' && s[i+2]=='\n' )
        result += ' ';
    }
    else if( s[i]=='\\' )
      result += "\\\\";
    else
      result += s[i];
  }
  return result;
}
//---------------------------------------------------------------------------
int TMYIniFile::Find(String Section, String Ident, int from)
{
  if( Section.Length() == 0 )
    return -1;

  // Секция та-же ?
  if( MySameText(CurSection, Section) && from==0 )
  {
    // Да - текущая секция CurSectionIndex
	if( Ident.Length() > 0 )
    {
	  if( CurParamIndex<0 )
        CurParamIndex = CurSectionIndex + 1; // Предполагаем, что требуется следующий параметр
      else
        CurParamIndex++; // Предполагаем, что требуется следующий параметр
    }
  }
  else
  {
	CurSection = "";
    CurSectionIndex = -1;
    CurParamIndex = -1;
	// Нет: ищем секцию...
    for(int i=from; i<list->Count; i++)
    {
	  if( ! IsSectionName(list->Strings[i]) ) continue;
	  if( MySameText( String("[" + Section + "]"), list->Strings[i]) )
	  {
		CurSection = Section;
		CurSectionIndex = i;
		if( Ident.Length() > 0 )
          CurParamIndex = CurSectionIndex + 1; // Предполагаем, что требуется следующий параметр
        break;
      }
    }
  }
  // Секция не найдена или не надо искать параметр
  if( CurSectionIndex<0 || Ident.Length() == 0 )
	return CurSectionIndex;

  // Ищем название переменной... (если это требуется)
  if( Ident.Length() > 0 )
  {
    // Попробуем...
    if( CurParamIndex>=0 && CurParamIndex<list->Count )
	  if( MySameText(Ident, list->Names[CurParamIndex]) ) // ==
        return CurParamIndex;

    CurParamIndex = -1;
    for(int i=CurSectionIndex+1; i<list->Count; i++)
    {
      // Если следующая секция - значит не нашли переменную:
	  if( IsSectionName(list->Strings[i]) )
		break;

	  if( MySameText(Ident, list->Names[i]) ) // ==
      {
        CurParamIndex = i;
        break;
      }
	}
  }
  return CurParamIndex;
}
//---------------------------------------------------------------------------
AnsiString TMYIniFile::ReadString(char * Section, char * Ident, char * def)
{
  return ReadString(String(Section), String(Ident), def);
}
//---------------------------------------------------------------------------
String TMYIniFile::ReadString(String Section, String Ident, char * def)
{
  int i = Find(Section.c_str(), Ident.c_str());
  if( i<0 )
		return String(def);

  String s = list->Strings[i];
  i = s.Pos("=");
  if( i==0 ) // = not found
		return String(def);
	// found
	return s.Delete(1, i).TrimLeft();
}
//---------------------------------------------------------------------------
AnsiString TMYIniFile::ReadStringNOCR(char * Section, char * Ident, char * def)
{
  return SpecialReplace(ReadString(Section, Ident, def));
}
//---------------------------------------------------------------------------
int TMYIniFile::ReadInteger(char * Section, char * Ident, int def)
{
  return ReadInteger(String(Section), String(Ident), def);
}
//---------------------------------------------------------------------------
int TMYIniFile::ReadInteger(String Section, String Ident, int def)
{
  AnsiString s(def);
  AnsiString result = ReadString(Section, Ident, s.c_str());
  return atoi(result.c_str());
}
//---------------------------------------------------------------------------
bool TMYIniFile::ReadBool(char * Section, char * Ident, bool def)
{
  return ReadBool(String(Section), String(Ident), def);
}
//---------------------------------------------------------------------------
bool TMYIniFile::ReadBool(String Section, String Ident, bool def)
{
  AnsiString s(def?"1":"0");
  AnsiString result = ReadString(Section, Ident, s.c_str());
  return (bool)atoi(result.c_str());
}
//---------------------------------------------------------------------------
double TMYIniFile::ReadFloat(char * Section, char * Ident, double def)
{
  return ReadFloat(String(Section), String(Ident), def);
}
//---------------------------------------------------------------------------
double TMYIniFile::ReadFloat(String Section, String Ident, double def)
{
  AnsiString s(def);
  AnsiString result = ReadString(Section, Ident, s.c_str());
  // После  setlocale( LC_NUMERIC, "Russian" ); перестал работать atof и sscanf
  return atof(result.c_str());
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteString(char * Section, char * Ident, char * Value)
{
  WriteString(String(Section), String(Ident), Value);
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteString(String Section, String Ident, String Value)
{
  AnsiString str2 = AnsiString(Ident) + "=" + Value;

  // Есть такая секция ?
  int si, ii;
  si = Find(Section.c_str());
  if( si>=0 ) // Да
  {
    // Есть такая переменная ?
    ii = Find(Section.c_str(), Ident.c_str());
    if( ii>=0 ) // Да
    {
      // Изменим переменную
      list->Strings[ii] = str2;
    }
    else // Нет
    {
      // Добавим переменную
      while( ++si < list->Count )
		if( IsSectionName(list->Strings[si]) )
		  break;
      list->Insert(si, str2);
    }
  }
  else // Нет
  {
    //list->Add("");
	// Добавим секцию
    list->Add(AnsiString("[") + Section + "]");
    // Добавим переменную
    list->Add(str2);
  }
  bChange = true;
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteStringNOCR(char * Section, char * Ident, char * Value)
{
  AnsiString v = SpecialReverseReplace(Value);
  WriteString(Section, Ident, v.c_str());
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteInteger(char * Section, char * Ident, int Value)
{
  WriteInteger(String(Section), String(Ident), Value);
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteInteger(String Section, String Ident, int Value)
{
  WriteString(Section, Ident, IntToStr(Value));
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteBool(char * Section, char * Ident, bool Value)
{
  WriteBool(String(Section), String(Ident), Value);
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteBool(String Section, String Ident, bool Value)
{
  WriteString(Section, Ident, IntToStr((int)Value));
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteFloat(char * Section, char * Ident, double Value)
{
  WriteFloat(String(Section), String(Ident), Value);
}
//---------------------------------------------------------------------------
void TMYIniFile::WriteFloat(String Section, String Ident, double Value)
{
  char str[40];
  sprintf(str, "%g", Value);
  WriteString(Section, Ident, str);
}
//---------------------------------------------------------------------------
bool TMYIniFile::SectionExists(const AnsiString Section)
{
  return Find(Section.c_str()) >= 0;
}
//---------------------------------------------------------------------------
bool TMYIniFile::ValueExists(const AnsiString Section, const AnsiString Ident)
{
  return Find(Section.c_str(), Ident.c_str()) >= 0;
}
//---------------------------------------------------------------------------
/*
char * SkipStartCntlChars(char * str)
{
  if( !str ) return NULL;
  while( *str && (*str==' '||*str=='\t'||*str=='\n'||*str=='\r') ) str++;
  return str;
}
*/
//---------------------------------------------------------------------------
#pragma package(smart_init)
