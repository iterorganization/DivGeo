#include "VarSetDef.h"
#include "../core/Model.h"

VarSetDef::~VarSetDef()
{
  FreeLists();
}

std::string VarSetDef::Description() const
{ return data.sDescr; }

std::string VarSetDef::ShortInfo() const
{ return data.sName; }

std::string VarSetDef::DetailedInfo() const
{ return pModel->GetStr( STR::VARSETDEF ) + ": " + data.sDescr; }

void VarSetDef::ChangeHelpString( const std::string& _crsHelp )
{
  if( sHelp != _crsHelp )
    ActChangeValue( pModel, this, this, p_help, _crsHelp, REDRAW, DO_AT_ONCE );
}

int VarSetDef::Change( const VarSetDefData& _crData )
{
  SENDER_NAME( "VarSetDef::Change" );
  int err = 0;

  ActChangeVarSetDef acvsd( pModel, this, _crData.minVarSets, _crData.maxVarSets );
  if( acvsd.Do() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );

  ActChangeValue( pModel, this, this, p_name, _crData.sName, REDRAW, DO_AT_ONCE );
  ActChangeValue( pModel, this, this, p_descr, _crData.sDescr, REDRAW, DO_AT_ONCE );

/*  int i;
  VarSet vs;
  Index ix;
  if (i=GroupCount(pVSD->varSets),i<minVarSets || i>maxVarSets)
  FatalError("ChangeVarSetDef()-%s minmax: fatal error 1",pVSD->name); */

  return err;
}


void VarSetDef::Delete()
{
  ChangeHelpString( std::string( "" ) );

  Change( VarSetDefData( "", "", 0, data.maxVarSets ) );

  FOREACH_CONST( IComponentIterConst, itVD, varDefs )
    (*itVD)->Delete();
  varDefs.clear();
  FOREACH_CONST( IComponentIterConst, itVS, varSets )
    (*itVS)->Delete();
  varSets.clear();

  ActDelVarSetDef( pModel, this, DO_AT_ONCE );
}

VarDefPtr VarSetDef::AddVarDef(const VarDefData& _crVdData )
{
  SENDER_NAME( "VarSetDef::AddVarDef" );
  VarDefPtr pVD = new VarDef( pModel, this, _crVdData.varType, _crVdData.flags, _crVdData.index );

  ActAddVarDef actAVD = ActAddVarDef( pModel, pVD );

  if( actAVD.Do() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );

  ActChangeValue( pModel, pVD, pVD, VarDef::p_name, _crVdData.sName, REDRAW, DO_AT_ONCE );
  ActChangeValue( pModel, pVD, pVD, VarDef::p_descr, _crVdData.sDescr, REDRAW, DO_AT_ONCE );

  return pVD;
}

VarDefPtr VarSetDef::AddVarDef( const std::string& _crsName, const std::string& _crsDescr,
                     VarType _varType, long _flags, const NPoint& _crIndex )
{
  return AddVarDef( VarDefData( _crsName, _crsDescr, _varType, _flags, _crIndex ) );
}

VarSetPtr VarSetDef::AddVarSet()
{
  SENDER_NAME( "VarSetDef::AddVarSet" );
  if( data.maxVarSets <= varSets.size() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::TOO_MANY_VARSETS );

  VarSetPtr pVS = new VarSet( pModel );
  pVS->SetVarSetDef( this );

  ActAddVarSet actAVS = ActAddVarSet( pModel, pVS );

  if( actAVS.Do() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::MODEL_IS_LOCKED );

  return pVS;
}

IComponentPtr VarSetDef::GetLockingObject() const
{
  SENDER_NAME( "VarSetDef::GetLockingObject" );
  FOREACHPTRCONST( VarDefPtr, pVD, varDefs )
    if( pVD->IsLocked() )
      return pVD->GetLockingObject();
  pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LOCKING_OBJECT_ERROR );
  return null;
}

void VarSetDef::FreeLists()
{
  IComponentPtr pObject = null;
  FOREACHOBJCONST( pObject, varSets ) {
    VarSetPtr pVS = dgtype_cast< VarSetPtr >( pObject );
    pVS->ReleaseVSD();
    ReleaseObject( pObject );
  }
  varSets.clear();

  FreeVars();

  FOREACHOBJCONST( pObject, varDefs ) {
    VarDefPtr pVD = dgtype_cast< VarDefPtr >( pObject );
    pVD->ReleaseVSD();
    ReleaseObject( pObject );
  }
  varDefs.clear();
}

void VarSetDef::IncludeVarDef( VarDefPtr pVD )
{
  varDefs.push_back( DG_SHARE( pVD ) );
}

void VarSetDef::ExcludeVarDef( VarDefPtr _pVD )
{
  FOREACHPTR( VarDefPtr, pVD, varDefs ) {
    if( pVD == _pVD ) {
      ERASE_CURRENTPTR( varDefs );
      ReleaseObject( (IComponentPtr)_pVD );
      break;
    }
  }
}

void VarSetDef::IncludeVarSet( VarSetPtr pVS )
{
  varSets.push_back( DG_SHARE( pVS ) );
}

void VarSetDef::ExcludeVarSet( VarSetPtr _pVS )
{
  FOREACHPTR( VarSetPtr, pVS, varSets ) {
    if( pVS == _pVS ) {
      ERASE_CURRENTPTR( varSets );
      ReleaseObject( (IComponentPtr)_pVS );
      break;
    }
  }
}

VarDefPtr VarSetDef::GetVarDefByPosition( int _row, int _column ) const
{
  FOREACHPTRCONST( VarDefPtr, pVD, varDefs ) {
    if( pVD->Row() == _row && pVD->Column() == _column )
      return pVD;
  }
  return null;
}

VarDefPtr VarSetDef::GetVarDefByType( int type ) const
{
  FOREACHPTRCONST( VarDefPtr, pVD, varDefs ) {
    if( HasEachFlag( pVD->VarDefType(), type ) )
      return pVD;
  }
  return null;
}

void VarSetDef::AdjustVarSetList()
{
  unsigned i = varSets.size();
  if( i < data.minVarSets )
    for( ; i < data.minVarSets; i++ )
      AddVarSet();
  else if( i > data.maxVarSets )
    for( ; i > data.maxVarSets; i-- )
      varSets.front()->Delete();
}

int VarSetDef::CheckAllVars( VarPairList* pErrorPairs, VarDefPtr pVD ) const
{
  int r = 0, errCode = 0;
  FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
    if( pModel->Vars()->CheckVar( NULL, pVD, pVS, &errCode ) ) {
      r++;
      if( pErrorPairs != NULL )
        pErrorPairs->push_back( VarPair( pVD, pVS, errCode ) );
    }
  }
  return r;
}

bool VarDefCompare(const IComponentPtr _pVD1, const IComponentPtr _pVD2 )
{
  VarDefPtr pVD1 = dgtype_cast< VarDefPtr >( _pVD1 );
  VarDefPtr pVD2 = dgtype_cast< VarDefPtr >( _pVD2 );
  if( pVD1->Column() != pVD2->Column() )
    return pVD1->Column() < pVD2->Column();
  else
    return pVD1->Row() < pVD2->Row();
}

IComponentList VarSetDef::GetSortedVarDefs() const
{
  IComponentList varDefs_tmp( varDefs );
  varDefs_tmp.sort( VarDefCompare );
  return varDefs_tmp;
}

VarPtr VarSetDef::GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const
{
  UNUSED( _inpVS );
  FOREACH( VarIterConst, itV, variables )
    if( (*itV)->VD() == _inpVD )
      return (*itV);
  return null;
}
