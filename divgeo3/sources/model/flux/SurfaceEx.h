#ifndef SURFACEEX_H
#define SURFACEEX_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "CreatorId.h"
#include "EquilDecl.h"

struct SurfaceExParams
{
  Point origin;
  double level, zone;

  SurfaceExParams( Point _origin, double _level, double _zone ): origin( _origin ), level( _level ), zone( _zone ) { }
  void operator = ( const SurfaceExParams& _sxp ) { origin = _sxp.origin; level = _sxp.level; zone = _sxp.zone; }
  bool operator == ( const SurfaceExParams& _sxp ) { return origin == _sxp.origin && level == _sxp.level && zone == _sxp.zone; }
};

class SurfaceEx:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(SurfaceEx)
  DG_PROPERTIES { p_creator_id  = 0 };
  DG_PROPERTIES_TABLE { AddProperty( "creator_id", &crid ); }

  int zone,bDrawn,bCoordsOk;
  double level,nLevel;
  Point origin;
  CreatorId crid;
  int lastError;  /* NOT managed via Undo */

  int closed;
  PointArray line;

public:
  SurfaceEx( ModelPtr _pModel, int _zone, double _level, Point _origin );
  ~SurfaceEx();
  int Change( int _zone, double _level, int* pErr );
  int Change( Point pnt, int* pErr = null, bool bByXY = false );

  virtual void Delete();
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const { return origin; }

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  int RecalcPos();
  int InvalidatePos();

  /* Setters */
  int ChangeCreatorId( const CreatorId& _crid );
  void SetParameters( const SurfaceExParams& _sxp ) { origin = _sxp.origin; level = _sxp.level; zone = _sxp.zone; }

  /* Getters */
  bool HasLine()                  const { return line.size() != 0; }
  const PointArray* Line()        const { return &line; }
  bool IsDrawn()                  const { return bDrawn == 1 ? true : false; }
  bool CoordsOk()                 const { return bCoordsOk != 0; }
  int Zone()                      const { return zone; }
  double Level()                  const { return level; }
  const CreatorId& GetCreatorId() const { return crid; }
  Point Origin()                  const { return origin; }
  bool IsVirtual()                const { return zone == SZN_XY; }
  bool IsClosed() const;
  SurfaceExParams GetParameters() const { return SurfaceExParams( origin, level, zone ); }
};

struct SurfaceCompare
{
  double baseLevel;
  SurfaceCompare( double _baseLevel ): baseLevel( _baseLevel ) {}
  bool operator() ( const IComponentPtr p1, const IComponentPtr p2 ) const;
};

#endif
