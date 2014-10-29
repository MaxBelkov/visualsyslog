//---------------------------------------------------------------------------
#ifndef messstyleframeH
#define messstyleframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "messstyle.h"

//---------------------------------------------------------------------------
class TMessStyleFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TColorBox *TextColorBox;
    TLabel *Label1;
    TLabel *Label2;
    TColorBox *BackgroundColorBox;
    TCheckBox *BoldCB;
    TLabel *Label3;
    TCheckBox *ItalicCB;
    TCheckBox *UnderlineCB;
    void __fastcall StyleChange(TObject *Sender);
private:	// User declarations
    bool EnableStyleChange;
public:
    TNotifyEvent OnValuesChange;
public:		// User declarations
    __fastcall TMessStyleFr(TComponent* Owner);
    void ToDialog(TMessStyle * p);
    void FromDialog(TMessStyle * p);
    //void UpdatePreview(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TMessStyleFr *MessStyleFr;
//---------------------------------------------------------------------------
#endif
