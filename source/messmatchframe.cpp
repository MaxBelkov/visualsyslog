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
  OnValuesChange = NULL;
}
//---------------------------------------------------------------------------
void TMessMatchFr::ToDialog(TMessMatch * p)
{
  bEnableValuesChange = false;
  PriorityOperationCB->ItemIndex = p->OperationP;
  PriorityCB->ItemIndex = p->Priority + 1;
  FieldCB1->ItemIndex = p->Field1 * 2 + (p->Contains1 ? 0 : 1);
  TextEdit1->Text = p->Text1;
  FieldCB2->ItemIndex = p->Field2 * 2 + (p->Contains2 ? 0 : 1);
  TextEdit2->Text = p->Text2;
  MatchCaseCB->Checked = p->MatchCase;
  bEnableValuesChange = true;
}
//---------------------------------------------------------------------------
void TMessMatchFr::FromDialog(TMessMatch * p)
{
  p->OperationP = PriorityOperationCB->ItemIndex;
  p->Priority = PriorityCB->ItemIndex - 1;
  p->Field1 = FieldCB1->ItemIndex / 2;
  p->Contains1 = (FieldCB1->ItemIndex & 1) == 0;
  p->Text1 = TextEdit1->Text;
  p->Field2 = FieldCB2->ItemIndex / 2;
  p->Contains2 = (FieldCB2->ItemIndex & 1) == 0;
  p->Text2 = TextEdit2->Text;
  p->MatchCase = MatchCaseCB->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMessMatchFr::MatchChange(TObject *Sender)
{
  if( bEnableValuesChange )
  {
    if( OnValuesChange )
      OnValuesChange(this);
  }
}
//---------------------------------------------------------------------------

