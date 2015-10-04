#include "Topology.h"
#include "../core/Model.h"

GridPointSegPtr Topology::AddGridPointSeg( XPointSegPtr _pXPS, int _zone )
{ /* $ */
  GridPointSegPtr pGPS = new GridPointSeg( pModel, _pXPS, _zone );
  ActAddGridPointSeg( pModel, pGPS, DO_AT_ONCE );
  return pGPS;
}

GridPointSegPtr Topology::FindGridPointSeg( int _zone ) const
{
  if( !gridPointSegs.empty() ) {
    FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs )
      if( pGPS->CheckZone( _zone ) )
        return pGPS;
  }
  return null;
}

GridPointSegPtr Topology::FindGridPointSegBySegment( XPointSegPtr _pXPS ) const
{
  if( !gridPointSegs.empty() ) {
    FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs )
      if( pGPS->XPS() == _pXPS )
        return pGPS;
  }

  return null;
}

void Topology::RecalcAllGridPointSegSLines()
{
  FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs ) {
    pGPS->XPS()->CalcLine();
    double len = pModel->Vars()->CalcTargetIntersection( pGPS->XPS()->Line(), null, null );
    UpdateSegment( pGPS, len );
  }
}


bool Topology::UpdateSegment( GridPointSegPtr _pGPS, double _newLength )
{
  if( _newLength == _pGPS->LineLength() )
    return false;

  _pGPS->XPS()->ChangeLineLength( _newLength );
  _pGPS->UpdateLineLength();
  pModel->Agent()->AddToUpdate( _pGPS, UT::UPDATE );
  pModel->Agent()->AddToUpdate( _pGPS->XPS(), UT::UPDATE );
  return true;
}

int Topology::NextGridPointSegNumber() const
{
  int nMax = GPSEG_STARTNO;
  if( !gridPointSegs.empty() ) {
    FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs ) {
      int zone = pGPS->Zone();
      if( zone + 1 > nMax )
        nMax = zone + 1;
    }
  }

  return nMax;
}


int Topology::HitGridPointPosition(const Point& _crPos, int* _pZone, double* _pValue ) const
{
  int zHit = -1;
  double distHit = MAXDOUBLE;
  double valueHit = 0.;
  GridPointSegPtr pGPS_Hit = null;
  FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs ) {
    if( !pGPS->HasFlag( GPSF::USED ) )
      continue;

    double value = 0.;
    double dist = pGPS->DistToPointEx( _crPos, &value );
    if( zHit < 0 || dist < distHit ) {
      zHit = pGPS->Zone();
      distHit = dist;
      valueHit = value;
      pGPS_Hit = pGPS;
    }
  }

  if( valueHit < 0.)
    valueHit = 0.;
  if( valueHit > 1. )
    valueHit = 1.;

  if( pGPS_Hit != NULL && pGPS_Hit->Dir() < 0 )
    valueHit = 1 - valueHit;

  if( zHit >= 0 ) {
    if( _pValue != NULL )
      *_pValue = valueHit;
    if( _pZone != NULL )
      *_pZone = zHit;
  }

  return (zHit < 0) ? -1 : 0;
}
