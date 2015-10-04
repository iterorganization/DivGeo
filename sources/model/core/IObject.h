#ifndef IOBJECT_H
#define IOBJECT_H

#include <assert.h>
#include <string>
#include "../../common/ListExt.h"
#include "../../common/Types.h"

typedef unsigned long ulong;
typedef unsigned int uint;

// Object types
struct ObjectTypes
{
  enum EObjectType
  {
    UNKNOWN      =  0,
    ELEMENT      =  1,
    NODE         =  2,
    EQUIL        =  3,
    TEMPLATE     =  7,
    VARSETDEF    =  8,
    VARDEF       =  9,
    VARSET       = 10,
    VAR          = 11,
    APP          = 14,
    XAPP         = 15,
    VIEW         = 16,
    VIEWLABEL    = 17,
    SONNET       = 18,
    SEPARATOR    = 19,
    VARSDLG      = 20,
    SOURCE       = 21,
    CHORD        = 22,
    XPOINTTEST   = 23,
    XPOINTSEG    = 24,
    VIEWSHAPE    = 25,
    MESH         = 26,
    MESHCELL     = 27,
    MESHPOINT    = 28,
    MESHELEMENT  = 29,
    STRINGSOURCE = 30,
    GRIDPOINTSEG = 31,
    GRIDPOINTEX  = 32,
    SURFACEZONE  = 33,
    SURFACEEX    = 34,
    TOPOLOGY     = 35,

    COMMENT      = 40,

    CONTAINER    = 50
  };

  static const _NameRec names[];
};
typedef ObjectTypes::EObjectType ObjectType;
typedef ObjectTypes OT;

#define DG_OBJECT( name ) public: \
  name* GetPtr() { IncCounter(); return this; } \
  private:
#define DG_SHARE( p ) (p == null) ? null : p->GetPtr()

class IObject
{
protected:
  long counter; // share ptr functionality;
  bool locked; // destructor protection;

  void IncCounter() { counter++; }
  bool DecCounter() { return (--counter) == 0; }

  void Lock() { locked = true; }
  void Unlock() { locked = false; }

public:
  IObject(): counter( 0 ), locked( false ) {}
  virtual ~IObject() {}
  virtual ObjectType Type() const = 0;
  /* optional */
  virtual std::string Description() const { return ""; }  /* Object name */
  virtual std::string ShortInfo() const { return ""; }    /* Main values */
  virtual std::string DetailedInfo() const { return ""; } /* Detailed description */

  IObject* GetPtr() { counter++; return this; }
  bool ReleasePtr() { if( !locked ) return false;
    assert( counter > 0 ); return DecCounter(); }
};

template< typename T >
void ReleaseObject( T** _ppObject );
void ReleaseObject( IObject* _pObject );

template <typename T > static void RestrictToType
( std::list< T* >& _crGrp, ObjectType _type ); /* RestrictGroupToType */

#include "IObject.t.hpp"

#endif // IOBJECT_H
