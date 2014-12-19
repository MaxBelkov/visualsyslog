//---------------------------------------------------------------------------
#ifndef formhlH
#define formhlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include "messmatchframe.h"
#include <ExtCtrls.hpp>
#include "messstyleframe.h"
#include "gridcolumns.h"

class THighlightProfileList;
//---------------------------------------------------------------------------
class THighlightForm : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TComboBox *ProfileCB;
    TLabel *Label1;
    TGroupBox *GroupBox2;
    TGroupBox *OKGroupBox;
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
    TMessStyleFr *MessStyleFr;
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
    void __fastcall ProfileCBSelect(TObject *Sender);
    void __fastcall DrawGridDblClick(TObject *Sender);
    void __fastcall HelpButtonClick(TObject *Sender);

private:	// User declarations
    TStringGridLivingColumns * DrawGrid_LivingColumns;
    THighlightProfileList * localHPL;
    static int LastRuleIndex;

public:		// User declarations
    __fastcall THighlightForm(TComponent* Owner, int CurrentProfile, int dummy=0);
    void __fastcall FillRuleList(int SelectedIndex);
};
//---------------------------------------------------------------------------
extern PACKAGE THighlightForm *HighlightForm;
//---------------------------------------------------------------------------
#endif
