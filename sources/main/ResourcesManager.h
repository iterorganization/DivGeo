#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QHash>

#include "../common/ResourceTree.h"
#define null NULL

#define INIT_TREE TreeNode* dg__pNode__ = null;
#define GROUP( path ) dg__pNode__ = codes.AddNode( (path) );
#define KEY( key, value ) dg__pNode__->AddValue( (key), (value) );

template< typename T >
QStringList Arg( T _arg ) { return QStringList( QString("%1").arg( _arg ) ); }

class Console;

class ResourcesManager
{
public:
  typedef QMap< ulong, DataValue* > Dictionary;
  typedef QMap< QString, Dictionary > DictionariesMap;

protected:
  Console* pConsole;
  QString sFilename;
  bool isLoaded;

  TreeNode codes;

  mutable bool plainTextMode;

  DictionariesMap dictionaries;

  mutable QStringList unknown;

public:
  ResourcesManager( Console *_pConsole, const QString& _crsFilename );

  const QStringList& GetStrings( ulong _code, const QString& _crsDictionary ) const;
  const QString&      GetString( ulong _code, const QString& _crsDictionary ) const;
  DataValue*              Value( ulong _code, const QString& _crsDictionary ) const;

  const TreeNode::NodesList& TopLevelNodes() { return codes.Nodes(); }

  const TreeNode*     Node( const QString& _crsPath ) { return codes.Node( _crsPath ); }
  ulong               Code( const QString& _crsPath ) { return codes.Code( _crsPath ); }
  const QStringList&  Data( const QString& _crsPath ) { return codes.Data( _crsPath ); }
  DataValue*         Value( const QString& _crsPath ) { return codes.Value( _crsPath ); }

  bool IsLoaded() const { return isLoaded; }

protected:
  virtual void ConstructKeys() = 0;
  void LoadStrings( bool _loadUnregistered = false );
  bool ParseTextString( const QString& _crStr, QStringList& _rlResult ) const;
};

#endif // RESOURCESMANAGER_H
