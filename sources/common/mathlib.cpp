#include "mathlib.h"


///////////////////////////////////////////////////////////////////////////////////////////////////



double Point2PointDist( Point _p1, Point _p2 ) {
  return sqrt( dg_sqr(_p1.x - _p2.x) + dg_sqr( _p1.y - _p2.y ) );
}

double Point2VectorDist( Point _vp1, Point _vp2, Point _pnt, int* _pPosFlag, double* _pPos )
{

  _vp2 -= _vp1;
  _pnt -= _vp1;

  double sq = _vp2.x * _vp2.x + _vp2.y * _vp2.y;
  double k = _vp2.x * _pnt.x + _vp2.y * _pnt.y;

  if( _pPosFlag != NULL )
    *_pPosFlag = (k < sq / 2) ? ( k < 0 ? -1 : 1 ) : ( k > sq ? -2 : 2 );
  if( _pPos != NULL )
    *_pPos = dg_min( dg_max( k, 0 ), sq ) / sq;
  if( sq > 0 && k > 0 && k < sq )
    return fabs( _vp2.x * _pnt.y - _vp2.y * _pnt.x ) / sqrt( sq );
  if( k > 0 )
  _pnt += _vp1 - (_vp2 + _vp1);

  /*printf("pvdd:%g %g %g\n",x,y,sqrt(x*x+y*y));*/
  return sqrt( _pnt.x * _pnt.x + _pnt.y * _pnt.y );
}



///////////////////////////////////////////////////////////////////////////////////////////////////

double CrossProductZ( const Point& _crP1, const Point& _crP2, const Point& _crP0 )
{
  Point a = _crP2 - _crP1;
  Point b = _crP0 - _crP2;
  double r = a.x * b.y - a.y * b.x;
  return r;
}


/* Return -1 on non-intersection, 0 on intersection */
int VIntersect( Point _p1, Point _p2, Point _p3, Point _p4, double* _pAr, double* _pBr )
{
  double d = (_p4.y - _p3.y) * (_p2.x - _p1.x) - (_p4.x - _p3.x) * (_p2.y - _p1.y);
  if( d == 0 )
    return -2;
  double a = (_p4.y - _p3.y) * (_p3.x - _p1.x) - (_p4.x - _p3.x) * (_p3.y - _p1.y);
  double b = (_p2.y - _p1.y) * (_p3.x - _p1.x) - (_p2.x - _p1.x) * (_p3.y - _p1.y);
  a = a / d;
  b = b / d;

  if( _pAr != NULL )
    *_pAr = a;
  if( _pBr != NULL )
    *_pBr = b;
  if( a < 0 || a > 1 || b < 0 || b > 1 )
    return -1;
  return 0;
}

double CalcLineLength( const PointArray &_crXY_array )
{
  double l = 0;
  FOREACH_CONST( PointIterConst, itXY, _crXY_array ) {
    PointIterConst itXY_next = itXY + 1;
    if( itXY_next == ENDITER )
      break;
    l += hypot( *itXY - *itXY_next );
  }
  return l;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


int CompPolyLines( PointArray& _rvLine1, PointArray& _rvLine2 )
{
  if( _rvLine1.size() != _rvLine2.size() )
    return -1;

  PointIter itP1, itP2, itP1End = _rvLine1.end();
  for( itP1 = _rvLine1.begin(), itP2 = _rvLine2.begin(); itP1 != itP1End; ++itP1, ++itP2 )
    if( *itP1 != *itP2 )
      break;

  if( itP1 == _rvLine1.end() )
    return 0;

  std::reverse( _rvLine2.begin(), _rvLine2.end() );

  itP1End = _rvLine1.end();
  for( itP1 = _rvLine1.begin(), itP2 = _rvLine2.begin(); itP1 != itP1End; ++itP1, ++itP2 )
    if( *itP1 != *itP2 )
      break;

  std::reverse( _rvLine2.begin(), _rvLine2.end() );

  if( itP1 == _rvLine1.end() )
    return 0;

  return -1;
}

int CompPolyLinesEx( PointArray& _rvLine1, PointArray& _rvLine2, double _maxError )
{
  if( _rvLine1.size() != _rvLine2.size() )
    return -1;

  PointIter itP1, itP2, itP1End = _rvLine1.end();
  for( itP1 = _rvLine1.begin(), itP2 = _rvLine2.begin(); itP1 != itP1End; ++itP1, ++itP2 )
    if( hypot( *itP1 - *itP2 ) > _maxError )
      break;

  if( itP1 == _rvLine1.end() )
    return 0;

  std::reverse( _rvLine2.begin(), _rvLine2.end() );

  itP1End = _rvLine1.end();
  for( itP1 = _rvLine1.begin(), itP2 = _rvLine2.begin(); itP1 != itP1End; ++itP1, ++itP2 )
    if( hypot( *itP1 - *itP2 ) > _maxError )
      break;

  std::reverse( _rvLine2.begin(), _rvLine2.end() );

  if( itP1 == _rvLine1.end() )
    return 0;

  return -1;
}

/* Returns 0 on success */
int PolyLinesIntersect( const PointArray& _crvLine1, const PointArray& _crvLine2,
                        double* _pPos1, double* _pPos2 ) {
  double r1t,r2t,len1=0,len2=0;

  if( _crvLine1.size() < 2 || _crvLine2.size() < 2 )
    return -1;

  double r1 = 100.;
  double r2 = 100.;

  for( PointIterConst itXY1b = _crvLine1.begin() + 1, itP1End = _crvLine1.end();
       itXY1b != itP1End; ++itXY1b ) {
    Point xy1a = *(itXY1b - 1);
    Point xy1b = *itXY1b;
    len2 = 0;

    for( PointIterConst itXY2b = _crvLine2.begin() + 1, itP2End = _crvLine2.end();
         itXY2b != itP2End; ++itXY2b ) {
      Point xy2a = *(itXY2b - 1);
      Point xy2b = *itXY2b;

      if( VIntersect( xy1a, xy1b, xy2a, xy2b, &r1t, &r2t)) continue;

      /* Intersection detected */

      r1=dg_min(r1,r1t);
      r2=dg_min(r2,r2t);
      len2+=hypot( xy2b - xy2a )*dg_min(r2,1);
    }

    len1+=hypot( xy1b - xy1a )*dg_min(r1,1);

    if (r1<=1) break;
  }

  if (_pPos1!=NULL) *_pPos1=len1;
  if (_pPos2!=NULL) *_pPos2=len2;

  return r1<=1? 0 : -1;
}

void CutPolyLine( PointArray* _pLine, double _pos, bool _tail ) {
  if( _pLine->size() < 2 )
    return;

  double l = 0., l1 = 0.;
  for( PointIter itXY = _pLine->begin() + 1, itXYEnd = _pLine->end();
       itXY != itXYEnd; ++itXY, l1 = l ) {
    PointIter itXY1 = itXY - 1;

    double h = hypot( *itXY - *itXY1 );
    l += h;

    if( h == 0 )
      continue;

    if( l1 <= _pos && l >= _pos) {
      Point n = *itXY1 + (*itXY - *itXY1) * (_pos - l1) / h;

      //if( tail ) { //MB: originally pointers were compared here, now values
      //  itXY1 = std::find( pLine->begin(), pLine->end(), n ); // Find point with n-value
      //  pLine->erase( pLine->begin(), itXY1 ); // And remove all before it
      //}
      if( _tail ) { //MB: originally there was strange algorithm, current one must be checked
        *itXY1 = n; // Set new values to previous point
        _pLine->erase( _pLine->begin(), itXY1 ); // And remove all after it
      }
      else {
        *itXY = n; // Set new values to current point
        _pLine->erase( itXY + 1, _pLine->end() ); // And remove all after it
      }
      break;
    }
  }

  return;
}

double ProjectPointToPolyLine( const PointArray& _crvLine, const Point& _crPnt )
{
  if( _crvLine.size() < 2 )
    return 0;

  double dHit = MAXDOUBLE;
  double vHit = 0.;

  double s = 0;

  Point xy, xy1;
  FOREACHPAIRCONST( xy, xy1, _crvLine ) {

    double v;
    double d = Point2VectorDist( xy, xy1, _crPnt, NULL, &v );
    v = s + v * hypot( xy - xy1 );
    if( d < dHit ) {
      dHit = d;
      vHit = v;
    }
    s += hypot( xy - xy1 );
  }
  if( vHit < 0 )
    vHit = 0;
  if( vHit > s )
    vHit = s;

  return vHit;
}

double PointToPolyLineDist( const PointArray& _crvLine, const Point& _crPnt )
{
  double dMin = MAXDOUBLE;
  FOREACH_CONST( PointIterConst, itP, _crvLine ) {
    Point pnt_line = *itP;
    double d = hypot( _crPnt - pnt_line );
    if( d < dMin )
      dMin = d;
  }
  return dMin;
}


int GetPolyLinePoint( const PointArray& _crvLine, double _length, double* _pX, double* _pY )
{
  double s = 0.;

  assert( !_crvLine.empty() );

  Point xy, xy1;
  FOREACHPAIRCONST( xy, xy1, _crvLine ) {
    double l = hypot( xy - xy1 );
    if( l + s >= _length ) {
      xy = xy + (xy1 - xy) * (_length - s) / l;
      *_pX = xy.x;
      *_pY = xy.y;
      return 0;
    }
    s += l;
  }
  *_pX = xy.x;
  *_pY = xy.y;
  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


/* Returns angle "from" (dx1,dy1) "to" (dx2,dy2) */
double AngleBetween( const Point& _crP1, const Point& _crP2 )
{
  double a1 = acos( _crP1.x / hypot( _crP1 ) );
  if( _crP1.y < 0 )
    a1 = M_2XPI - a1;
  double a2 = acos( _crP2.x / hypot( _crP2 ) );
  if( _crP2.y < 0 )
    a2 = M_2XPI - a2;

  double a = a2 - a1;
  if( a < 0 )
    a += M_2XPI;

  return a;
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool PointInRectangle( const Point& _crPnt, const Point& _crRect_p1, const Point& _crRect_p2 )
{
  return (_crPnt.x >= dg_min( _crRect_p1.x, _crRect_p2.x ) && _crPnt.x <= dg_max( _crRect_p1.x, _crRect_p2.x ) &&
          _crPnt.y >= dg_min( _crRect_p1.y, _crRect_p2.y ) && _crPnt.y <= dg_max( _crRect_p1.y, _crRect_p2.y ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool VectorInRectangle( const Point& _crVr_p1, const Point& _crVr_p2, const Point& _crRect_p1, const Point& _crRect_p2 )
{
  Point p1 = _crVr_p1;
  Point p2 = _crVr_p2;
  Point r1 = _crRect_p1;
  Point r2 = _crRect_p2;

  if( r2.x < r1.x )
    dg_swap( r2.x, r1.x );
  if( r2.y < r1.y )
    dg_swap( r2.y, r1.y );

  if( p1.x < r1.x && p2.x > r1.x ) {
    p1.y += (p2.y - p1.y) * (r1.x - p1.x) / (p2.x - p1.x);
    p1.x = r1.x;
  }
  else if( p2.x < r1.x && p1.x > r1.x ) {
    p2.y += (p1.y - p2.y) * (r1.x - p2.x) / (p1.x - p2.x);
    p2.x = r1.x;
  }

  if( p1.x > r2.x && p2.x < r2.x ) {
    p1.y += (p2.y - p1.y) * (r2.x - p1.x) / (p2.x - p1.x);
    p1.x = r2.x;
  }
  else if( p2.x > r2.x && p1.x < r2.x ) {
    p2.y += (p1.y - p2.y) * (r2.x - p2.x) / (p1.x - p2.x);
    p2.x = r2.x;
  }

  if( p1.y < r1.y && p2.y > r1.y ) {
    p1.x += (p2.x - p1.x) * (r1.y - p1.y) / (p2.y - p1.y);
    p1.y = r1.y;
  }
  else if( p2.y < r1.y && p1.y > r1.y ) {
    p2.x += (p1.x - p2.x) * (r1.y - p2.y) / (p1.y - p2.y);
    p2.y = r1.y;
  }

  if( p1.y > r2.y && p2.y < r2.y ) {
    p1.x += (p2.x - p1.x) * (r2.y - p1.y) / (p2.y - p1.y);
    p1.y = r2.y;
  }
  else if( p2.y > r2.y && p1.y < r2.y ) {
    p2.x += (p1.x - p2.x) * (r2.y - p2.y) / (p1.y - p2.y);
    p2.y = r2.y;
  }

  if( p1.x < r1.x && p2.x < r1.x ) return false;
  if( p1.y < r1.y && p2.y < r1.y ) return false;
  if( p1.x > r2.x && p2.x > r2.x ) return false;
  if( p1.y > r2.y && p2.y > r2.y ) return false;

  return true;
}
