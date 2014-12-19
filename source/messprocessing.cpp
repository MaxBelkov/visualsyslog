//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "messageform.h"
#include "messprocessing.h"
#include "fdb.h"

const char * szProcessRuleNodes = "processrules";
  const char * szProcessRuleNode = "processrule";
    extern const char * szMatchNode; //= "match";
    const char * szProcessNode = "process";

extern String WorkDir;
extern TStorageFileList * fdb;

//---------------------------------------------------------------------------
TMessProcess::TMessProcess()
{
  bIgnore = false;
  bAlarm = false;
    AlarmMess = "{time} {message}";
  bSound = false;
    SoundFile = "alarm.wav";
    PlayCount = 1;
  bSendMail = false;
  bRunProg = false;
    bProgHide = false;
  bSaveToFile = false;
  SaveFile = -1;
}
//---------------------------------------------------------------------------
String TMessProcess::GetSoundFileName(void)
{
  if( SoundFile.Length() > 0 && ExtractFilePath(SoundFile).Length() == 0 )
    return WorkDir + SoundFile;
  return SoundFile;
}
//---------------------------------------------------------------------------
String TMessProcess::GetDescription(void)
{
  String rv;
  char * a = " AND ";

  if( bIgnore )
    rv += "Ignore";
  if( bAlarm )
  {
    if( ! rv.IsEmpty() ) rv += a;
    rv += "Show alarm";
  }
  if( bSound )
  {
    if( ! rv.IsEmpty() ) rv += a;
    rv += "Play sound";
  }
  if( bSendMail )
  {
    if( ! rv.IsEmpty() ) rv += a;
    rv += "Send e-mail";
  }
  if( bRunProg )
  {
    if( ! rv.IsEmpty() ) rv += a;
    rv += "Run program";
  }
  if( bSaveToFile )
  {
    if( ! rv.IsEmpty() ) rv += a;
    rv += "Save to file \"";

    TStorageFile * sf = fdb->GetByNumber( SaveFile );
    if( sf )
      rv += sf->file + "\"";
  }
  return rv;  
}
//---------------------------------------------------------------------------
void TMessProcess::Save(XMLElementEx * p)
{
  p->wb("ignore", bIgnore);
  p->wb("alarm", bAlarm);
    p->ws("alarmmess", AlarmMess);
  p->wb("sound", bSound);
    p->ws("soundfile", SoundFile);
    p->wi("playcount", PlayCount);
  p->wb("sendmail", bSendMail);
    p->ws("recipient", Recipient);
  p->wb("runprog", bRunProg);
    p->ws("progfile", ProgFile);
    p->wb("proghide", bProgHide);
  p->wb("savetofile", bSaveToFile);
    p->wi("savefile", SaveFile);
}
//---------------------------------------------------------------------------
void TMessProcess::Load(XMLElementEx * p)
{
  bIgnore = p->rb("ignore");
  bAlarm = p->rb("alarm");
    AlarmMess = p->rs("alarmmess");
  bSound = p->rb("sound");
    SoundFile = p->rs("soundfile", "alarm.wav");
    PlayCount = p->ri("playcount");
  bSendMail = p->rb("sendmail");
    Recipient = p->rs("recipient");
  bRunProg = p->rb("runprog");
    ProgFile = p->rs("progfile");
    bProgHide = p->rb("proghide");
  bSaveToFile = p->rb("savetofile");
    SaveFile = p->ri("savefile", -1);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TMessProcessRule::TMessProcessRule()
{
  bEnable = true;
}
//---------------------------------------------------------------------------
void TMessProcessRule::Save(XMLElementEx * p)
{
  tinyxml2::XMLDocument * d = p->GetDocument();
  XMLElement * e;

  p->wb("enable", bEnable);

  e = d->NewElement(szMatchNode);
  p->InsertEndChild(e);
  Match.Save((XMLElementEx *)e);

  e = d->NewElement(szProcessNode);
  p->InsertEndChild(e);
  Process.Save((XMLElementEx *)e);
}
//---------------------------------------------------------------------------
bool TMessProcessRule::Load(XMLElementEx * p)
{
  bool rv = true;
  bEnable = p->rb("enable", true);
  const XMLElement * e;
  e = p->FirstChildElement(szMatchNode);
  if( e ) Match.Load((XMLElementEx *)e);
  else rv = false;
  e = p->FirstChildElement(szProcessNode);
  if( e ) Process.Load((XMLElementEx *)e);
  else rv = false;
  return rv;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TMessProcessRuleList::TMessProcessRuleList()
{

}
//---------------------------------------------------------------------------
__fastcall TMessProcessRuleList::~TMessProcessRuleList(void)
{
  Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessRuleList::Clear(void)
{
  for(int i=0; i<Count; i++)
    delete (TMessProcessRule *)Items[i];
  TList::Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessRuleList::Add(TMessProcessRule * p)
{
  TList::Add( p );
}
//---------------------------------------------------------------------------
TMessProcessRule * __fastcall TMessProcessRuleList::Get(int index)
{
  if( index >= 0 && index < Count )
    return (TMessProcessRule *)Items[index];
  return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMessProcessRuleList::Del(int index)
{
  if( index >= 0 && index < Count )
  {
    delete (TMessProcessRule *)Items[index];
    TList::Delete(index);
  }
}
//---------------------------------------------------------------------------
void TMessProcessRuleList::Save(String file)
{
  tinyxml2::XMLDocument doc;
  doc.InsertEndChild( doc.NewDeclaration() );

  XMLNode * hls = doc.InsertEndChild( doc.NewElement(szProcessRuleNodes) );

  XMLElement * e;
  TMessProcessRule * p;
  for(int i=0; i<Count; i++)
  {
    p = (TMessProcessRule *)Items[i];
    e = doc.NewElement(szProcessRuleNode);
    hls->InsertEndChild(e);
    p->Save((XMLElementEx *)e);
  }

  XMLError err = doc.SaveFile(file.c_str());
  if( err != XML_SUCCESS )
    ReportError2("Error saving file: \"%s\" [%d]", file.c_str(), err);
}
//---------------------------------------------------------------------------
void TMessProcessRuleList::Load(String file)
{
  XMLError err;
  try
  {
    Clear();
    tinyxml2::XMLDocument doc;
    err = doc.LoadFile(file.c_str());
    if( err == XML_SUCCESS )
    {
      XMLElement * hls;
      hls = doc.RootElement();
      if( ! hls )
        throw 1;
      if( strcmpi(hls->Name(), szProcessRuleNodes) != 0 )
        throw 1;

      TMessProcessRule * mh;
      // process rules
      for(const XMLElement * pr = hls->FirstChildElement(szProcessRuleNode);
          pr;
          pr = pr->NextSiblingElement(szProcessRuleNode))
      {
        mh = new TMessProcessRule;
        mh->Load((XMLElementEx *)pr);
        Add(mh);
      }
    }
    else
      throw 0;
  }
  catch(int i)
  {
    if( i==0 )
      ReportError2("Error reading file: \"%s\" [%d]", file.c_str(), err);
    else
      ReportError2("Incorrect format of file: \"%s\"", file.c_str());
  }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
