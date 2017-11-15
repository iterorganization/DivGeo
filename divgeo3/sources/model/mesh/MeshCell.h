#ifndef MESHCELL_H
#define MESHCELL_H

#include "../core/IComponent.h"
#include "../core/IVarItem.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "MeshConstants.h"
#include "MeshDecl.h"

class MeshCell:
  public IComponent,
  public IValueHolder,
  public IVarItem
{
  DG_OBJECT(MeshCell)
  DG_PROPERTIES
  {
    p_center        = 0,
    p_field_ratio   = 1,
    p_center_backup = 2,
    p_backup_ratio  = 3
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "center", &center );
    AddProperty( "field_ratio", &ratio );
    AddProperty( "center_backup", &center_backup );
    AddProperty( "backup_ratio", &backupRatio );
  }

private:
  MeshPtr mesh;

  NPoint index;
  int eN,breakFlags;

  MeshPointPtr points[4];
  MeshCellPtr neighbours[4];
  MeshElementPtr elements[4];

  int cutFlags[4];

  Point center;
  Point center_backup;
  double ratio, backupRatio;

  mutable IComponentList points_ic;
  mutable IComponentList elements_ic;
  mutable IComponentList neighbours_ic;

public:
  static const ulong neighbours_count = 4;

  /* Constructors */
  MeshCell( ModelPtr _pModel, MeshPtr m, int _eN, NPoint _index,
    MeshPointPtr mp1, MeshPointPtr mp2, MeshPointPtr mp3, MeshPointPtr mp4,
    Point _center, double rat );
  ~MeshCell();

  /* IComponent implementation */
  virtual double  DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
  { UNUSED(_pPosFlag); return hypot( _crPnt.x - center.x, _crPnt.y - center.y ); }
  virtual bool    InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void    CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point   CentralPoint() const { return center; }
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Getters */
  bool PointInside( Point point, bool bUseBackups ) const;
  bool PointInside2( Point point, bool bUseBackups ) const; // DG3
  int IsIrregular() const;
  bool IsChanged()  const;
  MeshPointPtr   MPoint(int i)     const { return ( i < 0 || i > 3 ) ? NULL : points[i]; }
  MeshCellPtr    Neighbour(int i)  const { return ( i < 0 || i > 3 ) ? NULL : neighbours[i]; }
  MeshElementPtr Element(int i)    const { return ( i < 0 || i > 3 ) ? NULL : elements[i]; }
  int CutFlag(int i)    const { return ( i < 0 || i > 3 ) ? 0 : cutFlags[i]; } //TODO: error return
  int Nx()              const { return index.x; }
  int Ny()              const { return index.y; }
  int eNumber()         const { return eN; }
  Point Center()        const { return center; }
  Point BackupCenter()  const { return center_backup; }
  NPoint Index()        const { return index; }
  double Ratio()        const { return ratio; }
  double BackupRatio()  const { return backupRatio; }

  const IComponentList& ConnectedElements() const;
  const IComponentList& NeighbourCells() const;
  const IComponentList& ConnectedPoints() const;

  /* Setters */
  void SetCutFlag(int cf, int i);
  void SetNeighbor(MeshCellPtr mc, int i);
  void SetElement( MeshElementPtr me, int i );

  // Force exclude object without releasing
  void ExcludeCell( MeshCellPtr _pMC );
  void ExcludeElement( MeshElementPtr _pME );
  void ExcludePoint( MeshPointPtr _pMP );

  /* High-level actions */
  int RecalculateCenter();

  /* Mid-level actions */
  void SetCenter( Point _center );
  void SetFieldRatio( double _ratio );
  int SetBackups( Point _center, double _ratio );
};


#endif
