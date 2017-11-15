#include "Sources.h"
#include "../core/Model.h"

std::string Source::Description() const
{ return pModel->GetStr( STR::SOURCE ); }

std::string Source::ShortInfo() const
{ return position.ToString(); }

std::string Source::DetailedInfo() const
{ return Description() + " " + ShortInfo(); }

void Source::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMin = *_pMax = position;
  *_pMin = Point( dg_min( _pMin->x, position.x ), dg_min( _pMin->y, position.y ) );
  *_pMax = Point( dg_max( _pMax->x, position.x ), dg_max( _pMax->y, position.y ) );
}

double Source::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{ UNUSED(_pPosFlag); return hypot( _crPnt.x - position.x, _crPnt.y - position.y ); }

bool Source::InRectangle( const Point& _p1, const Point& _p2 ) const
{ return PointInRectangle( position, _p1, _p2 ); }

int Source::Change( const Point& _crPosition )
{
  ActChangeSource actChSrc( pModel, this, _crPosition );
  return actChSrc.Do();  
}


void Source::Delete()
{
  assert( !pModel->HasHighlighted( this ) );
  Mark( false );

  DeleteContainers();
  DeleteVars();
  
  ActDelSource( pModel, this, DO_AT_ONCE );
}
