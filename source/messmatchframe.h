//---------------------------------------------------------------------------
#ifndef messmatchframeH
#define messmatchframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "messmatch.h"
#include <Buttons.hpp>
#include <CheckLst.hpp>

//---------------------------------------------------------------------------
class TMessMatchFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TCheckBox *MatchCaseCB;
    TLabel *Label3;
    TComboBox *FieldCB1;
    TComboBox *FieldCB2;
    TMemo *Memo1;
    TMemo *Memo2;
    TCheckListBox *PriorityLB;
    TCheckListBox *FacilityLB;
    TCheckBox *NotCB;
    TLabel *Label2;
    TLabel *Label4;
    TSpeedButton *CheckAllFacButton;
    TSpeedButton *UncheckAllFacButton;
    TSpeedButton *UncheckAllPriButton;
    TSpeedButton *CheckAllPriButton;
    void __fastcall MatchChange(TObject *Sender);
    void __fastcall CheckAllFacButtonClick(TObject *Sender);
    void __fastcall UncheckAllFacButtonClick(TObject *Sender);
    void __fastcall CheckAllPriButtonClick(TObject *Sender);
    void __fastcall UncheckAllPriButtonClick(TObject *Sender);
private:	// User declarations
    bool bEnableValuesChange;
public:
    TNotifyEvent OnValuesChange;
public:		// User declarations
    __fastcall TMessMatchFr(TComponent* Owner);
    void ToDialog(TMessMatch * p);
    void FromDialog(TMessMatch * p);
};
//---------------------------------------------------------------------------
extern PACKAGE TMessMatchFr *MessMatchFr;
//---------------------------------------------------------------------------
#endif
