#include "../main/MainWnd.h"
#include "ViewWnd.h"

void CViewWnd::slotSplitElement( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotSplitElement" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolSplitPtr pToolSplit = new ToolSplit();
    pTool = pToolSplit;

    pToolSplit->pElementItemO = pScene->HitItemByType( SHW::ELEMS, mapToScene( position ) );
    if( pToolSplit->pElementItemO == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOELEMS );
      delete pTool;
      pTool = null;
      break;
    }
    IComponentPtr pElementObj = pToolSplit->pElementItemO->ModelObject();

    emit ChangeShowFlag( SHW::ELEMS, true );
    if( pElementObj->IsLocked() ) {
      pElementObj->Highlight();
      pModel->Agent()->Update();
      pModel->ShowLockReasonOf( pElementObj );
    }
    else {
      ElementPtr pElementO = dgtype_cast< ElementPtr >( pElementObj );
      NodePtr pNode = pModel->Struct()->AddNode( _C( mapToScene( position ) ) );
      ElementPtr pElemA = pModel->Struct()->AddElem( pElementO->Node(1), pNode );
      ElementPtr pElemB = pModel->Struct()->AddElem( pNode, pElementO->Node(2) );

      pElementO->Delete();
      pToolSplit->pElementItemO = null;
      pNode->HighlightDrag();

      pModel->Agent()->Update();

      pToolSplit->pNodeItem = pScene->GetItem( pNode );
      pToolSplit->pElementItemA = pScene->GetItem( pElemA );
      pToolSplit->pElementItemB = pScene->GetItem( pElemB );
    }
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolSplitPtr pToolSplit = static_cast< ToolSplitPtr >( pTool );
    if( pToolSplit->pNodeItem == null )
      break;
    NodePtr pNode = dgtype_cast< NodePtr >( pToolSplit->pNodeItem->ModelObject() );
    pNode->Change( _C( mapToScene( position ) ) );
    SetExamineMsg( pNode );
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolSplitPtr pToolSplit = static_cast< ToolSplitPtr >( pTool );
    IComponentPtr pObjInfo = null;
    if( pToolSplit->pElementItemO != null ) {
      pToolSplit->pElementItemO->ModelObject()->Highlight( false );
      pObjInfo = pToolSplit->pElementItemO->ModelObject();
    }
    if( pToolSplit->pElementItemA != null )
      pToolSplit->pElementItemA->ModelObject()->Highlight( false );
    if( pToolSplit->pElementItemB != null )
      pToolSplit->pElementItemB->ModelObject()->Highlight( false );

    if( pToolSplit->pNodeItem != null ) {
      pToolSplit->pNodeItem->ModelObject()->Highlight( false );
      pObjInfo = pToolSplit->pNodeItem->ModelObject();
    }

    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    pModel->ActionStack().Complete( "Tool:SplitElement" );
    emit UpdateObjectInfo( pObjInfo );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotJoinElements( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotJoinElements" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolJoinPtr pToolJoin = new ToolJoin();
    pTool = pToolJoin;
    pToolJoin->pNodeItem = pScene->HitItemByType( SHW::NODES, mapToScene( position ) );
    if( pToolJoin->pNodeItem == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NONODES );
      delete pTool;
      pTool = null;
    }
    emit ChangeShowFlag( SHW::ELEMS, true );
    NodePtr pNode = dgtype_cast< NodePtr >( pToolJoin->pNodeItem->ModelObject() );
    pToolJoin->isPossible = (pNode->CheckJoinPossibility() != 0 );
    pNode->HighlightDrag();
    pToolJoin->items = pScene->GetItemsList( pNode->ConnectedElements() );
    pToolJoin->items.append( pScene->GetItemsList( pNode->ConnectedSeparators() ) );
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolJoinPtr pToolJoin = static_cast< ToolJoinPtr >( pTool );
    IViewItemPtr pNodeItem = pScene->HitItemByType( SHW::NODES, mapToScene( position ) );
    if( pNodeItem == null || pNodeItem == pToolJoin->pNodeItem )
      break;
    NodePtr pNode = dgtype_cast< NodePtr >( pToolJoin->pNodeItem->ModelObject() );
    pNode->HighlightDrag( false );

    pToolJoin->pNodeItem = pNodeItem;
    pNode = dgtype_cast< NodePtr >( pToolJoin->pNodeItem->ModelObject() );
    pNode->HighlightDrag( true );
    pToolJoin->items = pScene->GetItemsList( pNode->ConnectedElements() );
    pToolJoin->items.append( pScene->GetItemsList( pNode->ConnectedSeparators() ) );

    pToolJoin->isPossible = (pNode->CheckJoinPossibility() != 0 );
    SetExamineMsg( pNode );
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolJoinPtr pToolJoin = static_cast< ToolJoinPtr >( pTool );
    if( pToolJoin->pNodeItem != null ) {
      NodePtr pNode = dgtype_cast< NodePtr >( pToolJoin->pNodeItem->ModelObject() );
      pNode->HighlightDrag( false );

      ElementPtr pNewElement = pNode->JoinElements();
      if( pNewElement != null ) {
        pToolJoin->items.clear();
        pModel->ActionStack().Complete( "Tool:JoinElements" );
        emit UpdateObjectInfo( pNewElement );
      }
    }

    pModel->ActionStack().Complete( "Tool:JoinElements" );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotConnectPoints( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotConnectPoints" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolConnectPtr pToolConnect = new ToolConnect();
    pTool = pToolConnect;
    pToolConnect->pNodeItemA = pScene->HitItemByType( SHW::NODES, mapToScene( position ) );
    if( pToolConnect->pNodeItemA == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NONODES );
      delete pTool;
      pTool = null;
    }
    pToolConnect->pNodeItemA->ModelObject()->Highlight();
    emit ChangeShowFlag( SHW::ELEMS, true );
    SetExamineMsg( pToolConnect->pNodeItemA->ModelObject() );
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolConnectPtr pToolConnect = static_cast< ToolConnectPtr >( pTool );
    IViewItemPtr pNodeItem = pScene->HitItemByType( SHW::NODES, mapToScene( position ) );
    if( pNodeItem == null || pNodeItem == pToolConnect->pNodeItemA || pNodeItem == pToolConnect->pNodeItemB )
      break;
    NodePtr pNodeA = dgtype_cast< NodePtr >( pToolConnect->pNodeItemA->ModelObject() );
    NodePtr pNodeB = dgtype_cast< NodePtr >( pNodeItem->ModelObject() );
    if( pNodeA->IsConnectedWith( pNodeB ) )
      break;
    if( pToolConnect->pNodeItemB != null )
      pToolConnect->pNodeItemB->ModelObject()->Highlight( false );
    pToolConnect->pNodeItemB = pNodeItem;
    pToolConnect->pNodeItemB->ModelObject()->Highlight();
    if( pToolConnect->pElementItem == null ) {
      ElementPtr pElem = pModel->Struct()->AddElem( pNodeA, pNodeB );
      pElem->Highlight();
      pModel->Agent()->Update();
      pToolConnect->pElementItem = pScene->GetItem( pElem );
    }
    else {
      ElementPtr pElement = dgtype_cast< ElementPtr >( pToolConnect->pElementItem->ModelObject() );
      pElement->Change( 2, pNodeB );
      pModel->Agent()->Update();
    }
    SetExamineMsg( pToolConnect->pElementItem->ModelObject() );    
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolConnectPtr pToolConnect = static_cast< ToolConnectPtr >( pTool );
    if( pToolConnect->pNodeItemA != null )
      pToolConnect->pNodeItemA->ModelObject()->Highlight( false );
    if( pToolConnect->pNodeItemB != null )
      pToolConnect->pNodeItemB->ModelObject()->Highlight( false );
    if( pToolConnect->pElementItem != null )
      pToolConnect->pElementItem->ModelObject()->Highlight( false );

    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    pModel->ActionStack().Complete( "ConnectPoints" );
    if( pToolConnect->pElementItem != null )
      emit UpdateObjectInfo( pToolConnect->pElementItem->ModelObject() );
    emit UpdateModelInfo();//1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotReposition( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotReposition" );
  UNUSED(position);
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;

    if( !pModel->HasTemplate() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOTEMPLATE );
      break;
    }

    ToolRepositionPtr pToolRepos = new ToolReposition( true );
    pTool = pToolRepos;
    pToolRepos->isPossible = true;

    Point position_t = _C( mapToScene( position ) );
    if( pModel->GetTemplate()->GetNearestPoint( position_t, &pToolRepos->pt, &pToolRepos->p0 ) != 0 ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::EMPTY_TEMPLATE );
      delete pTool;
      pTool = null;
      break;
    }

    if( pModel->GetTemplate()->DragStatus() != 0 &&
        pToolRepos->pt == pModel->GetTemplate()->DragPoint() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::REPOS_TEMPLATE_1 );
      delete pTool;
      pTool = null;
      break;
    }

    pToolRepos->pPointItem = pScene->CreateShapeItem( ViewShapeItem::POINT, pToolRepos->p0 );

    emit ChangeShowFlag( SHW::CHORDS, true );
    emit ChangeShowFlag( SHW::TEMPLATE, true );
    emit ChangeShowFlag( SHW::CHORDS3D, true );

    pConsole->Send( STATUS_INFO, FULL_SENDER,
                  (pModel->GetTemplate()->DragStatus() != 0) ?
                    DGMSG::REPOS_HANDLE_2 : DGMSG::REPOS_HANDLE_1 );
    pModel->Agent()->Update();
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolRepositionPtr pToolRepos = new ToolReposition();
    pTool = pToolRepos;

    QPointF position_t = mapToScene( position );
    double dist_element = 0., dist_separator = 0.;
    pToolRepos->pSeparatorItem = pScene->HitItemByType( SHW::SEPARATORS, position_t, &dist_separator );
    pToolRepos->pElementItem = pScene->HitItemByType( SHW::ELEMS, position_t, &dist_element );
    ElementPtr pElement = dgtype_cast< ElementPtr >( pToolRepos->pElementItem->ModelObject() );
    pElement->DistToPoint( _C( position_t ), &pToolRepos->posFlag );

    if( pToolRepos->pElementItem == null && pToolRepos->pSeparatorItem == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOELEMS );
      delete pTool;
      pTool = null;
      break;
    }
    if( pToolRepos->pElementItem != null && pToolRepos->pSeparatorItem != null ) {
      if( dist_element < dist_separator )
        pToolRepos->pSeparatorItem = null;
      else
        pToolRepos->pElementItem = null;
    }

    emit ChangeShowFlag( SHW::ELEMS, true );
    emit ChangeShowFlag( SHW::SEPARATORS, true );

    if( pToolRepos->pElementItem != null ) {
      pToolRepos->isPossible = !pElement->IsLocked();
      pToolRepos->posFlag = abs( pToolRepos->posFlag );
      pToolRepos->pNodeItem_first = pScene->GetItem( pElement->Node( pToolRepos->posFlag ) );

      pElement->Highlight();
      pToolRepos->pNodeItem_first->ModelObject()->Highlight();
    }
    else {
      SeparatorPtr pSeparator = dgtype_cast< SeparatorPtr >( pToolRepos->pSeparatorItem->ModelObject() );
      pToolRepos->isPossible = !pSeparator->IsLocked();
      pToolRepos->pNodeItem_first = pScene->GetItem( pSeparator->Node() );

      pSeparator->Highlight();
      pToolRepos->pNodeItem_first->ModelObject()->Highlight();
    }

    if( !pToolRepos->isPossible ) {
      if( pToolRepos->pElementItem != null )
        pModel->ShowLockReasonOf( pToolRepos->pElementItem->ModelObject() );
      else if( pToolRepos->pSeparatorItem != null )
        pModel->ShowLockReasonOf( pToolRepos->pSeparatorItem->ModelObject() );

      pModel->ActionStack().Cancel();
      delete pTool;
      pTool = null;
      break;
    }
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    QPointF position_t = mapToScene( position );
    ToolRepositionPtr pToolRepos = static_cast< ToolRepositionPtr >( pTool );
    if( pTool->isShifted ) {
      if( !pTool->isMoved )
        pTool->isMoved = true;

      IViewItemPtr pNodeItem = pScene->HitItemByType( SHW::NODES, position_t );
      if( pToolRepos->pNodeItem_second != pNodeItem ) {
        if( pToolRepos->pNodeItem_second != null )
          pToolRepos->pNodeItem_second->ModelObject()->Highlight( false );
        pToolRepos->pNodeItem_second = pNodeItem;
        if( pToolRepos->pLineItem != null ) {
          pScene->RemoveShapeItem( pToolRepos->pLineItem );
          pToolRepos->pLineItem = null;
        }
        NodePtr pNode = dgtype_cast< NodePtr >( pNodeItem->ModelObject() );
        pToolRepos->pLineItem = pScene->CreateShapeItem( ViewShapeItem::LINE, pToolRepos->p0, pNode->Position() );
      }
    }
    else {
      IViewItemPtr pNodeItem = pScene->HitItemByType( SHW::NODES, position_t );
      if( pToolRepos->pElementItem != null ) {
        if( pNodeItem == null )
          break;
        ElementPtr pElement = dgtype_cast< ElementPtr >( pToolRepos->pElementItem->ModelObject() );
        if( pNodeItem->ModelObject() == pElement->Node(3 - pToolRepos->posFlag) )
          break;
        NodePtr pNode = dgtype_cast< NodePtr >( pNodeItem->ModelObject() );
        if( pNode->IsConnectedWith( pElement->Node(3 - pToolRepos->posFlag) ) )
          break;

        pToolRepos->pNodeItem_first->ModelObject()->Highlight( false );
        pElement->Change( pToolRepos->posFlag, pNode );
        pToolRepos->pNodeItem_first = pScene->GetItem( pElement->Node( pToolRepos->posFlag ) );
      }
      else {
        if( pNodeItem == null )
          break;
        SeparatorPtr pSeparator = dgtype_cast< SeparatorPtr >( pToolRepos->pSeparatorItem->ModelObject() );
        if( pNodeItem->ModelObject() == pSeparator->Node() )
          break;
        NodePtr pNode = dgtype_cast< NodePtr >( pNodeItem->ModelObject() );

        pToolRepos->pNodeItem_first->ModelObject()->Highlight( false );
        pSeparator->Change( pNode );
        pToolRepos->pNodeItem_first = pScene->GetItem( pSeparator->Node() );
        pToolRepos->pNodeItem_first->ModelObject()->Highlight();
      }
    }
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolRepositionPtr pToolRepos = static_cast< ToolRepositionPtr >( pTool );
    IComponentPtr pObjInfo = null;
    if( pToolRepos->pElementItem != null ) {
      pToolRepos->pElementItem->ModelObject()->Highlight( false );
      pObjInfo = pToolRepos->pElementItem->ModelObject();
    }
    else if( pToolRepos->pSeparatorItem != null ) {
      pToolRepos->pSeparatorItem->ModelObject()->Highlight( false );
      pObjInfo = pToolRepos->pSeparatorItem->ModelObject();
    }

    if( pToolRepos->pNodeItem_first != null )
      pToolRepos->pNodeItem_first->ModelObject()->Highlight( false );
    if( pToolRepos->pNodeItem_second != null )
      pToolRepos->pNodeItem_second->ModelObject()->Highlight( false );

    if( pToolRepos->pPointItem != null )
      pScene->RemoveShapeItem( pToolRepos->pPointItem );
    if( pToolRepos->pLineItem != null )
      pScene->RemoveShapeItem( pToolRepos->pLineItem );

    int result = 0;
    if( pTool->isShifted ) {
      if( pTool->isMoved ) {
        if( pToolRepos->pElementItem != null )
          pModel->ShowLockReasonOf( pToolRepos->pElementItem->ModelObject() );
        else if( pToolRepos->pSeparatorItem != null )
          pModel->ShowLockReasonOf( pToolRepos->pSeparatorItem->ModelObject() );

        ToolRepositionPtr pToolRepos = static_cast< ToolRepositionPtr >( pTool );
        if( pToolRepos->pNodeItem_second != null ) {
          NodePtr pNode = dgtype_cast< NodePtr >( pToolRepos->pNodeItem_second->ModelObject() );
          pToolRepos->p0 = pNode->Position() - pToolRepos->p0;
          TemplatePtr pTempl = pModel->GetTemplate();
          if( pTempl->DragStatus() != 0 ) {
            /* Apply formulas */
            Point pa = pTempl->DragPoint();
            Point xy = pa;
            Point p1 = pTempl->TransformPoint( xy );
            Point pb = pToolRepos->pt;
            Point p2 = pNode->Position();
            result = pTempl->PlaceByHandles( pa, p1, pb, p2 );
            if( result == 0 )
              pTempl->SetDragStatus( 0, Point( 0. ) );
          }
          else {
            Point pa = pToolRepos->pt;
            Point p1 = pNode->Position();
            result = pTempl->PlaceByHandles( pa, p1, pa, p1 );
            if( result == 0 )
              pTempl->SetDragStatus( 1, pa );
          }

          delete pTool;
          pTool = null;
          if( result != 0 )
            pModel->ActionStack().Cancel();
          else
            pModel->ActionStack().Complete( "Tool:Reposition" );

          pConsole->Send( STATUS_INFO, FULL_SENDER, result );
        }
      }
      else { /* not moved */
        delete pTool;
        pTool = null;
        pModel->ActionStack().Cancel();
        pConsole->Send( STATUS_WARN, FULL_SENDER, DGMSG::CANCELED );
      }
    }
    else { /* not shifted */
      if( pTool->isPossible )
        pConsole->Send( STATUS_INFO, FULL_SENDER, "" );

      delete pTool;
      pTool = null;

      pModel->ActionStack().Complete( "Tool:Reposition" );
    }
    emit UpdateObjectInfo( pObjInfo );
    emit UpdateModelInfo(); //1408
    break;
  }
  }
}

void CViewWnd::slotReverseNormals( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotReverseNormals" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;

    ToolReversePtr pToolReverse = new ToolReverse( true );
    pTool = pToolReverse;
    IViewItemPtr pItem = pScene->HitItemByType( SHW::ELEMS, mapToScene( position ) );
    if( pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }
    ElementPtr pElement = dgtype_cast< ElementPtr >( pItem->ModelObject() );
    IComponentList elements;
    pElement->GetChain( null, null, &elements );
    pToolReverse->items = pScene->GetItemsList( elements );

    foreach( IViewItemPtr pItem, pToolReverse->items ) {
      pItem->ModelObject()->Highlight();
      if( pItem->ModelObject()->IsLocked() ) {
        pToolReverse->isPossible = false;
        pModel->ShowLockReasonOf( pItem->ModelObject() );
      }
    }

    if( pToolReverse->isPossible ) {
      foreach( IViewItemPtr pItem, pToolReverse->items ) {
        ElementPtr pElement = dgtype_cast< ElementPtr >( pItem->ModelObject() );
        pElement->Revert();
      }
    }
    emit ChangeShowFlag( SHW::ELEMS, true );
    emit ChangeShowFlag( SHW::NORMALS, true );
    pModel->Agent()->Update();
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolReversePtr pToolReverse = new ToolReverse();
    pTool = pToolReverse;

    IViewItemPtr pItem = pScene->HitViewItem( mapToScene( position ), SHW::ELEMS | SHW::CHORDS );
    if( pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }

    pToolReverse->items.append( pItem );
    pToolReverse->objects.push_back( pItem->ModelObject() );
    emit ChangeShowFlag( SHW::ELEMS, true );
    emit ChangeShowFlag( SHW::CHORDS, true );
    emit ChangeShowFlag( SHW::NORMALS, true );
    pToolReverse->isPossible = !pItem->ModelObject()->IsLocked();
    pItem->ModelObject()->Highlight();
    if( pToolReverse->isPossible ) {
      if( pItem->ModelObject()->Type() == OT::ELEMENT ) {
        pToolReverse->pItem = pItem;
        ElementPtr pElement = dgtype_cast< ElementPtr >( pItem->ModelObject() );
        pElement->Revert();
      }
      else {
        pToolReverse->pItem = pItem;
        ChordPtr pChord = dgtype_cast< ChordPtr >( pItem->ModelObject() );
        pChord->Change( pChord->Point_2(), pChord->Point_1() );
      }
    }
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    if( pTool->isShifted )
      break;
    ToolReversePtr pToolReverse = static_cast< ToolReversePtr >( pTool );
    if( !pToolReverse->isPossible )
      break;

    if( pToolReverse->pItem->ModelObject()->Type() == OT::ELEMENT ) {
      IViewItemPtr pItem = pScene->HitItemByType( SHW::ELEMS, mapToScene( position ) );
      if( pItem == null || pItem == pToolReverse->pItem || pItem->ModelObject()->IsLocked() )
        break;
      ElementPtr pElementPrev = dgtype_cast< ElementPtr >( pToolReverse->pItem->ModelObject() );
      ElementPtr pElement = dgtype_cast< ElementPtr >( pItem->ModelObject() );
      int connected = pElementPrev->IsConnectedWith( pElement );
      if( connected == 0 )
        break;
      if( pElement->Node(1)->IsRegular( &pToolReverse->objects ) )
        break;
      if( pElement->Node(2)->IsRegular( &pToolReverse->objects ) )
        break;
      pToolReverse->pItem = pItem;
      if( pToolReverse->items.contains( pItem ) )
        break;
      pElement->Highlight();
      pToolReverse->items.append( pItem );
      pToolReverse->objects.push_back( pItem->ModelObject() );
      if( connected == 2 )
        pElement->Revert();
    }
    else {
      IViewItemPtr pItem = pScene->HitItemByType( SHW::CHORDS, mapToScene( position ) );
      if( pItem == null || pItem == pToolReverse->pItem || pItem->ModelObject()->IsLocked() )
        break;
      pToolReverse->pItem = pItem;
      if( pToolReverse->items.contains( pItem ) )
        break;
      pItem->ModelObject()->Highlight();
      pToolReverse->items.append( pItem );
      pToolReverse->objects.push_back( pItem->ModelObject() );
      ChordPtr pChord = dgtype_cast< ChordPtr  >( pItem->ModelObject() );
      pChord->Change( pChord->Point_2(), pChord->Point_1() );
    }

    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolReversePtr pToolReverse = static_cast< ToolReversePtr >( pTool );
    foreach( IViewItemPtr pItem, pToolReverse->items )
      pItem->ModelObject()->Highlight( false );

    IComponentPtr pObjInfo = ( pTool->pItem != null ) ? pTool->pItem->ModelObject() : null;

    if( !pToolReverse->isPossible ) {
      if( pTool->pItem != null )
        pModel->ShowLockReasonOf( pToolReverse->pItem->ModelObject() );
      delete pTool;
      pTool = null;
      pModel->ActionStack().Cancel();
    }
    else {
      delete pTool;
      pTool = null;
      pModel->ActionStack().Complete( "Tool:ReverseNormals" );
      pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    }
    emit UpdateObjectInfo( pObjInfo );
    emit UpdateModelInfo(); //1408
    break;
  }
  }
}

void CViewWnd::slotAdjustChord ( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotAdjustChord" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolAdjustPtr pToolAdjust = new ToolAdjust( _C( mapToScene( position ) ) );
    pTool = pToolAdjust;
    pToolAdjust->pItem = pScene->HitItemByType( SHW::CHORDS, mapToScene( position ) );

    if( pToolAdjust->pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }

    ChordPtr pChord = dgtype_cast< ChordPtr >( pToolAdjust->pItem->ModelObject() );
    pChord->DistToPoint( _C( mapToScene( position ) ), &pToolAdjust->posFlag, pScene->IsTopView(), showFlags );
    pToolAdjust->len = (abs( pToolAdjust->posFlag ) == 1) ?
                         hypot( pChord->Point_1().x, pChord->Point_1().z ) :
                         hypot( pChord->Point_2().x, pChord->Point_2().z );
    pToolAdjust->isPossible = !pChord->IsLocked();
    pChord->Highlight();
    if( pToolAdjust->isPossible )
      SetExamineMsg( pChord );
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->isShifted )
      break;
    ToolAdjustPtr pToolAdjust = static_cast< ToolAdjustPtr >( pTool );
    if( !pToolAdjust->isPossible )
      break;

    Point pos_t = _C( mapToScene( position ) );

    ChordPtr pChord = dgtype_cast< ChordPtr >( pToolAdjust->pItem->ModelObject() );
    if( !pScene->IsTopView() ) {
        if( abs( pToolAdjust->posFlag ) == 1 )
          pChord->Change( Point( pChord->Point_1().x, pos_t.y, pChord->Point_1().z ), pChord->Point_2() );
        else
          pChord->Change( pChord->Point_1(), Point( pChord->Point_2().x, pos_t.y, pChord->Point_2().z ) );
        SetExamineMsg( pChord );
    }
    else {
        double factor = pToolAdjust->len / hypot( pos_t.x, pos_t.y );
        pos_t = pos_t.SwapYZ() * factor;
        if( abs( pToolAdjust->posFlag ) == 1 ) {
          pos_t.y = pChord->Point_1().y;
          pChord->Change( pos_t, pChord->Point_2() );
        }
        else {
          pos_t.y = pChord->Point_2().y;
          pChord->Change( pChord->Point_1(), pos_t );
        }
        SetExamineMsg( pChord );
    }

    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    ToolAdjustPtr pToolAdjust = static_cast< ToolAdjustPtr >( pTool );
    if( pToolAdjust->pItem != null )
      pToolAdjust->pItem->ModelObject()->Highlight( false );
    pModel->ActionStack().Complete( "Tool:AdjustChord" );
    if( pToolAdjust->pItem != null )
      emit UpdateObjectInfo( pToolAdjust->pItem->ModelObject() );
    if( pToolAdjust->isPossible )
      pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}



void CViewWnd::slotExtendChord ( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotExtendChord" );
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
    pTool->pItem = pScene->HitItemByType( SHW::CHORDS, mapToScene( position ) );
    if( pTool->pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }
    emit ChangeShowFlag( SHW::CHORDS, true );
    //TODO: emit ChangeShowFlag( SHW::CHORDS3D, true );

    pTool->pItem->ModelObject()->Highlight();
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->isShifted )
      break;

    IViewItemPtr pItem = pScene->HitItemByType( SHW::CHORDS, mapToScene( position ) );

    if( pItem == null || pItem == pTool->pItem )
      break;
    pTool->pItem->ModelObject()->Highlight( false );
    pTool->pItem = pItem;
    pTool->pItem->ModelObject()->Highlight();
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null || pTool->isShifted )
      break;

    if( pTool->pItem != null ) { /* eventually replace this with an error thing */
      ChordPtr pChord = dgtype_cast< ChordPtr >( pTool->pItem->ModelObject() );
      if( pChord->Extend( pScene->IsTopView() ) != 0 )
        pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOELEMSINPATH );
      pTool->pItem->ModelObject()->Highlight( false );
    }
    pModel->ActionStack().Complete( "Tool:ExtendChord" );
    if( pTool->pItem != null )
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  }
}
