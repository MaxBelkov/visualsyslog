//---------------------------------------------------------------------------
#ifndef fileframeH
#define fileframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include "fdb.h"
//---------------------------------------------------------------------------
class TFileFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TLabel *Label11;
    TOpenDialog *OpenDialog;
    TEdit *FileEdit;
    TSpeedButton *SpeedButton2;
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall Change(TObject *Sender);
private:	// User declarations
    TForm * GetForm(void);
    bool bEnableValuesChange;
    String DefaultRecipient;
public:
    TNotifyEvent OnValuesChange;
public:		// User declarations
    __fastcall TFileFr(TComponent* Owner);
    void ToDialog(TStorageFile * p);
    void FromDialog(TStorageFile * p);
    void CanEdit(bool b);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileFr *FileFr;
//---------------------------------------------------------------------------
#endif
