
#include "critsection.h"

//---------------------------------------------------------------------------
TCritSection::TCritSection()
{
  InitializeCriticalSection( &busy );
}
//---------------------------------------------------------------------------
TCritSection::~TCritSection()
{
  DeleteCriticalSection( &busy );
}
//---------------------------------------------------------------------------
void TCritSection::Enter(void)
{
  EnterCriticalSection( &busy );
}
//---------------------------------------------------------------------------
BOOL TCritSection::TryEnter(void)
{
  return TryEnterCriticalSection( &busy );
}
//---------------------------------------------------------------------------
void TCritSection::Leave(void)
{
  LeaveCriticalSection( &busy );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TCritSectionEnter::TCritSectionEnter(TCritSection * _pcs) : pcs(_pcs)
{
  if( pcs )
    pcs->Enter();
}
//---------------------------------------------------------------------------
TCritSectionEnter::~TCritSectionEnter()
{
  if( pcs )
    pcs->Leave();
}
//---------------------------------------------------------------------------

