#include "DgDataModel.Tree.h"

namespace dm {

TreeItem::TreeItem( TreeItem* _pParent, unsigned _line, const QString& _crName, const QString& _crValue ):
  name( _crName ),
  value( _crValue ),
  line( _line ),
  children(),
  pParentItem( _pParent ) {}

TreeItem::~TreeItem() {
  Clear();
}

TreeItem* TreeItem::AppendChild( TreeItem* _pChild ) {
  children.append( _pChild );
  return _pChild;
}

TreeItem* TreeItem::AppendChild( unsigned _line, const QString& _crName, const QString& _crValue ) {
  TreeItem* pChild = new TreeItem( this, _line, _crName, _crValue );
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
  switch( _column ) {
  case 0: return name;
  case 1: return line == LINE_TECHNIC ? "" : QString( "%1" ).arg( line );
  default: return value;
  }
}

void TreeItem::SetData( int _column, const QVariant& _crValue ) {
  switch( _column ) {
  case 0: name = _crValue.toString(); break;
  //case 1: line = _crValue.toString(); break;
  case 2: value = _crValue.toString(); break;
  }
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
