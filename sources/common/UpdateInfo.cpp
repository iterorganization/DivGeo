#include "UpdateInfo.h"
#include "./model/core/IComponent.h"

UpdateInfo::UpdateInfo( IComponentPtr _pObject, UpdateType _ut )
{
  isEmpty = false;
  switch( _ut ) {
  case UT::CREATE: create.push_back( _pObject->GetPtr() ); break;
  case UT::REMOVE: remove.push_back( _pObject->GetPtr() ); break;
  case UT::UPDATE: update.push_back( _pObject->GetPtr() ); break;
  case UT::USTYLE: ustyle.push_back( _pObject->GetPtr() ); break;
  }
}

UpdateInfo::UpdateInfo( const IComponentList& _crList, UpdateType _ut )
{
  isEmpty &= _crList.empty();
  IComponentListPtr pList = null;
  switch( _ut ) {
  case UT::CREATE: pList = &create; break;
  case UT::REMOVE: pList = &remove; break;
  case UT::UPDATE: pList = &update; break;
  case UT::USTYLE: pList = &ustyle; break;
  }
  IComponentPtr FOREACHOBJCONST( pObj, _crList )
    pList->push_back( pObj->GetPtr() );
}

void UpdateInfo::AddToCreate( IComponentPtr _pObject )
{
  if( _pObject == null )
    return;
  IComponentIter it = std::find( create.begin(), create.end(), _pObject );
  if( it != create.end() )
    return;

  it = std::find( remove.begin(), remove.end(), _pObject );
  if( it != remove.end() ) {
    remove.erase( it );
    ReleaseObject( _pObject );
    return;
  }

  it = std::find( update.begin(), update.end(), _pObject );
  assert( it == update.end() );

  it = std::find( ustyle.begin(), ustyle.end(), _pObject );
  assert( it == ustyle.end() );

  create.push_back( _pObject->GetPtr() );
  isEmpty = false;
}

void UpdateInfo::AddToRemove( IComponentPtr _pObject )
{
  if( _pObject == null )
    return;
  IComponentIter it = std::find( create.begin(), create.end(), _pObject );
  if( it != create.end() ) {
    create.erase( it );
    ReleaseObject( _pObject );
    return;
  }

  it = std::find( remove.begin(), remove.end(), _pObject );
  if( it != remove.end() )
    return;

  it = std::find( update.begin(), update.end(), _pObject );
  if( it != update.end() ) {
    update.erase( it );
    ReleaseObject( _pObject );
  }
  else {
    it = std::find( ustyle.begin(), ustyle.end(), _pObject );
    if( it != ustyle.end() ) {
      ustyle.erase( it );
      ReleaseObject( _pObject );
    }
  }

  remove.push_back( _pObject->GetPtr() );
  isEmpty = false;
}

void UpdateInfo::AddToUpdate( IComponentPtr _pObject, bool _fullUpdate )
{
  if( _pObject == null )
    return;
  IComponentIter it = std::find( create.begin(), create.end(), _pObject );
  if( it != create.end() )
    return;

  it = std::find( remove.begin(), remove.end(), _pObject );
  if( it != remove.end() )
    return;

  it = std::find( update.begin(), update.end(), _pObject );
  if( it != update.end() )
    return;

  it = std::find( ustyle.begin(), ustyle.end(), _pObject );
  if( it != ustyle.end() ) {
    if(  _fullUpdate ) {
      ustyle.erase( it );
      ReleaseObject( _pObject );
    }
    else
      return;
  }

  _fullUpdate ?
        update.push_back( _pObject->GetPtr() ):
        ustyle.push_back( _pObject->GetPtr() );
  isEmpty = false;
}

void UpdateInfo::Clear() {
  IComponentPtr pObj = null;
  FOREACHOBJCONST( pObj, create ) ReleaseObject( pObj );
  FOREACHOBJCONST( pObj, remove ) ReleaseObject( pObj );
  FOREACHOBJCONST( pObj, update ) ReleaseObject( pObj );
  FOREACHOBJCONST( pObj, ustyle ) ReleaseObject( pObj );
  create.clear();
  remove.clear();
  update.clear();
  ustyle.clear();
  isEmpty = true;
}

void UpdateInfo::Append( const UpdateInfo& _crUI ) {
  IComponentPtr pObj = null;
  FOREACHOBJCONST( pObj, _crUI.create ) AddToCreate( pObj );
  FOREACHOBJCONST( pObj, _crUI.remove ) AddToRemove( pObj );
  FOREACHOBJCONST( pObj, _crUI.ustyle ) AddToUpdate( pObj );
  FOREACHOBJCONST( pObj, _crUI.update ) AddToUpdate( pObj );
  isEmpty &= _crUI.isEmpty;
}

void UpdateInfo::Append( IComponentPtr _pObject, UpdateType _ut )
{
  Append( UpdateInfo( _pObject, _ut ) );
}

void UpdateInfo::Append( const IComponentList& _crList, UpdateType _ut )
{
  Append( UpdateInfo( _crList, _ut ) );
}
