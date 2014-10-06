//---------------------------------------------------------------------------
#ifndef setupH
#define setupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TSetupForm : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *OKButton;
    TBitBtn *CancelButton;
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TEdit *UdpPortEdit;
    TCheckBox *AutoStartCB;
    TComboBox *UdpInterfaceCB;
    TCheckBox *EnableUdpCB;
    TCheckBox *EnableTcpCB;
    TLabel *Label2;
    TComboBox *TcpInterfaceCB;
    TEdit *TcpPortEdit;
    void __fastcall OKButtonClick(TObject *Sender);
private:	// User declarations
public:
  bool bUdpRestart;
  bool bTcpRestart;
public:		// User declarations
    __fastcall TSetupForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupForm *SetupForm;
//---------------------------------------------------------------------------
#endif
