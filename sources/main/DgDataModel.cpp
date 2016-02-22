#include "DgDataModel.h"

DgDataModel::TreeItem::TreeItem( TreeItem* _pParent, const QString& _crName, const QString& _crValue ):
  name( _crName ),
  value( _crValue ),
  children(),
  pParentItem( _pParent ) {}

DgDataModel::TreeItem::~TreeItem() {
  Clear();
}

void DgDataModel::TreeItem::AppendChild( DgDataModel::TreeItem* _pChild ) {
  children.append( _pChild );
}

void DgDataModel::TreeItem::AppendChild( const QString& _crName, const QString& _crValue ) {
  children.append( new TreeItem( this, _crName, _crValue ) );
}

DgDataModel::TreeItem* DgDataModel::TreeItem::Child( int _row ) {
  return children.value( _row );
}

int DgDataModel::TreeItem::ChildCount() const {
  return children.count();
}

int DgDataModel::TreeItem::ColumnCount() const {
  return COLUMNS_COUNT;
}

QVariant DgDataModel::TreeItem::Data( int _column ) const {
  return _column == 0 ? name : value;
}

void DgDataModel::TreeItem::SetData( int _column, const QVariant& _crValue ) {
  if( _column == 0 )
    name = _crValue.toString();
  else
    value = _crValue.toString();
}

int DgDataModel::TreeItem::Row() const {
  if( pParentItem != 0 )
    return pParentItem->children.indexOf( const_cast< TreeItem* >( this ) );
  return 0;
}

DgDataModel::TreeItem* DgDataModel::TreeItem::ParentItem() {
  return pParentItem;
}

void DgDataModel::TreeItem::Clear() {
  qDeleteAll( children );
}

void DgDataModel::TreeItem::SetName( const QString& _crName ) {
  name = _crName;
}

void DgDataModel::TreeItem::SetValue( const QString& _crValue ) {
  value = _crValue;
}

/******************************************************************************/

DgDataModel::DgDataModel( const QString& _crFilename ):
  file( _crFilename ),
  root( 0, "/" ) {
  root.AppendChild( "filename", _crFilename );

  if( not file.exists() ) {
    status = NOTEXISTS;
    root.AppendChild( "status", "not exists" );
    return;
  }

  QFile::OpenMode flags = QFile::Text | QFile::ReadOnly;
  if( file.isWritable() ) {
    status = READWRITE;
    flags |= QFile::WriteOnly;
    root.AppendChild( "status", "read & write" );
  }
  else {
    status = READONLY;
    root.AppendChild( "status", "read only" );
  }

  if( not file.open( flags ) ) {
    status = OPENFAIL;
    root.Child( 1 )->SetValue( "opening failed" );
    return;
  }
}

QModelIndex DgDataModel::index( int _row, int _column, const QModelIndex& _crParent ) const {
  if( not hasIndex( _row, _column, _crParent ) )
    return QModelIndex();

  TreeItem* pParentItem;

  if( not _crParent.isValid() )
    pParentItem = const_cast< TreeItem* >( &root );
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

  if( pParentItem == &root )
    return QModelIndex();

  return createIndex( pParentItem->Row(), 0, pParentItem );
}

int DgDataModel::rowCount( const QModelIndex& _crParent ) const {
  TreeItem* pParentItem;
  if( _crParent.column() > 0 )
    return 0;

  if( not _crParent.isValid() )
    pParentItem = const_cast< TreeItem* >( &root );
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
  return QAbstractItemModel::flags( _crIndex );
}

void DgDataModel::Load() {
  root.Clear();

  //TODO:
}

void DgDataModel::Save() const {
  //TODO:
}
