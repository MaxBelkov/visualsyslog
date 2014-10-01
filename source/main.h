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
#include "gridcolumns.h" // Change TStringGrid columns width

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TLabel *Label1;
    TEdit *FilterEdit;
    TCheckBox *GotoNewMessCB;
    TStringGrid *LogSG;
    TPopupMenu *ClipboardPM;
    TMenuItem *N30;
    TLabel *Label2;
    TComboBox *FilterByPriorityCB;
    TSpeedButton *MoreButton;
    TFontDialog *FontDialog;
    TMenuItem *mFont;
    TMenuItem *mMoreMenu;
    TImageList *ImageList;
    TMenuItem *mClear;
    TStatusBar *StatusBar;
    TTimer *UdpTimer;
    TMenuItem *mOpenFileFolder;
    TMenuItem *mSetup;
    TMenuItem *mAbout;
    TMenuItem *N1;
    TSpeedButton *SpeedButton1;
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
    TMenuItem *mExit2;
    TOpenDialog *OpenDialog;
    TMenuItem *N3;
    TMenuItem *mViewFile;
    TSpeedButton *CancelViewButton;
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall N30Click(TObject *Sender);
    void __fastcall LogSGDblClick(TObject *Sender);
    void __fastcall LogSGDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall ClearFilterButtonClick(TObject *Sender);
    void __fastcall FilterByPriorityCBSelect(TObject *Sender);
    void __fastcall MoreButtonClick(TObject *Sender);
    void __fastcall FontButtonClick(TObject *Sender);
    void __fastcall FilterByPriorityCBDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall mClearClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall UdpTimerTimer(TObject *Sender);
    void __fastcall mOpenFileFolderClick(TObject *Sender);
    void __fastcall mSetupClick(TObject *Sender);
    void __fastcall mAboutClick(TObject *Sender);
    void __fastcall mOpenMainFormClick(TObject *Sender);
    void __fastcall mExitClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall mViewFileClick(TObject *Sender);
    void __fastcall CancelViewButtonClick(TObject *Sender);

private:	// User declarations
  // Change TStringGrid columns width
  TStringGridLivingColumns * LogSG_LivingColumns;

  TFile in;               // syslog file
  DWORD SizeToRead;       // size to read from syslog file
  int AddStringGridLines; // how many rows to add to sting grid

  String fFile;     // Full file name
  bool bLive;       // Live view ? (yes by default)

  String fFilter;   // Text filter string
  String fFilterIgnore; // Text filter ignore string
  int ApplyFilter;  // Text filter timer in seconds

  DWORD FileSize;   // Size of fFile when open
  DWORD ReadedSize; // Bytes read from fFile
  int TotalLines;   // Total lines
  int VisibleLines; // Lines visible with filter working

  AnsiString proto_line; // Used in Read() function

  DWORD LastBalloonShowTime;
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
  // Read/Read new part of the text file
  void __fastcall Read(void);
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

  // start udp server
  void ServerStart(void);
  // stop udp server
  void ServerStop(void);

  void UdpReceiveMessage(void);

  // Change icons and hint (State: 0-ok 1-warning 2-error)
  void __fastcall TrayChangeIcon(int State);
  void __fastcall TrayShowBallon(AnsiString Title, AnsiString Text, int State=0);
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

