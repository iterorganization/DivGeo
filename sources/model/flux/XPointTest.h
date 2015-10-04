#ifndef XPOINTTEST_H
#define XPOINTTEST_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "EquilDecl.h"

struct XPointMinMax
{
  NPoint p;
  int t;
  double lvl;
  XPointMinMax(): t( 0 ), lvl( 0. ) { }
  XPointMinMax( NPoint _p, int _t, double _lvl ): p( _p ), t( _t ), lvl( _lvl ) { }
};

class XPointTest:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(XPointTest)
  DG_PROPERTIES { p_id  = 0 };
  DG_PROPERTIES_TABLE { AddProperty( "id", &id ); }

  NPoint c1, c2;
  Point center;
  double lvlMin,lvlMax,level;
  XPointMinMax minMax[4];
  XPointSegList segs;
  IComponentList segs_ic;
  PointArray* gradients[4];

  int id;

public:
  XPointTest( ModelPtr _pModel, NPoint _c1, NPoint _c2, double _level, int _id );
  XPointTest( const XPointTest& _crOther );
  ~XPointTest();

  virtual void Delete();
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
  { UNUSED(_pPosFlag); return hypot( _crPnt.x - center.x, _crPnt.y - center.y ); }
  virtual Point CentralPoint() const { return center; }

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  void IncludeXPointSeg( XPointSegPtr _pXPS );
  void ExcludeXPointSeg( XPointSegPtr _pXPS );
  bool HasXPointSeg( XPointSegPtr _pXPS );

  IComponentPtr GetLockingObject();

  void FreeArrays();

  void CalcLevel() { level = (lvlMin + lvlMax) / 2.; }

  /* Getters */
  int ID()                const { return id; }
  double Level()          const { return level; }
  NPoint EndPos( ulong i )const { assert( i == 1 || i == 2 ); return (i == 1) ? c1 : c2; }
  Point Center()          const { return center; }
  NPoint MinMax( ulong i )        const { assert( i < 4 ); return minMax[i].p; }
  double MinMaxLevel( ulong i )   const { assert( i < 4 ); return minMax[i].lvl; }
  double MinMaxT( ulong i )       const { assert( i < 4 ); return minMax[i].t; }
  PointArray* Gradient( ulong i ) const { assert( i < 4 ); return gradients[i]; }
  const IComponentList& Segments();

  /* Setters */
  void SetMinMax( XPointMinMax _minMax[4] );
  void SetCenter( Point _center ) { center = _center; }
  void SetGradient( unsigned i, PointArray* gradient ) { assert( i < 4 ); gradients[i] = gradient; }


};

#endif
