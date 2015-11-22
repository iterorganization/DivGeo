#include "../main/MainWnd.h"
#include "ViewWnd.h"

void CViewWnd::slotUpdateView()
{
  state.viewPoly = mapToScene( rect() );
  state.viewRect = rect();
  if( pScene != null )
    pScene->UpdateGrid();
  viewport()->update();
  emit UpdateViewState( this );
}

QPoint CViewWnd::NearestScenePosition( const QPoint& view_point )
{
  const double safeOffset = 0.5;
  QPointF pos = mapToScene( view_point );
  QRectF sr = sceneRect();
  if( pos.y() < sr.top() )
    pos.setY( sr.top() + safeOffset );
  else if( pos.y() > sr.bottom() )
    pos.setY( sr.bottom() - safeOffset );
  if( pos.x() < sr.left() )
    pos.setX( sr.left() + safeOffset );
  else if( pos.x() > sr.right() )
    pos.setX( sr.right() - safeOffset );

  return mapFromScene( pos );
}

void CViewWnd::SetCenter( const QPointF& _crCenter )
{
  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_CENTER_CHANGED,
    QStringList( QString( "(%1, %2)" ).arg( state.center.x() ).arg( state.center.y() ) ) );
  state.center = _crCenter;
}

void CViewWnd::RotateCW( double angle_rad )
{
  SetAngle( state.xyAngle + angle_rad );
}

void CViewWnd::RotateCCW( double angle_rad )
{
  SetAngle( state.xyAngle - angle_rad );
}

void CViewWnd::SetAngle( double angle_rad, bool saveState )
{
  if( saveState )
    SaveState();
  state.SetAngle( angle_rad );

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_ANGLE_CHANGED,
                  QStringList( QString( "%1 deg." ).arg( state.xyAngle * RAD2DEG ) ) );

  ApplyTransform();
  slotUpdateView();
}

void CViewWnd::SetStretchMode( bool enable )
{
  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_STRETCH_MODE,
                  QStringList( SM_MSG( enable ? DG3::MODE_ON : DG3::MODE_OFF ) ) );
  state.inStretchMode = enable;
  if( HasAnyFlag( showFlags, SHW::STRETCH ) != enable )
    showFlags ^= SHW::STRETCH;
  if( !enable ) {
    ResetAspectRatio( true );
    pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_SCALE_CHANGED,
      QStringList( QString( "(%1, %2)" ).arg( state.scale.x ).arg( state.scale.y ) ) );
  }
}

void CViewWnd::SetColoredEquilMode( bool _enable )
{
  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_COLORED_EQUIL,
                  QStringList( SM_MSG( _enable ? DG3::ENABLE: DG3::DISABLE ) ) );
  state.coloredEquil = _enable;
  CurrentScene()->SetVisibility( showFlags, true );
  slotUpdateView(); // Calls pScene->UpdateGrid();
}

void CViewWnd::SetRadialGridMode( bool _enable )
{
  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_RADIAL_GRID,
                  QStringList( SM_MSG( _enable ? DG3::ENABLE: DG3::DISABLE ) ) );
  state.radialGrid = _enable;
  slotUpdateView(); // Calls pScene->UpdateGrid();
}

void CViewWnd::Scale( const Point& _crK, bool saveState )
{
  if( _crK.x == 0. || _crK.y == 0. )
    return;
  SetScale( state.scale*_crK, saveState );
}

void CViewWnd::SetScale( const Point& _crScale, bool saveState )
{
  if( saveState )
    SaveState();
  Point scale = _crScale;
  if( scale.x == 0. )
    scale.x = 1.;
  if( scale.y == 0. )
    scale.y = 1.;
  state.SetScale( scale );

  if( !state.inStretchMode )
    ResetAspectRatio();

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_SCALE_CHANGED,
    QStringList( QString( "(%1, %2)" ).arg( state.scale.x ).arg( state.scale.y ) ) );

  //state.center = mapToScene( rect().center() );
  ApplyTransform();
  slotUpdateView();
}

void CViewWnd::ResetAspectRatio( bool updateView )
{
  if( updateView )
    SaveState();
  state.scale = dg_min( state.scale.x, state.scale.y );
  if( updateView ) {
    ApplyTransform();
    slotUpdateView();
  }
}

void CViewWnd::ApplyTransform()
{
  QMatrix mx;
  mx.rotate( -state.xyAngle * RAD2DEG );
  mx.scale( state.scale.x, -state.scale.y );
  setMatrix( mx );
  centerOn( state.center );
}

void CViewWnd::SaveState()
{
  pScene->AddViewState(
        ViewStateRecord( state.center, state.scale, state.xyAngle ) );
  emit UndoStackEnabled( true );
}

void CViewWnd::FitToSceneRect( QRectF _rect )
{
  double margin = 50;
  SetCenter( _rect.center() );
  state.xyAngle = 0.;
  if( !_rect.isNull() ) {
    _rect.moveTo( _rect.topLeft() + QPointF( margin, margin ) );
    _rect.setSize( _rect.size() + QSizeF( margin, margin ) );
  }
  ApplyTransform();
  QRectF vr( mapToScene( 0, 0 ), mapToScene( width()-1, height()-1 ) );
  if( vr.width() == 0 || vr.height() == 0 )
    return;
  Point relScale( fabs( _rect.width()/vr.width() ), fabs( _rect.height()/vr.height() ) );
  if( state.inStretchMode ) {
    double maxRelScale = dg_max( relScale.x, relScale.y );
    if( maxRelScale != 0. )
      SetScale( state.scale / maxRelScale, false ); //1409 false
  }
  else {
    if( relScale.x != 0. && relScale.y != 0. )
      SetScale( state.scale / relScale, false ); //1409 false
  }
}

void CViewWnd::FitToModel()
{
  SaveState();
  SetupViewState();
  FitToSceneRect( pScene->FullBoundingRect() );
  slotUpdateView();
}

void CViewWnd::FitToSelected()
{
  SENDER_NAME_Q( "FitToSelected" );
  SaveState();
  SetupViewState();
  if( !pModel->HasMarked() ) {
    pConsole->Send( STATUS_WARN, SENDER, ERR::NOSELECTION );
    return;
  }
  QRectF rect = pScene->GroupBoundingRect( pModel->MarkedObjects() );
  FitToSceneRect( rect );
  slotUpdateView();
}


void CViewWnd::resizeEvent( QResizeEvent* pe)
{
  QGraphicsView::resizeEvent( pe );
  slotUpdateView();
}


void CViewWnd::SetPreviousViewState()
{
  if( !pScene->HasUndoStateRecords() )
    return;
  const ViewStateRecord& prevState = pScene->UndoViewChange();
  state.xyAngle = prevState.angle;
  state.scale = prevState.scale;
  state.center = prevState.center;

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_STATE,
    QStringList( QString( "(%1, %2)" ).arg( state.center.x() ).arg( state.center.y() ) )
              << QString( "(%1, %2)" ).arg( state.scale.x ).arg( state.scale.y )
              << QString( "%1" ).arg( state.xyAngle * RAD2DEG ) );
  ApplyTransform();
  slotUpdateView();
  if( !pScene->HasUndoStateRecords() )
    emit UndoStackEnabled( false );
}

void CViewWnd::SetNextViewState()
{
  if( !pScene->HasRedoStateRecords() )
    return;
  const ViewStateRecord& nextState = pScene->RedoViewChange();
  state.xyAngle = nextState.angle;
  state.scale = nextState.scale;
  state.center = nextState.center;

  pConsole->Send( LOG_INFO, sender_name, DG3::VIEW_STATE,
    QStringList( QString( "(%1, %2)" ).arg( state.center.x() ).arg( state.center.y() ) )
              << QString( "(%1, %2)" ).arg( state.scale.x ).arg( state.scale.y )
              << QString( "%1" ).arg( state.xyAngle * RAD2DEG ) );
  ApplyTransform();
  slotUpdateView();
}

void CViewWnd::SetupViewState()
{
  pScene->SetViewState( pState );
  uint currentFlag = 1;
  for( uint c = 0; c < ShowFlags::size; c++ ) {
    bool enable = HasAnyFlag( showFlags, currentFlag );
    pScene->UpdateVisibility( (ShowFlag)currentFlag, enable );
    currentFlag *= 2;
  }
}

void CViewWnd::paintEvent( QPaintEvent* pe )
{
  SetupViewState();
  QGraphicsView::paintEvent( pe );
}
