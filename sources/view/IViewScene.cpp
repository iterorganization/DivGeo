#include "IViewScene.h"

ulong IViewScene::OT2SHW( ObjectType _ot )
{
  switch( _ot ) {
  case OT::ELEMENT:       return SHW::ELEMS;
  case OT::SURFACEEX:     return SHW::SURFACES;
  case OT::SEPARATOR:     return SHW::SEPARATORS;
  case OT::SOURCE:        return SHW::SOURCES;
  case OT::CHORD:         return SHW::CHORDS | SHW::CHORDS3D;
  case OT::XPOINTTEST:    return SHW::XPOINTTESTS |SHW::XPOINT;
  case OT::XPOINTSEG:     return SHW::XPOINTSEGS;
  case OT::MESHCELL:      return SHWX::MESHCELLS;
  case OT::MESHELEMENT:   return SHWX::MESHELEMENTS;
  case OT::MESHPOINT:     return SHWX::MESHPOINTS;
  case OT::NODE:          return SHW::NODES;
  case OT::GRIDPOINTEX:   return SHW::GRIDPOINTS;
  case OT::GRIDPOINTSEG:  return SHW::GRIDPOINTS;
  case OT::COMMENT:       return SHW::COMMENTS;
  default: return 0;
  }
}

IViewItemPtr IViewScene::HitItemByType( ulong sf, QPointF position, double* pDist, double radius )
{
  ViewItemsList* pList = (sf == SHW::COMMENTS) ? &commentItems : SelectList( sf );
  if( pList == null )
    return null;
  double hitDist = 0.;
  IViewItemPtr hitItem = null;
  foreach( IViewItemPtr pItem, *pList ) {
    double dist = pItem->DistToPoint( position );
    if( (hitItem == null || dist < hitDist) && (radius == 0. || dist <= radius) ) {
      hitDist = dist;
      hitItem = pItem;
    }
  }
  if( pDist != null )
    *pDist = hitDist;
  return hitItem;
}

ViewItemsList IViewScene::GetItemsList(const IComponentList& objects, bool multitype )
{
  if( objects.empty() )
    return ViewItemsList();
  ViewItemsList* pList = multitype ? &viewItems :
                                SelectList( objects.front()->Type() );
  ViewItemsList result_list;
  if( pList == null )
    return result_list;
  foreach( IViewItemPtr pItem, *pList ) {
    if( pItem->ModelObject() == null )
      continue;
    if( Contains( objects, pItem->ModelObject() ) )
      result_list.append( pItem );
  }
  return result_list;
}

IViewItemPtr IViewScene::GetItem( IComponentPtr _pObject )
{
  if( _pObject == null )
    return null;
  IViewItemPtr pSingleItem = SelectSingleItem( _pObject->Type() );
  if( pSingleItem != null ) {
    if( pSingleItem->ModelObject() == _pObject )
      return pSingleItem;
    else
      return null;
  }
  ViewItemsList* pList = SelectList( _pObject->Type() );
  if( pList == null )
    return null;
  foreach( IViewItemPtr pItem, *pList ) {
    if( pItem->ModelObject() == null )
      continue;
    if( _pObject == pItem->ModelObject() )
      return pItem;
  }
  return null;
}

void IViewScene::Clear()
{
  QGraphicsScene::clear(); // Removes and deletes all items from the scene
  viewItems.clear();
  shapeItems.clear();
  labelItems.clear();
  commentItems.clear();
  pAxesItemA = null;
  pAxesItemO = null;
  pGridItem = null;
  pRadialGridItem = null;
  pHiRect = null;
  pRotTool = null;
  pLegendItem = null;
}

void IViewScene::Build( ulong sfs )
{
  const double sceneSize = 10000.; //TODO: model size can be more then this constant.
  this->setSceneRect( -sceneSize, -sceneSize, 2*sceneSize, 2*sceneSize );

  if( HasAnyFlag( sfs, SHW::AXES ) ) {
    if( pAxesItemA == null ) {
      pAxesItemA = new ViewAxesItem( true );
      pAxesItemA->SetSettingsManager( pPrefs );
      pAxesItemA->SetViewState( pState );
      viewItems.push_back( pAxesItemA );
      this->addItem( pAxesItemA );
    }

    if( pAxesItemO == null ) {
      pAxesItemO = new ViewAxesItem( false );
      pAxesItemO->SetSettingsManager( pPrefs );
      pAxesItemO->SetViewState( pState );
      viewItems.push_back( pAxesItemO );
      this->addItem( pAxesItemO );
    }
  }

  if( HasAnyFlag( sfs, SHW::GRID ) ) {
    if( pGridItem == null ) {
      pGridItem = new ViewGridItem();
      pGridItem->SetSettingsManager( pPrefs );
      pGridItem->SetViewState( pState );
      viewItems.push_back( pGridItem );
      this->addItem( pGridItem );
    }

    if( pRadialGridItem == null ) {
      pRadialGridItem = new ViewRadialGridItem();
      pRadialGridItem->SetSettingsManager( pPrefs );
      pRadialGridItem->SetViewState( pState );
      viewItems.push_back( pRadialGridItem );
      this->addItem( pRadialGridItem );
    }
  }

  if( pHiRect == null ) {
    pHiRect = new ViewHighlightedRectItem();
    pHiRect->SetSettingsManager( pPrefs );
    pHiRect->SetViewState( pState );
    viewItems.push_back( pHiRect );
    this->addItem( pHiRect );
  }

  if( pRotTool == null ) {
    pRotTool = new ViewRotateToolItem();
    pRotTool->SetSettingsManager( pPrefs );
    pRotTool->SetViewState( pState );
    viewItems.push_back( pRotTool );
    this->addItem( pRotTool );
  }
  pRotTool->setVisible( false );

  if( HasAnyFlag( sfs, SHW::COMMENTS ) ) {
    if( !commentItems.empty() ) {
      foreach( IViewItemPtr pItem, commentItems ) {
        viewItems.removeOne( pItem );
        ViewCommentItemPtr pViewItem = dgtype_cast< ViewCommentItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      commentItems.clear();
    }

    FOREACHPTRCONST( CommentPtr, pComment, pModel->Comments() ) {
      if( pComment->ParentScene() != (int)sceneType )
        continue;
      IViewItemPtr pLinkedItem = (pComment->ObjType() == OT::UNKNOWN) ? null :
        HitItemByType( OT2SHW( pComment->ObjType() ), _C( pComment->PosEnd() ) );
      ViewCommentItemPtr pItem = new ViewCommentItem( pComment,
                 (pLinkedItem == null) ? null : pLinkedItem->ModelObject() );
      if( pLinkedItem != null )
        pLinkedItem->IncludeComment( pItem );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      commentItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );
    }
  }

  /*if( pParams != null )
    SetParameters( pParams );*/
}

void IViewScene::UpdateHighlightRect( QPolygonF hr )
{
  pHiRect->SetRect( hr );
  pHiRect->UpdateGeometry();
}

void IViewScene::UpdateRotTool( bool enable )
{
  pRotTool->setVisible( enable );
  if( enable )
    pRotTool->UpdateGeometry();
}

void IViewScene::UpdateGrid()
{
  bool isEnabled = HasAnyFlag( showFlags, SHW::GRID );
  bool isRadial = (sceneType == ST::TOP && pState->radialGrid);
  pRadialGridItem->setVisible( isEnabled && isRadial );
  pGridItem->setVisible( isEnabled && !isRadial );

  if( sceneType == ST::TOP && pState->radialGrid ) {
    pRadialGridItem->UpdateGeometry();
    pRadialGridItem->UpdateDependentGeometry();
  }
  else {
    pGridItem->UpdateGeometry();
    pGridItem->UpdateDependentGeometry();
  }
  pAxesItemA->UpdateGeometry();
  pAxesItemA->UpdateDependentGeometry();
  pAxesItemO->UpdateGeometry();
  pAxesItemO->UpdateDependentGeometry();

  if( pLegendItem != null )
    pLegendItem->UpdateDependentGeometry();
}

bool IViewScene::UpdateShowFlag( ShowFlag sf, bool enable )
{
  if( ((showFlags & sf) != 0) == enable ) // flag's bit already has such value
    return false;

  if( enable )
    showFlags |= sf;
  else
    showFlags ^= sf;

  switch( sf ) {
  case SHW::AXES:
    pAxesItemA->setVisible( enable );
    pAxesItemO->setVisible( enable );
    return false;
    break;
  case SHW::GRID: {
    bool isRadial = (sceneType == ST::TOP && pState->radialGrid);
    pRadialGridItem->setVisible( enable && isRadial );
    pGridItem->setVisible( enable && !isRadial );
    }
    return false;
    break;
  case SHW::COMMENTS: {
    foreach( IViewItem* pItem, commentItems )
      pItem->SetVisible( enable );
    return false;
    break;
  }
  default:
    break;
  }

  return true;
}

void IViewScene::SetVisibility( ulong sfs, bool forced )
{
  ulong currentFlag = 0x00000001;

  if( forced )
    showFlags = sfs;
  for( uint c = 0; c < ShowFlags::size; c++ ) {
    bool enable = HasAnyFlag( sfs, currentFlag );
    if( forced )
      showFlags = showFlags ^ currentFlag;
    UpdateVisibility( (ShowFlag)currentFlag, enable );
    currentFlag *= 2;
  }
}

void IViewScene::SetViewState( const SViewState* _pState )
{
  if( pState == _pState )
    return;
  pState = _pState;
  foreach( IViewItemPtr pItem, viewItems )
    pItem->SetViewState( _pState );
}

void IViewScene::UpdateStyle()
{
    int c = 0;
  foreach( IViewItemPtr pItem, viewItems ) {
    c++;
    pItem->UpdateDependentGeometry();
    pItem->UpdateStyle();
  }
}

ViewItemsList IViewScene::ItemsInRectangle( ulong _flags, const QRect& _rect , const QTransform &_transform ) const
{
  UNUSED( _flags );
  UNUSED( _rect );
  UNUSED( _transform );
  return ViewItemsList();
}

ViewShapeItemPtr IViewScene::CreateShapeItem( ViewShapeItem::Shape _shape, const Point& _p1, const Point& _p2 )
{
  ViewShapeItemPtr pItem = new ViewShapeItem( _shape, _p1, _p2 );
  pItem->SetSettingsManager( pPrefs );
  pItem->SetViewState( pState );
  viewItems.append( pItem );
  shapeItems.append( pItem );
  addItem( pItem );
  return pItem;
}

void IViewScene::RemoveShapeItem( ViewShapeItemPtr _pItem )
{
  shapeItems.removeOne( _pItem );
  viewItems.removeOne( _pItem );
  removeItem( _pItem );
  delete _pItem;
}

void IViewScene::RemoveAllShapeItems()
{
  foreach( ViewShapeItemPtr pItem, shapeItems ) {
    viewItems.removeOne( pItem );
    removeItem( pItem );
    delete pItem;
  }
  shapeItems.clear();
}

ViewLabelItemPtr IViewScene::CreateLabelItem( IComponentPtr _pObject, const QString& _text )
{
  ViewLabelItemPtr pItem = new ViewLabelItem( _pObject, _text );
  pItem->SetSettingsManager( pPrefs );
  pItem->SetViewState( pState );
  viewItems.append( pItem );
  labelItems.append( pItem );
  addItem( pItem );
  return pItem;
}

void IViewScene::RemoveLabelItem( ViewLabelItemPtr _pItem )
{
  labelItems.removeOne( _pItem );
  viewItems.removeOne( _pItem );
  removeItem( _pItem );
  delete _pItem;
}

void IViewScene::RemoveAllLabelItems()
{
  foreach( ViewLabelItemPtr pItem, labelItems ) {
    viewItems.removeOne( pItem );
    removeItem( pItem );
    delete pItem;
  }
  labelItems.clear();
}

QRectF IViewScene::FullBoundingRect()
{
  bool showAxes = HasAnyFlag( showFlags, SHW::AXES );
  bool showGrid = HasAnyFlag( showFlags, SHW::GRID );
  UpdateShowFlag( SHW::AXES, false );
  UpdateShowFlag( SHW::GRID, false );
  pHiRect->SetVisible( false );
  bool showLegend = false;
  if( pLegendItem != null ) {
    showLegend = pLegendItem->isVisible();
    pLegendItem->SetVisible( false );
  }
  QRectF br;
  bool first = true;
  foreach( QGraphicsItem* pItem, items() ) {
    if( !pItem->isVisible() )
      continue;
    QRectF brItem = pItem->boundingRect();
    if( first ) {
      br = brItem;
      first = false;
    }
    else
      br = br.united( brItem );
  }
  UpdateShowFlag( SHW::AXES, showAxes );
  UpdateShowFlag( SHW::GRID, showGrid );
  pHiRect->SetVisible( true );
  if( pLegendItem != null )
    pLegendItem->SetVisible( showLegend );
  return br;
}

QRectF IViewScene::GroupBoundingRect( const IComponentList& _objects )
{
  ViewItemsList selected_items = GetItemsList(_objects, true );
  QRectF br;
  bool first = true;
  foreach( IViewItemPtr pItem, selected_items ) {
    QGraphicsItem* pGrItem = GetGraphicsItem( pItem );
    if( pGrItem == null || !pGrItem->isVisible() )
      continue;
    QRectF brItem = pGrItem->boundingRect();
    if( first ) {
      br = brItem;
      first = false;
    }
    else
      br = br.united( brItem );
  }
  return br;
}

void IViewScene::AddViewState( const ViewStateRecord& vsr ) { stackUndo.append( vsr ); }


ViewStateRecord IViewScene::UndoViewChange()
{
  ViewStateRecord vsr = stackUndo.last();
  stackRedo.append( vsr );
  stackUndo.pop_back();
  return vsr;
}

ViewStateRecord IViewScene::RedoViewChange()
{
  ViewStateRecord vsr = stackRedo.last();
  stackUndo.append( vsr );
  stackRedo.pop_back();
  return vsr;
}

void IViewScene::RemoveListItems( const IComponentList& _pObjects )
{
  FOREACHPTRCONST( IComponentPtr, pObject, _pObjects ) {
    IViewItemPtr pItem = GetItem( pObject );
    if( pItem == null ) {
      RemoveMultiItemObject( pObject );
      continue;
    }
    if( pItem->ModelObject()->Type() == OT::COMMENT ) {
      ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pItem );
      IViewItemPtr pLinkedItem = GetItem( pCommentItem->GetLinkedObject() );
      if( pLinkedItem != null )
        pLinkedItem->ExcludeComment( pCommentItem );
    }
    RemoveItem( pItem );
    delete pItem;
  }
}

void IViewScene::CreateListItems( const IComponentList& _pObjects )
{
  FOREACHPTRCONST( IComponentPtr, pObject, _pObjects ) {
    IViewItemPtr pItem = GetItem( pObject );
    if( pItem != null )
      continue;
    CreateItem( pObject );
  }
}

void IViewScene::FullUpdateListItems( const IComponentList& _pObjects )
{
  FOREACHPTRCONST( IComponentPtr, pObject, _pObjects ) {
    IViewItemPtr pItem = GetItem( pObject );
    if( pItem == null )
      continue;
    pItem->UpdateDependentGeometry();
    pItem->UpdateGeometry();
    pItem->UpdateStyle();
  }
}

void IViewScene::StyleUpdateListItems( const IComponentList& _pObjects )
{
  FOREACHPTRCONST( IComponentPtr, pObject, _pObjects ) {
    IViewItemPtr pItem = GetItem( pObject );
    if( pItem == null )
      continue;
    pItem->UpdateStyle();
  }
}
