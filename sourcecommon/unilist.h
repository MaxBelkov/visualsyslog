//---------------------------------------------------------------------------
#ifndef unilistH
#define unilistH

#include <vcl.h>

//---------------------------------------------------------------------------
template <class T>
class TUniList
{
private:
  TList * l;
  bool bEnableMT;
  CRITICAL_SECTION busy;  // Critical section for multi-thread access

public:
  TUniList(void);
  ~TUniList(void);
  // Start muti-thread syncronization in all methods
  void EnableMultiTreading(void);

  void Clear(void);
  T * Add(T * p);
  T * Insert(int i, T * p);
  void Delete(T * p);
  void Delete(int i);
  bool Contains(T * p);
  T * Extract(int i);

  __property T * Items[int i] = {read=Get, write=Set};
  __property int Count = {read=get_count};

private:
  T * Get(int i);
  void Set(int i, T * p);
  int get_count();
  // set/remove busy state
  inline void SetBusy(bool b);
};
//---------------------------------------------------------------------------

#include "unilist.cpp"

#endif
