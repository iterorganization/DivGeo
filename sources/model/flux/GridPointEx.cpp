#include "GridPointEx.h"
#include "../core/Model.h"

bool GridPointExCompare( const IComponentPtr p1, const IComponentPtr p2 )
{
  assert( p1->Type() == OT::GRIDPOINTEX &&
          p2->Type() == OT::GRIDPOINTEX );

  GridPointExPtr pGPX1 = dgtype_cast< GridPointExPtr >( p1 );
  GridPointExPtr pGPX2 = dgtype_cast< GridPointExPtr >( p2 );

  return pGPX1->Value() < pGPX2->Value();
}

GridPointEx::GridPointEx( ModelPtr _pModel, int _zone, double _value ):
  IComponent( OT::GRIDPOINTEX, _pModel ),
  zone( _zone ),
  bDrawn( 1 ),
  bCoordsOk( 0 ),
  value( _value ),
  crid()
{ DG_PROPERTIES_INIT; }

GridPointEx::~GridPointEx()
{
  /*Lock();
  Unlock();*/
}

double GridPointEx::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{ UNUSED(_pPosFlag); return hypot( _crPnt.x - position.x, _crPnt.y - position.y ); }

std::string GridPointEx::Description() const
{ return pModel->GetStr( STR::GRIDPOINT ); }

std::string GridPointEx::ShortInfo() const
{ return ToStdString( value ) + " " + position.ToString(); }

std::string GridPointEx::DetailedInfo() const
{ return Description() + " " /*+ ShortInfo()*/ + " @" + zone; }

void GridPointEx::Delete()
{
  ChangeCreatorId( CreatorId() );
  ActDelGridPointEx( pModel, this, DO_AT_ONCE );
}

int GridPointEx::ChangeCreatorId(const CreatorId& _crCrid )
{
  if( _crCrid == crid )
    return 0;

  ActChangeValue( pModel, this, this, p_creator_id, _crCrid, NO_REDRAW, DO_AT_ONCE );
  return 0;
}

int GridPointEx::RecalcPos()
{
  bCoordsOk = 0;
  GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSeg( zone );
  if( pGPS == NULL )
    return -1;

  pGPS->CalcPointPosition( value, &position, &derivative );
  bCoordsOk = 1;

  return 0;
}


void GridPointEx::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( bCoordsOk != 0 ) {
    if( _pMin->x > _pMax->x ) {
      *_pMin = MAXDOUBLE;
      *_pMax = -MAXDOUBLE;
    }
    *_pMin = Point( dg_min( _pMin->x, position.x ), dg_min( _pMin->y, position.y ) );
    *_pMax = Point( dg_max( _pMax->x, position.x ), dg_max( _pMax->y, position.y ) );
  }
}


int GridPointEx::Change( int _zone, double _value )
{
  SENDER_NAME( "GridPointEx::Change" );
  if( _value < 0 || _value > 1 )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::VALUE_OUT_OF_RANGE,
                         ARG( 0. )_ARG( 1. ) ); /* relcheck_ignore_line */

  ActChangeGridPointEx( pModel, this, _zone, _value, DO_AT_ONCE );
  
  if( crid.IsUnchanged() )
    ChangeCreatorId( crid.ChangedCopy() );

  return 0;
}
