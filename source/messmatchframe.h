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

//---------------------------------------------------------------------------
class TMessMatchFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TComboBox *PriorityCB;
    TLabel *Label1;
    TCheckBox *MatchCaseCB;
    TLabel *Label3;
    TComboBox *PriorityOperationCB;
    TComboBox *FieldCB1;
    TComboBox *FieldCB2;
    TMemo *Memo1;
    TMemo *Memo2;
    void __fastcall MatchChange(TObject *Sender);
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
