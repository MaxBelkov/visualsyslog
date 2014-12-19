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
  GetPriorities(PriorityLB->Items);
  GetFacilities(FacilityLB->Items);
}
//---------------------------------------------------------------------------
void TMessMatchFr::ToDialog(TMessMatch * p)
{
  bEnableValuesChange = false;

  NotCB->Checked = p->bNot;

  for(int i=0; i<LOG_NPRIORITIES; i++)
    PriorityLB->Checked[i] = p->PriorityMask & (1<<i);
  for(int i=0; i<LOG_NFACILITIES; i++)
    FacilityLB->Checked[i] = p->FacilityMask & (1<<i);

  FieldCB1->ItemIndex = p->Field1 * 2 + (p->Contains1 ? 0 : 1);
  Memo1->Lines = p->Text1;
  FieldCB2->ItemIndex = p->Field2 * 2 + (p->Contains2 ? 0 : 1);
  Memo2->Lines = p->Text2;
  MatchCaseCB->Checked = p->MatchCase;
  bEnableValuesChange = true;
}
//---------------------------------------------------------------------------
void TMessMatchFr::FromDialog(TMessMatch * p)
{
  p->bNot = NotCB->Checked;

  p->PriorityMask = 0;
  for(int i=0; i<LOG_NPRIORITIES; i++)
    p->PriorityMask |= PriorityLB->Checked[i] ? (1<<i) : 0;
  p->FacilityMask = 0;
  for(int i=0; i<LOG_NFACILITIES; i++)
    p->FacilityMask |= FacilityLB->Checked[i] ? (1<<i) : 0;

  p->Field1 = FieldCB1->ItemIndex / 2;
  p->Contains1 = (FieldCB1->ItemIndex & 1) == 0;
  p->Text1->Assign(Memo1->Lines);
  p->Field2 = FieldCB2->ItemIndex / 2;
  p->Contains2 = (FieldCB2->ItemIndex & 1) == 0;
  p->Text2->Assign(Memo2->Lines);
  p->MatchCase = MatchCaseCB->Checked;

  p->DeleteEmptyLines();
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
void __fastcall TMessMatchFr::CheckAllFacButtonClick(TObject *Sender)
{
  for(int i=0; i<FacilityLB->Items->Count; i++)
    FacilityLB->Checked[i] = true;
  MatchChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMessMatchFr::UncheckAllFacButtonClick(TObject *Sender)
{
  for(int i=0; i<FacilityLB->Items->Count; i++)
    FacilityLB->Checked[i] = false;
  MatchChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMessMatchFr::CheckAllPriButtonClick(TObject *Sender)
{
  for(int i=0; i<PriorityLB->Items->Count; i++)
    PriorityLB->Checked[i] = true;
  MatchChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMessMatchFr::UncheckAllPriButtonClick(TObject *Sender)
{
  for(int i=0; i<PriorityLB->Items->Count; i++)
    PriorityLB->Checked[i] = false;
  MatchChange(Sender);
}
//---------------------------------------------------------------------------

