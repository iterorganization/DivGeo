#include "ViewTopScene.h"

ViewTopScene::ViewTopScene(ModelPtr _pModel, const QString &_name, SceneType _sceneType,
                           const UserPreferences* _pPrefs, QObject *parent ):
  IViewScene( _pModel, _name, _sceneType, _pPrefs, parent )
{
  pRadialObjects = null;
  VarPtr pV = pModel->Vars()->GetVarPtrByType( VT::TOPVIEW );
  if( pV != null ) {
    UPtr val = pV->Value();
    if( !val.IsNull() )
      pRadialObjects = val.ListPtr();
  }

  Build();
}

ViewItemsList* ViewTopScene::SelectList( ulong sf )
{
  switch( sf ) {
  case SHW::SOURCES:  return &vpSourceRadialItems;
  case SHW::CHORDS:   return &vpChordItems;
  //case SHW::CHORDS:   return &vpChordRadialItems; //TODO:
  case SHW::ELEMS:    return &vpElementRadialItems;
  case SHW::COMMENTS: return &commentItems;
  default: return null;
  }
}

ViewItemsList* ViewTopScene::SelectList( ObjectType ot )
{
  switch( ot ) {
  case OT::ELEMENT:     return &vpElementRadialItems;
  case OT::SOURCE:      return &vpSourceRadialItems;
  //case OT::CHORD:       return &vpChordRadialItems; //TODO:
  case OT::CHORD:       return &vpChordItems;
  case OT::COMMENT:     return &commentItems;
  default: return null;
  }
}

IViewItemPtr ViewTopScene::SelectSingleItem( ObjectType ot )
{
  switch( ot ) {
  case OT::MESH:  return pMeshRadialItem;
  default:        return null;
  }
  return null;
}

void ViewTopScene::Clear()
{
  IViewScene::Clear();

  vpElementRadialItems.clear();
  vpSourceRadialItems.clear();
  vpChordRadialItems.clear();
  pMeshRadialItem = null;
  vpChordItems.clear();
}

void ViewTopScene::Build( ulong sfs )
{
  if( sfs == SHW::ALL )
    Clear();

  if( pModel == NULL )
    return;

  if( HasAnyFlag( sfs, SHW::SOURCES ) ) {
    if( !vpSourceRadialItems.empty() ) {
      foreach( IViewItemPtr pItem, vpSourceRadialItems ) {
        viewItems.removeOne( pItem );
        ViewSourceRadialTopItemPtr pViewItem = dgtype_cast< ViewSourceRadialTopItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpSourceRadialItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Sources() ) {
      ViewSourceRadialTopItemPtr pItem = new ViewSourceRadialTopItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpSourceRadialItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );

      IVarItemPtr pVarItem = dgtype_cast< SourcePtr >( pObject );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem ) );
    }
  }

  if( HasAnyFlag( sfs, SHW::CHORDS | SHW::CHORDS3D ) ) {

    // Radial
    if( !vpChordRadialItems.empty() ) {
      foreach( IViewItemPtr pItem, vpChordRadialItems ) {
        viewItems.removeOne( pItem );
        ViewChordRadialTopItemPtr pViewItem = dgtype_cast< ViewChordRadialTopItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpChordRadialItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Chords() ) {
      ViewChordRadialTopItemPtr pItem = new ViewChordRadialTopItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpChordRadialItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );

      IVarItemPtr pVarItem = dgtype_cast< ChordPtr >( pObject );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem ) );
    }

    // Plain
    if( !vpChordItems.empty() ) {
      foreach( IViewItemPtr pItem, vpChordItems ) {
        viewItems.removeOne( pItem );
        ViewChordTopItemPtr pViewItem = dgtype_cast< ViewChordTopItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpChordItems.clear();
    }

    FOREACHOBJCONST( pObject, pModel->Chords() ) {
      ViewChordTopItemPtr pItem = new ViewChordTopItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpChordItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );
    }
  }

  if( HasAnyFlag( sfs, SHW::ELEMS ) ) {
    if( !vpElementRadialItems.empty() ) {
      foreach( IViewItemPtr pItem, vpElementRadialItems ) {
        viewItems.removeOne( pItem );
        ViewElementRadialTopItemPtr pViewItem = dgtype_cast< ViewElementRadialTopItemPtr >( pItem );
        this->removeItem( pViewItem );
        delete pItem;
      }
      vpElementRadialItems.clear();
    }

    IComponentPtr FOREACHOBJCONST( pObject, pModel->Elements() ) {
      ViewElementRadialTopItemPtr pItem = new ViewElementRadialTopItem( pObject );
      pItem->SetSettingsManager( pPrefs );
      pItem->SetViewState( pState );
      vpElementRadialItems.push_back( pItem );
      viewItems.push_back( pItem );
      this->addItem( pItem );

      IVarItemPtr pVarItem = dgtype_cast< ElementPtr >( pObject );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem ) );
    }
  }

  if( HasAnyFlag( sfs, SHW::MESH ) && pModel->HasMesh() ) {
    if( pMeshRadialItem != null ) {
      viewItems.removeOne( pMeshRadialItem );
      this->removeItem( pMeshRadialItem );
      delete pMeshRadialItem;
    }

    MeshPtr pMesh = pModel->GetMesh();
    pMeshRadialItem = new ViewMeshRadialTopItem( pMesh );
    pMeshRadialItem->SetSettingsManager( pPrefs );
    pMeshRadialItem->SetViewState( pState );
    viewItems.push_back( pMeshRadialItem );
    this->addItem( pMeshRadialItem );

    bool top = false;
    if( pRadialObjects != null ) {
      FOREACHPTRCONST( MeshCellPtr, pMC, pMesh->Cells() ) {
        if( Contains( *pRadialObjects, (IVarItemPtr)pMC ) ) {
          top = true;
          break;
        }
      }
    }
    pMeshRadialItem->SetVisible( pRadialObjects != null && top );
  }

  IViewScene::Build();
}

IViewItemPtr ViewTopScene::CreateItem( IComponentPtr _pObject )
{
  IViewItemPtr pItem = null;
  switch( _pObject->Type() ) {
  case OT::ELEMENT: {
    ViewElementRadialTopItemPtr pElementItem = new ViewElementRadialTopItem( _pObject );
    pItem = pElementItem;
    vpElementRadialItems.push_back( pItem );
    this->addItem( pElementItem );

    IVarItemPtr pVarItem = dgtype_cast< ElementPtr >( _pObject );
    pItem->SetVisible( (pRadialObjects != null) &&
                       Contains( *pRadialObjects, pVarItem ) &&
                       HasAnyFlag( showFlags, SHW::ELEMS ) );
    break;
  }
  case OT::CHORD: {
    // Radial
    ViewChordRadialTopItemPtr pChordRadialItem = new ViewChordRadialTopItem( _pObject );
    pItem = pChordRadialItem;
    vpChordRadialItems.push_back( pItem );
    this->addItem( pChordRadialItem );

    IVarItemPtr pVarItem = dgtype_cast< ChordPtr >( _pObject );
    pItem->SetVisible( (pRadialObjects != null) &&
                       Contains( *pRadialObjects, pVarItem ) &&
                       HasAnyFlag( showFlags, SHW::CHORDS ) );

    // Plain
    ViewChordTopItemPtr pChordItem = new ViewChordTopItem( _pObject );
    pItem = pChordItem;
    vpChordItems.push_back( pItem );
    this->addItem( pChordItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::CHORDS ) );
    break;
  }
  case OT::SOURCE: {
    ViewSourceRadialTopItemPtr pSourceItem = new ViewSourceRadialTopItem( _pObject );
    pItem = pSourceItem;
    vpSourceRadialItems.push_back( pItem );
    this->addItem( pSourceItem );

    IVarItemPtr pVarItem = dgtype_cast< SourcePtr >( _pObject );
    pItem->SetVisible( (pRadialObjects != null) &&
                       Contains( *pRadialObjects, pVarItem ) &&
                       HasAnyFlag( showFlags, SHW::SOURCES ) );
    break;
  }
  case OT::MESH: {
    pMeshRadialItem = new ViewMeshRadialTopItem( _pObject );
    pItem = pMeshRadialItem;
    this->addItem( pMeshRadialItem );

    bool top = false;
    if( pRadialObjects != null ) {
      FOREACHPTRCONST( MeshCellPtr, pMC, pModel->GetMesh()->Cells() ) {
        if( Contains( *pRadialObjects, (IVarItemPtr)pMC ) ) {
          top = true;
          break;
        }
      }
    }
    pItem->SetVisible( top && (pRadialObjects != null) &&
                       HasAnyFlag( showFlags, SHW::MESH ) );
  }
  case OT::COMMENT: {
    CommentPtr pComment = dgtype_cast< CommentPtr >( _pObject );
    if( pComment->ParentScene() != (int)sceneType )
      break;
    IViewItemPtr pLinkedItem = (pComment->ObjType() == OT::UNKNOWN) ? null :
      HitItemByType( OT2SHW( pComment->ObjType() ), _C( pComment->PosEnd() ) );
    ViewCommentItemPtr pCommentItem = new ViewCommentItem( _pObject,
             (pLinkedItem == null) ? null : pLinkedItem->ModelObject() );
    pItem = pCommentItem;
    commentItems.push_back( pItem );
    this->addItem( pCommentItem );
    pItem->SetVisible( HasAnyFlag( showFlags, SHW::COMMENTS ) );
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

IViewItemPtr ViewTopScene::HitViewItem( QPointF position, ulong hitFlags, double radius )
{
  if( HasAnyFlag( hitFlags, SHW::CHORDS ) )
    return HitItemByType( SHW::CHORDS, position, null, radius );
  else return null;
}

void ViewTopScene::UpdateVisibility( ShowFlag sf, bool enable )
{
  if( !UpdateShowFlag( sf, enable ) )
    return;

  switch( sf ) {
  case SHW::ELEMS:
    foreach( IViewItem* pItem, vpElementRadialItems ) {
      IVarItemPtr pVarItem = dgtype_cast< ElementPtr >( pItem->ModelObject() );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem )
                         && enable );
    }
    break;
  case SHW::MESH:
    if( pModel->HasMesh() ) {
      bool top = false;
      if( pRadialObjects != null ) {
        FOREACHPTRCONST( MeshCellPtr, pMC, pModel->GetMesh()->Cells() ) {
          if( Contains( *pRadialObjects, (IVarItemPtr)pMC ) ) {
            top = true;
            break;
          }
        }
      }
      pMeshRadialItem->setVisible( enable && pRadialObjects != null && top );
    }
    break;
  case SHW::SOURCES:
    foreach( IViewItem* pItem, vpSourceRadialItems ) {
      IVarItemPtr pVarItem = dgtype_cast< SourcePtr >( pItem->ModelObject() );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem )
                         && enable );
    }
    break;
  case SHW::CHORDS:
    foreach( IViewItem* pItem, vpChordRadialItems ) {
      IVarItemPtr pVarItem = dgtype_cast< ChordPtr >( pItem->ModelObject() );
      pItem->SetVisible( pRadialObjects != null && Contains( *pRadialObjects, pVarItem )
                         && enable );
    }
    foreach( IViewItem* pItem, vpChordItems )
      pItem->SetVisible( enable );
    break;
  case SHW::LABELS:
    foreach( ViewLabelItemPtr pItem, labelItems )
      pItem->SetVisible( enable );
    break;
  default:
    break;
  }
}


ViewItemsList ViewTopScene::ItemsInRectangle( ulong _flags, const QRect& _rect , const QTransform &_transform ) const
{
  UNUSED( _flags );
  UNUSED( _rect );
  UNUSED( _transform );
  return ViewItemsList();
}



QGraphicsItem* ViewTopScene::GetGraphicsItem( IViewItemPtr _pItem ) const
{
  if( _pItem == null || _pItem->ModelObject() == null )
    return null;
  switch( _pItem->ModelObject()->Type() ) {
  case OT::MESH:          return static_cast< ViewMeshRadialTopItemPtr >( _pItem );
  case OT::ELEMENT:       return static_cast< ViewElementRadialTopItemPtr >( _pItem );
  case OT::SOURCE:        return static_cast< ViewSourceRadialTopItemPtr >( _pItem );
  case OT::CHORD:
    if( _pItem->IsRadial() )
      return static_cast< ViewChordRadialTopItemPtr >( _pItem );
    else
      return static_cast< ViewChordTopItemPtr >( _pItem );
  case OT::COMMENT:       return static_cast< ViewCommentItemPtr >( _pItem );
  default:
    throw "no such type";
    return null;
  }
}

void ViewTopScene::RemoveItem( IViewItemPtr _pItem )
{
  if( _pItem == null || _pItem->ModelObject() == null )
    return;
  ObjectType ot = _pItem->ModelObject()->Type();
  ViewItemsList* pList = SelectList( ot );
  if( pList != null )
    pList->removeOne( _pItem );
  else if( _pItem == pMeshRadialItem )
    pMeshRadialItem = null;
  else
    return;

  viewItems.removeOne( _pItem );
  this->removeItem( GetGraphicsItem( _pItem ) );
}

bool ViewTopScene::RemoveMultiItemObject( IComponentPtr _pObject )
{
  UNUSED( _pObject );
  return false;
}
