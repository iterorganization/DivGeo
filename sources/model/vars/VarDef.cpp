#include "VarDef.h"
#include "../core/Model.h"

VarDef::VarDef( ModelPtr _pModel, VarSetDefPtr _pVSD,
        VarType _varType, long _flags, const NPoint& _crIndex ):
  IComponent( OT::VARDEF, _pModel ),
  pVarSetDef( DG_SHARE( _pVSD ) ),
  data( "", "", _varType, _flags, _crIndex ),
  help( "" )
{ DG_PROPERTIES_INIT; }

VarDef::~VarDef()
{
  if( pVarSetDef != null ) {
    pVarSetDef->ExcludeVarDef( this );
    ReleaseObject( (IComponentPtr*)&pVarSetDef );
  }

  FOREACH_CONST( VarIterConst, iV, vars ) {
    ((VarPtr)*iV)->ReleaseVarDef();
    ReleaseObject( (IComponentPtr)*iV );
  }
}

std::string VarDef::Description() const
{ return pVarSetDef->Descr() + "." + data.sDescr; }

std::string VarDef::ShortInfo() const
{ return data.index.ToString() + " " + data.sName; }

std::string VarDef::DetailedInfo() const
{ return pModel->GetStr( STR::VARDEF ) + ": " + Description(); }

void VarDef::IncludeToVSD()
{
  pVarSetDef->IncludeVarDef( this );
}

void VarDef::ExcludeFromVSD()
{
  pVarSetDef->ExcludeVarDef( this );
}

void VarDef::ReleaseVSD()
{
  ReleaseObject( (IComponentPtr*)&pVarSetDef );
}


VarPtr VarDef::GetVarWithObject( IVarItemPtr _pObj ) const
{
  // Find first Var with object
  FOREACH_CONST( VarIterConst, iV, vars ) {
    IVarItemListPtr pObjects = (*iV)->Value().ListPtr();
    if( pObjects != null && Contains( *pObjects, _pObj ) )
      return (*iV);
  }
  return null;
}

IComponentPtr VarDef::GetLockingObject() const
{
  SENDER_NAME( "VarDef::GetLockingObject" );
  FOREACH_CONST( VarIterConst, iV, vars )
    if( (*iV)->IsLocked() )
      return (*iV)->GetLockingObject();
  pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LOCKING_OBJECT_ERROR );
  return null;
}

void VarDef::FreeLists()
{
  vars.clear();
}

void VarDef::IncludeVar( VarPtr pV )
{
  vars.push_back( DG_SHARE( pV ) );
}

void VarDef::ExcludeVar( VarPtr pV )
{
  FOREACH( VarIter, iV, vars ) {
    if( (*iV) == pV ) {
      ERASE( iV, vars );
      ReleaseObject( (IComponentPtr)pV );
      break;
    }
  }
}

void VarDef::ChangeHelpString( const std::string &_help )
{
  if( help.compare( _help ) != 0 )
    ActChangeValue( pModel, this, this, p_help, _help, REDRAW, DO_AT_ONCE );
}

void VarDef::ChangeEnumString( const std::string &_enum )
{
  if( enumValues.compare( _enum ) != 0 )
    ActChangeValue( pModel, this, this, p_enum, _enum, REDRAW, DO_AT_ONCE );
}

void VarDef::Delete()
{
  VarList vars_copy = vars;
  FOREACH_CONST( VarIterConst, iV, vars_copy )
    (*iV)->Delete();
  vars_copy.clear();

  ChangeHelpString( std::string("") );
  ChangeEnumString( std::string("") );

  Change( pVarSetDef, data );

  ActDelVarDef( pModel, this, DO_AT_ONCE );
}

int VarDef::Change( VarSetDefPtr _pVSD, const VarDefData& _crOther )
{
  SENDER_NAME( "VarDef::Change" );
  int err=0;

  ValidatePtr( _pVSD, "ChangeVarDef__" );

  if( HasAnyFlag( (_crOther.flags ^ data.flags), VF::MULTIPLE ) ||
      HasAnyFlag( (_crOther.varType ^ data.varType), VTF::HASGROUP ) ||
      ( HasAnyFlag( (_crOther.varType | data.varType), VTF::HAS_MESH_OBJECTS )
        && _crOther.varType != data.varType ) ||
      _pVSD != pVarSetDef ) {
    VarList vars_copy = vars;
    FOREACH_CONST( VarIterConst, iV, vars_copy )
      (*iV)->Delete();
    vars_copy.clear();
    vars.clear();
    err = STR::VARSLOST;
  }

  ActChangeVarDef actCVD( pModel, this, _pVSD, _crOther );
  if( actCVD.Do() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );

  VarList vars_copy = vars;
  FOREACH_CONST( VarIterConst, itV, vars_copy ) {
    const UPtr& crVal = (*itV)->Value();
    if( !crVal.IsEmpty() && pModel->Vars()->CheckValue( crVal, data.varType, null ) ) {
      (*itV)->Delete();
      err=STR::VARSLOST;
    }
  }
  vars_copy.clear();

  return err;

}



bool VarDef::HasLockedVars( VarSetPtr pVS, std::string& reason ) const
{
  reason.clear();
  FOREACH_CONST( VarIterConst, itV, vars ) {
    if( ((VarPtr)(*itV))->VS() == pVS && ((VarPtr)(*itV))->IsLocked() ) {
      reason = ((VarPtr)(*itV))->GetLockingObject()->Description();
      return true;
    }
  }
  return false;
}

void VarDef::SetParameters( VarSetDefPtr _pVSD, const VarDefData& _crOther )
{
  ReleaseObject( (IComponentPtr*)&pVarSetDef );
  pVarSetDef = DG_SHARE( _pVSD );
  data.varType = _crOther.varType;
  data.flags = _crOther.flags;
  data.index = _crOther.index;
}
