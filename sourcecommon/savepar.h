/*
 В этом модуле определяется VCL класс TSaveParams
 Он служат базовым для классов TSaveParamsINI и TSaveParamsXML

 Автор Бельков М.

 Пример использования:

   TSaveParamsINI * p = new TSaveParamsINI;
   // Установка следующего свойства перед свойством File предотвращает
   // чтение содержимого INI файла 
   //p->CreateNewFile = true;
   // Указываем файл для работы
   // При этом происходит чтение файла
   // Можно вызывать повторно для другого файла
   // Если файл не указан - используется "params.ini" в папке программы
   p->File = "c:\\myfile.ini";

   // Добавление значений по названию
   p->CurrentSection = "setup"; // Установим название группы (секции)
   p->Values["bool_value"] = true;
   p->Values["int_value"] = 123;
   p->Values["string_value"] = "привет!";
   p->Values["double_value"] = 123.456;

   // Добавление значений по индексу (с нуля)
   p->CurrentSection = "group";
   p->ValuesByIndex[0] = "строка";

   // Добавление размеров и положения формы TForm и ширин колонок TStringGrid
   // Добавление производится в секцию с именем = имени формы
   *p << Form1 << StringGrid1;

   // Добавление значений через класс TParamValue
   TParamValue par1("par1", 123); // Создаем параметр с именем par1 значением 123
   TParamValue par2("par2"); // Создаем параметр с именем par2 без значения
   par2.value = 321;
   p->CurrentSection = "group";
   *p << par1 << par2;

   // Получение значений по названию
   p->CurrentSection = "setup";
   bool b = p->Values["bool_value"];
   int i = p->Values["int_value"];
   AnsiString s = p->Values["string_value"];
   // проверка на наличие параметра с именем "double_value"
   double d;
   Variant v = p->Values["double_value"];
   if( ! v.IsEmpty() )
     d = v;

   // Получение значений по индексу
   p->CurrentSection = "group";
   AnsiString s1 = p->ValuesByIndex[0];

   // Получение размеров и положения формы TForm и ширин колонок TStringGrid
   *p >> Form1 >> StringGrid1;

   // Получение значений через класс TParamValue
   p->CurrentSection = "group";
   *p >> par1 >> par2;

   p->Save(); // Немедленная запись на диск

   delete p; // деструктор пишет все на диск
*/
//---------------------------------------------------------------------------
#ifndef saveparH
#define saveparH

#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Forms.hpp>
#include <DBGrids.hpp>

//---------------------------------------------------------------------------
// Класс параметра
class TParamValue
{
public:
  AnsiString name;
  Variant value;

public:
  TParamValue(AnsiString n, Variant v=Variant());
  TParamValue(void);
  bool Empty(void);
};
//---------------------------------------------------------------------------
// Базовый класс для чтения и записи параметров
class TSaveParams : public TObject
{
protected:
  AnsiString fFile;
  bool fCreateNewFile;
  AnsiString fCurrentSection;
  int fMinimumColumnWidth;

private:
  // Эти функции работают с параметрами в текущей секции CurrentSection
  void __fastcall SetByName(AnsiString name, Variant value);
  Variant __fastcall GetByName(AnsiString name);
  void __fastcall SetByIndex(int index, Variant value);
  Variant __fastcall GetByIndex(int index);

public:
  // inFile: файл со значениями
  // inCreateNewFile: true-не читаем содержимое файла inFile
  //                  false-чтение содержимого файла inFile
  TSaveParams(AnsiString inFile, bool inCreateNewFile=false);
  //TSaveParams(void);

  // Добавление значения value параметра name в секцию Section
  void virtual Set(AnsiString Section, AnsiString name, Variant value) = 0;
  // Получение значения параметра name из секции Section
  // Если указанный параметр не найден метод IsEmpty() у возвращаемого Variant вернет true
  Variant virtual Get(AnsiString Section, AnsiString name, Variant defval=Variant()) = 0;

  // проверка существования секции Section
  bool virtual SectionExists(const AnsiString Section) = 0;
  // проверка существования значения name в секции Section
  bool virtual ValueExists(const AnsiString Section, const AnsiString name) = 0;

  // чтение значений из файла File
  bool virtual Load(AnsiString File) = 0;
  // запись значений в файл, из которого их прочитали
  bool virtual Save(void) = 0;

public:
  // Сохранение и восстановление положения и размеров формы
  void SetVcl(TForm * p);
  void GetVcl(TForm * p);

  // Сохранение положения формы
  void SetVclPos(TForm * p);
  void GetVclPos(TForm * p);

  // Сохранение и восстановление ширин колонок для разных компонентов
  void SetVcl(TStringGrid * p);
  void GetVcl(TStringGrid * p);
  void SetVcl(TDBGrid * p);
  void GetVcl(TDBGrid * p);
  void SetVcl(TListView * p);
  void GetVcl(TListView * p);
  // Объекты VCL
  void SetVcl(TEdit * p);
  void GetVcl(TEdit * p);
  void SetVcl(TCheckBox * p);
  void GetVcl(TCheckBox * p);

// Сохранение и восстановление Шрифтов для любых компонентов,
// у кого есть шрифт. В параметр Section передается имя родительского
// компонента, например p->Owner->Name, а в параметр Name - имя компонента
// для чего сохраняется и восстанавливается шрифт, например p->Name.
// А параметр Font - тот самый шрифт компонента, например p->Font.
  void SetFont(AnsiString Section, AnsiString Name, TFont* Font);
  void GetFont(AnsiString Section, AnsiString Name, TFont* Font);

  // Сохранение и восстановление одного параметра или массива параметров
  void SetVcl(TParamValue * p);
  void GetVcl(TParamValue * p);
  void SetMany(TParamValue * p, int count);
  void GetMany(TParamValue * p, int count);

  // Эти функции получают значение из текущей секции CurrentSection
  // и позволяют указывать значения по умолчанию
  Variant GetByNameDef(AnsiString name, Variant defval);
  Variant GetByIndexDef(int index, Variant defval);

  // Полезные операторы
public:
  TSaveParams & operator << (TForm * p);
  TSaveParams & operator >> (TForm * p);
  TSaveParams & operator < (TForm * p);
  TSaveParams & operator > (TForm * p);
  TSaveParams & operator << (TStringGrid * p);
  TSaveParams & operator >> (TStringGrid * p);
  TSaveParams & operator << (TDBGrid * p);
  TSaveParams & operator >> (TDBGrid * p);
  TSaveParams & operator << (TListView * p);
  TSaveParams & operator >> (TListView * p);
  TSaveParams & operator << (TEdit * p);
  TSaveParams & operator >> (TEdit * p);
  TSaveParams & operator << (TCheckBox * p);
  TSaveParams & operator >> (TCheckBox * p);
  TSaveParams & operator << (TParamValue & p);
  TSaveParams & operator >> (TParamValue & p);

  // Функции для легкого перехода с класса TIniFile
public:
  int ReadInteger(const AnsiString Section, const AnsiString Ident, int Default);
  AnsiString ReadString(const AnsiString Section, const AnsiString Ident, const AnsiString Default);
  double ReadFloat(const AnsiString Section, const AnsiString Ident, double Default);
  bool ReadBool(const AnsiString Section, const AnsiString Ident, bool Default);

  void WriteInteger(const AnsiString Section, const AnsiString Ident, int Value);
  void WriteString(const AnsiString Section, const AnsiString Ident, const AnsiString Value);
  void WriteFloat(const AnsiString Section, const AnsiString Ident, double Value);
  void WriteBool(const AnsiString Section, const AnsiString Ident, bool Value);

  // Свойства
__published:
  __property AnsiString File = {read=fFile, write=fFile};
  __property AnsiString CurrentSection = {read=fCurrentSection, write=fCurrentSection};
  __property bool CreateNewFile = {read=fCreateNewFile, write=fCreateNewFile};
  // Минимальная ширина колонки, которая устанавливается при восстановлении ее размера
  // по умолчанию 0, т.е. можно устанавливать любой размер колонки
  // Пример: установка MinimumColumnWidth в значение 10 гарантирут, что ширина колонки
  // не будет установлена менее 10
  __property int MinimumColumnWidth = {read=fMinimumColumnWidth, write=fMinimumColumnWidth};

  // Эти свойства работают с параметрами в текущей секции CurrentSection
  __property Variant Values[AnsiString name] = { read=GetByName, write=SetByName };
  __property Variant ValuesByIndex[int index] = { read=GetByIndex, write=SetByIndex };
};
//---------------------------------------------------------------------------
#endif

