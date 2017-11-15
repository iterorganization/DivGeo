#include "IVarOrigin.h"
#include "../vars/Var.h"

void IVarOrigin::IncludeVar( VarPtr _pV )
{
  assert( !Contains( variables, _pV ) );
  variables.push_back( _pV->GetPtr() );
}

void IVarOrigin::ExcludeVar( VarPtr _pV )
{
  if( Remove( variables, _pV ) )
    ReleaseObject( (IComponentPtr)_pV );
}

void IVarOrigin::FreeVars()
{
  FOREACH_CONST( VarIter, itV, variables ) {
    ((VarPtr)*itV)->ReleaseOrigin();
    ReleaseObject( (IComponentPtr)*itV );
  }
  variables.clear();
}

void IVarOrigin::DeleteVars()
{
  VarList variables_copy = variables;
  FOREACH_CONST( VarIter, itV, variables_copy )
    ((VarPtr)*itV)->Set( this, UPtr() );
    //((VarPtr)*itV)->Delete();
  variables.clear();
}

VarPtr IVarOrigin::GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const
{
  FOREACH_CONST( VarIterConst, itV, variables )
    if( (*itV)->VD() == _inpVD && (*itV)->VS() == _inpVS )
      return (*itV);
  return null;
}
