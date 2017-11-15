#ifndef LISTEXT_H
#define LISTEXT_H

#include <list>
#include <algorithm>

#define null NULL

template< typename T >
static typename std::list< T* >& operator += ( std::list< T* >& _rSource,
                                               const std::list< T* >& _crAdd ) {
  _rSource.insert( _rSource.end(), _crAdd.begin(), _crAdd.end() );
  return _rSource;
}

template< typename T >
static bool Contains
( const std::list< T* >& _crList,
  const T* const _inpObject ) /* InGroup */
{
  assert( _inpObject != null );
  return std::find( _crList.begin(), _crList.end(), _inpObject ) != _crList.end();
}

template< typename T, typename O >
static unsigned IndexOf
( const std::list< T* >& _crList,
  const O* const _inpObject ) /* GroupIndex */
{
  assert( _inpObject != null );
  unsigned index = 0;
  typename std::list< T* >::const_iterator it = _crList.begin();
  typename std::list< T* >::const_iterator itEnd = _crList.end();
  for( ; it != itEnd && static_cast< const T* >( _inpObject ) != *it; ++it, index++ );
  assert( it != itEnd );
  return index;
}

template <typename T >
/*static typename std::list< T* >::iterator*/ bool Remove
( std::list< T* >& _crList,
  const T* _cpObj ) /* GroupDel */
{
  assert( _cpObj != null );
  typename std::list< T* >::iterator it = std::find( _crList.begin(), _crList.end(), _cpObj );
  if( it == _crList.end() )
    return false/*it*/;
  else {
    /*return*/ _crList.erase( it );
    return true;
  }
}


template <typename T >
static bool ContainsAll
( const std::list< T* >& _crFrom,
  const std::list< T* >& _crIn ) /* GroupInGroup */
{
  typename std::list< T* >::const_iterator itEnd = _crFrom.end();
  for( typename std::list< T* >::const_iterator it = _crFrom.begin();
       it != itEnd; ++it )
    if( !Contains( _crIn, *it ) )
      return false;
  return true;
}

template <typename TPtr >
static TPtr At
( const std::list< TPtr >& _crGrp,
  unsigned _id ) /* GroupAt */
{
  if( _crGrp.size() - 1 < _id )
    return null;
  typename std::list< TPtr >::const_iterator it = _crGrp.begin();
  std::advance( it, _id );
  return *it;
}

#endif // LISTEXT_H
