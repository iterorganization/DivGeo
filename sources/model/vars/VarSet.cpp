#include "VarSet.h"
#include "../core/Model.h"

VarSet::~VarSet()
{
  FOREACH_CONST( VarIterConst, iV, associated ) {
    ((VarPtr)*iV)->ReleaseVarSet();
    ReleaseObject( (IComponentPtr)*iV );
  }

  FreeVars();

  if( pVarSetDef != null ) {
    pVarSetDef->ExcludeVarSet( this );
    ReleaseObject( (IComponentPtr*)&pVarSetDef );
  }
}

std::string VarSet::Description() const
{
  VarDefPtr pVD = null;
  FOREACHPTRCONST( VarDefPtr, pVD_tmp, pVarSetDef->VarDefs() ) {
    if( HasAnyFlag( pVD_tmp->Flags(), VF::LAYERINDEX ) ) {
      pVD = pVD_tmp;
      break;
    }
  }

  std::stringstream ss;
  ss << pVarSetDef->Descr();
  if( pVD != null ) {
    UPtr val = pModel->Vars()->GetVar( this, pVD, null );
    ss << " " << (val.IsNull() ? "null" : val.StringRef()); //1411 ()
  }
  else if( pVarSetDef->VarSetsNum() > 1 )
    ss << " #" << pVarSetDef->IndexOf( this ) + 1; //1411 index from '1'

  return ss.str();
}

std::string VarSet::ShortInfo() const
{ return ToStdString( variables.size() ); }

std::string VarSet::DetailedInfo() const
{ return pModel->GetStr( STR::VARSET ) + ": " + Description(); }

void VarSet::IncludeToVSD()
{
  pVarSetDef->IncludeVarSet( this );
}

void VarSet::ExcludeFromVSD()
{
  pVarSetDef->ExcludeVarSet( this );
}

void VarSet::ReleaseVSD()
{
  ReleaseObject( (IComponentPtr*)&pVarSetDef );
}

IComponentPtr VarSet::GetLockingObject() const
{
  SENDER_NAME( "VarSet::GetLockingObject" );
  FOREACH_CONST( VarIterConst, iV, variables ) {
    if( (*iV)->IsLocked() )
      return (*iV)->GetLockingObject();
  }
  pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LOCKING_OBJECT_ERROR );
  return null;
}

void VarSet::Delete()
{
  SENDER_NAME( "VarSet::Delete" );
  if( !pVarSetDef->CheckVarSetsNum() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::WRONG_VARSETS_NUMBER );

  DeleteVars();

  ActDelVarSet actDVS( pModel, this );
  if( actDVS.Do() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );
}

void VarSet::SetVarSetDef( VarSetDefPtr _pVsd )
{
  ReleaseObject( (IComponentPtr*)&pVarSetDef );
  pVarSetDef = DG_SHARE( _pVsd );
}

void VarSet::IncludeAssociated( VarPtr _pVar )
{
  assert( !Contains( associated, _pVar ) );
  associated.push_back( _pVar->GetPtr() );
}

void VarSet::ExcludeAssociated( VarPtr _pVar )
{
  if( Remove( associated, _pVar ) )
    ReleaseObject( (IComponentPtr)_pVar );
}

VarPtr VarSet::GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const
{
  UNUSED( _inpVS );
  FOREACH_CONST( VarIterConst, itV, variables )
    if( (*itV)->VD() == _inpVD )
      return (*itV);
  return null;
}
