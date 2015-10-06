//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "cfg.h"
#include "syslog.h"
#include "server.h"

extern TMainCfg MainCfg;

//---------------------------------------------------------------------------
TSyslogMessage::TSyslogMessage()
{
  PRI = -1; // not exist
}
//---------------------------------------------------------------------------
void TSyslogMessage::Clear()
{
  SourceAddr = "";
  PRI = -1;
  Facility = "";
  Priority = "";
  DateStr = "";
  HostName = "";
  Tag = "";
  Msg = "";
}
//---------------------------------------------------------------------------
bool TSyslogMessage::FromStringSyslogd(char * p, int size, sockaddr_in * from_addr)
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
    // invalid facility number not allowed: message filtering mechanism will fail
    // replace invalid facility number by LOGALERT
    if( LOG_FAC(PRI) >= LOG_NFACILITIES )
      PRI = LOG_PRI(PRI) | LOG_LOGALERT;

    Facility = getcodetext(LOG_FAC(PRI) << 3, facilitynames);
    Priority = getcodetext(LOG_PRI(PRI), prioritynames);
  }

  if( IsValidSyslogDate(p) )
  {
    DateStr = String(p, 15);
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
      HostName = String(p, i);
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
      Tag = String(p, i);
      p += i + 2;
      break;
    }
  }

  // and now - process message
  // Replace all tabs by spaces,
  // cut all carriage returns and line feeds,
  // replace all chars 0..32 by '.'
  Msg = "";
  for(int i=0; p[i]; i++)
  {
    if( p[i] == '\t' || p[i] == '\n' )
      Msg += ' ';
    else if( p[i] == '\r' )
      ;
    else if( p[i] >= 0 && p[i] < 32 )
      Msg += '.';
    else
      Msg += p[i];
  }

  if( MainCfg.bReceiveUTF8 )
    Msg = Utf8ToAnsi(Msg);

  return true;
}
//---------------------------------------------------------------------------
AnsiString TSyslogMessage::ToString(void)
{
  return SourceAddr + '\t' + DateStr + '\t' + HostName + '\t' +
  ((PRI >= 0) ? (Facility + '\t' + Priority) : AnsiString('\t')) + '\t' +
  Tag + '\t' + Msg + CR;
}
//---------------------------------------------------------------------------
void TSyslogMessage::FromString(char * p, int len)
{
  // SourceAddr = String(p, int count) 5 time faster than
  // String SourceAddr += *p

  if( ! p )
    return;
  if( ! *p )
    return;

  int i=0, c;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  SourceAddr = String(p+i-c, c);
  if( p[i]=='\t' ) i++; // skip tab char

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  DateStr = String(p+i-c, c);
  if( p[i]=='\t' ) i++;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  HostName = String(p+i-c, c);
  if( p[i]=='\t' ) i++;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  Facility = String(p+i-c, c);
  if( p[i]=='\t' ) i++;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  Priority = String(p+i-c, c);
  if( p[i]=='\t' ) i++;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  Tag = String(p+i-c, c);
  if( p[i]=='\t' ) i++;

  for(c=0; p[i]!='\t' && i<len; i++,c++);
  Msg = String(p+i-c, c);

  // -1 if gettextcode nothing found
  PRI = gettextcode(Priority.c_str(), prioritynames);
  if( PRI >= 0 )
  {
    int i = gettextcode(Facility.c_str(), facilitynames);
    if( i >= 0 )
      PRI |= i;
  }
}
//---------------------------------------------------------------------------
/*
bool TSyslogMessage::SaveToFile(const String & file)
{
  TFile out(file, GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                  OPEN_ALWAYS,
                  FILE_ATTRIBUTE_NORMAL);
  if( ! out )
    return false;
  out.ToEnd();

  return Save(out);
}
*/
/*
//---------------------------------------------------------------------------
bool TSyslogMessage::SaveToFile(TFile & out)
{
  out << ToString();
  return ! out.GetError();
}
*/
/*
//---------------------------------------------------------------------------
bool TSyslogMessage::SaveToFile(const String & file, TFile & out)
{
  if( ! out.IsOpen() )
  {
    out.Open(file, GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                   OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL);
    if( ! out )
      return false;
    out.ToEnd();
  }
  return Save(out);
}
*/
//---------------------------------------------------------------------------
String TSyslogMessage::ToStringClipboard(void)
{
  return
  DateStr + '\t' + SourceAddr + '\t' + HostName + '\t' +
  ((PRI >= 0) ? (Facility + '\t' + Priority) : String('\t')) + '\t' +
  Tag + '\t' + Msg + CR;
}
//---------------------------------------------------------------------------
// {time} {ip} {host} {facility} {priority} {tag} {message}
String TSyslogMessage::Format(String fmt)
{
  int i;
  if( (i = fmt.Pos("{time}")) > 0 )
  {
    fmt.Delete(i, 6);
    fmt.Insert(DateStr, i);
  }
  if( (i = fmt.Pos("{ip}")) > 0 )
  {
    fmt.Delete(i, 4);
    fmt.Insert(SourceAddr, i);
  }
  if( (i = fmt.Pos("{host}")) > 0 )
  {
    fmt.Delete(i, 6);
    fmt.Insert(HostName, i);
  }
  if( (i = fmt.Pos("{facility}")) > 0 )
  {
    fmt.Delete(i, 10);
    fmt.Insert(Facility, i);
  }
  if( (i = fmt.Pos("{priority}")) > 0 )
  {
    fmt.Delete(i, 10);
    fmt.Insert(Priority, i);
  }
  if( (i = fmt.Pos("{tag}")) > 0 )
  {
    fmt.Delete(i, 5);
    fmt.Insert(Tag, i);
  }
  if( (i = fmt.Pos("{message}")) > 0 )
  {
    fmt.Delete(i, 9);
    fmt.Insert(Msg, i);
  }
  return fmt;
}
//---------------------------------------------------------------------------
// sample: "Sep  2 09:46:37"
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

  // space
  if( *p++ != ' ' )
    return false;

  // day
  if( *p != ' ' && ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // space
  if( *p++ != ' ' )
    return false;

  // hour
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // :
  if( *p++ != ':' )
    return false;

  // min
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // :
  if( *p++ != ':' )
    return false;

  // sec
  if( ! isdigit(*p) )
    return false;
  if( ! isdigit(*(p+1)) )
    return false;
  p += 2;

  // space
  if( *p++ != ' ' )
    return false;

  return true;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
