#ifndef IOBJECT_T_HPP
#define IOBJECT_T_HPP

template< typename T >
void ReleaseObject( T **_ppObject )
{
  if( _ppObject == null )
    return;
  IObject* pObject = *_ppObject;
  if( pObject != null ) {
    if( pObject->ReleasePtr() )
      delete pObject;
    *_ppObject = null;
  }
}

// It is pointers-unsafe method. Use it with unmanaged pointers only.
template <typename T > static void RestrictToType
( std::list< T* >& _crGrp, ObjectType _type ) /* RestrictGroupToType */
{
  for( typename std::list< T* >::iterator it = _crGrp.begin();
       it != _crGrp.end(); ++it )
    if( (*it)->Type() != _type )
      it = --_crGrp.erase( it );
}

#endif // IOBJECT_T_HPP
