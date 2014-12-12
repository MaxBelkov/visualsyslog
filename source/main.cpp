//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop

#include "sendmail.h"
#include "tcpcon.h"      // tcp server connections
#include "udpcon.h"      // udp server connections
#include "syslog.h"      // base syslog definitions
#include "server.h"      // parse syslog packets to TSyslogMessage
#include "utils.h"       // common functions
#include "messageform.h" // cool message box
#include "saveini.h"     // TSaveParamsINI
#include "file.h"        // work with files
#include "setup.h"
#include "aboutbox.h"
#include "cfg.h"         // program config
#include "messmatch.h"   // Message filtering
#include "messhl.h"      // Message highlighting profiles
#include "messprocessing.h" // Message processing
#include "formhl.h"      // Highlighting setup
#include "formprocess.h" // Process messages setup
#include "sound.h"       // play sound thread
#include "AlarmForm.h"   // show alarm message
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// Start size in bytes, to read from text file
#define StartSizeToRead (1024*1024)
// Max lines count in the string grid when receive messages
#define MaxGridLinesReceive 10000

#define SB(n) MainForm->StatusBar->Panels->Items[n]->Text

// save main form screen position and other params to .ini file
TSaveParamsINI * AppParams = NULL;

String ApplicationExeName;
String WorkDir;
String ErrorlogFile;
String MainCfgFile;
String HighlightFile;
String ProcessFile;

THighlightProfileList * HP = NULL;
TMessProcessRuleList * ProcessRules = NULL;

// syslog file
TFile syslogout;
String SyslogFile;

// raw file
TFile rawout;
String RawFile;

BYTE HiVer=0, LoVer=0, RelVer=0;
String GetVersionString(void);
String GetFullAppName(void);

bool bHideToTray;

TMainCfg MainCfg;
static TMessStyle DefaultMessStyle;

TMainForm * MainForm = NULL;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject * Sender)
{
  CreateGrid();
  ViewFileMode = false;
  LastBalloonShowTime = 0UL;
  ApplicationExeName = GetApplicationExeName();

  // Extract program version for resources
  WORD Major, Minor, Release, Build;
  if( GetFileVersion(ApplicationExeName, Major, Minor, Release, Build) )
  {
    HiVer = (BYTE)Major;
    LoVer = (BYTE)Minor;
    RelVer = (BYTE)Release;
  }

  if( bHideToTray )
    Application->ShowMainForm = false;

  char str[MAX_PATH];
  // CSIDL_COMMON_APPDATA   C:\Documents and Settings\All Users\Application Data
  // CSIDL_LOCAL_APPDATA    C:\Documents and Settings\username\Local Settings\Application Data (nonroaming)
  if( SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
					  NULL, SHGFP_TYPE_CURRENT, str) == S_OK )
  {
	WorkDir = IncludeTrailingPathDelimiter(str) + "visualsyslog\\";
    if( ! DirectoryExists(WorkDir) )
      if( ! CreateDir(WorkDir) )
        ReportError2("Error creating directory: \"%s\"", WorkDir.c_str());
  }

  SyslogFile = WorkDir + "syslog";
  RawFile = WorkDir + "raw";
  DeleteFile(RawFile);
  ErrorlogFile = WorkDir + "errors.txt";
  //MainCfgFile = WorkDir + "cfg.ini";
  MainCfgFile = WorkDir + "cfg.xml";
  HighlightFile = WorkDir + "highlight.xml";
  ProcessFile = WorkDir + "process.xml";

  MainCfg.Load(MainCfgFile);

  HP = new THighlightProfileList;
  HP->Load(HighlightFile);

  ProcessRules = new TMessProcessRuleList;
  ProcessRules->Load(ProcessFile);

  AppParams = new TSaveParamsINI(WorkDir + "params.ini");
  AppParams->MinimumColumnWidth = 10;

  MessList = new TList;
  MessList->Capacity = 10000;

  TalkingThread = new TSound(true);
  TalkingThread->Resume();

  if( UdpServerCreate() )
  {
    UdpServerStart();
  }

  if( TcpServerCreate() )
  {
    TcpServerStart();
  }

  Init(true, 1);
  SetFile(SyslogFile);

  Timer->Enabled = true;
  NetTimer->Enabled = true;

  TrayChangeIcon(0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Init(bool _bLive, int _ProtoFormat)
{
  bLive = _bLive;
  FilterTimer = 0;
  SizeToRead = StartSizeToRead;

  // For "non-life" protocol remove functions:
  //   "Goto new line"
  //   "Clear screen"
  if( ! bLive )
  {
    aGotoNewLine->Visible = false;
    aClear->Visible = false;
  }

  *AppParams >> this >> (TStringGrid *)LogSG;

  Variant v = AppParams->Values["GotoNewMess"];
  if( ! v.IsEmpty() )
	aGotoNewLine->Checked = v;

  v = AppParams->Values["TextFilter"];
  if( ! v.IsEmpty() )
  {
	FilterEdit1->Text = v;
    fFilter = v;
    FieldCB1->ItemIndex = AppParams->Values["Field1"];
  }
  v = AppParams->Values["TextFilter2"];
  if( ! v.IsEmpty() )
  {
	FilterEdit2->Text = v;
    fFilter2 = v;
    FieldCB2->ItemIndex = AppParams->Values["Field2"];
  }
  FilterByPriorityCB->ItemIndex = AppParams->Values["PriorityFilter"];
  UpdateFilterButton();

  String FontName = AppParams->Values["FontName"];
  if( FontName.Length() > 0 )
  {
    LogSG->Font->Name = FontName;
    LogSG->Font->Size = AppParams->Values["FontSize"];
  }
  HP->CurrentProfile = AppParams->Values["HighlighProfile"];

  FillProfilePopupMenu();
  SetLinesHeight();

  LogSG_LivingColumns = new TStringGridLivingColumns((TStringGrid *)LogSG);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  // Save all
  *AppParams << this << (TStringGrid *)LogSG;
  AppParams->Values["GotoNewMess"] = aGotoNewLine->Checked;

  AppParams->Values["TextFilter"] = FilterEdit1->Text;
  AppParams->Values["Field1"] = FieldCB1->ItemIndex;
  AppParams->Values["TextFilter2"] = FilterEdit2->Text;
  AppParams->Values["Field2"] = FieldCB2->ItemIndex;
  AppParams->Values["PriorityFilter"] = FilterByPriorityCB->ItemIndex;

  AppParams->Values["FontName"] = LogSG->Font->Name;
  AppParams->Values["FontSize"] = LogSG->Font->Size;
  AppParams->Values["HighlighProfile"] = HP->CurrentProfile;

  NetTimer->Enabled = false;
  Timer->Enabled = false;

  TcpServerStop();
  TcpServerDestroy();

  UdpServerStop();
  UdpServerDestroy();

  rawout.Close();
  syslogout.Close();

  if( TalkingThread )
  {
    TalkingThread->Stop();
    TalkingThread->Terminate();
    TalkingThread->WaitFor();
    delete TalkingThread;
    TalkingThread = NULL;
  }

  if( ShowAlarmForm )
  {
    delete ShowAlarmForm;
    ShowAlarmForm = NULL;
  }

  Clear();
  delete MessList;
  MessList = NULL;

  delete ProcessRules;
  ProcessRules = NULL;

  delete HP;
  HP = NULL;

  delete LogSG_LivingColumns;

  delete AppParams;
  AppParams = NULL;

  TSendmailThread::Exit();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CreateGrid(void)
{
  LogSG = new TDrawGrid2(MainForm);
  LogSG->Options >> goRangeSelect << goDrawFocusSelected
                 << goColSizing << goRowSelect << goThumbTracking;
  LogSG->Name = "LogSG";
  LogSG->AlignWithMargins = true;
  LogSG->Align = alClient;
  LogSG->ColCount = 7;
  LogSG->DefaultRowHeight = 20;
  LogSG->FixedCols = 0;
  LogSG->ParentFont = false;
  LogSG->PopupMenu = ClipboardPM;
  LogSG->OnDblClick = LogSGDblClick;
  LogSG->OnDrawCell = (TDrawCellEvent)&LogSGDrawCell;
  LogSG->Parent = GroupBox2;
  // set resonnable default column width for first run
  LogSG->ColWidths[0] = 110;
  LogSG->ColWidths[1] = 90;
  LogSG->ColWidths[2] = 90;
  LogSG->ColWidths[3] = 80;
  LogSG->ColWidths[4] = 80;
  LogSG->ColWidths[5] = 120;
  LogSG->ColWidths[6] = 520;

  ActiveControl = LogSG;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetFile(String f)
{
  TWaitCursor wait;

  in.Close();
  fFile = f;
  Clear();
  // important OPEN_ALWAYS
  in.Open(fFile, GENERIC_READ,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         OPEN_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL);
  if( !in )
    return;

  FileSize = in.GetSize64();
  // Since the beginning of the file, look at the size limit
  if( FileSize > (ULONGLONG)SizeToRead )
  {
    aMoreLines->Enabled = true;

    in.Pointer64 = FileSize - SizeToRead;
    // Positioning at the beginning of a new line in the file
    GotoNewLine();
  }
  else
  {
    aMoreLines->Enabled = false;
  }

  Read(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::GotoNewLine(void)
{
  char c;
  for(int i=0; i<1024; i++)
  {
    in >> c;
    if( c=='\n' ) break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aMoreLinesExecute(TObject *Sender)
{
  // Increase the amount of reading from a file
  SizeToRead = ReadedSize + StartSizeToRead;
  RedrawProto();
  ActiveControl = LogSG;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  if( fFilter != FilterEdit1->Text )
  {
    fFilter = FilterEdit1->Text;
    FilterTimer = 2;
  }
  if( fFilter2 != FilterEdit2->Text )
  {
    fFilter2 = FilterEdit2->Text;
    FilterTimer = 2;
  }
  else if( FilterTimer > 0 )
  {
    FilterTimer--;
    if( FilterTimer == 0 )
    {
      UpdateFilterButton();
      // Read protocol file again
      RedrawProto();
    }
  }

  // Check for new records, if the protocol is "live"
  if( bLive )
    if( in.IsOpen() )
      Read(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateCaption(void)
{
  String s = GetFullAppName() +
             "  [View file " + (ViewFileMode ? ViewFileName : fFile);
  // If the text file is not completely read
  if( aMoreLines->Enabled )
    s += " the last " + GetBytesStringEng(ReadedSize) +
         " of the " + GetBytesStringEng(FileSize);
  s += "]";
  if( Caption != s )
    Caption = s;

  int lc = MessList->Count;
  if( TotalLines != lc )
    GroupBox2->Caption = "Displaying " + IntToStr(lc) + " messages of " +
      IntToStr(TotalLines);
  else
    GroupBox2->Caption = "Displaying " + IntToStr(lc) + " messages";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Clear(void)
{
  for(int i=0, l=MessList->Count; i<l; i++)
    delete (TSyslogMessage *)MessList->Items[i];
  MessList->Clear();

  LogSG->RowCount = 2;

  TotalLines = 0;
  ReadedSize = 0;

  UpdateCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Read(bool bAllowAddVisibleLines)
{
  BYTE * p = new BYTE[SizeToRead];
  in.Read(p, SizeToRead);
  if( in.Bytes > 0 )
  {
    TSyslogMessage * sm;
    bool bHaveNewMessagesToDysplay = false;
    int DeletedMessagesCount = 0;

    TMessMatch MessMatch;
    MessMatch.OperationP = 0; // =
    MessMatch.Priority = FilterByPriorityCB->ItemIndex - 1;
    MessMatch.Field1 = FieldCB1->ItemIndex / 2;
    MessMatch.Contains1 = (FieldCB1->ItemIndex & 1) == 0;
    MessMatch.Text1->Text = FilterEdit1->Text;
    MessMatch.Field2 = FieldCB2->ItemIndex / 2;
    MessMatch.Contains2 = (FieldCB2->ItemIndex & 1) == 0;
    MessMatch.Text2->Text = FilterEdit2->Text;
    MessMatch.MatchCase = true;

    ReadedSize += in.Bytes;

    // It is possible that the text file is filled so quickly that
    // he read to the end and not at the end of the array p [] is to truncate
    // proto_line var is global, to save cut line part

    for(DWORD i=0, c=in.Bytes; i<c; i++)
    {
      if( p[i]=='\n' || p[i]=='\r' )
      {
        if( proto_line.Length() > 0 )
        {
          // Now proto_line contains new line
          TotalLines++;

          sm = new TSyslogMessage;
          sm->ProcessMessageFromFile(proto_line.c_str());

          if( ! MessMatch.Match(sm) )
          {
            // and the type does not match the required - continue
            proto_line.SetLength(0);
            delete sm;
            continue;
          }

          if( ! bAllowAddVisibleLines )
          {
            // Increasing the number of lines is not allowed
            if( MessList->Count >= MaxGridLinesReceive )
            {
              delete (TSyslogMessage *)MessList->Items[0];
              MessList->Delete(0);
              TotalLines--;
              DeletedMessagesCount++;
            }
          }
          
          MessList->Add(sm);
          bHaveNewMessagesToDysplay = true;

          proto_line.SetLength(0);
        }
        continue;
	  }
	  proto_line += (char)p[i];
	} // End for by protocol characters

    // Establish the actual number of lines (but not less than 2)
    LogSG->RowCount = MAX(2, MessList->Count + 1); // + 1 fixed line

    if( bHaveNewMessagesToDysplay )
    {
      if( DeletedMessagesCount > 0 )
        LogSG->Invalidate();

      // Goto new message ?
      if( aGotoNewLine->Checked )
      {
        // Yes
        LogSG->Row = LogSG->RowCount - 1;
      }
      else
      {
        // No: save cursor position and first visible line in view
        if( DeletedMessagesCount > 0 )
        {
          int r = LogSG->Row - DeletedMessagesCount;
          if( r < 1 )
            r = 1;
          int tr = LogSG->TopRow - DeletedMessagesCount;
          if( tr < 1 )
            tr = 1;

          LogSG->bAllowUpdate = false; // prevent flicker
          LogSG->Row = r;
          LogSG->TopRow = tr;
          LogSG->bAllowUpdate = true;
        }
      }
    }

    FileSize = in.GetSize64();
    UpdateCaption();
  }
  delete [] p;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mCopyToClipboardClick(TObject *Sender)
{
  String str;
  TSyslogMessage * p;
  for(int i=0, l=MessList->Count; i<l; i++)
  {
    p = (TSyslogMessage *)MessList->Items[i];
    str += p->ClipboardString();
  }
  setClipboard(str);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LogSGDblClick(TObject *Sender)
{
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( sm )
    ShowMessage2(this,
                 AnsiString("Time: ") + sm->DateStr +
                 "\nIP: " + sm->SourceAddr +
                 "\nHost: " + sm->HostName +
                 "\nFacility: " + sm->Facility +
                 "\nPriority: " + sm->Priority +
                 "\nTag: " + sm->Tag +
                 "\nMessage: " + sm->Msg,
                 "Message content",
                 true, // modal
                 false); // error
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LogSGDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  TCanvas * c = ((TStringGrid *)Sender)->Canvas;
  String s;
  if( ARow==0 ) // Grid title
  {
    switch( ACol )
    {
      case 0: s = " Time"; break;
      case 1: s = " IP"; break;
      case 2: s = " Host"; break;
      case 3: s = " Facility"; break;
      case 4: s = " Priority"; break;
      case 5: s = " Tag"; break;
      case 6: s = " Message"; break;
    }
  }
  else
  {
    TSyslogMessage * sm = GetMessageByIndex(ARow - 1);
    if( sm )
    {
      switch( ACol )
      {
        case 0: s = sm->DateStr; break;
        case 1: s = sm->SourceAddr; break;
        case 2: s = sm->HostName; break;
        case 3: s = sm->Facility; break;
        case 4: s = sm->Priority; break;
        case 5: s = sm->Tag; break;
        case 6: s = sm->Msg; break;
      }

      TMessStyle * pMessStyle = &DefaultMessStyle;
      TMessHighlightList * mhl = HP->GetCurrentProfile();
      if( mhl )
      {
        TMessStyle * ms = mhl->FindStyleForMessage(sm);
        if( ms )
          pMessStyle = ms;
      }

      if( State.Contains(gdSelected) ) // Selected line
      {
      }
      else
      {
        c->Brush->Color = pMessStyle->BackgroundColor;
        c->Font->Color = pMessStyle->TextColor;
      }

      pMessStyle->SetFontStyle(c->Font);
    }
  }
  int x = Rect.Left + 2;
  int y = Rect.Top + ((Rect.Bottom - Rect.Top - c->TextHeight(s)) / 2);
  c->TextRect(Rect, x, y, s);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearFilterButtonClick(TObject *Sender)
{
  FilterEdit1->Text = "";
  fFilter = "";

  FilterEdit2->Text = "";
  fFilter2 = "";

  FilterTimer = 0;

  FilterByPriorityCB->ItemIndex = 0;

  UpdateFilterButton();
  RedrawProto();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnApplyFilter(TObject *Sender)
{
  UpdateFilterButton();
  RedrawProto();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aFontExecute(TObject *Sender)
{
  FontDialog->Font = LogSG->Font;
  TFontStyles st = FontDialog->Font->Style; // save style
  if( ! FontDialog->Execute() )
    return;
  FontDialog->Font->Style = st; // restore style
  LogSG->Font = FontDialog->Font;
  SetLinesHeight();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetLinesHeight(void)
{
  int rh = LogSG->Font->Size + 6;
  LogSG->DefaultRowHeight = rh + rh / 2;
  FilterByPriorityCB->ItemHeight = 18;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aClearExecute(TObject *Sender)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NetTimerTimer(TObject *Sender)
{
  UdpReceiveMessage();

  // Accept new TCP connections
  TcpAccept();

  TcpReceiveMessage();
}
//---------------------------------------------------------------------------
bool WriteToLogRawMessage(char * p)
{
  if( ! rawout.IsOpen() )
  {
    rawout.Open(RawFile, GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         OPEN_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL);
    if( ! rawout )
      return false;
    rawout.ToEnd();
  }

  String s(p);
  s += szCR;
  rawout << s;
  return true;
}
//---------------------------------------------------------------------------
bool WriteToLogError(String fmt, ...)
{
  TFile out(ErrorlogFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
  if( ! out )
    return false;
  out.ToEnd();

  va_list argptr;
  va_start(argptr, fmt);
  String s;
  s.vprintf(fmt.c_str(), argptr);

  // Print in the status bar
  if( s.Pos("ERROR\t") > 0 )
    SB(2) = s.SubString(7, s.Length()-6);

  s = Now().DateTimeString() + '\t' + s + szCR;
  out << s;
  va_end(argptr);
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mOpenFilesLocExecute(TObject *Sender)
{
  ShellExecute(GetDesktopWindow(), "explore", WorkDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aSetupExecute(TObject *Sender)
{
  bool se = IsShortcutExist(CSIDL_STARTUP);

  SetupForm = new TSetupForm(this);
  SetupForm->AutoStartCB->Checked = se;

  if( SetupForm->ShowModal() == mrOk )
  {
    if( SetupForm->bUdpRestart )
    {
      UdpServerStop();
      UdpServerStart();
    }
    if( SetupForm->bTcpRestart )
    {
      TcpServerStop();
      TcpServerStart();
    }

    if( SetupForm->AutoStartCB->Checked != se )
    {
      if( se )
        DeleteShortcut(CSIDL_STARTUP);
      else
        CreateShortcut(CSIDL_STARTUP);
    }
    MainCfg.Save(MainCfgFile);
  }
  delete SetupForm;
  SetupForm = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RedrawProto(void)
{
  SetFile(fFile);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aAboutExecute(TObject *Sender)
{
  AboutBoxForm = new TAboutBoxForm(this);
  AboutBoxForm->ShowModal();
  delete AboutBoxForm;
}
//---------------------------------------------------------------------------
String GetVersionString(void)
{
  return IntToStr(HiVer) + "." + IntToStr(LoVer) + "." + IntToStr(RelVer);
}
//---------------------------------------------------------------------------
String GetFullAppName(void)
{
  return Application->Title + " " + GetVersionString();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mOpenMainFormClick(TObject *Sender)
{
  // open the application if it is minimized
  Application->Restore();

  // If the main form is minimized or hidden
  if( WindowState==wsMinimized )
    WindowState = wsNormal;
  else
    Show();

  // bring to the fore
  Application->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aExitExecute(TObject *Sender)
{
  Application->Terminate();
}
//---------------------------------------------------------------------------
// State: 0-ok 1-warning 2-error
void __fastcall TMainForm::TrayChangeIcon(int State)
{
  if( ! TrayIcon->Visible )
  {
    // Without start & stop Animate show application icon in the tray !
    TrayIcon->Animate = false;
    TrayIcon->Visible = true;
  }

  if( TrayIcon->IconIndex != State )
    TrayIcon->IconIndex = State;

  String tip = GetFullAppName();

  if( ! SameText(TrayIcon->Hint, tip) )
    TrayIcon->Hint = tip;
}
//---------------------------------------------------------------------------
/* State: 0-ok 1-warning 2-error
void __fastcall TMainForm::TrayShowBallon(AnsiString Title, AnsiString Text, int State)
{
  // That messages are not received, not more than once every 1000 ms
  DWORD tc = GetTickCount();
  if( tc - LastBalloonShowTime <= 1000UL )
    return;

  if( ! TrayIcon->Visible )
  {
    TrayIcon->Animate = false;
    TrayIcon->Visible = true;
  }

  LastBalloonShowTime = tc;
  TrayIcon->BalloonTitle = Title;
  TrayIcon->BalloonHint = Text;
  switch( State )
  {
    case 0: TrayIcon->BalloonFlags = bfInfo; break;
    case 1: TrayIcon->BalloonFlags = bfWarning; break;
    case 2: TrayIcon->BalloonFlags = bfError; break;
    default: TrayIcon->BalloonFlags = bfNone; break;
  }
  TrayIcon->ShowBalloonHint();
}
*/
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caNone;
  Hide();
}
//---------------------------------------------------------------------------
void __fastcall CreateShortcut(int where)
{
  TWaitCursor wait;
  String ShortcuteName = Application->Title;
  String ShortcutFileName;

  try
  {
    char szLinkPath[MN];
    HRESULT hr = SHGetFolderPath(NULL, where, NULL, 0, szLinkPath);
    if( FAILED(hr) )
      throw Exception(FormatLastError2(hr) + " [1:" + IntToStr(hr) + "]");

    ShortcutFileName = String(szLinkPath) + "\\" + ShortcuteName + ".lnk";

    hr = CreateLink(ApplicationExeName.c_str(),
                    "",
                    ShortcutFileName.c_str(),
                    "",
                    "tray");
    if( FAILED(hr) )
      throw Exception(FormatLastError2(hr) + " [2:" + IntToStr(hr) + "]");
  }
  catch(const Exception & E)
  {
    ReportError2("Error creating shortcut \"%s\"\nApplication \"%s\"\nError: \"%s\"",
      ShortcutFileName.c_str(), ApplicationExeName.c_str(), E.Message.c_str());
  }
  catch(...)
  {
    ReportError2("Error creating shortcut \"%s\"\nApplication \"%s\"",
      ShortcutFileName.c_str(), ApplicationExeName.c_str());
  }
}
//---------------------------------------------------------------------------
void __fastcall DeleteShortcut(int where)
{
  String ShortcuteName = Application->Title;
  String ShortcutFileName;

  try
  {
    char szLinkPath[MN];
    HRESULT hr = SHGetFolderPath(NULL, where, NULL, 0, szLinkPath);
    if( FAILED(hr) )
      throw Exception(FormatLastError2(hr) + " [1:" + IntToStr(hr) + "]");

    ShortcutFileName = String(szLinkPath) + "\\" + ShortcuteName + ".lnk";

    if( ! ::DeleteFile(ShortcutFileName.c_str()) )
    {
      hr = GetLastError();
      throw Exception(FormatLastError2(hr) + " [2:" + IntToStr(hr) + "]");
    }
  }
  catch(const Exception & E)
  {
    ReportError2("Failed to delete a shortcut \"%s\"\nError: \"%s\"",
      ShortcutFileName.c_str(), E.Message.c_str());
  }
  catch(...)
  {
    ReportError2("Failed to delete a shortcut \"%s\"",
      ShortcutFileName.c_str());
  }
}
//---------------------------------------------------------------------------
bool __fastcall IsShortcutExist(int where)
{
  String ShortcuteName = Application->Title;
  String ShortcutFileName;

  char szLinkPath[MN];
  HRESULT hr = SHGetFolderPath(NULL, where, NULL, 0, szLinkPath);
  if( FAILED(hr) )
    return false;

  ShortcutFileName = String(szLinkPath) + "\\" + ShortcuteName + ".lnk";
  return FileExists(ShortcutFileName);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aViewFileExecute(TObject *Sender)
{
  OpenDialog->Filter = "All files (*.*)|*.*";
  OpenDialog->FilterIndex = 1;
  OpenDialog->Title = "View syslog file";
  if( ! OpenDialog->Execute() )
    return;

  ViewFileName = OpenDialog->FileName;
  TFile viewin(ViewFileName, GENERIC_READ,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL);

  if( ! viewin )
    return;

  TmpViewFileName = GetTemporaryFileName();
  TFile out(TmpViewFileName, GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL);
  if( ! out )
    return;
  out.ToEnd();

  TWaitCursor wait;

  AnsiString file_line;
  DWORD rsize = 1024*1024;
  ULONGLONG fsize = viewin.GetSize64();
  BYTE * p = new BYTE[rsize];

  while( fsize > 0 )
  {
    viewin.Read(p, MIN(rsize,fsize));
    if( viewin.Bytes <= 0 )
      break;
    fsize -= viewin.Bytes;
    
    for(DWORD i=0, c=viewin.Bytes; i<c; i++)
    {
      if( p[i]=='\n' || p[i]=='\r' )
      {
        if( file_line.Length() > 0 )
        {
          // Now file_line contains new line
          TSyslogMessage sm;
          sm.ProcessMessageFromSyslogd(file_line.c_str(), file_line.Length(), NULL);
          sm.Save(out);
          file_line.SetLength(0);
        }
        continue;
      }
      file_line += (char)p[i];
    } // End for
  }
  delete [] p;

  SetFile(TmpViewFileName);
  
  aCancelViewFile->Hint = String("Cancel view file ") + ViewFileName;
  SetViewFileMode(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aCancelViewFileExecute(TObject *Sender)
{
  SetViewFileMode(false);
  SetFile(SyslogFile);
  DeleteFile(TmpViewFileName);
}
//---------------------------------------------------------------------------
void PrintSB(int i, String s)
{
  SB(i) = s;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aGotoNewLineExecute(TObject *Sender)
{
  aGotoNewLine->Checked = ! aGotoNewLine->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetViewFileMode(bool b)
{
  ViewFileMode = b;
  aCancelViewFile->Visible = b;
  aSetup->Visible = ! b;
  aProcessSetup->Visible = ! b;
  aGotoNewLine->Visible = ! b;
  mOpenFilesLoc->Visible = ! b;
  aClear->Visible = ! b;
  UpdateCaption();
}
//---------------------------------------------------------------------------
TSyslogMessage * __fastcall TMainForm::GetMessageByIndex(int i)
{
  if( i < 0 || i >= MessList->Count )
    return NULL;
  return (TSyslogMessage *)MessList->Items[i];
}
//---------------------------------------------------------------------------
__fastcall TDrawGrid2::TDrawGrid2(Classes::TComponent* AOwner) :
    TDrawGrid(AOwner)
{
  bAllowUpdate = true;
}
//---------------------------------------------------------------------------
void __fastcall TDrawGrid2::Update(void)
{
  if( bAllowUpdate )
    TDrawGrid::Update();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aRunIEExecute(TObject *Sender)
{
  String url;
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( sm )
    url = String("http://") + sm->SourceAddr;

  ShellExecute(GetDesktopWindow(), "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aPingExecute(TObject *Sender)
{
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( ! sm )
    return;

  if( sm->SourceAddr.Length() == 0 )
    return;

  char * szCurDir = NULL;
  char szSysDir[MN];
  if( GetSystemDirectory(szSysDir, sizeof(szSysDir)) > 0 )
    szCurDir = szSysDir;

  DWORD Error;
  String s = String("ping.exe ") + sm->SourceAddr;
  if( ! RunProg(s.c_str(), SW_NORMAL, false, &Error, szCurDir) )
    ReportError2("Error in command \"%s\": %s [%d]",
      s.c_str(), FormatLastError2(Error).c_str(), Error);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aFilterByIPExecute(TObject *Sender)
{
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( ! sm )
    return;
  if( sm->SourceAddr.Length() == 0 )
    return;
  FilterEdit1->Text = sm->SourceAddr;
  FieldCB1->ItemIndex = 4;
  FilterTimer = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aFilterByHostExecute(TObject *Sender)
{
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( ! sm )
    return;
  if( sm->HostName.Length() == 0 )
    return;
  FilterEdit1->Text = sm->HostName;
  FieldCB1->ItemIndex = 6;
  FilterTimer = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aFilterByFacilityExecute(TObject *Sender)
{
  TSyslogMessage * sm = GetMessageByIndex(LogSG->Row - 1);
  if( ! sm )
    return;
  if( sm->Facility.Length() == 0 )
    return;
  FilterEdit1->Text = sm->Facility;
  FieldCB1->ItemIndex = 8;
  FilterTimer = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateFilterButton(void)
{
  ClearFilterButton->Visible = FilterEdit1->Text.Length() > 0 ||
                               FilterEdit2->Text.Length() > 0 ||
                               FilterByPriorityCB->ItemIndex > 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aHighlightingSetupExecute(TObject *Sender)
{
  HighlightForm = new THighlightForm(this, HP->CurrentProfile);
  if( HighlightForm->ShowModal() == mrOk )
  {
    HP->Load(HighlightFile);
    LogSG->Invalidate();
  }
  delete HighlightForm;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FillProfilePopupMenu(void)
{
  for(int i=0; i<HP->Count; i++)
  {
    TMessHighlightList * mhl = (TMessHighlightList *)HP->Items[i];

    TMenuItem * NewItem = new TMenuItem(HighlightingProfilesPM);
    NewItem->Caption = mhl->ProfileName;
    NewItem->Tag = i;
    NewItem->OnClick = ChangeProfileClick;
    NewItem->RadioItem = true;
    if( i == HP->CurrentProfile )
      NewItem->Checked = true;

    HighlightingProfilesPM->Items->Add(NewItem);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChangeProfileClick(TObject *Sender)
{
  HP->CurrentProfile = ((TMenuItem *)Sender)->Tag;
  ((TMenuItem *)Sender)->Checked = true;
  LogSG->Invalidate();
}
//---------------------------------------------------------------------------
static void OnReceiveMail(TLetter * l)
{
  if( ! l->result )
    WriteToLogError(String("ERROR\tSent e-mail:") + l->error);
}
//---------------------------------------------------------------------------
// return false if message p should be ignored
bool ProcessMessageRules(TSyslogMessage * p)
{
  // test:
  //AlarmShow(p->Format("{time} {message}"));

  bool rv = true;
  TMessProcessRule * pr;
  for(int i=0; i<ProcessRules->Count; i++)
  {
    pr = ProcessRules->Get(i);
    if( ! pr->bEnable )
      continue;
    if( ! pr->Match.Match(p) )
      continue;

    if( pr->Process.bIgnore )
    {
      rv = false;
    }
    if( pr->Process.bAlarm )
    {
      AlarmShow(p->Format(pr->Process.AlarmMess));
    }
    if( pr->Process.bSound )
    {
      TalkingThread->Play(pr->Process.SoundFile, pr->Process.PlayCount);
    }
    if( pr->Process.bSendMail )
    {
      TLetter l;
      l = MainCfg.Letter;
      if( pr->Process.Recipient.Length() > 0 )
        l.recipient = pr->Process.Recipient;
      l.subject = p->Format(l.subject);
      l.message = p->Format(l.message);
      l.callback = OnReceiveMail;
      TSendmailThread::Send(&l);
    }
    if( pr->Process.bRunProg )
    {
      DWORD LastError;
      String cmd = p->Format(pr->Process.ProgFile);
      if( ! RunProg(cmd.c_str(),
                    pr->Process.bProgHide ? SW_HIDE : SW_SHOWNORMAL,
                    false, &LastError) )
        WriteToLogError("ERROR\tRun program %s: %s",
          cmd.c_str(), FormatLastError2(LastError).c_str());
    }
    if( pr->Process.bSaveToFile )
    {
      String f = pr->Process.SaveFile;
      if( ExtractFilePath(f).Length() == 0 )
        f = WorkDir + f;
      if( ! p->Save(f) )
        WriteToLogError("ERROR\tSave message to file: %s", f.c_str());
    }
  }
  return rv;
}
//---------------------------------------------------------------------------
void AlarmShow(String s)
{
  if( ! ShowAlarmForm )
    ShowAlarmForm = new TShowAlarmForm(MainForm);

  if( ShowAlarmForm->ListBox->Font->Name != MainForm->LogSG->Font->Name ||
      ShowAlarmForm->ListBox->Font->Size != MainForm->LogSG->Font->Size )
    ShowAlarmForm->ListBox->Font = MainForm->LogSG->Font;

  if( s.Length() > 0 )
  {
    ShowAlarmForm->ListBox->Items->Add(s);
    int c = ShowAlarmForm->ListBox->Items->Count;
    ShowAlarmForm->ListBox->ItemIndex = c - 1;
    if( c > 10000 )
      ShowAlarmForm->ListBox->Items->Delete(0);
  }

  if( ShowAlarmForm->WindowState==wsMinimized )
    ShowAlarmForm->WindowState = wsNormal;
  else if( MainForm->WindowState==wsMinimized )
    MainForm->WindowState = wsNormal;
  else
    ShowAlarmForm->Show();

  ShowAlarmForm->FormStyle = fsNormal;
  ShowAlarmForm->FormStyle = fsStayOnTop;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aShowAlarmsExecute(TObject *Sender)
{
  AlarmShow("");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aProcessSetupExecute(TObject *Sender)
{
  ProcessForm = new TProcessForm(this);
  if( ProcessForm->ShowModal() == mrOk )
  {
    ProcessRules->Load(ProcessFile);
  }
  delete ProcessForm;
}
//---------------------------------------------------------------------------

