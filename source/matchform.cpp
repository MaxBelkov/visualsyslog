//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "matchform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "messmatchframe"
#pragma resource "*.dfm"
TFilterForm *FilterForm;
//---------------------------------------------------------------------------
__fastcall TFilterForm::TFilterForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
