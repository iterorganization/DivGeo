#ifndef CHORDS_H
#define CHORDS_H

#include "../core/IComponent.h"
#include "../core/IVarItem.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

namespace Actions { class ActChangeChord; }

class Chord:
  public IComponent,
  public IVarItem,
  public IVarOrigin
{
  DG_OBJECT(Chord)

  Point p1, p2;

public:
  /* Constructors */
  Chord( ModelPtr _pModel, Point _p1, Point _p2, bool _bActiveTopView );
  ~Chord() { FreeVars(); FreeContainers(); }

  /* IComponent implementation */
  virtual void    Delete();
  virtual double  DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool    InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void    CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point   CentralPoint() const { return (p1 + p2) / 2.; }
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  int Change( Point _p1, Point _p2 );

  /* Advanced DistToPoint() */
  double DistToPoint( const Point& _crPnt, int* _pPosFlag, bool _topView, ulong _showFlags ) const;

  /* Extend chord to nearest element. See more code in dg2 sources. */
  int Extend(bool _topView);

  /* Getters */
  Point Point_1() { return p1; }
  Point Point_2() { return p2; }
  bool Is3D() const { return p1.z != 0. || p2.z != 0.; }
  int Length() const { return hypot( p2.x - p1.x, p2.y - p1.y ); }

  /* Setters */
  friend class Actions::ActChangeChord;
private:
  void SetPoints( const Point& _p1, const Point& _p2 ) { p1 = _p1; p2 = _p2; }
};

#endif
