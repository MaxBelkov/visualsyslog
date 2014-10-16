//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messmatchframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMessMatchFr *MessMatchFr;
//---------------------------------------------------------------------------
__fastcall TMessMatchFr::TMessMatchFr(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
