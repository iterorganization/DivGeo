#include "GridPointSeg.h"
#include "../core/Model.h"

bool GridPointSegCompare(const IComponentPtr p1, const IComponentPtr p2 )
{
  assert( p1->Type() == OT::GRIDPOINTSEG &&
          p2->Type() == OT::GRIDPOINTSEG );

  GridPointSegPtr pGPS1 = dgtype_cast< GridPointSegPtr >( p1 );
  GridPointSegPtr pGPS2 = dgtype_cast< GridPointSegPtr >( p2 );

  return pGPS1->Zone() < pGPS2->Zone();
}

GridPointSeg::GridPointSeg( ModelPtr _pModel, XPointSegPtr _pXPS, int _zone ):
  IComponent( OT::GRIDPOINTSEG, _pModel ),
  zone( _zone ),
  dir( 1 ),
  targetDir( 1 ),

  shortName( "" ),
  longName( "" ),

  pXPS( DG_SHARE( _pXPS ) ),
  flags( 0 ),
  pLine( null )
{
  DG_PROPERTIES_INIT;
  lineLength = CalcLineLength( *pXPS->Line() );
  level = pXPS->GetXptLevel();
}

GridPointSeg::~GridPointSeg()
{
  Lock();
  ReleaseObject( (IComponentPtr*)&pXPS );
  Unlock();
}

std::string GridPointSeg::Description() const
{ return pModel->GetStr( STR::GRID_SEGMENT ); }

std::string GridPointSeg::ShortInfo() const
{ return shortName << " (" << zone << ")"; }

std::string GridPointSeg::DetailedInfo() const
{ return Description() << " " << zone << "-" << shortName << " (" << longName << ")"; }

double GridPointSeg::DistToPointEx( Point pnt, double* pos ) const
{
  double distHit = MAXDOUBLE;
  double valueHit = 0.;
  double s = 0.;
  PointIter itXY_first = pLine->begin() + 1;
  FOREACH_CONST( PointIterConst, itXY, (*pLine) ) {
    if( itXY == pLine->begin() )
      continue;
    Point xy1 = *(itXY - 1);
    Point xy = *itXY;
    double value = 0.;
    double dist = Point2VectorDist( xy, xy1, pnt, NULL, &value );
    double local = hypot( xy.x - xy1.x, xy.y - xy1.y ) / lineLength;
    value = s + value * local;
    if( itXY == itXY_first || dist < distHit ) {
      distHit = dist;
      valueHit = value;
    }
    s += local;
  }
  if( pos != null )
    *pos = valueHit;
  return distHit;
}

int GridPointSeg::CalcPointPosition( double value, Point* pPosition, Point* pDerivative ) const
{
  SENDER_NAME( "GridPointSeg::CalcPointPosition" );
  assert( pPosition != null && pDerivative != null );

  if( dir < 0 )
    value = 1 - value;

  if( pLine->empty() )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LINE_IS_EMPTY );

  double s = 0;
  FOREACH_CONST( PointIterConst, itXY, (*pLine) ) {
    PointIterConst itXY_next = itXY + 1;
    if( itXY_next == pLine->end() )
      break;
    double l = hypot( itXY->x - itXY_next->x, itXY->y - itXY_next->y ) / lineLength;
    if( l +s >= value ) {
      *pPosition = *itXY + (*itXY_next - *itXY) * (value - s) / l;
      *pDerivative = (*itXY_next - *itXY) / l / lineLength;
      return 0;
    }
    s += l;
  }

  *pPosition = pLine->back();
  *pDerivative = Point( 1., 0. );

  return 0;
}

double GridPointSeg::GetXpsLevel() const
{ 
  return pXPS->GetXptLevel(); 
}

void GridPointSeg::SetLine( void* p )
{
  pLine = (p == NULL) ? NULL : pXPS->Line();
}

void GridPointSeg::UpdateLineLength()
{
  if( pXPS != null )
    lineLength = pXPS->LineLength();
}

void GridPointSeg::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( !pLine->empty() ) {
    FOREACH_CONST( PointIterConst, itP, (*pLine) ) {
      Point xy = *itP;
      if( _pMin->x > _pMax->x ) {
        *_pMin = MAXDOUBLE;
        *_pMax = -MAXDOUBLE;
      }
      *_pMin = Point( dg_min( _pMin->x, xy.x ), dg_min( _pMin->y, xy.y ) );
      *_pMax = Point( dg_max( _pMax->x, xy.x ), dg_max( _pMax->y, xy.y ) );
    }
  }
}

void GridPointSeg::Delete()
{
  ActChangeValue( pModel, this, this, p_short_name, std::string(), REDRAW, DO_AT_ONCE );
  ActChangeValue( pModel, this, this, p_long_name, std::string(), REDRAW, DO_AT_ONCE );
  ActDelGridPointSeg( pModel, this, DO_AT_ONCE );
}


void GridPointSeg::SetZone( int _zone )
{ ActChangeValue( pModel, this, this, p_zone, _zone, REDRAW, DO_AT_ONCE ); }

void GridPointSeg::SetDir( int _dir )
{ ActChangeValue( pModel, this, this, p_dir, _dir, REDRAW, DO_AT_ONCE ); }

void GridPointSeg::SetTargetDir( int _targetDir )
{ ActChangeValue( pModel, this, this, p_target_dir, _targetDir, REDRAW, DO_AT_ONCE ); }

void GridPointSeg::SetShortName( const std::string& _shortName )
{ ActChangeValue( pModel, this, this, p_short_name, _shortName, REDRAW, DO_AT_ONCE ); }

void GridPointSeg::SetLongName( const std::string& _longName )
{ ActChangeValue( pModel, this, this, p_long_name, _longName, REDRAW, DO_AT_ONCE ); }

void GridPointSeg::SetFlags( long _flags )
{ ActChangeValue( pModel, this, this, p_flags, _flags, REDRAW, DO_AT_ONCE ); }
