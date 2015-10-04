#include "ModelViewProxy.h"
#include "MainWnd.h"

ModelViewProxy::ModelViewProxy(ModelPtr _pModel, QTreeWidget* _pTree, ModelViewManager* _pManager,
                               CMainWnd* _pMainWnd, const UserPreferences* _pPrefs,
                               Console* _pConsole, StringsManager *_pSM ):
  pTree( _pTree ),
  pManager( _pManager ),
  pMainWnd( _pMainWnd ),
  pPrefs( _pPrefs ),
  pConsole( _pConsole ),
  pSM( _pSM ),
  pModel( _pModel ),
  pTopologyView( null ),
  pCurrentView( null ),
  id_view( 0 ),

  pCreateSurfaceDlg( null ),
  pCreateGridPointDlg( null ),

  isSaved( false )
{
  QString model_str = QString( "Model-%1" ).arg( ToQString( _pModel->Name() ) );
  pModelItem = new QTreeWidgetItem( QStringList( model_str ) );
  pModelItem->setFlags( Qt::NoItemFlags );
  pModelItem->setData( 0, Qt::UserRole, QVariant::fromValue( _pModel ) );

  QTreeWidgetItem* pBtnItem = new QTreeWidgetItem();
  pBtnItem->setFlags( Qt::ItemIsEnabled );
  pModelItem->addChild( pBtnItem );
  QPushButton* pBtn = new QPushButton( SM_GUI( GUI::VIEW::ADD ) );
  pBtn->setFixedWidth( 100 );
  pTree->setItemWidget( pBtnItem, 0, pBtn );
  connect( pBtn, SIGNAL(clicked()), this, SLOT(slotCreateView()) );

  ViewScenePtr pScene = new ViewScene( _pModel, "Section", ST::SECTION, pPrefs, this );
  connect( pScene,    SIGNAL(message(MessageType,AlarmLevel,QString,QString)),
           pConsole,  SLOT(Send(MessageType,AlarmLevel,QString,QString)) );
  connect( pScene,    SIGNAL(message(MessageType,AlarmLevel,QString,ulong,QStringList)),
           pConsole,  SLOT(Send(MessageType,AlarmLevel,QString,ulong,QStringList)) );
  scenes.push_back( pScene );

  ViewTopScenePtr pTopScene = new ViewTopScene( _pModel, "Top", ST::TOP, pPrefs, this );
  connect( pTopScene, SIGNAL(message(MessageType,AlarmLevel,QString,QString)),
           pConsole,  SLOT(Send(MessageType,AlarmLevel,QString,QString)) );
  connect( pTopScene, SIGNAL(message(MessageType,AlarmLevel,QString,ulong,QStringList)),
           pConsole,  SLOT(Send(MessageType,AlarmLevel,QString,ulong,QStringList)) );
  scenes.push_back( pTopScene );

  connect( &smDialogs,  SIGNAL(mapped(QWidget*)),
           this,        SLOT(slotCloseVarsEditDlg(QWidget*)) );
}

ModelViewProxy::~ModelViewProxy()
{
  if( !isSaved )
    SaveModel( true );

  foreach( IViewScenePtr pScene, scenes )
    delete pScene;

  foreach( CViewWndPtr pView, mapViews.keys() ) {
    pView->disconnect();
    pView->setAttribute( Qt::WA_DeleteOnClose );
    pView->close();
  }

  delete pModel;
  delete pModelItem;
}


bool ModelViewProxy::SetCurrentView( CViewWndPtr _pView )
{
  if( !mapViews.contains( _pView ) )
    return false;
  pCurrentView = _pView;
  return true;
}

void ModelViewProxy::UpdateViews( const UpdateInfo& _crUI )
{
  pMainWnd->UpdateModelInfo();
  pMainWnd->UpdateActionsInfo();

  if( !_crUI.isEmpty ) {
    foreach( IViewScenePtr pScene, scenes ) {
      pScene->CreateListItems( _crUI.create );
      pScene->RemoveListItems( _crUI.remove );
      pScene->FullUpdateListItems( _crUI.update );
      pScene->StyleUpdateListItems( _crUI.ustyle );
    }
  }

  foreach( CViewWndPtr pView, mapViews.keys() )
    pView->viewport()->update();
}

void ModelViewProxy::UpdateStyle()
{
  foreach( IViewScenePtr pScene, scenes ) {
    pScene->SetupBackground();
    pScene->UpdateStyle();
  }
  foreach( CViewWndPtr pView, mapViews.keys() )
    pView->viewport()->update();
}

void ModelViewProxy::ShowAll( bool show )
{
  foreach( DlgVarsEdit* pDialog, mapVarEditDlgs.values() )
    pDialog->setVisible( show );
  if( pTopologyView != null && pTopologyView->TopologyDialog() != null )
    pTopologyView->TopologyDialog()->setVisible( show );

  if( pCreateSurfaceDlg!= null )
    pCreateSurfaceDlg->setVisible( show );
  if( pCreateGridPointDlg != null )
    pCreateGridPointDlg->setVisible( show );
}

void ModelViewProxy::UpdateScenes( ulong sfs )
{
  foreach( IViewScenePtr pScene, scenes ) {
    pScene->Build( sfs );
    pScene->SetVisibility( pScene->ShowFlags(), true );
  }
}

CViewWndPtr ModelViewProxy::CreateView()
{
  // Create view object
  QString sViewName = QString( "#%1" ).arg( id_view++ );
  CViewWndPtr pView = new CViewWnd( pModel, sViewName, pConsole, pSM, pMainWnd, pPrefs );
  pView->setAttribute( Qt::WA_DeleteOnClose );

  connect( pView, SIGNAL(UpdateViewState(CViewWnd*)), this, SLOT(UpdateViewBranch(CViewWnd*)) );
  connect( pView, SIGNAL(UpdateViews(UpdateInfo) ),   this, SLOT(UpdateViews(UpdateInfo)) );

  foreach( IViewScenePtr pScene, scenes ) {
    pView->AddScene( pScene->Type(), pScene );
    pScene->SetupBackground();
  }

  pConsole->Send( LOG_INFO, "DG", DG3::VIEW_CREATED,
            QStringList( pView->TitleName() ) << QString::fromStdString( pModel->Name() ) );


  // Add view tree item
  QTreeWidgetItem* pViewItem = new QTreeWidgetItem();
  pViewItem->setFlags( Qt::ItemIsSelectable );
  pViewItem->setData( 0, Qt::UserRole, QVariant::fromValue( pView ) );
  pModelItem->insertChild( pModelItem->childCount() - 1, pViewItem );
  mapViews.insert( pView, pViewItem );

  pView->SelectCurrentScene( ST::SECTION );
  pTree->expandItem( pViewItem );
  pTree->setCurrentItem( pViewItem );
  pTree->clearSelection();
  pViewItem->setSelected( true );

  pCurrentView = pView;
  emit ViewCreated( pView );

  return pView;
}

void ModelViewProxy::UpdateViewBranch( CViewWnd* _pView )
{
  QTreeWidgetItem* pViewItem = mapViews.value( _pView );
  for( int c = pViewItem->childCount(); c >= 0; c-- )
    pViewItem->removeChild( pViewItem->child( c ) );

  QTreeWidgetItem* pChildItem = null;
#define MVM_ADDCHILD( str ) \
  pChildItem = new QTreeWidgetItem( QStringList( str ) ); \
  pChildItem->setFlags( Qt::NoItemFlags ); \
  pViewItem->addChild( pChildItem )

  MVM_ADDCHILD( SM_GUI( GUI::VIEW::SCENE )  + ": " + _pView->CurrentScene()->Name() );
  MVM_ADDCHILD( SM_GUI( GUI::VIEW::CENTER ) + ": " + ToQString( _pView->GetCenter() ) );
  MVM_ADDCHILD( SM_GUI( GUI::VIEW::SCALE )  + ": " + ToQString( _pView->GetScale() ) );
  MVM_ADDCHILD( SM_GUI( GUI::VIEW::ANGLE )  + ": " + ToQString( _pView->GetAngleDeg() ) );
#undef MVM_ADDCHILD

  pViewItem->setText( 0, QString( "View-%1" ).arg( _pView->TitleName() ) );
}

void ModelViewProxy::RemoveView( CViewWndPtr _pView )
{
  if( _pView == null )
    return;

  // Remove view item
  pModelItem->removeChild( mapViews.value( _pView ) );
  mapViews.remove( _pView );

  pConsole->Send( LOG_INFO, "DG", DG3::VIEW_CLOSED,
              QStringList( _pView->TitleName() ) << ToQString( pModel->Name() ) );

  // Remove model if need
  if( mapViews.empty() ) {
    QString dialogText = SM_DLG( DLG::MODEL_CLOSE::NOVIEWS ).arg( ToQString( pModel->Name() ) );
    int retcode = pModel->IsUnsaved() ?
                    QMessageBox::information( (QWidget*)pMainWnd, QString( "DivGeo" ), dialogText,
                                              SM_DLG( DLG::MODEL_CLOSE::DONOTCLOSE ),
                                              SM_DLG( DLG::MODEL_CLOSE::SAVEANDCLOSE ),
                                              SM_DLG( DLG::MODEL_CLOSE::CLOSEUNSAVED ) ) :
                    QMessageBox::information( (QWidget*)pMainWnd, QString( "DivGeo" ), dialogText,
                                              SM_DLG( DLG::MODEL_CLOSE::DONOTCLOSE ),
                                              SM_DLG( DLG::BTN::CLOSE ) );
    switch( retcode ) {
    case 1: SaveModel();
    case 2: isSaved = true; emit DeleteProxy( this );
    case 0: return;
    }
  }

  pCurrentView = mapViews.begin().key();
  QTreeWidgetItem* pViewItem = mapViews.value( pCurrentView );
  pTree->setCurrentItem( pViewItem );
  pTree->clearSelection();
  pViewItem->setSelected( true );
}

void ModelViewProxy::UpdateModelName()
{
  foreach( IViewScene* pScene, scenes )
    pScene->UpdateSenderName();

  QString sModelName = QString( "Model-%1" ).arg( ToQString( pModel->Name() ) );
  pModelItem->setText( 0, sModelName );

  foreach( CViewWndPtr pView, mapViews.keys() ) {
    pView->UpdateSenderName();
    pView->UpdateWindowTitle();
    UpdateViewBranch( pView );
  }
}

void ModelViewProxy::SaveModel( bool _ask )
{
  if( !pModel->IsUnsaved() )
    return;
  if( _ask ) {
    QString sName = QString::fromStdString( pModel->Name() );
    if( QMessageBox::question( (QWidget*)pMainWnd,
    SM_DLG( DLG::MODEL_CLOSE::UNSAVED ), SM_DLG( DLG::MODEL_CLOSE::UNSAVED ).arg( sName ) ) ==
        QMessageBox::No )
      return;
  }
  QString sFileName = QString::fromStdString( pModel->FileName() );
  if( sFileName.isEmpty() )
    sFileName = pMainWnd->SelectModelFile( pModel );
  SaveModelAs( sFileName );
}

void ModelViewProxy::SaveModelAs( const QString& _crsName, bool _bCopy )
{
  SENDER_NAME_Q( QString( "ModelViewProxy " ) + _crsName );
  pModel->SetViewAttributes( 0, 0, 0, pCurrentView->ShowFlags() );
  ulong result = (ulong)pModel->Save( _crsName.toStdString(), DGFM::APP );
  if( result == 0 ) {
    pConsole->Send( LOG_INFO, QString::fromStdString( pModel->Name() ), DGMSG::FILESAVED );

    if( !_bCopy ) {
      pModel->SetFileName( _crsName.toStdString() );
      pModel->ActionStack().Complete( "SaveModel" );

      QFileInfo file( _crsName );
      QString sModelName = file.completeBaseName();

      pModel->SetTitleName( sModelName.toStdString() );
      UpdateModelName();

      pConsole->Send( LOG_INFO, SENDER, DG3::MODEL_RENAMED,
                    QStringList( ToQString( pModel->Name() ) ) );
    }

    pMainWnd->AddToRecentList( _crsName );
  }
  pMainWnd->UpdateModelInfo();
}

void ModelViewProxy::slotCreateVarsEditDlg( VarSetPtr _pVS )
{
  DlgVarsEdit* pDialog = mapVarEditDlgs.value( _pVS, null );
  if( pDialog != null ) {
    pDialog->show();
    pDialog->setFocus();
    pDialog->raise();
  }
  else {
    pDialog = new DlgVarsEdit( _pVS, pModel, pCurrentView, pConsole, pSM, pMainWnd );
    connect( pDialog, SIGNAL(sgnlHelp(int)), pMainWnd, SLOT(slotDialogHelp(int)) );
    connect( pDialog, SIGNAL(sgnlVarHelp(QString,QString)),
             pMainWnd,  SLOT(slotVarHelp(QString,QString)) );
    mapVarEditDlgs.insert( _pVS, pDialog );
    connect( pDialog, SIGNAL(finished(int)),
             &smDialogs, SLOT(map()) );
    smDialogs.setMapping( (QObject*)pDialog, (QWidget*)pDialog );
    pDialog->show();
  }
}

void ModelViewProxy::slotCloseVarsEditDlg( QWidget* _pWgt )
{
  DlgVarsEdit* pDialog = qobject_cast< DlgVarsEdit* >( _pWgt );
  pDialog->close();
  mapVarEditDlgs.remove( pDialog->GetVS() );
}

void ModelViewProxy::OpenTopologyDialog()
{
  if( pTopologyView != null ) {
    if( pCurrentView != pTopologyView )
      emit ViewSelected( pTopologyView );
    DlgTopology* pDialog = pTopologyView->TopologyDialog();
    pDialog->show();
    pDialog->raise();
    pDialog->setFocus();
  }
  else {
    pTopologyView = pCurrentView;
    pTopologyView->SetEditTopology( true );
    pTopologyView->UpdateShowFlag( SHW::XPOINTSEGS, true );
    DlgTopology* pDialog = new DlgTopology( pModel, pSM, pConsole, pMainWnd );
    connect( pDialog, SIGNAL(sgnlHelp(int)), pMainWnd, SLOT(slotDialogHelp(int)) );
    pTopologyView->SetTopologyDialog( pDialog );

    connect( pDialog, SIGNAL(finished(int)),
             this,    SLOT(slotCloseTopologyDlg()) );

    connect( pTopologyView, SIGNAL(ObjectSelected(IComponentPtr)),
             pDialog,       SLOT(LoadSelectedObject(IComponentPtr)) );

    pTopologyView->viewport()->update();
    pDialog->show();
  }
}

void ModelViewProxy::slotCloseTopologyDlg()
{
  pTopologyView->SetEditTopology( false );
  pTopologyView->UpdateShowFlag( SHW::XPOINTSEGS, false );
  pTopologyView->SetTopologyDialog( null );
  pTopologyView = null;
  pCurrentView->viewport()->update();
}

void ModelViewProxy::LoadSessionData( const SessionModelRecord& _crSMR )
{
  if( !_crSMR.IsOk() )
    return;

  if( _crSMR.Views().count() > 0 ) {
    for( int c = 0; c < _crSMR.Views().count(); c++ ) {
      CViewWndPtr pView = CreateView();
      //emit ViewSelected( pView ); // To make the view correctly paintable
      pView->LoadSessionData( _crSMR.Views()[c] );
      const quint16* tools = _crSMR.Views()[c].tools;
      pMainWnd->slotSelect( Qt::LeftButton | tools[0] );
      pMainWnd->slotSelect( Qt::MiddleButton | tools[1] );
      pMainWnd->slotSelect( Qt::RightButton | tools[2] );
    }
  }
}

void ModelViewProxy::UpdateViewsGeometry()
{
  if( mapViews.count() > 0 ) {
    foreach( CViewWndPtr pView, mapViews.keys() ) {
      pView->ApplyTransform();
      pView->slotUpdateView();
    }
  }
}

SessionModelRecord ModelViewProxy::SaveSessionData() const
{
  SessionModelRecord smr( ToQString( pModel->FileName() ) );
  if( mapViews.count() > 0 ) {
    foreach( CViewWndPtr pView, mapViews.keys() ) {
      SessionViewRecord svr = pView->SaveSessionData();
      smr.AddViewRecord( svr );
    }
  }
  return smr;
}

DlgCreateSurface* ModelViewProxy::OpenCreateSurfacesDlg()
{
  if( pCreateSurfaceDlg != null )
    pCreateSurfaceDlg->deleteLater();

  pCreateSurfaceDlg = new DlgCreateSurface( pSM, pConsole,
    pModel->OutputMode(), pCurrentView->LastExaminedSX(),
    pModel->GetFluxModel()->GetTopology()->SurfaceZones(), pMainWnd );
  connect( pCreateSurfaceDlg, SIGNAL(sgnlHelp(int)), pMainWnd, SLOT(slotDialogHelp(int)) );
  connect( pCreateSurfaceDlg, SIGNAL(signalMarkBoundingElem(int)),
           pMainWnd,          SLOT(slotMarkBoundingElem(int)) );
  pCreateSurfaceDlg->show();

  return pCreateSurfaceDlg;
}

DlgCreateGridPoint* ModelViewProxy::OpenCreateGridPointsDlg()
{
  if( pCreateGridPointDlg != null )
    pCreateGridPointDlg->deleteLater();

  pCreateGridPointDlg = new DlgCreateGridPoint( pSM, pConsole,
    pModel->OutputMode(), pCurrentView->LastExaminedGPX(),
    pModel->GetFluxModel()->GetTopology()->GridPointSegs(), pMainWnd );
  connect( pCreateGridPointDlg, SIGNAL(sgnlHelp(int)), pMainWnd, SLOT(slotDialogHelp(int)) );
  pCreateGridPointDlg->show();

  return pCreateGridPointDlg;
}

void ModelViewProxy::ChangeOutputModeForDialogs( int _om )
{
  if( pCreateGridPointDlg != null )
    pCreateGridPointDlg->setOutputMode( _om );
  if( pCreateSurfaceDlg != null )
    pCreateSurfaceDlg->setOutputMode( _om );
}
