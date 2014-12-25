//---------------------------------------------------------------------------
//#include <shlobj.h>
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#pragma hdrstop

#define NO_WIN32_LEAN_AND_MEAN

#include "file.h"
#include "inif.h"
#include "utils.h"

//---------------------------------------------------------------------------
void ProcessString(char * s, char comment, char * del)
{
  bool DelSpace = true;
  int i, j;
  for(i=0,j=0; s[i]; i++)
  {
    if( s[i]=='\n' || s[i]=='\r' || s[i]==comment )
    {
      break;
    }
    else if( s[i]=='\"' )
    {
      DelSpace = !DelSpace;
      //continue;
    }
    else if( s[i]==' ' || s[i]=='\t' )
    {
      if( DelSpace ) continue;
    }

    // Удалим символы, перечисленные в строке del
    if( del )
      if( strchr(del, s[i]) )
        continue;

    s[j++] = s[i];
  }
  s[j] = 0;
}
//---------------------------------------------------------------------------
// Удалим символы, перечисленные в строке del
void DeleteCharactersFromString(char * s, char * del)
{
  if( !s || !del ) return;
  int i, j;
  for(i=0,j=0; s[i]; i++)
  {
    if( strchr(del, s[i]) ) continue;
    // Символ s[i] в строке del не найден - оставим его
    s[j++] = s[i];
  }
  s[j] = 0;
}
//---------------------------------------------------------------------------
// Заменить в строке s все символя, указанные в строке source на символ dest
void ReplaceCharactersFromString(char * s, char * source, char dest)
{
  if( !s || !source ) return;
  for(int i=0; s[i]; i++)
    if( strchr(source, s[i]) )
      s[i] = dest;
}
//---------------------------------------------------------------------------
void ReplaceCharacters(AnsiString & s, char from, char to)
{
  for(int i=1, l=s.Length(); i<=l; i++)
    if( s[i]==from )
	  s[i]=to;
}
//---------------------------------------------------------------------------
// Это полный путь к файлу ?
bool IsFullPath(char * szName)
{
  if( !szName )
    return false;
  int l = strlen(szName);
  if( l>3 )
  {
    if( szName[0]=='\\' && szName[1]=='\\' )
      return true;
    if( szName[1]==':' && szName[2]=='\\' )
      return true;
  }
  return false;
}
//---------------------------------------------------------------------------
// Это файловая маска ? (содержит ? или *)
bool IsFileMask(char * szName)
{
  if( !szName )
    return false;
  int l = strlen(szName);
  for(int i=0; i<l; i++)
    if( szName[i]=='?' || szName[i]=='*' )
      return true;
  return false;
}
//---------------------------------------------------------------------------
void DeleteBadCharsFromFileName(String & name)
{
  // Уберем из ShortcutName, запрещенные в именах файлов (mask)
  String mask(__UTEXT("*:?/\\<\">|"));
  for(int i=1,l=mask.Length(),pos; i<=l; i++)
    while( (pos = name.Pos(mask[i])) > 0 )
      name.Delete(pos, 1);
}
//---------------------------------------------------------------------------
int strcmpfixedlen(char * str1, char * str2)
{
  int i = 0;
  while( str1[i] && str2[i] )
  {
	if( toupper(str1[i]) != toupper(str2[i]) )
	  return 0; // Глюк - надо писать break;
	i++;
  }
  return i;
}
//---------------------------------------------------------------------------
/*
int strcmpfixedlen(String str1, String str2)
{
  int l = MIN(str1.Length(), str2.Length());
  str1 = str1.UpperCase();
  str2 = str2.UpperCase();
  int i = 0;
  while( i < l )
  {
	if( str1[i+1] != str2[i+1] )
	  break;
	i++;
  }
  return i;
}
*/
//---------------------------------------------------------------------------
int SkipSpace(char * buf)
{
  int i = 0;
  while( buf[i]==' ' || buf[i]=='\t' ) i++;
  return i;
}
//---------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TMyFont::Get(TFont * font)
{
  Color  = font->Color;
  Height = font->Height;
  strncpy(Name, ((AnsiString)font->Name).c_str(), 31);
  Name[31] = NULL;
  Pitch = font->Pitch;
  PixelsPerInch = font->PixelsPerInch;
  Size = font->Size;

  Style = 0;
  if( font->Style.Contains(fsBold) )      Style |= 1;
  if( font->Style.Contains(fsItalic) )    Style |= 2;
  if( font->Style.Contains(fsUnderline) ) Style |= 4;
  if( font->Style.Contains(fsStrikeOut) ) Style |= 8;
}
//---------------------------------------------------------------
void TMyFont::Set(TFont * font)
{
  if( font->Color != Color )
    font->Color  = Color;
  if( font->Height != Height )
    font->Height = Height;
  if( font->Name != Name )
    font->Name = Name;
  if( font->Pitch != Pitch )
    font->Pitch = Pitch;
  if( PixelsPerInch==0 ) PixelsPerInch = 120; // ?
  if( font->PixelsPerInch != PixelsPerInch )
    font->PixelsPerInch = PixelsPerInch;
  if( font->Size != Size )
  font->Size = Size;

  TFontStyles st;
  if( Style & 1 ) st << fsBold;
  if( Style & 2 ) st << fsItalic;
  if( Style & 4 ) st << fsUnderline;
  if( Style & 8 ) st << fsStrikeOut;
  if( font->Style != st )
    font->Style = st;
}
//---------------------------------------------------------------------------
bool TMyFont::Load(TMYIniFile * f, char * Section)
{
  bool bRetVal = true;
  try
  {
    strncpy(Name, f->ReadString(Section, "FontName").c_str(), 31);
    Name[31] = NULL;
    if( strlen(Name)<=0 )
    {
      strcpy(Name, "Tahoma");
      bRetVal = false;
    }
    Height = f->ReadInteger(Section, "FontHeight", -13);
    Pitch = (TFontPitch)f->ReadInteger(Section, "FontPitch", 0);
    PixelsPerInch = f->ReadInteger(Section, "FontPixelsPerInch", 120);
    Size = f->ReadInteger(Section, "FontSize", 8);
    Style = (BYTE)f->ReadInteger(Section, "FontStyle", 0);
    Color = (TColor)f->ReadInteger(Section, "FontColor", clBlack);
  }
  catch(...) { bRetVal = false; }
  return bRetVal;
}
//---------------------------------------------------------------------------
bool TMyFont::Save(TMYIniFile * f, char * Section)
{
  bool bRetVal = true;
  try
  {
    f->WriteString(Section, "FontName", Name);
    f->WriteInteger(Section, "FontHeight", Height);
    f->WriteInteger(Section, "FontPitch", Pitch);
    f->WriteInteger(Section, "FontPixelsPerInch", PixelsPerInch);
    f->WriteInteger(Section, "FontSize", Size);
    f->WriteInteger(Section, "FontStyle", Style);
    f->WriteInteger(Section, "FontColor", Color);
  }
  catch(...) { bRetVal = false; }
  return bRetVal;
}
//---------------------------------------------------------------------------
bool DeleteDirectory(char * szPath, bool bDelTopDir)
{
  int t = strlen(szPath);
  if( t<3 ) return false;
  if( szPath[t-1]!='\\' ) strcat(szPath, "\\");

  // Найдем все файлы *.* в директории szPath
  HANDLE handle;
  WIN32_FIND_DATA FD;
  char start[MN];
  wsprintf(start, "%s*.*", szPath);
  bool bRetVal = true;

  handle = FindFirstFile(start, &FD);
  if( handle != INVALID_HANDLE_VALUE )
  do
  {
    wsprintf(start, "%s%s", szPath, FD.cFileName);
    if( FD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
    {
      // !="." && !=".."
      if( strcmpi(FD.cFileName, ".") && strcmpi(FD.cFileName, "..") )
      {
        if( !DeleteDirectory(start) )
          bRetVal = false;
      }
    }
    else
    {
      if( !::DeleteFile(start) )
      {
        // Если установлен атрибут "только для чтения" - снимем его и попробуем снова
        DWORD a = ::GetFileAttributes(start);
        if( a==0xFFFFFFFF )
          bRetVal = false;
        else
        {
          if( a & FILE_ATTRIBUTE_READONLY )
          {
            if( ::SetFileAttributes(start, a^FILE_ATTRIBUTE_READONLY) )
            {
              if( !::DeleteFile(start) )
                bRetVal = false;
            }
            else
              bRetVal = false;
          }
          else
            bRetVal = false;
        }
      }
    }
  } while( FindNextFile(handle, &FD) );
  FindClose(handle);

  if( bDelTopDir )
  {
    Sleep(500); // После удаления всех файлов директория почему-то не удаляется в XP !
    // В Vista & 7 она вообще никогда не удаляется !
    if( !::RemoveDirectory(szPath) )
      bRetVal = false;
  }

  return bRetVal;
}
//---------------------------------------------------------------------------
// Копирует файлы из папки szFrom по маске szMask в папку szTo со всему поддиректориями
bool CopyDirectory(char * szFrom, char * szMask, char * szTo)
{
  bool bRetVal = true;
  HANDLE handle;
  WIN32_FIND_DATA FD;
  //
  char sfile[MN];
  char dfile[MN];
  wsprintf(sfile, "%s%s", szFrom, szMask);
  //
  handle = FindFirstFile(sfile, &FD);
  //
  if( handle != INVALID_HANDLE_VALUE )
  do
  {
    wsprintf(sfile, "%s%s", szFrom, FD.cFileName);
    wsprintf(dfile, "%s%s", szTo, FD.cFileName);
    //
    if( FD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
    {
      // !="." && !=".."
      if( strcmpi(FD.cFileName, ".") && strcmpi(FD.cFileName, "..") )
      {
        if( ::GetFileAttributes(dfile)==0xFFFFFFFF ) ::CreateDirectory(dfile, NULL);
        TerminateDirectory(sfile);
        TerminateDirectory(dfile);
        if( !CopyDirectory(sfile, szMask, dfile) ) bRetVal = false;
      }
    }
    else
    {
      if( !::CopyFile(sfile, dfile, FALSE)  ) bRetVal = false;
    }
    //
  } while( FindNextFile(handle, &FD) );
  FindClose(handle);
  return bRetVal;
}
//---------------------------------------------------------------------------
#if !defined(DANFOSS)
void ReportError(char *fmt, ...)
{
#ifndef SYSTEMSERVICE
  char str[1000];
  va_list argptr;
  va_start(argptr, fmt);
  wvsprintf(str, fmt, argptr);
  va_end(argptr);
  Application->MessageBox(String(str).c_str(), Application->Title.c_str(), MB_OK|MB_ICONERROR);
#endif
}
//---------------------------------------------------------------------------
void ReportMess(char *fmt, ...)
{
#ifndef SYSTEMSERVICE
  char str[1000];
  va_list argptr;
  va_start(argptr, fmt);
  wvsprintf(str, fmt, argptr);
  va_end(argptr);
  Application->MessageBox(String(str).c_str(), Application->Title.c_str(), MB_OK);
#endif
}
//---------------------------------------------------------------------------
bool ReportYesNo(char *fmt, ...)
{
#ifndef SYSTEMSERVICE
  char str[1000];
  va_list argptr;
  va_start(argptr, fmt);
  wvsprintf(str, fmt, argptr);
  va_end(argptr);
  return Application->MessageBox(String(str).c_str(), Application->Title.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES;
#else
  return false;
#endif
}
#endif
//---------------------------------------------------------------------------
void TerminateDirectory(char * szDir)
{
  if( ! szDir ) return;
  int i = lstrlen( szDir );
  if( i < 1 ) return;
  if( szDir[i-1]!='\\' )
    lstrcat(szDir, "\\");
}
//---------------------------------------------------------------------------
void UnTerminateDirectory(char * szDir)
{
  if( ! szDir ) return;
  int i = lstrlen( szDir );
  if( i < 1 ) return;
  if( szDir[i-1]=='\\' )
    szDir[i-1] = 0;
}
//---------------------------------------------------------------------------
//
extern char szLogFile[];
//---------------------------------------------------------------------------
void WriteToLogTime(char * fmt, bool savetime, ...)
{
  char str[1000];
  va_list argptr;
  va_start(argptr, savetime);
  wvsprintf(str, fmt, argptr);
  va_end(argptr);
  //
  TDateTime t = Now();
  char szTime[40];
  if( savetime )
    wsprintf(szTime, "%s %s  ", t.DateString().c_str(), t.TimeString().c_str());
  else
    *szTime=0;
  //
  TFile out(szLogFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
  if( !out ) return;
  out.SetPointer(0, FILE_END);
  out << szTime << str << "\r\n";
}
//---------------------------------------------------------------------------
// У АРМ LanMon и у Данфосс Контур и у OPC DA server свои функции WriteToLog !
#if !defined(ARMLanMon) && !defined(DANFOSS) && !defined(OPC_DA)
void WriteToLog(char *fmt, ...)
{
  char str[2000];
  va_list argptr;
  va_start(argptr, fmt);
  wvsprintf(str, fmt, argptr);
  va_end(argptr);

  TDateTime t = Now();
  char szTime[40];
  wsprintf(szTime, "%s %s  ", AnsiString(t.DateString()).c_str(), AnsiString(t.TimeString()).c_str());

  TFile out(AnsiString(szLogFile).c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
  if( !out ) return;
  out.SetPointer(0, FILE_END);
  out << szTime << str << "\r\n";
}
#endif
//---------------------------------------------------------------------------
char DigitToChar(BYTE num)
{
  num &= 0x0F;
  if( num<10 ) return (char)('0' + num);
  else         return (char)('A' + num - 10);
}
//---------------------------------------------------------------------------
BYTE CharToDigit(char ch, char cl)
{
  BYTE rv = 0;
  if( ch>='0' && ch<='9' ) rv += (BYTE)((ch-'0')<<4);
  else if( ch>='A' && ch<='F' ) rv += (BYTE)((ch-'A'+0x0A)<<4);
  else if( ch>='a' && ch<='f' ) rv += (BYTE)((ch-'a'+0x0A)<<4);

  if( cl>='0' && cl<='9' ) rv += (BYTE)(cl-'0');
  else if( cl>='A' && cl<='F' ) rv += (BYTE)(cl-'A'+0x0A);
  else if( cl>='a' && cl<='f' ) rv += (BYTE)(cl-'a'+0x0A);
  return rv;
}
//---------------------------------------------------------------------------
AnsiString ToHexMas(void * data, int size)
{
  AnsiString s;
  if( data )
    for(int i=0; i<size; i++)
    {
      s += DigitToChar((BYTE)(((BYTE *)data)[i]>>4));
      s += DigitToChar(((BYTE *)data)[i]);
    }
  return s;
}
//---------------------------------------------------------------------------
bool FromHexMas(AnsiString s, void * data, int size)
{
  if( s.Length()<size*2 || !data || !size )
    return false;

  int index = 1;
  for(int i=0; i<size; i++)
  {
    if( s.Length() < index+1 ) return false;
    if( s[index] == ' ' ) { index++; i--; continue; }
    if( ! IsHexDigit(s[index]) || ! IsHexDigit(s[index+1]) ) return false;
    ((BYTE *)data)[i] = CharToDigit(s[index], s[index+1]);
    index += 2;
  }
  return true;
}
//---------------------------------------------------------------------------
String GetTimeString(TDateTime interval)
{
  if( (double)interval < 0.0 )
    return __UTEXT("?");

  String rv;
  DWORD v;
  // days >= 2
  if( (double)interval >= 2.0 )
  {
    v = (DWORD)(double)interval;
    rv.printf(__UTEXT("%lu %s"), v, GetDaysName(v).c_str());
  }
  // hours >= 2
  else if( (double)interval >= 0.083 )
  {
    v = (DWORD)((double)interval*24.0);
    rv.printf(__UTEXT("%lu %s"), v, GetHoursName(v).c_str());
  }
  // min >=2
  else if( (double)interval >= 0.00138 )
    rv.printf(__UTEXT("%lu мин"), (DWORD)((double)interval*1440.0));
  // sec >= 2
  else
    rv.printf(__UTEXT("%lu сек"), (DWORD)((double)interval*86400.0));
  return rv;
}
//---------------------------------------------------------------------------
String GetDaysName(DWORD days)
{
  switch( days )
  {
    case 1:
      return __UTEXT("день");
    case 2:
    case 3:
    case 4:
      return __UTEXT("дня");
  }
  return __UTEXT("дней");
}
//---------------------------------------------------------------------------
String GetHoursName(DWORD hours)
{
  switch( hours )
  {
    case 1:
      return __UTEXT("час");
    case 2:
    case 3:
    case 4:
      return __UTEXT("часа");
  }
  return __UTEXT("часов");
}
//---------------------------------------------------------------------------
AnsiString GetBytesString(DWORD count)
{
  char buf[100];
  // Mb >= 2
  if( count >= 1048576 )
    wsprintf(buf, "%lu.%u Мб", count/1048576, (WORD)((float)(count%1048576)/104857.60));
  // Kb >= 2
  else if( count >= 2048 )
    wsprintf(buf, "%lu.%u Кб", count/1024, (WORD)((float)(count%1024)/102.40));
  // b >= 2
  else
    wsprintf(buf, "%lu б", count);
  return AnsiString(buf);
}
//---------------------------------------------------------------------------
String GetBytesStringEng(ULONGLONG count)
{
  String rv;
  // Gb >= 1
  if( count >= ONE_GB )
    rv.printf("%lu.%u Gb", count/ONE_GB, (WORD)((double)(count%ONE_GB)/(ONE_GB/10.0)));
  // Mb >= 1
  else if( count >= ONE_MB )
    rv.printf("%lu.%u Mb", (ULONG)(count/ONE_MB), (WORD)((double)(count%ONE_MB)/(ONE_MB/10.0)));
  // Kb >= 2
  else if( count >= 2*ONE_KB )
    rv.printf("%lu.%u Kb", count/ONE_KB, (WORD)((double)(count%ONE_KB)/(ONE_KB/10.0)));
  // b >= 2
  else
    rv.printf("%lu bytes", count);
  return rv;
}
//---------------------------------------------------------------------------
AnsiString TimeIntervalToString(TDateTime interval)
{
  AnsiString rv;
  DWORD dd, hh, mm, ss;
  dd = (DWORD)(double)interval;
  interval -= (double)dd;
  hh = (DWORD)((double)interval/(1.0/24.0));
  interval -= (double)hh*(1.0/24.0);
  mm = (DWORD)((double)interval/(1.0/1440.0));
  interval -= (double)mm*(1.0/1440.0);
  ss = (DWORD)((double)interval/(1.0/86400.0));
  return rv.sprintf("%u %02u:%02u:%02u", dd, hh, mm, ss);
}
//---------------------------------------------------------------------------
//#define AllowFormVisibleInPixel 100
bool CorrectFormRect(int & l, int & t, int & w, int & h)
{
  if( l<=-10000 || t<=-10000 || w<=-10000 || h<=-10000 )
  {
    l = 50;
    t = 50;
    w = 300;
    h = 300;
    return false;
  }

  // Определит размеры виртуального рабочего стола dw, dh
  #if __BORLANDC__ >= 0x0560
    // 0x0550	- C++Builder 5.0 Enterprise
    // 0x0560	- C++Builder 6.0 Enterprise
    int dw = Screen->DesktopWidth;
	int dh = Screen->DesktopHeight;
	int dl = Screen->DesktopLeft;
	int dt = Screen->DesktopTop;
  #else
	int dw = Screen->Width;
	int dh = Screen->Height;
	int dl = 0;
	int dt = 0;
  #endif
  /*
  dl = -1200
  dt = 0
  dw = 2280
  dh = 1200
  */
  // Ширина окна не может быть больше рабочего стола
  if( w > dw )
	w = dw;

  // Высота окна не может быть больше рабочего стола
  if( h > dh )
    h = dh;

  // Окно выходит за левую границу ?
  if( l < dl )
  {
	// Если видно менее AllowFormVisibleInPixel пикселей - переместим его
	//if( l+w < AllowFormVisibleInPixel ) l = 0;
	l = dl;
  }

  // Окно уходит за правую границу ?
  if( ABS(dl)+l+w > dw )
  {
	// Если видно менее AllowFormVisibleInPixel пикселей от окна - переместим его
	//if( dw-l < AllowFormVisibleInPixel ) l = 0;
	l -= MIN(ABS(dl)+l, ABS(dl)+l+w-dw);
  }

  // Окно уходит вверх ?
  if( t < dt )
  {
	t = dt;
  }

  // Окно уходит вниз ?
  if( ABS(dt)+t+h > dh )
  {
	// Если видно менее AllowFormVisibleInPixel пикселей от окна - переместим его
	//if( dh-t < AllowFormVisibleInPixel ) t = 0;
	t -= MIN(ABS(dt)+t, ABS(dt)+t+h-dh);
  }

  return true;
}
//---------------------------------------------------------------------------
extern char szPositionFile[];
//---------------------------------------------------------------------------
void LoadFormPosition(TMYIniFile * p, TForm * f)
{
  try
  {
	int l = p->ReadInteger(AnsiString(f->Name).c_str(), "Left", -10000);
	int t = p->ReadInteger(AnsiString(f->Name).c_str(), "Top", -10000);
	int w = p->ReadInteger(AnsiString(f->Name).c_str(), "Width", -10000);
	int h = p->ReadInteger(AnsiString(f->Name).c_str(), "Height", -10000);
    if( CorrectFormRect(l, t, w, h) ) f->SetBounds(l, t, w, h);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void SaveFormPosition(TMYIniFile * p, TForm * f)
{
  try
  {
	p->WriteInteger(AnsiString(f->Name).c_str(), "Left", f->Left);
	p->WriteInteger(AnsiString(f->Name).c_str(), "Top", f->Top);
	p->WriteInteger(AnsiString(f->Name).c_str(), "Width", f->Width);
	p->WriteInteger(AnsiString(f->Name).c_str(), "Height", f->Height);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void LoadPosition(TForm * f, int * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    // open in read only mode
    p = new TMYIniFile(szPositionFile, false, false);
    LoadFormPosition(p, f);
    // Считаем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[20];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		param[i] = p->ReadInteger(AnsiString(f->Name).c_str(), str, 0);
      }
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void LoadPosition(TForm * f, double * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    // open in read only mode
    p = new TMYIniFile(szPositionFile, false, false);
    LoadFormPosition(p, f);
    // Считаем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[40];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		param[i] = p->ReadFloat(AnsiString(f->Name).c_str(), str, 0.0);
      }
	}
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void LoadPosition(TForm * f, AnsiString * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    // open in read only mode
    p = new TMYIniFile(szPositionFile, false, false);
    LoadFormPosition(p, f);
    // Считаем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[40];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		param[i] = p->ReadString(AnsiString(f->Name).c_str(), str);
      }
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void SavePosition(TForm * f, int * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    p = new TMYIniFile(szPositionFile);
    SaveFormPosition(p, f);
    /*
    p->WriteInteger(f->Name.c_str(), "Left", f->Left);
    p->WriteInteger(f->Name.c_str(), "Top", f->Top);
    p->WriteInteger(f->Name.c_str(), "Width", f->Width);
    p->WriteInteger(f->Name.c_str(), "Height", f->Height);
    */
    // Пишем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[20];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		p->WriteInteger(AnsiString(f->Name).c_str(), str, param[i]);
      }
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void SavePosition(TForm * f, double * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    p = new TMYIniFile(szPositionFile);
    SaveFormPosition(p, f);
    /*
    p->WriteInteger(f->Name.c_str(), "Left", f->Left);
    p->WriteInteger(f->Name.c_str(), "Top", f->Top);
    p->WriteInteger(f->Name.c_str(), "Width", f->Width);
    p->WriteInteger(f->Name.c_str(), "Height", f->Height);
    */
    // Пишем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[20];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		p->WriteFloat(AnsiString(f->Name).c_str(), str, param[i]);
      }
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void SavePosition(TForm * f, AnsiString * param, int count)
{
  TMYIniFile * p = NULL;
  try
  {
    p = new TMYIniFile(szPositionFile);
    SaveFormPosition(p, f);
    /*
    p->WriteInteger(f->Name.c_str(), "Left", f->Left);
    p->WriteInteger(f->Name.c_str(), "Top", f->Top);
    p->WriteInteger(f->Name.c_str(), "Width", f->Width);
    p->WriteInteger(f->Name.c_str(), "Height", f->Height);
    */
    // Пишем дополнительные параметры, если надо
    if( param && count>0 )
    {
      char str[20];
      for(int i=0; i<count; i++)
      {
        wsprintf(str, "PARAM%d", i+1);
		p->WriteString(AnsiString(f->Name).c_str(), str, param[i].c_str());
      }
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
/*
void ASSERT(bool b)
{
  if( !b ) ::MessageBox(NULL, "Error !", "ASSERT(...)", MB_ICONERROR);
}
*/
//---------------------------------------------------------------
char * MyNewStr(char * s, int size)
{
  if( !s ) return NULL;
  if( !size ) size = strlen(s)+1;
  char * p = new char[size];
  lstrcpyn(p, s, size);
  return p;
}
//---------------------------------------------------------------------------
// CmdLine - полная командная строка
bool RunProg(char * CmdLine, WORD wShowWindow, bool bWait,
             DWORD * pExitCode, char * szCurDir, DWORD * pdwProcessId)
{
  bool RetVal;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  GetStartupInfo(&si);
  si.wShowWindow = wShowWindow;

  RetVal = CreateProcess(NULL, CmdLine, NULL, NULL, FALSE,
    NORMAL_PRIORITY_CLASS, NULL, szCurDir, &si, &pi);

  if( RetVal )
  {
    if( bWait )
      WaitForSingleObject(pi.hProcess, INFINITE);

    if( pExitCode )
      GetExitCodeProcess(pi.hProcess, pExitCode);
  }
  else
  {
    if( pExitCode )
      *pExitCode = GetLastError();
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  if( pdwProcessId )
    *pdwProcessId = pi.dwProcessId;
  return RetVal;
}
//---------------------------------------------------------------------------
#if !defined(DANFOSS)
bool RunTelnet(AnsiString params)
{
  AnsiString TelnetPath;
  char str[260];
  if( GetSystemDirectory(str, sizeof(str)) > 0 )
  {
    lstrcat(str, "\\telnet.exe");
	if( FileExists(str) )
      TelnetPath = str;
  }
  if( TelnetPath.Length() == 0 )
  {
    if( GetWindowsDirectory(str, sizeof(str)) > 0 )
    {
      lstrcat(str, "\\telnet.exe");
      if( FileExists(str) )
        TelnetPath = str;
    }
  }
  if( TelnetPath.Length() == 0 )
  {
    TelnetPath = "telnet.exe";
  }

  TelnetPath += " ";
  TelnetPath += params;

  DWORD Error;
  if( RunProg(TelnetPath.c_str(), SW_NORMAL, false, &Error) )
    return true;
  ReportError("Ошибка запуска программы: %s [%lu]", TelnetPath.c_str(), Error);
  return false;
}
#endif
//---------------------------------------------------------------------------
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszWorkDir,
        LPSTR lpszPathLink, LPSTR lpszDesc, LPSTR lpszArgument)
{
  HRESULT hres;
  IShellLink *psl;
  CoInitialize(NULL);
  hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);

  if( SUCCEEDED(hres) )
  {
    IPersistFile *ppf;
    // Путь к программе
    psl->SetPath(lpszPathObj);
    // Рабочий каталог для програмы
    if( lstrlen(lpszWorkDir) > 0 )
    {
      psl->SetWorkingDirectory(lpszWorkDir);
    }
    else
    {
	  psl->SetWorkingDirectory(AnsiString(ExtractFilePath(lpszPathObj)).c_str());
    }
    // Название ярлыка
    psl->SetDescription(lpszDesc);
    // Командная строка для програмы
    psl->SetArguments(lpszArgument);
	hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if( SUCCEEDED(hres) )
    {
      wchar_t wsz[MAX_PATH];
      MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);
      hres = ppf->Save(wsz, true);
      ppf->Release();
    }
    psl->Release();
  }
  CoUninitialize();
  return hres;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Максимальное количество "старых" карт, доступное в меню
void AddToLastOpenDocs(char * szIniFile, char * szDocFile)
{
  if( !szDocFile ) return;
  if( lstrlen(szDocFile)<=0 ) return;
  //
  TMYIniFile * p = NULL;
  try
  {
    p = new TMYIniFile(szIniFile);
    char Section[] = "LASTDOCS";
    char str[20];
    //
    AnsiString ld[MAX_LAST_OPEN_DOCS];
    ld[0] = szDocFile;

    // Прочитаем все имена файлов с 1 по 9 (10-ый уходит)
    for(int i=0; i<MAX_LAST_OPEN_DOCS-1; i++)
    {
      wsprintf(str, "%u", i+1);
      ld[i+1] = p->ReadString(Section, str);
    }

    int index = 1; // Номер секции
    for(int i=0; i<MAX_LAST_OPEN_DOCS; i++)
    {
      // Уберем повторяющиеся имена файлов...
      bool b = false;
      for(int j=0; j<i; j++)
      {
        // Такой ld[i] уже был ? - перейдем к следующему
        if( MySameText(ld[j],ld[i]) ) { b=true; break; }
      }
      if( b ) continue;
      //
      wsprintf(str, "%u", index);
      p->WriteString(Section, str, ld[i].c_str());
      index++;
    }

    // Допишем пустыми строками
    for(int i=index; i<MAX_LAST_OPEN_DOCS; i++)
    {
      wsprintf(str, "%u", index);
      p->WriteString(Section, str, "");
      index++;
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
void AddLastOpenDocsToMenu(char * szIniFile, TMenuItem ** mi)
{
  TMYIniFile * p = NULL;
  try
  {
    // open in read only mode
    p = new TMYIniFile(szIniFile, false, false);
    char Section[] = "LASTDOCS";
    char str[20];
    char str1[MN];

    for(int i=0; i<MAX_LAST_OPEN_DOCS; i++)
    {
      wsprintf(str, "%u", i+1);
      lstrcpyn(str1, p->ReadString(Section, str).c_str(), sizeof(str1));
      //
      mi[i]->Caption = str1;
      mi[i]->Visible = lstrlen(str1) > 0;
    }
  }
  catch(...) {}
  delete p;
}
//---------------------------------------------------------------------------
int VarToBinary(Variant v, BYTE * buf, int size)
{
  int RetVal = 0;
  try
  {
    if( v.IsArray() )
    {
      int n = v.ArrayHighBound();
      for(int j = v.ArrayLowBound(); j<=n; j++)
      {
        RetVal += VarToBinary(v.GetElement(j), buf+RetVal, size-RetVal);
        if( RetVal >= size ) return RetVal;
      }
    }
    else
    {
      switch( v.Type() )
      {
        //case varEmpty	0x0000	The variant is Unassigned.
        //case varNull	0x0001	The variant is Null.
        case varSmallint: //	0x0002	16-bit signed integer (type short int)
          *(short int *)(buf+RetVal) = (short int)v;
          RetVal += sizeof(short int);
        break;
        case varInteger: //	0x0003	32-bit signed integer (type int).
          *(int *)(buf+RetVal) = (int)v;
          RetVal += sizeof(int);
        break;
        case varSingle: //	0x0004	Single-precision floating-point value (type float)
          *(float *)(buf+RetVal) = (float)v;
          RetVal += sizeof(float);
        break;
        case varDouble: //	0x0005	Double-precision floating-point value (type double).
          *(double *)(buf+RetVal) = (double)v;
          RetVal += sizeof(double);
        break;
        case varCurrency: //	0x0006	Currency floating-point value (type Currency).
		  *(Currency *)(buf+RetVal) = (Currency)(double)v;
		  RetVal += sizeof(Currency);
        break;
		case varDate: //	0x0007	Date and time value (type TDateTime).
		  *(TDateTime *)(buf+RetVal) = (TDateTime)(double)v;
          RetVal += sizeof(TDateTime);
        break;
		case varOleStr: //	0x0008	Reference to a dynamically allocated Unicode string.
		{
		  AnsiString s = v;
		  lstrcpy(buf+RetVal, s.c_str());
		  RetVal += lstrlen(s.c_str());
		}
		break;
        //case varDispatch	0x0009	Reference to an OLE automation object (an IDispatch interface pointer).
        case varError: //	0x000A	Operating system error code.
          *(int *)(buf+RetVal) = (int)v;
          RetVal += sizeof(int);
        break;
        case varBoolean: //	0x000B	16-bit boolean (type WordBool).
          *(WordBool *)(buf+RetVal) = (WordBool)v;
          RetVal += sizeof(WordBool);
        break;
        //case varVariant	0x000C	Indicates another Variant.
        //case varUnknown	0x000D	Reference to an unknown OLE object (an IUnknown interface pointer).
        case varByte: //	0x0011	Indicates an unsigned char.
          *(unsigned char *)(buf+RetVal) = (unsigned char)v;
          RetVal += sizeof(unsigned char);
        break;
		case varString: //	0x0100	Reference to a dynamically allocated string (type AnsiString).
		{
		  AnsiString s = v;
		  lstrcpy(buf+RetVal, s.c_str());
		  RetVal += lstrlen(s.c_str());
		}
		break;
        //case varTypeMask	0x0FFF	Indicates the type of each array element.
        //case varArray	0x2000	Indicates a Variant array.
        //case varByRef	0x4000	Indicates that the variant contains a reference as opposed to a value.
      }
    }
  }
  catch(...)
  {
  }
  return RetVal;
}
//---------------------------------------------------------------------------
// Расчет контрольной суммы массива длиной N по адресу P
BYTE Summa(void * P, WORD N)
{
  BYTE S = 0;
  for(int i=0; i<N; i++) S += ((BYTE *)P)[i];
  return S;
}
//---------------------------------------------------------------------------
BYTE FromBCD(BYTE val)
{
  return (BYTE)( (BYTE)((val>>4)*10) + (BYTE)(val&0x0F) );
}
//---------------------------------------------------------------------------
BYTE ToBCD(BYTE val)
{
  val %= (BYTE)100;
  return (BYTE)( (val/10<<4) + (val%10) );
}
//---------------------------------------------------------------------------
AnsiString ByteToBinary(BYTE val)
{
  char str[10];
  for(int j=0; j<8; j++) str[7-j] = (val & (1<<j)) ? '1' : '0';
  str[8] = 0;
  return AnsiString(str);
}
//---------------------------------------------------------------------------
AnsiString ExtractSubString(AnsiString s, int WordNumber, AnsiString delim)
{
  AnsiString rv;
  try
  {
	int l = s.Length();
    int StartPos;
    int i = 1;
    while( i<=l && s.IsDelimiter(delim, i) ) i++;
    StartPos = i;

    for(; i<=l; i++)
    {
      if( s.IsDelimiter(delim, i) )
      {
        if( ! --WordNumber )
        {
          rv = s.SubString(StartPos, i-StartPos);
          break;
        }
        else
        {
          // Skip all delimiters
          while( i+1<=l && s.IsDelimiter(delim, i+1) ) i++;
          StartPos = i+1;
        }
      }
    }
    // Последнее слово
    if( ! --WordNumber )
    {
      rv = s.SubString(StartPos, i-StartPos);
    }
  }
  catch(...)
  {
  }
  return rv;
}
//---------------------------------------------------------------------------
/* Транслитерация русских букв по ГОСТ
        а - a              к - k             х - kh
        б - b              л - l             ц - c
        в - v              м - m             ч - ch
        г - g              н - n             ш - sh
        д - d              о - o             щ - shh
        е - e              п - p             ъ - "
        ё - jo             р - r             ы - y
        ж - zh             с - s             ь - '
        з - z              т - t             э - eh
        и - i              у - u             ю - ju
        й - jj             ф - f             я - ja
*/
//---------------------------------------------------------------------------
void Transliterate(char * source, char * dest)
{
  if( !source || !dest ) return;

  char RS[]  = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя";
  char RD1[] = "ABVGDEZZIJKLMNOPRSTUFKCCSS\"Y'EJJabvgdezzijklmnoprstufkccss\"y'ejj";
  char RD2[] = "      H  J           H HHH\"Y'HUA      h  j           h hhh\"y'hua";

  int len = strlen(source);
  for(int i=0; i<len; i++)
  {
	for(int j=0; j<(int)sizeof(RS); j++)
    {
      if( source[i]==RS[j] )
	  { *dest++=RD1[j]; if( RD2[j]!=' ' ) *dest++=RD2[j]; goto Next; }
    }
    *dest++ = source[i];
  Next:;
  }
  *dest = 0;
}
//---------------------------------------------------------------------------
void SetBit(BYTE * data, int bit, bool set)
{
  if( set ) data[bit/8] |= (BYTE)(1<<(bit%8));
  else data[bit/8] &= ~(BYTE)(1<<(bit%8));
}
//---------------------------------------------------------------------------
bool GetBit(BYTE * data, int bit)
{
  return data[bit/8] & (BYTE)(1<<(bit%8));
}
//---------------------------------------------------------------------------
BYTE HexVal(char hex)
{
  hex = (char)CharUpper((LPTSTR)hex);
  if( (hex>='0') && (hex<='9') ) return (BYTE)(hex-'0');
  if( (hex>='A') && (hex<='F') ) return (BYTE)(hex-'A'+10);
  return 0;
}
//---------------------------------------------------------------------------
int VariantGetValuesCount(Variant v)
{
  int rv = 0;
  try
  {
    if( ! v.IsNull() )
    {
      rv = 1;
      if( v.IsArray() )
        rv = v.ArrayHighBound() - v.ArrayLowBound() + 1;
    }
  }
  catch(...)
  {}
  return rv;
}
//---------------------------------------------------------------------------
AnsiString FormatLastError(DWORD & error)
{
  AnsiString rv;
  error = GetLastError();
  LPVOID lpMsgBuf;
  if( ! FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL)
    )
    return rv;
  rv = (LPCTSTR)lpMsgBuf;
  //lstrcpyn(szMess, (LPCTSTR)lpMsgBuf, MessSize);
  // Free the buffer.
  LocalFree( lpMsgBuf );
  return rv;
}
//---------------------------------------------------------------------------
AnsiString FormatLastError2(DWORD error)
{
  AnsiString rv;
  LPVOID lpMsgBuf;
  if( ! FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL)
    )
    return rv;
  rv = (LPCTSTR)lpMsgBuf;
  LocalFree( lpMsgBuf );
  return rv;
}
//---------------------------------------------------------------------------
AnsiString __fastcall GetValueFromString(AnsiString s)
{
  int i = s.Pos('=');
  if( i > 0 )
  {
    return s.SubString(i+1, s.Length()-i);
  }
  return "";
}
//---------------------------------------------------------------------------
bool GetFileVersion(AnsiString File, WORD & Major, WORD & Minor, WORD & Release, WORD & Build)
{
  DWORD Dummy;
  DWORD size = GetFileVersionInfoSize(File.c_str(), &Dummy);
  if( size<=0 )
    return false;

  void * p = new BYTE[size];
  if( !p )
    return false;

  if( ! GetFileVersionInfo(File.c_str(), 0, size, p) )
  {
    delete [] p;
    return false;
  }

  VS_FIXEDFILEINFO * pfi;
  if( ! VerQueryValue(p, "\\", (void **)&pfi, (PUINT)&Dummy) )
  {
    delete [] p;
    return false;
  }

  Major = HIWORD(pfi->dwFileVersionMS);
  Minor = LOWORD(pfi->dwFileVersionMS);
  Release = HIWORD(pfi->dwFileVersionLS);
  Build = LOWORD(pfi->dwFileVersionLS);

  delete [] p;
  return true;
}
//---------------------------------------------------------------------------
AnsiString IPAddrToString(DWORD ip)
{
  AnsiString s;
  s.printf("%d.%d.%d.%d",
          (ip & 0xff) ,
          (ip & 0xff00) >> 8,
          (ip & 0xff0000) >> 16,
          (ip & 0xff000000)>>24);
  return s;
}
//---------------------------------------------------------------------------
bool IsHexDigit(char c)
{
  return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f');
}
//---------------------------------------------------------------------------
// Кол-во 100 наносекундных тиков в сутках
#define T_ONEDAY    864000000000.0
const ULONGLONG cDelphiTime = (21968297ui64<<32) + 2836185088ui64;
//---------------------------------------------------------------------------
ULONGLONG TDateTimeToFileTime(double t)
{
  //return ((ULONGLONG)(DWORD)t) * T_ONEDAY +
  //		 ((ULONGLONG)(DWORD)((t - (double)(DWORD)t) * 864000000.0)) * 1000 +
  //		  cDelphiTime;
  return t * T_ONEDAY + cDelphiTime; // это то-же ?
}
/* Альтернатива
FILETIME TDateTimeToFileTime(TDateTime t)
{
  SYSTEMTIME st;
  FILETIME local_ft, ft;
  DateTimeToSystemTime(t, st);
  SystemTimeToFileTime(&st, &local_ft);
  LocalFileTimeToFileTime(&local_ft, &ft); // local -> UTC
  return ft;
}
*/
//---------------------------------------------------------------------------
double TFileTimeToDateTime(ULONGLONG t)
{
  return (double)(t - cDelphiTime) / (double)T_ONEDAY;
}
//---------------------------------------------------------------------------
int iscodedelim(char c)
{
  if( strchr("+-*^/=;(),&|<>% \t\r", c) || c==0 ) return 1;
  return 0;
}
//---------------------------------------------------------------------------
AnsiString __fastcall MakePostgresString(AnsiString source, int maxlen)
{
  try
  {
    for(int i=1,l=source.Length(); i<=l; i++)
    {
      if( source[i]=='\'' )
      {
        source.Insert('\'', i++);
        l++;
      }
      else if( source[i]=='\\' )
      {
        source.Insert('\\', i++);
        l++;
      }
    }
  }
  catch(...)
  {}
  if( maxlen>0 )
	if( source.Length() > maxlen )
      source.SetLength(maxlen);
  return AnsiString('\'') + source + '\'';
}
//---------------------------------------------------------------------------
bool __fastcall WaitService(char * szServiceName, DWORD dwCurrentState, DWORD to)
{
  bool rv = false;
  SC_HANDLE hsm, hs;  DWORD tc;  SERVICE_STATUS SS;
  hsm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if( !hsm )
  {
    Sleep(to);
    return rv;
  }
  hs = OpenService(hsm, szServiceName, SERVICE_ALL_ACCESS);
  if( !hs )
  {
    CloseServiceHandle(hsm);
    return rv;
  }
  tc = GetTickCount();
  while( QueryServiceStatus(hs, &SS) && SS.dwCurrentState!=dwCurrentState && GetTickCount()-tc<to ) Sleep(2);
  if( SS.dwCurrentState==dwCurrentState )
    rv = true;
  CloseServiceHandle(hs);
  CloseServiceHandle(hsm);
  return rv;
}
//---------------------------------------------------------------------------
void StringGridToClipboard(TStringGrid * p)
{
  String str;
  for(int j=0; j<p->RowCount; j++)
  {
    for(int i=0; i<p->ColCount; i++)
    {
      str += p->Cells[i][j];
      str += (i==p->ColCount-1) ? "\r\n" : "\t";
    }
  }
  //Clipboard()->SetTextBuf(str.c_str()); don't work in CodeGear 2007
  setClipboard(str);
}
//---------------------------------------------------------------------------
void TreeToClipboard(TTreeView * p)
{
  String str;
  TTreeNode * n;
  for(int i=0; i<p->Items->Count; i++)
  {
	n = p->Items->Item[i];
    for(int j=0; j<n->Level; j++)
      str += "\t";
    str += n->Text + "\r\n";
  }
  //Clipboard()->SetTextBuf(str.c_str()); don't work in CodeGear 2007
  setClipboard(str);
}
//---------------------------------------------------------------------------
void StringsToClipboard(TStrings * p)
{
  String str;
  for(int i=0; i<p->Count; i++)
  {
    str += p->Strings[i] + "\r\n";
  }
  //Clipboard()->SetTextBuf(str.c_str()); don't work in CodeGear 2007
  setClipboard(str);
}
//---------------------------------------------------------------------------
bool MyStrToFloat(String str, double & result)
{
  bool rv = false;
  try
  {
    MyStrToFloatException(str, result);
    rv = true;
  }
  catch(...)
  {
  }
  return rv;
}
//---------------------------------------------------------------------------
void MyStrToFloatException(String str, double & result)
{
  #if __BORLANDC__ >= 0x0630
  char ds = FormatSettings.DecimalSeparator;
  #else
  char ds = DecimalSeparator;
  #endif
  int i;
  if( ds != '.' )
  {
    i = str.Pos('.');
    if( i>0 )
      str[i] = ds;
  }
  if( ds != ',' )
  {
    i = str.Pos(',');
    if( i>0 )
      str[i] = ds;
  }
  result = (double)StrToFloat(str);
}
//---------------------------------------------------------------------------
double MyStrToFloatException(String str)
{
  double rv = 0.0;
  MyStrToFloatException(str, rv);
  return rv;
}
//---------------------------------------------------------------------------
double MyStrToFloatDef(String str, double def)
{
  double rv = def;
  try
  {
    MyStrToFloatException(str, rv);
  }
  catch(...)
  {
  }
  return rv;
}
//---------------------------------------------------------------------------
String MyFloatToStr(double v, int digit)
{
  String m;
  m.printf(__UTEXT("%%.%df"), digit);
  TVarRec vr[1];
  vr[0] = v;
  return Format(m, vr, ARRAYSIZE(vr) - 1);
}
//---------------------------------------------------------------------------
String GetDayOfWeekNameLong(int dow)
{
  if( dow==8 )
    dow = 1;
#if __BORLANDC__ >= 0x0630
  if( dow>=1 && dow<=7 )
	return FormatSettings.LongDayNames[dow-1];
#else
  if( dow>=1 && dow<=7 )
	return LongDayNames[dow-1];
#endif
  return IntToStr(dow);
}
//---------------------------------------------------------------------------
String GetMonthNameLong(int month)
{
#if __BORLANDC__ >= 0x0630
  if( month>=1 && month<=12 )
	return FormatSettings.LongMonthNames[month-1];
#else
  if( month>=1 && month<=12 )
	return LongMonthNames[month-1];
#endif
  return IntToStr(month);
}
//---------------------------------------------------------------------------
String DateTimeIntellektFormat(TDateTime dt, String _TimeFormat)
{
	TDateTime t = Now();
	unsigned short c_year, c_month, c_day;
	t.DecodeDate(&c_year, &c_month, &c_day);
	unsigned short year, month, day;
	dt.DecodeDate(&year, &month, &day);
	// Это сегодня ?
	if( (int)t == (int)dt )
	{
		// да: время
		// Используем системные настройки даты времени
		//return dt.TimeString();
		return dt.FormatString(_TimeFormat);
	}
	// Это в пределах текущего года ?
	else if( c_year == year )
	{
		// да: число месяц время
		return dt.FormatString("d mmm " + _TimeFormat);
	}
	/*
	// Это в пределах текущего месяца ?
	else if( (int)t - (int)dt < day )
	{
		// да - число, месяц + время
		return dt.FormatString("d mmm " + _TimeFormat);
	}
	*/
	/*
	// Это в пределах недели ?
	else if( (int)t - (int)dt < 7 )
	{
		// да - название дня недели + время
		return GetDayOfWeekNameLong(DayOfWeek(dt)) + " " + dt.FormatString(_TimeFormat);
	}
	*/
	else
	{
		// Используем системные настройки для форматирования даты времени
		//return dt.FormatString(FormatSettings.ShortDateFormat + " " + _TimeFormat);
		// число месяц год время
		return dt.FormatString("d mmm yyyy " + _TimeFormat);
	}
}
//---------------------------------------------------------------------------
bool setClipboard(AnsiString & lines){
	HGLOBAL handle;
	LPTSTR linesCopy;
	int lines_length = lines.Length();

	if (!OpenClipboard(NULL))
		return false;
	
	EmptyClipboard();

	if((handle = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, lines_length + 1))==NULL)
		goto closeclip;

	if((linesCopy = (LPTSTR)GlobalLock(handle))==NULL)
		goto freemem;

	memcpy(linesCopy, lines.data(), lines_length);
	linesCopy[lines_length] = '\0';

	GlobalUnlock(handle);

	if (SetClipboardData(CF_TEXT, handle)==NULL) 
		goto freemem;

	if (SetClipboardData(CF_LOCALE, handle)==NULL)
		goto freemem;

	CloseClipboard();

	return true;

freemem:
	GlobalFree(handle);
	
closeclip:
	CloseClipboard();
	
	return false;
}
//---------------------------------------------------------------------------
void __fastcall FindFiles(TStrings * p, String dir, String mask)
{
  TSearchRec sr;
  if( FindFirst(IncludeTrailingBackslash(dir) + mask, faAnyFile, sr)==0 )
  {
    // Первый файл найден
    do
    {
      p->Add(sr.Name);
    }
    while( FindNext(sr)==0 );
  }
  FindClose(sr);
}
//---------------------------------------------------------------------------
String __fastcall GetRecommendedPath(bool common, String folder, String def)
{
  String result;

  // CSIDL_APPDATA (roaming)
  //    XP       C:\Documents and Settings\username\Application Data
  //    Vista&7  C:\\Users\\username\\AppData\\Roaming\\
  // CSIDL_COMMON_APPDATA   C:\Documents and Settings\All Users\Application Data
  // CSIDL_LOCAL_APPDATA    C:\Documents and Settings\username\Local Settings\Application Data (nonroaming)
  char str[MN];
  if( SHGetFolderPath(NULL, (common ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA) | CSIDL_FLAG_CREATE,
					  NULL, SHGFP_TYPE_CURRENT, str) == S_OK )
  {
	result = IncludeTrailingPathDelimiter(str);
	if( folder.Length() > 0 )
	{
	  result += IncludeTrailingPathDelimiter(folder);
	  if( ! DirectoryExists(result) )
	    CreateDir(result);
	}
  }
  else
  {
	result = def;
  }
  return result;
}
//---------------------------------------------------------------------------
bool RunPaint(String file, String * error)
{
  char str[MAX_PATH * 2];
  char szPaint[] = "mspaint.exe";
  char szPath[MAX_PATH];

  // Запуск paint
  if( SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szPath) != S_OK )
    return false;
  if( file.Length() > 0 )
    wsprintf(str, "%s\\%s \"%s\"", szPath, szPaint, file.c_str());
  else
    wsprintf(str, "%s\\%s", szPath, szPaint);
  if( ! RunProg(str, SW_MAXIMIZE) )
  {
    if( SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, szPath) != S_OK )
      return false;
    if( file.Length() > 0 )
      wsprintf(str, "%s\\Accessories\\%s \"%s\"", szPath, szPaint, file.c_str());
    else
      wsprintf(str, "%s\\Accessories\\%s", szPath, szPaint);
    if( ! RunProg(str, SW_MAXIMIZE) )
    {
      if( error )
        *error = AnsiString().sprintf("Ошибка запуска редактора %s !\n(%s)", szPaint, str);
      return false;
    }
  }
  return true;
}
//---------------------------------------------------------------------------
bool __fastcall MySameText(String s1, String s2)
{
  return SameText(s1, s2, loUserLocale);
}
//---------------------------------------------------------------------------
String FormatRegExpString(String source)
{
  String MetaChars = __UTEXT("[]\\/^$.|?*+(){}");
  for(int i=1; i<=source.Length(); i++)
    if( MetaChars.Pos(source[i]) > 0 )
    {
      source.Insert("\\", i);
      i++;
    }
  return source;
}
//---------------------------------------------------------------------------
struct colstring ColorNamesRus[] =
{
  //{ Custom..., "Custom..." },
  { clBlack, "Черный" },
  { clMaroon, "Коричнево-малиновый" },
  { clGreen, "Зеленый" },
  { clOlive, "Оливковый" },
  { clNavy, "Тёмно-синий" },
  { clPurple, "Фиолетовый" },
  { clTeal, "Сине-зелёный" },
  { clGray, "Серый" },
  { clSilver, "Серебряный" },
  { clRed, "Красный" },
  { clLime, "Лайм" },
  { clYellow, "Жёлтый" },
  { clBlue, "Синий" },
  { clFuchsia, "Фуксия" },
  { clAqua, "Цвет морской волны" },
  { clWhite, "Белый" },
  { clMoneyGreen, "Серо-зеленый" },
  { clSkyBlue, "Голубой" },
  { clCream, "Кремовый" },
  { clMedGray, "Светло-серый" },
  { clActiveBorder, "Акт. рамка" },
  { clActiveCaption, "Акт. заголовок" },
  { clAppWorkSpace, "Рабочая область" },
  { clBackground, "Фон" },
  { clBtnFace, "Фон кнопки" },
  { clBtnHighlight, "Выделение кнопки" },
  { clBtnShadow, "Тень кнопки" },
  { clBtnText, "Текст кнопки" },
  { clCaptionText, "Текст заголовка" },
  { clDefault, "По умолчанию" },
  { clGradientActiveCaption, "Градиент акт. заголовка" },
  { clGradientInactiveCaption, "Градиент неакт. заголовка" },
  { clGrayText, "Серый текст" },
  { clHighlight, "Выделение" },
  { clHighlightText, "Выделенный текст" },
  { clHotLight, "Выделение мышью" },
  { clInactiveBorder, "Неакт. рамка" },
  { clInactiveCaption, "Неакт. заголовок" },
  { clInactiveCaptionText, "Текст неакт. заголовка" },
  { clInfoBk, "Инфо фон" },
  { clInfoText, "Инфо текст" },
  { clMenu, "Меню фон" },
  { clMenuBar, "Строка меню" },
  { clMenuHighlight, "Выделение в меню" },
  { clMenuText, "Текст в меню" },
  { clNone, "НЕТ" },
  { clScrollBar, "Полоса прокрутки" },
  { cl3DDkShadow, "3D тень" },
  { cl3DLight, "3D светлый" },
  { clWindow, "Окно" },
  { clWindowFrame, "Рамка окна" },
  { clWindowText, "Текст в окне" }
};
String GetColorNameRus(TColor c)
{
  int i = 0;
  do
  {
    if( ColorNamesRus[i].val == (int)c )
      return ColorNamesRus[i].string;
  }
  while( ColorNamesRus[i++].val != clWindowText );
  return "";
}
//---------------------------------------------------------------------------
void ColorBoxRus(TColorBox * p)
{
  // TColorBox не реагирует на свойство формы Scaled = true
  // На больших шрифтах размер TColorBox не изменяется, а шрифт
  // становится большой !
  // Пробуем это исправить:
  int rh = p->Canvas->TextHeight("Йур");
  p->ItemHeight = rh + rh / 2;

  for(int i=0; i<p->Items->Count; i++)
  {
    if( i==0 && p->Items->Strings[i] == "Custom..." )
    {
      p->Items->Strings[i] = "Выбор...";
      continue;
    }
    String s = GetColorNameRus((TColor)(int)p->Items->Objects[i]);
    if( s.Length() > 0 )
      p->Items->Strings[i] = s;
  }
}
//---------------------------------------------------------------------------
void MySetLocalTime(TDateTime t)
{
  SYSTEMTIME st;
  DateTimeToSystemTime(t, st);
  if( ! SetLocalTime( &st ) )
  {
    DWORD error;
    String errmess = FormatLastError(error);
    throw Exception(errmess + " [" + error + "]");
  }
  // SetLocalTime надо вызывать 2 раза если используется переход на летнее время
  //SetLocalTime( &st );
}
//---------------------------------------------------------------------------
int GetCurrentElevationType(void)
{
  DWORD pID = GetCurrentProcessId();
  if( pID == 0 )
    return 0;

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pID);
  if( hProcess == NULL )
    return 0;

  HANDLE hToken;
  if( ! OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) )
  {
    CloseHandle(hProcess);
    return 0;
  }

  TOKEN_INFORMATION_CLASS TokenInformationClass = TokenElevationType;
  TOKEN_ELEVATION_TYPE ElevationType;
  DWORD ReturnLength;

  if( ! GetTokenInformation(hToken, TokenInformationClass, &ElevationType,
      sizeof(ElevationType), &ReturnLength) )
  {
    CloseHandle(hToken);
    CloseHandle(hProcess);
    return -1;
  }

  CloseHandle(hToken);
  CloseHandle(hProcess);
  return ElevationType;
}
//---------------------------------------------------------------------------
String GetApplicationExeName(void)
{
  String rv = Application->ExeName;
  // Удалим мусор из пути ".\" - он неадекватно интерпретируется некоторыми
  // Откуда он вообще берется ?
  int pos = rv.Pos("\\.\\");
  if( pos>0 )
	rv.Delete(pos, 2);
  return rv;  
}
//---------------------------------------------------------------------------
String DateTimeToISO8601(TDateTime v)
{
  return FormatDateTime("yyyymmdd'T'hhnnss.zzz", v) + "0";
}
//---------------------------------------------------------------------------
// s = "yyyy[mm[dd[Thh[nn[ss[.zzzz]]]]]]"
TDateTime DateTimeFromISO8601(String s)
{
  int l = s.Length();
  unsigned short year, month=0, day=0;
  unsigned short hour=0, min=0, sec=0, msec=0;

  if( l<4 )
    throw Exception(AnsiString("DateTimeFromISO8601(): неверная дата время: ") + s);

  year = StrToInt(s.SubString(1,4));
  if( l>=6 ) month = StrToInt(s.SubString(5,2));
  if( l>=8 ) day = StrToInt(s.SubString(7,2));
  if( l>=11 )
    if( s[9]=='T' )
    {
      hour = StrToInt(s.SubString(10,2));
      if( l>=13 ) min = StrToInt(s.SubString(12,2));
      if( l>=15 ) sec = StrToInt(s.SubString(14,2));
      if( l>=20 ) if( s[16]=='.' ) msec = StrToInt(s.SubString(17,3));
    }
  return TDateTime(year, month, day) + TDateTime(hour, min, sec, msec);
}
//---------------------------------------------------------------------------
bool StringStartWith(String Str, String StartStr)
{
  int l = StartStr.Length();
  int l1 = Str.Length();
  if( l1 < l )
    return false;
  for(int i=1; i<=l; i++)
    if( Str[i] != StartStr[i] )
      return false;
  return true;
}
//---------------------------------------------------------------------------
String GetTemporaryFileName(void)
{
  char path[MAX_PATH]=".";
  char name[MAX_PATH]="xxx";

  GetTempPath(sizeof(path),path);
  GetTempFileName(path,"TMP",0,name);

  return String(name);
}
//---------------------------------------------------------------------------
char * szSecurePassword = "Y_6IZ!h7IYb4QgE";
AnsiString SecurePassword(AnsiString password)
{
  WORD l = password.Length();
  int size = MAX(25, l+2);
  BYTE * data = new BYTE[size];

  Randomize();
  for(int i=2; i<size; i++)
    data[i] = (BYTE)(Random()*256.0);

  *(WORD *)data = l;
  strcpy(data+2, password.c_str());

  for(int i=0, j=0; i<size; i++)
  {
    data[i] ^= (BYTE)szSecurePassword[j++];
    if( ! szSecurePassword[j] )
      j = 0;
  }
  AnsiString rv = ToHexMas(data, size);
  delete [] data;
  return rv;
}
//---------------------------------------------------------------------------
AnsiString UnsecurePassword(AnsiString secure_password)
{
  WORD size = secure_password.Length() / 2;
  BYTE * data = new BYTE[size];
  AnsiString rv;
  if( FromHexMas(secure_password, data, size) )
  {
    for(int i=0, j=0; i<size; i++)
    {
      data[i] ^= (BYTE)szSecurePassword[j++];
      if( ! szSecurePassword[j] )
        j = 0;
    }
    WORD l = *(WORD *)data;
    rv = AnsiString((char *)(data+2), l);
  }
  delete [] data;
  return rv;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
