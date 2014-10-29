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
    TLabel *Label2;
    TComboBox *FilterByPriorityCB;
    TLabel *Label1;
    TEdit *TextEdit1;
    TComboBox *TextContainsCB1;
    TCheckBox *MatchCaseCB;
    TLabel *Label3;
    TComboBox *TextContainsCB2;
    TEdit *TextEdit2;
    TSpeedButton *InfoButton;
    void __fastcall MatchChange(TObject *Sender);
    void __fastcall InfoButtonClick(TObject *Sender);
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
