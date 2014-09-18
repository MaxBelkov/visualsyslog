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
    TSpeedButton *ClearFilterButton;
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

private:	// User declarations
  // Change TStringGrid columns width
  TStringGridLivingColumns * LogSG_LivingColumns;

  TFile in;      // syslog file
  DWORD maxsize; // Max syslog file size in bytes
  int AddStringGridLines; // how many rows to add to sting grid

  String fFile;     // Full file name
  bool bLive;       // Live view ? (yes by default)

  String fFilter;   // Text filter string
  int ApplyFilter;  // Text filter timer in seconds

  DWORD FileSize;   // Size of fFile when open
  DWORD ReadedSize; // Bytes read from fFile
  int TotalLines;   // Total lines
  int VisibleLines; // Lines visible with filter working

  AnsiString proto_line; // Used in Read() function

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

  void ReceiveMessage(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
bool WriteToLogRawMessage(char * p);
//---------------------------------------------------------------------------
#endif

