#ifndef ELEMENT_T_HPP
#define ELEMENT_T_HPP

#include "Node.h"

template< typename T >
ElementPtr Element::GetNextElem( std::list< T* >* _pElems, int _node ) const
{
  SENDER_NAME( "Element::GetNextElem" );
  assert( _node == 1 || _node == 2 );

  ElementPtr pElemReturned = null;

  int i = 0;
  const IComponentList& crElements = Node_Elements( _node );
  FOREACHPTRCONST( ElementPtr, pElemSelected, crElements ) {
    if( pElemSelected == this )
      continue;
    if( _pElems != null && !Contains( *_pElems, (T*)pElemSelected ) )
      continue;
    pElemReturned = pElemSelected;
    i++;
  }
  if( i != 1)
    return null;

  return pElemReturned;
}

#endif // ELEMENT_T_HPP
