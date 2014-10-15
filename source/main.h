//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include "gridcolumns.h"
#include <ActnList.hpp>
#include <ToolWin.hpp> // Change TStringGrid columns width

//---------------------------------------------------------------------------
// override standart component class for preventing flicker
class TDrawGrid2 : public TDrawGrid
{
public:
  bool bAllowUpdate;

public:
  #pragma option push -w-inl
  __fastcall virtual TDrawGrid2(Classes::TComponent* AOwner);
  #pragma option pop
  virtual void __fastcall Update(void);
};
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TLabel *Label1;
    TEdit *FilterEdit;
    TPopupMenu *ClipboardPM;
    TMenuItem *N30;
    TLabel *Label2;
    TComboBox *FilterByPriorityCB;
    TFontDialog *FontDialog;
    TImageList *ImageList;
    TStatusBar *StatusBar;
    TTimer *NetTimer;
    TMenuItem *N1;
    TLabel *Label3;
    TEdit *FilterIgnoreEdit;
    TSpeedButton *ClearFilterButton;
    TImageList *TrayImageList;
    TPopupMenu *TrayPopupMenu;
    TMenuItem *mOpenMainForm;
    TMenuItem *N_1;
    TMenuItem *mExit;
    TTrayIcon *TrayIcon;
    TMenuItem *N2;
    TOpenDialog *OpenDialog;
    TMenuItem *N3;
    TImageList *ImageList32;
    TActionList *ActionList;
    TAction *aSetup;
    TMenuItem *Setup1;
    TAction *aFont;
    TMenuItem *Font1;
    TAction *aAbout;
    TMenuItem *About1;
    TAction *aViewFile;
    TMenuItem *Viewfile1;
    TAction *aMoreLines;
    TMenuItem *Readpreviouslines1;
    TAction *mOpenFilesLoc;
    TMenuItem *Openfileslocation1;
    TAction *aClear;
    TMenuItem *Clearscreen1;
    TAction *aExit;
    TMenuItem *erminate1;
    TToolBar *ToolBar;
    TToolButton *ToolButton1;
    TToolButton *ToolButton2;
    TToolButton *ToolButton3;
    TToolButton *ToolButton4;
    TToolButton *ToolButton5;
    TToolButton *ToolButton6;
    TToolButton *ToolButton7;
    TToolButton *ToolButton8;
    TToolButton *ToolButton9;
    TToolButton *ToolButton10;
    TAction *aGotoNewLine;
    TMenuItem *Gotonewline1;
    TToolButton *ToolButton11;
    TAction *aCancelViewFile;
    TToolButton *ToolButton12;
    TMenuItem *Cancelviewfile1;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall N30Click(TObject *Sender);
    void __fastcall LogSGDblClick(TObject *Sender);
    void __fastcall LogSGDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall ClearFilterButtonClick(TObject *Sender);
    void __fastcall FilterByPriorityCBSelect(TObject *Sender);
    void __fastcall FilterByPriorityCBDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall NetTimerTimer(TObject *Sender);
    void __fastcall mOpenMainFormClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall aSetupExecute(TObject *Sender);
    void __fastcall aFontExecute(TObject *Sender);
    void __fastcall aAboutExecute(TObject *Sender);
    void __fastcall aViewFileExecute(TObject *Sender);
    void __fastcall aMoreLinesExecute(TObject *Sender);
    void __fastcall mOpenFilesLocExecute(TObject *Sender);
    void __fastcall aClearExecute(TObject *Sender);
    void __fastcall aExitExecute(TObject *Sender);
    void __fastcall aGotoNewLineExecute(TObject *Sender);
    void __fastcall aCancelViewFileExecute(TObject *Sender);

private:	// User declarations
  TDrawGrid2 * LogSG;
  // Change LogSG columns width
  TStringGridLivingColumns * LogSG_LivingColumns;

  TList * MessList;       // List of messages to dislpay

  TFile in;               // syslog file
  DWORD SizeToRead;       // size to read from syslog file

  String fFile;     // Full file name
  bool bLive;       // Live view ? (yes by default)

  String fFilter;   // Text filter string
  String fFilterIgnore; // Text filter ignore string
  int ApplyFilter;  // Text filter timer in seconds

  DWORD FileSize;   // Size of fFile when open
  DWORD ReadedSize; // Bytes read from fFile
  int TotalLines;   // Total lines readed from file

  AnsiString proto_line; // Used in Read() function

  DWORD LastBalloonShowTime;

  // View file mode
  bool ViewFileMode;
  String ViewFileName;
  String TmpViewFileName;

private:
  // Clear string grig
  // Set working file name to f
  // Read maxsize bytes from the end of file f and show in the string grid
  void __fastcall SetFile(String f);
  // Read protocol text file again
  // Called when filters changes
  void __fastcall RedrawProto(void);
  // Positioning to the new line in the text file
  void __fastcall GotoNewLine(void);
  // Clear string grig
  void __fastcall Clear(void);
  // [Read the tail | Read new part] of the syslog file
  void __fastcall Read(bool bAllowAddVisibleLines);
  // Print captions of main form and string grig
  void __fastcall UpdateCaption(void);
  // Get line color by priority
  TColor GetLogRecordColor(int priority);
  // Set lines height when change font
  void __fastcall SetLinesHeight(void);
  // Initialization code
  // Called once when program start
  void __fastcall Init(bool _bLive, int _ProtoFormat);

public:		// User declarations
  __fastcall TMainForm(TComponent* Owner);
  void __fastcall CreateGrid(void);

  // Change icons and hint (State: 0-ok 1-warning 2-error)
  void __fastcall TrayChangeIcon(int State);
  void __fastcall TrayShowBallon(AnsiString Title, AnsiString Text, int State=0);
  void __fastcall SetViewFileMode(bool b);

  // Get display message by index
  TSyslogMessage * __fastcall GetMessageByIndex(int i);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;

bool WriteToLogRawMessage(char * p);
// Write message to errors.txt & Print in the status bar
bool WriteToLogError(String fmt, ...);

// where = CSIDL_STARTUP
void __fastcall CreateShortcut(int where);
void __fastcall DeleteShortcut(int where);
bool __fastcall IsShortcutExist(int where);
//---------------------------------------------------------------------------
#endif

