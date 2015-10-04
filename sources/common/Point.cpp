#include "Point.h"

const std::string NPoint::INDEX = "[%x, %y]";
const std::string NPoint::SIZE = "[%x x %y]";
const std::string NPoint::POSITION = "(%x, %y)";

std::string NPoint::ToString( const std::string& _crsFormat ) const
{
  size_t x_pos = 0, y_pos = 0;
  bool isOk = true;
  if( _crsFormat.empty() || _crsFormat.length() < 5 )
    isOk = false;
  else {
    x_pos = _crsFormat.find( "%x" );
    if( x_pos == std::string::npos )
      isOk = false;
    else {
      y_pos = _crsFormat.find( "%y" );
      if( y_pos == std::string::npos || y_pos < x_pos )
        isOk = false;
    }
  }

  std::stringstream ss;
  if( isOk ) {
    ss << _crsFormat.substr( 0, x_pos ) << x
       << _crsFormat.substr( x_pos+2, y_pos-x_pos-2 ) << y
       << _crsFormat.substr( y_pos+2 );
  }
  else
    ss << "[" << x << ", " << y << "]";
  return ss.str();
}

const std::string Point::POS2D = "(%x, %y)";
const std::string Point::POS3D = "(%x, %y, %z)";

std::string Point::ToString( const std::string& _crsFormat ) const
{
  size_t x_pos = 0, y_pos = 0, z_pos = 0;
  bool isOk = true;
  if( _crsFormat.empty() || _crsFormat.length() < 5 )
    isOk = false;
  else {
    x_pos = _crsFormat.find( "%x" );
    if( x_pos == std::string::npos )
      isOk = false;
    else {
      y_pos = _crsFormat.find( "%y" );
      if( y_pos == std::string::npos || y_pos < x_pos )
        isOk = false;
      else {
        z_pos = _crsFormat.find( "%z" );
        if( z_pos < y_pos )
          isOk = false;
      }
    }
  }

  std::stringstream ss;
  if( isOk ) {
    ss << _crsFormat.substr( 0, x_pos ) << x
       << _crsFormat.substr( x_pos+2, y_pos-x_pos-2 ) << y;
    if( z_pos != std::string::npos )
      ss << _crsFormat.substr( y_pos+2, z_pos-y_pos-2 ) << z
         << _crsFormat.substr( z_pos+2 );
    else
      ss << _crsFormat.substr( y_pos+2 );
  }
  else
    ss << "(" << x << ", " << y << ")";
  return ss.str();
}

Point operator* ( double k, const Point& p ) { return p * k; }
Point operator/ ( double k, const Point& p ) { return Point( k / p.x, k / p.y, ( p.z != 0 ) ? k / p.z : 0. ); }
Point fabs( const Point& p ) { return Point( fabs( p.x ), fabs( p.y ), fabs( p.z ) ); }
Point signOf( const Point& p ) { return Point( dg_sign( p.x ), dg_sign( p.y ), dg_sign( p.z ) ); }
Point Min( const Point& p1, const Point& p2 ) { return Point( p1.x < p2.x ? p1.x : p2.x, p1.y < p2.y ? p1.y : p2.y ); }
Point Max( const Point& p1, const Point& p2 ) { return Point( p1.x > p2.x ? p1.x : p2.x, p1.y > p2.y ? p1.y : p2.y ); }
double hypot( const Point& p ) { return hypot( p.x, p.y ); }

PointIter dg__InitPair( PointIter itBegin, PointPtr pPnt2 )
{
  *pPnt2 = *itBegin;
  return itBegin + 1;
}

bool dg__CheckPair( PointIter itCur, PointIter itEnd, PointPtr pPnt1, PointPtr pPnt2 )
{
  if( itCur != itEnd ) {
    *pPnt1 = *pPnt2;
    *pPnt2 = *itCur;
    return true;
  }
  else
    return false;
}

PointIterConst dg__InitPairConst( PointIterConst itBegin, PointPtr pPnt2 )
{
  *pPnt2 = *itBegin;
  return itBegin + 1;
}

bool dg__CheckPairConst( PointIterConst itCur, PointIterConst itEnd, PointPtr pPnt1, PointPtr pPnt2 )
{
  if( itCur != itEnd ) {
    *pPnt1 = *pPnt2;
    *pPnt2 = *itCur;
    return true;
  }
  else
    return false;
}
