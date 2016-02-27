#ifndef DGDATAMODEL_TREE_H
#define DGDATAMODEL_TREE_H

#include <QString>
#include <QList>
#include <QVariant>

namespace dm {

class TreeItem {
  static const int COLUMNS_COUNT = 2;

  QString                name;
  QString               value;
  QList< TreeItem* > children;
  TreeItem*       pParentItem;

public:
  TreeItem( TreeItem* _pParent, const QString& _crName, const QString& _crValue = "" );
  ~TreeItem();

  void Load();
  void Save() const;

  TreeItem* AppendChild( TreeItem* _pChild );
  TreeItem* AppendChild( const QString& _crName, const QString& _crValue = "" );
  TreeItem*       Child( int _row );
  int        ChildCount() const;
  int       ColumnCount() const;
  QVariant         Data( int _column ) const;
  void          SetData( int _column, const QVariant& _crValue );
  int               Row() const;
  TreeItem*  ParentItem();
  void            Clear();
  void          SetName( const QString& _crName );
  void         SetValue( const QString& _crValue );

};

} // namespace dm


#endif // DGDATAMODEL_TREE_H

