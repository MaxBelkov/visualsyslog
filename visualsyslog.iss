[Setup]
AppName=Visual Syslog server
AppVerName=Visual Syslog server 1.0
VersionInfoVersion=1.0.0.0

AppPublisher=Max Belkov
AppPublisherURL=https://github.com/MaxBelkov/visualsyslog
;AppUpdatesURL=
AppComments=Syslog server (daemon) for Windows with a graphical user interface
AppCopyright=GNU GENERAL PUBLIC LICENSE Version 2
DefaultDirName={pf}\visualsyslog\
DefaultGroupName=Visual Syslog
OutputBaseFilename=visualsyslog_setup
Compression=lzma/max
SolidCompression=yes
;WizardImageFile=setup\treyler.bmp
;WizardSmallImageFile=setup\treyler_little.bmp
SetupIconFile=visualsyslog.ico

[Files]
Source: "Release\visualsyslog.exe"; DestDir: "{app}";

[Icons]
Name: "{group}\Visual Syslog server"; Filename: "{app}\visualsyslog.exe"; WorkingDir: "{app}";
Name: "{userdesktop}\Visual Syslog server"; Filename: "{app}\visualsyslog.exe"; WorkingDir: "{app}";
