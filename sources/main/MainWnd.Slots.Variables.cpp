#include "MainWnd.h"

void CMainWnd::UpdateVarsMenu()
{
  QMenu* pVarsMenu = mapMenus.value( MENU::VARIABLES::TITLE );
  pVarsMenu->clear();
  mapVarsAdd.clear();
  mapVarsEdit.clear();
  mapVarsRemove.clear();
  QMenu* pAddSubMenu = pVarsMenu->addMenu( CreateMenu( MENU::VARIABLES::ADD::TITLE ) )->menu();
  QMenu* pRemoveSubMenu = pVarsMenu->addMenu( CreateMenu( MENU::VARIABLES::REMOVE::TITLE ) )->menu();
  pVarsMenu->addSeparator();

  FOREACHPTRCONST( VarSetDefPtr, pVSD, pMV->CurrentModel()->Vars()->VarSetDefs() ) {
    long varSetsNum = pVSD->VarSetsNum();
    if( varSetsNum < pVSD->MaxVarSets() )
      CreateAction( pAddSubMenu, pVSD, VA::ADD );
    if( varSetsNum == 0 )
      continue;
    CreateAction( pVarsMenu, pVSD, VA::EDIT );
    if( varSetsNum > pVSD->MinVarSets() )
      CreateAction( pRemoveSubMenu, pVSD, VA::REMOVE );
  }
  if( pAddSubMenu->isEmpty() )
    pAddSubMenu->setDisabled( true );
  if( pRemoveSubMenu->isEmpty() )
    pRemoveSubMenu->setDisabled( true );
}

QAction* CMainWnd::CreateAction( QMenu* _pMenu, VarSetDefPtr _pVSD, VarAction _va )
{
  QAction* pResult = null;
  QString sVsdName = QString::fromStdString( _pVSD->Name() );
  QString sVsdDescr = QString::fromStdString( _pVSD->Descr() );

  if( _pVSD->VarSetsNum() > 1 ) {
    QMenu* pSubMenu = new QMenu( sVsdDescr );
    pSubMenu->setStatusTip( sVsdDescr );
    _pMenu->addMenu( pSubMenu );

    ulong c = 1;
    FOREACHPTRCONST( VarSetPtr, pVS, _pVSD->VarSets() ) {
      QString sVsLabel = QString::fromStdString( _pVSD->GetLayerLabel( pVS ) );
      QString sVsNumber = sVsLabel.isEmpty() ? QString( "#%1" ).arg( c ) : sVsLabel;
      c++;
      pResult = CreateAction( pSubMenu, sVsdName + sVsNumber, sVsNumber, _va );
      pResult->setData( (_va == VA::ADD) ? QVariant::fromValue( _pVSD ) :
                                           QVariant::fromValue( pVS ) );
    }
  }
  else {
    // if _va != ADD then always varSetsNum > 0
    pResult = CreateAction( _pMenu, sVsdName, sVsdDescr, _va );
    if( _va == VA::ADD )
      pResult->setData( QVariant::fromValue( _pVSD ) );
    else {
      VarSetPtr pVS = dgtype_cast< VarSetPtr >( _pVSD->VarSets().front() );
      pResult->setData( QVariant::fromValue( pVS ) );
    }
  }
  return pResult;
}

QAction* CMainWnd::CreateAction( QMenu* _pMenu, const QString& _crsName,
                                 const QString& _crsDescr, VarAction _va )
{
  QAction* pAction = null;
  switch( _va ) {
  case VA::ADD:
    pAction = _pMenu->addAction( _crsDescr, &smVarsAdd, SLOT(map()) );
    mapVarsAdd.insert( _crsName, pAction );
    smVarsAdd.setMapping( pAction, _crsName );
    break;
  case VA::REMOVE:
    pAction = _pMenu->addAction( _crsDescr, &smVarsRemove, SLOT(map()) );
    mapVarsRemove.insert( _crsName, pAction );
    smVarsRemove.setMapping( pAction, _crsName );
    break;
  case VA::EDIT:
    pAction = _pMenu->addAction( _crsDescr, &smVarsEdit, SLOT(map()) );
    mapVarsEdit.insert( _crsName, pAction );
    smVarsEdit.setMapping( pAction, _crsName );
    break;
  }

  pAction->setStatusTip( _crsDescr );
  return pAction;
}

void CMainWnd::slotAddLayer( const QString& _crsVarName )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VARIABLES::ADD_LAYER )
                  .arg( _crsVarName ) );
  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr pModel = pProxy->GetModel();

  QAction* pAction = mapVarsAdd.value( _crsVarName, null );
  if( pAction == null )
    return;
  QVariant qv = pAction->data();
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarSetDefPtr" ) )
    return;
  VarSetDefPtr pVSD = qv.value< VarSetDefPtr >();
  if( pVSD == null )
    return;

  VarSetPtr pVS = pVSD->AddVarSet();
  pModel->ActionStack().Complete( "AddLayer" );
  UpdateVarsMenu();
  pProxy->slotCreateVarsEditDlg( pVS );
}

void CMainWnd::slotRemoveLayer( const QString& _crsVarName )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VARIABLES::REMOVE_LAYER )
                  .arg( _crsVarName ) );
  QAction* pAction = mapVarsRemove.value( _crsVarName, null );
  if( pAction == null )
    return;
  QVariant qv = pAction->data();
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarSetPtr" ) )
    return;
  VarSetPtr pVS = qv.value< VarSetPtr >();
  if( pVS == null )
    return;

  pVS->Delete();
  pMV->CurrentModel()->ActionStack().Complete();
  UpdateVarsMenu();
}

void CMainWnd::slotDisplayLayer( const QString& _crsVarName )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VARIABLES::DISPLAY_LAYER )
                  .arg( _crsVarName ) );
  ModelViewProxyPtr pProxy = pMV->CurrentProxy();

  QAction* pAction = mapVarsEdit.value( _crsVarName, null );
  if( pAction == null )
    return;
  QVariant qv = pAction->data();
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarSetPtr" ) )
    return;
  VarSetPtr pVS = qv.value< VarSetPtr >();
  if( pVS == null )
    return;

  pProxy->slotCreateVarsEditDlg( pVS );
}
