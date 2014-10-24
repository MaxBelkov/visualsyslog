//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messmatchframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMessMatchFr *MessMatchFr;
//---------------------------------------------------------------------------
__fastcall TMessMatchFr::TMessMatchFr(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TMessMatchFr::ToDialog(TMessMatch * p)
{
  FilterByPriorityCB->ItemIndex = p->Priority + 1;
  TextContainsCB1->ItemIndex = p->TextContains1 ? 0 : 1;
  TextEdit1->Text = p->Text1;
  TextContainsCB2->ItemIndex = p->TextContains2 ? 0 : 1;
  TextEdit2->Text = p->Text2;
  MatchCaseCB->Checked = p->MatchCase;
}
//---------------------------------------------------------------------------
void TMessMatchFr::FromDialog(TMessMatch * p)
{
  p->Priority = FilterByPriorityCB->ItemIndex - 1;
  p->TextContains1 = TextContainsCB1->ItemIndex == 0;
  p->Text1 = TextEdit1->Text;
  p->TextContains2 = TextContainsCB2->ItemIndex == 0;
  p->Text2 = TextEdit2->Text;
  p->MatchCase = MatchCaseCB->Checked;
}
//---------------------------------------------------------------------------

