#include "IManager.h"

IComponentListPtr TypeRecord::SelectList( ObjectType _ot ) const
{
  if( pList != null )
    return pList;
  else
    return pManager->SelectList( _ot );
}

bool IManager::IncludeObject( IComponentPtr _pObject )
{
  if( _pObject == null )
    return false;
  TypeMap::iterator it = types.find( _pObject->Type() );
  if( it == types.end() )
    return false;
  const TypeRecord& crTR = it->second;
  if( crTR.IsManaged() ) {
    crTR.List()->push_back( _pObject->GetPtr() );
    return true;
  }
  else
    return crTR.Manager()->IncludeObject( _pObject );
}

bool IManager::ExcludeObject( IComponentPtr _pObject )
{
  if( _pObject == null )
    return false;
  TypeMap::iterator it = types.find( _pObject->Type() );
  if( it == types.end() )
    return false;
  const TypeRecord& crTR = it->second;
  if( crTR.IsManaged() ) {
    crTR.List()->remove( _pObject );
    ReleaseObject( _pObject );
    return true;
  }
  else
    return crTR.Manager()->ExcludeObject( _pObject );
}

void IManager::Release()
{
  IComponentPtr pObj = null;
  TypeMap::const_iterator citEnd = types.end();
  for( TypeMap::iterator it = types.begin(); it != citEnd; ++it ) {
    const TypeRecord& crTR = it->second;
    if( crTR.IsManaged() ) {
      FOREACHOBJCONST( pObj, *crTR.List() ) ReleaseObject( pObj );
      crTR.List()->clear();
    }
    /*else delete crTR.Manager();*/
  }
}

int IManager::ObjectIndex( IComponentInPtr _pinObject ) const
{
  assert( _pinObject != null );
  TypeMap::const_iterator it = types.find( _pinObject->Type() );
  assert( it != types.end() );
  const TypeRecord& crTR = it->second;
  if( crTR.IsManaged() )
    return IndexOf( *crTR.List(), _pinObject );
  else
    return crTR.Manager()->ObjectIndex( _pinObject );
}

IComponentListPtr IManager::SelectList( ObjectType _ot ) const
{
   TypeMap::const_iterator it = types.find( _ot );
   assert( it != types.end() );
   const TypeRecord& crTR = it->second;
   return crTR.SelectList( _ot );
}
