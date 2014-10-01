//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "udp.h"
#include "syslog.h"
#include "server.h"

//---------------------------------------------------------------------------
TSyslogMessage::TSyslogMessage()
{
  PRI = -1; // not exist
}
//---------------------------------------------------------------------------
bool TSyslogMessage::ProcessMessageFromSyslogd(char * p, int size,
  sockaddr_in * from_addr)
{
  if( from_addr )
    SourceAddr = inet_ntoa(from_addr->sin_addr);
  else
    SourceAddr = "";

  PRI = -1; // not exist
  if( *p == '<' )
  {
    for(int i=1; i<5 && p[i]; i++)
    {
      if( p[i] == '>' )
      {
        PRI = atoi(p+1);
        p += i + 1;
        break;
      }
      if( ! isdigit(p[i]) )
        break;
    }
  }
  if( PRI >= 0 )
  {
    Facility = getcodetext(LOG_FAC(PRI) << 3, facilitynames);
    Priority = getcodetext(LOG_PRI(PRI), prioritynames);
  }

  if( IsValidSyslogDate(p) )
  {
    DateStr.SetLength(15);
    lstrcpyn(DateStr.c_str(), p, 16);
    p += 16; // including space
  }
  else
  {
    // month names in english in compliance to syslog rfc
    TFormatSettings fs;
    GetLocaleFormatSettings(0x0409, fs); // usa
    DateStr = FormatDateTime("mmm dd hh:nn:ss", Now(), fs);
  }

  // try to find host name
  for(int i=0; i<255 && p[i]; i++)
  {
    if( p[i] == ' ' )
    {
      // found
      HostName.SetLength(i);
      lstrcpyn(HostName.c_str(), p, i+1);
      p += i + 1;
      break;
    }
    else if( p[i] == ':' || p[i] == '[' || p[i] == ']' )
    {
      // host name not exist - this is program name
      break;
    }
  }

  // try to find program name
  for(int i=0; i<(48+2) && p[i]; i++)
  {
    if( p[i] == ':' && p[i+1] == ' ' )
    {
      // found
      Tag.SetLength(i);
      lstrcpyn(Tag.c_str(), p, i+1);
      p += i + 2;
      break;
    }
  }

  // and now - text message 
  // Replace all tabs by spaces
  // and cut line end
  for(int i=0; p[i]; i++)
  {
    if( p[i] == '\t' )
      p[i] = ' ';
    else if( p[i] == '\n' )
      p[i] = 0;
  }

  Msg = p;

  return true;
}
//---------------------------------------------------------------------------
void TSyslogMessage::ProcessMessageFromFile(char * p)
{
  for(; *p && *p!='\t'; p++)
    SourceAddr += *p;

  for(p++; *p && *p!='\t'; p++)
    DateStr += *p;

  for(p++; *p && *p!='\t'; p++)
    HostName += *p;

  for(p++; *p && *p!='\t'; p++)
    Facility += *p;

  for(p++; *p && *p!='\t'; p++)
    Priority += *p;

  for(p++; *p && *p!='\t'; p++)
    Tag += *p;

  for(p++; *p && *p!='\t'; p++)
    Msg += *p;
}
//---------------------------------------------------------------------------
bool TSyslogMessage::Save(const String & file)
{
  TFile out(file, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
  if( ! out )
    return false;
  out.SetPointer(0, FILE_END);

  return Save(out);
}
//---------------------------------------------------------------------------
bool TSyslogMessage::Save(TFile & out)
{
  out <<
    SourceAddr + '\t' + DateStr + '\t' + HostName + '\t' +
    ((PRI >= 0) ? (Facility + '\t' + Priority) : String('\t')) + '\t' +
    Tag + '\t' + Msg + CR;
  return ! out.GetError();  
}
//---------------------------------------------------------------------------
//Sep  2 09:46:37
bool IsValidSyslogDate(const char * p)
{
  char * szMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec" };
  int Month = 0;
  for(int i=0; i<12; i++)
  {
    if( strncmp(p, szMonths[i], 3) == 0 )
      Month = i + 1;
  }
  if( Month == 0 )
    return false;
  p += 3;

  // пробел
  if( *p++ != ' ' )
    return false;

  // день
  if( *p != ' ' && ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // пробел
  if( *p++ != ' ' )
    return false;

  // Час
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // :
  if( *p++ != ':' )
    return false;

  // Мин
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // :
  if( *p++ != ':' )
    return false;

  // Сек
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // пробел
  if( *p++ != ' ' )
    return false;

  return true;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
