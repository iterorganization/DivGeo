#ifndef IMANAGER_H
#define IMANAGER_H

#include <map>
#include "IComponent.h"
#include "DgTokens.h"

#define DG_HIERARCHY_INIT InitHierarchy()
#define DG_HIERARCHY_TABLE void DG_HIERARCHY_INIT

class IManager;
typedef IManager* IManagerPtr;

class TypeRecord
{
  IManagerPtr pManager; // Can not hold ** due to type cast fail
  IComponentListPtr pList;

public:
  TypeRecord():
    pManager( null ), pList( null ) {}

  TypeRecord( IManagerPtr _pManager ):
    pManager( _pManager ), pList( null ) {}

  TypeRecord( IComponentListPtr _pList ):
    pManager( null ), pList( _pList ) {}

  bool IsManaged() const { return pList != null; }
  IManagerPtr Manager() const { return pManager; }
  IComponentListPtr List() const { return pList; }
  IComponentListPtr SelectList( ObjectType _ot ) const;
};

class IManager
{
  typedef std::map< ObjectType, TypeRecord > TypeMap;
  TypeMap types;

protected:
  void RegisterType( ObjectType _ot, IManager* _pManager ) {
    types[ _ot ] = TypeRecord( _pManager );
  }

  void RegisterType( ObjectType _ot, IComponentListPtr _pList ) {
    types[ _ot ] = TypeRecord( _pList );
  }

public:
  bool IncludeObject( IComponentPtr _pObject );
  bool ExcludeObject( IComponentPtr _pObject );
  void Release();
  int ObjectIndex( IComponentInPtr _pinObject ) const;
  IComponentListPtr SelectList( ObjectType _ot ) const;

  virtual ~IManager() { Release(); }

  virtual int ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 ) = 0;
  virtual int WriteObjects( Token _token, FILE* _pFile, int _mode = 0 ) const = 0;
};

#endif // IMANAGER_H
