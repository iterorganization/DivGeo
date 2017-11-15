/* Spline routines */

//#include "dg.h"
#include "Spline.h"

#define ID_PARAMSPLINEINFO   0x25486324
#define ID_SIMPLESPLINEINFO  0x56488145
#define ID_SURFACESPLINEINFO 0x34278923

static void spline(int n,double* x,double* y,double* b,double* c,double* d);

/* Simple spline //////////////////////////////////////////////////// */

SimpleSpline::SimpleSpline():
  id( ID_SIMPLESPLINEINFO ),
  y( null ),
  t( null ),
  b( null ),
  c( null ),
  d( null )
{}

SimpleSpline::SimpleSpline( std::vector< Point >& xy_array ):
  id( ID_SIMPLESPLINEINFO ),
  y( null ),
  t( null ),
  b( null ),
  c( null ),
  d( null )
{
  Init( xy_array );
}

void SimpleSpline::FreeArrays()
{
  if( y != null ) delete[] y;
  if( t != null ) delete[] t;
  if( b != null ) delete[] b;
  if( c != null ) delete[] c;
  if( d != null ) delete[] d;
}


void SimpleSpline::Init( std::vector< Point >& xy_array )
{
  pointCount = xy_array.size();
  assert( pointCount > 0 );
  FreeArrays();
  y = new double[ pointCount ];
  t = new double[ pointCount ];
  b = new double[ pointCount ];
  c = new double[ pointCount ];
  d = new double[ pointCount ];

  tMin = MAXDOUBLE;
  tMax = -MAXDOUBLE;

  unsigned i = 0;
  PointIter itEnd = xy_array.end();
  for( PointIter it = xy_array.begin(); it != itEnd; ++it ) {
    if( i > 0 && it->x == t[i-1]) // skip point if argument is the same
      continue;
    
    t[i] = it->x;
    y[i] = it->y;

    tMin = dg_min( tMin, t[i] );
    tMax = dg_max( tMax, t[i] );
    i++;
  }

  pointCount = i;
  if( pointCount > 1 )
    spline( pointCount, t, y, b, c, d);
}

SimpleSpline::~SimpleSpline()
{
  assert( id == ID_SIMPLESPLINEINFO );
  FreeArrays();
}

int SimpleSpline::CalcValue( double t, double* pY ) const
{
  assert( pY != null );
  assert( id == ID_SIMPLESPLINEINFO );
  assert( t >= tMin && t <= tMax );

  if( pointCount == 1 ) {
    *pY = y[0];
    return 0;
  }

  unsigned i = 0;
  for( ; i < pointCount; i++ )
    if( i > 0 && this->t[i] >= t )
      break;

  i = dg_min( i, pointCount - 1 );
  assert( i > 0 );
  --i;

  double v = t - this->t[i];

  *pY = y[i] + b[i]*v + c[i]*v*v + d[i]*v*v*v;

  return 0;
}

int SimpleSpline::CalcDY( double _t, double* pDY ) const
{
  assert( pDY != null );
  assert( id == ID_SIMPLESPLINEINFO );
  assert( _t >= tMin && _t <= tMax );

  if( pointCount == 1 ) {
    *pDY = 0;
    return 0;
  }

  unsigned i = 0;
  for( i = 0; i < pointCount; i++ )
    if( i > 0 && t[i] >= _t )
      break;

  i = dg_min( i, pointCount - 1 );
  assert( i > 0 );
  i--;

  double v = _t - t[i];

  *pDY = b[i] + 2*c[i]*v + 3*d[i]*v*v;

  return 0;
}


/* Parametric spline //////////////////////////////////////////////// */

ParamSpline::ParamSpline( std::vector< Point >& xy_array ):
 id( ID_PARAMSPLINEINFO )
{
  pointCount = xy_array.size();
  assert( pointCount > 0 );

  x = new double[ pointCount ];
  y = new double[ pointCount ];
  t = new double[ pointCount ];
  bx = new double[ pointCount ];
  cx = new double[ pointCount ];
  dx = new double[ pointCount ];
  by = new double[ pointCount ];
  cy = new double[ pointCount ];
  dy = new double[ pointCount ];

  double t2 = 0.;
  int i = 0;
  for( std::vector< Point >::iterator it = xy_array.begin(); it != xy_array.end(); it++ ) {
    if( i > 0 && it->x == x[i-1] && it->y == y[i-1] )
      continue;

    x[i] = it->x;
    y[i] = it->y;

    if( i > 0 ) {
      double h = hypot( x[i] - x[i-1], y[i] - y[i-1] );
      if( h <= 0 )
        continue;
      t2 += h;
    }
    t[i] = t2;
    i++;
  }

  pointCount = i;

  if( pointCount > 1 ) {
    spline( pointCount, t, x, bx, cx, dx );
    spline( pointCount, t, y, by, cy, dy );
  }

  tMin = 0;
  tMax = t2;
}

ParamSpline::~ParamSpline()
{
  assert( id == ID_PARAMSPLINEINFO );

  if( x != null ) delete[] x;
  if( y != null ) delete[] y;
  if( t != null ) delete[] t;
  if( bx != null ) delete[] bx;
  if( cx != null ) delete[] cx;
  if( dx != null ) delete[] dx;
  if( by != null ) delete[] by;
  if( cy != null ) delete[] cy;
  if( dy != null ) delete[] dy;
}

int ParamSpline::CalcValue( double _t, double* pX, double* pY ) const
{
  assert( pX != null && pY != null );
  assert( id == ID_PARAMSPLINEINFO );
  assert( _t >= tMin && _t <= tMax );

  if( pointCount == 1 ) {
    *pX = x[0];
    *pY = y[0];
    return 0;
  }

  unsigned i = 0;
  for( i=0; i < pointCount; i++ )
    if( i > 0 && t[i] >= _t)
      break;

  i = dg_min( i, pointCount - 1 );
  assert( i > 0 );
  i--;

  double v = _t - t[i];

  *pX = x[i] + bx[i]*v + cx[i]*v*v + dx[i]*v*v*v;
  *pY = y[i] + by[i]*v + cy[i]*v*v + dy[i]*v*v*v;

  return 0;
}


/* Surface spline /////////////////////////////////////////////////// */

SurfaceSpline::SurfaceSpline( int pointsX, int pointsY, double* ptx, double *pty, double* ptz ):
  id( ID_SURFACESPLINEINFO )
{
  assert( ptx != null && pty != null && ptz != null );
  assert( pointsX > 1 && pointsY >= 1 );

  /* Allocate & init structures */
  pointCount[0] = pointsX;
  pointCount[1] = pointsY;

  for( unsigned n = 0; n < 2; n++ )
    t[n] = new double[pointCount[n]];

  for( unsigned i = 0; i < pointCount[0]; i++ )
    t[0][i] = ptx[i];
  for( unsigned i = 0; i < pointCount[1]; i++ )
    t[1][i] = pty[i];

  /* Allocate  a read/write duplicate of ptz */
  double* ptzOld = ptz;
  ptz = new double[pointsX*pointsY];
  memcpy( ptz, ptzOld, sizeof(double) * pointsX*pointsY );

  /* Rearrange x/y in ascending order, if needed */
  unsigned k = pointCount[0]; // X
  if( t[0][0] > t[0][1] ) {
    for( unsigned j = 0; j < k / 2; j++ ) { //TODO: why k/2?
      dg_swap( t[0][j], t[0][k-1-j] );
      for( unsigned i = 0; i < pointCount[1]; i++ )
        dg_swap( ptz[i*pointCount[0]+j], ptz[i*pointCount[0]+(k-1-j)] );
    }
  }

  k = pointCount[1]; // Y
  if( t[1][0] > t[1][1] ) {
    for( unsigned i = 0; i < k / 2; i++ ) {
      dg_swap( t[1][i], t[1][k-1-i] );
      for( unsigned j = 0; j < pointCount[0]; i++ )
        dg_swap( ptz[i*pointCount[0]+j], ptz[(k-1-i)*pointCount[0]+j] );
    }
  }

  // Check order
  for( unsigned i = 0; i < 2; i++ )
    for( unsigned j = 1; j < pointCount[i]; j++ ) {
      if( t[i][j] < t[i][j-1] ) {
        delete[] ptz;
        for( k = 0; k < 2; k++ )
          delete[] t[k];
        //delete this
      }
  }

  /* Find out minimum, maximum "t" values */
  for( unsigned n = 0; n < 2; n++ ) {
    tMin[n] = MAXDOUBLE;
    tMax[n] = -MAXDOUBLE;
    for( unsigned i = 0; i < pointCount[n]; i++ ) {
      tMin[n] = dg_min( tMin[n], t[n][i] );
      tMax[n] = dg_max( tMax[n], t[n][i] );
    }
  }

  coeff = (double(*)[4][4])new double[sizeof(*coeff)/*4x4*/*pointCount[0]*pointCount[1]];

  /* Calculate coefficients */
  SimpleSpline* SS_array = new SimpleSpline[pointCount[1]];

  /* Calculate 1-D spline coefficients for each "row" */
  std::vector< Point > xy_array;
  for( unsigned i = 0; i < pointCount[1]; i++ ) {
    xy_array.clear();
    for( unsigned j = 0; j < pointCount[0]; j++ )
      xy_array.push_back( Point( t[0][j], ptz[i*pointCount[0]+j] ) );

    SS_array[i].Init( xy_array );
  }

  /* Calculate sets of 2D coefficients for each cell */
  for( unsigned j = 0; j < pointCount[0]; j++ ) {
    for( unsigned k = 0; k < 4; k++ ) {

      /* For each "column" and for each "horizontal" coefficient,
         build a "vertical" spline */
      double c = 0.;
      xy_array.clear();
      for( unsigned i = 0; i < pointCount[1]; i++ ) {
        switch( k ) {
          case 0: c = SS_array[i].Y(j); break;
          case 1: c = SS_array[i].B(j); break;
          case 2: c = SS_array[i].C(j); break;
          case 3: c = SS_array[i].D(j); break;
          default: assert( 0 );
        }
        xy_array.push_back( Point( t[1][i], c ) );
      }

      SimpleSpline ss( xy_array );

      /* Store coefficients */
      for( unsigned i = 0; i < pointCount[1]; i++ ) {
        unsigned index = i*pointCount[0]+j;
        coeff[index][0][k] = ss.Y(i);
        coeff[index][1][k] = ss.B(i);
        coeff[index][2][k] = ss.C(i);
        coeff[index][3][k] = ss.D(i);
      }
    }
  }

  /* Free "horizontal" splines and ptz double */
  delete[] SS_array;
  delete[] ptz;
}

SurfaceSpline::~SurfaceSpline()
{ 
  assert( id == ID_SURFACESPLINEINFO );

  if( coeff != null )
    delete[] coeff;
  for( unsigned i = 0; i < 2; i++ )
    if( t[i] != null )
      delete[] t[i];
}

int SurfaceSpline::CalcValue( double tx, double ty, double* pZ, double* pdX, double* pdY ) const
{
  /* Check for valid args */
  //assert( pZ != null && pdX != null && pdY != null );
  assert( id == ID_SURFACESPLINEINFO );
  assert( tx >= tMin[0] && tx <= tMax[0] && 
          ty >= tMin[1] && ty <= tMax[1] );

  /* Find the cell */
  int nx = -1, ny = -1;
  /*for( unsigned i = 0; i < pointCount[0] - 1; i++ )
    if( dg_inrange( tx, t[0][i], t[0][i+1] ) ) {
      nx = i;
      break;
    }

  for( unsigned i = 0; i < pointCount[1] - 1; i++ )
    if( dg_inrange( ty, t[1][i], t[1][i+1] ) ) {
      ny = i;
      break;
    }*/

  { // calc nx
    int i = 0;
    int j = pointCount[0] - 1;
    assert( j > 0 );

    while( i + 1 < j ) {
      int k = (i + j) / 2;
      if( t[0][k] < tx )
        i = k;
      else
        j = k;
  /* printf("x i j %d %d\n",i,j); */
    }
    if( dg_inrange( tx, t[0][i], t[0][i+1] ) )
      nx = i;
  }

  { // calc ny
    int i = 0;
    int j = pointCount[1] - 1;
    assert( j > 0 );
    while( i + 1 < j ) {
      int k = (i + j) / 2;
      if( t[1][k] < ty )
        i = k;
      else
        j = k;
  /* printf("y i j %d %d\n",i,j); */
    }
    if( dg_inrange( ty, t[1][i], t[1][i+1] ) ) 
      ny = i;
  }

  assert( nx >= 0 && ny >= 0 );
  /* printf("%d %d\n",nx,ny); */


  /* Calculate the value */
  if( pZ != NULL ) {
    *pZ = 0;
    double py = 1.;
    for( unsigned i = 0; i < 4; i++ ) {
      double px = 1.;
      for( unsigned j = 0; j < 4; j++ ) {
        *pZ += coeff[ny*pointCount[0]+nx][i][j] * px * py;
        px *= (tx - t[0][nx]);
      }
      py *= (ty - t[1][ny]);
    }
  }
  /* printf("%e %e\n",tx-si->t[0][nx],ty-si->t[1][ny]); */

  /* Calculate derivatives, if needed */
  if( pdX != NULL ) {
    *pdX = 0;
    double py = 1.;
    for( unsigned i = 0; i < 4; i++ ) {
      double px = 1.;
      for( unsigned j = 1; j < 4; j++ ) {
        *pdX += j * coeff[ny*pointCount[0]+nx][i][j] * px * py;
        px *= (tx - t[0][nx]);
      }
      py *= (ty - t[1][ny]);
    }
  }
  /* printf("%g\n",*pdX); */

  if( pdY != NULL ) {
    *pdY = 0;
    double py=1.;
    for( unsigned i = 1; i < 4; i++ ) {
      double px = 1.;
      for( unsigned j = 0; j < 4; j++ ) {
        *pdY += i * coeff[ny*pointCount[0]+nx][i][j] * px * py;
        px *= (tx - t[0][nx]);
      }
      py *= (ty - t[1][ny]);
    }
  }

  return 0;
}


/* Spline routine /////////////////////////////////////////////////// */

/* --- Converted from: Netlib spline.f */

static void spline(int n,double* x,double* y,double* b,double* c,double* d) {

/*
c  the coefficients b(i), c(i), and d(i), i=1,2,...,n are computed
c  for a cubic interpolating spline
c
c    s(x) = y(i) + b(i)*(x-x(i)) + c(i)*(x-x(i))**2 + d(i)*(x-x(i))**3
c
c    for  x(i) .le. x .le. x(i+1)
c
c  input..
c
c    n = the number of data points or knots (n.ge.2)
c    x = the abscissas of the knots in strictly increasing order
c    y = the ordinates of the knots
c
c  output..
c
c    b, c, d  = arrays of spline coefficients as defined above.
c
c  using  p  to denote differentiation,
c
c    y(i) = s(x(i))
c    b(i) = sp(x(i))
c    c(i) = spp(x(i))/2
c    d(i) = sppp(x(i))/6  (derivative from the right)
c
c  the accompanying function subprogram  seval  can be used
c  to evaluate the spline.
c
c
*/

  int nm1,ib,i;
  double t;

  /* Accomodate Fortran arrays */

  x--;
  y--;
  b--;
  c--;
  d--;

  nm1 = n-1;
  if (n<2) return;
  if (n<3) goto _50;

/*
c
c  set up tridiagonal system
c
c  b = diagonal, d = offdiagonal, c = right hand side.
c
*/
   d[1] = x[2] - x[1];
   c[2] = (y[2] - y[1])/d[1];
   for (i=2;i<=nm1;i++) {
      d[i] = x[i+1] - x[i];
      b[i] = 2.*(d[i-1] + d[i]);
      c[i+1] = (y[i+1] - y[i])/d[i];
      c[i] = c[i+1] - c[i];
   }
/*
c
c  end conditions.  third derivatives at  x(1)  and  x(n)
c  obtained from divided differences
c
*/
  b[1] = -d[1];
  b[n] = -d[n-1];
  c[1] = 0.;
  c[n] = 0.;
  if (n==3) goto _15;
  c[1] = c[3]/(x[4]-x[2]) - c[2]/(x[3]-x[1]);
  c[n] = c[n-1]/(x[n]-x[n-2]) - c[n-2]/(x[n-1]-x[n-3]);
  c[1] = c[1]*d[1]*d[1]/(x[4]-x[1]);
  c[n] = -c[n]*d[n-1]*d[n-1]/(x[n]-x[n-3]);
/*
c
c  forward elimination
c
*/
  _15:

  for (i=2;i<=n;i++) {
    t = d[i-1]/b[i-1];
    b[i] = b[i] - t*d[i-1];
    c[i] = c[i] - t*c[i-1];
  }

/*
c
c  back substitution
c
*/

  c[n] = c[n]/b[n];
  for (ib=1;ib<=nm1;ib++) {
    i = n-ib;
    c[i] = (c[i] - d[i]*c[i+1])/b[i];
  }

/*
c
c  c(i) is now the sigma(i) of the text
c
c  compute polynomial coefficients
c
*/

  b[n] = (y[n] - y[nm1])/d[nm1] + d[nm1]*(c[nm1] + 2.*c[n]);
  for (i=1;i<=nm1;i++) {
    b[i] = (y[i+1] - y[i])/d[i] - d[i]*(c[i+1] + 2.*c[i]);
    d[i] = (c[i+1] - c[i])/d[i];
    c[i] = 3.*c[i];
  }
  c[n] = 3.*c[n];
  d[n] = d[n-1];
  return;

  _50:

  b[1] = (y[2]-y[1])/(x[2]-x[1]);
  c[1] = 0.;
  d[1] = 0.;
  b[2] = b[1];
  c[2] = 0.;
  d[2] = 0.;
  return;
}

