//---------------------------------------------------------------------------
//#include <vcl.h>
#pragma hdrstop

#include "gridcolumns.h"

#define MinColumnWidth 30
//---------------------------------------------------------------------------
TStringGridLivingColumns::TStringGridLivingColumns(TStringGrid * p)
{
  SG = p;

  // «апас 20 дл€ возможного увеличени€ кол-ва колонок
  ColumnKoeff = new double[SG->ColCount + 20];
  SaveColumnKoeff();

  oldOnResize = ((TForm *)SG->Owner)->OnResize;
  ((TForm *)SG->Owner)->OnResize = FormResize;

  oldOnMouseUp = SG->OnMouseUp;
  SG->OnMouseUp = MouseUp;
}
//---------------------------------------------------------------------------
TStringGridLivingColumns::~TStringGridLivingColumns()
{
  ((TForm *)SG->Owner)->OnResize = oldOnResize;
  SG->OnMouseUp = oldOnMouseUp;

  delete [] ColumnKoeff;
  ColumnKoeff = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TStringGridLivingColumns::FormResize(TObject *Sender)
{
  ResizeColumnWidth();
  if( oldOnResize )
    oldOnResize(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TStringGridLivingColumns::MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  if( Button == mbLeft )
  {
    // —лишком узкие колонки не разрешаем
    for(int i=0, c=SG->ColCount; i<c; i++)
      if( SG->ColWidths[i] < MinColumnWidth )
        SG->ColWidths[i] = MinColumnWidth;

    SaveColumnKoeff();
  }
  if( oldOnMouseUp )
    oldOnMouseUp(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
int __fastcall TStringGridLivingColumns::GetAllColumnsWidth(void)
{
  int AllColWidth = 0;
  for(int i=0, c=SG->ColCount; i<c; i++)
    AllColWidth += SG->ColWidths[i];
  // ¬сегда предполагаем что ширина каждой колонки больше на 1 пиксель -
  // это предотвращает по€вление горизонтального скроллинга
  return AllColWidth + SG->ColCount;
}
//---------------------------------------------------------------------------
void TStringGridLivingColumns::SaveColumnKoeff(void)
{
  oldClientWidth = SG->ClientWidth;

  int AllColWidth = GetAllColumnsWidth();
  for(int i=0, c=SG->ColCount; i<c; i++)
    ColumnKoeff[i] = (double)SG->ColWidths[i] / (double)AllColWidth;
}
//---------------------------------------------------------------------------
void TStringGridLivingColumns::ResizeColumnWidth(void)
{
  int cw = SG->ClientWidth;
  // ќбходим лишний вызов OnResize при показе формы
  if( oldClientWidth == cw )
    return;

  // ѕомним, что при по€влении полосы верт. скрола
  // SG->ClientWidth уменьшитс€ на ее ширину
  // ѕервоначальна€ установка ширины колонок производитс€
  // на пустом гриде без полосы верт. скрола

  // ќставим место дл€ полосы верт. скрола,
  // если сейчас верт. скрола нет
  if( ! HaveScrollBar(SG->Handle) )
  {
    int ScrolBarWidth = GetSystemMetrics(SM_CXVSCROLL);
    cw -= ScrolBarWidth;
  }

  int AllColWidth = GetAllColumnsWidth();

  if( oldClientWidth > cw ) // идет уменьшение
  {
    oldClientWidth = cw;
    // ≈сли уменьшение не задевает колонки - не измен€ем их ширину
    if( AllColWidth < cw )
      return;
  }
  else if( oldClientWidth < cw ) // идет увеличение
  {
    oldClientWidth = cw;
    // ≈сли не все колонки влаз€т - не измен€ем их ширину
    if( AllColWidth > cw )
      return;
  }

  // »зменим ширину колонок
  for(int i=0, c=SG->ColCount; i<c; i++)
    SG->ColWidths[i] = ColumnKoeff[i] * (double)cw;
}
//---------------------------------------------------------------------------
bool HaveScrollBar(HWND hwnd)
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  BOOL bHaveScrollBar = GetScrollInfo(hwnd, SB_VERT, &si);
  if( ! bHaveScrollBar )
    return false;
  if( si.nMax == 0 || si.nMax == 100 )
    return false;
  return true;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
