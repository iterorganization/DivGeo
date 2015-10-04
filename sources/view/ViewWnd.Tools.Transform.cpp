#include "../main/MainWnd.h"
#include "ViewWnd.h"

void CViewWnd::slotZoomPan( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotZoomPan" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  case PRESSED:
  {
    if( pTool != null )
      break;
    ToolTransformViewPtr pToolTransform = new ToolTransformView( _C( position ), eventid == SHIFTCLICK );
    pTool = pToolTransform;
    pToolTransform->position = _C( NearestScenePosition( position ) );
    break;
  }
  case MOVED:
  {
    if( pTool == null )
        break;
    ToolTransformViewPtr pToolTransform = static_cast< ToolTransformViewPtr >( pTool );
    Point pos = _C( NearestScenePosition( position ) );
    Point dif = pos - pToolTransform->position;

    if( dif == 0 )
      break;

    pTool->SetMoved();

    QRect hiRect;
    if( pTool->isShifted ) // Shifted view rect
      hiRect = QRect( rect().topLeft() + dif, rect().bottomRight() + dif );
    else { // Cursor selection
      Point leftTopPoint = Min( pToolTransform->position, pToolTransform->position + dif );
      Point sizePoint = fabs( dif );
      hiRect = QRect( leftTopPoint.x, leftTopPoint.y, sizePoint.x, sizePoint.y );
    }

    pScene->UpdateHighlightRect( mapToScene( hiRect ) );
    viewport()->update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
        break;
    ToolTransformViewPtr pToolTransform = static_cast< ToolTransformViewPtr >( pTool );

    if( pTool->isMoved && !pTool->isShifted ) { // Zoom in selected region
      Point pos = _C( NearestScenePosition( position ) );
      Point viewSize( rect().width(), rect().height() );
      Point hiRectSize = ( fabs( pos - pToolTransform->position ) );
      Point zoomFactorOld = state.scale;

      SetCenter( mapToScene( _C( (pos + pToolTransform->position) / 2. ) ) );
      SetScale( zoomFactorOld * viewSize / hiRectSize );
    }
    else if( !pTool->isMoved && pTool->isShifted ) { // Zoom out
      Point zoomFactorOld = state.scale;

      SetCenter( mapToScene( _C( position ) ) );
      SetScale( zoomFactorOld * Point( 0.5 ) );
    }
    else { // Shift view center with selection (shift+press+move) or without (click)
      SaveState();
      SetCenter( mapToScene( NearestScenePosition( position ) ) );
      ApplyTransform();
    }

    // Common
    pScene->UpdateHighlightRect();
    UnhighlightAll();
    delete pTool;
    pTool = null;
    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    slotUpdateView();
    break;
  }
  default:
    break;
  }
}


void CViewWnd::slotRotate( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotRotate" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;
    SetAngle( 0. );
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;
    ToolTransformViewPtr pToolTV = new ToolTransformView( _C( position ) );
    pTool = pToolTV;

    pToolTV->center = _C( rect().center() );
    pToolTV->position = _C( position );
    pToolTV->angle = state.xyAngle;

    SaveState();
    pScene->UpdateRotTool( true );
    viewport()->update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolTransformViewPtr pToolTV = static_cast< ToolTransformViewPtr >( pTool );

    Point pntHandle = _C( position );
    Point vrInit = pToolTV->position - pToolTV->center;
    Point vrNew = pntHandle - pToolTV->center;
    double offset_angle = AngleBetween( vrInit, vrNew );

    pScene->UpdateRotTool( true );
    SetAngle( pToolTV->angle - offset_angle, false );
    break;
  }
  case RELEASED:
  {
    pScene->UpdateRotTool( false );
    if( pTool != null ) {
      delete pTool;
      pTool = null;
    }

    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    viewport()->update();
    break;
  }
  }
}



void CViewWnd::slotStretch( QPoint position, EEventID eventid )
{
  UNUSED(position);
  SENDER_NAME_Q( "slotStretch" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  case PRESSED:
  {
    if( pTool != null )
      break;
    ToolStretchPtr pToolStretch = new ToolStretch(
                 state.scale, _C( position ), eventid == SHIFTCLICK );
    pTool = pToolStretch;
    QPoint rectCenter = rect().center();
    Point dist = fabs( pTool->position - rectCenter );
    if( dist == 0. ) {
      delete pTool;
      pTool = null;
      break;
    }
    pToolStretch->hr = QRect( rectCenter - dist, rectCenter + dist );
    SaveState();
    viewport()->update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;

    ToolStretchPtr pToolStretch  = static_cast< ToolStretchPtr >( pTool );
    if( !pTool->isMoved ) {
      Point dif = fabs( pTool->position - position );
      pToolStretch->horizontal = (dif.x >= dif.y);
      pToolStretch->vertical = (dif.x <= dif.y);
      pTool->isMoved = true;
    }

    QPoint rectCenter = rect().center();
    Point dist_first = pTool->position - rectCenter;
    Point dist_current = fabs( _C( position - rectCenter ) );
    Point relScale = fabs( dist_current / dist_first );
    QRect hiRect( rectCenter - dist_current, rectCenter + dist_current );

    if( !pTool->isShifted ) {
      if( !pToolStretch->horizontal ) {
        relScale.x = 1.;
        hiRect.setLeft( pToolStretch->hr.left() );
        hiRect.setRight( pToolStretch->hr.right() );
      }

      if( !pToolStretch->vertical ) {
        relScale.y = 1.;
        hiRect.setBottom( pToolStretch->hr.bottom() );
        hiRect.setTop( pToolStretch->hr.top() );
      }
    }

    pScene->UpdateHighlightRect( mapToScene( hiRect ) );
    SetScale( pToolStretch->oldScale * relScale, false );
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    delete pTool;
    pTool = null;
    pScene->UpdateHighlightRect();
    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    viewport()->update();
    break;
  }
  }
}
