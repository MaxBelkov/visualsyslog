//---------------------------------------------------------------------------
#ifndef serverH
#define serverH

#include "udp.h"
#include "file.h"
//---------------------------------------------------------------------------
class TSyslogMessage
{
public:
  String SourceAddr; // IP
  int PRI;           // -1: PRI not exist in message
  String Facility;
  String Priority;   // also called Severity
  String DateStr;
  String HostName;
  String Tag;
  String Msg;

public:
  TSyslogMessage();
  void Clear();
  // p must be null terminated
  bool ProcessMessageFromSyslogd(char * p, int size, sockaddr_in * from_addr);
  void ProcessMessageFromFile(char * p);
  // Write to file
  //bool Save(const String & file);
  bool Save(TFile & out);
  bool Save(const String & file, TFile & out);
  String ClipboardString(void);
};
//---------------------------------------------------------------------------
bool IsValidSyslogDate(const char * p);
//---------------------------------------------------------------------------
#endif
