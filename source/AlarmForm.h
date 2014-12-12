//---------------------------------------------------------------------------
#ifndef AlarmFormH
#define AlarmFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TShowAlarmForm : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *OKButton;
    TListBox *ListBox;
    TBitBtn *ClearButton;
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall ClearButtonClick(TObject *Sender);
    void __fastcall OKButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TShowAlarmForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TShowAlarmForm *ShowAlarmForm;
//---------------------------------------------------------------------------
#endif
