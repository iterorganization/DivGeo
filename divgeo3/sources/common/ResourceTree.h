#ifndef RESOURCETREE_H
#define RESOURCETREE_H

#include <QMap>
#include <QHash>
#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>

#define null NULL

struct DataValue {
  static const ulong NOCODE = 0xfffffff1;
  static const ulong EMPTY = 0xffffffff;

  ulong code;
  QString sKey;
  bool mark;
  QStringList strings;
  mutable QTreeWidgetItem* pItem;

  DataValue( const QString& _crsKey = "", ulong _code = EMPTY, bool _mark = false ):
    code( _code ),
    sKey( _crsKey ),
    mark( _mark ),
    pItem( null ) { }
  DataValue( const QString& _crsKey, const QStringList& _crStrings ):
    code( NOCODE ),
    sKey( _crsKey ),
    mark( true ),
    strings( _crStrings ),
    pItem( null ) { }
};

class TreeNode {
  QString path;

public:
  typedef QHash< QString, TreeNode* > NodesMap;
  typedef QHash< QString, DataValue* > ValuesMap;
  typedef QList< TreeNode > NodesList;
  typedef QList< DataValue > ValuesList;

private:
  NodesMap nodes;
  NodesList nodes_ordered;
  ValuesMap values;
  ValuesList values_ordered;

  QStringList unmarked;
  bool unmarked_ready;

  static const QStringList empty;

public:
  TreeNode( const QString& _path ):
    unmarked_ready( false ) {
    if( _path.isEmpty() || _path == "/" )
      path = "";
    else
      path = _path + ".";
  }
  ~TreeNode() { }

  const QString& Path() const { return path; }
  QString Key() const { return path.isEmpty() ? "" : path.left( path.length() - 1 ); }

  TreeNode*           Node( const QString& path );
  ulong               Code( const QString& path, bool mark = false );
  const QStringList&  Data( const QString& path ) const;
  DataValue*         Value( const QString& path ) const;

  TreeNode* AddNode( const QString& _path );
  TreeNode* AddValue( const QString& _path, ulong _code );
  TreeNode* SetValue( const QString& _path, const QStringList& _crStrings, bool _append = false );

  const QStringList& GetUnmarked();
  void ResetMarks();

  const NodesList& Nodes() const { return nodes_ordered; }
  const ValuesList& Values() const { return values_ordered; }
};


#endif // RESOURCETREE_H
