#ifndef LISTCONTAINER_H
#define LISTCONTAINER_H

#include "IVarItem.h"
#include "IComponent.h"

template< typename T >
class ListContainer:
  public IVarItem,
  public IComponent
{
public:
  typename std::list< T* >* pList;

  ListContainer( std::list< T* >* _pList ):
    IComponent( OT::CONTAINER, null ),
    pList( _pList ) { }

  ~ListContainer() { if( pList != null ) delete pList; }

  void ReleaseList() {
    /*FOREACH( std::list< T* >::iterator, it, *pList )
      ReleaseObject( (*it) );*/
    pList = null;
  }

  // IObject implementation
  virtual std::string Description() const { return ""; }
  virtual std::string DetailedInfo() const { return ""; }
  virtual std::string ShortInfo() const { return std::string( "[" ) << pList->size() << "]"; }
};

typedef ListContainer< IVarItem > VarItemListContainer;
DG_DEFINE_PTR(VarItemListContainer)
DG_DEFINE_PLAINARRAY(VarItemListContainerPtr)

typedef ListContainer< IComponent > ComponentListContainer;
DG_DEFINE_PTR(ComponentListContainer)
DG_DEFINE_PLAINARRAY(ComponentListContainerPtr)

template< typename T >
static void FreeContainerList( std::list< T* >* pList ) {
  assert( pList != null );
  FOREACH_CONST( IVarItemIter, it, (*pList) )
    if( (*it)->Type() == OT::CONTAINER )
      delete (*it);
  pList->clear();
  //delete pList;
}

template< typename T >
static void FreeContainersArray( std::list< ListContainer< T >* > _pArray, bool _deleteList = true ) {
  FOREACH_CONST( typename std::list< ListContainer< T >* >::iterator, it, _pArray ) {
    if( !_deleteList )
      (*it)->ReleaseList();
    delete (*it);
  }
  _pArray.clear();
  //delete _pArray;
}

template< typename T >
static void MergeGroupOfGroups( std::list< T* >* dest, std::list< T* >* src ) {
  assert( dest != null && src != null && dest != src );
  FOREACH_CONST( IVarItemIter, it1, (*src) ) {
    ListContainer< T >* pCont = static_cast< ListContainer< T >* >( *it1 );
    FOREACH_CONST( IVarItemIter, it2, (*pCont->pList) )
      dest->push_back( *it2 );
  }
}

template< typename T >
static typename std::list< ListContainer< T >* >::iterator Find( std::list< ListContainer< T >* >& grp, const std::list< T* >* pObj ) /* GroupDel */ {
  assert( pObj != null );
  FOREACH_CONST( typename std::list< ListContainer< T >* >::iterator, it, grp )
    if( (*it)->pList == pObj )
      return it;
  return grp.end();
}

template< typename T >
static bool Contains( std::list< ListContainer< T >* >& grp, const std::list< T* >* pObj ) /* InGroup */ {
  assert( pObj != null );
  if( Find( grp, pObj ) != grp.end() )
    return true;
  return false;
}

template< typename T >
static typename std::list< ListContainer< T >* >::iterator Remove( std::list< ListContainer< T >* >& grp, const std::list< T* >* pObj ) /* GroupDel */ {
  assert( pObj != null );
  return grp.erase( Find( grp, pObj ) );
}

#endif // LISTCONTAINER_H
