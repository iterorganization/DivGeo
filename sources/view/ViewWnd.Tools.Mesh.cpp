#include "../main/MainWnd.h"
#include "ViewWnd.h"

ViewItemsSet CViewWnd::MeshPointsViewItems( IComponentList& processed_points )
{
  /* Collect connected elements and cells */
  IComponentList connected_elements;
  IComponentList connected_cells;
  FOREACHPTRCONST( MeshPointPtr, pMP, processed_points ) {
    const IComponentList& tmpcells = pMP->ConnectedCells();
    FOREACHPTRCONST( MeshCellPtr, pMC, tmpcells ) {
      connected_cells.insert( connected_cells.end(), tmpcells.begin(), tmpcells.end() );
      const IComponentList& tmpelms = pMC->ConnectedElements();
      connected_elements.insert( connected_elements.end(), tmpelms.begin(), tmpelms.end() );
    }
  }

  /* Select their view items */
  ViewItemsSet selected_items = ViewItemsSet::fromList( pScene->GetItemsList( connected_elements ) );
  selected_items.unite( ViewItemsSet::fromList( pScene->GetItemsList( connected_cells ) ) );
  selected_items.unite( ViewItemsSet::fromList( pScene->GetItemsList( processed_points ) ) );

  /* Mesh points has no view geometry */
  return selected_items;
}

ViewItemsSet CViewWnd::MeshPointDragHighlightWithBorder( MeshPointPtr _pPoint, bool include )
{
  IComponentList processed_points = _pPoint->HighlightDragH( include );
  processed_points.sort();
  processed_points.unique();
  ViewItemsSet selected_items = MeshPointsViewItems( processed_points );
  return selected_items;
}

ViewItemsSet CViewWnd::MeshPointDragHighlightError( MeshPointPtr _pPoint, bool include )
{
  _pPoint->HighlightDrag( include );
  IComponentList processed_points;
  processed_points.push_back( _pPoint );
  ViewItemsSet selected_items = MeshPointsViewItems( processed_points );
  foreach( IViewItemPtr pItem, selected_items ) {
    pItem->SetFlag( IViewItem::p_showError, include );
    pItem->UpdateStyle();
  }
  return selected_items;
}

ViewItemsSet CViewWnd::MeshPointDragHighlightUpdate( MeshPointPtr _pPoint, bool include )
{
  _pPoint->HighlightDrag( include );
  IComponentList processed_points;
  processed_points.push_back( _pPoint );
  ViewItemsSet selected_items = MeshPointsViewItems( processed_points );
  return selected_items;
}

void CViewWnd::MeshPointChangeGeometry( MeshPointPtr _pPoint, Point _position, bool _freeMotion )
{
  if( _freeMotion )
    _pPoint->Change( _position );
  else
    _pPoint->MoveH( _position );
}

void CViewWnd::slotMoveMeshPoint( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotMoveMeshPoint" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  case PRESSED:
  {
    if( pTool != null )
      break;

    Point position_t = _C( mapToScene( position ) );

    // Hit object
    emit ChangeShowFlag( SHW::MESH, true );
    double hit_radius = pPrefs->visual.meshPointRadius / state.scale.x;
    IViewItemPtr pItem = pScene->HitViewItem(
                           _C( position_t ), SHWX::MESHPOINTS, hit_radius );
    if( pItem == null )
      return;

    // Build tool data
    ToolMoveMeshPointPtr pToolMove = new ToolMoveMeshPoint( eventid == SHIFTCLICK );
    pTool = pToolMove;
    pToolMove->pItem = pItem;
    pToolMove->isPossible = !pItem->ModelObject()->IsLocked();

    MeshPointPtr pPoint = dgtype_cast< MeshPointPtr >( pItem->ModelObject() );
    if( pPoint->CutFlag() != 0 ) {
      pToolMove->isPossible = false;
      //TODO: RemoveAllLabels();
      pToolMove->group = MeshPointDragHighlightError( pPoint );
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::MOVE_CUT_MESH_PT );
      bReleaseQueued = true; // auto release
      break;
    }

    if( pTool->isShifted ) { /* Free point motion */
      //TODO: RemoveAllLabels();
      pToolMove->group = MeshPointDragHighlightUpdate( pPoint );
    }
    else /* Dependent point motion */
      if( !SetupSlideMeshPoint( pToolMove ) ) {
        bReleaseQueued = true; // auto release
        break;
      }

    SetExamineMsg( pPoint );

    pModel->Agent()->Update();
    break;
  }

  case MOVED:
  {
    if( pTool == null || pTool->pItem == null )
      break;

    ToolMoveMeshPointPtr pToolMove = static_cast< ToolMoveMeshPointPtr >( pTool );
    MeshPointPtr pMP = dgtype_cast< MeshPointPtr >( pToolMove->pItem->ModelObject() );
    Point position_t = _C( mapToScene( position ) );

    if( pMP->Position() == position_t )
      break;

    if( pTool->isShifted ) {
      MeshPointChangeGeometry( pMP, position_t, true );
      pTool->isMoved = true;
    }
    else
      DoSlideMeshPoint( pToolMove, position_t );

    SetExamineMsg( pMP );
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;

    ToolMoveMeshPointPtr pToolMove = static_cast< ToolMoveMeshPointPtr >( pTool );

    /* Unhighlight all used objects */
    if( pToolMove->isShifted ) {
      MeshPointPtr pPoint = dgtype_cast< MeshPointPtr >( pToolMove->pItem->ModelObject() );
      MeshPointDragHighlightWithBorder( pPoint, false );
    }
    else if( pToolMove->ptsCount > 0 ) {
      for( int i = 0; i < pToolMove->ptsCount; i++ ) {
        MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];
        MeshPointDragHighlightWithBorder( pMPN->pPoint, false );
      }
    }
    else {
      foreach( IViewItemPtr pItem, pToolMove->group ) {
        pItem->ModelObject()->Highlight( false );
        pItem->SetFlag( IViewItem::p_showError, false );
        pItem->UpdateStyle();
      }
    }

    if( pToolMove->pSpline != null )
      delete pToolMove->pSpline;

    if( pTool->isPossible )
      pConsole->Send( STATUS_INFO, FULL_SENDER, "" );

    if( pTool->isPossible && pTool->isMoved )
      pModel->ActionStack().Complete( "Tool:MoveMeshPoint" );
    else
      pModel->ActionStack().Cancel();

    /* Update information */
    emit UpdateObjectInfo( pTool->pItem == null ? null : pTool->pItem->ModelObject() );
    emit UpdateModelInfo(); //1408
    delete pTool;
    pTool = null;
    break;
  }
  default:
    break;
}
}

bool CViewWnd::SetupSlideMeshPoint( ToolMoveMeshPointPtr pToolMove )
{
  SENDER_NAME_Q( "SetupSlideMeshPoint" );
  assert( !pToolMove->isShifted );
  MeshPointPtr pPoint = dgtype_cast< MeshPointPtr >( pToolMove->pItem->ModelObject() );

  /* If we are on a double border, jump to the outermost mesh point */
  if( pModel->HasDoubleMeshBorder() ) {
    pPoint = pPoint->OutermostPoint();
    if( pPoint == null )
      return false;
    pToolMove->pItem = pScene->GetItem( pPoint );
  }

  /* Calculate the line along which to slide */
  if( pModel->CheckMeshSlidingMode( MSM::USE_SURFACE ) ) {

    /* Check if equil exists */
    if( !pModel->GetFluxModel()->HasEquil() ) {
      pToolMove->isPossible = false;
      pPoint->HighlightDrag();
      pToolMove->group = MeshPointDragHighlightError( pPoint );
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NOEQUIL );
      return false;
    }
    int err = 0;
    /* Try create appropriate surface */
    SurfaceExPtr pSX = pModel->GetFluxModel()->AddSurfaceEx( pPoint->Backup(), &err );
    if( pSX == null ) {
      pToolMove->isPossible = false;
      pToolMove->group = MeshPointDragHighlightError( pPoint );
      pConsole->Send( WND_ERROR, FULL_SENDER, err );
      return false;
    }
    /* Check if surface is good */
    if( !pSX->CoordsOk() || !pSX->HasLine() ) {
      pSX->Delete();
      pToolMove->isPossible = false;
      pToolMove->group = MeshPointDragHighlightError( pPoint );
      pConsole->Send( WND_ERROR, FULL_SENDER, DG3::BAD_SURFACE );
      return false;
    }

    /* Copy line */
    pToolMove->line = *pSX->Line();

    if( pSX->IsClosed() ) {
      // Move first half of the sequence to the end
      //  (the same as to move points one by one from begin to end)
      PointArray tmp = pToolMove->line;
      PointIter itMidPoint = tmp.begin();
      for( unsigned i = 0; i < tmp.size() / 2; i++ )
        itMidPoint++;

      pToolMove->line.clear();
      pToolMove->line.insert( pToolMove->line.begin(), itMidPoint, tmp.end() );
      pToolMove->line.insert( pToolMove->line.end(), tmp.begin(), itMidPoint );
    }

    pSX->Delete();
  } // end if surface sliding

  else if( pModel->CheckMeshSlidingMode( MSM::USE_POINTS ) ) {
    MeshPointList chain = pPoint->GetChain( false );
    assert( !chain.empty() );

    pToolMove->line.clear();
    pToolMove->line.resize( chain.size() );
    ulong c = 0;
    FOREACH_CONST( MeshPointIterConst, itMP, chain )
      pToolMove->line[ c++ ] = (*itMP)->Backup();
  }
  else
    assert( false );

  if( pModel->CheckMeshSlidingMode( MSM::USE_SPLINE ) )
    pToolMove->pSpline = new ParamSpline( pToolMove->line );

  /* Determine the current position on the line */
  double pos = ProjectPointToPolyLine( pToolMove->line, pPoint->Backup() );

  /* Get a chain of neighbours */
  MeshPointList chain = pPoint->GetChain( true );
  assert( !chain.empty() );

  if( chain.size() < 3 ) {
    chain.clear();
    pToolMove->isPossible = false;
    pToolMove->group = MeshPointDragHighlightError( pPoint );
    pConsole->Send( WND_ERROR, FULL_SENDER, DG3::MP_CHAIN_IS_TOO_SHORT );
    return false;
  }

  /* Re-align the chain so that the position value always grows */
  int index = std::distance( chain.begin(), std::find( chain.begin(), chain.end(), pPoint ) );
  if( index > 0 ) {
    MeshPointIter itMP1 = chain.begin();
    std::advance( itMP1, index - 1 );
    double pos1 = ProjectPointToPolyLine( pToolMove->line, (*itMP1)->Backup() );
    if( pos1 > pos )
      std::reverse( chain.begin(), chain.end() );
  }
  else if( index < (int)chain.size() - 1 ) {
    MeshPointIter itMP1 = chain.begin();
    std::advance( itMP1, index + 1 );
    double pos1 = ProjectPointToPolyLine( pToolMove->line, (*itMP1)->Backup() );
    if( pos1 < pos )
      std::reverse( chain.begin(), chain.end() );
  }

  /* Allocate the pts array */
  pToolMove->ptsCount = chain.size();
  pToolMove->ptsPtNo = std::distance( chain.begin(), std::find( chain.begin(), chain.end(), pPoint ) );

  pToolMove->ptsA = MeshPointNeighbourArray( pToolMove->ptsCount );
  pToolMove->pts = MeshPointNeighbourArray( pToolMove->ptsCount );

  int i = 0;
  FOREACH_CONST( MeshPointIterConst, itMP1, chain ) {
    MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];
    pMPN->pPoint = *itMP1;
    pMPN->oldPnt = (*itMP1)->Position();
    pMPN->oldPos = ProjectPointToPolyLine( pToolMove->line, (*itMP1)->Position() );
    pMPN->isMoved = false;
    pToolMove->group.unite( MeshPointDragHighlightWithBorder( pMPN->pPoint, false ) );
    i++;
  }

  /* Calculate array and position limits */
  pToolMove->ptsMin = -1;
  for( int i = pToolMove->ptsPtNo - 1; i >= 0; i-- ) {
    if( pToolMove->pts[i].pPoint->CutFlag() ) {
      pToolMove->ptsMin = i;
      break;
    }
    if( pToolMove->pts[i].oldPos > pToolMove->pts[i+1].oldPos ) {
      pToolMove->ptsMin = i + 1;
      break;
    }
  }

  pToolMove->ptsMax = pToolMove->ptsCount;
  for( int i = pToolMove->ptsPtNo + 1; i < pToolMove->ptsCount; i++ ) {
    if( pToolMove->pts[i].pPoint->CutFlag() ) {
      pToolMove->ptsMax = i;
      break;
    }
    if( pToolMove->pts[i].oldPos < pToolMove->pts[i-1].oldPos ) {
      pToolMove->ptsMax = i - 1;
      break;
    }
  }

  /* Detect double lines at ends */

  /*  d->bDoubleLeft=d->bDoubleRight=0;
  if (d->ptsCount>=3) {
    if (d->ptsMin<0 && MeshPointDist(d->pts[0].mpt,d->pts[1].mpt)<
        w->app->meshBorderThreshold) d->bDoubleLeft=1;
    if (d->ptsMax>=d->ptsCount &&
        MeshPointDist(d->pts[d->ptsCount-1].mpt,d->pts[d->ptsCount-2].mpt)<
        w->app->meshBorderThreshold) d->bDoubleRight=1;
  } */

  assert( pToolMove->ptsCount >= 3 );

/*  if (d->bDoubleLeft && d->ptsPtNo==1)
    d->mpt=d->pts[--d->ptsPtNo].mpt;

  if (d->bDoubleRight && d->ptsPtNo==d->ptsCount-2)
    d->mpt=d->pts[++d->ptsPtNo].mpt;*/

  /* Calculate position limits */

  int doubleBorderOffset = pModel->HasDoubleMeshBorder() ? 1 : 0;
  double dpL = dg_max( 0, pToolMove->ptsPtNo - doubleBorderOffset ) * pModel->MeshSlidingThreshold();
  double dpR = dg_max( 0, pToolMove->ptsCount - pToolMove->ptsPtNo - doubleBorderOffset - 1 ) * pModel->MeshSlidingThreshold();

  pToolMove->posMin = (pToolMove->ptsMin < 0) ? dpL : pToolMove->pts[pToolMove->ptsMin].oldPos + dpL;
  pToolMove->posMax = (pToolMove->ptsMax >= pToolMove->ptsCount) ? CalcLineLength( pToolMove->line ) - dpR : pToolMove->pts[pToolMove->ptsMax].oldPos - dpR;

  MeshPointDragHighlightWithBorder( pPoint );
  return true;
}



void CViewWnd::DoSlideMeshPoint( ToolMoveMeshPointPtr pToolMove, Point position )
{
  if( pToolMove == null || !pToolMove->isPossible )
    return;

  if( pToolMove->line.empty() )
    return;

  double pos = ProjectPointToPolyLine( pToolMove->line, position );
  double len = CalcLineLength( pToolMove->line );

  /* Treat different cases on closed surfaces? */
  if( pToolMove->posMin < pToolMove->posMax ) {
    pos = dg_max( pToolMove->posMin, pos );
    pos = dg_min( pToolMove->posMax, pos );
  }
  else if( pos > pToolMove->posMax &&
           pos < pToolMove->posMin ) {
    pos = ( fabs( pos - pToolMove->posMax ) <
            fabs( pos - pToolMove->posMin ) ) ?
          pToolMove->posMax :
          pToolMove->posMin;
  }

  /* Calculate the new position for the dragged point */
  Point np;
  if( pToolMove->pSpline != null )
    pToolMove->pSpline->CalcValue( pos, &np.x, &np.y );
  else
    GetPolyLinePoint( pToolMove->line, pos, &np.x, &np.y );

  //pConsole->slotSend( MT::LOG, FULL_SENDER, QString( "dbg: p{%1, %2} pos=%3 mp{%4, %5}" )
  //    .arg( position.x ).arg( position.y ).arg( pos ).arg( np.x ).arg( np.y ) );

  MeshPointPtr pPoint = dgtype_cast< MeshPointPtr >( pToolMove->pItem->ModelObject() );
  if( np == pPoint->Position() )
    return;

  MeshPointChangeGeometry( pPoint, np );
  pToolMove->isMoved = true;

  /* Push neighboring mesh points, if needed */

  /* To the left */
  int i = 0;
  double pos_fixed = pos;
  for( i = pToolMove->ptsPtNo - 1; i >= 0 && i > pToolMove->ptsMin; i-- ) {
    MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];

    if( pModel->HasDoubleMeshBorder() &&
        pMPN->pPoint->IsOnBorder() )
      continue;

    pos_fixed -= pModel->MeshSlidingThreshold();
    if( pos_fixed < 0 )
      pos_fixed += len;
    if( pMPN->oldPos <= pos_fixed )
      break;

    MeshPointDragHighlightWithBorder( pMPN->pPoint );

    if( pToolMove->pSpline != null )
      pToolMove->pSpline->CalcValue( pos_fixed, &np.x, &np.y );
    else
      GetPolyLinePoint( pToolMove->line, pos_fixed, &np.x, &np.y );

    MeshPointChangeGeometry( pMPN->pPoint, np );
    pMPN->isMoved = true;
  }

  /* Unhighlight & restore the remainders */
  if( i == 0 && pModel->HasDoubleMeshBorder() )
    i++;

  int k = i + 1;
  for( ; i >= 0; i-- ) {
    MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];
    if( pMPN->isMoved ) {
      MeshPointChangeGeometry( pMPN->pPoint, pMPN->oldPnt );
      MeshPointDragHighlightWithBorder( pMPN->pPoint, false );
      pMPN->isMoved = false;
    }
  }

  MeshPointNeighbourPtr pMPNk = &pToolMove->pts[k];
  MeshPointDragHighlightWithBorder( pMPNk->pPoint );

  /* To the right */
  pos_fixed = pos;
  for( i = pToolMove->ptsPtNo + 1; i < pToolMove->ptsCount && i < pToolMove->ptsMax; i++ ) {
    MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];
    if( pModel->HasDoubleMeshBorder() &&
        pMPN->pPoint->IsOnBorder() )
      continue;

    pos_fixed += pModel->MeshSlidingThreshold();

    if( pos_fixed > len )
      pos_fixed -= len;
    if( pMPN->oldPos >= pos_fixed )
      break;

    MeshPointDragHighlightWithBorder( pMPN->pPoint );

    if( pToolMove->pSpline != null )
      pToolMove->pSpline->CalcValue( pos_fixed, &np.x, &np.y );
    else
      GetPolyLinePoint( pToolMove->line, pos_fixed, &np.x, &np.y );

    MeshPointChangeGeometry( pMPN->pPoint, np );
    pMPN->isMoved = true;
  }

  /* Unhighlight & restore the remainders */
  if( i == pModel->GetMesh()->Size().x && pModel->HasDoubleMeshBorder() )
    i--;

  k = i - 1;
  for( ; i < pToolMove->ptsCount; i++ ) {
    MeshPointNeighbourPtr pMPN = &pToolMove->pts[i];
    if( pMPN->isMoved ) {
      MeshPointChangeGeometry( pMPN->pPoint, pMPN->oldPnt );
      MeshPointDragHighlightWithBorder( pMPN->pPoint, false );
      pMPN->isMoved = false;
    }
  }

  pMPNk = &pToolMove->pts[k];
  MeshPointDragHighlightWithBorder( pMPNk->pPoint );

  /* Move the companion if a double point is dragged */
}

