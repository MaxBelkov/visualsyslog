//---------------------------------------------------------------------------

#ifndef matchformH
#define matchformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "messmatchframe.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFilterForm : public TForm
{
__published:	// IDE-managed Components
    TMessMatchFr *MessMatchFr;
    TBitBtn *OKButton;
    TBitBtn *CancelButton;
private:	// User declarations
public:		// User declarations
    __fastcall TFilterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFilterForm *FilterForm;
//---------------------------------------------------------------------------
#endif
