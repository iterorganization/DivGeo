#ifndef _SPLINE_H
#define _SPLINE_H

#include "strlib.h"
#include "Point.h"

/* Spline routines */

typedef class SimpleSpline
{
  int id;

  unsigned pointCount;
  double tMin,tMax;

  double *y, *t, *b, *c, *d;

protected:
  void FreeArrays();

public:
  SimpleSpline();
  SimpleSpline( std::vector< Point >& xy_array );
  void Init( std::vector< Point >& xy_array );
  ~SimpleSpline();
  int CalcValue( double _t, double* pY ) const;
  int CalcDY( double _t, double* pDY ) const;

  /* getters */
  double Y( unsigned i ) { assert( i < pointCount ); return y[i]; }
  double B( unsigned i ) { assert( i < pointCount ); return b[i]; }
  double C( unsigned i ) { assert( i < pointCount ); return c[i]; }
  double D( unsigned i ) { assert( i < pointCount ); return d[i]; }
}* SimpleSplinePtr;

typedef std::vector< SimpleSplinePtr > SimpleSplineArray;
typedef SimpleSplineArray::iterator SimpleSplineIter;



typedef class ParamSpline
{
  int id;

  unsigned pointCount;
  double tMin,tMax;

  double *x,*y,* t,* bx,* cx,* dx,* by,* cy,* dy;

public:
  ParamSpline( std::vector< Point >& xy_array );
  ~ParamSpline();
  int CalcValue( double _t, double* pX, double* pY ) const;

}* ParamSplinePtr;



typedef class SurfaceSpline
{
  int id;

  unsigned pointCount[2];
  double tMin[2],tMax[2];

  double* t[2];
  double (*coeff)[4][4];

public:
  SurfaceSpline( int pointsX, int pointsY, double* ptx, double *pty, double* ptz );
  ~SurfaceSpline();
  int CalcValue( double tx, double ty, double* pZ, double* pdX, double* pdY ) const;
  bool ValidateXY( double x, double y ) const { return x >= tMin[0] && y >= tMin[1] && x <= tMax[0] && y <= tMax[1]; }
}* SurfaceSplinePtr;


#endif
