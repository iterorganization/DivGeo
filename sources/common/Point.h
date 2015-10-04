#ifndef dg_sign
#define dg_sign(a) ((a)<0 ? -1 : ((a)>0 ? 1 : 0))
#endif

#ifndef POINT_H
#define POINT_H

#include <vector>
#include <string>
#include <sstream>
#include <list>
#include "math.h"

struct Point;

typedef struct NPoint
{
  int x, y;
  NPoint(): x( 0. ), y( 0. ) {} 
  NPoint( int v ): x( v ), y( v ) {} 
  NPoint( int _x, int _y ): x( _x ), y( _y ) {}
  NPoint( const NPoint& p ): x( p.x ), y( p.y ) {}

  bool operator == ( const NPoint& p ) const { return x == p.x && y == p.y; }
  bool operator == ( int v ) const { return x == v && y == v; }
  bool operator != ( const NPoint& p ) const { return x != p.x || y != p.y; }
  bool operator != ( int v ) const { return x != v || y != v; }

  NPoint operator* ( int k ) const { return NPoint( x * k, y * k ); }
  NPoint operator/ ( int k ) const { return NPoint( x / k, y / k ); }  
  NPoint operator+ ( int v ) const { return NPoint( x + v, y + v ); }  
  NPoint operator- ( int v ) const { return NPoint( x - v, y - v ); }

  NPoint operator* ( const NPoint& p ) const { return NPoint( x * p.x, y * p.y ); }
  NPoint operator/ ( const NPoint& p ) const { return NPoint( x / p.x, y / p.y ); }  
  NPoint operator+ ( const NPoint& p ) const { return NPoint( x + p.x, y + p.y ); }
  NPoint operator- ( const NPoint& p ) const { return NPoint( x - p.x, y - p.y ); }

  NPoint& operator*= ( int k ) { x *= k; y *= k; return *this; }
  NPoint& operator/= ( int k ) { x /= k; y /= k; return *this; }
  NPoint& operator+= ( int v ) { x += v; y += v; return *this; }
  NPoint& operator-= ( int v ) { x -= v; y -= v; return *this; }

  NPoint& operator*= ( const NPoint& p ) { x *= p.x; y *= p.y; return *this; }
  NPoint& operator/= ( const NPoint& p ) { x /= p.x; y /= p.y; return *this; }
  NPoint& operator+= ( const NPoint& p ) { x += p.x; y += p.y; return *this; }
  NPoint& operator-= ( const NPoint& p ) { x -= p.x; y -= p.y; return *this; }

  bool operator< ( int v ) const { return  x < v && y < v; }
  bool operator> ( int v ) const { return  x > v && y > v; }
  bool operator<= ( int v ) const { return  x <= v && y <= v; }
  bool operator>= ( int v ) const { return  x >= v && y >= v; }

  bool operator< ( const NPoint& p ) const { return  x < p.x && y < p.y; }  
  bool operator> ( const NPoint& p ) const { return  x > p.x && y > p.y; }
  bool operator<= ( const NPoint& p ) const { return  x <= p.x && y <= p.y; }  
  bool operator>= ( const NPoint& p ) const { return  x >= p.x && y >= p.y; }

  static const std::string INDEX;
  static const std::string SIZE;
  static const std::string POSITION;

  std::string ToString( const std::string& _crsFormat = "" ) const;

}* NPointPtr;

typedef struct Point
{
  double x, y, z;
  Point(): x( 0. ), y( 0. ), z( 0. ) {} 
  Point( double v ): x( v ), y( v ), z( v ) {} 
  Point( double _x, double _y ): x( _x ), y( _y ), z( 0. ) {}
  Point( double _x, double _y, double _z ): x( _x ), y( _y ), z( _z ) {}
  Point( const Point& p ): x( p.x ), y( p.y ), z( p.z ) {}
  Point( const NPoint& p ): x( p.x ), y( p.y ), z( 0. ) {}

  bool operator == ( const Point& p ) const { return x == p.x && y == p.y && z == p.z; }  //TODO:VERSION accuracy
  bool operator == ( double v ) const { return x == v && y == v && z == v; }
  bool operator != ( const Point& p ) const { return x != p.x || y != p.y || z != p.z; }
  bool operator != ( double v ) const { return x != v || y != v || z != v; }

  Point operator* ( double k ) const { return Point( x * k, y * k, z * k ); }
  Point operator/ ( double k ) const { return Point( x / k, y / k, z / k ); }  
  Point operator+ ( double v ) const { return Point( x + v, y + v, z + v ); }  
  Point operator- ( double v ) const { return Point( x - v, y - v, z - v ); }

  Point operator* ( const Point& p ) const { return Point( x * p.x, y * p.y, z * p.z ); }
  Point operator/ ( const Point& p ) const { return Point( x / p.x, y / p.y, ( p.z != 0 ) ? z / p.z : z ); }  
  Point operator+ ( const Point& p ) const { return Point( x + p.x, y + p.y, z + p.z ); }
  Point operator- ( const Point& p ) const { return Point( x - p.x, y - p.y, z - p.z ); }

  Point& operator*= ( double k ) { x *= k; y *= k; z *= k; return *this; }
  Point& operator/= ( double k ) { x /= k; y /= k; z /= k; return *this; }
  Point& operator+= ( double v ) { x += v; y += v; z += v; return *this; }
  Point& operator-= ( double v ) { x -= v; y -= v; z -= v; return *this; }

  Point& operator*= ( const Point& p ) { x *= p.x; y *= p.y; z *= p.z; return *this; }
  Point& operator/= ( const Point& p ) { x /= p.x; y /= p.y; if( p.z != 0 ) z /= p.z; return *this; }
  Point& operator+= ( const Point& p ) { x += p.x; y += p.y; z += p.z; return *this; }
  Point& operator-= ( const Point& p ) { x -= p.x; y -= p.y; z -= p.z; return *this; }

  bool operator< ( double v ) const { return  x < v && y < v && (z < v || (z == 0. && v == 0.)); }
  bool operator> ( double v ) const { return  x > v && y > v && (z > v || (z == 0. && v == 0.)); }
  bool operator<= ( double v ) const { return  x <= v && y <= v && z <= v; }
  bool operator>= ( double v ) const { return  x >= v && y >= v && z >= v; }

  bool operator< ( const Point& p ) const { return  x < p.x && y < p.y && (z < p.z || (z == 0. && p.z == 0.)); }  
  bool operator> ( const Point& p ) const { return  x > p.x && y > p.y && (z > p.z || (z == 0. && p.z == 0.)); }
  bool operator<= ( const Point& p ) const { return  x <= p.x && y <= p.y && z <= p.z; }  
  bool operator>= ( const Point& p ) const { return  x >= p.x && y >= p.y && z >= p.z; }

  operator NPoint() const { return NPoint( x, y ); }

  Point SwapXY() const { return Point( y, x, z ); }
  Point SwapYZ() const { return Point( x, z, y ); }

  std::string ToString( const std::string& _crsFormat = "" ) const;

  static const std::string POS2D;
  static const std::string POS3D;

}* PointPtr;

typedef std::vector< Point > PointArray;
typedef PointArray::iterator PointIter;
typedef PointArray::const_iterator PointIterConst;

Point operator* ( double k, const Point& p );
Point operator/ ( double k, const Point& p );
Point fabs( const Point& p );
Point signOf( const Point& p );
Point Min( const Point& p1, const Point& p2 );
Point Max( const Point& p1, const Point& p2 );
double hypot( const Point& p );

PointIter dg__InitPair( PointIter itBegin, PointPtr pPnt2 );
bool dg__CheckPair( PointIter itCur, PointIter itEnd, PointPtr pPnt1, PointPtr pPnt2 );
PointIterConst dg__InitPairConst( PointIterConst itBegin, PointPtr pPnt2 );
bool dg__CheckPairConst( PointIterConst itCur, PointIterConst itEnd, PointPtr pPnt1, PointPtr pPnt2 );

#ifndef CURRENTITER
  #define CURRENTITER dg__it__
  #define ENDITER dg__it_end__
#endif
#ifndef FOREACHPAIR
  #define FOREACHPAIR( pnt1, pnt2, grp ) \
    for( PointIter CURRENTITER = dg__InitPair( (grp).begin(), &(pnt2) ); \
         dg__CheckPair( CURRENTITER, (grp).end(), &(pnt1), &(pnt2) ); \
         ++CURRENTITER )

  #define FOREACHPAIRCONST( pnt1, pnt2, grp ) \
    for( PointIterConst CURRENTITER = dg__InitPairConst( (grp).begin(), &(pnt2) ), \
                        ENDITER = (grp).end(); \
         dg__CheckPairConst( CURRENTITER, ENDITER, &(pnt1), &(pnt2) ); \
         ++CURRENTITER )
#endif


#endif
