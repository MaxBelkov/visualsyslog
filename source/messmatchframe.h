//---------------------------------------------------------------------------

#ifndef messmatchframeH
#define messmatchframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
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
private:	// User declarations
public:		// User declarations
    __fastcall TMessMatchFr(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMessMatchFr *MessMatchFr;
//---------------------------------------------------------------------------
#endif
