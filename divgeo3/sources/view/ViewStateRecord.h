#ifndef VIEWUNDORECORD_H
#define VIEWUNDORECORD_H

#include <QList>
#include <QPointF>
#include "../common/Point.h"

struct ViewStateRecord
{
  QPointF center;
  Point scale;
  double angle;

  ViewStateRecord( const QPointF& _center, const Point& _scale, double _angle ):
    center( _center ), scale( _scale ), angle( _angle ) {}
};

typedef QList< ViewStateRecord > ViewStateList;

#endif // VIEWUNDORECORD_H
