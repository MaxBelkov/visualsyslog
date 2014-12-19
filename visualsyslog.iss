#define pname 'Visual Syslog Server'
#define pver GetFileVersion('Release\visualsyslog.exe')

[Setup]
AppName={#pname}
AppVerName={#pname} {#pver}
VersionInfoVersion={#pver}

AppPublisher=Max Belkov
AppPublisherURL=https://github.com/MaxBelkov/visualsyslog
AppComments=Syslog Server for Windows with a graphical user interface
AppCopyright=GNU GENERAL PUBLIC LICENSE Version 2
DefaultDirName={pf}\visualsyslog\
DefaultGroupName=Visual Syslog
OutputBaseFilename=visualsyslog_setup
Compression=lzma/max
SolidCompression=yes
SetupIconFile=visualsyslog.ico

[Tasks]
Name: Firewall; Description: "Add an exception to the Windows Firewall";

[Files]
Source: "Release\visualsyslog.exe"; DestDir: "{app}";
Source: "changelog.txt"; DestDir: "{app}";
; ssl/tls support for smtp server
Source: "install\libeay32.dll"; DestDir: "{app}";
Source: "install\libssl32.dll"; DestDir: "{app}";
Source: "install\ssleay32.dll"; DestDir: "{app}";
; default config files
Source: "install\highlight.xml"; DestDir: "{localappdata}\visualsyslog"; Flags: onlyifdoesntexist
Source: "install\cfg.xml"; DestDir: "{localappdata}\visualsyslog"; Flags: onlyifdoesntexist
Source: "install\process.xml"; DestDir: "{localappdata}\visualsyslog"; Flags: onlyifdoesntexist
Source: "install\alarm.wav"; DestDir: "{localappdata}\visualsyslog"; Flags: onlyifdoesntexist

[Icons]
Name: "{group}\Visual Syslog server"; Filename: "{app}\visualsyslog.exe"; WorkingDir: "{app}";
Name: "{userdesktop}\Visual Syslog server"; Filename: "{app}\visualsyslog.exe"; WorkingDir: "{app}";

[Run]
; Below vista
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\visualsyslog.exe"""; Flags: runhidden; OnlyBelowVersion: 0,6.0; Tasks: Firewall;
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\visualsyslog.exe"" ""Visual Syslog Server"" ENABLE"; Flags: runhidden; OnlyBelowVersion: 0,6.0; Tasks: Firewall;
; Vista and later
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=""Visual Syslog Server"" program=""{app}\visualsyslog.exe"""; Flags: runhidden; MinVersion: 0,6.0.6000; Tasks: Firewall;
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall add rule name=""Visual Syslog Server"" program=""{app}\visualsyslog.exe"" dir=in action=allow enable=yes"; Flags: runhidden; MinVersion: 0,6.0.6000; Tasks: Firewall;

Filename: "{app}\visualsyslog.exe"; Description: "Launch Visual Syslog Server"; Flags: postinstall nowait skipifsilent

[UninstallRun]
; Below vista
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\visualsyslog.exe"""; Flags: runhidden; OnlyBelowVersion: 0,6.0; Tasks: Firewall;
; Vista and later
Filename: "{sys}\netsh.exe"; Parameters: "advfirewall firewall delete rule name=""Visual Syslog Server"" program=""{app}\visualsyslog.exe"""; Flags: runhidden; MinVersion: 0,6.0.6000; Tasks: Firewall;
