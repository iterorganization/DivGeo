#include "Topology.h"
#include "../core/Model.h"

SurfaceZonePtr Topology::AddSurfaceZone( int _szNo, int _gpsNo1, int _gpsNo2,
                                         int _orient, ElementPtr _pElem )
{
  assert( _orient == 1 || _orient == -1 );
  SurfaceZonePtr pSZ = new SurfaceZone( pModel, _szNo, _gpsNo1, _gpsNo2, _orient, _pElem );
  ActAddSurfaceZone( pModel, pSZ, DO_AT_ONCE );
  RecalcSurfaceZoneSign( pSZ );
  return pSZ;
}

SurfaceZonePtr Topology::FindSurfaceZone( int _zone ) const
{
  if( !surfaceZones.empty() ) {
    FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones )
      if( pSZ->CheckZone( _zone ) )
        return pSZ;
  }

  return NULL;
}


int Topology::GetSurfaceZoneByXY( const Point& _crPnt, double* _pLevel, int* _pErr ) const
{
  SENDER_NAME( "GetSurfaceZoneByXY" );
  int pErrBackup;
  if( _pErr == NULL )
    _pErr = &pErrBackup;

  double level;
  if( pEquil->GetLevel( _crPnt, &level, null ) != 0 ) {
    *_pErr = pModel->SendMessage( LOG_WARNING, SENDER, ERR::OUTOFEQUIL, ARGS( _crPnt.ToString() ) );
    return -1;
  }

  SurfaceZoneList zones;
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones )
    if( SurfaceZoneLevelOk( pSZ, level ) )
      zones.push_back( pSZ );

  if( zones.size() < 1 ) {
    zones.clear();
    *_pErr = pModel->SendMessage( LOG_WARNING, SENDER, ERR::NOSURFZONE, ARGS( _crPnt.ToString() ) );
    return -1;
  }

  SurfaceZoneIter itSZ, ENDITER = zones.end();
  /*FOREACH_NODECL_CONST( itSZ, zones )
    if( !(*itSZ)->HasBounds() )
      break;

   MB: Calculation method in const method. Extracted to all non-const callers
  if( itSZ != zones.end() )
    RecalcTopologyCache();
  */

  double dMin = MAXDOUBLE;
  SurfaceZonePtr pSZ_Min = null;
  FOREACH_NODECL_CONST( itSZ, zones ) {
    pSZ = (*itSZ);
    if( !pSZ->HasBounds() )
      continue;

    /* gps=FindGridPointSeg(a,sz->gpZone1);
    if (gps==NULL) continue; -- old */

    double d = pSZ->CalcDistToBounds( _crPnt );
    if( d < dMin ) {
      dMin = d;
      pSZ_Min = pSZ;
    }
  }

  /* $ */
  /* if (GroupCount(g)>1) {
    FreeGroup(g);
    *pErr=ERR::NOSONNET;
    return -1;
  }

  sz=Group1st(g,NULL); */

  zones.clear();
  pSZ = pSZ_Min;

  if( pSZ == NULL ) {
    *_pErr = pModel->SendMessage( LOG_WARNING, SENDER, ERR::NOSURFZONE, ARGS( _crPnt.ToString() ) );
    return -1;
  }

  if( _pLevel != NULL )
    *_pLevel = level;

  *_pErr = 0;
  return pSZ->Zone();
}


int Topology::SurfaceZoneLevelOk( SurfaceZonePtr _pSZ, double _level ) const
{
  //SENDER_NAME( "Equil::SurfaceZoneLevelOk" );
  GridPointSegPtr pGPS = FindGridPointSeg( _pSZ->GPZone(1) );
  if( pGPS == NULL )
    return 0; /*MODEL_ERROR_0( ERR::BADGPZONENUMBER ); */

  double l = pGPS->GetXpsLevel();
  if( (_level - l) * _pSZ->Sign() < 0 )
    return 0; /* MODEL_ERROR_0( ERR::OUTOFSURFZONE ); */

  if( _pSZ->GPZone(2) >= 0 ) {
    GridPointSegPtr pGPS2 = FindGridPointSeg( _pSZ->GPZone(2) );
    if( pGPS2 == NULL )
      return 0; /* MODEL_ERROR_0( ERR::BADGPZONENUMBER ); */

    pGPS2->GetXpsLevel();
    if( (_level - l) * _pSZ->Sign() < 0 )
      return 0; /* MODEL_ERROR_0( ERR::OUTOFSURFZONE ); */
  }

  return 1;
}



int Topology::CheckSurfaceZoneData( int _szNo, int _gpsNo1, int _gpsNo2, int _sign ) const
{
  UNUSED( _szNo );
  SENDER_NAME( "CheckSurfaceZoneData" );
  GridPointSegPtr pGPS = FindGridPointSeg( _gpsNo1 );
  if( pGPS == NULL )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADGPZONENUMBER, ARG( _szNo )_ARG( _gpsNo1 ) );

  if( _gpsNo2 >= 0 ) {
    GridPointSegPtr pGPS2 = FindGridPointSeg( _gpsNo2 );
    if( pGPS2 == NULL )
      return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADGPZONENUMBER, ARG( _szNo )_ARG( _gpsNo2 ) );
    if( _sign > 0 && pGPS2->GetXpsLevel() < pGPS->GetXpsLevel() )
      return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADSURFACEZONE1 );
    if( _sign < 0 && pGPS2->GetXpsLevel() > pGPS->GetXpsLevel() )
      return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADSURFACEZONE1 );
  }
  return 0;
}


bool Topology::SurfaceZoneBoundsCached() const
{
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones ) {
    if( !pSZ->HasBounds() )
      return false;
  }
  return true;
}

void Topology::RecalcAllSurfaceZonesSignes() const
{
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones )
    RecalcSurfaceZoneSign( pSZ );
}


int Topology::RecalcSurfaceZoneSign( SurfaceZonePtr _pSZ ) const
{
  SENDER_NAME( "RecalcSurfaceZoneSign" );
  GridPointSegPtr pGPS = FindGridPointSeg( _pSZ->GPZone(1) );
  if( pGPS == NULL )
    return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADGPZONENUMBER,
                                ARG( _pSZ->Zone() )_ARG( _pSZ->GPZone(1) ) );

  /* Check the level to the "left" side */
  int size = pGPS->LineSize() + 1;
  if( size < 3)
    return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSURFACEZONE1 ); /* $ - change error msg */

  int halfsize = size / 2;
  int halfsize1 = halfsize + 1;
  if( halfsize1 < size - 1 )
    halfsize1++;

  Point xy = pGPS->GetPoint( halfsize );
  double level;
  Point grad;
  if( pEquil->GetLevel( xy, &level, &grad ) )
    return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSURFACEZONE1 );  /* $ - change error msg */

  /* Angle between the line and the gradient */
  Point xy1 = pGPS->GetPoint( halfsize1 );
  double sina = pGPS->Dir() * (grad.y * (xy1.x - xy.x) - grad.x * (xy1.y - xy.y));
  _pSZ->SetSign( (sina < 0)? -1 : 1 );

  return 0;
}


bool Topology::HasDuplicatedZones( int _zone, SurfaceZonePtr _pSZ ) const
{
  int count = 0;
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones )
    if( pSZ->Zone() == _zone && pSZ != _pSZ )
      count++;
  return count > 1;
}

void Topology::SortZonesAndSegments()
{
  gridPointSegs.sort( GridPointSegCompare );
  surfaceZones.sort( SurfaceZoneCompare );
}

int Topology::FindSurfaceOriginPointEx( int _zone, double _level, double* _px, double* _py ) const
{
  SENDER_NAME( "FindSurfaceOriginPointEx" );
  bool bPeakSearch = (_level == MAXDOUBLE);

  if( bPeakSearch )
      _level = 0.; // to prevent crash while printing MAXDOUBLE value for warning

  SurfaceZonePtr pSZ = FindSurfaceZone( _zone );
  if( pSZ == NULL )
    return pModel->SendMessage( LOG_WARNING, SENDER, ERR::BADSURFZONENUMBER, ARG( _zone ) );

  GridPointSegPtr pGPS = FindGridPointSeg( pSZ->GPZone(1) );
  if( pGPS == NULL )
    return pModel->SendMessage( LOG_WARNING, SENDER, ERR::BADGPZONENUMBER_LVL,
                                ARG( _level )_ARG( _zone )_ARG( pSZ->GPZone(1) ) );

  double gpsLevel = pGPS->GetXpsLevel();
  if( !bPeakSearch && ((_level - gpsLevel) * pSZ->Sign()) < 0 )
    return pModel->SendMessage( LOG_WARNING, SENDER, ERR::OUTOFSURFZONE,
                                ARG( _zone )_ARG( _level )_ARG( gpsLevel ) );

  if( pSZ->GPZone(2) >= 0 ) {
    GridPointSegPtr pGPS2 = FindGridPointSeg( pSZ->GPZone(2) );
    if( pGPS2 == NULL )
      return pModel->SendMessage( LOG_WARNING, SENDER, ERR::BADGPZONENUMBER_LVL,
                                  ARG( _level )_ARG( _zone )_ARG( pSZ->GPZone(2) ) );
    double gps2Level = pGPS2->GetXpsLevel();
    if( !bPeakSearch && ((_level - gps2Level) * pSZ->Sign()) > 0 )
      return pModel->SendMessage( LOG_WARNING, SENDER, ERR::OUTOFSURFZONE,
                                  ARG( _zone )_ARG( _level )_ARG( gps2Level ) );
  }

  Point xy = pGPS->GetMiddlePoint();

  NPoint index;
  int i = pEquil->Cell( xy, &index );

  if( i != 0 )
    return pModel->SendMessage( LOG_WARNING, SENDER, ERR::FSOPX_1 ); /* Out of equilibrium */

  /* Direct hit - return immediately */
  if( pEquil->Cell( index ) == _level ) {
    Point p = pEquil->GetPoint( index );
    *_px = p.x;
    *_py = p.y;

    return 0;
  }

  /* Initialize the peak search flag */
  if( bPeakSearch ) {
    _level = ( pSZ->Sign() < 0 ) ? -MAXDOUBLE : MAXDOUBLE;
  }

  /* Determine the needed gradient sign */
  int gradSign = pEquil->Cell( index ) > _level ? -1 : 1;

  double l, ol;
  NPoint oIndex;
  /* gradSign>0: go "upwards"  gradSign<0: go "downwards" */
  while( 1 ) {
    oIndex = index;

    if( oIndex.x > 1 && ((pEquil->Cell( NPoint( oIndex.x - 1, oIndex.y ) ) -
                          pEquil->Cell( index )) * gradSign) > 0) {
      index = NPoint( oIndex.x - 1, oIndex.y );
    }
    if( oIndex.y > 1 && ((pEquil->Cell( NPoint( oIndex.x, oIndex.y - 1 ) ) -
                          pEquil->Cell( index )) * gradSign) > 0) {
      index = NPoint( oIndex.x, oIndex.y - 1 );
    }
    if( oIndex.x < pEquil->Size().x - 2 && ((pEquil->Cell( NPoint( oIndex.x + 1, oIndex.y ) ) -
                                             pEquil->Cell( index )) * gradSign) > 0) {
      index = NPoint( oIndex.x + 1, oIndex.y );
    }
    if( oIndex.y < pEquil->Size().y - 2 && ((pEquil->Cell( NPoint( oIndex.x, oIndex.y + 1 ) ) -
                                             pEquil->Cell( index )) * gradSign) > 0) {
      index = NPoint( oIndex.x, oIndex.y + 1 );
    }
    if( oIndex == index ) {
      if( bPeakSearch )
        break;
      else
        return pModel->SendMessage( LOG_WARNING, SENDER, ERR::LEVELNOTFOUND );
    }

    ol = pEquil->Cell( oIndex );
    l = pEquil->Cell( index );

    if( dg_min( l, ol ) <= _level && dg_max( l, ol ) >= _level )
      break;
  }

  //TODO: WTF?? Found:

  if( bPeakSearch || l == ol )
    l = 0;
  else
    l = (_level - ol) / (l - ol);

  Point oPnt = pEquil->GetPoint( oIndex );
  Point pnt = oPnt + (pEquil->GetPoint( index ) - oPnt) * l;
  *_px = pnt.x;
  *_py = pnt.y;

  return 0;
}


int Topology::GetCarreSurfaceExCount( int _zone, int _nSpec ) const
{
  SurfaceZonePtr pSZ = FindSurfaceZone( _zone );
  if( pSZ == null )
    return 0;

  return ( pSZ->GPZone(2) >= 0 ) ? _nSpec : _nSpec + 1;
}
