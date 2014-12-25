//---------------------------------------------------------------------------
// Work with files in windows
// Version 3 2014-11-11
//---------------------------------------------------------------------------
#ifndef SaturnFileH
#define SaturnFileH

/*
-------------------------- Описание параметров ------------------------------
DWORD Access (доступ к файлу (возможно объединение по OR)):

0             - не для доступа (запрос к устройству)
GENERIC_READ  - на чтение
GENERIC_WRITE - на запись
-----------------------------------------------------------------------------
DWORD ShareMode (разделение файла (возможно объединение по OR)):

0                 - Файл не может разделяться
FILE_SHARE_DELETE - For Windows NT only
FILE_SHARE_READ   - Файл может быть открыт даже если кто-то его уже читает
FILE_SHARE_WRITE  - Файл может быть открыт даже если кто-то его уже пишет
-----------------------------------------------------------------------------
DWORD CreationDistribution (как открывать файл):

CREATE_NEW        - Creates a new file. The function fails if the specified file
                    already exists.
CREATE_ALWAYS     - Creates a new file. The function overwrites the file if it exists.
                    Existing file truncated to zero bytes.
OPEN_EXISTING     - Opens the file. The function fails if the file does not exist.
OPEN_ALWAYS       - Opens the file, if it exists. If the file does not exist,
                    the function creates the file as if dwCreationDistribution
                    were CREATE_NEW.
TRUNCATE_EXISTING - Opens the file. Once opened, the file is truncated so that
                    its size is zero bytes. The calling process must open the
                    file with at least GENERIC_WRITE access. The function fails
                    if the file does not exist.
-----------------------------------------------------------------------------
DWORD FlagsAndAttributes (атрибуты файла (возможно объединение по OR)):

FILE_ATTRIBUTE_NORMAL - используется обычно (см. Help [CreateFile])

FILE_FLAG_RANDOM_ACCESS   - будем хреначить файл произвольно (оптимизация кэширования)
FILE_FLAG_SEQUENTIAL_SCAN - будем хреначить файл последовательно
FILE_FLAG_DELETE_ON_CLOSE - атоматически удалить файл сразу по закрытии
FILE_FLAG_WRITE_THROUGH - не кэшировать запись
Запись с флагом FILE_FLAG_WRITE_THROUGH выполняется несколько десятков мсек !!!
-----------------------------------------------------------------------------
LONG Dist (может быть положительная и отрицательная):

DWORD MoveMethod (от чего перемещать указатель файла):
FILE_BEGIN   - от начала
FILE_CURRENT - от текущей позиции указателя
FILE_END     - от конца
*/
// Максимальная длина имени файла
#define MN 260
// Атрибуты обычного файла для чтения
#define READ_FILE           GENERIC_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL
// Атрибуты файла для записи в новый файл
#define WRITE_FILE_NEW      GENERIC_WRITE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL

class TFile
{
private:
  HANDLE handle;    //
  bool Error;       // Последняя операция завершилась с ошибкой ?
  bool Exceptions;  // Генерить иключение при ошибках ?
  DWORD LastError;  // GetLastError
  DWORD bytes;      // Байт прочитано/записано в последней операции

public:
  __fastcall TFile(bool enableExceptions = false);
  __fastcall TFile(char * FileName, DWORD Access, DWORD ShareMode,
                   DWORD CreationDistribution,
                   DWORD FlagsAndAttributes,
                   bool enableExceptions = false);
  __fastcall TFile(String FileName, DWORD Access, DWORD ShareMode,
                   DWORD CreationDistribution,
                   DWORD FlagsAndAttributes,
                   bool enableExceptions = false);

  __fastcall ~TFile();

public:
  bool __fastcall Open(char * FileName, DWORD Access, DWORD ShareMode,
                       DWORD CreationDistribution, DWORD FlagsAndAttributes);
  bool __fastcall Open(String FileName, DWORD Access, DWORD ShareMode,
                       DWORD CreationDistribution, DWORD FlagsAndAttributes);
  bool __fastcall Close(void);
  bool __fastcall IsOpen(void);

  bool __fastcall Write(BYTE * data, DWORD size);
  bool __fastcall Read(BYTE * data, DWORD size);

  bool __fastcall Flush(void);

  // Установить положение указателя в файле
  //   MoveMethod (от чего перемещать указатель файла):
  //   FILE_BEGIN   - от начала
  //   FILE_CURRENT - от текущей позиции указателя
  //   FILE_END     - от конца
  bool __fastcall SetPointer(LONG Dist, DWORD MoveMethod);
  // Установить положение указателя в файле относительно его начала
  bool __fastcall SetPointer(LONG Dist);
  // Получить положение указателя в файле
  LONG __fastcall GetPointer(void);
  // Получить размер файла
  DWORD __fastcall GetSize(void);

  bool __fastcall SetPointer64(LONGLONG Dist, DWORD MoveMethod);
  bool __fastcall SetPointer64(LONGLONG Dist);
  LONGLONG __fastcall GetPointer64(void);
  ULONGLONG __fastcall GetSize64(void);

  // set pointer to file begin
  bool __fastcall ToStart(void);
  // set pointer to file end
  bool __fastcall ToEnd(void);

  // Установить конец файла на текущей позиции указателя
  // (можно увеличивать и уменьшать файлы)
  // Файл должен быть открыт как GENERIC_WRITE
  bool __fastcall SetEnd(void);

  // Записать строку string и добавить "\r\n"
  bool __fastcall WriteString(char * string);
  bool __fastcall WriteString(AnsiString string);
  //bool __fastcall WriteString(String string);
  bool WriteHex(char * title, void * data, DWORD size);

  // Прочитать из файла строку (до \r\n)
  bool __fastcall ReadString(char * data, DWORD size);
  bool __fastcall ReadString(AnsiString & data, DWORD maxsize);

  DWORD __fastcall GetDosTime(void);
  bool __fastcall SetDosTime(DWORD t);

  // Получить последнее состояние файла
  bool __fastcall GetError(void) { return Error; }
  HANDLE __fastcall GetHandle(void) { return handle; }

  // Получить информацию о файле
  bool GetInfo(BY_HANDLE_FILE_INFORMATION * p);

  // The file handle must have GENERIC_READ access right
  double GetModificationTime(void);

  // получить сообщение об ошибке
  String GetErrorMessage(void);

public:
  __property LONG Pointer = {read=GetPointer, write=SetPointer };
  __property LONGLONG Pointer64 = {read=GetPointer64, write=SetPointer64 };
  __property DWORD Bytes = {read=bytes};

  __property DWORD ErrorCode = {read=LastError};
  __property String ErrorMessage = {read=GetErrorMessage};

public:
  bool operator ! () { return GetError(); }

  TFile & operator << (char * string) { Write((BYTE *)string, lstrlen(string)); return *this; }
  TFile & operator << (AnsiString string) { Write((BYTE *)string.c_str(), string.Length()); return *this; }
  TFile & operator << (char i) { Write((BYTE *)&i, sizeof(i)); return *this; }
  TFile & operator << (int i)  { Write((BYTE *)&i, sizeof(i)); return *this; }
  TFile & operator << (double i)  { Write((BYTE *)&i, sizeof(i)); return *this; }

  TFile & operator >> (char & i) { Read((BYTE *)&i, sizeof(i)); return *this; }
  TFile & operator >> (BYTE & i) { Read((BYTE *)&i, sizeof(i)); return *this; }
  TFile & operator >> (int & i) { Read((BYTE *)&i, sizeof(i)); return *this; }
};
//---------------------------------------------------------------------------
// szName является длинным именем файла ?
bool IsPath(char * szName);
// Получить в out время последнего изменения файла name
bool GetFileModificationTime(AnsiString name, ULONGLONG & out);
//---------------------------------------------------------------------------
extern char CR[];
//---------------------------------------------------------------------------
#endif
