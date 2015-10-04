#ifndef SOURCES_H
#define SOURCES_H

#include "../core/IComponent.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

class Source:
  public IComponent,
  public IVarOrigin,
  public IVarItem
{
  DG_OBJECT(Source)

  Point position;

public:
  /* Constructors */
  Source( ModelPtr _pModel, const Point& _crPosition ):
    IComponent( OT::SOURCE, _pModel ),
    position( _crPosition ) { }
  ~Source() { FreeVars(); FreeContainers(); }

  /* IComponent implementation */
  virtual void        Delete();
  virtual double      DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool        InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void        CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point       CentralPoint() const { return position; }
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  int Change( const Point& _crPosition );

  /* Getters */
  const Point& Position() const { return position; }
  double X()              const { return position.x; }
  double Y()              const { return position.y; }

  /* Setters */
  void SetPosition( Point _position ) { position = _position; }
};

#endif
