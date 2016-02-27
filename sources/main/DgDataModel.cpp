#include "DgDataModel.h"
#include "DgDataModel.Tree.h"

namespace dm {

DgDataModel::DgDataModel( const QString& _crFilename ):
  file( _crFilename ),
  pRoot( new TreeItem( 0, TreeItem::LINE_TECHNIC, "/" ) ) {
  TreeItem* pFileinfo = pRoot->AppendChild( TreeItem::LINE_TECHNIC, "fileinfo" );
  pFileinfo->AppendChild( TreeItem::LINE_TECHNIC, "filename", _crFilename );

  if( not file.exists() ) {
    status = NOTEXISTS;
    pFileinfo->AppendChild( TreeItem::LINE_TECHNIC, "status", "not exists" );
    return;
  }

  QFile::OpenMode flags = QFile::Text | QFile::ReadOnly;

  if( not file.open( flags ) ) {
    status = OPENFAIL;
    pFileinfo->Child( 1 )->SetValue( "opening failed" );
    return;
  }

  if( file.isWritable() ) {
    status = READWRITE;
    flags |= QFile::WriteOnly;
    pFileinfo->AppendChild( TreeItem::LINE_TECHNIC, "status", "read & write" );
  }
  else {
    status = READONLY;
    pFileinfo->AppendChild( TreeItem::LINE_TECHNIC, "status", "read only" );
  }
}

DgDataModel::~DgDataModel() {
  delete pRoot;
}

QModelIndex DgDataModel::index( int _row, int _column, const QModelIndex& _crParent ) const {
  if( not hasIndex( _row, _column, _crParent ) )
    return QModelIndex();

  TreeItem* pParentItem;

  if( not _crParent.isValid() )
    pParentItem = const_cast< TreeItem* >( pRoot );
  else
    pParentItem = static_cast< TreeItem* >( _crParent.internalPointer() );

  TreeItem* pChildItem = pParentItem->Child( _row );
  if( pChildItem != 0 )
    return createIndex( _row, _column, pChildItem );
  else
    return QModelIndex();
}

QModelIndex DgDataModel::parent( const QModelIndex& _crIndex ) const {
  if( not _crIndex.isValid() )
    return QModelIndex();

  TreeItem* pChildItem = static_cast< TreeItem* >( _crIndex.internalPointer() );
  TreeItem* pParentItem = pChildItem->ParentItem();

  if( pParentItem == pRoot )
    return QModelIndex();

  return createIndex( pParentItem->Row(), 0, pParentItem );
}

int DgDataModel::rowCount( const QModelIndex& _crParent ) const {
  TreeItem* pParentItem;
  if( _crParent.column() > 0 )
    return 0;

  if( not _crParent.isValid() )
    pParentItem = const_cast< TreeItem* >( pRoot );
  else
    pParentItem = static_cast< TreeItem* >( _crParent.internalPointer() );

  return pParentItem->ChildCount();
}

int DgDataModel::columnCount( const QModelIndex& ) const {
  return COLUMNS_COUNT;
}

QVariant DgDataModel::data( const QModelIndex& _crIndex, int _role ) const {
  if( not _crIndex.isValid() )
    return QVariant();

  if( _role != Qt::DisplayRole )
    return QVariant();

  TreeItem* pItem = static_cast< TreeItem* >( _crIndex.internalPointer() );

  return pItem->Data( _crIndex.column() );
}

bool DgDataModel::setData( const QModelIndex& _crIndex, const QVariant& _crValue, int _role ) {
  if( not _crIndex.isValid() )
    return false;

  if( _role != Qt::DisplayRole )
    return false;

  TreeItem* pItem = static_cast< TreeItem* >( _crIndex.internalPointer() );
  pItem->SetData( _crIndex.column(), _crValue );

  return true;
}

Qt::ItemFlags DgDataModel::flags( const QModelIndex& _crIndex ) const {
  if( not _crIndex.isValid() )
    return 0;

  Qt::ItemFlags f = Qt::ItemIsEnabled;//QAbstractItemModel::flags( _crIndex );

  TreeItem* pItem = static_cast< TreeItem* >( _crIndex.internalPointer() );
  if( pItem->ParentItem() == pRoot and _crIndex.row() == 0 ) {}
  else if( status == READWRITE )
    f |= Qt::ItemIsEditable;
  return f;
}

void DgDataModel::Save() const {
  //TODO:
}

} // namespace dm
