#include "../main/MainWnd.h"
#include "ViewWnd.h"

void CViewWnd::slotMark( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotMark" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;

    ToolMarkPtr pToolMark = new ToolMark( _C( position ), true );
    pTool = pToolMark;

    IViewItemPtr pItem = pScene->HitViewItem( mapToScene( position ), SHW::ELEMS );
    if( pItem == null )
      break;

    ElementPtr pElem = dgtype_cast< ElementPtr >( pItem->ModelObject() );
    IComponentList elements;
    pElem->GetChain( null, null, &elements );
    pToolMark->items = pScene->GetItemsList( elements );

    showFlags |= SHW::ELEMS;
    pModel->MarkGroup( elements, !pElem->IsMarked() );
    pModel->Agent()->Update();
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolMarkPtr pToolMark = new ToolMark( _C( position ) );
    pTool = pToolMark;

    pTool->pItem = pScene->HitViewItem( mapToScene( position ), SHW::MARKED|SHW::CHORDS3D );
    if( pTool->pItem == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOELEMS );
      delete pTool;
      pTool = null;
      break;
    }

    IComponentPtr pObject = pTool->pItem->ModelObject();

    switch( pObject->Type() ) {
    case OT::SEPARATOR:
    case OT::SOURCE:
    case OT::CHORD:
    case OT::MESHELEMENT:
    case OT::MESHCELL:
      pObject->Mark( !pObject->IsMarked() ); // Invert mark
      break;
    case OT::ELEMENT:
      pToolMark->pFirstItem = pTool->pItem;
      pToolMark->include = !pObject->IsMarked();
      pObject->Mark( pToolMark->include ); // Invert mark
      break;
    default:
      assert( false );
    }
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolMarkPtr pToolMark = static_cast< ToolMarkPtr >( pTool );

    if( pTool->isShifted ) { // Mark objects in selection
      if( pScene->IsTopView() )
        break;

      /* Prepare first object and hit flags */
      if( !pTool->isMoved ) {
        pTool->SetMoved();
        //TODO: pModel->UndoStack().CancelNonDestructive(); // Unmark first object

        pToolMark->flags = pModel->GetMarkedTypes();
        if( pToolMark->flags == 0 )
          pToolMark->flags= SHW::MARKED;
      }

      /* Calc highlight rect */
      Point pos = _C( NearestScenePosition( position ) );
      Point dif = pos - pToolMark->position;
      Point leftTopPoint = Min( pToolMark->position, pToolMark->position + dif );
      Point sizePoint = fabs( dif );
      QRect hiRect( leftTopPoint.x, leftTopPoint.y, sizePoint.x, sizePoint.y );
      pScene->UpdateHighlightRect( mapToScene( hiRect ) );

      /* Unmark objects out of highlight rect */
      ViewItemsList items_tmp = pToolMark->items;
      foreach( IViewItemPtr pItem, items_tmp ) {
        if( !pItem->InRectangle( hiRect, viewportTransform() ) ) {
          pToolMark->items.removeOne( pItem );
          pItem->ModelObject()->Mark( false );
        }
      }

      /* Select items in highlight rect */
      ViewItemsList items_selected = pScene->ItemsInRectangle( pToolMark->flags, hiRect, viewportTransform() );

      /* Mark new and unmarked items */
      foreach( IViewItemPtr pItem, items_selected ) {
        if( pToolMark->items.contains( pItem ) )
          continue;
        if( pItem->ModelObject()->IsMarked() )
          continue;
        pItem->ModelObject()->Mark();
        pToolMark->items.append( pItem );
      }
      items_selected.clear();
    }
    else if( pToolMark->pFirstItem != null ) {
      /* Mark linked elements */
      IViewItemPtr pItem = pScene->HitViewItem( mapToScene( position ), SHW::ELEMS );
      if( pItem == null || pItem == pToolMark->pFirstItem )
        break;
      ElementPtr pElem = dgtype_cast< ElementPtr >( pItem->ModelObject() );
      ElementPtr pFirstElem = dgtype_cast< ElementPtr >( pToolMark->pFirstItem->ModelObject() );
      if( pElem->IsConnectedWith( pFirstElem ) == 0 )
        break;
      pToolMark->pFirstItem = pItem;
      pToolMark->pFirstItem->ModelObject()->Mark( pToolMark->include );
    }

    pModel->Agent()->Update( false ); //1409 false
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;

    pScene->UpdateHighlightRect();
    pModel->ActionStack().Complete( "Tool:Mark" );
    if( pTool->pItem != null ) //1408
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  default:
    break;
  }
}

void CViewWnd::slotExamine( QPoint position, EEventID eventid )
{
  UNUSED(position);
  SENDER_NAME_Q( "slotExamine" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    pTool = new ToolBase();
    pTool->pItem = pScene->HitViewItem( mapToScene( position ), SHW::EXAMINABLE & showFlags );
    if( pTool->pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }

    pTool->pItem->ModelObject()->Highlight( true );
    SetExamineMsg( pTool->pItem->ModelObject() );
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->isShifted )
      break;
    IViewItemPtr pItem = null;
    if( pTool->pItem != null )
      pItem = pScene->HitViewItem( mapToScene( position ), SHW::EXAMINABLE & showFlags );
    if( pItem == null || pItem == pTool->pItem )
      break;
    pTool->pItem->ModelObject()->Highlight( false );
    pTool->pItem = pItem;

    //UnhighlightAll();

    if( pItem->ModelObject()->Type() == OT::MESHPOINT ) {
      MeshPointPtr pMP = dgtype_cast< MeshPointPtr >( pItem->ModelObject() );
      for( int i = MCN_START; i < MCN_END; i++ )
        if( pMP->Element(i) != null )
          pMP->Element(i)->Highlight();
    }
    else if( pItem->ModelObject()->Type() == OT::MESHCELL ) {
      MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( pItem->ModelObject() );
      for( int i = MCN_START; i < MCN_END; i++ )
        pMC->Element(i)->Highlight();
    }

    pItem->ModelObject()->Highlight();
    SetExamineMsg( pItem->ModelObject() );
    pModel->Agent()->Update( false ); //1409 false
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    if( pTool->pItem != null ) {
      if( !pTool->isShifted )
        emit ObjectSelected( pTool->pItem->ModelObject() );
      pTool->pItem->ModelObject()->Highlight( false );      
    }

    pModel->ActionStack().Complete( "Tool:Examine" );

    if( pTool->pItem != null )
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}


void CViewWnd::slotMove( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotMove" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;

    ToolMovePtr pToolMove = new ToolMove( _C( position ), true );
    pTool = pToolMove;
    pToolMove->pItem = pScene->HitViewItem( mapToScene( position ), SHW::MOVEABLE & showFlags );
    if( pToolMove->pItem == null || pToolMove->pItem->ModelObject() == null ) {
      delete pTool;
      pTool = null;
      break;
    }

    if( pToolMove->pItem->ModelObject()->Type() == OT::CHORD ) {
      ChordPtr pChord = dgtype_cast< ChordPtr >( pToolMove->pItem->ModelObject() );
      pChord->DistToPoint( _C( mapToScene( position ) ), &pToolMove->posFlag );
      pToolMove->len = (abs( pToolMove->posFlag ) == 1) ?
                         hypot( pChord->Point_1().x, pChord->Point_1().z ) :
                         hypot( pChord->Point_2().x, pChord->Point_2().z );
      if( pToolMove->len > 1.e15 )
        pConsole->Send( LOG_INFO, FULL_SENDER, "DEBUG: really dig d->r" );
      pToolMove->isPossible = true;
    }
    else
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::BADSTRETCHTYPE );

    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolMovePtr pToolMove = new ToolMove( _C( position ) );
    pTool = pToolMove;
    pToolMove->pItem = pScene->HitViewItem( mapToScene( position ), SHW::MOVEABLE & showFlags );
    if( pToolMove->pItem == null || pToolMove->pItem->ModelObject() == null ) {
      delete pTool;
      pTool = null;
      break;
    }
    pToolMove->pItem->ModelObject()->Highlight();

    IComponentPtr pObject = pToolMove->pItem->ModelObject();

    pToolMove->isPossible = !pObject->IsLocked();

    /* Prohibit most movements of surfaces/grid points in Carre mode */
    if( pModel->CarreOutputMode() && pModel->GetFluxModel()->HasEquil() ) {
      if( pObject->Type() == OT::GRIDPOINTEX ) {
        pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::CARRE_INHIBITS );
        pToolMove->isPossible = false;
      }

      if( pObject->Type() == OT::SURFACEEX ) {
        SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
        if( !pSX->IsVirtual() ) {
          SurfaceZonePtr pSZ = pModel->GetFluxModel()->GetTopology()->FindSurfaceZone( pSX->Zone() );
          if( pSZ != null && pSZ->HasFlag( SZF::LIMITBYSURFACE ) )
            pModel->GetFluxModel()->RemoveVirtualSurfaces( pSX );
          else {
            pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::CARRE_INHIBITS );
            pToolMove->isPossible = false;
          }
        }
      }
    }

    if( pObject->Type() == OT::NODE ) {
      NodePtr pNode = dgtype_cast< NodePtr >( pObject );
      pNode->HighlightDrag();
      pToolMove->group = pScene->GetItemsList( pNode->ConnectedElements() );
      pToolMove->group.append( pScene->GetItemsList( pNode->ConnectedSeparators() ) );
    }
    else if( pObject->Type() == OT::CHORD ) {
      ChordPtr pChord = dgtype_cast< ChordPtr >( pToolMove->pItem->ModelObject() );
      pChord->DistToPoint( _C( mapToScene( position ) ), &pToolMove->posFlag, pScene->IsTopView(), showFlags );
    }

    if( pToolMove->isPossible )
      SetExamineMsg( pObject );

    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;

    ToolMovePtr pToolMove = static_cast< ToolMovePtr >( pTool );
    IComponentPtr pObject = pToolMove->pItem->ModelObject();
    Point pos_t = _C( mapToScene( position ) );

    if( !pTool->isShifted ) {
      if( !pToolMove->isPossible )
        break;
      int result = 0;
      switch( pObject->Type() ) {
      case OT::NODE: {
        NodePtr pNode = dgtype_cast< NodePtr >( pObject );
        pNode->Change( pos_t );
        break;
      }
      case OT::SURFACEEX: {
        SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
        if( pSX->IsVirtual() )
          result = pSX->Change( pos_t );
        else {
          result = pSX->Change( pos_t, null, true );
          pToolMove->isPossible = (pSX->CoordsOk() && pSX->Zone() != SZN_XY);
          if( !pToolMove->isPossible )
            pConsole->Send( STATUS_WARN, FULL_SENDER, result );
        }
        break;
      }
      case OT::GRIDPOINTEX: {
        if( !pModel->GetFluxModel()->HasEquil() )
          break;

        int zone = 0;
        double value = 0.;
        if( pModel->GetFluxModel()->GetTopology()->HitGridPointPosition( pos_t, &zone, &value ) != 0 )
          break;

        GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
        if( zone != pGPX->Zone() )
          break;

        pGPX->Change( zone, value );
        break;
      }
      case OT::SOURCE: {
        SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
        result = pSrc->Change( pos_t );
        if( result != 0 )
          pConsole->Send( STATUS_WARN, FULL_SENDER, result );
        break;
      }
      case OT::CHORD: {
        ChordPtr pChord = dgtype_cast< ChordPtr >( pObject );
        Point p1, p2;
        if( pScene->IsTopView() ) {
          if( abs( pToolMove->posFlag ) == 1 ) {
            p1 = pos_t.SwapYZ(); p1.y = pChord->Point_1().y;
            p2 = pChord->Point_2();
          }
          else {
            p1 = pChord->Point_1();
            p2 = pos_t.SwapYZ(); p2.y = pChord->Point_2().y;
          }
        }
        else {
          if( abs( pToolMove->posFlag ) == 1 ) {
            p1 = pos_t; p1.z = pChord->Point_1().z;
            p2 = pChord->Point_2();
          }
          else {
            p1 = pChord->Point_1();
            p2 = pos_t; p2.z = pChord->Point_2().z;
          }
        }

        result = pChord->Change( p1, p2 );

        if( result != 0 )
          pConsole->Send( STATUS_WARN, FULL_SENDER, result );
        break;
      }
      default:
        pConsole->Send( FATAL_ERROR, FULL_SENDER, DG3::UNKNOWN_OBJECT_TYPE );
      }
      if( result == 0 )
        SetExamineMsg( pObject );
    } // end not shifted
    else {
      if( !pToolMove->isPossible )
        break;

      if( pObject->Type() == OT::CHORD ) {
        ChordPtr pChord = dgtype_cast< ChordPtr >( pObject );
        Point p1, p2;
        if( !pScene->IsTopView() ) {
          if( abs( pToolMove->posFlag ) == 1 ) {
            p1 = pChord->Point_1(); p1.y = pos_t.y;
            p2 = pChord->Point_2();
          }
          else {
            p1 = pChord->Point_1();
            p2 = pChord->Point_2(); p2.y = pos_t.y;
          }
        }
        else {
          double factor = pToolMove->len / hypot( pos_t.x, pos_t.y );
          if( abs( pToolMove->posFlag ) == 1 ) {
            p1 =  pos_t.SwapYZ() * factor; p1.y = pChord->Point_1().y;
            p2 = pChord->Point_2();
          }
          else {
            p1 = pChord->Point_1();
            p2 = pos_t.SwapXY() * factor; p2.y = pChord->Point_2().y;
          }
          SetExamineMsg( pChord );
        }

        pChord->Change( p1, p2 );
        SetExamineMsg( pChord );
      }
    }

    /*pToolMove->pItem->UpdateGeometry();
    pToolMove->pItem->UpdateDependentGeometry();
    pToolMove->pItem->UpdateStyle();
    foreach( IViewItemPtr pItem, pToolMove->group ) {
      pItem->UpdateGeometry();
      pItem->UpdateDependentGeometry();
      pItem->UpdateStyle();
    }*/

    pModel->Agent()->Update( false ); //1409 false
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;

    ToolMovePtr pToolMove = static_cast< ToolMovePtr >( pTool );

    UnhighlightAll();

    if( !pTool->isShifted ) {
      if( !pToolMove->isPossible ) {
        if( pToolMove->pItem->ModelObject()->IsLocked() )
          pModel->ShowLockReasonOf( pToolMove->pItem->ModelObject() );
        pModel->ActionStack().Cancel();
      }
      else {
        pModel->ActionStack().Complete( "Tool:Move" );
        pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
      }
    }
    else {
      if( !pToolMove->isPossible )
        pModel->ActionStack().Cancel();
      else {
        pModel->ActionStack().Complete( "Tool:Move" );
        pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
      }
    }

    emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}
