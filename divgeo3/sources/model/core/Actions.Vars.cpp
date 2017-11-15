#include "Actions.h"
#include "Model.h"

namespace Actions
{
int ActAddVarSetDef::Do()
{
  if( pModel->IsLocked() )
    return 1;

  VarSetDefPtr pVSD = dgtype_cast< VarSetDefPtr >( pObject );
  ActDelVarSetDef* pADVSD = new ActDelVarSetDef( pModel, pVSD );
  pModel->Vars()->IncludeObject( pObject );
  pModel->ActionStack().AddUndoRec( pADVSD );
  LogMessage( DG3::ACT_ADDED );
  return 0;
}


int ActDelVarSetDef::Do()
{
  SENDER_NAME( "ActDelVarSetDef::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarSetDefPtr pVSD = dgtype_cast< VarSetDefPtr >( pObject );
  if( pVSD->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

  assert( pVSD->VarSetsNum() == 0 );
  assert( pVSD->VarDefsNum() == 0 );
  ActAddVarSetDef* pAAVSD = new ActAddVarSetDef( pModel, pVSD );
  pModel->ExcludeObject( pObject );
  pVSD->FreeLists();
  LogMessage( DG3::ACT_DELETED );
  pModel->ActionStack().AddUndoRec( pAAVSD );
  return 0;
}

int ActChangeVarSetDef::Do()
{
  SENDER_NAME( "ActChangeVarSetDef::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarSetDefPtr pVSD = dgtype_cast< VarSetDefPtr >( pObject );

  if( pVSD->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

  long minVS_old = pVSD->MinVarSets();
  long maxVS_old = pVSD->MaxVarSets();
  ActChangeVarSetDef* pACVSD = new ActChangeVarSetDef( pModel, pVSD, pVSD->MinVarSets(), pVSD->MaxVarSets() );
  pVSD->SetVarSetsLimits( minVarSets, maxVarSets );
  pModel->ActionStack().AddUndoRec( pACVSD );

  std::StringList args;
  if( minVS_old != minVarSets ) {
    std::stringstream ss;
    ss << "min_varsets" << ": " << minVS_old << " -> " << minVarSets;
    args.push_back( ss.str() );
  }
  if( maxVS_old != maxVarSets ) {
    std::stringstream ss;
    ss << "max_varsets" << ": " << maxVS_old << " -> " << maxVarSets;
    args.push_back( ss.str() );
  }
  LogMessage( DG3::ACT_CHANGED, args );
  return 0;
}


int ActAddVarDef::Do()
{
  if( pModel->IsLocked() )
    return 1;

  VarDefPtr pVD = dgtype_cast< VarDefPtr >( pObject );
  ActDelVarDef* pADVD = new ActDelVarDef( pModel, pVD );
  pVD->IncludeToVSD();
  pModel->Vars()->IncludeObject( pObject );
  pModel->ActionStack().AddUndoRec( pADVD );
  LogMessage( DG3::ACT_ADDED );
  return 0;
}

int ActDelVarDef::Do()
{
  SENDER_NAME( "ActDelVarSetDef::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarDefPtr pVD = dgtype_cast< VarDefPtr >( pObject );
  if( pVD->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

  ActAddVarDef* pAAVD = new ActAddVarDef( pModel, pVD );
  pModel->ExcludeObject( pObject );
  pVD->ExcludeFromVSD();
  pVD->FreeLists();
  LogMessage( DG3::ACT_DELETED );
  pModel->ActionStack().AddUndoRec( pAAVD );
  return 0;
}

ActChangeVarDef::ActChangeVarDef(ModelPtr _pModel, VarDef* _pVD, VarSetDef* _pVSD,
                                 const VarDefData& _crVdData, bool doAtOnce ):
  IAction( _pModel, (IComponentPtr)_pVD, ACT::CHANGE_VARDEF ),
  pVSD( _pVSD )
{ pData = new VarDefData( _crVdData ); AT_ONCE }

ActChangeVarDef::~ActChangeVarDef()
{ delete pData; }

int ActChangeVarDef::Do()
{
  SENDER_NAME( "ActChangeVarDef::Do" );
  if( pModel->IsLocked() )
    return 1;

  if( pObject->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

  VarDefPtr pVD = dgtype_cast< VarDefPtr >( pObject );
  assert( !( (pVD->VarDefType() ^ pData->varType) & VTF::HASGROUP) || !pVD->HasVars() );
  VarSetDefPtr pVsdOld = pVD->VSD();
  VarDefData oldData = pVD->GetData();
  ActChangeVarDef* pACVD = new ActChangeVarDef( pModel, pVD, pVsdOld, oldData );
  pVD->VSD()->ExcludeVarDef( pVD );

  pVD->SetParameters( pVSD, *pData );
  pVD->VSD()->IncludeVarDef( pVD );
  pModel->ActionStack().AddUndoRec( pACVD );

  std::StringList args;
  if( pVsdOld != pVSD ) {
    std::stringstream ss;
    ss << "varsetdef" << ": " << pVsdOld->Name() << " -> " << pVSD->Name();
    args.push_back( ss.str() );
  }
  if( oldData.index != pData->index ) {
    std::stringstream ss;
    ss << "[row, column]" << ": " << oldData.index.ToString() << " -> " << pData->index.ToString();
    args.push_back( ss.str() );
  }
  if( oldData.flags != pData->flags ) {
    std::stringstream ss;
    ss << "flags" << ": " << std::hex << "0x" << oldData.flags
       << " -> " << std::hex << "0x" << pData->flags;
    args.push_back( ss.str() );
  }
  if( oldData.varType != pData->varType ) {
    std::stringstream ss;
    ss << "type" << ": " << std::hex << "0x" << oldData.varType
       << " -> " << std::hex << "0x" << pData->varType;
    args.push_back( ss.str() );
  }
  LogMessage( DG3::ACT_CHANGED, args );
  return 0;
}


int ActAddVar::Do()
{
  SENDER_NAME( "ActAddVar::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarPtr pV = dgtype_cast< VarPtr >( pObject );
  ActDelVar* pADV = new ActDelVar( pModel, pV );
  pV->VD()->IncludeVar( pV );
  if( pV->VS() != null )
    pV->VS()->IncludeAssociated( pV );
  pV->Origin()->IncludeVar( pV );
  if( HasAnyFlag( pV->VD()->VarDefType(), VTF::TARGET ) )
    pModel->SetChangeFlag( CHF::TARGETS );
  pModel->ActionStack().AddUndoRec( (IAction*)pADV );
  LogMessage( DG3::ACT_ADDED );
  return 0;
}


int ActDelVar::Do()
{
  SENDER_NAME( "ActDelVar::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarPtr pV = dgtype_cast< VarPtr >( pObject );
  if( pV->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

  ActAddVar* pAAV = new ActAddVar( pModel, pV );
  pV->Origin()->ExcludeVar( pV );
  pV->VD()->ExcludeVar( pV );
  if( pV->VS() != null )
    pV->VS()->ExcludeAssociated( pV );
  pV->FreeValue();
  if( HasAnyFlag( pV->VD()->VarDefType(), VTF::TARGET ) )
    pModel->SetChangeFlag( CHF::TARGETS );
  LogMessage( DG3::ACT_DELETED );
  pModel->ActionStack().AddUndoRec( pAAV );
  return 0;
}


int ActDelVarSet::Do()
{
  SENDER_NAME( "ActDelVarSet::Do" );
  if( pModel->IsLocked() )
    return 1;

  VarSetPtr pVS = dgtype_cast< VarSetPtr >( pObject );

  if( pVS->IsLocked() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );
  //assert(IsEmptyGroup(vs->vars));
  ActAddVarSet* pAAVS = new ActAddVarSet( pModel, pVS );
  pModel->ExcludeObject( pObject );
  pVS->ExcludeFromVSD();
  pVS->FreeVars();
  LogMessage( DG3::ACT_DELETED );
  pModel->ActionStack().AddUndoRec( pAAVS );
  return 0;
}


int ActAddVarSet::Do()
{
  if( pModel->IsLocked() )
    return 1;

  VarSetPtr pVS = dgtype_cast< VarSetPtr >( pObject );
  ActDelVarSet* pADVS = new ActDelVarSet( pModel, pVS );
  pVS->IncludeToVSD();
  pModel->Vars()->IncludeObject( pObject );
  pModel->ActionStack().AddUndoRec( pADVS );
  LogMessage( DG3::ACT_ADDED );
  return 0;
}


int ActChangeGroup::Do()
{
  if( pModel->IsLocked() )
    return 1;

  VarPtr pVar = dgtype_cast< VarPtr >( pObject );
  if( HasAnyFlag( pVar->VD()->VarDefType(), VTF::TARGET ) )
    pModel->SetChangeFlag( CHF::TARGETS );

  if( pVar->HasComponent( pItem ) == include )
    return 1;

  ActChangeGroup* pACG = new ActChangeGroup( pModel, pVar, pItem, !include, bRedraw );

  if( include ) {
    pVar->AddComponent( pItem );
    pItem->AddContainer( pVar );
  }
  else {
    pVar->DeleteComponent( pItem );
    pItem->RemoveContainer( pVar );
  }

  pModel->ActionStack().AddUndoRec( (IAction*)pACG );

  std::string sInclude = pModel->GetStr( include ? DG3::ACT_INCLUDE : DG3::ACT_EXCLUDE );
  LogMessage( DG3::ACT_GROUP, ARGS( pItem->DetailedInfo() ) + sInclude );
  return 0;
}

} // Actions

