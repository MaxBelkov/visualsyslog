//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
#include "messhl.h"
#include "saveini.h"     // TSaveParamsINI
#include "formprocess.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "messmatchframe"
#pragma link "messprocessframe"
#pragma resource "*.dfm"

extern TSaveParamsINI * AppParams;
extern String ProcessFile;
int TProcessForm::LastRuleIndex = 0;
TProcessForm * ProcessForm;
//---------------------------------------------------------------------------
__fastcall TProcessForm::TProcessForm(TComponent* Owner) : TForm(Owner)
{
  localPRL = new TMessProcessRuleList;
  localPRL->Load(ProcessFile);

  FillRuleList(LastRuleIndex);

  *AppParams >> this >> (TStringGrid *)DrawGrid;
  DrawGrid_LivingColumns = new TStringGridLivingColumns((TStringGrid *)DrawGrid);

  MessMatchFr->OnValuesChange = OnFrameValuesChange;
  MessProcessFr->OnValuesChange = OnFrameValuesChange;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::FormDestroy(TObject *Sender)
{
  *AppParams << this << (TStringGrid *)DrawGrid;
  if( DrawGrid->Row > 0 )
    LastRuleIndex = DrawGrid->Row - 1;
  delete DrawGrid_LivingColumns;
  delete localPRL;
  localPRL = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::FillRuleList(int SelectedIndex)
{
  DrawGrid->RowCount = 1 + localPRL->Count;
  if( DrawGrid->RowCount > 1 )
  {
    DrawGrid->FixedRows = 1;
    if( DrawGrid->RowCount > SelectedIndex + 1 )
      DrawGrid->Row = SelectedIndex + 1;
    else if( DrawGrid->RowCount >= 2 )
      DrawGrid->Row = 1;
  }
  DrawGrid->OnClick(this);
  DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::OKButtonClick(TObject *Sender)
{
  localPRL->Save(ProcessFile);
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::AddButtonClick(TObject *Sender)
{
  TMessProcessRule * p = new TMessProcessRule;
  localPRL->Add(p);
  DrawGrid->RowCount = 1 + localPRL->Count;
  DrawGrid->Row = DrawGrid->RowCount - 1;
  DrawGrid->FixedRows = 1;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::DelButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 2 )
    return;
  int ARow = DrawGrid->Row - 1;
  localPRL->Del(ARow);
  DrawGrid->RowCount = 1 + localPRL->Count;
  DrawGrid->OnClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::UpButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 3 )
    return;
  if( DrawGrid->Row == 1 )
    return;
  int ARow = DrawGrid->Row - 1;
  localPRL->Exchange(ARow, ARow - 1);
  DrawGrid->Row = DrawGrid->Row - 1;
  DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::DownButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 3 )
    return;
  if( DrawGrid->Row == DrawGrid->RowCount-1 )
    return;
  int ARow = DrawGrid->Row - 1;
  localPRL->Exchange(ARow, ARow + 1);
  DrawGrid->Row = DrawGrid->Row + 1;
  DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::DrawGridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  TCanvas * c = ((TStringGrid *)Sender)->Canvas;
  String s;

  if( ARow==0 ) // Grid title
  {
    switch( ACol )
    {
      case 0: s = " Active"; break;
      case 1: s = " Match"; break;
      case 2: s = " Action"; break;
    }
    int x = Rect.Left + 2;
    int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
    c->TextRect(Rect, x, y, s);
  }
  else
  {
    TMessProcessRule * p = localPRL->Get(ARow-1);
    if( p )
    {
      if( ACol == 0 )
      {
        ImageList->Draw(c,
          Rect.Left + ((Rect.Right - Rect.Left - ImageList->Width) / 2),
          Rect.Top + ((Rect.Bottom - Rect.Top - ImageList->Height) / 2),
          p->bEnable ? 0 : 1, true);
      }
      else if( ACol == 1 )
      {
        s = String(" ") + p->Match.GetDescription();

        int x = Rect.Left + 2;
        int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
        c->TextRect(Rect, x, y, s);
      }
      else // ACol == 2
      {
        s = String(" ") + p->Process.GetDescription();
        //if( State.Contains(gdSelected) ) // Selected line
        //{
        //}
        //else
        //{
          //c->Brush->Color = p->Style.BackgroundColor;
          //c->Font->Color = p->Style.TextColor;
        //}

        //p->Style.SetFontStyle(c->Font);

        int x = Rect.Left + 2;
        int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
        c->TextRect(Rect, x, y, s);
      }
    }
  }
}
//---------------------------------------------------------------------------
// show params of current rule
void __fastcall TProcessForm::DrawGridClick(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
  {
    ActiveCB->Visible = false;
    MessMatchFr->Visible = false;
    MessProcessFr->Visible = false;
    return;
  }
  else
  {
    ActiveCB->Visible = true;
    MessMatchFr->Visible = true;
    MessProcessFr->Visible = true;
  }

  int ARow = DrawGrid->Row - 1;
  TMessProcessRule * p = localPRL->Get(ARow);
  if( p )
  {
    ActiveCB->OnClick = NULL;
    ActiveCB->Checked = p->bEnable;
    ActiveCB->OnClick = OnFrameValuesChange;

    MessMatchFr->ToDialog(&p->Match);
    MessProcessFr->ToDialog(&p->Process);
  }
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::DrawGridDblClick(TObject *Sender)
{
  ActiveCB->Checked = ! ActiveCB->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::OnFrameValuesChange(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
    return;
  int ARow = DrawGrid->Row - 1;
  TMessProcessRule * p = localPRL->Get(ARow);
  if( p )
  {
    p->bEnable = ActiveCB->Checked;
    MessMatchFr->FromDialog(&p->Match);
    MessProcessFr->FromDialog(&p->Process);
    DrawGrid->Invalidate();
  }
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::HelpButtonClick(TObject *Sender)
{
  ReportMess2(
  "Each message always goes all the actions in the list\n"
  );
}
//---------------------------------------------------------------------------

