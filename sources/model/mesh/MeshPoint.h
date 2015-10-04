#ifndef MESHPOIN_H
#define MESHPOIN_H

#include "../../common/Includes.h"
#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../core/CoreConstants.h"
#include "MeshConstants.h"

#include "MeshDecl.h"

class MeshPoint:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(MeshPoint)
  DG_PROPERTIES { p_backup_coords = 0 };
  DG_PROPERTIES_TABLE { AddProperty( "backup_coords", &backup ); }

  MeshPtr pMesh;

  MeshCellPtr cells[4];
  MeshElementPtr elements[4];

  Point position;
  Point backup;

  MeshPointPtr neighbours[4];

  int cutFlag;
  NPoint index;

  mutable IComponentList cells_ic;
  mutable IComponentList elements_ic;
  mutable IComponentList neighbours_ic;
  
public:
  static const ulong neighbours_count = 4;

  /* Constructors */
  MeshPoint( ModelPtr _pModel, MeshPtr m, Point _position );
  ~MeshPoint();

  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
  { UNUSED(_pPosFlag); return hypot( _crPnt.x - position.x, _crPnt.y - position.y ); }
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const { return position; }

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  void HighlightDrag( bool include = true );
  /* Also highlight upper/lower neighbours, if on double line */
  IComponentList HighlightDragV( bool include = true );
  /* Also highlight left/right neighbours, if on double line */
  IComponentList HighlightDragH( bool include = true );
  /* Also move upper/lower neighbors, if on double line */
  void MoveV( const Point& _crPos );
  /* Also move left/right neighbors, if on double line */
  void MoveH( const Point& _crPos );

  MeshPointPtr OutermostPoint();
  MeshPointList GetChain( bool stopOnCuts );
  double DistanceTo( MeshPointPtr pMP );

  /* Getters */
  Point Backup()    const { return backup; }
  Point Position()  const { return position; }
  NPoint Index()    const { return index; }
  double X()        const { return position.x; }
  double Y()        const { return position.y; }
  MeshCellPtr     Cell( unsigned int i )      const { assert( i < 4 ); return cells[i]; }
  MeshElementPtr  Element( unsigned int i )   const { assert( i < 4 ); return elements[i]; }
  MeshPointPtr    Neighbour( unsigned int i ) const { assert( i < 4 ); return neighbours[i]; }
  int CutFlag()     const { return cutFlag; }
  int Locks()       const { return locks; }

  bool IsOnBorder() const;
  bool IsChanged() const { return position != backup; }

  const IComponentList& ConnectedElements() const;
  const IComponentList& ConnectedCells() const;
  const IComponentList& NeighbourPoints() const;


  /* Setters */

  // Force exclude object without releasing
  void ExcludeCell( MeshCellPtr _pMC ) {
    for( uint c = 0; c < 4; c++ )
      if( cells[c] != null && cells[c] == _pMC )
        ReleaseObject( (IComponentPtr*)&cells[c] );
  }

  void ExcludeElement( MeshElementPtr _pME ) {
    for( uint c = 0; c < 4; c++ )
      if( elements[c] != null && elements[c] == _pME )
        ReleaseObject( (IComponentPtr*)&elements[c] );
  }

  void ExcludePoint( MeshPointPtr _pMP ) {
    for( uint c = 0; c < 4; c++ )
      if( neighbours[c] != null && neighbours[c] == _pMP )
        ReleaseObject( (IComponentPtr*)&neighbours[c] );
  }

  void SetCell     ( MeshCellPtr mc, int i );
  void SetElement  ( MeshElementPtr me, int i );
  void SetNeighbor ( MeshPointPtr mp, int i);
  void SetCutFlag  ( int cf )     { cutFlag = cf; }
  void SetIndex    ( NPoint _index ) { index = _index; }
  void SetPosition ( Point _position ) { position = _position; }

  /* Mid-level actions */
  int Change( Point _position );
  int SetBackupCoords( Point coords );

};

#endif
