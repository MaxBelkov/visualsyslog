//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "setup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetupForm * SetupForm;
//---------------------------------------------------------------------------
__fastcall TSetupForm::TSetupForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
