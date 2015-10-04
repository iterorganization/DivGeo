#include "Var.h"
#include "../core/Model.h"

std::string Var::Description() const
{
  IVarOriginPtr pObj = (pVS != null) ? pVS : pOrigin;
  return pObj->Description() + "." + pVD->Descr();
}

std::string Var::ShortInfo() const
{
  switch( value.Type() ) {
  case UPtr::STR: return value.StringRef();
  case UPtr::LIST: return ToStdString( value.ListRef().size() );
  default: return "";
  }
}

std::string Var::DetailedInfo() const
{ return pModel->GetStr( STR::VAR ) + ": " + Description(); }

IComponentPtr Var::GetLockingObject() const
{
  //TODO: always will be error
  SENDER_NAME( "Var::GetLockingObject" );
  pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LOCKING_OBJECT_ERROR );
  return null;
}

Var::Var(ModelPtr _pModel, IVarOriginPtr _pOrigin, VarDefPtr _pDef, VarSetPtr _pSet ):
  IComponent( OT::VAR, _pModel ),
  pOrigin( DG_SHARE( _pOrigin ) ),
  pVD( DG_SHARE( _pDef ) ),
  pVS( DG_SHARE( _pSet ) )
{
  DG_PROPERTIES_INIT;
  if( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) )
    value.SetEmptyList();
}

Var::~Var()
{
  if( !value.IsNull() ) {
    if( value.Type() == UPtr::LIST ) {
      FOREACH_CONST( IVarItemIterConst, it, value.ListRef() )
        ((IVarItemPtr)*it)->RemoveContainer( this );
    }
    value.Release();
  }

  if( pOrigin != null ) {
    pOrigin->ExcludeVar( this );
    ReleaseObject( (IComponentPtr*)&pOrigin );
  }

  if( pVD != null ) {
    pVD->ExcludeVar( this );
    ReleaseObject( (IComponentPtr*)&pVD );
  }

  if( pVS != null ) {
    pVS->ExcludeAssociated( this );
    ReleaseObject( (IComponentPtr*)&pVS );
  }
}

void Var::FreeValue()
{
  if( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) ) {
    assert( value.ListRef().empty() );
    //delete value.ToListPtr();
  }
  value.Release();
}

void Var::Delete()
{
  SENDER_NAME( "Var::Delete" );

  if( !value.IsNull() )
    Change( UPtr() );

  ActDelVar act = ActDelVar( pModel, this );
  if( act.Do() != 0 )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );
}

void Var::Set( IVarOriginPtr _pOrigin, const UPtr& _crNewVal )
{
  pModel->Vars()->SetVar( _pOrigin, pVD, pVS, _crNewVal );
}

void Var::Change( const UPtr& _crNewVal )
{
  if( value.IsEqual( _crNewVal ) )
    return;

  if( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) ) {
    IVarItemList valueListCopy = value.ListRef();
    FOREACH_CONST( IVarItemIterConst, it, valueListCopy ) {
      if( _crNewVal.IsNull() || !Contains( _crNewVal.ListRef(), *it ) )
        ActChangeGroup( pModel, this, *it, EXCLUDE, REDRAW, DO_AT_ONCE );
    }

    if( !_crNewVal.IsNull() ) {
      FOREACH_CONST( IVarItemIterConst, it, _crNewVal.ListRef() ) {
        if( value.IsNull() || !Contains( value.ListRef(), *it ) )
          ActChangeGroup( pModel, this, *it, INCLUDE, REDRAW, DO_AT_ONCE );
        //MB: it seems to be a bug. My bug.
        //if( *it == pNewVal->List()->front() ) break;
      }
    }
  }
  else
    ActChangeValue( pModel, this, this, Var::p_value, _crNewVal, REDRAW, DO_AT_ONCE );
}

void Var::ChangeEx( IVarItemPtr _pItem, bool _include )
{
  assert( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) );
  ActChangeGroup( pModel, this, _pItem, _include, REDRAW, DO_AT_ONCE );
}

