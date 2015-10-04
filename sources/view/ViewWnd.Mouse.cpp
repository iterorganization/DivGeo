#include "../main/MainWnd.h"
#include "ViewWnd.h"

CViewWnd::ToolState CViewWnd::GetToolState()
{
  QVector< QPair< Qt::MouseButton, ToolId > > toolStates( 3 );
  toolStates[0] = qMakePair( Qt::LeftButton, toolId_left );
  toolStates[1] = qMakePair( Qt::MiddleButton, toolId_middle );
  toolStates[2] = qMakePair( Qt::RightButton, toolId_right );
  return toolStates;
}

void CViewWnd::SetToolState( Qt::MouseButton mb, ToolId toolid )
{
  switch( mb ) {
  case Qt::LeftButton: toolId_left = toolid; break;
  case Qt::MiddleButton: toolId_middle = toolid; break;
  case Qt::RightButton: toolId_right = toolid; break;
  default: break;
  }
}

void CViewWnd::mousePressEvent(QMouseEvent* me)
{
  if( currentButton != Qt::NoButton )
    return;

  currentButton = me->button();

  bDoubleClickProcceed = false;
  if( doubleClickTimer.isActive() ) {
    doubleClickTimer.stop();
    mouseDoubleClickEvent( me );
    delete pLastEvent;
  }
  else {
    doubleClickTimer.setSingleShot( true );
    doubleClickTimer.start( QApplication::doubleClickInterval()/2 );
    pLastEvent = new QMouseEvent( *me );
  }
}

void CViewWnd::mousePress()
{
  QPointF scenePos = mapToScene( pLastEvent->pos() );
  QString sMsgText = SM_LOG( LOG::TOOLS::TOOL );
  switch( pLastEvent->button() ) {
  case Qt::LeftButton:    sMsgText = sMsgText.arg( SM_LOG( toolId_left ) ); break;
  case Qt::RightButton:   sMsgText = sMsgText.arg( SM_LOG( toolId_right ) ); break;
  case Qt::MiddleButton:  sMsgText = sMsgText.arg( SM_LOG( toolId_middle ) ); break;
  default: break;
  }
  sMsgText += " ";

  emit UpdateObjectInfo( null );

  if( HasAnyFlag( QApplication::keyboardModifiers(), Qt::ShiftModifier ) ) {
    QString sEventText = SM_LOG( LOG::TOOLS::SHIFTPRESSED ).arg( scenePos.x() ).arg( scenePos.y() );
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText + sEventText );

    switch( pLastEvent->button() ) {
    case Qt::LeftButton:   emit signalMouseLeftButton( pLastEvent->pos(), SHIFTCLICK ); break;
    case Qt::RightButton:  emit signalMouseRightButton( pLastEvent->pos(), SHIFTCLICK ); break;
    case Qt::MiddleButton: emit signalMouseMiddleButton( pLastEvent->pos(), SHIFTCLICK ); break;
    default: break;
    }
    pConsole->SetLoggingMode( LM::COLLECT );
  }
  else {
    QString sEventText = SM_LOG( LOG::TOOLS::PRESSED ).arg( scenePos.x() ).arg( scenePos.y() );
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText + sEventText );

    switch( pLastEvent->button() ) {
    case Qt::LeftButton:   emit signalMouseLeftButton( pLastEvent->pos(), PRESSED ); break;
    case Qt::RightButton:  emit signalMouseRightButton( pLastEvent->pos(), PRESSED ); break;
    case Qt::MiddleButton: emit signalMouseMiddleButton( pLastEvent->pos(), PRESSED ); break;
    default: break;
    }
    pConsole->SetLoggingMode( LM::COLLECT );
  }

  if( bReleaseQueued && pLastEvent->button() == currentButton ) {
    pConsole->SetLoggingMode( LM::USUAL );
    QString sEventText = SM_LOG( LOG::TOOLS::RELEASED ).arg( scenePos.x() ).arg( scenePos.y() );
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText + sEventText );

    switch( pLastEvent->button() ) {
    case Qt::LeftButton:   emit signalMouseLeftButton( pLastEvent->pos(), RELEASED ); break;
    case Qt::RightButton:  emit signalMouseRightButton( pLastEvent->pos(), RELEASED ); break;
    case Qt::MiddleButton: emit signalMouseMiddleButton( pLastEvent->pos(), RELEASED ); break;
    default: break;
    }
    currentButton = Qt::NoButton;
    bReleaseQueued = false;
    return;
  }
  else
    pressPosition = pLastEvent->pos();

  delete pLastEvent;
}

void CViewWnd::mouseReleaseEvent( QMouseEvent* me )
{
  if( me->button() != currentButton )
    return;

  if( doubleClickTimer.isActive() ) {
    bReleaseQueued = true;
    return;
  }

  if( bDoubleClickProcceed ) {
    bDoubleClickProcceed = false;
    return;
  }

  pConsole->SetLoggingMode( LM::USUAL );
  pConsole->OutputCollection();

  QPointF scenePos = mapToScene( me->pos() );
  QString sMsgText = SM_LOG( LOG::TOOLS::TOOL ) + " " +
                     SM_LOG( LOG::TOOLS::RELEASED ).arg( scenePos.x() ).arg( scenePos.y() );

  switch( me->button() ) {
  case Qt::LeftButton:
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_left ) ) );
    emit signalMouseLeftButton( me->pos(), RELEASED );
    break;
  case Qt::RightButton:
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_right ) ) );
    emit signalMouseRightButton( me->pos(), RELEASED );
    break;
  case Qt::MiddleButton:
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_middle ) ) );
    emit signalMouseMiddleButton( me->pos(), RELEASED );
    break;
  default: break;
  }  
  currentButton = Qt::NoButton;

  return;
}

void CViewWnd::mouseMoveEvent( QMouseEvent* me )
{
  if( currentButton == Qt::NoButton ) {
    EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();
    Point pnt = _C( mapToScene( me->pos() ) );
    double level;
    QString sLevel = "?";
    if( pEquil != null && pEquil->GetLevel( _C( pnt ), &level, null ) == 0 )
      sLevel = QString( "%1" ).arg( level );
    NPoint cell;
    QString sCell = "?";
    if( pEquil != null && pEquil->Cell( pnt, &cell ) == 0 )
      sCell = QString::fromStdString( cell.ToString( NPoint::INDEX ) );
    pConsole->Send( MT::DYNAMIC, AL::INFO, "", QString( "%1\tcell:%2\tlevel:%3" )
                    .arg( ToQString( pnt ), -20 )
                    .arg( sCell, -14 )
                    .arg( sLevel, -10 ) );
    return;
  }


  if( doubleClickTimer.isActive() ) {
    doubleClickTimer.stop();
    mousePress();
  }

  pConsole->ClearCollection();

  QPointF scenePos = mapToScene( me->pos() );
  QString sMsgText = SM_LOG( LOG::TOOLS::TOOL ) + " " +
                     SM_LOG( LOG::TOOLS::MOVED ).arg( scenePos.x() ).arg( scenePos.y() );

  const int moveMinDelta = 10;
  bool noMove = qAbs( me->pos().x() - pressPosition.x() ) < moveMinDelta ||
                qAbs( me->pos().y() - pressPosition.y() ) < moveMinDelta;
  if( currentButton == Qt::LeftButton &&
      !( noMove && toolId_left == TOOLS::ZOOMPAN ) ) {
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_left ) ) );
    emit signalMouseLeftButton( me->pos(), MOVED );
  }
  else if( currentButton == Qt::MiddleButton &&
           !( noMove && toolId_middle == TOOLS::ZOOMPAN ) ) {
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_middle ) ) );
    emit signalMouseMiddleButton( me->pos(), MOVED );
  }
  else if( currentButton == Qt::RightButton &&
           !( noMove && toolId_right == TOOLS::ZOOMPAN ) ) {
    pConsole->Send( LOG_WIP_INFO, sender_name, sMsgText.arg( SM_LOG( toolId_right ) ) );
    emit signalMouseRightButton( me->pos(), MOVED );
  }
}

void CViewWnd::mouseDoubleClickEvent( QMouseEvent* me )
{
  if( me->button() != currentButton )
    return;

  if( doubleClickTimer.isActive() ) {
    doubleClickTimer.stop();
    delete pLastEvent;
  }

  if( bReleaseQueued )
    bReleaseQueued = false;

  bDoubleClickProcceed = true;

  switch( currentButton ) {
  case Qt::LeftButton:   emit signalMouseLeftButton( me->pos(), DOUBLECLICK ); break;
  case Qt::RightButton:  emit signalMouseRightButton( me->pos(), DOUBLECLICK ); break;
  case Qt::MiddleButton: emit signalMouseMiddleButton( me->pos(), DOUBLECLICK ); break;
  default: break;
  }  

  currentButton = Qt::NoButton;
}


ulong CViewWnd::PrepareHitFlags( ulong hitFlags )
{
  if( HasEachFlag( hitFlags, SHW::NODES ) )
    hitFlags |= SHW::IRRNODES;

  ulong mask = showFlags;
  if( HasEachFlag( mask, SHW::MESH ) )
    mask |= SHWX::MESHCELLS | SHWX::MESHELEMENTS | SHWX::MESHPOINTS;

  if( !pState->editTopology )
    mask &= ~(SHW::XPOINTTESTS);

  return hitFlags & mask;
}


void CViewWnd::wheelEvent( QWheelEvent *pe )
{
  // MB: wheel disabled
  UNUSED( pe );
  slotUpdateView();
}
