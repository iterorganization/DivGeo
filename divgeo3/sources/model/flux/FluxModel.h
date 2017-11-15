#ifndef FLUXMODEL_H
#define FLUXMODEL_H

//#include "../core/Model.h"

#include "Equil.h"
#include "Topology.h"
#include "SurfaceEx.h"
#include "GridPointEx.h"

class Model;
typedef Model* ModelPtr;

namespace Actions {
class ActAddEquil;
class ActDelEquil;
class ActAddTopology;
class ActDelTopology;
}

class FluxModel:
    public IManager
{
  DG_HIERARCHY_TABLE {
    //RegisterType( OT::EQUIL,        pFlux );
    RegisterType( OT::XPOINTTEST,   pTopology );
    RegisterType( OT::XPOINTSEG,    pTopology );
    RegisterType( OT::GRIDPOINTSEG, pTopology );
    RegisterType( OT::SURFACEZONE,  pTopology );

    RegisterType( OT::GRIDPOINTEX,  &gridPointsEx );
    RegisterType( OT::SURFACEEX,    &surfacesEx );
  }

  ModelPtr pModel;

  // Equilibrium mesh
  EquilPtr pEquil;

  // Editable topology created by user, imported or the same as native topology
  TopologyPtr pTopology;
  // Constant topology loaded from main .dg file, or saved to it after editing
  TopologyPtr pTopology_native;

  // Contours of constant poloidal flux
  IComponentList surfacesEx;
  // Points on the separatrix
  IComponentList gridPointsEx;

  mutable IComponentList surfacesEx_virtual;

  mutable IComponentList surfaceZones_sorted; // Used by
  mutable IComponentList gpSegments_sorted;   //   output methods

  mutable IComponentList surfacesEx_selected;   // Used by
  mutable IComponentList gridPointsEx_selected; //   zone specified getters

public:
  FluxModel( ModelPtr _pModel );
  virtual ~FluxModel();

  virtual int     ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 );
  virtual int     WriteObjects( Token _token, FILE* _pFile, int _mode = 0 ) const;

  EquilPtr        GetEquil() const { return pEquil; }
  bool            HasEquil() const { return pEquil != null; }
  ulong           AddEquil( const char* _fName );
private:
  friend class Actions::ActAddEquil;
  friend class Actions::ActDelEquil;
  void            SetEquil( EquilPtr _pEquil );

public:
  TopologyPtr     GetTopology() const { return pTopology; }
  bool            HasTopology() const { return pTopology != null; }
  int             AddTopology( const std::string& _crsFileName = std::string(), bool _detectXPoints = true );
private:
  friend class Actions::ActAddTopology;
  friend class Actions::ActDelTopology;
  void            SetTopology( TopologyPtr _pTopology );

public:
  const IComponentList& GridPointsEx() const { return gridPointsEx; }
  const IComponentList& SurfacesEx()   const { return surfacesEx; }

  const IComponentList& SurfacesEx( int _zone ) const; // Usez SZ
  const IComponentList& GridPointsEx( int _zone ) const; // Uses GPS

  bool               HasVirtualSurfaces() const;
  const IComponentList& VirtualSurfaces() const;

  void            ProcessChanges( ulong _changeFlags );
  /* Recalc all surfaces and gridPoints */
  void            UpdateAfterTopologyChange();

  /*****************************************************************************
   * Surfaces (toroidal)
   *****************************************************************************/
public:
  SurfaceExPtr    AddSurfaceEx( int _zone, double _level, int* _pErr = null );
  SurfaceExPtr    AddSurfaceEx( const Point& _crPnt, int* _pErr = null, bool _bByXY = false );
  int             CountSurfaces( int _area ) const;

  /* Find SX by area and level ar by origin, return null if not found. */
  SurfaceExPtr    FindSurface( int _area, double _level ) const;
  SurfaceExPtr    FindSurface( const Point& _crOrigin ) const;
  /* Return # of radial cells */
  std::string     GetSurfacesStatsStr() const; // Uses SZ
  /* Used only from Read[Old]DgFile. Converts XY-Surfaces into regular ones using "old" topology */
  void            ConvertOldDgSurfaces( int* _pErrorFlags );
  /* Remove all virtual surfaces in zone except _pSX_NonVirtual */
  IComponentList  RemoveVirtualSurfaces( SurfaceExPtr _pSX_NonVirtual );
  /* Remove all surfaces with matching creator excepting 'unchanged' flag and return number of removed surfaces */
  int             RemoveSurfacesByCreatorId( const CreatorId& _crCrId, IComponentListPtr _vSX_removed = null );

  /* Carre Surfaces calculation stack */
  int             RebuildCarreSurfaces( int _area );
  int             DistributeSurfaces( const CreatorId& _crCrId, CreatorId* _pOldCrid );
private:
  int             FindCarreMinMaxSurfaceLevel( int _nArea, double* _pL1, double* _pL2, int* _bOuterSurfaceNeeded ) const;
  int             FindMinMaxSegment( int _signMinMax, Point _p1, Point _p2, double* _pLevel ) const;

  /* Utilities */
public:

  /*****************************************************************************
   * GridPoints (poloidal)
   *****************************************************************************/
public:
  GridPointExPtr  AddGridPointEx( int _zone, double _value );
  int             CountGridPoints( int _area ) const;

  /* Calc GPX number on its segment */
  int             GetGridPointExNumber( GridPointExPtr _pGPX ) const;
  /* Find GPX by area and value, return null if not found. */
  GridPointExPtr  FindGridPoint( int _area, double _value ) const;
  /* Return # of poloidal cells */
  std::string     GetGridPointStatsStr() const; // Uses GPS
  /* Remove all GPX on segment */
  void            RemoveGridPointsByArea( int _area );

  /* Carre GridPoints calculation stack */
  int             RebuildCarreGridPoints( int _area );
  int             DistributeGridPoints( const CreatorId& _crCrId );

  /* Utilities */
public:
  /* Set new length of GPS and recalculate its GPXs. Extracted from RecalcGridPointSegLine. Redraw required. */
  bool            UpdateSegment( GridPointSegPtr _pGPS, double _newLength );

  /*****************************************************************************
   * Targets output
   *****************************************************************************/

public:
  int             WriteTargetsFile( FILE* _pFile ) const;
private:
  void            WriteTargets_Surfaces( FILE* _pFile ) const;
  void            WriteTargets_GridPoints( FILE* _pFile ) const;
  /* Make sure surfaces are intact */
  int             Carre_CheckSurfaces(/*out*/IComponentList& _rCreators_SX ) const;
  /* Make sure grid points are intact */
  int             Carre_CheckGridPoints(/*out*/IComponentList& _rCreators_GPX ) const;
  /* Output Carre values for surfaces */
  void            Carre_WriteSurfaces(FILE* _pFile, const IComponentList& _crCreators_SX ) const;
  /* Output the position of the innermost surface */
  int             Carre_WriteInnermostSurface( FILE* _pFile ) const;
  /* Output Carre values for grid points */
  void            Carre_WriteGridPoints(FILE* _pFile, const IComponentList& _rCreators_GPX ) const;

};

typedef FluxModel* FluxModelPtr;

#endif // FLUXMODEL_H

#include "../core/Model.h"
