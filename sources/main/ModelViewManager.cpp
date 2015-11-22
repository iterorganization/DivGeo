#include "ModelViewManager.h"
#include "MainWnd.h"

const char* ModelViewManager::scpSessionFileName = "session.bin";

ModelViewManager::ModelViewManager( CMainWnd* _pMainWnd, const UserPreferences* _pPrefs,
                                    Console* _pConsole,
                                    StringsManager *_pSM, QWidget *parent) :
  QDockWidget(parent),
  sender_name( "DG" ),
  pMainWnd( _pMainWnd ),
  pPrefs( _pPrefs ),
  pConsole( _pConsole ),
  pSM( _pSM ),
  fileDialog( this ),
  id_model( 0 ),
  pCurrentProxy( null )
{
  setFeatures( QDockWidget::DockWidgetMovable );
  setMinimumWidth( 220 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  fileDialog.setNameFilter( QString( "DG model (%1)" ).arg( pPrefs->common.sExtFilterModel ) );
  fileDialog.setViewMode( QFileDialog::Detail );
  fileDialog.setDirectory( QApplication::applicationDirPath() );
  fileDialog.setDefaultSuffix( "dg" );

  QWidget* titleWidget = new QWidget( parent );
  setTitleBarWidget( titleWidget );

  dockWidgetContents = new ResizableWidget();
  dockWidgetContents->SetCustomHeight( 100 );
  dockWidgetContents->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  setWidget(dockWidgetContents);
  QVBoxLayout *pVerticalLayout = new QVBoxLayout( dockWidgetContents );
  pVerticalLayout->setMargin( 1 );
  pVerticalLayout->setSpacing( 1 );

  pTree = new QTreeWidget( this );
  pTree->setColumnCount( 1 );
  pTree->header()->hide();
  pTree->setTextElideMode( Qt::ElideRight );
  pTree->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  pTree->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pTree->setSelectionMode( QAbstractItemView::SingleSelection );
  pVerticalLayout->addWidget( pTree );

  connect( pTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
           this,  SLOT(SelectionChanged(QTreeWidgetItem*,int)) );
}

void ModelViewManager::Delete()
{
  SaveSessionData();
  foreach( ModelViewProxyPtr pProxy, proxies.values() )
    RemoveModel( pProxy );
}

QString ModelViewManager::SelectModelPath() const
{
  QStringList fileNames;
  fileDialog.setFileMode( QFileDialog::ExistingFile );
  if( fileDialog.exec() == QFileDialog::Accepted ) {
    fileNames = fileDialog.selectedFiles();
    if( fileNames.count() < 1 )
      return "";
    if( QFileInfo( fileNames[0] ).suffix() != "dg" )
      return "";
    return fileNames[0];
  }

  return "";
}

QString ModelViewManager::SelectAnotherPath( const QString& _crsPath )
{
  fileDialog.setFileMode( QFileDialog::AnyFile );
  fileDialog.selectFile( _crsPath );
  QString sNewPath;
  QStringList fileNames;
  do {
    if( !sNewPath.isEmpty() )
      QMessageBox::information( this,
      SM_DLG( DLG::MODEL_OPEN::TITLE ), SM_DLG( DLG::MODEL_OPEN::SAME_NAME ) );
    if( fileDialog.exec() ) {
      fileNames = fileDialog.selectedFiles();
      if( fileNames.count() < 1 )
        return "";
      if( QFileInfo( fileNames[0] ).suffix() != "dg" )
        return "";
    }
    else
      return "";
    sNewPath = fileNames[0];
  } while( sNewPath == _crsPath );

  QFile( _crsPath ).copy( sNewPath );
  return sNewPath;
}

bool ModelViewManager::LoadModel( ModelPtr _pModel, const QString& _crsPath )
{
  static const char* scList = " > ";
  int errFlags = 0;
  int err = _pModel->Load( _crsPath.toStdString(), &errFlags );

  QStringList errMsgList = pMainWnd->GetLoadErrFlagsDescription( errFlags );
  if( errMsgList.empty() && err == DGMSG::OK )
    pConsole->Send( LOG_INFO, sender_name, DG3::MODEL_LOADED_SUCCESSFULLY );
  else if( !errMsgList.empty() ){
    QString errText = SM_MSG( DG3::MODEL_LOADED_WITH_ERRORS );
    errText.append( "\n" );
    foreach( const QString& str, errMsgList ) {
      errText.append( scList );
      errText.append( str );
      errText.append( "\n" );
    }
    errText.chop( 1 );
    pConsole->Send( WND_ERROR, sender_name, errText );
  }

  return (err == DGMSG::OK);
}

ModelViewProxyPtr ModelViewManager::OpenModel( const QString& _crsPath, bool _createView )
{
  QString sFilePath = _crsPath.isEmpty() ?
                        SelectModelPath() : _crsPath;
  if( ModelIsLoaded( sFilePath ) ) {
    if( QMessageBox::information( this,
        SM_DLG( DLG::MODEL_OPEN::TITLE ), SM_DLG( DLG::MODEL_OPEN::MESSAGE ),
        QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel )
      return null;
    sFilePath = SelectAnotherPath( sFilePath );
  }

  std::string sModelName = QFileInfo( sFilePath ).completeBaseName().toStdString();
  ModelAgent* pAgent = new ModelAgent( pConsole, pSM );
  ModelPtr pModel = new Model( pAgent, sModelName );

  ModelViewProxyPtr pProxy = null;
  if( LoadModel( pModel, sFilePath ) ) {
    pModel->ActionStack().FreeUndoInfo();
    pModel->Agent()->Clear();
    if( pModel->GetFluxModel()->HasEquil() ) {
      TopologyPtr pTopology = pModel->GetFluxModel()->GetTopology();
      pTopology->RecalcCache();
      pTopology->RecalcAllGridPointSegSLines();
    }
    pModel->ActionStack().Complete( "Model loading" );
  // end LoadNormalApp

    pProxy = AddModel( pModel, pAgent );
    if( _createView )
      pProxy->CreateView();
  }
  else {
    delete pModel;
    delete pAgent;
  }

  return pProxy;
}

ModelViewProxyPtr ModelViewManager::NewModel( const QString& _crsPath, bool _createView )
{
  std::string sModelName = QString( "#%1" ).arg( GetNewModelId() ).toStdString();
  ModelAgent* pAgent = new ModelAgent( pConsole, pSM );
  ModelPtr pModel = new Model( pAgent, sModelName );
  if( !_crsPath.isEmpty() && QFile( _crsPath ).exists() ) {
    if( !LoadModel( pModel, _crsPath ) ) {
      delete pModel;
      pModel = new Model( pAgent, sModelName );
    }
  }
  else
    pConsole->Send( LOG_INFO, sender_name, ERR::NOCONFIG );

  pConsole->Send( LOG_INFO, sender_name, DG3::NEW_MODEL_CREATED,
                  QStringList( ToQString( sModelName ) ) );

  time_t timer;
  std::string timestr = asctime( localtime( (time(&timer),&timer) ) );
  unsigned timestr_len = timestr.length();
  if( timestr[timestr_len - 1] == '\n' )
    timestr[timestr_len - 1] = 0;
  pModel->SetFileName( "" );
  pModel->SetCreationTime( timestr );
  pModel->SetTopologyName( "" );
  pModel->ActionStack().FreeUndoInfo();
  ModelViewProxyPtr pProxy = AddModel( pModel, pAgent );
  if( _createView )
    pProxy->CreateView();

  return pProxy;
}

void ModelViewManager::SaveAll()
{
  foreach( ModelViewProxyPtr pProxy, proxies.values() ) {
    pProxy->SaveModel( true );
    pProxy->MarkSaved();
  }
}

bool ModelViewManager::ModelIsLoaded( const QString& _crsPath ) const
{
  foreach( ModelPtr pModel, proxies.keys() )
    if( QString::fromStdString( pModel->FileName() )
        .compare( _crsPath, Qt::CaseInsensitive ) == 0 )
      return true;
  return false;
}

void ModelViewManager::SelectionChanged( QTreeWidgetItem* current, int )
{
  QVariant qv = current->data( 0, Qt::UserRole );
  if( qv.isNull() ||
      qv.userType() != QMetaType::type( "CViewWndPtr" ) )
    return;
  emit ViewSelected( qv.value< CViewWndPtr >() );
}

ModelViewProxyPtr ModelViewManager::AddModel( ModelPtr _pModel, ModelAgent* _pAgent )
{
  if( _pModel == null || proxies.value( _pModel, null ) != null )
    return null;
  ModelViewProxyPtr pProxy = new ModelViewProxy( _pModel, pTree, this, pMainWnd, pPrefs, pConsole, pSM );
  proxies.insert( _pModel, pProxy );
  _pAgent->setParent( pProxy );

  connect( pProxy, SIGNAL(ViewCreated(CViewWndPtr)), this, SIGNAL(ViewCreated(CViewWndPtr)) );
  connect( pProxy, SIGNAL(ViewSelected(CViewWndPtr)), this, SIGNAL(ViewSelected(CViewWndPtr)) );
  connect( pProxy, SIGNAL(DeleteProxy(ModelViewProxy*)), this, SLOT(RemoveModel(ModelViewProxy*)) );

  connect( _pAgent, SIGNAL(UpdateViews(UpdateInfo)), pProxy, SLOT(UpdateViews(UpdateInfo)) );

  pTree->addTopLevelItem( pProxy->ModelItem() );
  pTree->expandAll();

  pCurrentProxy = pProxy;

  return pProxy;
}

void ModelViewManager::RemoveModel( ModelViewProxy* _pProxy )
{
  proxies.remove( proxies.key( _pProxy ) );
  delete _pProxy;
  pCurrentProxy = null;
  pTree->setCurrentItem( null );
  pTree->clearSelection();
  pMainWnd->UpdateModelInfo();
  pMainWnd->UpdateObjectInfo( null );
}

void ModelViewManager::SelectCurrentView( CViewWndPtr _pSelectedView )
{
  CViewWndPtr pCurrentView = (pCurrentProxy == null) ? null : pCurrentProxy->CurrentView();
  if( pCurrentView == _pSelectedView )
    return;
  ModelViewProxyPtr pSelectedProxy = proxies.value( _pSelectedView->GetModel(), null );
  if( _pSelectedView  == null || pSelectedProxy == null || !pSelectedProxy->HasView( _pSelectedView ) )
    return;

  // Show or hide model/view specifiec dialogs
  if( pCurrentProxy != pSelectedProxy ) { // another model
    if( pCurrentProxy != null )
      pCurrentProxy->ShowAll( false );
    pSelectedProxy->ShowAll( true );
  }
  else { // same model, another view
    if( pCurrentView != null &&
        pCurrentView->TopologyDialog() != null )
      pCurrentView->TopologyDialog()->hide();
    if( _pSelectedView->TopologyDialog() != null )
      _pSelectedView->TopologyDialog()->show();
  }

  pCurrentProxy = pSelectedProxy;
  QTreeWidgetItem* pViewItem = pCurrentProxy->ViewItem( _pSelectedView );
  pCurrentProxy->SetCurrentView( _pSelectedView );
  pTree->setCurrentItem( pViewItem );
  pTree->clearSelection();
  pViewItem->setSelected( true );
  pMainWnd->UpdateModelInfo();
}

ModelViewProxyPtr ModelViewManager::GetProxy( CViewWndPtr _pView ) const
{
  foreach( ModelViewProxyPtr pProxy, proxies.values() )
    if( pProxy->HasView( _pView ) )
      return pProxy;
  return null;
}

void ModelViewManager::UpdateAllViewsStyle()
{
  foreach( ModelViewProxyPtr pProxy, proxies.values() )
    pProxy->UpdateStyle();
}

void ModelViewManager::UpdateAllViewsGeometry()
{
  QList< ModelViewProxyPtr > lProxies = proxies.values();
  if( lProxies.count() > 0 ) {
    foreach( ModelViewProxyPtr pProxy, lProxies ) {
      pProxy->UpdateViewsGeometry();
    }
  }
}

void ModelViewManager::SaveSessionData()
{
  QFile file( pPrefs->ProfileDir() + "/" + scpSessionFileName );
  file.open( QFile::WriteOnly );
  SessionData sd;
  sd.panels = session.panels;
  QList< ModelViewProxyPtr > lProxies = proxies.values();
  if( lProxies.count() > 0 ) {
    foreach( ModelViewProxyPtr pProxy, lProxies ) {
      if( pProxy->GetModel()->FileName().empty() )
        continue;
      SessionModelRecord smr = pProxy->SaveSessionData();
      sd.AddModelRecord( smr );
    }
  }
  QByteArray ba = sd.Save();
  file.write( ba );
}

void ModelViewManager::LoadSessionData()
{
  if( not sessionModels.empty() ) {
    foreach( const QString& crArg, sessionModels )
      OpenModel( crArg );
    return;
  }
  QFile file( pPrefs->ProfileDir() + "/" + scpSessionFileName );
  if( !file.exists() )
    return;

  file.open( QFile::ReadOnly );
  QByteArray ba = file.readAll();
  session = SessionData( ba.data() );
  if( !session.IsOk() )
    return;

  QString sModelsList;
  foreach( const SessionModelRecord& crMR, session.Models() )
    sModelsList += crMR.sName + "\n";

  QString sMsg = SM_DLG( DLG::SESSION::MESSAGE ).arg( sModelsList );
  if( session.Models().count() > 0 ) {
    if( QMessageBox::question( pMainWnd,
      SM_DLG( DLG::SESSION::TITLE ), sMsg, QMessageBox::Yes, QMessageBox::No )
        == QMessageBox::No )
      return;

    for( int c = 0; c < session.Models().count(); c++ ) {
      if( !session.Models()[c].sName.isEmpty() ) {
        ModelViewProxyPtr pProxy = OpenModel( session.Models()[c].sName, false );
        if( pProxy != null )
          pProxy->LoadSessionData( session.Models()[c] );
      }
    }
  }
}

void ModelViewManager::SkipSessionBut( const QStringList& _crsPaths )
{
  sessionModels = _crsPaths;
  sessionModels.erase( sessionModels.begin() );
}
