#include "PointConverters.h"

//const QPoint operator*( const QPoint& p1, const QPoint& p2 ) { return QPoint( p1.x() * p2.x(), p1.y() * p2.y() ); }
const QPointF operator*( const QPointF& p1, const QPoint&  p2 ) { return QPointF( p1.x() * p2.x(), p1.y() * p2.y() ); }
//const QPointF operator*( const QPointF& p1, const QPointF& p2 ) { return QPointF( p1.x() * p2.x(), p1.y() * p2.y() ); }
const QPointF operator*( const QPointF& p1, const Point&   p2 ) { return QPointF( p1.x() * p2.x,   p1.y() * p2.y ); }

const QPointF operator/( const QPointF& p1, const QPoint&  p2 ) { return QPointF( p1.x() / p2.x(), p1.y() / p2.y() ); }
//const QPointF operator/( const QPointF& p1, const QPointF& p2 ) { return QPointF( p1.x() / p2.x(), p1.y() / p2.y() ); }
const QPointF operator/( const QPointF& p1, const Point&   p2 ) { return QPointF( p1.x() / p2.x,   p1.y() / p2.y ); }

const QPointF operator+( const QPointF& p1, const Point&   p2 ) { return QPointF( p1.x() + p2.x,   p1.y() + p2.y ); }
const QPoint  operator+( const QPoint&  p1, const Point&   p2 ) { return QPoint ( p1.x() + p2.x,   p1.y() + p2.y ); }
const Point   operator+( const Point&   p1, const QPointF& p2 ) { return Point  ( p1.x   + p2.x(), p1.y   + p2.y() ); }

const QPointF operator-( const QPointF& p1, const Point&   p2 ) { return QPointF( p1.x() - p2.x,   p1.y() - p2.y ); }
const QPoint  operator-( const QPoint&  p1, const Point&   p2 ) { return QPoint ( p1.x() - p2.x,   p1.y() - p2.y ); }
const Point   operator-( const Point&   p1, const QPointF& p2 ) { return Point  ( p1.x   - p2.x(), p1.y   - p2.y() ); }

const QPoint QPointRound( const QPointF& pnt ) { return QPoint( pnt.x(), pnt.y() ); }

void Rotate( double angle_rad, Point center, Point* point )
{
  Point offset = *point - center;
  double cosa = cos( angle_rad );
  double sina = sin( angle_rad );
  point->x = center.x + (cosa * offset.x - sina * offset.y);
  point->y = center.y + (sina * offset.x + cosa * offset.y);
}

QPoint min( const QPoint& p1, const QPoint& p2 ) { return QPoint( p1.x() < p2.x() ? p1.x() : p2.x(), p1.y() < p2.y() ? p1.y() : p2.y() ); }
QPoint max( const QPoint& p1, const QPoint& p2 ) { return QPoint( p1.x() > p2.x() ? p1.x() : p2.x(), p1.y() > p2.y() ? p1.y() : p2.y() ); }

QPointF min( const QPointF& p1, const QPointF& p2 ) { return QPointF( p1.x() < p2.x() ? p1.x() : p2.x(), p1.y() < p2.y() ? p1.y() : p2.y() ); }
QPointF max( const QPointF& p1, const QPointF& p2 ) { return QPointF( p1.x() > p2.x() ? p1.x() : p2.x(), p1.y() > p2.y() ? p1.y() : p2.y() ); }

