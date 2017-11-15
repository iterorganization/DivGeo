#ifndef GRIDPOINTEX_H
#define GRIDPOINTEX_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "CreatorId.h"
#include "EquilDecl.h"

class GridPointEx:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(GridPointEx)
  DG_PROPERTIES { p_creator_id  = 0 };
  DG_PROPERTIES_TABLE { AddProperty( "creator_id", &crid ); }

  int zone,bDrawn,bCoordsOk;
  double value;
  Point position;
  Point derivative;
  CreatorId crid;

public:
  GridPointEx( ModelPtr _pModel, int _zone, double _value );
  ~GridPointEx();

  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const { return position; }

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
virtual std::string ShortInfo() const;

  int ChangeCreatorId( const CreatorId& _crCrid );
  int RecalcPos();

  virtual void Delete();
  int Change( int _zone, double _value );

  /* Setters */
  void SetZone( int _zone ) { zone = _zone; }
  void SetValue( int _value ) { value = _value; }

  /* Getters */
  int Zone()                      const { return zone; }
  double Value()                  const { return value; }
  bool CheckZone( int _zone )     const { return zone == _zone; }
  const CreatorId& GetCreatorId() const { return crid; }
  bool IsDrawn()                  const { return bDrawn == 1 ? true : false; }
  bool CoordsOk()                 const { return bCoordsOk != 0; }
  Point Position()                const { return position; }
  Point Derivative()              const { return derivative; }
};

bool GridPointExCompare( const IComponentPtr p1, const IComponentPtr p2 );

#endif
