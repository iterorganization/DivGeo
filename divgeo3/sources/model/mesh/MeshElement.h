#ifndef MESHELEMENT_H
#define MESHELEMENT_H

#include "../core/IComponent.h"
#include "../core/IVarItem.h"
#include "../core/CoreConstants.h"

#include "MeshConstants.h"
#include "MeshDecl.h"

class MeshElement:
  public IComponent,
  public IVarItem
{
  DG_OBJECT(MeshElement)

  MeshPtr mesh;
  MeshPointPtr points[2];
  MeshCellPtr cells[2];

  int cutFlag;
  bool bVertical;

  mutable IComponentList points_ic;
  mutable IComponentList cells_ic;

public:
  static const ulong neighbours_count = 2;

  /* Constructors */
  MeshElement( ModelPtr _pModel, MeshPtr m, MeshPointPtr mp1, MeshPointPtr mp2, bool isVertical );
  ~MeshElement();

  /* IComponent implementation */
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const;
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;
  
  /* Getters */
  MeshPointPtr MPoint(int i)  { return ( i < 0 || i > 1 ) ? NULL : points[i]; }
  MeshCellPtr  Cell(int i)   { return ( i < 0 || i > 1 ) ? NULL : cells[i]; }
  int  CutFlag()     { return cutFlag; }
  bool IsVertical()  { return bVertical; }

  int GetID_1() const;
  int GetID_2() const;

  const IComponentList& ConnectedPoints() const;
  const IComponentList& ConnectedCells() const;

  /* Setters */
  void SetCell( MeshCellPtr mc, int i );
  void SetCutFlag (int cf) { cutFlag = cf; }

  // Force exclude object without releasing
  void ExcludeCell( MeshCellPtr _pMC );
  void ExcludePoint( MeshPointPtr _pMP );

};

#endif
