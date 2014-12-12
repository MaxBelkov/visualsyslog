//---------------------------------------------------------------------------
#ifndef formprocessH
#define formprocessH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include "gridcolumns.h"
#include "messmatchframe.h"
#include "messprocessframe.h"

class THighlightProfileList;
//---------------------------------------------------------------------------
class TProcessForm : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox2;
    TGroupBox *GroupBox4;
    TBitBtn *OKButton;
    TBitBtn *CancelButton;
    TDrawGrid *DrawGrid;
    TImageList *ImageList;
    TBitBtn *HelpButton;
    TSpeedButton *AddButton;
    TSpeedButton *DelButton;
    TSpeedButton *UpButton;
    TSpeedButton *DownButton;
    TScrollBox *ScrollBox1;
    TCheckBox *ActiveCB;
    TMessMatchFr *MessMatchFr;
    TMessProcessFr *MessProcessFr;
    void __fastcall AddButtonClick(TObject *Sender);
    void __fastcall DelButtonClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall OKButtonClick(TObject *Sender);
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall DrawGridClick(TObject *Sender);
    void __fastcall UpButtonClick(TObject *Sender);
    void __fastcall DownButtonClick(TObject *Sender);
    void __fastcall OnFrameValuesChange(TObject *Sender);
    void __fastcall DrawGridDblClick(TObject *Sender);
    void __fastcall HelpButtonClick(TObject *Sender);

private:	// User declarations
    TStringGridLivingColumns * DrawGrid_LivingColumns;
    TMessProcessRuleList * localPRL;
    static int LastRuleIndex;

public:		// User declarations
    __fastcall TProcessForm(TComponent* Owner);
    void __fastcall FillRuleList(int SelectedIndex);
};
//---------------------------------------------------------------------------
extern PACKAGE TProcessForm * ProcessForm;
//---------------------------------------------------------------------------
#endif
