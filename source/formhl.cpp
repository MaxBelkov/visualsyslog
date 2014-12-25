//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
#include "messhl.h"
#include "saveini.h"     // TSaveParamsINI
#include "cfg.h"         // program config
#include "gr.h"          // 3D
#include "formhl.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "messmatchframe"
#pragma link "messstyleframe"
#pragma resource "*.dfm"

extern TSaveParamsINI * AppParams;
extern String HighlightFile;
extern TMainCfg MainCfg;
int THighlightForm::LastRuleIndex = 0;
THighlightForm * HighlightForm;
//---------------------------------------------------------------------------
__fastcall THighlightForm::THighlightForm(TComponent* Owner, int CurrentProfile,
  int dummy) : TForm(Owner)
{
  localHPL = new THighlightProfileList;
  localHPL->Load(HighlightFile);

  TMessHighlightList * p;

  ProfileCB->Items->BeginUpdate();
  for(int i=0; i<localHPL->Count; i++)
  {
    p = (TMessHighlightList *)localHPL->Items[i];
    ProfileCB->Items->Add(p->ProfileName);
  }
  ProfileCB->ItemIndex = CurrentProfile;
  ProfileCB->Items->EndUpdate();

  FillRuleList(LastRuleIndex);

  *AppParams >> this;
  DrawGrid_LivingColumns = new TStringGridLivingColumns((TStringGrid *)DrawGrid);

  MessMatchFr->OnValuesChange = OnFrameValuesChange;
  MessStyleFr->OnValuesChange = OnFrameValuesChange;
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::FormDestroy(TObject *Sender)
{
  *AppParams << this;
  if( DrawGrid->Row > 0 )
    LastRuleIndex = DrawGrid->Row - 1;
  delete DrawGrid_LivingColumns;
  delete localHPL;
  localHPL = NULL;
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::ProfileCBSelect(TObject *Sender)
{
  FillRuleList(0);
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::FillRuleList(int SelectedIndex)
{
  localHPL->CurrentProfile = ProfileCB->ItemIndex;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    DrawGrid->RowCount = 1 + p->Count;
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
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::OKButtonClick(TObject *Sender)
{
  localHPL->Save(HighlightFile);
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::AddButtonClick(TObject *Sender)
{
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    TMessHighlight * mh = new TMessHighlight;
    p->Add(mh);
    DrawGrid->RowCount = 1 + p->Count;
    DrawGrid->Row = DrawGrid->RowCount - 1;
    DrawGrid->FixedRows = 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::DelButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 2 )
    return;
  int ARow = DrawGrid->Row - 1;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    p->Del(ARow);
    DrawGrid->RowCount = 1 + p->Count;
    DrawGrid->OnClick(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::UpButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 3 )
    return;
  if( DrawGrid->Row == 1 )
    return;
  int ARow = DrawGrid->Row - 1;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    p->Exchange(ARow, ARow - 1);
    DrawGrid->Row = DrawGrid->Row - 1;
    DrawGrid->Invalidate();
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::DownButtonClick(TObject *Sender)
{
  if( DrawGrid->RowCount < 3 )
    return;
  if( DrawGrid->Row == DrawGrid->RowCount-1 )
    return;
  int ARow = DrawGrid->Row - 1;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    p->Exchange(ARow, ARow + 1);
    DrawGrid->Row = DrawGrid->Row + 1;
    DrawGrid->Invalidate();
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::DrawGridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  TCanvas * c = ((TStringGrid *)Sender)->Canvas;
  String s;

  if( ARow==0 ) // Grid title
  {
    switch( ACol )
    {
      case 0: s = " Active"; break;
      case 1: s = " Match & Style preview"; break;
    }

    if( MainCfg.b3D )
    {
      FillVolume(c, Rect, c->Brush->Color);
      c->Brush->Style = bsClear;
    }

    int x = Rect.Left + 2;
    int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
    c->TextRect(Rect, x, y, s);
  }
  else
  {
    TMessHighlightList * p = localHPL->GetCurrentProfile();
    if( p )
    {
      TMessHighlight * mh = p->Get(ARow-1);
      if( mh )
      {
        if( ACol == 0 )
        {
          ImageList->Draw(c,
            Rect.Left + ((Rect.Right - Rect.Left - ImageList->Width) / 2),
            Rect.Top + ((Rect.Bottom - Rect.Top - ImageList->Height) / 2),
            mh->bEnable ? 0 : 1, true);
        }
        else // ACol == 1
        {
          s = String(" ") + mh->Match.GetDescription();
          /*
          if( State.Contains(gdSelected) ) // Selected line
          {
          }
          else
          {
          */
            c->Brush->Color = mh->Style.BackgroundColor;
            c->Font->Color = mh->Style.TextColor;
          //}

          mh->Style.SetFontStyle(c->Font);

          if( MainCfg.b3D )
          {
            FillVolume(c, Rect, c->Brush->Color);
            c->Brush->Style = bsClear;
          }

          int x = Rect.Left + 2;
          int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
          c->TextRect(Rect, x, y, s);
        }
      }
    }
  }

}
//---------------------------------------------------------------------------
// show params of current rule
void __fastcall THighlightForm::DrawGridClick(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
  {
    ActiveCB->Visible = false;
    MessMatchFr->Visible = false;
    MessStyleFr->Visible = false;
    return;
  }
  else
  {
    ActiveCB->Visible = true;
    MessMatchFr->Visible = true;
    MessStyleFr->Visible = true;
  }
  
  int ARow = DrawGrid->Row - 1;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    TMessHighlight * mh = p->Get(ARow);
    if( mh )
    {
      ActiveCB->OnClick = NULL;
      ActiveCB->Checked = mh->bEnable;
      ActiveCB->OnClick = OnFrameValuesChange;

      MessMatchFr->ToDialog(&mh->Match);
      MessStyleFr->ToDialog(&mh->Style);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::DrawGridDblClick(TObject *Sender)
{
  ActiveCB->Checked = ! ActiveCB->Checked;
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::OnFrameValuesChange(TObject *Sender)
{
  if( DrawGrid->Row < 1 )
    return;
  int ARow = DrawGrid->Row - 1;
  TMessHighlightList * p = localHPL->GetCurrentProfile();
  if( p )
  {
    TMessHighlight * mh = p->Get(ARow);
    if( mh )
    {
      mh->bEnable = ActiveCB->Checked;
      MessMatchFr->FromDialog(&mh->Match);
      MessStyleFr->FromDialog(&mh->Style);
      DrawGrid->Invalidate();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall THighlightForm::HelpButtonClick(TObject *Sender)
{
  ReportMess2(
  "- Rules in the list are processed from top to bottom\n"
  "- Match rule \"Text contains\" searches for text in all fields of the message: "
    "Time, IP, Host, Facility, Priority, Tag, Message"
  );
}
//---------------------------------------------------------------------------

