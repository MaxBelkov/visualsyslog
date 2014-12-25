//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "File.h"

char CR[] = "\r\n\x0";
//---------------------------------------------------------------------------
__fastcall TFile::TFile(bool enableExceptions) :
  handle(NULL), Error(false), Exceptions( enableExceptions ), LastError(0), bytes(0)
{
}
//---------------------------------------------------------------------------
__fastcall TFile::TFile(char * FileName, DWORD Access, DWORD ShareMode,
  DWORD CreationDistribution, DWORD FlagsAndAttributes, bool enableExceptions) :
  handle(NULL),
  Error(false),
  Exceptions( enableExceptions ),
  LastError(0),
  bytes(0)
{
  Open(FileName, Access, ShareMode, CreationDistribution, FlagsAndAttributes);
}
//---------------------------------------------------------------------------
__fastcall TFile::TFile(String FileName, DWORD Access, DWORD ShareMode,
  DWORD CreationDistribution, DWORD FlagsAndAttributes, bool enableExceptions) :
  handle(NULL),
  Error(false),
  Exceptions( enableExceptions ),
  LastError(0),
  bytes(0)
{
  Open(FileName, Access, ShareMode, CreationDistribution, FlagsAndAttributes);
}
//---------------------------------------------------------------------------
__fastcall TFile::~TFile()
{
  Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TFile::Open(char * FileName, DWORD Access, DWORD ShareMode,
                            DWORD CreationDistribution, DWORD FlagsAndAttributes)
{
  if( handle ) return false;
  LastError = 0;
  HANDLE h = CreateFile(FileName, Access, ShareMode, NULL,
                        CreationDistribution, FlagsAndAttributes, NULL);
  Error = (h == INVALID_HANDLE_VALUE);
  if( Error ) LastError = GetLastError();
  else handle = h; // Все ОК !

  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::Open(String FileName, DWORD Access, DWORD ShareMode,
                            DWORD CreationDistribution, DWORD FlagsAndAttributes)
{
  return Open(AnsiString(FileName).c_str(), Access, ShareMode,
              CreationDistribution, FlagsAndAttributes);
}
//---------------------------------------------------------------------------
bool __fastcall TFile::Close(void)
{
  if( ! handle ) return false;
  LastError = 0;
  Error = ! CloseHandle( handle );
  if( Error ) LastError = GetLastError();
  handle = NULL;
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::IsOpen(void)
{
  return (handle==NULL || handle==INVALID_HANDLE_VALUE) ? false : true;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::Write(BYTE * data, DWORD size)
{
  bytes = 0;
  if( ! handle || !data )
  {
    Error = true;
    if( Exceptions && Error ) throw 0;
    return false;
  }
  LastError = 0;
  Error = ! (WriteFile(handle, data, size, &bytes, NULL) && bytes == size);
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::Read(BYTE * data, DWORD size)
{
  bytes = 0;
  if( ! handle || !data )
  {
    Error = true;
    if( Exceptions && Error ) throw 0;
    return false;
  }
  LastError = 0;
  Error = ! (ReadFile(handle, data, size, &bytes, NULL) && bytes == size);
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::Flush(void)
{
  if( ! handle ) return false;
  LastError = 0;
  Error = ! FlushFileBuffers(handle);
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetPointer(LONG Dist, DWORD MoveMethod)
{
  if( ! handle )
  {
    Error = true;
    if( Exceptions && Error ) throw 0;
    return false;
  }
  LastError = 0;
  Error = SetFilePointer(handle, Dist, NULL, MoveMethod) == INVALID_SET_FILE_POINTER;
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetPointer(LONG Dist)
{
  return SetPointer(Dist, FILE_BEGIN);
}
//---------------------------------------------------------------------------
LONG __fastcall TFile::GetPointer(void)
{
  if( ! handle ) return NULL;
  LastError = 0;
  DWORD i = SetFilePointer(handle, 0, NULL, FILE_CURRENT);
  Error = (i == 0xFFFFFFFF);
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return i;
}
//---------------------------------------------------------------------------
DWORD __fastcall TFile::GetSize(void)
{
  if( ! handle ) return NULL;
  LastError = 0;
  Error = false;
  DWORD i = GetFileSize(handle, NULL);
  if( i == INVALID_FILE_SIZE )
  {
    Error = true;
    i = 0;
  }
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return i;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetPointer64(LONGLONG Dist, DWORD MoveMethod)
{
  if( ! handle )
  {
    Error = true;
    if( Exceptions && Error ) throw 0;
    return false;
  }
  LastError = 0;
  LARGE_INTEGER li;
  li.QuadPart = Dist;
  DWORD dw = SetFilePointer(handle, li.LowPart, &li.HighPart, MoveMethod);
  Error = dw == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR;
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetPointer64(LONGLONG Dist)
{
  return SetPointer64(Dist, FILE_BEGIN);
}
//---------------------------------------------------------------------------
LONGLONG __fastcall TFile::GetPointer64(void)
{
  if( ! handle )
  {
    Error = true;
    if( Exceptions && Error ) throw 0;
    return false;
  }
  LastError = 0;
  LARGE_INTEGER li;
  li.QuadPart = 0;
  li.LowPart = SetFilePointer(handle, 0, &li.HighPart, FILE_CURRENT);
  Error = li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR;
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return li.QuadPart;
}
//---------------------------------------------------------------------------
ULONGLONG __fastcall TFile::GetSize64(void)
{
  if( ! handle ) return NULL;
  LastError = 0;
  Error = false;
  ULARGE_INTEGER li;
  li.LowPart = GetFileSize(handle, &li.HighPart);
  if( li.LowPart == INVALID_FILE_SIZE )
  {
    Error = true;
    li.QuadPart = 0ui64;
  }
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return li.QuadPart;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::ToStart(void)
{
  return SetPointer64(0i64, FILE_BEGIN);
}
//---------------------------------------------------------------------------
bool __fastcall TFile::ToEnd(void)
{
  return SetPointer64(0i64, FILE_END);
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetEnd(void)
{
  if( ! handle ) return false;
  LastError = 0;
  Error = ! SetEndOfFile( handle );
  if( Error ) LastError = GetLastError();
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::WriteString(char * string)
{
  Error = ! (Write((BYTE *)string, lstrlen(string)) && Write((BYTE *)CR, lstrlen(CR)));
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::WriteString(AnsiString string)
{
  return WriteString(string.c_str());
}
//---------------------------------------------------------------------------
bool __fastcall TFile::ReadString(char * data, DWORD size)
{
  if( ! handle ) return false;
  LONG pos = GetPointer();

  DWORD i;
  //bool found=false;
  int eol = 0;
  DWORD bytes;
  Error = ! ReadFile(handle, data, size-1, &bytes, NULL);
  if( !bytes ) { Error = true; goto End; }

  // Ищем хоть сколько-то хоть каких-то завершателей строки
  for(i=0; (i<size-1) && (i<bytes); i++)
  {
    if( data[i]=='\r' || data[i]=='\n' ) { if( ++eol>2 ) break; }
    else { if( eol>0 ) break; }
  }
  data[i]=0;
  SetPointer(pos + i);
End:;
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::ReadString(AnsiString & data, DWORD maxsize)
{
  data.SetLength(maxsize);
  return ReadString(data.c_str(), maxsize);
}
//---------------------------------------------------------------------------
DWORD __fastcall TFile::GetDosTime(void)
{
  DWORD dos = 0L;
  FILETIME CT, LAT, LWT, NORMAL;
  if( ! handle ) return dos;
  Error = ! GetFileTime(handle, &CT, &LAT, &LWT);
  FileTimeToLocalFileTime(&LWT, &NORMAL);

  //FileTimeToDosDateTime(&CT, ((LPWORD)&dos)+1, (LPWORD)&dos);
  //FileTimeToDosDateTime(&LAT, ((LPWORD)&dos)+1, (LPWORD)&dos);
  FileTimeToDosDateTime(&NORMAL, ((LPWORD)&dos)+1, (LPWORD)&dos);

  if( Exceptions && Error ) throw 0;
  return dos;
}
//---------------------------------------------------------------------------
bool __fastcall TFile::SetDosTime(DWORD t)
{
  //DWORD dos = 0L;
  FILETIME CT, LAT, LWT, NORMAL;
  if( ! handle ) return false;
  Error = ! GetFileTime(handle, &CT, &LAT, &LWT);
  DosDateTimeToFileTime((WORD)(t>>16), (WORD)t, &NORMAL);

  LocalFileTimeToFileTime(&NORMAL, &LWT);
  Error = ! SetFileTime(handle, &CT, &LAT, &LWT);

  if( Exceptions && Error ) throw 0;
  return true;
}
//---------------------------------------------------------------------------
// TFileTimeToDateTime уже есть в utils.cpp
// Количество 100 наносекундных тиков в сутках
#define _T_ONEDAY    864000000000.0
const ULONGLONG _cDelphiTime = ((21968297ui64)<<32) + 2836185088ui64;
double _TFileTimeToDateTime(ULONGLONG * t) // unsigned __int64
{
  return (double)(*t - _cDelphiTime) / (double)_T_ONEDAY;
}
//---------------------------------------------------------------------------
double TFile::GetModificationTime(void)
{
  FILETIME CT, LAT, LWT, LOCAL;
  if( ! handle ) return 0.0;
  Error = ! GetFileTime(handle, &CT, &LAT, &LWT);
  if( Exceptions && Error ) throw 0;
  if( Error ) return 0.0;

  Error = ! FileTimeToLocalFileTime(&LWT, &LOCAL);
  if( Exceptions && Error ) throw 0;
  if( Error ) return 0.0;

  return _TFileTimeToDateTime((ULONGLONG *)&LOCAL);
}
//---------------------------------------------------------------------------
bool GetFileModificationTime(AnsiString name, ULONGLONG & out)
{
  TFile in(name.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
  if( !in ) return false;
  FILETIME CT, LAT, LWT;
  if( ! GetFileTime(in.GetHandle(), &CT, &LAT, &LWT) )
    return false;
  if( ! FileTimeToLocalFileTime(&LWT, (FILETIME *)&out) )
    return false;
  return true;
}
//---------------------------------------------------------------------------
bool TFile::GetInfo(BY_HANDLE_FILE_INFORMATION * p)
{
  if( ! handle ) return false;
  Error = !::GetFileInformationByHandle(handle, p);
  if( Exceptions && Error ) throw 0;
  return ! Error;
}
//---------------------------------------------------------------------------
bool TFile::WriteHex(char * title, void * data, DWORD size)
{
  if( !data || size >= 200 ) return false;
  //
  char str[800];
  char tmp[201];
  DWORD i;

  strcpy(str, title);
  strcat(str, "  ");
  for(i=0; i<size; i++)
  {
    BYTE c = *((BYTE *)data+i);
    wsprintf(tmp, "%02X", c);
    strcat(str, tmp);
  }
  strcat(str, "  ");
  for(i=0; i<size; i++)
  {
    char c = *((char *)data+i);
    tmp[i] = (c>31) ? c : '.';
  }
  tmp[i] = 0;
  strcat(str, tmp);

  Error = !WriteString(str);
  if( Error )
  {
    if( Exceptions ) throw 0;
    return ! Error;
  }
  return true;
}
//---------------------------------------------------------------------------
bool IsPath(char * szName)
{
  if( !szName ) return false;
  int i = lstrlen(szName);
  for(i--; i>0; i--)
  {
    if( szName[i]==':' || szName[i]=='\\' ) return true;
  }
  return false;
}
//---------------------------------------------------------------------------
String TFile::GetErrorMessage(void)
{
  AnsiString rv;
  LPVOID lpMsgBuf;
  if( ! FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		LastError,
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
#pragma package(smart_init)

