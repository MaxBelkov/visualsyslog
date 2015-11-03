//---------------------------------------------------------------------------
/*
  Умное изменение ширины колонок вместе с изменением ширины TStringGrid
  Переопределяет OnResize формы - владелицы TStringGrid
  Переопределяет OnMouseUp TStringGrid

  В конструкторе формы:
    // Создаем объект строго ПОСЛЕ восстановления размеров формы и TStringGrid
    LivingColumns = new TStringGridLivingColumns(StringGrid1);

  В деструкторе формы:
    delete LivingColumns;
*/
#ifndef gridcolumnsH
#define gridcolumnsH

#include <Grids.hpp>
//---------------------------------------------------------------------------
class TStringGridLivingColumns
{
private:
  TStringGrid * SG;
  double * ColumnKoeff;
  int oldClientWidth;
 
  Classes::TNotifyEvent oldOnResize;
  TMouseEvent oldOnMouseUp;

private:
  // Запомнить коэффициент ширины каждой колонки относительно общей ширины колонок  
  void SaveColumnKoeff(void);
  // Изменить размеры колонок в соответствии с текущим ClientWidth
  void ResizeColumnWidth(void);
  // Дать суммарную ширину всех колонок
  int __fastcall GetAllColumnsWidth(void);
  // Отслеживание изменения размеров формы
  void __fastcall FormResize(TObject *Sender);
  // Отслеживание изменения ширины колонки
  void __fastcall MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);

public:
  TStringGridLivingColumns(TStringGrid * p);
  ~TStringGridLivingColumns();
};
//---------------------------------------------------------------------------
// Функция учитывает специфику VCL
bool HaveScrollBar(HWND hwnd);
//---------------------------------------------------------------------------
#endif
