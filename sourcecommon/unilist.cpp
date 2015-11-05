//---------------------------------------------------------------------------
template <class T>
TUniList<T>::TUniList(void)
{
  l = new TList;
  pcs = NULL;
}
//---------------------------------------------------------------------------
template <class T>
TUniList<T>::~TUniList(void)
{
  Clear();
  delete l;
  l = NULL;
  if( pcs )
  {
    delete pcs;
    pcs = NULL;
  }
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::EnableMultiTreading(void)
{
  if( pcs )
    return;
  pcs = new TCritSection();
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Clear(void)
{
  TCritSectionEnter cse(pcs);
/*
  for(int i=0, c=l->Count; i<c; i++)
    delete (T *)l->Items[i];
*/
  l->Clear();
}
//---------------------------------------------------------------------------
template <class T>
int TUniList<T>::Add(T * p)
{
  TCritSectionEnter cse(pcs);
  return l->Add(p);
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Insert(int i, T * p)
{
  TCritSectionEnter cse(pcs);
  l->Insert(i, p);
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Delete(T * p)
{
  TCritSectionEnter cse(pcs);
  l->Remove(p);
/*
  delete p;
*/  
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Extract(int i)
{
  TCritSectionEnter cse(pcs);
  T * rv = NULL;
  if( i>=0 && i<l->Count )
  {
    rv = (T *)l->Items[i];
    l->Delete(i);
  }
  return rv;
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::ExtractLast(void)
{
  TCritSectionEnter cse(pcs);
  T * rv = NULL;
  int c = l->Count;
  if( c > 0 )
  {
    rv = (T *)l->Items[c-1];
    l->Delete(c-1);
  }
  return rv;
}
//---------------------------------------------------------------------------
template <class T>
bool TUniList<T>::Contains(T * p)
{
  TCritSectionEnter cse(pcs);
  return l->IndexOf(p) >= 0;
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::Get(int i)
{
  TCritSectionEnter cse(pcs);
  //return GetNB(i);
  if( i>=0 && i<l->Count )
    return (T *)l->Items[i];
  return NULL;
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::Set(int i, T * p)
{
  TCritSectionEnter cse(pcs);
  if( i>=0 && i<l->Count )
    l->Items[i] = p;
}
//---------------------------------------------------------------------------
template <class T>
int TUniList<T>::GetCount(void)
{
  TCritSectionEnter cse(pcs);
  return l->Count;
}
//---------------------------------------------------------------------------
template <class T>
T * TUniList<T>::GetNB(int i)
{
  if( i>=0 && i<l->Count )
    return (T *)l->Items[i];
  return NULL;
}
//---------------------------------------------------------------------------
template <class T>
void TUniList<T>::SetNB(int i, T * p)
{
  if( i>=0 && i<l->Count )
    l->Items[i] = p;
}
//---------------------------------------------------------------------------
template <class T>
int TUniList<T>::GetCountNB(void)
{
  return l->Count;
}
//---------------------------------------------------------------------------
template <class T>
bool TUniList<T>::SetBusy(bool b)
{
  if( ! pcs )
    return false;
  if( b )
    pcs->Enter();
  else
    pcs->Leave();
  return true;  
}
//---------------------------------------------------------------------------

