#include "ViewScene.h"

ViewScene::ViewScene(ModelPtr _pModel, const QString &_name, SceneType _sceneType,
                     const UserPreferences* _pPrefs, QObject *parent ):
  IViewScene( _pModel, _name, _sceneType, _pPrefs, parent ),
  pEquilItem( null ),
  pTemplateItem( null ),
  pSonnetDataItem( null )
{
  Build();
}

ViewItemsList* ViewScene::SelectList( ulong sf )
{
  switch( sf ) {
  case SHW::ELEMS:          return &vpElementItems;
  case SHW::SURFACES:       return &vpSurfaceExItems;
  case SHW::SEPARATORS:     return &vpSeparatorItems;
  case SHW::SOURCES:        return &vpSourceItems;
  case SHW::CHORDS3D:
  case SHW::CHORDS:         return &vpChordItems;
  case SHW::XPOINTTESTS:    return &vpXPointTestItems;
  case SHW::XPOINTSEGS:     return &vpXPointSegItems;
  case SHWX::MESHCELLS:     return &vpMeshCellItems;
  case SHWX::MESHELEMENTS:  return &vpMeshElementItems;
  case SHWX::MESHPOINTS:    return &vpMeshPointQuasiItems;
  case SHW::NODES:
  case SHW::IRRNODES:       return &vpNodeItems;
  case SHW::GRIDPOINTS:     return &vpGridPointExItems;
  case SHW::COMMENTS:       return &commentItems;
  default: return null;
  }
}

ViewItemsList* ViewScene::SelectList( ObjectType ot )
{
  switch( ot ) {
  case OT::ELEMENT:       return &vpElementItems;
  case OT::SURFACEEX:     return &vpSurfaceExItems;
  case OT::SEPARATOR:     return &vpSeparatorItems;
  case OT::SOURCE:        return &vpSourceItems;
  case OT::CHORD:         return &vpChordItems;
  case OT::XPOINTTEST:    return &vpXPointTestItems;
  case OT::XPOINTSEG:     return &vpXPointSegItems;
  case OT::MESHCELL:      return &vpMeshCellItems;
  case OT::MESHELEMENT:   return &vpMeshElementItems;
  case OT::MESHPOINT:     return &vpMeshPointQuasiItems;
  case OT::NODE:          return &vpNodeItems;
  case OT::GRIDPOINTEX:   return &vpGridPointExItems;
  case OT::GRIDPOINTSEG:  return &vpGridPointSegItems;
  case OT::COMMENT:       return &commentItems;
  default: return null;
  }
}

IViewItemPtr ViewScene::SelectSingleItem( ObjectType ot )
{
  switch( ot ) {
  case OT::TEMPLATE:  return pTemplateItem;
  case OT::EQUIL:     return pEquilItem;
  case OT::SONNET:    return pSonnetDataItem;
  default:            return null;
  }
  return null;
}

void ViewScene::Clear()
{
  IViewScene::Clear();

  viewItemsWithText.clear();

  pEquilItem = null;
  pTemplateItem = null;
  pSonnetDataItem = null;

  vpNodeItems.clear();
  vpElementItems.clear();
  vpSeparatorItems.clear();
  vpSourceItems.clear();
  vpChordItems.clear();

  vpSurfaceExItems.clear();
  vpXPointTestItems.clear();
  vpXPointSegItems.clear();
  vpGridPointExItems.clear();
  vpGridPointSegItems.clear();

  vpMeshPointQuasiItems.clear();
  vpMeshElementItems.clear();
  vpMeshCellItems.clear();
}

void ViewScene::Build( ulong sfs )
{
  if( sfs == SHW::ALL )
    Clear();

  if( pModel == NULL )
    return;

  EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();

  if( HasAnyFlag( sfs, SHW::EQUIL ) && pEquil != null ) {
    if( pEquilItem != null ) {
      viewItems.removeOne( pEquilItem );
      this->removeItem( pEquilItem );
      delete pEquilItem;
    }

    pEquilItem = new ViewEquilItem( pEquil, NPoint( 40, 40 ) );
    pEquilItem->SetSettingsManager( pPrefs );
    pEquilItem->SetViewState( pState );
    viewItems.push_back( pEquilItem );
    this->addItem( pEquilItem );

    if( pLegendItem != null ) {
      viewItems.removeOne( pLegendItem );
      this->removeItem( pLegendItem );
      delete pLegendItem;
    }

    pLegendItem = new ViewLegendItem( pEquil );
    pLegendItem->SetSettingsManager( pPrefs );
    viewItems.push_back( pLegendItem );
    this->addItem( pLegendItem );
    pLegendItem->SetViewState( pState ); // Unusual order to make scene accessable
  }

  if( HasAnyFlag( sfs, SHW::TEMPLATE ) && pModel->HasTemplate() ) {
    if( pTemplateItem != null ) {
      viewItems.removeOne( pTemplateItem );
      this->removeItem( pTemplateItem );
      delete pTemplateItem;
    }

    pTemplateItem = new ViewTemplateItem( pModel->GetTemplate() );
    pTemplateItem->SetSettingsManager( pPrefs );
    pTemplateItem->SetViewState( pState );
    viewItems.push_back( pTemplateItem );
    this->addItem( pTemplateItem );
  }

  if( pEquil != null ) {
    FluxModelPtr pFlux = pModel->GetFluxModel();
    TopologyPtr pTopology = pFlux->GetTopology();

    if( HasAnyFlag( sfs, SHW::XPOINTTESTS ) ) {
      if( !vpXPointTestItems.empty() ) {
        foreach( IViewItemPtr pItem, vpXPointTestItems ) {
          viewItems.removeOne( pItem );
          ViewXPointTestItemPtr pViewItem = dgtype_cast< ViewXPointTestItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpXPointTestItems.clear();
      }

      IComponentPtr FOREACHOBJCONST( pObject, pTopology->XPointTests() ) {
        ViewXPointTestItemPtr pItem = new ViewXPointTestItem( pObject, pEquil );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpXPointTestItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }
    }

    if( HasAnyFlag( sfs, SHW::GRIDPOINTS | SHW::SURFACES ) ) {
      if( !vpXPointSegItems.empty() ) {
        foreach( IViewItemPtr pItem, vpXPointSegItems ) {
          viewItems.removeOne( pItem );
          ViewXPointSegItemPtr pViewItem = dgtype_cast< ViewXPointSegItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpXPointSegItems.clear();
      }

      IComponentPtr FOREACHOBJCONST( pObject, pTopology->XPointSegs() ) {
        ViewXPointSegItemPtr pItem = new ViewXPointSegItem( pObject, pTopology );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpXPointSegItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }
    }

    if( HasAnyFlag( sfs, SHW::GRIDPOINTS ) ) {
      if( !vpGridPointSegItems.empty() ) {
        foreach( IViewItemPtr pItem, vpGridPointSegItems ) {
          viewItems.removeOne( pItem );
          ViewGridPointSegItemPtr pViewItem = dgtype_cast< ViewGridPointSegItemPtr >( pItem );
          this->removeItem( pViewItem );
          viewItemsWithText.removeOne( pItem );
          delete pItem;
        }
        vpGridPointSegItems.clear();
      }

      IComponentPtr FOREACHOBJCONST( pObject, pTopology->GridPointSegs() ) {
        ViewGridPointSegItemPtr pItem = new ViewGridPointSegItem( pObject );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpGridPointSegItems.push_back( pItem );
        viewItems.push_back( pItem );
        viewItemsWithText.push_back( pItem );
        this->addItem( pItem );
      }
    }

    //pEquil->Object_Reset( OT::SURFACEZONE );
    //while( !pEquil->Object_End() ); // no any drawing

    if( HasAnyFlag( sfs, SHW::SURFACES ) ) {
      if( !vpSurfaceExItems.empty() ) {
        foreach( IViewItemPtr pItem, vpSurfaceExItems ) {
          viewItems.removeOne( pItem );
          ViewSurfaceExItemPtr pViewItem = dgtype_cast< ViewSurfaceExItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpSurfaceExItems.clear();
      }

      IComponentPtr FOREACHOBJCONST( pObject, pFlux->SurfacesEx() ) {
        ViewSurfaceExItemPtr pItem = new ViewSurfaceExItem( pObject );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpSurfaceExItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }
    }

    if( HasAnyFlag( sfs, SHW::GRIDPOINTS ) ) {
      if( !vpGridPointExItems.empty() ) {
        foreach( IViewItemPtr pItem, vpGridPointExItems ) {
          viewItems.removeOne( pItem );
          ViewGridPointExItemPtr pViewItem = dgtype_cast< ViewGridPointExItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpGridPointExItems.clear();
      }

      IComponentPtr FOREACHOBJCONST( pObject, pFlux->GridPointsEx() ) {
        ViewGridPointExItemPtr pItem = new ViewGridPointExItem( pObject );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpGridPointExItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }
    }
  }

  if( pModel->HasMesh() ) {
    MeshPtr pMesh = pModel->GetMesh();
    if( HasAnyFlag( sfs, SHW::MESH | SHW::MESHDETAILS ) ) {
      if( !vpMeshCellItems.empty() ) {
        foreach( IViewItemPtr pItem, vpMeshCellItems ) {
          viewItems.removeOne( pItem );
          ViewMeshCellItemPtr pViewItem = dgtype_cast< ViewMeshCellItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpMeshCellItems.clear();
      }

      FOREACHPTRCONST( MeshCellPtr, pMC, pMesh->Cells() ) {
        ViewMeshCellItemPtr pItem = new ViewMeshCellItem( pMC );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpMeshCellItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }
    }
    if( HasAnyFlag( sfs, SHW::MESH ) ) {
      if( !vpMeshElementItems.empty() ) {
        foreach( IViewItemPtr pItem, vpMeshElementItems ) {
          viewItems.removeOne( pItem );
          ViewMeshElementItemPtr pViewItem = dgtype_cast< ViewMeshElementItemPtr >( pItem );
          this->removeItem( pViewItem );
          delete pItem;
        }
        vpMeshElementItems.clear();
      }

      FOREACHPTRCONST( MeshElementPtr, pME, pMesh->Elements() ) {
        ViewMeshElementItemPtr pItem = new ViewMeshElementItem( pME );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpMeshElementItems.push_back( pItem );
        viewItems.push_back( pItem );
        this->addItem( pItem );
      }

      if( !vpMeshPointQuasiItems.empty() ) {
        foreach( IViewItemPtr pItem, vpMeshPointQuasiItems ) {
          viewItems.removeOne( pItem );
          //ViewMeshPointQuasiItemPtr pViewItem = dgtype_cast< ViewMeshPointQuasiItemPtr >( pItem );
          //this->removeItem( pViewItem ); // not a QGraphicsItem
          delete pItem;
        }
        vpMeshPointQuasiItems.clear();
      }

      FOREACHPTRCONST( MeshPointPtr, pMP, pMesh->Points() ) {
        ViewMeshPointQuasiItemPtr pItem = new ViewMeshPointQuasiItem( pMP );
        pItem->SetSettingsManager( pPrefs );
        pItem->SetViewState( pState );
        vpMeshPointQuasiItems.push_back( pItem );
        viewItems.push_back( pItem );
        //this->addItem( pItem ); // not a QGraphicsItem
      }
    }
  }

  if( HasAnyFlag( sfs, SHW::MESH ) && pModel->HasSonnetData() ) {
    if( pSonnetDataItem != null ) {
      viewItems.removeOne( pSonnetDataItem );
      this->removeItem( pSonnetDataItem );
      delete pSonnetDataItem;
    }

    pSonnetDataItem = new ViewSonnetDataItem( pModel->GetSonnetData() );
    pSonnetDataItem->SetSettingsManager( pPrefs );
    pSonnetDataItem->SetViewState( pState );
    viewItems.push_back( pSonnetDataItem );
    this->addItem( pSonnetDataItem );
  }

  if( HasAnyFlag( sfs, SHW::ELEMS ) ) {
    if( !vpElementItems.empty() ) {
      foreach( IViewItemPtr pItem, vpElementItems ) {
        viewItems.removeOne( pItem );
        ViewElementItemPtr pViewItem = dgtype_cast< ViewElementItemPtr >( pItem );
        this->removeItem( pViewItem );
        viewItemsWithText.removeOne( pItem );
        delete pItem;
      }
      vpElementItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Struct()->Elements() ) {
      ViewElementItemPtr pItem = new ViewElementItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpElementItems.push_back( pItem );
      viewItems.push_back( pItem );
      viewItemsWithText.push_back( pItem );
      this->addItem( pItem );
    }
  }

  if( HasAnyFlag( sfs, SHW::SEPARATORS ) ) {
    if( !vpSeparatorItems.empty() ) {
      foreach( IViewItemPtr pItem, vpSeparatorItems ) {
        viewItems.removeOne( pItem );
        ViewSeparatorItemPtr pViewItem = dgtype_cast< ViewSeparatorItemPtr >( pItem );
        this->removeItem( pViewItem );
        viewItemsWithText.removeOne( pItem );
        delete pItem;
      }
      vpSeparatorItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Struct()->Separators() ) {
      ViewSeparatorItemPtr pItem = new ViewSeparatorItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpSeparatorItems.push_back( pItem );
      viewItems.push_back( pItem );
      viewItemsWithText.push_back( pItem );
      this->addItem( pItem );
    }
  }

  if( HasAnyFlag( sfs, SHW::SOURCES ) ) {
    if( !vpSourceItems.empty() ) {
      foreach( IViewItemPtr pItem, vpSourceItems ) {
        viewItems.removeOne( pItem );
        ViewSourceItemPtr pViewItem = dgtype_cast< ViewSourceItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpSourceItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Sources() ) {
      ViewSourceItemPtr pItem = new ViewSourceItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpSourceItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );
    }
  }

  if( HasAnyFlag( sfs, SHW::NODES ) ) {
    if( !vpNodeItems.empty() ) {
      foreach( IViewItemPtr pItem, vpNodeItems ) {
        viewItems.removeOne( pItem );
        ViewNodeItemPtr pViewItem = dgtype_cast< ViewNodeItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpNodeItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Struct()->Nodes() ) {
      ViewNodeItemPtr pItem = new ViewNodeItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpNodeItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );
    }
  }

  if( HasAnyFlag( sfs, SHW::CHORDS | SHW::CHORDS3D ) ) {
    if( !vpChordItems.empty() ) {
      foreach( IViewItemPtr pItem, vpChordItems ) {
        viewItems.removeOne( pItem );
        ViewChordItemPtr pViewItem = dgtype_cast< ViewChordItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpChordItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Struct()->Chords() ) {
      ViewChordItemPtr pItem = new ViewChordItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpChordItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );
    }
  }

  IViewScene::Build();
}

IViewItemPtr ViewScene::CreateItem( IComponentPtr _pObject )
{
  if( _pObject == null )
    return null;

  IViewItemPtr pItem = null;
  switch( _pObject->Type() ) {
  case OT::NODE: {
    ViewNodeItemPtr pNodeItem = new ViewNodeItem( _pObject );
    pNodeItem->SetFlag( IViewItem::p_showIrregular, HasAnyFlag( showFlags, SHW::IRRNODES ) );
    pNodeItem->SetFlag( IViewItem::p_showAll, HasAnyFlag( showFlags, SHW::NODES ) );
    pItem = pNodeItem;
    vpNodeItems.push_back( pItem );
    this->addItem( pNodeItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::NODES ) );
    break;
  }
  case OT::ELEMENT: {
    ViewElementItemPtr pElementItem = new ViewElementItem( _pObject );
    pElementItem->SetFlag( IViewItem::p_showNormals, HasAnyFlag( showFlags, SHW::NORMALS ) );
    pElementItem->SetFlag( IViewItem::p_showNumbers, HasAnyFlag( showFlags, SHW::NUMBERS ) );
    pItem = pElementItem;
    vpElementItems.push_back( pItem );
    viewItemsWithText.push_back( pItem );
    this->addItem( pElementItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::ELEMS ) );
    break;
  }
  case OT::CHORD: {
    ViewChordItemPtr pChordItem = new ViewChordItem( _pObject );
    pChordItem->SetFlag( IViewItem::p_showXProjection, HasAnyFlag( showFlags, SHW::CHORDS ) );
    pChordItem->SetFlag( IViewItem::p_showZProjection, HasAnyFlag( showFlags, SHW::CHORDS3D ) );
    pItem = pChordItem;
    vpChordItems.push_back( pItem );
    this->addItem( pChordItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::CHORDS ) );
    break;
  }
  case OT::SOURCE: {
    ViewSourceItemPtr pSourceItem = new ViewSourceItem( _pObject );
    pItem = pSourceItem;
    vpSourceItems.push_back( pItem );
    this->addItem( pSourceItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::SOURCES ) );
    break;
  }
  case OT::GRIDPOINTEX: {
    ViewGridPointExItemPtr pGPXItem = new ViewGridPointExItem( _pObject );
    pItem = pGPXItem;
    vpGridPointExItems.push_back( pItem );
    this->addItem( pGPXItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::GRIDPOINTS ) );
    break;
  }
  case OT::SURFACEEX: {
    ViewSurfaceExItemPtr pSXItem = new ViewSurfaceExItem( _pObject );
    pItem = pSXItem;
    vpSurfaceExItems.push_back( pItem );
    this->addItem( pSXItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::SURFACES ) );
    break;
  }
  case OT::XPOINTTEST: {
    ViewXPointTestItemPtr pXPTItem = new ViewXPointTestItem( _pObject, pModel->GetFluxModel()->GetEquil() );
    pItem = pXPTItem;
    vpXPointTestItems.push_back( pItem );
    this->addItem( pXPTItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::XPOINTTESTS ) );
    break;
  }
  case OT::XPOINTSEG: {
    ViewXPointSegItemPtr pXPSItem = new ViewXPointSegItem( _pObject, pModel->GetFluxModel()->GetTopology() );
    pItem = pXPSItem;
    vpXPointSegItems.push_back( pItem );
    this->addItem( pXPSItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::XPOINTSEGS ) );
    break;
  }
  case OT::GRIDPOINTSEG: {
    ViewGridPointSegItemPtr pGPSItem = new ViewGridPointSegItem( _pObject );
    pItem = pGPSItem;
    vpGridPointSegItems.push_back( pItem );
    this->addItem( pGPSItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::GRIDPOINTS ) );
    break;
  }
  case OT::COMMENT: {
    CommentPtr pComment = dgtype_cast< CommentPtr >( _pObject );
    if( pComment->ParentScene() != (int)sceneType )
      break;
    IViewItemPtr pLinkedItem = (pComment->ObjType() == OT::UNKNOWN) ? null :
      HitItemByType( OT2SHW( pComment->ObjType() ), _C( pComment->PosEnd() ) );
    ViewCommentItemPtr pCommentItem = new ViewCommentItem( _pObject,
      pLinkedItem == null ? null : pLinkedItem->ModelObject() );
    if( pLinkedItem != null )
      pLinkedItem->IncludeComment( pCommentItem );
    pItem = pCommentItem;
    commentItems.push_back( pItem );
    this->addItem( pCommentItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::COMMENTS ) );
    break;
  }
  case OT::MESH: {
    Build( SHW::MESH | SHW::MESHDETAILS );
    ulong showFlagsTmp = showFlags;
    showFlags ^= (SHW::MESH | SHW::MESHDETAILS);
    SetVisibility( showFlagsTmp );
    break;
  }
  case OT::EQUIL: {
    Build( SHW::EQUIL );
    ulong showFlagsTmp = showFlags;
    showFlags ^= SHW::EQUIL;
    SetVisibility( showFlagsTmp );
    break;
  }
  case OT::TEMPLATE: {
    Build( SHW::TEMPLATE );
    ulong showFlagsTmp = showFlags;
    showFlags ^= SHW::TEMPLATE;
    SetVisibility( showFlagsTmp );
    break;
  }
  default:
    break;
  }

  if( pItem != null ) {
    viewItems.push_back( pItem );
    pItem->SetSettingsManager( pPrefs );
    pItem->SetViewState( pState );
  }
  return pItem;
}

IViewItemPtr ViewScene::HitViewItem( QPointF position, ulong hitFlags, double radius )
{
  hitFlags &= showFlags; // Exclude hidden items

  double radius_pnt = (radius == 0.) ?
                        (pPrefs->visual.meshPointRadius / pState->scale.x):
                        radius;

  double dist = 0., hitDist = 0.;
  IViewItemPtr pItem = null, hitItem = null;
  EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();

  if( pEquil != null && HasAnyFlag( hitFlags, SHW::XPOINTTESTS ) ) {
    pItem = HitItemByType( SHW::XPOINTTESTS, position, &dist );
    if( pItem != null ) {
      XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pItem->ModelObject() );
      double xdist = fabs( pEquil->GetPoint( pXPT->EndPos(2) ).x - pEquil->GetPoint( pXPT->EndPos(1) ).x );
      if( pItem != null &&
          (hitItem == null || dist <= hitDist || dist <= xdist) &&
          (radius_pnt == 0. || dist <= radius_pnt) ) {
        hitDist = dist;
        hitItem = pItem;
      }
    }
  }

  ulong flags_pnt[] = { SHWX::MESHPOINTS, SHW::NODES, SHW::IRRNODES,
                        SHW::GRIDPOINTS, SHW::SOURCES };
  ulong flags_size = sizeof( flags_pnt ) / sizeof( ulong );
  for( ulong c = 0; c < flags_size; c++ ) {
    if( HasAnyFlag( hitFlags, flags_pnt[c] ) ) {
      dist = 0.;
      pItem = HitItemByType( flags_pnt[c], position, &dist, radius_pnt );
      if( pItem != null && (hitItem == NULL || dist <= hitDist ) ) {
        hitDist = dist;
        hitItem = pItem;
      }
    }
  }

  if( hitItem != null )
    return hitItem;

  ulong flags_vct[] = { SHW::ELEMS, SHW::SURFACES, SHW::SEPARATORS,
                        SHW::CHORDS /*SHW::CHORDS3D*/, SHW::XPOINTSEGS,
                        SHWX::MESHELEMENTS, SHWX::MESHCELLS, SHW::COMMENTS };
  flags_size = sizeof( flags_vct ) / sizeof( ulong );

  for( ulong c = 0; c < flags_size; c++ ) {
    if( HasAnyFlag( hitFlags, flags_vct[c] ) ) {
      dist = 0.;
      pItem = HitItemByType( flags_vct[c], position, &dist, radius );
      if( pItem != null && (hitItem == NULL || dist <= hitDist ) ) {
        hitDist = dist;
        hitItem = pItem;
      }
    }
  }
  return hitItem;
}

void ViewScene::UpdateVisibility( ShowFlag sf, bool enable )
{
  if( !UpdateShowFlag( sf, enable ) )
    return;

  switch( sf ) {
  case SHW::ELEMS:
    foreach( IViewItem* pItem, vpElementItems )
      pItem->SetVisible( enable );
    break;
  case SHW::NORMALS:
    foreach( IViewItem* pItem, vpElementItems )
      pItem->SetFlag( ViewElementItem::p_showNormals, enable );
    foreach( IViewItem* pItem, vpChordItems )
      pItem->SetFlag( ViewChordItem::p_showNormals, enable );
    break;
  case SHW::NUMBERS:
    foreach( IViewItem* pItem, vpElementItems )
      pItem->SetFlag( ViewElementItem::p_showNumbers, enable );
    foreach( IViewItem* pItem, vpSeparatorItems )
      pItem->SetFlag( ViewSeparatorItem::p_showNumbers, enable );
    break;
  case SHW::NODES:
    foreach( IViewItem* pItem, vpNodeItems ) {
      pItem->SetFlag( ViewNodeItem::p_showAll, enable );
      pItem->UpdateStyle();
    }
    break;
  case SHW::IRRNODES:
    foreach( IViewItem* pItem, vpNodeItems ) {
      pItem->SetFlag( ViewNodeItem::p_showIrregular, enable );
      pItem->UpdateStyle();
    }
    break;
  case SHW::TEMPLATE:
    if( (pModel->HasTemplate() || !enable) && pTemplateItem != null )
      pTemplateItem->setVisible( enable );
    break;
  case SHW::EQUIL:
    if( (pModel->GetFluxModel()->HasEquil() || !enable) && pEquilItem != null ) {
      pEquilItem->setVisible( enable );
      pLegendItem->setVisible( enable && pState->coloredEquil );
    }
    break;
  case SHW::SURFACES:
    if( pModel->GetFluxModel()->HasEquil() || !enable ) {
      // Change XPS visibility only if SHW::GRIDPOINTS disabled
      if( !HasAnyFlag( showFlags, SHW::GRIDPOINTS ) )
        foreach( IViewItem* pItem, vpXPointSegItems ) {
          pItem->SetVisible( enable );
          pItem->UpdateStyle();
        }
      foreach( IViewItem* pItem, vpSurfaceExItems )
        pItem->SetVisible( enable );
    }
    break;
  case SHW::GRIDPOINTS:
    if( pModel->GetFluxModel()->HasEquil() || !enable ) {
      // Change XPS visibility only if SHW::SURFACES disabled
      if( !HasAnyFlag( showFlags, SHW::SURFACES ) )
        foreach( IViewItem* pItem, vpXPointSegItems ) {
          pItem->SetVisible( enable );
          pItem->UpdateStyle();
        }
      foreach( IViewItem* pItem, vpGridPointExItems )
        pItem->SetVisible( enable );
      foreach( IViewItem* pItem, vpGridPointSegItems )
        pItem->SetVisible( enable );
    }
    break;
  case SHW::LABELS:
    foreach( ViewLabelItemPtr pItem, labelItems )
      pItem->SetVisible( enable );
    break;
  case SHW::MESH:
    if( pModel->HasMesh() || !enable ) {
      SETFLAG( showFlags, SHWX::MESHELEMENTS, enable );
      SETFLAG( showFlags, SHWX::MESHPOINTS, enable );
      /* Cells can be hidden independent of SHW::MESHDETAILS
       * but shown if SHW::MESHDETAILS enabled only */
      if( HasAnyFlag( showFlags, SHW::MESHDETAILS ) || !enable ) {
        SETFLAG( showFlags, SHWX::MESHCELLS, enable );
        //SETFLAG( showFlags, SHWX::MESHPOINTS, enable );
        SETFLAG( showFlags, SHW::MESHDETAILS, enable );
        foreach( IViewItem* pItem, vpMeshCellItems )
          pItem->SetVisible( enable );
      }
      foreach( IViewItem* pItem, vpMeshElementItems )
        pItem->SetVisible( enable );
    }
    if( (pModel->HasSonnetData() || !enable) && pSonnetDataItem != null )
      pSonnetDataItem->setVisible( enable );
    break;
  case SHW::MESHDETAILS:
    if( pModel->HasMesh() || !enable ) {
      /* Cells can be hidden independent of SHW::MESHDETAILS
       * but shown if SHW::MESHDETAILS enabled only */
      if( HasAnyFlag( showFlags, SHW::MESH ) || !enable ) {
        SETFLAG( showFlags, SHWX::MESHCELLS, enable );
        //SETFLAG( showFlags, SHWX::MESHPOINTS, enable );
        SETFLAG( showFlags, SHW::MESHDETAILS, enable );
        foreach( IViewItem* pItem, vpMeshCellItems )
          pItem->SetVisible( enable );
      }
    }
    break;
  case SHW::SEPARATORS:
    foreach( IViewItem* pItem, vpSeparatorItems )
      pItem->SetVisible( enable );
    break;
  case SHW::SOURCES:
    foreach( IViewItem* pItem, vpSourceItems )
      pItem->SetVisible( enable );
    break;
  case SHW::CHORDS:
    foreach( IViewItem* pItem, vpChordItems ) {
      pItem->SetFlag( ViewChordItem::p_showXProjection, enable );
      pItem->UpdateStyle();
    }
    break;
  case SHW::CHORDS3D:
    foreach( IViewItem* pItem, vpChordItems ) {
      pItem->SetFlag( ViewChordItem::p_showZProjection, enable );
      pItem->UpdateStyle();
    }
    break;
  case SHW::XPOINTTESTS:
    if( pModel->GetFluxModel()->HasEquil() || !enable ) {
      foreach( IViewItem* pItem, vpXPointTestItems )
        pItem->SetVisible( enable );
    }
    break;
  default:
    break;
  }
}


ViewItemsList ViewScene::ItemsInRectangle( ulong flags, const QRect& _rect, const QTransform& _transform ) const
{
  ulong mask = showFlags;
  if( HasAnyFlag( mask, SHW::MESH ) )
    mask |= SHWX::MESHCELLS|SHWX::MESHELEMENTS|SHWX::MESHPOINTS;
  if( HasAnyFlag( flags, SHW::NODES ) )
    flags |= SHW::IRRNODES;
  flags &= mask;

  ViewItemsList result_list;

#ifndef INRECT
#define INRECT( _flag, _list )                          \
  if( HasAnyFlag( flags, (_flag) ) ) {                  \
    foreach( IViewItemPtr pItem, (_list) ) {            \
      try {                                             \
        if( pItem->InRectangle( _rect, _transform ) )   \
          result_list.append( pItem );                  \
      }                                                 \
      catch( int ex ) {                                 \
        continue;                                       \
      }                                                 \
    }                                                   \
  }
#endif

  INRECT( SHW::ELEMS,         vpElementItems );
  INRECT( SHW::SOURCES,       vpSourceItems );
  INRECT( SHW::CHORDS,        vpChordItems );
  INRECT( SHW::SEPARATORS,    vpSeparatorItems );
  INRECT( SHWX::MESHCELLS,    vpMeshCellItems );
  INRECT( SHWX::MESHELEMENTS, vpMeshElementItems );

#undef INRECT

  return result_list;
}

QGraphicsItem* ViewScene::GetGraphicsItem( IViewItemPtr _pItem ) const
{
  if( _pItem == null || _pItem->ModelObject() == null )
    return null;
  switch( _pItem->ModelObject()->Type() ) {
  case OT::ELEMENT:       return static_cast< ViewElementItemPtr >( _pItem );
  case OT::SURFACEEX:     return static_cast< ViewSurfaceExItemPtr >( _pItem );
  case OT::SEPARATOR:     return static_cast< ViewSeparatorItemPtr >( _pItem );
  case OT::SOURCE:        return static_cast< ViewSourceItemPtr >( _pItem );
  case OT::CHORD:         return static_cast< ViewChordItemPtr >( _pItem );
  case OT::XPOINTTEST:    return static_cast< ViewXPointTestItemPtr >( _pItem );
  case OT::XPOINTSEG:     return static_cast< ViewXPointSegItemPtr >( _pItem );
  case OT::MESHCELL:      return static_cast< ViewMeshCellItemPtr >( _pItem );
  case OT::MESHELEMENT:   return static_cast< ViewMeshElementItemPtr >( _pItem );
  case OT::NODE:          return static_cast< ViewNodeItemPtr >( _pItem );
  case OT::GRIDPOINTEX:   return static_cast< ViewGridPointExItemPtr >( _pItem );
  case OT::GRIDPOINTSEG:  return static_cast< ViewGridPointSegItemPtr >( _pItem );
  case OT::COMMENT:       return static_cast< ViewCommentItemPtr >( _pItem );
  default:
    return null;
  }
}

void ViewScene::RemoveItem( IViewItemPtr _pItem )
{
  if( _pItem == null || _pItem->ModelObject() == null )
    return;

  ViewItemsList* pList = SelectList( _pItem->ModelObject()->Type() );
  if( pList != null )
    pList->removeOne( _pItem );
  else if( _pItem == pEquilItem )
    pEquilItem = null;
  else if( _pItem == pTemplateItem )
    pTemplateItem = null;
  else if( _pItem == pSonnetDataItem )
    pSonnetDataItem = null;
  else return;

  viewItemsWithText.removeOne( _pItem );
  viewItems.removeOne( _pItem );

  QGraphicsItem* pQItem = GetGraphicsItem( _pItem );
  if( pQItem != null )
    this->removeItem( pQItem );
}

bool ViewScene::RemoveMultiItemObject( IComponentPtr _pObject )
{
  if( _pObject->Type() == OT::MESH ) {
    foreach( IViewItemPtr pItem, vpMeshCellItems ) {
      ViewMeshCellItemPtr pCellItem = static_cast< ViewMeshCellItemPtr >( pItem );
      viewItemsWithText.removeOne( pCellItem );
      viewItems.removeOne( pCellItem );
      this->removeItem( pCellItem );
      delete pItem;
    }
    vpMeshCellItems.clear();

    foreach( IViewItemPtr pItem, vpMeshElementItems ) {
      ViewMeshElementItemPtr pElementItem = static_cast< ViewMeshElementItemPtr >( pItem );
      viewItemsWithText.removeOne( pElementItem );
      viewItems.removeOne( pElementItem );
      this->removeItem( pElementItem );
      delete pItem;
    }
    vpMeshElementItems.clear();

    foreach( IViewItemPtr pItem, vpMeshPointQuasiItems ) {
      ViewMeshPointQuasiItemPtr pPointItem = static_cast< ViewMeshPointQuasiItemPtr >( pItem );
      viewItemsWithText.removeOne( pPointItem );
      viewItems.removeOne( pPointItem );
      //this->removeItem( pPointItem ); // Not a QGraphicsItem
      delete pItem;
    }
    vpMeshPointQuasiItems.clear();
    return true;
  }
  return false;
}
