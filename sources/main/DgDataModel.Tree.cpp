#include "DgDataModel.Tree.h"

namespace dm {

TreeItem::TreeItem( TreeItem* _pParent, const QString& _crName, const QString& _crValue ):
  name( _crName ),
  value( _crValue ),
  children(),
  pParentItem( _pParent ) {}

TreeItem::~TreeItem() {
  Clear();
}

TreeItem* TreeItem::AppendChild( TreeItem* _pChild ) {
  children.append( _pChild );
  return _pChild;
}

TreeItem* TreeItem::AppendChild( const QString& _crName, const QString& _crValue ) {
  TreeItem* pChild = new TreeItem( this, _crName, _crValue );
  children.append( pChild );
  return pChild;
}

TreeItem* TreeItem::Child( int _row ) {
  return children.value( _row );
}

int TreeItem::ChildCount() const {
  return children.count();
}

int TreeItem::ColumnCount() const {
  return COLUMNS_COUNT;
}

QVariant TreeItem::Data( int _column ) const {
  return _column == 0 ? name : value;
}

void TreeItem::SetData( int _column, const QVariant& _crValue ) {
  if( _column == 0 )
    name = _crValue.toString();
  else
    value = _crValue.toString();
}

int TreeItem::Row() const {
  if( pParentItem != 0 )
    return pParentItem->children.indexOf( const_cast< TreeItem* >( this ) );
  return 0;
}

TreeItem* TreeItem::ParentItem() {
  return pParentItem;
}

void TreeItem::Clear() {
  qDeleteAll( children );
}

void TreeItem::SetName( const QString& _crName ) {
  name = _crName;
}

void TreeItem::SetValue( const QString& _crValue ) {
  value = _crValue;
}

} // namespace dm
