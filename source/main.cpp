//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop

#include "udp.h"         // udp server
#include "syslog.h"      // base syslog definitions
#include "server.h"      // parse syslog packets to TSyslogMessage
#include "utils.h"       // common functions
#include "messageform.h" // cool message box
#include "saveini.h"     // TSaveParamsINI
#include "file.h"        // work with files
#include "setup.h"
#include "aboutbox.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// Cool colors
#define cl_Yellow ((TColor)RGB(255,255,130))
#define cl_Red    ((TColor)RGB(255,130,130))
#define cl_LightGray   ((TColor)RGB(220,220,220))
#define cl_Green  ((TColor)RGB(128,255,128))

#define cl_Olive  ((TColor)RGB(180,180,100))
#define cl_Gray   ((TColor)RGB(130,130,130))
#define cl_Blue   ((TColor)RGB(89,222,255))
#define cl_Rose   ((TColor)RGB(255,151,203))

// Start size in bytes, to read from text file
#define StartSizeToRead (1024*1024)
// Lines count to add to string grid
#define StartProtoGridLines 200
// Max lines count in the string grid
#define MaxGridLines 200000

// save StringGrid content to clipboard (defined in utils.cpp)
void StringGridToClipboard(TStringGrid * p);

#define SB(n) MainForm->StatusBar->Panels->Items[n]->Text

// save main form screen position and other params to .ini file
TSaveParamsINI * AppParams = NULL;

TUDP * udp = NULL;
int UdpPort = 514;

String ApplicationExeName;
String WorkDir;
String SyslogFile;
String ErrorlogFile;

// raw file
TFile rawout;
String RawFile;

BYTE HiVer=0, LoVer=0;
String GetVersionString(void);
String GetFullAppName(void);

bool bHideToTray;

TMainForm * MainForm = NULL;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject * Sender)
{
  LastBalloonShowTime = 0UL;
  ApplicationExeName = GetApplicationExeName();

  // Extract program version for resources
  WORD Major, Minor, Release, Build;
  if( GetFileVersion(ApplicationExeName, Major, Minor, Release, Build) )
  {
    HiVer = (BYTE)Major;
    LoVer = (BYTE)Minor;
  }

  if( bHideToTray )
    Application->ShowMainForm = false;

  TrayChangeIcon(0);

  char str[MAX_PATH];
  // CSIDL_COMMON_APPDATA   C:\Documents and Settings\All Users\Application Data
  // CSIDL_LOCAL_APPDATA    C:\Documents and Settings\username\Local Settings\Application Data (nonroaming)
  if( SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
					  NULL, SHGFP_TYPE_CURRENT, str) == S_OK )
  {
	WorkDir = IncludeTrailingPathDelimiter(str) + "visualsyslog\\";
    SyslogFile = WorkDir + "syslog";
    RawFile = WorkDir + "raw";
    ErrorlogFile = WorkDir + "errors.txt";
    if( ! DirectoryExists(WorkDir) )
      if( ! CreateDir(WorkDir) )
        ReportError2("Error creating directory: \"%s\"", WorkDir.c_str());
  }

  AppParams = new TSaveParamsINI(WorkDir + "params.ini");
  AppParams->MinimumColumnWidth = 10;

  Variant v = AppParams->Get("Setup", "UdpPort");
  if( ! v.IsEmpty() )
    if( (int)v > 0 )
	  UdpPort = v;

  udp = new TUDP();
  if( udp->GetError() )
  {
    String err = String("Error udp: ") + udp->GetErrorMessageEng();
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
  }
  else
  {
    ServerStart();
  }

  Init(true, 1);
  SetFile(SyslogFile);

  Timer->Enabled = true;
  UdpTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  UdpTimer->Enabled = false;
  Timer->Enabled = false;

  ServerStop();
  
  delete udp;
  udp = NULL;

  rawout.Close();

  delete LogSG_LivingColumns;
  // Save all
  *AppParams << this << LogSG;
  AppParams->Values["GotoNewMess"] = GotoNewMessCB->Checked;
  AppParams->Values["TextFilter"] = FilterEdit->Text;
  AppParams->Values["TextFilterIgnore"] = FilterIgnoreEdit->Text;

  AppParams->Values["FontName"] = LogSG->Font->Name;
  AppParams->Values["FontSize"] = LogSG->Font->Size;
  AppParams->Values["FontStyle"] = LogSG->Font->Style.ToInt();

  AppParams->Set("Setup", "UdpPort", UdpPort);

  delete AppParams;
  AppParams = NULL;
}
//---------------------------------------------------------------------------
void TMainForm::ServerStart(void)
{
  if( udp->Open("0.0.0.0", UdpPort, "0.0.0.0", 0) )
  {
    SB(0) = String("syslog server ") + udp->GetLocalAddrPort();
  }
  else
  {
    String err = String("Error udp: ") + udp->GetErrorMessageEng() +
      " [udp port " + IntToStr(UdpPort) + "]";
    WriteToLogError(String("ERROR\t") + err);
    ReportError2(err);
  }
}
//---------------------------------------------------------------------------
void TMainForm::ServerStop(void)
{
  SB(0) = "syslog server not started";
  udp->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Init(bool _bLive, int _ProtoFormat)
{
  bLive = _bLive;
  ApplyFilter = 0;
  SizeToRead = StartSizeToRead;
  AddStringGridLines = StartProtoGridLines;

  // For "non-life" protocol remove function:
  //   "Goto new line"
  //   "Clear"
  if( ! bLive )
  {
    GotoNewMessCB->Visible = false;
    mClear->Visible = false;
  }
  LogSG->Cells[0][0] = " Time";
  LogSG->Cells[1][0] = " IP";
  LogSG->Cells[2][0] = " Host";
  LogSG->Cells[3][0] = " Facility";
  LogSG->Cells[4][0] = " Priority";
  LogSG->Cells[5][0] = " Tag";
  LogSG->Cells[6][0] = " Message";

  *AppParams >> this >> LogSG;

  Variant v = AppParams->Values["GotoNewMess"];
  if( ! v.IsEmpty() )
	GotoNewMessCB->Checked = v;

  v = AppParams->Values["TextFilter"];
  if( ! v.IsEmpty() )
  {
	FilterEdit->Text = v;
    fFilter = v;
    ClearFilterButton->Visible = true;
  }
  v = AppParams->Values["TextFilterIgnore"];
  if( ! v.IsEmpty() )
  {
	FilterIgnoreEdit->Text = v;
    fFilterIgnore = v;
    ClearFilterButton->Visible = true;
  }

  String FontName = AppParams->Values["FontName"];
  if( FontName.Length() > 0 )
  {
    LogSG->Font->Name = FontName;
    LogSG->Font->Size = AppParams->Values["FontSize"];
    LogSG->Font->Style = TFontStyles((int)AppParams->Values["FontStyle"]);
  }

  SetLinesHeight();

  LogSG_LivingColumns = new TStringGridLivingColumns(LogSG);
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

  FileSize = in.GetSize();
  // Since the beginning of the file, look at the size limit
  if( FileSize > SizeToRead )
  {
    MoreButton->Visible = true;
    mMoreMenu->Visible = true;

    in.Pointer = FileSize - SizeToRead;
    // Positioning at the beginning of a new line in the file
    GotoNewLine();
  }
  else
  {
    MoreButton->Visible = false;
    mMoreMenu->Visible = false;
  }

  Read();
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
void __fastcall TMainForm::MoreButtonClick(TObject *Sender)
{
  // Increase the amount of reading from a file
  SizeToRead = ReadedSize + StartSizeToRead;
  // Increase the number of lines added
  AddStringGridLines += StartProtoGridLines;
  RedrawProto();
  ActiveControl = LogSG;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
  if( fFilter != FilterEdit->Text )
  {
    fFilter = FilterEdit->Text;
    ApplyFilter = 2;
  }
  if( fFilterIgnore != FilterIgnoreEdit->Text )
  {
    fFilterIgnore = FilterIgnoreEdit->Text;
    ApplyFilter = 2;
  }
  else if( ApplyFilter > 0 )
  {
    ApplyFilter--;
    if( ApplyFilter == 0 )
    {
      ClearFilterButton->Visible = FilterEdit->Text.Length() > 0 ||
                                   FilterIgnoreEdit->Text.Length() > 0;
      RedrawProto();
    }
  }

  // Check for new records, if the protocol is "live"
  if( bLive )
    if( in.IsOpen() )
      Read();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateCaption(void)
{
  String s = GetFullAppName() + "  [" + fFile + "]";
  // If the text file is not completely read
  if( MoreButton->Visible )
    s += " the last " + GetBytesStringEng(ReadedSize) +
         " of the " + GetBytesStringEng(FileSize);
  if( Caption != s )
    Caption = s;

  int lc = VisibleLines;
  if( TotalLines != lc )
    GroupBox2->Caption = "Displaying " + IntToStr(lc) + " lines of " +
      IntToStr(TotalLines);
  else
    GroupBox2->Caption = "Total " + IntToStr(lc) + " lines";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Clear(void)
{
  LogSG->RowCount = 2;
  for(int i=0; i<LogSG->ColCount; i++)
    LogSG->Cells[i][1] = "";

  TotalLines = 0;
  VisibleLines = 0;
  ReadedSize = 0;

  UpdateCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Read(void)
{
  BYTE * p = new BYTE[SizeToRead];
  in.Read(p, SizeToRead);
  if( in.Bytes > 0 )
  {
    ReadedSize += in.Bytes;
    // It is possible that the text file is filled so quickly that
    // he read to the end and not at the end of the array p [] is to truncate
    // proto_line var is global, to save cut line part
    bool TextFilterOn = fFilter.Length() > 0;
    bool TextFilterIgnoreOn = fFilterIgnore.Length() > 0;

    for(DWORD i=0, c=in.Bytes; i<c; i++)
    {
      if( p[i]=='\n' || p[i]=='\r' )
      {
        if( proto_line.Length() > 0 )
        {
          // Now proto_line contains new line
          TotalLines++;

          // Text to find enabled
          if( TextFilterOn )
            if( proto_line.Pos(fFilter) == 0 )
            {
              // and the desired substring is NOT found - continue
              proto_line.SetLength(0);
              continue;
            }

          // Text to ignore enabled
          if( TextFilterIgnoreOn )
            if( proto_line.Pos(fFilterIgnore) > 0 )
            {
              // and the desired substring is found - continue
              proto_line.SetLength(0);
              continue;
            }

          TSyslogMessage sm;
          sm.ProcessMessageFromFile(proto_line.c_str());

          int priority = gettextcode(sm.Priority.c_str(), prioritynames);
          /*
          now sm.PRI is not used
          int facility = gettextcode(sm.Facility.c_str(), facilitynames);
          sm.PRI = priority + facility;
          */

          // Filter enabled
          if( FilterByPriorityCB->ItemIndex > 0 )
            if( FilterByPriorityCB->ItemIndex-1 != priority )
            {
              // and the type does not match the required - continue
              proto_line.SetLength(0);
              continue;
            }

          VisibleLines++;

          // Add lines to string grid by AddStringGridLines lines: this faster !
          if( LogSG->RowCount < VisibleLines + 1 ) // + 1 fixed line
            LogSG->RowCount = LogSG->RowCount + AddStringGridLines;
          // Add lines by one a very long !
          //LogSG->RowCount = VisibleLines + 1; // + 1 fixed line

          int index = VisibleLines;
          LogSG->Cells[0][index] = sm.DateStr;
          LogSG->Cells[1][index] = sm.SourceAddr;
          LogSG->Cells[2][index] = sm.HostName;
          LogSG->Cells[3][index] = sm.Facility;
          LogSG->Cells[4][index] = sm.Priority;
          LogSG->Cells[5][index] = sm.Tag;
          LogSG->Cells[6][index] = sm.Msg;

          LogSG->Objects[0][index] = (TObject *)priority;

          proto_line.SetLength(0);
        }
        continue;
	  }
	  proto_line += (char)p[i];
	} // End for by protocol characters

    // Establish the actual number of lines (but not less than 2x)
    LogSG->RowCount = MAX(2, VisibleLines + 1); // + 1 fixed line

    FileSize = in.GetSize();
    UpdateCaption();

    // Moves to the last line if enabled
    if( GotoNewMessCB->Checked )
      LogSG->Row = LogSG->RowCount - 1;
  }
  delete [] p;

  if( LogSG->RowCount >= MaxGridLines )
  {
    ReportMess2("Maximum number of lines exceeded (%d)", MaxGridLines);
    // Read protocol file again
    RedrawProto();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N30Click(TObject *Sender)
{
  StringGridToClipboard((TStringGrid *)ClipboardPM->PopupComponent);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LogSGDblClick(TObject *Sender)
{
  ShowMessage2(this,
               LogSG->Cells[6][LogSG->Row],
	  		   "Message time " + LogSG->Cells[0][LogSG->Row],
               true, // modal
               false); // error
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LogSGDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
  if( ARow<1 ) return; // Grid title

  TCanvas * c = ((TStringGrid *)Sender)->Canvas;
  int x,y;

  if( State.Contains(gdSelected) ) // Selected line
  {
  }
  else
  {
    c->Brush->Color = GetLogRecordColor((int)LogSG->Objects[0][ARow]);
    c->FillRect(Rect);
    x = Rect.Left + 2;
    y = Rect.Top + ((Rect.Bottom - Rect.Top -
        c->TextHeight(LogSG->Cells[ACol][ARow])) / 2);
    c->TextRect(Rect, x, y, LogSG->Cells[ACol][ARow]);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearFilterButtonClick(TObject *Sender)
{
  FilterEdit->Text = "";
  fFilter = "";

  FilterIgnoreEdit->Text = "";
  fFilterIgnore = "";

  ApplyFilter = 0;

  FilterByPriorityCB->ItemIndex = 0;

  ClearFilterButton->Visible = false;
  
  // Read protocol file again
  RedrawProto();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FilterByPriorityCBSelect(TObject *Sender)
{
  ClearFilterButton->Visible = true;
  // Read protocol file again
  RedrawProto();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FilterByPriorityCBDrawItem(TWinControl *Control,
      int Index, TRect &Rect, TOwnerDrawState State)
{
  TCanvas * c = FilterByPriorityCB->Canvas;
  TStrings * items = FilterByPriorityCB->Items;

  int priority = LOG_INFO;
  if( Index > 0 )
    priority = Index - 1;
  c->Brush->Color = GetLogRecordColor(priority);
  c->FillRect(Rect);

  int VertInc = ((Rect.Bottom - Rect.Top) - c->TextHeight(items->Strings[Index]))/2;
  c->Font->Color = clBlack;
  c->TextRect(Rect, Rect.Left + 4, Rect.Top + VertInc, items->Strings[Index]);
}
//---------------------------------------------------------------------------
TColor TMainForm::GetLogRecordColor(int priority)
{
  // priority not defined (not received from syslog client)
  if( priority < 0 )
    return clWindow;
  switch( LOG_PRI(priority) )
  {
    case LOG_EMERG:
    case LOG_ALERT:
    case LOG_CRIT:
    case LOG_ERR:
      return cl_Red;
    case LOG_WARNING:
      return cl_Yellow;
    case LOG_NOTICE:
    case LOG_INFO:
      return clWindow;
    case LOG_DEBUG:
      return cl_LightGray;
  }
  return cl_Green; // default :)
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FontButtonClick(TObject *Sender)
{
  FontDialog->Font = LogSG->Font;
  if( ! FontDialog->Execute() )
    return;
  LogSG->Font = FontDialog->Font;
  SetLinesHeight();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetLinesHeight(void)
{
  // This work always sometimes
  //int rh = LogSG->Canvas->TextHeight("Qj");
  // This work always
  int rh = LogSG->Font->Size + 6;
  LogSG->DefaultRowHeight = rh + rh / 2;
  FilterByPriorityCB->ItemHeight = 18;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mClearClick(TObject *Sender)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UdpTimerTimer(TObject *Sender)
{
  UdpReceiveMessage();
}
//---------------------------------------------------------------------------
void TMainForm::UdpReceiveMessage(void)
{
  if( ! udp )
    return;
  if( ! udp->IsOpen() )
    return;

  struct sockaddr_in a;
  DWORD rl;
  while( (rl = udp->ReadLength()) > 0 )
  {
    BYTE * ReceiveBuffer = new BYTE[rl + 1];
    if( ! udp->ReadFrom(ReceiveBuffer, rl, (struct sockaddr *)&a) )
    {
      if( udp->GetWSAError() == MYERROR_READCOUNT )
        ; // Good error :) In the receive buffer still have UDP packets
      else
      {
        WriteToLogError(String("ERROR\tudp: ") + udp->GetErrorMessageEng());
      }
    }

    if( udp->bytes > 0 )
    {
      // NULL terminate syslog message
      ReceiveBuffer[udp->bytes] = 0;

      WriteToLogRawMessage((char *)ReceiveBuffer);

      TSyslogMessage sm;
      sm.ProcessMessageFromSyslogd((char *)ReceiveBuffer, udp->bytes, &a);
      sm.Save(SyslogFile);
    }
    delete [] ReceiveBuffer;
  }
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
    rawout.SetPointer(0, FILE_END);
  }
  if( ! rawout )
    return false;

  String s(p);
  s += CR;
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
  out.SetPointer(0, FILE_END);

  va_list argptr;
  va_start(argptr, fmt);
  String s;
  s.vprintf(fmt.c_str(), argptr);

  // Print in the status bar
  if( s.Pos("ERROR\t") > 0 )
    SB(1) = s.SubString(7, s.Length()-6);

  s = Now().DateTimeString() + '\t' + s + CR;
  out << s;
  va_end(argptr);
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mOpenFileFolderClick(TObject *Sender)
{
  ShellExecute(GetDesktopWindow(), "explore", WorkDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mSetupClick(TObject *Sender)
{
  bool se = IsShortcutExist(CSIDL_STARTUP);

  SetupForm = new TSetupForm(this);
  SetupForm->PortEdit->Text = IntToStr(UdpPort);
  SetupForm->AutoStartCB->Checked = se;

  if( SetupForm->ShowModal() == mrOk )
  {
    int port;
    if( TryStrToInt(SetupForm->PortEdit->Text, port) )
    {
      if( port > 0 && port < 65535 && port != UdpPort )
      {
        UdpPort = port;
        ServerStop();
        ServerStart();
      }
    }
    if( SetupForm->AutoStartCB->Checked != se )
    {
      if( se )
        DeleteShortcut(CSIDL_STARTUP);
      else
        CreateShortcut(CSIDL_STARTUP);
    }
  }
  delete SetupForm;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RedrawProto(void)
{
  SetFile(fFile);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mAboutClick(TObject *Sender)
{
  AboutBoxForm = new TAboutBoxForm(this);
  AboutBoxForm->ShowModal();
  delete AboutBoxForm;
}
//---------------------------------------------------------------------------
String GetVersionString(void)
{
  return IntToStr(HiVer) + "." + IntToStr(LoVer);
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
void __fastcall TMainForm::mExitClick(TObject *Sender)
{
  Application->Terminate();
}
//---------------------------------------------------------------------------
// State: 0-ok 1-warning 2-error
void __fastcall TMainForm::TrayChangeIcon(int State)
{
  if( TrayIcon->IconIndex != State )
    TrayIcon->IconIndex = State;

  String tip = GetFullAppName() +
               "\nUDP port: " + IntToStr(UdpPort);

  if( ! SameText(TrayIcon->Hint, tip) )
    TrayIcon->Hint = tip;
}
//---------------------------------------------------------------------------
// State: 0-ok 1-warning 2-error
void __fastcall TMainForm::TrayShowBallon(AnsiString Title, AnsiString Text, int State)
{
  // That messages are not received, not more than once every 1000 ms
  DWORD tc = GetTickCount();
  if( tc - LastBalloonShowTime <= 1000UL )
    return;
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
void __fastcall TMainForm::mViewFileClick(TObject *Sender)
{
  OpenDialog->Filter = "All files (*.*)|*.*";
  OpenDialog->FilterIndex = 1;
  OpenDialog->Title = "View syslog file";
  if( ! OpenDialog->Execute() )
    return;

  String viewfile = OpenDialog->FileName;
  TFile viewin(viewfile, GENERIC_READ,
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
  out.SetPointer(0, FILE_END);

  TWaitCursor wait;

  AnsiString file_line;
  DWORD rsize = 1024*1024;
  DWORD fsize = viewin.GetSize();
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
  
  CancelViewButton->Hint = String("Cancel view file ") + viewfile;
  CancelViewButton->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CancelViewButtonClick(TObject *Sender)
{
  CancelViewButton->Visible = false;
  SetFile(SyslogFile);
  DeleteFile(TmpViewFileName);
}
//---------------------------------------------------------------------------

