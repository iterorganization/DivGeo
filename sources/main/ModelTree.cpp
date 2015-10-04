#include "ModelTree.h"

ModelTree::ModelTree( StringsManager *_pSM ):
  pSM( _pSM ),
  pModel( null )
{
  str_yes = SM_GUI( GUI::COMMON::YES );
  str_no  = SM_GUI( GUI::COMMON::NO );
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index, const QString& str )
{
  QTreeWidgetItem* pChild = pItem->child( index );
  pChild->setText( 1, str );
  pChild->setToolTip( 1, str );
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index,
                              IComponentPtr pObject, int expand_depth, int _mode )
{
  AddObjectDescr( pItem->child( index ), pObject, expand_depth-1, true, _mode );
  pItem->child( index )->setData( 1, Qt::UserRole, QVariant::fromValue( pObject ) );
  pItem->child( index )->setData( 1, Qt::UserRole + 10, QVariant::fromValue( _mode ) );
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index,
                              IVarItemPtr pObject, int expand_depth )
{
  AddObjectDescr( pItem->child( index ), pObject, expand_depth-1, true );
  pItem->child( index )->setData( 1, Qt::UserRole, QVariant::fromValue( pObject ) );
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index,
                              IVarOriginPtr pObject, int expand_depth )
{
  AddObjectDescr( pItem->child( index ), pObject, expand_depth-1, true );
  pItem->child( index )->setData( 1, Qt::UserRole, QVariant::fromValue( pObject ) );
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index,
                              const IComponentList& objects,
                              int expand_depth, ulong group_size )
{
  if( group_size == 0 || objects.size() < group_size ) {
    AddObjectList( pItem->child( index ), objects, expand_depth-1 );
    QVariant qv = QVariant::fromValue( (IComponentListPtr)&objects );
    pItem->child( index )->setData( 1, Qt::UserRole, qv );
  }
  else {
    ulong groups_num = objects.size() / group_size;
    if( objects.size() % group_size != 0 )
      groups_num++;

    QTreeWidgetItem* pChild = pItem->child( index );
    pChild->setText( 1, ToQString( (ulong)objects.size()) );
    Clear( pChild );
    for( ulong c = 0; c < groups_num; c++ ) {
      ulong min_index = c*group_size;
      ulong max_index = (c+1)*group_size-1;
      if( max_index > objects.size()-1 )
        max_index = objects.size()-1;
      AddChild( pChild, QString( "%1..%2" ).arg( min_index ).arg( max_index ) );
      QTreeWidgetItem* pChild = pItem->child( index );
      AddObjectList( pChild->child(c), objects, expand_depth-1, min_index, group_size );
      QVariant qv = QVariant::fromValue( (IComponentListPtr)&objects );
      pChild->child(c)->setData( 1, Qt::UserRole, qv );
      pChild->child(c)->setData( 1, Qt::UserRole+1, QVariant( (uint)min_index ) );
      pChild->child(c)->setData( 1, Qt::UserRole+2, QVariant( (uint)group_size ) );
    }
  }
}

void ModelTree::SetChildData( QTreeWidgetItem* pItem, int index,
                              const VarList& vars, int expand_depth )
{
  AddVarList( pItem->child( index ), vars, expand_depth-1 );
  QVariant qv = QVariant::fromValue( (VarListPtr)&vars );
  pItem->child( index )->setData( 1, Qt::UserRole, qv );
}

void ModelTree::SetChildLabel( QTreeWidgetItem* pItem, int index, const QString& str )
{
  QTreeWidgetItem* pChild = pItem->child( index );
  pChild->setText( 1, str );
  pChild->setToolTip( 1, str );
}

void ModelTree::AddChild( QTreeWidgetItem* pItem, const QString& str )
{
  QTreeWidgetItem* pChild = new QTreeWidgetItem( QStringList( str ) );
  pChild->setToolTip( 0, str );
  pItem->addChild( pChild );
}

void ModelTree::Clear( QTreeWidgetItem* pItem )
{
  for( int c = pItem->childCount(); c >= 0; c-- )
    pItem->removeChild( pItem->child( c ) );
}

void ModelTree::AddVarList( QTreeWidgetItem* pItem, const VarList& vars, int expand_depth )
{
  Clear( pItem );
  pItem->setText( 1, ToQString( (ulong)vars.size() ) );
  if( expand_depth == 0 ) { // Tmp item to make expanding handle visible
    pItem->addChild( new QTreeWidgetItem() );
    return;
  }
  FOREACH_CONST( VarIterConst, itV, vars ) {
    QString name = ToQString( (*itV)->VD()->Name() );
    QString descr = ToQString( (*itV)->VD()->Descr() );
    QTreeWidgetItem* pChild = new QTreeWidgetItem( QStringList( name ) );
    QVariant qv = QVariant::fromValue( (IComponentPtr)(*itV) );
    pChild->setData( 1, Qt::UserRole, qv );
    AddObjectDescr( pChild, (IComponentPtr)*itV, true, expand_depth-1 );
    pChild->setToolTip( 0, descr );
    pItem->addChild( pChild );
  }
}

template< typename T >
void ModelTree::AddObjectList( QTreeWidgetItem* _pItem, const std::list< T* >& _crObjects,
                               int _expand_depth, ulong _from, ulong _count )
{
  Clear( _pItem );
  _pItem->setText( 1, _count == 0 ? ToQString( (ulong)_crObjects.size()) : "" );
  if( _expand_depth == 0 ) { // Tmp item to make expanding handle visible
    _pItem->addChild( new QTreeWidgetItem() );
    return;
  }
  if( _count == 0 ) {
    _from = 0;
    _count = _crObjects.size();
  }

  typename std::list< T* >::const_iterator it = _crObjects.begin(),
                                        itEnd = _crObjects.end();
  if( _from != 0 )
    std::advance( it, _from );
  for( ulong c = 0; c < _count && it != itEnd; c++, ++it ) {
    QTreeWidgetItem* pChild = new QTreeWidgetItem();
    QString str_name = AddObjectDescr( pChild, *it, true, _expand_depth-1 );
    T* pObject = *it;
    QVariant qv = QVariant::fromValue( pObject );
    pChild->setData( 1, Qt::UserRole, qv );
    pChild->setText( 0, str_name );
    _pItem->addChild( pChild );
  }
}



////////////////////////////////////////////////////////////////////////////////

QString ModelTree::AddObjectDescr( QTreeWidgetItem* _pItem, IVarItemPtr _pObject,
    int _expand_depth, bool _rebuild )
{
  std::string str_title;
  QString str_type;

  if( _pItem->childCount() == 0 )
    _rebuild = true;

  if( _expand_depth == 0 )
    Clear( _pItem );

  if( _pObject == null ) {
    str_title = "null";
    str_type = SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );
  }
  else {
    str_title = _pObject->ShortInfo();
    if( _expand_depth == 0 )
      _pItem->addChild( new QTreeWidgetItem() );
    else {
      switch( _pObject->Type() ) {
      case OT::ELEMENT: {
        ElementPtr pElem = dgtype_cast< ElementPtr >( _pObject );
        str_type = AddBranchForElement( _pItem, pElem, _rebuild );
        break;
      }
      case OT::CHORD: {
        ChordPtr pCh = dgtype_cast< ChordPtr >( _pObject );
        str_type = AddBranchForChord( _pItem, pCh, _rebuild );
        break;
      }
      case OT::MESHCELL: {
        MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( _pObject );
        str_type = AddBranchForMeshCell( _pItem, pMC, _rebuild );
        break;
      }
      case OT::MESHELEMENT: {
        MeshElementPtr pME = dgtype_cast< MeshElementPtr >( _pObject );
        str_type = AddBranchForMeshElement( _pItem, pME, _rebuild );
        break;
      }
      case OT::CONTAINER: {
        VarItemListContainerPtr pCont = dgtype_cast< VarItemListContainerPtr >( _pObject );
        str_type = AddBranchForContainer( _pItem, pCont, _rebuild );
        break;
      }
      default:
        break;
      }
    }
  }

  _pItem->setText( 1, QString::fromStdString( str_title ) );
  _pItem->setToolTip( 1, QString::fromStdString( str_title ) );

  return str_type;
}


QString ModelTree::AddObjectDescr( QTreeWidgetItem* _pItem, IVarOriginPtr _pObject,
    int _expand_depth, bool _rebuild )
{
  std::string str_title;
  QString str_type;

  if( _pItem->childCount() == 0 )
    _rebuild = true;

  if( _expand_depth == 0 )
    Clear( _pItem );

  if( _pObject == null ) {
    str_title = "null";
    str_type = SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );
  }
  else {
    str_title = _pObject->ShortInfo();
    if( _expand_depth == 0 )
      _pItem->addChild( new QTreeWidgetItem() );
    else {
      switch( _pObject->Type() ) {
      case OT::ELEMENT: {
        ElementPtr pElem = dgtype_cast< ElementPtr >( _pObject );
        str_type = AddBranchForElement( _pItem, pElem, _rebuild );
        break;
      }
      case OT::CHORD: {
        ChordPtr pCh = dgtype_cast< ChordPtr >( _pObject );
        str_type = AddBranchForChord( _pItem, pCh, _rebuild );
        break;
      }
      case OT::SOURCE: {
        SourcePtr pSrc = dgtype_cast< SourcePtr >( _pObject );
        str_type = AddBranchForSource( _pItem, pSrc, _rebuild );
        break;
      }
      case OT::SEPARATOR: {
        SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( _pObject );
        str_type = AddBranchForSeparator( _pItem, pSep, _rebuild );
        break;
      }
      case OT::VARSET: {
        VarSetPtr pVS = dgtype_cast< VarSetPtr >( _pObject );
        str_type = AddBranchForVarSet( _pItem, pVS, _rebuild );
        break;
      }
      case OT::VARSETDEF: {
        VarSetDefPtr pVSD = dgtype_cast< VarSetDefPtr >( _pObject );
        str_type = AddBranchForVarSetDef( _pItem, pVSD, _rebuild );
        break;
      }
      default:
        break;
      }
    }
  }

  _pItem->setText( 1, QString::fromStdString( str_title ) );
  _pItem->setToolTip( 1, QString::fromStdString( str_title ) );

  return str_type;
}


QString ModelTree::AddObjectDescr( QTreeWidgetItem* _pItem, IComponentPtr _pObject,
    int _expand_depth, bool _rebuild, int _mode )
{
  std::string str_title;
  QString str_type;

  if( _pItem->childCount() == 0 )
    _rebuild = true;

  if( _expand_depth == 0 )
    Clear( _pItem );

  if( _pObject == null ) {
    str_title = "null";
    str_type = SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );
  }
  else {
    str_title = _pObject->ShortInfo();
    if( _expand_depth == 0 )
      _pItem->addChild( new QTreeWidgetItem() );
    else {
      switch( _pObject->Type() ) {
      case OT::ELEMENT: {
        ElementPtr pElem = dgtype_cast< ElementPtr >( _pObject );
        str_type = AddBranchForElement( _pItem, pElem, _expand_depth, _rebuild );
        break;
      }
      case OT::SEPARATOR: {
        SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( _pObject );
        str_type = AddBranchForSeparator( _pItem, pSep, _expand_depth, _rebuild );
        break;
      }
      case OT::SOURCE: {
        SourcePtr pSrc = dgtype_cast< SourcePtr >( _pObject );
        str_type = AddBranchForSource( _pItem, pSrc, _expand_depth, _rebuild );
        break;
      }
      case OT::CHORD: {
        ChordPtr pCh = dgtype_cast< ChordPtr >( _pObject );
        str_type = AddBranchForChord( _pItem, pCh, _expand_depth, _rebuild );
        break;
      }
      case OT::NODE: {
        NodePtr pNode = dgtype_cast< NodePtr >( _pObject );
        str_type = AddBranchForNode( _pItem, pNode, _expand_depth, _rebuild );
        break;
      }
      case OT::MESHPOINT: {
        MeshPointPtr pMP = dgtype_cast< MeshPointPtr >( _pObject );
        str_type = AddBranchForMeshPoint( _pItem, pMP, _expand_depth, _rebuild );
        break;
      }
      case OT::MESHCELL: {
        MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( _pObject );
        str_type = AddBranchForMeshCell( _pItem, pMC, _expand_depth, _rebuild );
        break;
      }
      case OT::MESHELEMENT: {
        MeshElementPtr pME = dgtype_cast< MeshElementPtr >( _pObject );
        str_type = AddBranchForMeshElement( _pItem, pME, _expand_depth, _rebuild );
        break;
      }
      case OT::XPOINTTEST: {
        XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( _pObject );
        str_type = AddBranchForXPointTest( _pItem, pXPT, _expand_depth, _rebuild );
        break;
      }
      case OT::XPOINTSEG: {
        XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( _pObject );
        str_type = AddBranchForXPointSeg( _pItem, pXPS, _expand_depth, _rebuild );
        break;
      }
      case OT::GRIDPOINTSEG: {
        GridPointSegPtr pGPS = dgtype_cast< GridPointSegPtr >( _pObject );
        str_type = AddBranchForGridPointSeg( _pItem, pGPS, _expand_depth, _rebuild );
        break;
      }
      case OT::GRIDPOINTEX: {
        GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( _pObject );
        str_type = AddBranchForGridPointEx( _pItem, pGPX, _expand_depth, _rebuild );
        break;
      }
      case OT::SURFACEZONE: {
        SurfaceZonePtr pSZ = dgtype_cast< SurfaceZonePtr >( _pObject );
        str_type = AddBranchForSurfaceZone( _pItem, pSZ, _expand_depth, _rebuild );
        break;
      }
      case OT::SURFACEEX: {
        SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( _pObject );
        str_type = AddBranchForSurfaceEx( _pItem, pSX, _expand_depth, _rebuild );
        break;
      }
      case OT::CONTAINER: {
        VarItemListContainerPtr pCont = dgtype_cast< VarItemListContainerPtr >( _pObject );
        str_type = AddBranchForContainer( _pItem, pCont, _expand_depth, _rebuild );
        break;
      }
      case OT::VARSETDEF: {
        VarSetDefPtr pVSD = dgtype_cast< VarSetDefPtr >( _pObject );
        str_type = AddBranchForVarSetDef( _pItem, pVSD, _expand_depth, _rebuild );
        break;
      }
      case OT::VARDEF: {
        VarDefPtr pVD = dgtype_cast< VarDefPtr >( _pObject );
        str_type = AddBranchForVarDef( _pItem, pVD, _expand_depth, _rebuild );
        break;
      }
      case OT::VARSET: {
        VarSetPtr pVS = dgtype_cast< VarSetPtr >( _pObject );
        str_type = AddBranchForVarSet( _pItem, pVS, _expand_depth, _rebuild );
        break;
      }
      case OT::VAR: {
        VarPtr pVar = dgtype_cast< VarPtr >( _pObject );
        str_type = AddBranchForVar( _pItem, pVar, _expand_depth, _rebuild, _mode );
        break;
      }
      default:
        break;
      }
    }
  }

  _pItem->setText( 1, QString::fromStdString( str_title ) );
  _pItem->setToolTip( 1, QString::fromStdString( str_title ) );

  return str_type;
}

QString ModelTree::AddActionDescr( QTreeWidgetItem* _pItem, IActionPtr _pAction,
                                   int _expand_depth, bool _rebuild )
{
  QString str_title;
  QString str_type;

  if( _pItem->childCount() == 0 )
    _rebuild = true;

  if( _expand_depth == 0 )
    Clear( _pItem );

  if( _pAction == null ) {
    str_title = "null";
    str_type = SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );
  }
  else {
    if( _expand_depth == 0 )
      _pItem->addChild( new QTreeWidgetItem() );

    QString sObjDescr = _pAction->Object() == null ?
                          SM_GUI( GUI::MODEL_INFO::TYPE::MODEL ) :
                          QString::fromStdString( _pAction->Object()->DetailedInfo() );

    switch( _pAction->Type() ) {
    case ACT::CHANGE_VALUE: {
      ActChangeValue* pActCV = static_cast< ActChangeValue* >( _pAction );
      QString sParam = QString::fromStdString( pActCV->Holder()->GetName( pActCV->ID() ) );
      QString sValue = QString::fromStdString( pActCV->Value().ToString() );

      str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::CHANGE_VALUE );
      str_title = sObjDescr + " [" + sParam + "]";

      int i = 0;
      if( _rebuild || _pItem->childCount() != 3 ) {
        Clear( _pItem );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ACTIONS::PARAMETER ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ACTIONS::VALUE ) );
        if( _pAction->Object() == null )
          AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TYPE::MODEL ) );
        else
          AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
      }
      SetChildData( _pItem, i++, sParam );
      SetChildData( _pItem, i++, sValue );
      if( _pAction->Object() == null )
        SetChildData( _pItem, i++, "" );
      else
        SetChildData( _pItem, i++, _pAction->Object(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::CHANGE_OBJGROUP: {
      ActChangeGroup* pActCG = static_cast< ActChangeGroup* >( _pAction );
      QString sInclude = pActCG->Include() ? SM_MSG( DG3::ACT_INCLUDE ) :
                                             SM_MSG( DG3::ACT_EXCLUDE );

      str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::CHANGE_OBJGROUP );
      str_title = sObjDescr + ", " + ToQString( pActCG->VarItem()->ShortInfo() ) +
                 "[" + sInclude + "]";

      int i = 0;
      if( _rebuild || _pItem->childCount() != 3 ) {
        Clear( _pItem );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ACTIONS::ACTION ) );
        AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VAR ) );
        AddChild( _pItem, TypeToStr( pActCG->VarItem()->Type() ) + ":" );
      }
      SetChildData( _pItem, i++, sInclude );
      SetChildData( _pItem, i++, pActCG->Object(), _expand_depth, 1 ); /*branch*/
      SetChildData( _pItem, i++, pActCG->VarItem(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::MARK_OBJECT: {
      ActMarkObject* pActMO = static_cast< ActMarkObject* >( _pAction );
      QString sInclude = pActMO->Include() ? SM_MSG( DG3::ACT_INCLUDE ) :
                                             SM_MSG( DG3::ACT_EXCLUDE );

      str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::MARK_OBJECT );
      str_title = sObjDescr + "[" + sInclude + "]";

      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ACTIONS::ACTION ) );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
      }
      SetChildData( _pItem, i++, sInclude );
      SetChildData( _pItem, i++, pActMO->Object(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::HIGHLIGHT_OBJECT: {
      ActHighlightObject* pActHO = static_cast< ActHighlightObject* >( _pAction );
      QString sInclude = pActHO->Include() ? SM_MSG( DG3::ACT_INCLUDE ) :
                                             SM_MSG( DG3::ACT_EXCLUDE );

      str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::HIGHLIGHT_OBJECT );
      str_title = sObjDescr + "[" + sInclude + "]";

      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ACTIONS::ACTION ) );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
      }
      SetChildData( _pItem, i++, sInclude );
      SetChildData( _pItem, i++, pActHO->Object(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::CHANGE_CHORD: {
      ActChangeChord* pActCh = static_cast< ActChangeChord* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 3 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::POINT_1 ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::POINT_2 ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->P1() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->P2() ) );
      break;
    }
    case ACT::CHANGE_ELEMENT: {
      ActChangeElem* pActCh = static_cast< ActChangeElem* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 4 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ID ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::NODE_1 ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::NODE_2 ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->ID() ) );
      SetChildData( _pItem, i++, pActCh->Node1(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, pActCh->Node2(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::CHANGE_GRIDPOINTEX: {
      ActChangeGridPointEx* pActCh = static_cast< ActChangeGridPointEx* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 3 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::EQUIL::VALUE ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Zone() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Value() ) );
      break;
    }
    case ACT::CHANGE_MESHPOINT: {
      ActChangeMeshPoint* pActCh = static_cast< ActChangeMeshPoint* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Position() ) );
      break;
    }
    case ACT::CHANGE_NODE: {
      ActChangeNode* pActCh = static_cast< ActChangeNode* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Position() ) );
      break;
    }
    case ACT::CHANGE_SEPARATOR: {
      ActChangeSeparator* pActCh = static_cast< ActChangeSeparator* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 4 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ID ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
        AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::NODE ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->ID() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Position() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Node()->Position() ) );
      break;
    }
    case ACT::CHANGE_SOURCE: {
      ActChangeSource* pActCh = static_cast< ActChangeSource* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Position() ) );
      break;
    }
    case ACT::CHANGE_SURFACEEX: {
      ActChangeSurfaceEx* pActCh = static_cast< ActChangeSurfaceEx* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 4 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LEVEL) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ORIGIN ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Zone() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Level() ) );
      SetChildData( _pItem, i++, ToQString( pActCh->XY() ) );
      break;
    }
    case ACT::CHANGE_TEMPLATE: {
      ActChangeTemplate* pActCh = static_cast< ActChangeTemplate* >( _pAction );
      const TemplateParams& crTP = pActCh->Params();
      int i = 0;
      if( _rebuild || _pItem->childCount() != 4 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TEMPLATE::INCR ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TEMPLATE::SCALE) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TEMPLATE::ANGLE ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( crTP.incr ) );
      SetChildData( _pItem, i++, ToQString( crTP.scale ) );
      SetChildData( _pItem, i++, ToQString( crTP.angle ) );

      break;
    }
    case ACT::CHANGE_VARDEF: {
      ActChangeVarDef* pActCh = static_cast< ActChangeVarDef* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 7 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::NAME ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::DESCR ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::ROWCOLUMN ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::VARTYPE ) );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::FLAGS ) );
        AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARSETDEF ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( pActCh->Data()->sName ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Data()->sDescr ) );
      SetChildData( _pItem, i++, ToQString( pActCh->Data()->index, false ) );
      SetChildData( _pItem, i++, TypeFlagToStr( pActCh->Data()->varType ) );
      SetChildData( _pItem, i++, FlagToStr( pActCh->Data()->flags ) );
      SetChildData( _pItem, i++, (IComponentPtr)pActCh->VSD(), _expand_depth ); /*branch*/
      break;
    }
    case ACT::CHANGE_VARSETDEF: {
      ActChangeVarSetDef* pActCh = static_cast< ActChangeVarSetDef* >( _pAction );
      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
        AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::MINMAXSETS ) );
      }
      SetChildData( _pItem, i++, pActCh->Object(), _expand_depth ); /*branch*/
      SetChildData( _pItem, i++, ToQString( NPoint( pActCh->MinVarSets(), pActCh->MaxVarSets() ), false ) );
      break;
    }
    default: {
      //const QString& crsType = TypeToStr( pActSub->Object()->Type() );
      if( (_pAction->Type()&ACT::MASK_ACTION) == ACT::ADD_ACTION )
        str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::ADD_X ) +
                   TypeToStr( _pAction->Object()->Type() );
      else if( (_pAction->Type()&ACT::MASK_ACTION) == ACT::DEL_ACTION )
        str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::DEL_X ) +
                   TypeToStr( _pAction->Object()->Type() );
      else
        str_type = SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );

      str_title = sObjDescr;

      int i = 0;
      if( _rebuild || _pItem->childCount() != 2 ) {
        Clear( _pItem );
        AddChild( _pItem, TypeToStr( _pAction->Object()->Type() ) + ":" );
      }
      SetChildData( _pItem, i++, _pAction->Object(), _expand_depth ); /*branch*/
      break;
    }
    }

    if( HasAnyFlag( _pAction->Type(), ACT::MASK_GROUP ) &&
        (_pAction->Type()&ACT::MASK_ACTION) == ACT::CHANGE_ACTION ) {
      str_type = SM_GUI( GUI::MODEL_INFO::ACTIONS::CHG_X ) +
                 TypeToStr( _pAction->Object()->Type() );
      str_title = sObjDescr;
    }
  }

  _pItem->setText( 0, str_type );
  _pItem->setText( 1, str_title );
  _pItem->setToolTip( 1, str_title );

  return str_type;
}

void ModelTree::UpdateTreeItem( QTreeWidgetItem* pItem )
{
  /*for( int c = 0; c < pItem->childCount(); c++ )
    pTree->collapseItem( pItem->child( c ) );*/

  QVariant data = pItem->data( 1, Qt::UserRole );
  if( data.isNull() )
    return;
  else if( data.userType() == QMetaType::type( "IComponentListPtr" ) ) {
    QVariant data2 = pItem->data( 1, Qt::UserRole+2 );
    if( data2.isNull() )
      AddObjectList( pItem, *data.value<IComponentListPtr>(), 1 );
    else {
      ulong gfrom = pItem->data( 1, Qt::UserRole+1 ).toUInt();
      ulong gsize = data2.toUInt();
      AddObjectList( pItem, *data.value<IComponentListPtr>(), 1, gfrom, gsize );
    }
  }
  else if( data.userType() == QMetaType::type( "IVarItemListPtr" ) ) {
    QVariant data2 = pItem->data( 1, Qt::UserRole+2 );
    if( data2.isNull() )
      AddObjectList( pItem, *data.value<IVarItemListPtr>(), 1 );
    else {
      ulong gfrom = pItem->data( 1, Qt::UserRole+1 ).toUInt();
      ulong gsize = data2.toUInt();
      AddObjectList( pItem, *data.value<IVarItemListPtr>(), 1, gfrom, gsize );
    }
  }
  else if( data.userType() == QMetaType::type( "IComponentPtr" ) ) {
    QVariant data10 = pItem->data( 1, Qt::UserRole+10 );
    int mode = 0;
    if( !data10.isNull() )
      mode = data10.toInt();
    AddObjectDescr( pItem, data.value<IComponentPtr>(), 1, true, mode );
  }
  else if( data.userType() == QMetaType::type( "IVarOriginPtr" ) )
    AddObjectDescr( pItem, data.value<IVarOriginPtr>(), 1, true );
  else if( data.userType() == QMetaType::type( "IVarItemPtr" ) )
    AddObjectDescr( pItem, data.value<IVarItemPtr>(), 1, true );
  else if( data.userType() == QMetaType::type( "VarListPtr" ) )
    AddVarList( pItem, *data.value<VarListPtr>(), 1 );
  else if( data.userType() == QMetaType::type( "IActionPtr" ) )
    AddActionDescr( pItem, data.value<IActionPtr>(), 1, true );
}

QString ModelTree::FlagToStr( long flags ) const
{
  QString result;
  if( HasAnyFlag( flags, VF::NOEXPORT ) )
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::NOEXPORT );
  if( HasAnyFlag( flags, VF::FORELEMS ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::FORELEMS );
  }
  if( HasAnyFlag( flags, VF::FORSEPARATORS ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::FORSEPARATORS );
  }
  if( HasAnyFlag( flags, VF::FORSOURCES ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::FORSOURCES );
  }
  if( HasAnyFlag( flags, VF::FORCHORDS ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::FORCHORDS );
  }
  if( HasAnyFlag( flags, VF::LAYERINDEX ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::VDFLAGS::LAYERINDEX );
  }
  return result;
}

QString ModelTree::TypeFlagToStr( VarType type ) const
{
  QString result;
  if( HasAnyFlag( type, VTF::HASELEMS ) )
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::HASELEMS );
  if( HasAnyFlag( type, VTF::LOCKGROUP ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::LOCKGROUP );
  }
  if( HasAnyFlag( type, VTF::SINGLE ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::SINGLE );
  }
  if( HasAnyFlag( type, VTF::STRUCTPART ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::STRUCTPART );
  }
  if( HasAnyFlag( type, VTF::HASCHORDS ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::HASCHORDS );
  }
  if( HasAnyFlag( type, VTF::HAS_MESH_OBJECTS ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::HASMESH );
  }
  if( HasAnyFlag( type, VTF::TARGET ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::TARGET );
  }
  if( HasAnyFlag( type, VTF::FILENAME ) ) {
    if( !result.isEmpty() ) result += "|";
    result += SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS::FILENAME );
  }
  return result;
}

const QString& ModelTree::TypeToStr( VarType type ) const
{
  switch( type ) {
  case VT::INT:             return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::INT );
  case VT::FLOAT:           return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::FLOAT );
  case VT::TEXT:            return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::TEXT );
  case VT::FILENAME:        return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::FILENAME );
  case VT::ELEM:            return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::ELEM );
  case VT::ELEMS:           return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::ELEMS );
  case VT::TARGET1:         return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::TARGET1 );
  case VT::TARGET2:         return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::TARGET2 );
  case VT::STRUCTURE:       return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::STRUCTURE );
  case VT::STRUCTPART:      return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::STRUCTPART );
  case VT::CHORDS:          return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::CHORDS );
  case VT::MESH_CELLS:      return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::MESHCELLS );
  case VT::MESH_ELEMENTS:   return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::MESHELEMS );
  case VT::MESH_H_ELEMENTS: return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::MESH_HELEMS );
  case VT::MESH_V_ELEMENTS: return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::MESH_VELEMS );
  case VT::TOPVIEW:         return SM_GUI( GUI::MODEL_INFO::VARS::TYPES::TOPVIEW );
  }
  return SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN ); /*never*/
}

const QString& ModelTree::TypeToStr( ObjectType type ) const
{
  switch( type ) {
  case OT::ELEMENT:       return SM_GUI( GUI::MODEL_INFO::TYPE::ELEMENT );
  case OT::NODE:          return SM_GUI( GUI::MODEL_INFO::TYPE::NODE );
  case OT::EQUIL:         return SM_GUI( GUI::MODEL_INFO::TYPE::EQUIL );
  case OT::TEMPLATE:      return SM_GUI( GUI::MODEL_INFO::TYPE::TEMPLATE );
  case OT::VARSETDEF:     return SM_GUI( GUI::MODEL_INFO::TYPE::VARSETDEF );
  case OT::VARDEF:        return SM_GUI( GUI::MODEL_INFO::TYPE::VARDEF );
  case OT::VARSET:        return SM_GUI( GUI::MODEL_INFO::TYPE::VARSET );
  case OT::VAR:           return SM_GUI( GUI::MODEL_INFO::TYPE::VAR );
  case OT::SEPARATOR:     return SM_GUI( GUI::MODEL_INFO::TYPE::SEPARATOR );
  case OT::SOURCE:        return SM_GUI( GUI::MODEL_INFO::TYPE::SOURCE );
  case OT::CHORD:         return SM_GUI( GUI::MODEL_INFO::TYPE::CHORD );
  case OT::XPOINTTEST:    return SM_GUI( GUI::MODEL_INFO::TYPE::XPOINTTEST );
  case OT::XPOINTSEG:     return SM_GUI( GUI::MODEL_INFO::TYPE::XPOINTSEG );
  case OT::MESH:          return SM_GUI( GUI::MODEL_INFO::TYPE::MESH );
  case OT::MESHCELL:      return SM_GUI( GUI::MODEL_INFO::TYPE::MESHCELL );
  case OT::MESHPOINT:     return SM_GUI( GUI::MODEL_INFO::TYPE::MESHPOINT );
  case OT::MESHELEMENT:   return SM_GUI( GUI::MODEL_INFO::TYPE::MESHELEMENT );
  case OT::GRIDPOINTSEG:  return SM_GUI( GUI::MODEL_INFO::TYPE::GRIDPOINTSEG );
  case OT::GRIDPOINTEX:   return SM_GUI( GUI::MODEL_INFO::TYPE::GRIDPOINTEX );
  case OT::SURFACEZONE:   return SM_GUI( GUI::MODEL_INFO::TYPE::SURFACEZONE );
  case OT::SURFACEEX:     return SM_GUI( GUI::MODEL_INFO::TYPE::SURFACEEX );
  case OT::TOPOLOGY:      return SM_GUI( GUI::MODEL_INFO::TYPE::TOPOLOGY );
  default:                return SM_GUI( GUI::MODEL_INFO::TYPE::UNKNOWN );
  }
}

template< typename T>
QString ModelTree::AddBranchForContainer(QTreeWidgetItem* _pItem, ListContainer<T>* _pCont, int _expand_depth, bool _rebuild )
{
  UNUSED( _rebuild );
  if( _expand_depth > 0 )
    AddObjectList( _pItem, *_pCont->pList, _expand_depth-1 );
  else // Tmp item to make expanding handle visible
    _pItem->addChild( new QTreeWidgetItem() );
  _pItem->setData( 1, Qt::UserRole, QVariant::fromValue( _pCont->pList ) );

  return SM_GUI( GUI::MODEL_INFO::TYPE::CONTAINER );
}
