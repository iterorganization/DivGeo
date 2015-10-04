#ifndef IVALUEHOLDER_H
#define IVALUEHOLDER_H

#include <string>
#include <sstream>
#include "../../common/Point.h"
#include "IComponent.h"
#include "IVarItem.h"

/*****************************************************************************
 * Universal Pointer (Variant)
 * Holdes pointers on member fields registered as properties (shared mode)
 * or contains variable data (not shared mode)
 *
 * Attention!!!
 * Don't use assigment operator with temporary objects like this:
 *      val = UPtr( data );
 * because all temporary data will be destroyed before assignment.
 * Use SetValue() method instead.
 ****************************************************************************/

class UPtr
{
public:
  enum EType { INT, LONG, ULONG, DOUBLE, BOOL, LIST, STR, WSTR, CRID, POINT, INDEX, UPTR, PTR };
private:

  EType type;
  union UN
  {
    int*          p_int;        UN( int* _p ):          p_int( _p ) {}
    long*         p_long;       UN( long* _p ):         p_long( _p ) {}
    ulong*        p_ulong;      UN( ulong* _p ):        p_ulong( _p ) {}
    double*       p_double;     UN( double* _p ):       p_double( _p ) {}
    bool*         p_bool;       UN( bool* _p ):         p_bool( _p ) {}
    IVarItemList* p_list;       UN( IVarItemList* _p ): p_list( _p ) {}
    std::string*  p_stdstr;     UN( std::string* _p ):  p_stdstr( _p ) {}
    std::wstring* p_stdwstr;    UN( std::wstring* _p ): p_stdwstr( _p ) {}
    CreatorId*    p_crid;       UN( CreatorId* _p ):    p_crid( _p ) {}
    Point*        p_point;      UN( Point* _p ):        p_point( _p ) {}
    NPoint*       p_index;      UN( NPoint* _p ):       p_index( _p ) {}
    UPtr*         p_uptr;       UN( UPtr* _p ):         p_uptr( _p ) {}
    void*         ptr;          UN( void* _p ):         ptr( _p ) {}

    UN( const UN& _cr ): ptr( _cr.ptr ) {}
    UN(): ptr( null ) {}
  } value;
  bool isShared;

public:

  EType Type() const { return type; }

  static const ulong NOSEL = -2L;
  static const ulong DIFF = -1L;

  /* Share constructors */
  UPtr( void* _p = null, EType _type = PTR ): type( _type ), value( _p ), isShared( true ) {}

  UPtr( int* _p ):          type( INT ),    value( _p ), isShared( true ) {}
  UPtr( long* _p ):         type( LONG ),   value( _p ), isShared( true ) {}
  UPtr( ulong* _p ):        type( ULONG ),  value( _p ), isShared( true ) {}
  UPtr( double* _p ):       type( DOUBLE ), value( _p ), isShared( true ) {}
  UPtr( bool* _p ):         type( BOOL ),   value( _p ), isShared( true ) {}
  UPtr( Point* _p ):        type( POINT ),  value( _p ), isShared( true ) {}
  UPtr( NPoint* _p ):       type( INDEX ),  value( _p ), isShared( true ) {}
  UPtr( UPtr* _p ):         type( UPTR ),   value( _p ), isShared( true ) {}
  UPtr( std::string* _p ):  type( STR ),    value( _p ), isShared( true ) {}
  UPtr( std::wstring* _p ): type( WSTR ),   value( _p ), isShared( true ) {}
  UPtr( CreatorId* _p ):    type( CRID ),   value( _p ), isShared( true ) {}
  UPtr( IVarItemList* _p ): type( LIST ),   value( _p ), isShared( true )
  { FOREACH_CONST( IVarItemIter, it, *value.p_list ) ((IVarItemPtr)*it)->GetPtr(); }

  /* Copy constructors */
  UPtr( const UPtr& _cr ): type( _cr.type ), value( _cr.value ), isShared( _cr.isShared )
  {
    if( type == LIST ) {
      FOREACH_CONST( IVarItemIter, it, *value.p_list )
        ((IVarItemPtr)*it)->GetPtr();
    }
  }

  UPtr( int _v ):     type( INT ),    value( new int( _v ) ),     isShared( false ) {}
  UPtr( long _v ):    type( LONG ),   value( new long( _v ) ),    isShared( false ) {}
  UPtr( ulong _v ):   type( ULONG ),  value( new ulong( _v ) ),   isShared( false ) {}
  UPtr( double _v ):  type( DOUBLE ), value( new double( _v ) ),  isShared( false ) {}
  UPtr( bool _v ):    type( BOOL ),   value( new bool( _v ) ),    isShared( false ) {}
  UPtr( Point _v ):   type( POINT ),  value( new Point( _v ) ),   isShared( false ) {}
  UPtr( NPoint _v ):  type( INDEX ),  value( new NPoint( _v ) ),  isShared( false ) {}
  UPtr( const std::string& _cr ):   type( STR ),  value( new std::string( _cr ) ),  isShared( false ) {}
  UPtr( const std::wstring& _cr ):  type( WSTR ), value( new std::wstring( _cr ) ), isShared( false ) {}
  UPtr( const CreatorId& _cr ):     type( CRID ), value( new CreatorId( _cr ) ),    isShared( false ) {}
  UPtr( const IVarItemList& _cr ):  type( LIST ), value( new IVarItemList ),        isShared( false ) {
    FOREACH_CONST( IVarItemIterConst, it, _cr )
      value.p_list->push_back( ((IVarItemPtr)*it)->GetPtr() );
  }

  UPtr( EType _type ) { Init( _type ); }

  /* Pointers initializer */
  void Init( EType _type ) {
    Release();
    type = _type;
    isShared = false;

    switch( type ) {
    case INT:     value.p_int     = new int;           break;
    case LONG:    value.p_long    = new long;          break;
    case ULONG:   value.p_ulong   = new ulong;         break;
    case DOUBLE:  value.p_double  = new double;        break;
    case BOOL:    value.p_bool    = new bool;          break;
    case STR:     value.p_stdstr  = new std::string;   break;
    case WSTR:    value.p_stdwstr = new std::wstring;  break;
    case CRID:    value.p_crid    = new CreatorId;     break;
    case LIST:    value.p_list    = new IVarItemList;  break;
    case POINT:   value.p_point   = new Point;         break;
    case INDEX:   value.p_index   = new NPoint;        break;
    default: break;
    }
  }

  /* Copy initializer */
  void SetValue( const UPtr& _crUPtr, bool _clear = false )
  {
    if( _clear )
      Init( _crUPtr.type );
    assert( type == UPTR || type == _crUPtr.type );

    switch( type ) {
    case INT:     *value.p_int      = _crUPtr.Int();        break;
    case LONG:    *value.p_long     = _crUPtr.Long();       break;
    case ULONG:   *value.p_ulong    = _crUPtr.ULong();      break;
    case DOUBLE:  *value.p_double   = _crUPtr.Double();     break;
    case BOOL:    *value.p_bool     = _crUPtr.Bool();       break;
    case STR:     *value.p_stdstr   = _crUPtr.StringRef();  break;
    case WSTR:    *value.p_stdwstr  = _crUPtr.WStringRef(); break;
    case CRID:    *value.p_crid     = _crUPtr.CridRef();    break;
    case LIST:    *value.p_list     = _crUPtr.ListRef();    break;
    case POINT:   *value.p_point    = _crUPtr.PointRef();   break;
    case INDEX:   *value.p_index    = _crUPtr.NPointRef();  break;
    case UPTR:    value.p_uptr->SetValue( _crUPtr, true );  break;
    default: break;
    }

    if( type == LIST ) {
      FOREACH_CONST( IVarItemIter, it, *value.p_list )
        ((IVarItemPtr)*it)->GetPtr();
    }
  }

  void SetEmptyList() { Init( LIST ); }
  void SetEmptyString() { Init( STR ); }

  /* Delete contained object of it is not shared */
  ~UPtr() { Release(); }
  void Release()
  {
    if( value.ptr == null )
      return;

    if( type == LIST ) {
      FOREACH_CONST( IVarItemIter, it, *value.p_list )
        ReleaseObject( *it );
    }

    if( !isShared ) {
      switch( type ) {
      case INT:     delete value.p_int;     break;
      case LONG:    delete value.p_long;    break;
      case ULONG:   delete value.p_ulong;   break;
      case DOUBLE:  delete value.p_double;  break;
      case BOOL:    delete value.p_bool;    break;
      case STR:     delete value.p_stdstr;  break;
      case WSTR:    delete value.p_stdwstr; break;
      case CRID:    delete value.p_crid;    break;
      case LIST:    delete value.p_list;    break;
      case POINT:   delete value.p_point;   break;
      case INDEX:   delete value.p_index;   break;
      default: break;
      }
    }
    value.ptr = null;
  }

  /* Ptr operations */
  UPtr Share() const {
    switch( type ) {
    case INT:     return value.p_int;
    case LONG:    return value.p_long;
    case ULONG:   return value.p_ulong;
    case DOUBLE:  return value.p_double;
    case BOOL:    return value.p_bool;
    case STR:     return value.p_stdstr;
    case WSTR:    return value.p_stdwstr;
    case CRID:    return value.p_crid;
    case LIST:    return value.p_list;
    case POINT:   return value.p_point;
    case INDEX:   return value.p_index;
    case UPTR:    return value.p_uptr;
    case PTR:     return value.ptr;
    default: assert( false );
    }
    return UPtr();
  }
  UPtr Copy() const {
    switch( type ) {
    case INT:     return *value.p_int;
    case LONG:    return *value.p_long;
    case ULONG:   return *value.p_ulong;
    case DOUBLE:  return *value.p_double;
    case BOOL:    return *value.p_bool;
    case STR:     return *value.p_stdstr;
    case WSTR:    return *value.p_stdwstr;
    case CRID:    return *value.p_crid;
    case LIST:    return *value.p_list;
    case POINT:   return *value.p_point;
    case INDEX:   return *value.p_index;
    case UPTR:    return value.p_uptr->Copy();
    case PTR:     return UPtr();
    default: assert( false );
    }
    return UPtr();
  }
  UPtr Move() {
    UPtr tmp = Share();
    isShared = true;
    Release();
    return tmp;
  }

  /* Compare operator (shared only) */
  bool operator == ( const UPtr& _crUPtr ) const { return value.ptr == _crUPtr.value.ptr; }

  /* Check operators */
  bool IsNull() const { return value.ptr == null; }
  bool IsEqual( const UPtr& _crUPtr ) const { return value.ptr == _crUPtr.value.ptr; }
  bool IsNOSEL() const { return value.ptr == (void*)NOSEL; }
  bool IsDIFF() const { return value.ptr == (void*)DIFF; }
  bool IsEmpty() const
  {
    if( value.ptr == null )
      return true;
    if( type == LIST && value.p_list->empty() )
      return true;
    if( type == STR && value.p_stdstr->empty() )
      return true;
    return false;
  }

  bool IsEqualByValue( const UPtr& _crUPtr ) const {
    if( type != _crUPtr.type )
      return false;
    switch( type ) {
    case INT:     return *value.p_int     == *_crUPtr.value.p_int;
    case LONG:    return *value.p_long    == *_crUPtr.value.p_long;
    case ULONG:   return *value.p_ulong   == *_crUPtr.value.p_ulong;
    case DOUBLE:  return *value.p_double  == *_crUPtr.value.p_double;
    case BOOL:    return *value.p_bool    == *_crUPtr.value.p_bool;
    case STR:     return *value.p_stdstr  == *_crUPtr.value.p_stdstr;
    case WSTR:    return *value.p_stdwstr == *_crUPtr.value.p_stdwstr;
    case CRID:    return *value.p_crid    == *_crUPtr.value.p_crid;
    case LIST:    return *value.p_list    == *_crUPtr.value.p_list;
    case POINT:   return *value.p_point   == *_crUPtr.value.p_point;
    case INDEX:   return *value.p_index   == *_crUPtr.value.p_index;
    case UPTR:    return value.p_uptr->IsEqualByValue( _crUPtr.value.p_uptr );
    case PTR:     return value.ptr        == _crUPtr.value.ptr;
    default: assert( false );
    }
    return false;
  }

  /* Text output method */
  std::string ToString() const
  {
    std::stringstream ss;
    switch( type ) {
    case INT:     ss << *value.p_int;     break;
    case LONG:    ss << *value.p_long;    break;
    case ULONG:   ss << *value.p_ulong;   break;
    case DOUBLE:  ss << *value.p_double;  break;
    case BOOL:    ss << *value.p_bool;    break;
    case STR:     ss << *value.p_stdstr;  break;
    case WSTR:    ss << "WSTRING"; break;
    case CRID:    ss << value.p_crid->ToStdString(); break;
    case LIST:    ss << "list [" << value.p_list->size() << "]"; break;
    case POINT:   ss << value.p_point->ToString(); break;
    case INDEX:   ss << value.p_index->ToString(); break;
    case UPTR:    ss << value.p_uptr->ToString(); break;
    case PTR:     ss << "null"; break;
    default: break;
    }
    return ss.str();
  }

  /* Type cast methods */
  int*            IntPtr()      const { return value.p_int; }
  long*           LongPtr()     const { return value.p_long; }
  ulong*          ULongPtr()    const { return value.p_ulong; }
  double*         DoublePtr()   const { return value.p_double; }
  bool*           BoolPtr()     const { return value.p_bool; }
  Point*          PointPtr()    const { return value.p_point; }
  NPoint*         NPointPtr()   const { return value.p_index; }
  std::string*    StringPtr()   const { return value.p_stdstr; }
  std::wstring*   WStringPtr()  const { return value.p_stdwstr; }
  CreatorId*      CridPtr()     const { return value.p_crid; }
  IVarItemListPtr ListPtr()     const { return value.p_list; }
  void*           VoidPtr()     const { return value.ptr; }

  int             Int()         const { return *value.p_int; }
  long            Long()        const { return *value.p_long; }
  ulong           ULong()       const { return *value.p_ulong; }
  double          Double()      const { return *value.p_double; }
  bool            Bool()        const { return *value.p_bool; }
  Point&          PointRef()    const { return *value.p_point; }
  NPoint&         NPointRef()   const { return *value.p_index; }
  std::string&    StringRef()   const { return *value.p_stdstr; }
  std::wstring&   WStringRef()  const { return *value.p_stdwstr; }
  CreatorId&      CridRef()     const { return *value.p_crid; }
  IVarItemList&   ListRef()     const { return *value.p_list; }
};

#define DG_PROPERTIES public: enum EProperties
#define DG_PROPERTIES_INIT InitProperties()
#define DG_PROPERTIES_TABLE private: void DG_PROPERTIES_INIT

typedef class IValueHolder
{
protected:
  struct PropertyPair
  {
    std::string sName;
    UPtr fieldPtr;
    PropertyPair() {}
    PropertyPair( const std::string& _crsName, const UPtr& _crFieldPtr ):
      sName( _crsName ), fieldPtr( _crFieldPtr ) {}
  };

  std::vector< PropertyPair > properties;

  void AddProperty( const std::string& _crsName, const UPtr& _crFieldPtr )
  { properties.push_back( PropertyPair( _crsName, _crFieldPtr ) ); }

public:
  void SetValue( uchar _id, const UPtr& _crValue )
  { properties[_id].fieldPtr.SetValue( _crValue ); }

  UPtr GetValue( uchar _id ) const
  { return UPtr( properties[_id].fieldPtr.Copy() ); }

  const std::string& GetName( uchar _id ) const
  { return properties[_id].sName; }

  virtual ~IValueHolder() {}

}* IValueHolderPtr;


#endif // IVALUEHOLDER_H
