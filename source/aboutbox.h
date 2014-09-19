//---------------------------------------------------------------------------
#ifndef aboutboxH
#define aboutboxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TAboutBoxForm : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel3;
    TBevel *Bevel2;
    TLabel *Label;
    TImage *ProgramIcon;
    TLabel *Comments;
    TBitBtn *OKButton;
        TLabel *Label1;
        void __fastcall LabelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TAboutBoxForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutBoxForm *AboutBoxForm;
//---------------------------------------------------------------------------
#endif
