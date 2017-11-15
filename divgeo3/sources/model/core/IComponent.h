#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "CoreConstants.h"
#include "IObject.h"
#include "../../common/Macroes.h"
#include "../../common/Resources.Msg.h"
#include "../../common/Constants.h"
#include "../../common/Point.h"
#include "../flux/CreatorId.h"

#define null NULL
#define UNUSED(x) (void)(x)
#define dgtype_cast static_cast //dynamic_cast
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

class IComponent;
DG_DEFINE_PTR(IComponent)
DG_DEFINE_LIST(IComponent)

class Model;
typedef Model* ModelPtr;

// Base common component class
class IComponent:
    virtual public IObject
{
  DG_OBJECT(IComponent)

protected:
  ModelPtr pModel;
  ObjectType type;
  int locks;

public:
  IComponent( ObjectType _type, ModelPtr _pModel ):
    pModel( _pModel ), type( _type ), locks( 0 ) {}
  virtual ~IComponent() {}
  virtual ObjectType Type() const { return type; }

  // Common methods
  bool IsLocked() const { return !!locks; }
  bool HasModel( ModelPtr _pModel ) const { return _pModel == pModel; }
  ModelPtr GetModel() const { return pModel; }
  bool IsHighlighted(); // defined in Model.cpp
  bool IsMarked();
  void Mark( bool _enable = true );
  void Highlight( bool _enable = true );

  // IObject implementation
  virtual std::string Description() const { return ""; }
  virtual std::string ShortInfo() const { return ""; }
  virtual std::string DetailedInfo() const { return ""; }

  // Optional methods
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
  { UNUSED(_crPnt); UNUSED(_pPosFlag); throw "not implemented"; return MAXDOUBLE; }
  virtual bool InRectangle( const Point& p1, const Point& p2 ) const
  { UNUSED(p1); UNUSED(p2); throw "not implemented"; return false; }
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const
  { UNUSED(_pMin); UNUSED(_pMax); throw "not implemented"; }
  virtual Point CentralPoint() const { throw "not implemented"; }
  virtual void Delete() { throw "not implemented"; }
};

DG_DEFINE_PTR(IComponent)
DG_DEFINE_LIST(IComponent)

#ifndef CURRENTITER
  #define CURRENTITER dg__it__
  #define ENDITER dg__it_end__
#endif

/* Use "IComponentPtr FOREACHOBJ( ptr, grp )" construction for ptr declaration */
#ifndef FOREACHOBJ
  #define FOREACHOBJ( ptr, grp ) \
    ptr = (grp).front(); \
    for( IComponentIter CURRENTITER = (grp).begin();\
          CURRENTITER != (grp).end(); \
          ++CURRENTITER, ptr = (CURRENTITER != (grp).end() ) ? (*CURRENTITER ) : null )

  #define FOREACHOBJCONST( ptr, grp ) \
    ptr = (grp).front(); \
    for( IComponentIterConst CURRENTITER = (grp).begin(), ENDITER = (grp).end();\
          CURRENTITER != ENDITER; \
          ++CURRENTITER, ptr = (CURRENTITER != ENDITER ) ? (*CURRENTITER ) : null )
#endif

#ifndef FOREACHPTR
  #define FOREACHPTR( type, ptr, grp ) \
    type ptr = dgtype_cast< type >( (grp).empty() ? null : (grp).front() ); \
    for( IComponentIter CURRENTITER = (grp).begin();                      \
          CURRENTITER != (grp).end();                                          \
          ++CURRENTITER, ptr = (CURRENTITER != (grp).end() ) ? dgtype_cast< type >( *CURRENTITER ) : null )

  #define FOREACHPTRCONST( type, ptr, grp ) \
    type ptr = dgtype_cast< type >( (grp).empty() ? null : (grp).front() ); \
    for( IComponentIterConst CURRENTITER = (grp).begin(), ENDITER = (grp).end(); \
          CURRENTITER != ENDITER;                                          \
          ++CURRENTITER, ptr = (CURRENTITER != ENDITER ) ? dgtype_cast< type >( *CURRENTITER ) : null )
#endif

#endif
