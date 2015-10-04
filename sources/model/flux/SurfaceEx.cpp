#include "SurfaceEx.h"
#include "../core/Model.h"

bool SurfaceCompare::operator() ( const IComponentPtr p1, const IComponentPtr p2 ) const
{
  assert( p1->Type() == OT::SURFACEEX &&
          p2->Type() == OT::SURFACEEX );

  SurfaceExPtr pSX1 = dgtype_cast< SurfaceExPtr >( p1 );
  SurfaceExPtr pSX2 = dgtype_cast< SurfaceExPtr >( p2 );

  if( pSX1->Zone() != pSX2->Zone() )
    return pSX1->Zone() < pSX2->Zone();

  double l1 = fabs( pSX1->Level() - baseLevel );
  double l2 = fabs( pSX2->Level() - baseLevel );

  return l1 < l2;
}

SurfaceEx::SurfaceEx( ModelPtr _pModel, int _zone, double _level, Point _origin ):
  IComponent( OT::SURFACEEX, _pModel ),
  zone( _zone ),
  bDrawn( 1 ),
  bCoordsOk( 0 ),
  level( _level ),
  nLevel( 0. ),
  origin( _origin ),
  crid(),
  lastError( 0 ),  
  closed( -1 )
{ DG_PROPERTIES_INIT; }

SurfaceEx::~SurfaceEx()
{
  /*Lock();
  Unlock();*/
}

std::string SurfaceEx::Description() const
{ return pModel->GetStr( STR::SURFACE ); }

std::string SurfaceEx::ShortInfo() const
{ return ToStdString( level ); }

std::string SurfaceEx::DetailedInfo() const
{ return Description() << " " << level << " (" << zone << ")"; }

double SurfaceEx::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{
  UNUSED(_pPosFlag);
  double distHit = MAXDOUBLE;
  PointIterConst itXY_second = line.begin() + 1;
  FOREACH_CONST( PointIterConst, itXY_2, line ) {
    if( itXY_2 == line.begin() )
      continue;
    Point xy1 = *(itXY_2 - 1);
    Point xy2 = *itXY_2;

    double dist = Point2VectorDist( xy1, xy2, _crPnt, NULL, NULL );
    if( itXY_2 == itXY_second || dist < distHit )
      distHit = dist;
  }
  return distHit;
}

int SurfaceEx::InvalidatePos()
{
  if( bCoordsOk != 0 )
  {
    line.clear();
    bCoordsOk = 0;
  }
  else 
    assert( line.empty() );

  return 0;
}

int SurfaceEx::RecalcPos()
{
  SENDER_NAME( "SurfaceEx::RecalcPos" );
  InvalidatePos();

  EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();

  if( pEquil == null ) {
    lastError = pModel->SendMessage( WND_ERROR, SENDER, ERR::NOEQUIL );
    return lastError;
  }

  if( zone != SZN_XY ) {
    lastError = pModel->GetFluxModel()->GetTopology()->
                FindSurfaceOriginPointEx( zone, level, &origin.x, &origin.y );
    if( lastError != 0 )
      return lastError;
  }

  NPoint index;
  if( pEquil->CellEx( origin, &index, pEquil->Size() ) )
    return lastError = pModel->SendMessage( WND_ERROR, SENDER, ERR::OUTOFEQUIL,
                                            ARGS( origin.ToString() ) );

  double lvl;
  if( pEquil->GetLevel( origin, &lvl, null ) != 0 )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::LEVEL_PROBLEM );

  int r = pEquil->CalcSurfaceLine( index, lvl, &line, pEquil->Size() ); /* $$$ */
  if( r < 0 )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::FLAT_EQUIL );

  closed = r;
  if( zone == SZN_XY )
    level = lvl;

  bCoordsOk = true;

  return 0;
}

void SurfaceEx::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( !line.empty() ) {
    FOREACH_CONST( PointIterConst, itP, line ) {
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

int SurfaceEx::Change( int _zone, double _level, int* pErr )
{
  int pErrBackup;

  if( pErr == NULL )
    pErr = &pErrBackup;
  if( pErr != NULL )
    *pErr = 0;

  ActChangeSurfaceEx act( pModel, this, _zone, _level, DO_AT_ONCE );

  if( crid.IsUnchanged() )
    ChangeCreatorId( crid.ChangedCopy() );

  return 0;
}

int SurfaceEx::Change( Point pnt, int* pErr, bool bByXY )
{
  int pErrBackup;

  if( pErr == NULL )
    pErr = &pErrBackup;
  if( pErr != NULL )
    *pErr = 0;

  /* SurfaceExPtr pSX = null; */

  if( bByXY ) {
    double level = 0;
    int zone = pModel->GetFluxModel()->GetTopology()->GetSurfaceZoneByXY( pnt, &level, pErr );
    if( zone >= 0 )
      ActChangeSurfaceEx( pModel, this, zone, level, DO_AT_ONCE );
    else
      ActChangeSurfaceEx( pModel, this, pnt, DO_AT_ONCE );
  }
  else
    ActChangeSurfaceEx( pModel, this, pnt, DO_AT_ONCE );

  /*if( pSX == null )
    pSX = new SurfaceEx( pModel, SZN_XY, 0, pnt );

  ActAddSurfaceEx act( pModel, pSX, DO_AT_ONCE );*/

  if( crid.IsUnchanged() )
    ChangeCreatorId( crid.ChangedCopy() );

  return 0;
}


int SurfaceEx::ChangeCreatorId(const CreatorId &_crid )
{
  if( _crid == crid )
    return 0;

  ActChangeValue act( pModel, this, this, p_creator_id, _crid, NO_REDRAW, DO_AT_ONCE );
  return 0;
}



bool SurfaceEx::IsClosed() const
{
  if( !bCoordsOk )
    return 0;

  if( line.empty() || line.size() < 1 )
    return 0;

  Point xy = line.front();
  Point xy1 = line.back();
  
  return xy == xy1;
}


void SurfaceEx::Delete()
{
  ChangeCreatorId( CreatorId() );

  ActDelSurfaceEx actDelSX( pModel, this );
  actDelSX.Do();
}
