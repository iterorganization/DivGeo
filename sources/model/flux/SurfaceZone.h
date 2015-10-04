#ifndef SURFACEZONE_H
#define SURFACEZONE_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "EquilDecl.h"
#include "../material/Element.h"

class SurfaceZone:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(SurfaceZone)
  DG_PROPERTIES
  {
    p_short_name  = 0,
    p_long_name   = 1,
    p_flags       = 2
  };
  DG_PROPERTIES_TABLE
  {
    AddProperty( "short_name", &shortName );
    AddProperty( "long_name", &longName );
    AddProperty( "flags", &flags );
  }

  int zone;
  std::string shortName;
  std::string longName;
  int gpZone1,gpZone2,zone_sign,orient;
  long flags;
  double level1,level2;
  PointArray bounds;
  ElementPtr innermost;

public:
  SurfaceZone( ModelPtr _pModel, int _szNo, int _gpsNo1, int _gpsNo2, int _orient, ElementPtr _pInner );
  ~SurfaceZone();

  virtual void Delete();
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const
  { UNUSED(_pMin); UNUSED(_pMax); return; }

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  double CalcDistToBounds( Point pnt ) { return PointToPolyLineDist( bounds, pnt ); }

  /* Actions */
  void SetShortName( const std::string& _crsShortName );
  void SetLongName( const std::string& _crsLongName );
  void SetFlags( long _flags );

  /* Getters */
  bool HasBounds()            const { return !bounds.empty(); }
  bool HasFlag( int _flags )  const { return (flags & _flags) != 0; }
  bool IsLimited()            const { return HasAnyFlag( flags, SZF::LIMITBYSURFACE ); }
  bool CheckZone( int _zone ) const { return zone == _zone; }
  int GPZone( unsigned n )    const { assert( n == 1 || n == 2 ); return n == 1 ? gpZone1 : gpZone2; }
  double Level1()             const { return level1; }
  double Level2()             const { return level2; }
  int Sign()                  const { return zone_sign; }
  int Zone()                  const { return zone; }
  int Orient()                const { return orient; }
  long Flags()                const { return flags; }
  ElementPtr Innermost()      const { return innermost; }
  bool HasInnermost()         const { return innermost != null; }
  const std::string& ShortName()  const { return shortName; }
  const std::string& LongName()   const { return longName; }

  /* Setters */
  void FreeBounds() { bounds.clear(); }
  void SetBounds( const PointArray& _bounds ) { bounds = _bounds; }
  void SetSign( int sign );
  void SetInnermost( ElementPtr _pElem );
};

bool SurfaceZoneCompare( const IComponentPtr p1, const IComponentPtr p2 );

#endif
