#include "IVarItem.h"
#include "../vars/Var.h"

void IVarItem::AddContainer( VarPtr _pV ) {
  assert( !Contains( containers, _pV ) );
  containers.push_back( _pV->GetPtr() );
}

void IVarItem::RemoveContainer( VarPtr _pV ) {
  Remove( containers, _pV );
  ReleaseObject( (IComponentPtr)_pV );
}


void IVarItem::FreeContainers() {
  FOREACH_CONST( VarIter, itV, containers ) {
    ((VarPtr)*itV)->DeleteComponent( this );
    ReleaseObject( (IComponentPtr)*itV );
  }
  containers.clear();
}

void IVarItem::DeleteContainers() {
  VarList containers_copy = containers;
  FOREACH_CONST( VarIter, itV, containers_copy )
    ((VarPtr)*itV)->ChangeEx( this, false );
}

/* Check if gPart is the part of gStructure */
bool CheckStructurePart( const IVarItemList& _crStructure, const IVarItemList& _crPart, IVarItemPtr* _ppErrObj ) {
  FOREACH_CONST( IVarItemIterConst, it, _crPart ) {
    if( !Contains( _crStructure, *it ) ) {
      if( _ppErrObj != null )
        *_ppErrObj = *it;
      return false;
    }
  }
  return true;
}

