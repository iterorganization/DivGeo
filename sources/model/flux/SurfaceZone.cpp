#include "SurfaceZone.h"
#include "../core/Model.h"

bool SurfaceZoneCompare(const IComponentPtr p1, const IComponentPtr p2 )
{
  assert( p1->Type() == OT::SURFACEZONE &&
          p2->Type() == OT::SURFACEZONE );

  SurfaceZonePtr pSZ1 = dgtype_cast< SurfaceZonePtr >( p1 );
  SurfaceZonePtr pSZ2 = dgtype_cast< SurfaceZonePtr >( p2 );

  return pSZ1->Zone() < pSZ2->Zone();
}

SurfaceZone::SurfaceZone( ModelPtr _pModel, int _szNo, int _gpsNo1, int _gpsNo2, int _orient, ElementPtr _pInner ):
  IComponent( OT::SURFACEZONE, _pModel ),
  zone( _szNo ),

  shortName( "" ),
  longName( "" ),

  gpZone1( _gpsNo1 ), 
  gpZone2( _gpsNo2 ),
  zone_sign( 0 ),
  orient( _orient ),  

  flags( 0 ),
  level1( 0 ),
  level2( 0 ),

  innermost( DG_SHARE( _pInner ) )
{ DG_PROPERTIES_INIT; }

SurfaceZone::~SurfaceZone()
{
  Lock();
  ReleaseObject( (IComponentPtr*)&innermost );
  Unlock();
}

std::string SurfaceZone::Description() const
{ return pModel->GetStr( STR::SURFACE_ZONE ); }

std::string SurfaceZone::ShortInfo() const
{ return shortName + " (" + zone + ")"; }

std::string SurfaceZone::DetailedInfo() const
{ return Description() + " " + zone + "-" + shortName + " (" + longName + ")"; }


void SurfaceZone::Delete()
{
  ActChangeValue( pModel, this, this, p_short_name, std::string(), REDRAW, DO_AT_ONCE );
  ActChangeValue( pModel, this, this, p_long_name, std::string(), REDRAW, DO_AT_ONCE );
  ActDelSurfaceZone( pModel, this, DO_AT_ONCE );
}

void SurfaceZone::SetShortName( const std::string& _crsShortName )
{
  ActChangeValue( pModel, this, this, p_short_name, _crsShortName, REDRAW, DO_AT_ONCE );
}

void SurfaceZone::SetLongName( const std::string& _crsLongName )
{
  ActChangeValue( pModel, this, this, p_long_name, _crsLongName, REDRAW, DO_AT_ONCE );
}

void SurfaceZone::SetFlags( long _flags )
{
  ActChangeValue( pModel, this, this, p_flags, _flags, REDRAW, DO_AT_ONCE );
}

void SurfaceZone::SetInnermost( ElementPtr _pElem )
{
  ReleaseObject( (IComponentPtr*)&innermost );
  innermost = DG_SHARE( _pElem );
}

void SurfaceZone::SetSign( int sign )
{
  zone_sign = sign * orient;
}
