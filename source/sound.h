//---------------------------------------------------------------------------
#ifndef soundThreadH
#define soundThreadH

#define MAX_SOUND_FILE_COUNT 1
//---------------------------------------------------------------------------
class TSoundQueueItem
{
public:
  String File[MAX_SOUND_FILE_COUNT];   // Звуковые файлы
  int Count;        // Сколько раз проговаривать звуки
  int Pause;        // Сколько времени ждать между проигрываниями (мсек.)
public:
  TSoundQueueItem(void);
  bool Add(String _File);
  bool HaveSound(void);
};
//---------------------------------------------------------------------------
// Максимальный размер очереди звуков на проигрывание
#define TT_QUEUE_SIZE 20
// Говорящий поток
class TSound : public TThread
{
// Очередь файлов на проигрывание  TSound
private:
  TSoundQueueItem Queue[TT_QUEUE_SIZE];
  int QueueSize;
  CRITICAL_SECTION busy;

protected:
  void __fastcall Execute();
  void __fastcall Wait(DWORD msec);

public:
  __fastcall TSound(bool CreateSuspended);
  __fastcall ~TSound(void);

public:
  bool AddToQueue(TSoundQueueItem * p);
  bool IsSilence(void);
  // Удалить из очереди проигрываний 1 задание
  void DeleteFromQueue(void);

  void Stop(void);
  bool Play(AnsiString File, int Count=1);

private:
  bool __fastcall ReadBusy(void);
  bool __fastcall ReadAlmostFree(void);

__published:
  // Проигрывание занято ?
  __property bool Busy = { read=ReadBusy };
  // Проигрывание свободно или почти свободно (1 звук 1 проигрывание) ?
  __property bool AlmostFree = { read=ReadAlmostFree };
};
//---------------------------------------------------------------------------
extern TSound * TalkingThread;
void MyPlaySound(AnsiString file);
//---------------------------------------------------------------------------
#endif

