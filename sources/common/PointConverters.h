#ifndef VIEWMODELUTILITIES_H
#define VIEWMODELUTILITIES_H

#include <math.h>
#include <QPoint>
#include <QLineF>
#include "../common/Point.h"

/* Some point math operators and cast functions */
const QPointF operator*( const QPointF& p1, const QPoint&  p2 );
//const QPointF operator*( const QPointF& p1, const QPointF& p2 );
const QPointF operator*( const QPointF& p1, const Point&   p2 );

const QPointF operator/( const QPointF& p1, const QPoint&  p2 );
//const QPointF operator/( const QPointF& p1, const QPointF& p2 );
const QPointF operator/( const QPointF& p1, const Point&   p2 );

const QPointF operator+( const QPointF& p1, const Point&   p2 );
const QPoint  operator+( const QPoint&  p1, const Point&   p2 );
const QPointF operator+( const QPointF& p1, const QPointF& p2 );
const Point   operator+( const Point&   p1, const QPointF& p2 );

const QPointF operator-( const QPointF& p1, const Point&   p2 );
const QPoint  operator-( const QPoint&  p1, const Point&   p2 );
const QPointF operator-( const QPointF& p1, const QPointF& p2 );
const Point   operator-( const Point&   p1, const QPointF& p2 );

const QPoint Min( const QPoint& p1, const QPoint& p2 );
const QPoint Max( const QPoint& p1, const QPoint& p2 );

const QPointF Min( const QPointF& p1, const QPointF& p2 );
const QPointF Max( const QPointF& p1, const QPointF& p2 );

const QPoint QPointRound( const QPointF& pnt );

class Convert
{
  Point p;
public:
  Convert( const QPoint& pnt ): p( pnt.x(), pnt.y(), 0.) { }
  Convert( const QPointF& pnt ): p( pnt.x(), pnt.y(), 0.) { }
  Convert( const Point& pnt ): p( pnt ) { }
  Convert( const NPoint& pnt ): p( pnt.x, pnt.y, 0. ) { }

  operator Point() { return p; }
  operator NPoint() { return NPoint( floor( p.x + 0.5 ), floor( p.y + 0.5 ) ); }
  operator QPointF() { return QPointF( p.x, p.y ); }
  operator QPoint() { return QPoint( floor( p.x + 0.5 ), floor( p.y + 0.5 ) ); }

  Point   toPoint() { return p; }
  NPoint  toNPoint() { return NPoint( floor( p.x + 0.5 ), floor( p.y + 0.5 ) ); }
  QPointF toQPointF() { return QPointF( p.x, p.y ); }
  QPoint  toQPoint() { return QPoint( floor( p.x + 0.5 ), floor( p.y + 0.5 ) ); }
};

typedef Convert _C;

static inline QLineF QLineFromPoints( Point p1, Point p2 ) { return QLineF( p1.x, p1.y, p2.x, p2.y ); }

void Rotate( double angle_rad, Point center, Point* point );

#endif
