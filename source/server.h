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

  // Read message from string p (string must be in syslog RFC 3164 format),
  // p must be null terminated
  bool FromStringSyslogd(char * p, int size, sockaddr_in * from_addr);

  // Format message to string (saving to file)
  AnsiString ToString(void);
  // Read message from string p[len]
  void FromString(char * p, int len);

  // Convert message to text string in clipboard format
  String ToStringClipboard(void);

  // Convert message to text string in custom format
  // fmt may contains: {time} {ip} {host} {facility} {priority} {tag} {message}
  String Format(String fmt);

  // Write to file
  //bool SaveToFile(const String & file);
  //bool SaveToFile(TFile & out);
  //bool SaveToFile(const String & file, TFile & out);
};
//---------------------------------------------------------------------------
bool IsValidSyslogDate(const char * p);
//---------------------------------------------------------------------------
#endif
