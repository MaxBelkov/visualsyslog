//---------------------------------------------------------------------------
#ifndef messprocessframeH
#define messprocessframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include "messprocessing.h"
//---------------------------------------------------------------------------
class TMessProcessFr : public TFrame
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TCheckBox *IgnoreCB;
    TCheckBox *AlarmCB;
    TLabel *Label11;
    TEdit *AlarmMessEdit;
    TPopupMenu *InsertPopupMenu;
    TMenuItem *time1;
    TMenuItem *ip1;
    TMenuItem *host1;
    TMenuItem *facility1;
    TMenuItem *priority1;
    TMenuItem *tag1;
    TMenuItem *message1;
    TCheckBox *SoundCB;
    TEdit *SoundFileEdit;
    TLabel *Label2;
    TEdit *PlayCountEdit;
    TOpenDialog *OpenDialog;
    TSpeedButton *SelWavCommonButton;
    TSpeedButton *PlayButton;
    TCheckBox *SendMailCB;
    TCheckBox *RunProgCB;
    TEdit *ProgFileEdit;
    TSpeedButton *SpeedButton1;
    TCheckBox *ProgHideCB;
    TSpeedButton *InsertProgramButton;
    TSpeedButton *InsertAlarmMessButton;
    TCheckBox *SaveToFileCB;
    TEdit *RecipientEdit;
    TComboBox *SaveFileCB;
    TSpeedButton *SpeedButton2;
    void __fastcall InsertAlarmMessButtonClick(TObject *Sender);
    void __fastcall pmInsertClick(TObject *Sender);
    void __fastcall SelWavCommonButtonClick(TObject *Sender);
    void __fastcall PlayButtonClick(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall InsertProgramButtonClick(TObject *Sender);
    void __fastcall Change(TObject *Sender);
    void __fastcall RecipientEditEnter(TObject *Sender);
    void __fastcall RecipientEditExit(TObject *Sender);
    void __fastcall SpeedButton2Click(TObject *Sender);
private:	// User declarations
    TForm * GetForm(void);
    bool bEnableValuesChange;
    String DefaultRecipient;
public:
    TNotifyEvent OnValuesChange;
public:		// User declarations
    __fastcall TMessProcessFr(TComponent* Owner);
    void ToDialog(TMessProcess * p);
    void FromDialog(TMessProcess * p);
};
//---------------------------------------------------------------------------
extern PACKAGE TMessProcessFr *MessProcessFr;
//---------------------------------------------------------------------------
#endif
