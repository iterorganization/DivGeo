#include "../main/MainWnd.h"
#include "ViewWnd.h"

CViewWnd::CViewWnd(ModelPtr _pModel, const QString &_name,
                    Console* _pConsole, StringsManager* _pSM,
                    CMainWnd* _pMainWnd, const UserPreferences* _pPrefs ):
  QGraphicsView( _pMainWnd ),
  pConsole( _pConsole ),
  pSM( _pSM ),
  pMainWnd( _pMainWnd ),
  pPrefs( _pPrefs ),
  pModel( _pModel ),
  pScene( null ),
  pTool( null ),
  pTopologyDialog( null ),

  pLastExaminedSX( null ),
  pLastExaminedGPX( null ),

  toolId_left( TOOLS::DEFAULT_LEFT_TOOL ),
  toolId_middle( TOOLS::DEFAULT_MID_TOOL ),
  toolId_right( TOOLS::DEFAULT_RIGHT_TOOL ),
  currentButton( Qt::NoButton ),

  bReleaseQueued( false ),
  bDoubleClickProcceed( false )
{
  setMouseTracking( true );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setOptimizationFlags(QGraphicsView::DontSavePainterState);
  setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  setRenderHint(QPainter::Antialiasing, false);
  setDragMode(QGraphicsView::RubberBandDrag);

  pState = &state;

  title_name = _name;
  UpdateSenderName();

  // Connects
  connect( &doubleClickTimer, SIGNAL(timeout()), this, SLOT(mousePress()));
  connect( (QObject*)horizontalScrollBar(), SIGNAL( sliderMoved(int) ), this, SLOT( slotUpdateView() ) );
  connect( (QObject*)verticalScrollBar(), SIGNAL( sliderMoved(int) ), this, SLOT( slotUpdateView() ) );

  // Initialization
  /* TODO: convert state format from dg2 (min, max) to dg3 (center, scale)
  state.SetMinMaxPositions( pModel->MinPosition(), pModel->MaxPosition() );
  state.SetAngle( pModel->XYAngle() );
  state.center = mapToScene( rect().center() );*/
  showFlags = pModel->ShowFlags();

  // Update adjusted positions
  // TODO: pModel->SetViewAttributes( state.posMin, state.posMax, state.xyAngle, showFlags );
}

CViewWnd::~CViewWnd()
{
  //pScene = null;
}

void CViewWnd::AddScene( SceneType _sceneType, IViewScenePtr _pScene )
{
  if( _pScene != null ) {
    mapScenes.insert( _sceneType, _pScene );
    _pScene->SetViewState( pState );
  }
}

void CViewWnd::SelectCurrentScene( SceneType _sceneType )
{
  if( _sceneType == ST::TOP )
    showFlags |= SHW::TOPVIEW;
  else
    showFlags &= ~SHW::TOPVIEW;

  pScene = mapScenes.value( _sceneType, pScene );
  UpdateWindowTitle();
  this->setScene( pScene );
  state.viewPoly = mapToScene( rect() );
  state.viewRect = rect();
  pScene->SetViewState( pState );
  pScene->SetVisibility( showFlags, true );

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_SCENE,
            QStringList( pScene->Name() ) );

  emit UndoStackEnabled( pScene->HasUndoStateRecords() );
  ApplyTransform();
  slotUpdateView();
}

void CViewWnd::UnhighlightAll()
{
  pModel->UnhighlightAll();
  pScene->UpdateStyle();
  foreach( IViewScenePtr pScene_selected, mapScenes.values() ) {
    pScene_selected->RemoveAllLabelItems();
    pScene_selected->RemoveAllShapeItems();
  }
  pScene->UpdateHighlightRect();
  emit UpdateViews();
}


void CViewWnd::UpdateShowFlag( ShowFlag showFlag, bool value )
{
  pScene->UpdateVisibility( showFlag, value );
  showFlags = pScene->ShowFlags();
  viewport()->update();
}

#define QARGS( _arg ) QStringList( QString( "%1" ).arg( _arg ) )
#define QARG( _arg ) << QString( "%1" ).arg( _arg )

void CViewWnd::SetExamineMsg( IComponentPtr _pObject, bool toLog )
{
  MessageType mt = toLog ? MT::STATUS : MT::DYNAMIC;

  if( _pObject == null ) {
    pConsole->Send( mt, AL::INFO, sender_name, "" );
    return;
  }

  switch( _pObject->Type() ) {
  case OT::NODE: {
    NodePtr pNode = dgtype_cast< NodePtr >( _pObject );
    ulong irregular_code = (ulong)pNode->IsIrregular();
    if( irregular_code == 0 )
      irregular_code = STR::REGULAR;
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMNODE,
          QARGS( pNode->X() )
          QARG( pNode->Y() )
          QARG( SM_MSG( irregular_code ) ) );
    break;
  }
  case OT::ELEMENT: {
    ElementPtr pElement = dgtype_cast< ElementPtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMELEM,
          QARGS( pElement->Id() )
          QARG( pElement->Node(1)->X() )
          QARG( pElement->Node(1)->Y() )
          QARG( pElement->Node(2)->X() )
          QARG( pElement->Node(2)->Y() )
          QARG( pElement->Length() ) );
    break;
  }
  case OT::SURFACEEX: {
    SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( _pObject );
    int sx_zone = pSX->Zone();
    SurfaceZonePtr pSZ = ( sx_zone != SZN_XY ) ?
      pModel->GetFluxModel()->GetTopology()->FindSurfaceZone( sx_zone ) : null;
    pConsole->Send( mt, AL::INFO, sender_name,
          (sx_zone == SZN_XY) ? DGMSG::EXAM_SURFACE_XY : DGMSG::EXAMSURFACE,
          QARGS( (pSZ != null) ? QString::fromStdString( pSZ->LongName() ) : "---" )
          QARG( pSX->Level() )
          QARG( pSX->Origin().x )
          QARG( pSX->Origin().y )
          QARG( QString::fromStdString( pSX->GetCreatorId().ToStdString() ) ) );
    pLastExaminedSX = pSX;
    break;
  }
  case OT::GRIDPOINTEX: {
    GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( _pObject );
    GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSeg( pGPX->Zone() );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMGRIDPOINT,
          QARGS( pGPX->Zone() )
          QARG( (pGPS != null) ? QString::fromStdString( pGPS->LongName() ) : "---" )
          QARG( pGPX->Value() )
          QARG( pModel->GetFluxModel()->GetGridPointExNumber( pGPX ) )
          QARG( QString::fromStdString( pGPX->GetCreatorId().ToStdString() ) ) );
    pLastExaminedGPX = pGPX;
    break;
  }
  case OT::SEPARATOR: {
    SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMSEPARATOR,
          QARGS( pSep->Id() )
          QARG( pModel->ObjectIndex( pSep ) + 1 ) );
    break;
  }
  case OT::SOURCE: {
    SourcePtr pSrc = dgtype_cast< SourcePtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMSOURCE,
          QARGS( pSrc->X() )
          QARG( pSrc->Y() ) );
    break;
  }
  case OT::CHORD: {
    ChordPtr pChord = dgtype_cast< ChordPtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAMCHORD,
          QARGS( pChord->Point_1().x )
          QARG( pChord->Point_1().y )
          QARG( pChord->Point_1().z )
          QARG( pChord->Point_2().x )
          QARG( pChord->Point_2().y )
          QARG( pChord->Point_2().z )
          QARG( pChord->Length() )
          QARG( pModel->ObjectIndex( pChord ) + 1 ) );
    break;
  }
  case OT::MESHCELL: {
    MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( _pObject );
    int irregular_code = pMC->IsIrregular();
    QString irregular_str = (irregular_code != 0) ?
                              SM_MSG( irregular_code ) : "---";
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAM_MESH_CELL,
          QARGS( pMC->eNumber() )
          QARG( pMC->Nx() )
          QARG( pMC->Ny() )
          QARG( pMC->Center().x )
          QARG( pMC->Center().y )
          QARG( pMC->Ratio() )
          QARG( irregular_str )
          QARG( pMC->BackupRatio() ) );
    break;
  }
  case OT::MESHELEMENT: {
    MeshElementPtr pME = dgtype_cast< MeshElementPtr >( _pObject );
    QString cut_str = (pME->CutFlag() != 0) ?
                        SM_MSG( STR::MESH_ELEMENT_CUT ) : "---";
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAM_MESH_ELEMENT,
          QARGS( pME->MPoint(0)->X() )
          QARG( pME->MPoint(0)->Y() )
          QARG( pME->MPoint(1)->X() )
          QARG( pME->MPoint(1)->Y() )
          QARG( cut_str ) );
    break;
  }
  case OT::MESHPOINT: {
    MeshPointPtr pMP = dgtype_cast< MeshPointPtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAM_MESH_POINT,
          QARGS( pMP->X() )
          QARG( pMP->Y() )
          QARG( pMP->Index().x )
          QARG( pMP->Index().y ) );
    break;
  }
  case OT::XPOINTTEST:
  {
    assert( pModel->GetFluxModel()->HasEquil() );
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( _pObject );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAM_XPOINTTEST,
          QARGS( pXPT->Level() )
          QARG( pXPT->Center().x )
          QARG( pXPT->Center().y )
          QARG( pXPT->ID() ) );
    break;
  }
  case OT::XPOINTSEG: {
    XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( _pObject );
    GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSegBySegment( pXPS );
    pConsole->Send( mt, AL::INFO, sender_name,
          DGMSG::EXAM_XPOINTSEG,
          QARGS( pGPS->Zone() )
          QARG( QString::fromStdString( pGPS->LongName() ) )
          QARG( pGPS->LineLength() )
          QARG( pGPS->Level() ) );
    break;
  }
  default:
    pConsole->Send( mt, AL::INFO, sender_name, "" );
    break;
  }
}


void CViewWnd::RemoveObjectItem( IComponentPtr _pObject )
{
  foreach( IViewScenePtr pScene, mapScenes ) {
    IViewItemPtr pItem = pScene->GetItem( _pObject );
    if( pItem == null )
      continue;
    pScene->RemoveItem( pItem );
    delete pItem;
  }
}

void CViewWnd::RemoveObjectListItems( const IComponentList& _pObjects )
{
  foreach( IViewScenePtr pScene, mapScenes )
    pScene->RemoveListItems( _pObjects );
}

void CViewWnd::CreateObjectItem( IComponentPtr _pObject )
{
  foreach( IViewScenePtr pScene, mapScenes ) {
    IViewItemPtr pItem = pScene->GetItem( _pObject );
    if( pItem != null )
      continue;
    pScene->CreateItem( _pObject );
  }
}

void CViewWnd::CreateObjectListItems( const IComponentList& _pObjects )
{
  foreach( IViewScenePtr pScene, mapScenes )
    pScene->CreateListItems( _pObjects );
}

void CViewWnd::UpdateObjectItem( IComponentPtr _pObject )
{
  foreach( IViewScenePtr pScene, mapScenes ) {
    IViewItemPtr pItem = pScene->GetItem( _pObject );
    if( pItem != null )
      continue;
    pItem->UpdateDependentGeometry();
    pItem->UpdateGeometry();
    pItem->UpdateStyle();
  }
}

void CViewWnd::UpdateObjectListItems( const IComponentList& _pObjects )
{
  foreach( IViewScenePtr pScene, mapScenes )
    pScene->FullUpdateListItems( _pObjects );
}

void CViewWnd::closeEvent( QCloseEvent* pe )
{
  emit ViewClosed( this );
  pe->accept();
}

void CViewWnd::focusInEvent( QFocusEvent* pe )
{
  emit ViewInFocus( this );
  pe->accept();
}

void CViewWnd::LoadSessionData( const SessionViewRecord& _crSVR )
{
  //move( _crSVR.window_position );
  //resize( _crSVR.window_size );

  SelectCurrentScene( (SceneType)_crSVR.scene_type );

  state.center = _C( _crSVR.scene_center );
  state.scale = _C( _crSVR.scene_scale );
  if( state.scale.x == 0. ) state.scale.x = 1.;
  if( state.scale.y == 0. ) state.scale.y = 1.;
  state.xyAngle = _crSVR.scene_angle;

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_STATE,
    QStringList( QString( "(%1, %2)" ).arg( state.center.x() ).arg( state.center.y() ) )
              << QString( "(%1, %2)" ).arg( state.scale.x ).arg( state.scale.y )
              << QString( "%1" ).arg( state.xyAngle * RAD2DEG ) );

  ApplyTransform();
  slotUpdateView();
}

SessionViewRecord CViewWnd::SaveSessionData() const
{
  quint16 tools[] = { toolId_left, toolId_middle, toolId_right };
  return SessionViewRecord( pos(), size(),
                            _C( state.center ), _C( state.scale ),
                            state.xyAngle, pScene->Type(), tools );
}
