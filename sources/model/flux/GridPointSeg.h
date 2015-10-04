#ifndef GRIDPOINTSEG_H
#define GRIDPOINTSEG_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "EquilDecl.h"

class Topology;
typedef Topology* TopologyPtr;

/* $ TEMP - non-undoable */
class GridPointSeg:
  public IComponent,
  public IValueHolder
{ 
  DG_OBJECT(GridPointSeg)
  DG_PROPERTIES
  {
    p_short_name  = 0,
    p_long_name   = 1,
    p_zone        = 2,
    p_dir         = 3,
    p_target_dir  = 4,
    p_flags       = 5
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "short_name",  &shortName );
    AddProperty( "long_name",   &longName );
    AddProperty( "zone",        &zone );
    AddProperty( "dir",         &dir );
    AddProperty( "target_dir",  &targetDir );
    AddProperty( "flags",       &flags );
  }

  int zone,dir,targetDir/*Kill*/;
  std::string shortName;
  std::string longName;
  XPointSegPtr pXPS;
  long flags;
  double lineLength,level;
  PointArray* pLine;

public:
  GridPointSeg( ModelPtr _pModel, XPointSegPtr _pXPS, int _zone );
  ~GridPointSeg();

  virtual void Delete();
  double DistToPointEx( Point pnt, double* pos ) const;
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  int CalcPointPosition( double value, Point* pPosition, Point* pDerivative ) const;

  /* Actions */
  void SetZone( int _zone );
  void SetDir( int _dir );
  void SetTargetDir( int _targetDir );
  void SetShortName( const std::string& _shortName );
  void SetLongName( const std::string& _longName );
  void SetFlags( long _flags );

  /* Getters */
  bool IsUsed()               const { return HasAnyFlag( flags, GPSF::USED ); }
  bool HasFlag( long _flag )  const { return (flags & _flag) != 0; }
  long Flags()                const { return flags; }
  bool HasLine()              const { return pLine != NULL; }
  bool CheckZone( int _zone ) const { return zone == _zone; }
  int Dir()                   const { return dir; }
  int TargetDir()             const { return targetDir; }
  int Zone()                  const { return zone; }
  double Level()              const { return level; }
  double GetXpsLevel() const;
  Point GetMiddlePoint()          const { return (*pLine)[ pLine->size()/2 ]; }
  Point GetPoint( unsigned i )    const { assert( i < pLine->size() ); return (*pLine)[i]; }
  XPointSegPtr XPS()              const { return pXPS; }
  int LineSize()                  const { return pLine->size(); }
  double LineLength()             const { return lineLength; }
  const std::string& ShortName()  const { return shortName; }
  const std::string& LongName()   const { return longName; }

  /* Setters */
  void SetLine( void* p = (void*)1 );
  void UpdateLineLength();
};

bool GridPointSegCompare( const IComponentPtr p1, const IComponentPtr p2 );

#endif
