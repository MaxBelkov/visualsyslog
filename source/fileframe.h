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
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFileFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox;
    TLabel *Label11;
    TOpenDialog *OpenDialog;
    TEdit *FileEdit;
    TSpeedButton *SpeedButton2;
    TRadioButton *RotationRB1;
    TRadioButton *RotationRB2;
    TRadioButton *RotationRB3;
    TEdit *SizeEdit;
    TComboBox *MultCB;
    TComboBox *MomentCB;
    TEdit *HourEdit;
    TLabel *Label1;
    TPanel *Panel1;
    TRadioButton *RenamingRB0;
    TRadioButton *RenamingRB1;
    TLabel *Label4;
    TEdit *CountEdit;
    TEdit *NewNameEdit;
    TLabel *HelpLabel;
    TUpDown *CountEditUD;
    TUpDown *HourEditUD;
    TUpDown *SizeEditUD;
    TLabel *Label2;
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall Change(TObject *Sender);
    void __fastcall HelpLabelClick(TObject *Sender);
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
    void SetDefaultFile(bool b);
    void RotationEnable(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileFr *FileFr;
//---------------------------------------------------------------------------
#endif
