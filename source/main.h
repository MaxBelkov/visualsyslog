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
#include <ActnList.hpp>
#include <ToolWin.hpp> // Change TStringGrid columns width
#include "gridcolumns.h"

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
    TPopupMenu *ClipboardPM;
    TMenuItem *mCopyToClipboard;
    TLabel *Label2;
    TFontDialog *FontDialog;
    TImageList *ImageList;
    TStatusBar *StatusBar;
    TTimer *NetTimer;
    TMenuItem *N1;
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
    TToolButton *ToolButton8;
    TToolButton *ToolButton9;
    TToolButton *ToolButton10;
    TAction *aGotoNewLine;
    TMenuItem *Gotonewline1;
    TToolButton *ToolButton11;
    TAction *aCancelViewFile;
    TToolButton *ToolButton12;
    TMenuItem *Cancelviewfile1;
    TAction *aRunIE;
    TMenuItem *RunInternetExplorer1;
    TAction *aPing;
    TMenuItem *Ping1;
    TAction *aFilterByIP;
    TAction *aFilterByHost;
    TMenuItem *N4;
    TMenuItem *FilteringbyIP1;
    TMenuItem *FilteringbyHost1;
    TAction *aHighlightingSetup;
    TToolButton *ToolButton13;
    TPopupMenu *HighlightingProfilesPM;
    TMenuItem *HighlightingSetup1;
    TAction *aShowAlarms;
    TMenuItem *Showalarmswindow1;
    TAction *aProcessSetup;
    TToolButton *ToolButton14;
    TMenuItem *Messageprocessingsetup1;
    TComboBox *SelectFileCB;
    TLabel *MessMatchLabel;
    TSpeedButton *FilterButton;
    TSpeedButton *ClearFilterButton2;
    TLabel *Label1;
    TLabel *ViewFileInfoLabel;
    TAction *aDisplayFilter;
    TMenuItem *Displayfilter1;
    TAction *aFilterByTag;
    TMenuItem *FilteringbyTag1;
    TAction *aHideBars;
    TMenuItem *Hidetoolbars1;
    TAction *aRotatePrev;
    TAction *aRotateNext;
    TMenuItem *Viewpreviouspart1;
    TMenuItem *Viewnextpart1;
    TToolButton *ToolButton7;
    TToolButton *ToolButton15;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall mCopyToClipboardClick(TObject *Sender);
    void __fastcall LogSGDblClick(TObject *Sender);
    void __fastcall LogSGDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall FormDestroy(TObject *Sender);
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
    void __fastcall aRunIEExecute(TObject *Sender);
    void __fastcall aPingExecute(TObject *Sender);
    void __fastcall aFilterByIPExecute(TObject *Sender);
    void __fastcall aFilterByHostExecute(TObject *Sender);
    void __fastcall aHighlightingSetupExecute(TObject *Sender);
    void __fastcall aShowAlarmsExecute(TObject *Sender);
    void __fastcall aProcessSetupExecute(TObject *Sender);
    void __fastcall SelectFileCBSelect(TObject *Sender);
    void __fastcall ClearFilterButton2Click(TObject *Sender);
    void __fastcall aDisplayFilterExecute(TObject *Sender);
    void __fastcall aFilterByTagExecute(TObject *Sender);
    void __fastcall aHideBarsExecute(TObject *Sender);
    void __fastcall aRotatePrevExecute(TObject *Sender);
    void __fastcall aRotateNextExecute(TObject *Sender);

public:
  // Main grid
  TDrawGrid2 * LogSG;

private:	// User declarations
  // Change LogSG columns width
  TStringGridLivingColumns * LogSG_LivingColumns;

  TList * MessList;       // List of messages to dislpay

  BYTE * FileReadBuffer;  // buffer for file reading
  TFile in;               // current viewed syslog file
  DWORD SizeToRead;       // size to read from syslog file

  int FileNumber;   // Current viewed file number (TStorageFileList::GetByNumber)
  int FileRotationNumber; // Current viewed file rotation number
  String fFile;     // Current viewed file name
  bool bLive;       // Live view ? (yes by default)
  TMessMatch MessMatch; // Display filter

  ULONGLONG FileSize;   // Size of fFile when open
  DWORD ReadedSize; // Bytes read from fFile
  int TotalLines;   // Total lines readed from file

  DWORD LastBalloonShowTime;

  // View file mode
  bool ViewFileMode;
  String ViewFileName;
  String TmpViewFileName;

  bool bFirstTimerTick;

private:
  // Clear string grig
  // Set working file name to f
  // Read maxsize bytes from the end of file f and show in the string grid
  void __fastcall SetFile(String f);
  // Rotation
  friend void __fastcall PreRotateFile(String FileName);
  friend void __fastcall PostRotateFile(void);
  // Read protocol text file again
  // Called when filters changes
  void __fastcall RedrawProto(void);
  // Positioning to the new line in the text file
  void __fastcall GotoNewLine(void);
  // Clear string grig
  void __fastcall Clear(void);
  // Read the tail of the syslog file
  void __fastcall Read(bool bAllowAddVisibleLines);
  // Print captions of string grig and view file info
  void __fastcall UpdateDislpayInfo(void);
  // Set lines height when change font
  void __fastcall SetLinesHeight(void);
  // Initialization code
  // Called once when program start
  void __fastcall Init(bool _bLive);
  // Get file to view by number
  String GetFileName(int number);
  // apply MessMatch
  void ApplyFilter(void);

public:		// User declarations
  __fastcall TMainForm(TComponent* Owner);
  void __fastcall CreateGrid(void);

  // Change icons and hint (State: 0-ok 1-warning 2-error)
  void __fastcall TrayChangeIcon(int State);
  void __fastcall SetViewFileMode(bool b);

  // Get display message by index
  TSyslogMessage * __fastcall GetMessageByIndex(int i);

  void __fastcall FillProfilePopupMenu(void);
  void __fastcall ChangeProfileClick(TObject *Sender);

  void __fastcall UpdateRotationViewControl(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm * MainForm;

void AlarmShow(String s);

bool WriteToLogRawMessage(char * p);
// Write message to errors.txt & Print in the status bar
bool WriteToLogError(String fmt, ...);

// where = CSIDL_STARTUP
void __fastcall CreateShortcut(int where);
void __fastcall DeleteShortcut(int where);
bool __fastcall IsShortcutExist(int where);
//---------------------------------------------------------------------------
#endif

