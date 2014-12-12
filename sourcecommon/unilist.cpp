//---------------------------------------------------------------------------
template <class T>
TUniList<T>::TUniList(void)
{
  l = new TList;
  bEnableMT = false;
}
//---------------------------------------------------------------------------
template <class T>
TUniList<T>::~TUniList(void)
{
  Clear();
  delete l;
  l = NULL;
  if( bEnableMT )
  {
    DeleteCriticalSection( &busy );
    bEnableMT = false;
  }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Clear(void)
{
  SetBusy(true);
  try
  {
    for(int i=0, c=l->Count; i<c; i++)
      delete (T *)l->Items[i];
    l->Clear();
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Add(T * p)
{
  SetBusy(true);
  try
  {
    l->Add(p);
  }
  __finally
  { SetBusy(false); }
  return p;
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Insert(int i, T * p)
{
  SetBusy(true);
  try
  {
    l->Insert(i, p);
    return (T *)l->Items[i];
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Delete(T * p)
{
  SetBusy(true);
  try
  {
    l->Remove(p);
    delete p;
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Delete(int i)
{
  SetBusy(true);
  try
  {
    if( i>=0 && i<l->Count )
    {
      delete (T *)l->Items[i];
      l->Delete(i);
    }
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
bool TUniList<T>::Contains(T * p)
{
  SetBusy(true);
  try
  {
    return l->IndexOf(p)>=0;
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Extract(int i)
{
  SetBusy(true);
  T * rv = NULL;
  try
  {
    if( i>=0 && i<l->Count )
    {
      rv = (T *)l->Items[i];
      l->Delete(i);
    }
  }
  __finally
  { SetBusy(false); }
  return rv;
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Get(int i)
{
  SetBusy(true);
  try
  {
    if( i>=0 && i<l->Count )
      return (T *)l->Items[i];
    return NULL;
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Set(int i, T * p)
{
  SetBusy(true);
  try
  {
    if( i>=0 && i<l->Count )
      l->Items[i] = p;
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
int TUniList<T>::get_count()
{
  SetBusy(true);
  try
  {
    return l->Count;
  }
  __finally
  { SetBusy(false); }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::EnableMultiTreading(void)
{
  if( bEnableMT )
    return;
  InitializeCriticalSection( &busy );
  bEnableMT = true;
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::SetBusy(bool b)
{
  if( ! bEnableMT )
    return;
  if( b ) EnterCriticalSection( &busy );
  else LeaveCriticalSection( &busy );
}
//---------------------------------------------------------------------------
