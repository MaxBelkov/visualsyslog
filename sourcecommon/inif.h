//---------------------------------------------------------------------------
#ifndef inifH
#define inifH

#include "File.h"

//---------------------------------------------------------------------------
// Возможна запись/чтение строковых параметров, содержащих символы
// перевода строки "\r\n". Это реализуется следующей автозаменой:
// - при записи заменяем: "\r\n" на "\", а "\" на "\\"
// - при чтении заменяем: "\" на "\r\n", а "\\" на "\"
//---------------------------------------------------------------------------
class TMYIniFile: public TFile
{
private:
  AnsiString FileName;
  TStringList * list;     // Список строк где хранится .INI файл 
  bool bChange;           // Была произведена запись хоть 1-ой переменной
  String CurSection;      // Название текущей секции
  int CurSectionIndex;    // Индекс текущей секции в списке
  int CurParamIndex;      // Индекс текущего параметра в списке
  int IniError;           // Код ошибки:
                          // 0 - OK
                          // 1 - Can't open file
  bool bAnyError;         // true - любая ошибка (в том числе открытия файла на чтение)

private:
  // Поиск секции
  int Find(String Section, String Ident="", int from=0);
  bool IsSectionName(String Section);
  AnsiString SpecialReplace(AnsiString s);
  AnsiString SpecialReverseReplace(AnsiString s);

public:
  // bCreateNewFile - Создать новый чистый файл. Содержимое старого игнорируется.
  TMYIniFile(char * File, bool enableExceptions = false, bool bCreateNewFile = false);
  ~TMYIniFile();
  // Запишем все на диск
  bool UpdateFile(void);

  AnsiString ReadString(char * Section, char * Ident, char * def="");
  String ReadString(String Section, String Ident, char * def="");
  AnsiString ReadStringNOCR(char * Section, char * Ident, char * def="");

  int ReadInteger(char * Section, char * Ident, int def=0);
  int ReadInteger(String Section, String Ident, int def=0);

  bool ReadBool(char * Section, char * Ident, bool def=false);
  bool ReadBool(String Section, String Ident, bool def=false);

  double ReadFloat(char * Section, char * Ident, double def=0.0);
  double ReadFloat(String Section, String Ident, double def=0.0);

  void WriteString(char * Section, char * Ident, char * Value);
  void WriteString(String Section, String Ident, String Value);

  void WriteStringNOCR(char * Section, char * Ident, char * Value);

  void WriteInteger(char * Section, char * Ident, int Value);
  void WriteInteger(String Section, String Ident, int Value);

  void WriteBool(char * Section, char * Ident, bool Value);
  void WriteBool(String Section, String Ident, bool Value);

  void WriteFloat(char * Section, char * Ident, double Value);
  void WriteFloat(String Section, String Ident, double Value);

  bool SectionExists(const AnsiString Section);
  bool ValueExists(const AnsiString Section, const AnsiString Ident);

  int GetError(void) { return IniError; }
  bool operator ! () { return GetError()!=0; }
  __property bool Change = {read=bChange, write=bChange};
  __property bool AnyError = {read=bAnyError, write=bAnyError};
};
//---------------------------------------------------------------------------
//char * SkipStartCntlChars(char * str);
//---------------------------------------------------------------------------
#endif
