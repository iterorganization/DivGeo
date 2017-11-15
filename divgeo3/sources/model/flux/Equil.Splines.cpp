#include "Equil.h"
#include "../core/Model.h"

/* Calculate group of points representing a surface
   Return: -1 = invalid origin
            0 = ok, surface not closed
            1 = ok, surface closed
*/

int Equil::CalcSurfaceLine( NPoint _index, double level, std::vector< Point >* pXY_array, NPoint _size ) const
{
  NPoint oIndex = _index;

  SurfCell sc;
  CalcSurfData( _index, level, &sc, _size );
  if( sc.n != 2 && sc.n != 4)
    return -1;
  int d = sc.d[0];

  pXY_array->clear();
  do {
    pXY_array->push_back( Point( sc.x[d], sc.y[d] ) );

    if( d == CS_YM ) _index.y--;
    if( d == CS_YP ) _index.y++;
    if( d == CS_XM ) _index.x--;
    if( d == CS_XP ) _index.x++;

    CalcSurfData( _index, level, &sc, _size );
    if( sc.n == 2 )
      d = (d^2) == sc.d[0] ? sc.d[1] : sc.d[0];

    if( _index == oIndex ) {
      pXY_array->push_back( Point( sc.x[d], sc.y[d] ) );
      return 1;
    }
  } while( sc.n == 2 || sc.n == 4 );

  std::reverse( pXY_array->begin(), pXY_array->end() );

  _index = oIndex;
  CalcSurfData( _index, level, &sc, _size );
  d = (sc.n == 2) ? sc.d[1] : sc.d[0]^2;

  do {
    pXY_array->push_back( Point( sc.x[d], sc.y[d] ) );

    if( d == CS_YM ) _index.y--;
    if( d == CS_YP ) _index.y++;
    if( d == CS_XM ) _index.x--;
    if( d == CS_XP ) _index.x++;

    CalcSurfData( _index, level, &sc, _size );
    if( sc.n == 2 )
      d = (d^2) == sc.d[0] ? sc.d[1] : sc.d[0];

  } while( sc.n == 2 || sc.n == 4 );

  return 0;
}


#define XEx( cx, sx ) (x[0] + (x[(sx)-1] - x[0]) * (cx) / ((sx) - 1))
#define YEx( cy, sy ) (y[0] + (y[(sy)-1] - y[0]) * (cy) / ((sy) - 1))

void Equil::CalcSurfData( NPoint _index, double level, SurfCellPtr pSC, NPoint _size ) const
{
  pSC->n = pSC->f = 0;
  if( _index.x < 0 || _index.y < 0 || _index.x >= _size.x - 1 || _index.y >= _size.y - 1 )
    return;

  double a1 = CorrCellEx( _index, level, _size );
  double a2 = CorrCellEx( NPoint( _index.x + 1, _index.y ), level, _size );

  if( dg_inrange( level, a1, a2 ) ) {
    int d = CS_YM;
    pSC->x[d] = XEx( _index.x, _size.x ) + (XEx( _index.x+1, _size.x ) - XEx( _index.x, _size.x )) * (level - a1) / (a2 - a1);
    pSC->y[d] = YEx( _index.y, _size.y );
    pSC->f |= CSF_YM;
    pSC->d[pSC->n++] = d;
  }

  a2 = CorrCellEx( NPoint( _index.x, _index.y + 1 ), level, _size );
  if( dg_inrange_s( level, a1, a2 ) ) {
    int d = CS_XM;
    pSC->x[d] = XEx( _index.x, _size.x );
    pSC->y[d] = YEx( _index.y, _size.y ) + (YEx( _index.y + 1, _size.y ) - YEx( _index.y, _size.y )) * (level - a1) / (a2 - a1);
    pSC->f |= CSF_XM;
    pSC->d[pSC->n++] = d;
  }

  a1 = CorrCellEx( NPoint( _index.x, _index.y + 1 ), level, _size );
  a2 = CorrCellEx( NPoint( _index.x + 1, _index.y + 1 ), level, _size );
  if( dg_inrange( level, a1, a2 ) ) {
    int d = CS_YP;
    pSC->x[d] = XEx( _index.x, _size.x ) + (XEx( _index.x+1, _size.x ) - XEx( _index.x, _size.x )) * (level - a1) / (a2 - a1);
    pSC->y[d] = YEx( _index.y + 1, _size.y );
    pSC->f |= CSF_YP;
    pSC->d[pSC->n++] = d;
  }

  a1 = CorrCellEx( NPoint( _index.x + 1, _index.y ), level, _size );
  if( dg_inrange_s( level, a1, a2 ) ) {
    int d = CS_XP;
    pSC->x[d] = XEx( _index.x + 1, _size.x );
    pSC->y[d] = YEx( _index.y, _size.y ) + (YEx( _index.y + 1, _size.y ) - YEx( _index.y, _size.y )) * (level - a1) / (a2 - a1);
    pSC->f |= CSF_XP;
    pSC->d[pSC->n++] = d;
  }

  return;
}


void Equil::CalcValues()
{
  minVal = maxVal = val[0];
  for( int i = 0; i < size.x*size.y; i++ ) {
    minVal = dg_min( minVal, val[i] );
    maxVal = dg_max( maxVal, val[i] );
  }
  signInside = 0;
}


int Equil::CalcGradient( Point _pnt, double* pGrad ) const
{
  SENDER_NAME( "CalcGradient" );
  double r_dummy;
  if( pGrad == NULL )
    pGrad = &r_dummy;
  *pGrad = 0;

  if( _pnt.x <= dg_min( x[0], x[size.x - 1] ) ||
      _pnt.x >= dg_max( x[0], x[size.x - 1] ) ||
      _pnt.y <= dg_min( y[0], y[size.y - 1] ) ||
      _pnt.y >= dg_max( y[0], y[size.y - 1] ) )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER, ERR::MESH_OUT_OF_EQUIL );

  /*if( hSplines.empty() || hSplines.empty() )
    CalcSplines();*/ //TODO: it is a const method!

  double t;
  PointArray points_tmp;
  for( int i = 0; i < size.x; i++ ) {
    vSplines[i]->CalcValue( _pnt.y, &t );
    points_tmp.push_back( Point( x[i], t ) );
  }
  SimpleSpline ss1( points_tmp );
  Point grad;
  ss1.CalcDY( _pnt.x, &grad.x );
  points_tmp.clear();

  for( int i = 0; i < size.y; i++ ) {
    hSplines[i]->CalcValue( _pnt.x, &t );
    points_tmp.push_back( Point( y[i], t ) );
  }
  SimpleSpline ss2( points_tmp );
  ss2.CalcDY( _pnt.y, &grad.y );
  points_tmp.clear();

  *pGrad = hypot( grad.x, grad.y );

  return 0;
}


int Equil::CalcSplines()
{
  if( !hSplines.empty() && !vSplines.empty() )
    return 0;

  std::vector< Point > xy_array;

  FreeSplines();
  hSplines.resize( size.y );
  for( int j = 0; j < size.y; j++ )
  {
    xy_array.clear();
    for( int i = 0; i < size.x; i++ )
      xy_array.push_back( Point( x[i], Cell( NPoint( i, j ) ) ) );

    hSplines[j] = new SimpleSpline( xy_array );
    assert( hSplines[j] != NULL );
  }

  vSplines.resize( size.x );
  for( int j = 0; j < size.x; j++ )
  {
    xy_array.clear();
    for( int i = 0; i < size.y; i++ )
      xy_array.push_back( Point( y[i], Cell( NPoint( j, i ) ) ) );

    vSplines[j] = new SimpleSpline( xy_array );
    assert( vSplines[j] != NULL );
  }

  pSurface = new SurfaceSpline( size.x, size.y, x, y, val );

  return 0;
}

int Equil::FreeSplines()
{
  FOREACH_CONST( SimpleSplineIter, itS, hSplines )
    delete *itS;
  hSplines.clear();

  FOREACH_CONST( SimpleSplineIter, itS, vSplines )
    delete *itS;
  vSplines.clear();

  if( pSurface != NULL )
    delete pSurface;
  pSurface = null;

  return 0;
}


int Equil::GetLevel( Point _p, double* pLevel, Point* pGrad ) const
{
  /*MB: calculation method in const method. Try comment
  if( pSurface == null )
    CalcSplines();*/
  assert( pSurface != null );

  if( !pSurface->ValidateXY( _p.x, _p.y ) )
    return -1;

  if( pGrad != null )
    return pSurface->CalcValue( _p.x, _p.y, pLevel, &pGrad->x, &pGrad->y );
  else
    return pSurface->CalcValue( _p.x, _p.y, pLevel, null, null );

/* -- Bilinear
  int cx,cy;
  double a1,a2,a3,a4,a13,a24,a13d,a24d;

  if (GetEquilCell(eq,x,y,&cx,&cy)) return -1;

  a1=EqCell(eq,cx,cy);
  a2=EqCell(eq,cx+1,cy);
  a3=EqCell(eq,cx,cy+1);
  a4=EqCell(eq,cx+1,cy+1);
  a13=a1+(a1-a3)*(eq->y[cy]-y)/(eq->y[cy+1]-eq->y[cy]);
  a24=a2+(a2-a4)*(eq->y[cy]-y)/(eq->y[cy+1]-eq->y[cy]);
  if (pLevel!=NULL)
    *pLevel=a13+(a13-a24)*(eq->x[cx]-x)/(eq->x[cx+1]-eq->x[cx]);

  if (pGradX!=NULL) {
    *pGradX=-(a13-a24)/(eq->x[cx+1]-eq->x[cx]);
    a13d=-(a1-a3)/(eq->y[cy+1]-eq->y[cy]);
    a24d=-(a2-a4)/(eq->y[cy+1]-eq->y[cy]);
    *pGradY=a13d+(a13d-a24d)*(eq->x[cx]-x)/(eq->x[cx+1]-eq->x[cx]);
  }

  return 0;
*/
}


PointArray* Equil::CalcGradientLine( NPoint _p0, int steps, int bMinMax ) const
{
  PointArray* pGL = new PointArray();

  /*puts("[CalcGradientLine");*/

  NPoint np;
  NPoint p = _p0 * steps;
  NPoint op( -1, -1 );

  while( p > 0 && p < (size - 1) * steps ) {
    Point fp( x[p.x/steps] + (x[p.x/steps + 1] - x[p.x/steps]) * (p.x % steps) / steps,
              y[p.y/steps] + (y[p.y/steps + 1] - y[p.y/steps]) * (p.y % steps) / steps );

    pGL->push_back( fp );

    double xyLvl;
    if( GetLevel( fp, &xyLvl, NULL ) )
      break;

    double angleMax = -1;

    NPoint c1 = (p - 1) / steps;
    NPoint c2 = p / steps + 1;
    NPoint c = c1;

    do {
      NPoint oc = c;

      if( c.y == c1.y ) c.x == c2.x ? c.y++ : c.x++; else
      if( c.x == c2.x ) c.y == c2.y ? c.x-- : c.y++; else
      if( c.y == c2.y ) c.x == c1.x ? c.y-- : c.x--; else
      if( c.x == c1.x ) c.y == c1.y ? c.x++ : c.y--; else
      assert( 0 );

      for( int d = 0; d < steps; d++ ) {
        double lvl = Cell( oc ) + (Cell( c ) - Cell( oc )) * d / steps;
        double dist = hypot( x[oc.x] + (x[c.x] - x[oc.x]) * d / steps - fp.x,
                             y[oc.y] + (y[c.y] - y[oc.y]) * d / steps - fp.y );
        if( dist == 0 )
          continue;

        double angle = (lvl - xyLvl) / dist * bMinMax;

        if( angleMax < 0 || angle > angleMax ) {
          angleMax = angle;
          np = oc * steps + (c - oc) * d;
        }
      }
    } while( c != c1 );

    if( angleMax < 0 || np == op )
      break;

    /* printf("New xy: %d %d %d %d / %d %d %g\n",cx1,cy1,cx2,cy2,nx,ny,angleMax); */
    op = p;
    p = np;
  }

  /*printf("]CalcGradientLine: %d\n",GroupCount(g));*/
  return pGL;
}
