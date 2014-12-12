//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <mmsystem.h>
#include "utils.h"
#include "sound.h"

TSound * TalkingThread = NULL;
bool WriteToLogError(String fmt, ...);

//---------------------------------------------------------------------------
TSoundQueueItem::TSoundQueueItem(void)
{
  Count = 1;
  Pause = 1000;
}
//---------------------------------------------------------------------------
bool TSoundQueueItem::Add(String _File)
{
  for(int i=0; i<MAX_SOUND_FILE_COUNT; i++)
  {
    if( File[i].Length() == 0 )
    {
      File[i] = _File;
      return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------------
bool TSoundQueueItem::HaveSound(void)
{
  for(int i=0; i<MAX_SOUND_FILE_COUNT; i++)
    if( File[i].Length() > 0 )
      return true;
  return false;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TSound::TSound(bool CreateSuspended) :
           TThread(CreateSuspended)
{
  QueueSize = 0;
  InitializeCriticalSection( &busy );
}
//---------------------------------------------------------------------------
__fastcall TSound::~TSound(void)
{
  DeleteCriticalSection( &busy );
}
//---------------------------------------------------------------------------
void __fastcall TSound::Execute()
{
  try
  {
    int SleepPause;
    while( ! Terminated )
    {
      SleepPause = 10; // По умолчанию пауза 10 мсек
      if( QueueSize > 0 )
      {
        if( Queue[0].Count > 0 )
        {
          for(int i=0; i<MAX_SOUND_FILE_COUNT && ! Terminated; i++)
          {
            if( Queue[0].File[i].Length() > 0 )
            {
              MyPlaySound(Queue[0].File[i]);
            }
          }

          SleepPause = MAX(SleepPause, Queue[0].Pause);
          Queue[0].Count--;
        }
        else
        {
          DeleteFromQueue();
        }
      }
      Wait(SleepPause);
    }
  }
  catch(const Exception & E)
  {
    WriteToLogError("ERROR\tTSound::Execute: %s", E.Message.c_str());
  }
  catch(...)
  {
    WriteToLogError("ERROR\tTSound::Execute");
  }
}
//---------------------------------------------------------------------------
void __fastcall TSound::Wait(DWORD msec)
{
  DWORD tc = GetTickCount();
  while( ! Terminated && GetTickCount()-tc < msec )
    Sleep(1);
}
//--------------------------------------------------------------------
bool TSound::AddToQueue(TSoundQueueItem * p)
{
  EnterCriticalSection( &busy );
  bool bRetVal = false;
  if( p && QueueSize < TT_QUEUE_SIZE )
  {
    Queue[QueueSize] = *p;

    // Такой файл существует ?
    for(int i=0; i<MAX_SOUND_FILE_COUNT; i++)
    {
      if( Queue[QueueSize].File[i].Length() == 0 )
        continue;

      if( ! FileExists(Queue[QueueSize].File[i]) )
      {
        WriteToLogError("WARNING\tPlay sound: file \"%s\" not found !",
          Queue[QueueSize].File[i].c_str());
      }
      else
      {
        bRetVal = true;
      }
    }

    if( bRetVal )
      QueueSize++;
  }
  LeaveCriticalSection( &busy );
  return bRetVal;
}
//--------------------------------------------------------------------
void TSound::DeleteFromQueue(void)
{
  if( QueueSize <= 0 )
    return;

  EnterCriticalSection( &busy );
  for(int i=1; i<QueueSize; i++)
  {
    Queue[i-1] = Queue[i];
  }
  QueueSize--;
  LeaveCriticalSection( &busy );
}
//---------------------------------------------------------------------------
bool TSound::IsSilence(void)
{
  return QueueSize==0;
}
//---------------------------------------------------------------------------
void TSound::Stop(void)
{
  EnterCriticalSection( &busy );
  QueueSize = 0;
  LeaveCriticalSection( &busy );
}
//---------------------------------------------------------------------------
bool TSound::Play(AnsiString File, int Count)
{
  TSoundQueueItem t;
  t.Add(File);
  t.Count = Count;
  return AddToQueue(&t);
}
//---------------------------------------------------------------------------
bool __fastcall TSound::ReadBusy(void)
{
  return ! IsSilence();
}
//---------------------------------------------------------------------------
bool __fastcall TSound::ReadAlmostFree(void)
{
  bool rv = false;
  EnterCriticalSection( &busy );
  if( QueueSize == 0 )
    rv = true;
  else if( QueueSize == 1 )
  {
    if( Queue[0].Count < 2 )
      rv = true;
  }
  LeaveCriticalSection( &busy );
  return rv;
}
//---------------------------------------------------------------------------
void MyPlaySound(AnsiString file)
{
  if( file.Length() == 0 )
    return;
  if( ! FileExists(file) )
  {
    WriteToLogError("WARNING\tPlay sound: file \"%s\" not found !", file.c_str());
    return;
  }
  if( ::PlaySound(file.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_NOWAIT | SND_SYNC) == FALSE )
    WriteToLogError("ERROR\tPlay sound: \"%s\"", FormatLastError2(GetLastError()).c_str());
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

