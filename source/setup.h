//---------------------------------------------------------------------------
#ifndef setupH
#define setupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Grids.hpp>
#include "fileframe.h"
//---------------------------------------------------------------------------
class TSetupForm : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *OKButton;
    TBitBtn *CancelButton;
    TPageControl *PageControl;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TGroupBox *GroupBox1;
    TCheckBox *AutoStartCB;
    TGroupBox *GroupBox2;
    TLabel *Label3;
    TEdit *serverEdit;
    TLabel *Label4;
    TEdit *portEdit;
    TLabel *Label5;
    TEdit *usernameEdit;
    TLabel *Label6;
    TEdit *passwordEdit;
    TComboBox *sslComboBox;
    TLabel *Label7;
    TGroupBox *GroupBox3;
    TLabel *Label8;
    TEdit *senderEdit;
    TEdit *sender_nameEdit;
    TLabel *Label9;
    TLabel *Label10;
    TEdit *recipientEdit;
    TLabel *Label11;
    TEdit *subjectEdit;
    TMemo *messageMemo;
    TLabel *Label12;
    TLabel *Label13;
    TSpeedButton *SpeedButton1;
    TSpeedButton *SpeedButton2;
    TSpeedButton *SpeedButton3;
    TSpeedButton *SpeedButton4;
    TSpeedButton *InsertMemoButton;
    TPopupMenu *InsertPopupMenu;
    TMenuItem *time1;
    TMenuItem *ip1;
    TMenuItem *host1;
    TMenuItem *facility1;
    TMenuItem *priority1;
    TMenuItem *tag1;
    TMenuItem *message1;
    TSpeedButton *TestButton;
    TSpeedButton *InsertSubjectButton;
    TGroupBox *UdpGroupBox;
    TCheckBox *EnableUdpCB;
    TLabel *Label1;
    TComboBox *UdpInterfaceCB;
    TEdit *UdpPortEdit;
    TGroupBox *TcpGroupBox;
    TCheckBox *EnableTcpCB;
    TLabel *Label2;
    TComboBox *TcpInterfaceCB;
    TEdit *TcpPortEdit;
    TTabSheet *TabSheet3;
    TGroupBox *FilesGB;
    TSpeedButton *AddFileButton;
    TSpeedButton *DelFileButton;
    TDrawGrid *DrawGrid;
    TFileFr *FileFr;
    TGroupBox *GroupBox4;
    TCheckBox *D3CB;
    TGroupBox *GroupBox5;
    TCheckBox *WriteRawCB;
    TCheckBox *ReceiveUTF8CB;
    void __fastcall OKButtonClick(TObject *Sender);
    void __fastcall SmtpSelClick(TObject *Sender);
    void __fastcall pmInsertClick(TObject *Sender);
    void __fastcall InsertToMessageButtonClick(TObject *Sender);
    void __fastcall TestButtonClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall InsertSubjectButtonClick(TObject *Sender);
    void __fastcall AddFileButtonClick(TObject *Sender);
    void __fastcall DelFileButtonClick(TObject *Sender);
    void __fastcall DrawGridClick(TObject *Sender);
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
  TStorageFileList * localSFL;
  static int LastFileIndex;

public:
  bool bUdpRestart;
  bool bTcpRestart;
  static int LastTabIndex;
  
public:		// User declarations
    __fastcall TSetupForm(TComponent* Owner);
    bool FromInterface(TLetter * p);
    void ToInterface(TLetter * p);
    void __fastcall FillRuleList(int SelectedIndex);
    void __fastcall OnFrameValuesChange(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupForm *SetupForm;
//---------------------------------------------------------------------------
#endif
