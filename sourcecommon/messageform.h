/*
  В этом модуле определяется форма сообщения оператору.
  Форма создается динамически.
*/
//---------------------------------------------------------------------------
#ifndef messageformH
#define messageformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TMessageForm : public TForm
{
__published:	// IDE-managed Components
        TSpeedButton *OkButton;
		TSpeedButton *CloseButton;
    TGroupBox *GroupBox;
    TImage *Image;
    TMemo *Memo;
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall CloseButtonClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TMessageForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMessageForm *MessageForm;
//---------------------------------------------------------------------------
/*
 Показ окна с сообщением оператору
 Параметры:
   OwnerForm - указатель на родительскую форму. Если указать NULL, то окно
               будет помещено в панель задач
   Text - текст сообщения
   Caption - заголовок окна
   Modal - true будет выведено модальное окно
           false - немодальное (по умолчанию)
   Error - true будет показан значок "стоп" и одна кнопка "закрыть" (по умолчанию)
           false значка не будет и одна кнопка "ок"
   Position - задает положение окна на экране (по умолчанию по центру
           родительской форму)
*/
void __fastcall ShowMessage2(TComponent * OwnerForm,
                             String Text,
                             String Caption="Ошибка",
                             bool Modal=false,
                             bool Error=true,
                             TPosition Position=poOwnerFormCenter);

// Форматирование текстового сообщения об ошибке и вывод его на экран в модальную форму
void ReportError2(String fmt, ...);
// Форматирование текстовой строки и вывод ее на экран в модальную форму
void ReportMess2(String fmt, ...);

// Форматирование текстового сообщения об ошибке и вывод его на экран
// OwnerForm = Screen->ActiveForm | NULL
void ReportError3(bool bModal, TComponent * OwnerForm, String fmt, ...);
// Форматирование текстовой строки и вывод ее на экран
void ReportMess3(bool bModal, TComponent * OwnerForm, String fmt, ...);
//---------------------------------------------------------------------------
#endif

