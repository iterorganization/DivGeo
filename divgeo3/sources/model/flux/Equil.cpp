#include "Equil.h"
#include "../core/Model.h"

Equil::Equil( ModelPtr _pModel, const std::string& _fName ):
  IComponent( OT::EQUIL, _pModel ),
  fileName( _fName ),
  sender_name( "Equil" ),
  x( null ),
  y( null ),
  val( null ),
  pSurface( null ),
  cache( null )
{ }

std::string Equil::Description() const
{ return pModel->GetStr( STR::EQUIL ); }

std::string Equil::ShortInfo() const
{ return "\"" + fileName + "\" " + size.ToString( NPoint::SIZE ); }

std::string Equil::DetailedInfo() const
{ return Description() + " " + ShortInfo(); }

Equil::~Equil() { Release(); }

void Equil::FreeArrays()
{
  if( x != null ) {
    delete[] x;
    x = null;
  }
  if( y != null ) {
    delete[] y;
    y = null;
  }
  if( val != null ) {
    delete[] val;
    val = null;
  }
  size = 0;
}

void Equil::Delete()
{
  SENDER_NAME( "Delete" );
  ValidatePtr( pModel, "DelEquil" );
  if ( pModel->IsStricted() && this->IsLocked() )
    return; // ERR::LOCKED

  ActDelEquil( pModel, this, DO_AT_ONCE );
}


void Equil::Release()
{
  FreeSplines();
  FreeCache();
  FreeArrays();
}


void Equil::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMax = *_pMin = Point( x[0], y[0] );

  for( int i = 1; i < size.x; i++ ) {
    _pMin->x = dg_min( _pMin->x, x[i] );
    _pMax->x = dg_max( _pMax->x, x[i] );
  }
  for( int i = 1; i < size.y; i++ ) {
    _pMin->y = dg_min( _pMin->y, y[i] );
    _pMax->y = dg_max( _pMax->y, y[i] );
  }
}


void Equil::AllocCache( NPoint _size )
{
  assert( _size != 0 );

  if( cache != NULL)
    FreeCache();

  cache = new double[_size.x*_size.y];
  for( int i = _size.x*_size.y - 1; i >= 0; i-- )
    cache[i] = MAXDOUBLE;
  cacheSize = _size;
}

void Equil::FreeCache()
{
  //assert( cache != NULL );
  if( cache != null ) {
    delete[] cache;
    cache = null;
  }
  cacheSize = 0;
}



bool Equil::In( const Point& xy0, const NPoint& _index1, const NPoint& _index2 ) const
{
  Point p1 = GetPoint( _index1 );
  Point p2 = GetPoint( _index2 );

  return !( xy0.x < dg_min( p1.x, p2.x ) || xy0.x > dg_max( p1.x, p2.x ) ||
            xy0.y < dg_min( p1.y, p2.y ) || xy0.y > dg_max( p1.y, p2.y ) );
}


bool Equil::InS( const Point& xy0, const NPoint& _index1, const NPoint& _index2 ) const
{
  Point p1 = GetPoint( _index1 );
  Point p2 = GetPoint( _index2 );

  return !( xy0.x <= dg_min( p1.x, p2.x ) || xy0.x >= dg_max( p1.x, p2.x ) ||
            xy0.y <= dg_min( p1.y, p2.y ) || xy0.y >= dg_max( p1.y, p2.y ) );
}

Point Equil::CenterOffset( NPoint _p0, NPoint _p1, double k ) const
{
  return Point( x[_p0.x] + (x[_p1.x] - x[_p0.x]) * k, y[_p0.y] + (y[_p1.y] - y[_p0.y]) * k );
}

double Equil::Cell( NPoint _index ) const
{
  return val[ size.x * _index.y + _index.x ];
}

double Equil::CorrCell( NPoint _index, double level ) const
{
  double a = Cell( _index );
  return ( a == level ) ? ( a + (maxVal - minVal) * 1e-7 ) : a;
}

double Equil::CellEx( NPoint _index, NPoint _size ) const
{
  double l;

  /* return EqCell(eq,cx,cy); */

  assert( _index >= 0 && _index < _size );
  int index = _index.x + _index.y * cacheSize.x;

  if( cache != NULL && _size == cacheSize )
  {
    l = cache[index];
    if( l != MAXDOUBLE )
      return l;
  }

  double _x = x[0] + ( x[size.x - 1] - x[0] ) * _index.x / (_size.x - 1);
  double _y = y[0] + ( y[size.y - 1] - y[0] ) * _index.y / (_size.y - 1);

  _x = dg_max( _x, dg_min( x[0], x[size.x - 1] ) );
  _x = dg_min( _x, dg_max( x[0], x[size.x - 1] ) );
  _y = dg_max( _y, dg_min( y[0], y[size.y - 1] ) );
  _y = dg_min( _y, dg_max( y[0], y[size.y - 1] ) );

  assert( GetLevel( Point( _x, _y ), &l, NULL ) == 0 );

  if( cache != NULL && size == cacheSize )
    cache[index] = l;

  return l;
}

double Equil::CorrCellEx( NPoint _index, double level, NPoint _size ) const
{
  double a = CellEx( _index, _size );
  return ( a == level ) ? ( a + (maxVal - minVal) * 1e-7 ) : a;
}


int Equil::Cell( Point _pnt, NPoint* _pIndex ) const
{
  int i = 0;
  for( i = 0; i < size.x - 1; i++ )
    if( dg_inrange( _pnt.x, x[i], x[i+1] ) ) {
      _pIndex->x = i;
      break;
    }
  if( i >= size.x - 1 )
    return -1;

  for( i = 0; i < size.y - 1; i++ )
    if( dg_inrange( _pnt.y, y[i], y[i+1] ) ) {
      _pIndex->y = i;
      break;
    }
  if( i >= size.y - 1 )
    return -1;

  return 0;
}

int Equil::CellEx( Point _pnt, NPoint* _pIndex, NPoint _size ) const
{
  /* return GetEquilCell(eq,x,y,px,py); */
  /* puts("EqCellX"); */

  if( !dg_inrange( _pnt.x, x[0], x[size.x-1] ) ) return -1;
  if( !dg_inrange( _pnt.y, y[0], y[size.y-1] ) ) return -1;

  _pIndex->x = (_pnt.x - x[0]) / (x[size.x-1] - x[0]) * (_size.x - 1);
  _pIndex->y = (_pnt.y - y[0]) / (y[size.y-1] - y[0]) * (_size.y - 1);

  /* *px=dg_min(*px,sx-2);
  *py=dg_min(*py,sy-2);
  *px=dg_max(*px,0);
  *py=dg_max(*py,0);*/
  /*printf("%d %d\n",*px,*py); */

  /* int i;
  for (i=0;i<eq->sx-1;i++)
    if (dg_inrange(x,eq->x[i],eq->x[i+1])) {*px=i;break;}
  if (i>=eq->sx-1) return -1;
  for (i=0;i<eq->sy-1;i++)
    if (dg_inrange(y,eq->y[i],eq->y[i+1])) {*py=i;break;}
  if (i>=eq->sy-1) return -1; */

  return 0;
}


double Equil::XYToBorder( Point pnt ) const
{
  double sum = 0;

  if( pnt.y == y[0] )
    return fabs( pnt.x - x[0] );
  sum += fabs( x[size.x - 1] - x[0] );

  if( pnt.x == x[size.x - 1])
    return sum + fabs( pnt.y - y[0] );
  sum += fabs( y[size.y - 1] - y[0]);

  if( pnt.y == y[size.y - 1] )
    return sum + fabs( pnt.x - x[size.x - 1]);
  sum += fabs( x[size.x - 1] - x[0] );

  if( pnt.x == x[0] )
    return sum + fabs( pnt.y - y[size.y - 1] );
  /* sum+=fabs(eq->y[eq->sy-1]-eq->y[0]); -- Not needed */

  return MAXDOUBLE; /* Not on equilibrium border */
}


bool EquilBorderCompare(const EquilBorderSort b1, const EquilBorderSort b2 )
{
  return b1.b < b2.b;
}
