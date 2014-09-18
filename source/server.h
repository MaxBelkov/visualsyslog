//---------------------------------------------------------------------------
#ifndef serverH
#define serverH

//---------------------------------------------------------------------------
class TSyslogMessage
{
public:
  String SourceAddr;
  int PRI;
  String Facility;
  String Priority; // Severity
  String DateStr;
  String HostName;
  String Tag;
  String Msg;

public:
  TSyslogMessage();
  // p must be null terminated
  bool ProcessMessageFromSyslogd(char * p, int size, sockaddr_in * from_addr);
  void ProcessMessageFromFile(char * p);
  // Write to file
  bool Save(void);
};
//---------------------------------------------------------------------------
bool IsValidSyslogDate(const char * p);
//---------------------------------------------------------------------------
#endif
