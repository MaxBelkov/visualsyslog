//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
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
  FilterByPriorityCB->ItemIndex = p->Priority + 1;
  TextContainsCB1->ItemIndex = p->TextContains1 ? 0 : 1;
  TextEdit1->Text = p->Text1;
  TextContainsCB2->ItemIndex = p->TextContains2 ? 0 : 1;
  TextEdit2->Text = p->Text2;
  MatchCaseCB->Checked = p->MatchCase;
  bEnableValuesChange = true;
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
void __fastcall TMessMatchFr::MatchChange(TObject *Sender)
{
  if( bEnableValuesChange )
  {
    if( OnValuesChange )
      OnValuesChange(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMessMatchFr::InfoButtonClick(TObject *Sender)
{
  ReportMess2(
  "- All conditions are combined by a logical AND\n"
  "- Text search is performed in all the fields of the message: "
    "Time, IP, Host, Facility, Priority, Tag, Message\n"
  "- To match by IP address, specify the prefix I:\n"
  "- To match by Host, specify the prefix H:\n"
  "- To match by Facility, specify the prefix F:\n"
  "- To match by Tag, specify the prefix T:"
  );
}
//---------------------------------------------------------------------------

