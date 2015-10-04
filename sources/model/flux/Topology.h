#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "XPointTest.h"
#include "XPointSeg.h"
#include "GridPointSeg.h"
#include "SurfaceZone.h"

class Model;

class Topology:
    public IComponent,
    public IManager
{
  DG_OBJECT(Topology)
  DG_HIERARCHY_TABLE {
    RegisterType( OT::XPOINTTEST,   &xPointTests );
    RegisterType( OT::XPOINTSEG,    &xPointSegs );
    RegisterType( OT::GRIDPOINTSEG, &gridPointSegs );
    RegisterType( OT::SURFACEZONE,  &surfaceZones );
  }

  EquilPtr pEquil;

  std::string sFileName;

  IComponentList xPointTests;
  IComponentList xPointSegs;
  IComponentList gridPointSegs;
  IComponentList surfaceZones;

  Point ehint1, ehint2;

public:
  Topology( ModelPtr _pModel );
  virtual ~Topology();

  void SetEquil( EquilPtr _pEquil );
  void SetFileName( const std::string& _crsFileName ) { sFileName = _crsFileName; }

  /*****************************************************************************
   * IComponent implementation
   *****************************************************************************/

  virtual void Delete();

  /*****************************************************************************
   * Read / Write
   *****************************************************************************/
public:
  /* Import topology from *.dg file. Use ERR_NOEQUIL if Equil is null */
  int Import( const std::string& _crsFileName, bool _detectXPoints, int* _pErrFlags );

  /* Extracted from Model::WriteOutputFile */
  void OutputPoints( FILE* _pFile ) const;

  /* IManager implementation */
  virtual int ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 );
  virtual int WriteObjects(Token _token, FILE* _pFile, int /*_mode*/= 0 ) const;

  /*****************************************************************************
   * Common topology
   *****************************************************************************/
public:
  void            SortZonesAndSegments();
  int             FindSurfaceOriginPointEx( int _zone, double _level, double* _px, double* _py ) const;

  void            RecalcCache( int *_pErr = null );
  void            InvalidateCache();

  /*****************************************************************************
   * GridPointSegments
   *****************************************************************************/
public:
  GridPointSegPtr AddGridPointSeg( XPointSegPtr _pXPS, int _zone );
  GridPointSegPtr FindGridPointSeg( int _zone ) const;
  GridPointSegPtr FindGridPointSegBySegment( XPointSegPtr _pXPS ) const;

  /* Change line length and move points according to target elements */
  void            RecalcAllGridPointSegSLines(); // Used after each GPS/XPS change

  bool            UpdateSegment( GridPointSegPtr _pGPS, double _newLength ); /* Forwarded */

  /* Find GPS (_pZone) nearest to position (_crPos). _pValue - path to nearest point */
  int             HitGridPointPosition( const Point& _crPos, int* _pZone, double* _pValue ) const;
private:
  int             NextGridPointSegNumber() const; // Used in AddXPointSeg() only

  /*****************************************************************************
   * SurfaceZones
   *****************************************************************************/
public:
  SurfaceZonePtr  AddSurfaceZone( int _szNo, int _gpsNo1, int _gpsNo2, int _orient, ElementPtr _pElem );
  SurfaceZonePtr  FindSurfaceZone( int _zone ) const;
  bool            HasDuplicatedZones( int _zone, SurfaceZonePtr _pSZ ) const;
  int             GetSurfaceZoneByXY( const Point& _crPnt, double* _pLevel, int* _pErr ) const;
  bool            SurfaceZoneBoundsCached() const;
  void            RecalcAllSurfaceZonesSignes() const;
  int             GetCarreSurfaceExCount( int _zone, int _nSpec ) const;
private:
  int             SurfaceZoneLevelOk( SurfaceZonePtr _pSZ, double _level ) const; // Used in GetSurfaceZoneByXY() only
  int             CheckSurfaceZoneData( int _szNo, int _gpsNo1, int _gpsNo2, int _sign) const; // Is about GPS. Unused
  int             RecalcSurfaceZoneSign( SurfaceZonePtr _pSZ ) const; /* Uses GPS*/

  /*****************************************************************************
   * XPoint (XPointTest)
   *****************************************************************************/
public:
  XPointTestPtr   AddXPointTest( const NPoint& _crC1, const NPoint& _crC2,
                                 double _level, int _id  );
private:
  int             FindXPointCenter( XPointTestPtr _pXPT ) const; // Used in AddXPointTest() only
  XPointTestPtr   FindXPointTest( int _id ) const;
  int             CheckXPointRect( XPointTestPtr _pXPT ) const;
  int             CheckXPointLevels( const NPoint& _crC1, const NPoint& _crC2,
                                     const NPoint& _crP0, int _bMinMax ) const;
public:
  int             DetectXPoints();
private: /* Returns a list of rectangles surrounding X points */
  XPointTestList  FindXPointRects() const; // Used in DetectXPoints() only

public:
  void            CalcXPointGradients( XPointTestPtr _pXPT ) const; // Extracted from AddXPointTest::Do()


  /*****************************************************************************
   * Separatrix (XPointSeg)
   *****************************************************************************/
public:
  XPointSegPtr    AddXPointSeg( XPointTestPtr _pXPT, int _number, double _sp, double _ep );
  void            DeleteXPointSeg( XPointSegPtr _pXPS );
  int             DetectXPointSegs(); // Uses XPT
private:
  void            DeleteDuplicateXPointSegs(); // Used in DetectXPointSegs() only

public: /* Separatrix line calculation stack */
  void            CalcSeparatrixLine( XPointTestPtr _pXPT, PointArray* _pLine, int _idx ) const; // Used by XPS only
private:
  /* Create a spline with CalcSurfaceLine() method with additional points */
  int             CreateSurfaceLine( const NPoint& _crCell, double _level,
                                     PointArray* _pLine, bool &_rIsClosed ) const;
  /* Find a cell that contains the proper "surface" */
  int             FindSurfaceCell( const NPoint& _crC1, const NPoint& _crC2,
                                   double _level, int _idx, NPoint& _rCell ) const;

private:
  int             CalcXPointSegNumber( XPointSegPtr _pXPS, XPointTestPtr _pXPT ) const; // Unused

public:
  bool            HasDuplicatedSegments( int _zone, GridPointSegPtr _pGPS ) const;

  /*****************************************************************************
   * Utilities
   *****************************************************************************/
public:
  IComponentPtr GetLockingObject();

  /*****************************************************************************
   * Get
   *****************************************************************************/
public:
  const std::string& FileName() const { return sFileName; }

  bool HasSurfaceZones()  const { return !surfaceZones.empty(); }
  bool HasGridPointSegs() const { return !gridPointSegs.empty(); }

  const IComponentList& XPointTests()    const { return xPointTests; }
  const IComponentList& XPointSegs()     const { return xPointSegs; }
  const IComponentList& GridPointSegs()  const { return gridPointSegs; }
  const IComponentList& SurfaceZones()   const { return surfaceZones; }

};

typedef Topology* TopologyPtr;

#endif // TOPOLOGY_H
