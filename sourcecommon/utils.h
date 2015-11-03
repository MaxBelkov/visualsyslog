/*
  В этом модуле определяются различные функции общего назначения
 */
//---------------------------------------------------------------------------
#ifndef LMutilsH
#define LMutilsH

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ABS(n)     (((n) < 0) ? (-(n)) : (n))

#define MyOneDay  1.0
#define MyOneHour (1.0/24.0)
#define MyOneMin  (1.0/1440.0)
#define MyOneSec  (1.0/86400.0)
#define MyOneMSec (1.0/86400000.0)

#define ONE_KB 1024
#define ONE_MB (1024*1024)
#define ONE_GB (1024*1024*1024)

/* __BORLANDC__
 0x0550	- C++Builder 5.0 Enterprise
 0x0560	- C++Builder 6.0 Enterprise
 0x0593 - CodeGear 2007
 0x0643 - XE2
*/

#if __BORLANDC__ >= 0x0600
  // Unicode версии билдера
  #define __UTEXT(quote) L##quote      // r_winnt
#else
  #define __UTEXT(quote) quote         // r_winnt
#endif

// Для Unicode версий билдера работает TEXT("")
// но, только если char включен как unicode
// __UTEXT работает всегда

//---------------------------------------------------------------------------
// Удалить из строки s все: ' ' && '\t' && символы из строки del
// Причем, символы ' ' && '\t' заключенные между двойных кавычек не удаляются
// Отсеч комментарии: ;... (символ начала комментария задается в comment)
void ProcessString(char * s, char comment=';', char * del="\"");
// Удалить из строки s все символы, указанные в строке del
void DeleteCharactersFromString(char * s, char * del="\"");
// Заменить в строке s все символы, содержащиеся в строке source на символ dest
void ReplaceCharactersFromString(char * s, char * source, char dest);
// Заменить в строке s все символы from на символ to
void ReplaceCharacters(AnsiString & s, char from, char to);
// Определить: szName является полным путем c:\123 или URL \\comp\123 ?
bool IsFullPath(char * szName);
// Это файловая маска ? (содержит символы ? или *)
bool IsFileMask(char * szName);
// Удалить из имени файла name недопустимые символы
void DeleteBadCharsFromFileName(String & name);
// Сравнение строк без учета регистра
// Возвращает число совпадающих символов от начала строк
int strcmpfixedlen(char * str1, char * str2);
//int strcmpfixedlen(String str1, String str2);
// Подсчет кол-ва символов ' ' || '\t' в начале строки buf
int SkipSpace(char * buf);

// В проекте Danfoss Contour свои юникод версии этих функций
#if !defined(DANFOSS)
// Форматирование текстового сообщения об ошибке и вывод его на экран
void ReportError(char *fmt, ...);
// Форматирование текстовой строки и вывод ее на экран
void ReportMess(char *fmt, ...);
// Форматирование текстового запроса и вывод его на экран с кнопками "ДА" "НЕТ"
// Возврат: true  - если нажата кнопка "ДА"
//          false - если нажата кнопка "НЕТ"
bool ReportYesNo(char *fmt, ...);
// Запустить telnet.exe с параметром params
bool RunTelnet(AnsiString params);
#endif

// Добавить в конец szDir символ '\\' если там его уже нет
void TerminateDirectory(char * szDir);
// Добавить из конца szDir символ '\\' если там он есть
void UnTerminateDirectory(char * szDir);
// Сформатировать строку fmt и записать в конец файла szLogFile
// savetime определяет нужно ли добавить в начало строки текущее время
void WriteToLogTime(char *fmt, bool savetime, ...);

// У Danfoss Contour своя WriteToLog
#if !defined(DANFOSS)
void WriteToLog(char *fmt, ...);
#endif

//---------------------------------------------------------------------------
// Удаляет директорию и все поддиректории со всеми файлами !
bool DeleteDirectory(char * szPath, bool bDelTopDir=true);
// Копирует файлы из папки szFrom по маске szMask в папку szTo со всему поддиректориями
// Пути szFrom и szTo должны завершаться символом '\\'
bool CopyDirectory(char * szFrom, char * szMask, char * szTo);
//---------------------------------------------------------------------------
// для использования функции необходимо подключить к проекту файл shlobj.h
// lpszPathObj - путь к объекту (программе), для которого будет создаваться ярлык
// lpszWorkDir - рабочий каталог объекта (программы)
// lpszPathLink - полное имя файла ярлыка
// lpszDesc - комментарий
// lpszArgument - аргументы командной строки запускаемой программы
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszWorkDir,
                   LPSTR lpszPathLink, LPSTR lpszDesc,
                   LPSTR lpszArgument="");
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TMYIniFile;
// выравнивание по BYTE сделано для слвместимости с форматом карт LanMon 1
#pragma pack(push,1)
class TMyFont
{
public:
  TColor Color;
  int Height;
  char Name[32];
  TFontPitch Pitch;
  int PixelsPerInch;
  int Size;
  BYTE Style; // биты 1 2 4 8 {fsBold,fsItalic,fsUnderline,fsStrikeOut};

public:
  void Get(TFont *);
  void Set(TFont *);
  bool Load(TMYIniFile * f, char * Section);
  bool Save(TMYIniFile * f, char * Section);
};
#pragma pack(pop)
//-----------------------------------------------------------------
// Изменяет курсор мыщи на "песочные часы". Пример: TWaitCursor wait;
// Возвращает старый курсор при выходе объекта из области видимости
class TWaitCursor
{
public:
	TWaitCursor() : oldc((TCursor)Screen->Cursor)
	{ Screen->Cursor = (TCursor)crHourGlass; }
    ~TWaitCursor()  { Screen->Cursor = (TCursor)oldc; }

private:
    TCursor oldc;
};
//---------------------------------------------------------------------------
// Преобразовать шестнадцатеричной цифры hex в значение
BYTE HexVal(char hex);
// c - это шестнадцатеричная цифра ?
bool IsHexDigit(char c);
// Преобразовать значенеи num от 0 до 15 в символ шестнадцатеричной цифры от '0' до 'F'
char DigitToChar(BYTE num);
// Преобразовать два символа шестнадцатеричных цифр от '0' до 'F' в значение
BYTE CharToDigit(char ch, char cl);
// Преобразовать массив байт data[size] в шестнадцатеричную строку
AnsiString ToHexMas(void * data, int size);
// Преобразовать шестнадцатеричную строку s в массив байт data[size]
// Возврат: false если размера массива size недостаточно или
//          в строке встретился неверный символ
//          true если преобразование прошло успешно
bool FromHexMas(AnsiString s, void * data, int size);
//---------------------------------------------------------------------------
// Преобразовать интервал времени interval в строку для вывода на экран
String GetTimeString(TDateTime interval);
// Возврат: "день" or "дня" or "дней" в зав-ти от кол-ва дней days
String GetDaysName(DWORD days);
// Возврат: "час" or "часа" or "часов" в зав-ти от кол-ва часов hours
String GetHoursName(DWORD hours);

// Преобразовать кол-во байт в строку для вывода на экран
AnsiString GetBytesString(DWORD count);
String GetBytesStringEng(ULONGLONG count);
// Преобразовать интервал времени interval в строку для вывода на экран
// в формате: "Д ЧЧ:ММ:СС"
AnsiString TimeIntervalToString(TDateTime interval);
//---------------------------------------------------------------------------
// Проверка указанных размеров окна на правильность по
// отношению к текущему виртуальному рабочему столу
// Если координаты неверные - производится их исправление
bool CorrectFormRect(int & l, int & t, int & w, int & h);

// Восстановить положение формы f на рабочем столе из файла p
void LoadFormPosition(TMYIniFile * p, TForm * f);
// Сохранить положение формы f на рабочем столе в файле p
void SaveFormPosition(TMYIniFile * p, TForm * f);

// Восстановить положение формы f на рабочем столе и массив param[count] из файла szPositionFile
void LoadPosition(TForm * f, int * param=NULL, int count=NULL);
// Сохранить положение формы f на рабочем столе и массив param[count] в файле szPositionFile
void SavePosition(TForm * f, int * param=NULL, int count=NULL);
// --//--
void LoadPosition(TForm * f, double * param, int count);
// --//--
void SavePosition(TForm * f, double * param, int count);
// --//--
void LoadPosition(TForm * f, AnsiString * param, int count);
// --//--
void SavePosition(TForm * f, AnsiString * param, int count);
//---------------------------------------------------------------------------
//void ASSERT(bool b);
// Создать строку длиной size - копию s и вернуть указатель на нее
// Если size=0, то длина строки будет соотвествовать длине строки s
// После использования возвращенный укатель нужно удалить !
char * MyNewStr(char * s, int size=0);
// Запуск на выполнение программы с параметрами CmdLine
//   wShowWindow - задание положения окна для вызванной программы
//   bWait = false не ожидать завершения вызванной программы
//   bWait = true ждать завершения вызванной программы
//   pExitCode - код возврата вызванной программы (только для bWait = true)
//   szCurDir - опциональное задание текущей директории для запуска программы
//   pdwProcessId - идентификатор процесса вызванной программы (только для bWait = false)
bool RunProg(char * CmdLine, WORD wShowWindow=SW_NORMAL, bool bWait=false,
			 DWORD * pExitCode=NULL, char * szCurDir=NULL, DWORD * pdwProcessId=NULL);
// Запуск редактора Windows Paint для файла file
bool RunPaint(String file, String * error=NULL);
//---------------------------------------------------------------------------
#define MAX_LAST_OPEN_DOCS 10
// Добавить в файл настроек szIniFile имя файла документа szDocFile
void AddToLastOpenDocs(char * szIniFile, char * szDocFile);
// Добавить в меню массив пунктов меню mi[i] сохраненные пункты из файла szIniFile
void AddLastOpenDocsToMenu(char * szIniFile, TMenuItem ** mi);
// Преобразует вариантное значение или массив v в двоичный буфер buf размером size
// Возвращает реальный размер преобразованных данных
// Используется для сохранения вариантных значений в типизированных файлах
int VarToBinary(Variant v, BYTE * buf, int size);
// Расчет контрольной суммы массива длиной N по адресу P
BYTE Summa(void * P, WORD N);
// Преобразовать из BCD val в значение
BYTE FromBCD(BYTE val);
// Преобразовать в BCD из значения val
BYTE ToBCD(BYTE val);
// Преобразовать байт val в двоичное представление
AnsiString ByteToBinary(BYTE val);
// Извлечь из строки s слово № WordNumber (с 1)
// Разделителем слов в строке служит один из символов строки delim
AnsiString ExtractSubString(AnsiString s, int WordNumber, AnsiString delim=" \t\n\r");
//---------------------------------------------------------------------------
// Преобразовать строку русских букв source в строку латинских dest по правилам транслитерации
void Transliterate(char * source, char * dest);
// Установить бит № bit (с нуля) в массиве data в значение set
void SetBit(BYTE * data, int bit, bool set);
// Получить значение бита № bit (с нуля) в массиве data
bool GetBit(BYTE * data, int bit);
// Получить кол-во значений в v (так как v может быть массивом)
int VariantGetValuesCount(Variant v);
// Получает код последней ошибки GetLastError и сохраняет его в error
// Получает для error текстовое описание ошибки и возвращает
AnsiString FormatLastError(DWORD & error);
// Получает для error текстовое описание ошибки и возвращает
// error - код возвращаемый GetLastError
AnsiString FormatLastError2(DWORD error);
// Если s = "param=value" возвращает "value"
AnsiString __fastcall GetValueFromString(AnsiString s);
// Получает версию исполнимого файла File из его ресурса версии в переменные Major, Minor, Release, Build
// Возвращает false при ошибке и true при успехе
bool GetFileVersion(AnsiString File, WORD & Major, WORD & Minor, WORD & Release, WORD & Build);
// Преобразовывает IP адрес, закодированный в DWORD, в строку
AnsiString IPAddrToString(DWORD ip);
// Преобразовывает TDateTime t в __int64 FileTime
ULONGLONG TDateTimeToFileTime(double t);
// Преобразовывает __int64 FileTime t в TDateTime
double TFileTimeToDateTime(ULONGLONG t);
// Возвратить TRUE, если 'c' представляет собой один из символов строки "+-*^/=;(),&|<>% \t\r"
int iscodedelim(char c);
// Сформировать строку в соответствии с правилами PostgreSQL
AnsiString __fastcall MakePostgresString(AnsiString source, int maxlen=0);
// Открывает диспетчер служб и ожидает появления статуса службы szServiceName
// в течении таймаута to (в миллисекундах)
bool __fastcall WaitService(char * szServiceName, DWORD dwCurrentState, DWORD to=15000);
// Найти все файлы в директории dir по маске mask и поместить их короткие имена в p
void __fastcall FindFiles(TStrings * p, String dir, String mask);
//---------------------------------------------------------------------------
// Преобразование строки в значение double result
// Разделитель допускается '.' или ','
// Если в str - неверное число или пустая строка возвращает false
// Исключения не генерятся
bool MyStrToFloat(String str, double & result);
// То-же с генерацией исключений
void MyStrToFloatException(String str, double & result);
double MyStrToFloatException(String str);
// Исключения не генерятся
double MyStrToFloatDef(String str, double def);
// Преобразование числа v в строку, после точки digit цифр
String MyFloatToStr(double v, int digit);

// Преобразование номер дня недели dow в текстовое наименование дня недели
// dow как в DayOfWeek():
// 1 = Воскресенье 7 = Суббота (not ISO 8601 compliant)
// (8 = тоже Воскресенье для совместимости)
String GetDayOfWeekNameLong(int dow);

// Преобразование номера месяца 1-12 в текстовое наименование месяца
String GetMonthNameLong(int month);

// Преобразование даты времени dt в текстовую строку для вывода на экран
// Формат времени задается параметром _TimeFormat (пример "hh:mm:ss.zzz")
// Возвращаемое значение:
//   Если dt сегодня то возвращается только время по маске _TimeFormat
//   Если dt в текущем году то возвращается число название месяца и время по маске _TimeFormat
//   Во всех остальных случаях возвращается число название месяца год и время по маске _TimeFormat
#if __BORLANDC__ >= 0x0630
String DateTimeIntellektFormat(TDateTime dt, String _TimeFormat=FormatSettings.LongTimeFormat);
#else
String DateTimeIntellektFormat(TDateTime dt, String _TimeFormat=LongTimeFormat);
#endif
//---------------------------------------------------------------------------
// Корректное копирование однобайтовой строки в буфер обмена Windows
// Корректное в отличие от Clipboard()->SetTextBuf()
bool setClipboard(AnsiString & lines);
String __fastcall GetRecommendedPath(bool common, String folder, String def);
// Замена SameText, неработающей по умолчанию в Builder 2007
bool __fastcall MySameText(String s1, String s2);
// Преобразует source в строку для поиска в регулярных выражениях
// Выполняется экранирование спецсимволов
String FormatRegExpString(String source);
//---------------------------------------------------------------------------
struct colstring
{
  int val;
  char * string;
};
String GetColorNameRus(TColor c);
//---------------------------------------------------------------------------
void MySetLocalTime(TDateTime t);
// Получить текущий уровень прав (Windows Vista and up)
// Возвращает enum TOKEN_ELEVATION_TYPE:
//    -1 - ошибка GetTokenInformation (вероятно Windows version < Vista)
//    0  - ошибка WinAPI
//    1 (TokenElevationTypeDefault) - UAC выключен
//    2 (TokenElevationTypeFull)    - Run As Administrator
//    3 (TokenElevationTypeLimited) - UAC включен
int GetCurrentElevationType(void);
// Получить полный путь к исполняемому файлу программы
// Содержит правку глюка добавления в путь ".\\" под Windows Vista and up
String GetApplicationExeName(void);
//---------------------------------------------------------------------------
// Дата время в/из стандарт ISO 8601: "yyyy[mm[dd[Thh[nn[ss[.zzzz]]]]]]"
String DateTimeToISO8601(TDateTime v);
TDateTime DateTimeFromISO8601(String s);
// Строка Str начинается с StartStr ?
bool StringStartWith(String Str, String StartStr);
String GetTemporaryFileName(void);
//---------------------------------------------------------------------------
AnsiString SecurePassword(AnsiString password);
AnsiString UnsecurePassword(AnsiString secure_password);
//---------------------------------------------------------------------------
#endif
