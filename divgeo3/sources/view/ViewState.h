#ifndef VIEWSTATE_H
#define VIEWSTATE_H

#include <QPolygon>
#include <QPointF>
#include <QVector>

#include "../common/Point.h"
#include "../common/mathlib.h"

struct SViewState
{
  static const double min_scale = 0.01;
  static const double max_scale = 10000;

  // mapped
  QPolygonF viewPoly;
  QPointF center;
  QRect viewRect;

  Point scale;
  Point scaleSign;
  bool inStretchMode;
  bool radialGrid;
  bool coloredEquil;

  double xyAngle; // rad, ccw

  bool editTopology;

  void SetAngle( double _angle_rad ) {
    xyAngle = _angle_rad;
    if( xyAngle >= M_2XPI ) xyAngle -= M_2XPI;
    if( xyAngle < 0. )      xyAngle += M_2XPI;
  }

  void SetScale( const Point& _scale ) {
    scale = _scale;
    if( scale.x > max_scale ) scale.x = max_scale;
    if( scale.y > max_scale ) scale.y = max_scale;
    if( scale.x < min_scale ) scale.x = min_scale ;
    if( scale.y < min_scale ) scale.y = min_scale ;
  }

  void SetMinMaxPositions( const Point& _min_pos, const Point& _max_pos ) {
    Point posMin = _min_pos;
    Point posMax = _max_pos;
    if( posMin.x == posMax.x ) posMax.x += 1.;
    if( posMin.y == posMax.y ) posMax.y += 1.;
    if( posMax.x < posMin.x ) dg_swap( posMax.x, posMin.x );
    if( posMax.y < posMin.y ) dg_swap( posMax.y, posMin.y );
    viewRect.setTopLeft( _C( posMin ) );
    viewRect.setBottomRight( _C( posMax ) );
  }

  SViewState():
    center( 0., 0. ),
    scale( 1., 1. ),
    scaleSign( 1. ),
    inStretchMode( false ),
    radialGrid( true ),
    coloredEquil( false ),
    xyAngle( .0 ),
    editTopology( false ) { }
};

#endif // VIEWSTATE_H
