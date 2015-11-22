#include "FluxModel.h"

int FluxModel::WriteTargetsFile( FILE* _pFile ) const
{
  SENDER_NAME( "WriteTargetsFile" );
  int result = 0;

  /* Prepare sorted lists */
  gpSegments_sorted = pTopology->GridPointSegs();
  gpSegments_sorted.sort( GridPointSegCompare );

  surfaceZones_sorted = pTopology->SurfaceZones();
  surfaceZones_sorted.sort( SurfaceZoneCompare );

  /* Output targets */
  bool bCheck = !( pModel->HasOutputFlag( OF::TARGETS ) );
  if( bCheck && result != 0 )
    return result;

  WriteTargets_Surfaces( _pFile );
  WriteTargets_GridPoints( _pFile );

  /* Unknown code
  if (a->xpoint!=NULL) for (i=0;i<3;i++) {
    zfprintf(f,"zone %d\npoints\n",i);
    for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix))
      if (gp->area==i) zfprintf(f,"  %e\n",gp->value);
  } */

  /* Carre extras */
  if( pModel->CarreOutputMode() ) {
    /* Surfaces */
    bCheck = !pModel->HasOutputFlag( OF::SURFACES );

    IComponentList surfaces;
    result = Carre_CheckSurfaces( /*out*/surfaces );

    if( result == 0 )
      Carre_WriteSurfaces( _pFile, /*in*/surfaces );

    if( result != 0 && bCheck )
      return result;

    result = Carre_WriteInnermostSurface( _pFile );

    if( result != 0 && bCheck )
      return result;

    /* GridPoints */
    bCheck = !pModel->HasOutputFlag( OF::GRIDPOINTS );

    IComponentList gridPoints;
    result = Carre_CheckGridPoints( /*out*/gridPoints );

    if( result == 0 )
      Carre_WriteGridPoints( _pFile, /*in*/gridPoints );

    if( result != 0 && bCheck )
      return result;
  }

  return result;
}


void FluxModel::WriteTargets_Surfaces( FILE* _pFile ) const
{
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones_sorted ) {
    GridPointSegPtr pGPS = pTopology->FindGridPointSeg( pSZ->GPZone(1) );
    double level = (pGPS == null) ? 0. : pGPS->Level();
    IComponentList surfaces;
    FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
      if( pSX->CoordsOk() && pSX->Zone() == pSZ->Zone() )
        surfaces.push_back( pSX );
    }
    surfaces.sort( SurfaceCompare( level ) );

    zfprintf( _pFile, "region %d\nlevels\n", pSZ->Zone() );
    FOREACHPTRCONST( SurfaceExPtr, pSX2, surfaces )
      zfprintf( _pFile, "  %e\n", pSX2->Level() );
    surfaces.clear();
  }
}


void FluxModel::WriteTargets_GridPoints( FILE* _pFile ) const
{
  FOREACHPTRCONST( GridPointSegPtr, pGPS, gpSegments_sorted ) {
    if( !pGPS->IsUsed() )
      continue;
    IComponentList gridPoints;
    FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
      if( pGPX->CoordsOk() && pGPX->Zone() == pGPS->Zone() )
        gridPoints.push_back( pGPX );
    }
    gridPoints.sort( GridPointExCompare );

    zfprintf( _pFile, "zone %d\npoints\n", pGPS->Zone() );
    FOREACHPTRCONST( GridPointExPtr, pGPX2, gridPoints )
      zfprintf( _pFile, "  %e\n", pGPX2->Value() );
    gridPoints.clear();
  }
}

int FluxModel::Carre_CheckSurfaces( IComponentList& _rCreators_SX ) const
{
  SENDER_NAME( "Carre_CheckSurfaces" );
  int result = 0;

  /* Unknown code
    for (i=0;i<=3;i++) cid[i]=NULL; */

  CreatorId crid;
  _rCreators_SX.clear();
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
    if( !pSX->CoordsOk() )
      continue;

    SurfaceExPtr pSX2 = null;
    FOREACHPTRCONST( SurfaceExPtr, pSX2_tmp, _rCreators_SX ) {
      if( pSX2_tmp->Zone() == pSX->Zone() ) {
        pSX2 = pSX2_tmp;
        break;
      }
    }

    if( pSX2 == null ) {
      _rCreators_SX.push_back( pSX );
      crid = pSX->GetCreatorId();

      int surfaces_num = CountSurfaces( pSX->Zone() );
      int carre_surface_num = pTopology->GetCarreSurfaceExCount( pSX->Zone(), crid.Count() );

      if( !crid.IsUnchanged() || !crid.IsGood() || crid.CarreFlag() == 0 ||
          surfaces_num != carre_surface_num )
        result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_SURFACES );

      if( result == 0 ) {
        CreatorId crid2 = crid;
        int level = FindCarreMinMaxSurfaceLevel( pSX->Zone(), &crid2.level1, &crid2.level2, null );
        if( level != 0 || crid != crid2 )
          result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_TANGENT );
      }
    }
    else if( result == 0 && pSX->GetCreatorId() != crid )
      result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_SURFACES );
  }
  return result;
}


void FluxModel::Carre_WriteSurfaces( FILE* _pFile, const IComponentList& _crCreators_SX ) const
{
  ulong table_size = 0;
  std::vector< double > vDelta1;
  std::vector< double > vDelta2;
  std::vector< int > vCount;

  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones_sorted ) {
    FOREACHPTRCONST( SurfaceExPtr, pSX, _crCreators_SX )
      if( pSX->Zone() == pSZ->Zone() )
        break;
    CreatorId crid;
    if( pSX != null )
      crid = pSX->GetCreatorId();

    vDelta1.push_back( crid.Alpha1() * (crid.level2 - crid.level1) );
    vDelta2.push_back( crid.Alpha2() * (crid.level2 - crid.level1) );
    vCount.push_back( crid.Count() + 2 );
    table_size++;
  }

  zfprintf( _pFile, "dltr1\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %e\n", vDelta1[c] );
  zfprintf( _pFile, "dltrn\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %e\n", vDelta2[c] );
  zfprintf( _pFile, "npr\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %d\n", vCount[c] );
}

int FluxModel::Carre_WriteInnermostSurface( FILE* _pFile ) const
{
  SENDER_NAME( "Carre_WriteInnermostSurface" );
  int result = 0;
  zfprintf( _pFile, "pntrat\n" );

  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones_sorted ) {
    if( !pSZ->HasFlag( SZF::LIMITBYSURFACE ) )
      continue;

    GridPointSegPtr pGPS = pTopology->FindGridPointSeg( pSZ->GPZone(1) );
    if( pGPS == null ) {
      result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADGPZONENUMBER,
                                    ARG( pSZ->Zone() )_ARG( pSZ->GPZone(1) ) );
      break;
    }
    double level = pGPS->Level();
    SurfaceExPtr pSX2 = null;

    FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
      if( pSX->CoordsOk() &&
          pSX->Zone() == pSZ->Zone() &&
          (pSX2 == null ||
           fabs( pSX->Level() - level ) > fabs( pSX2->Level() - level ) ) ) //1410 '1.' -> 'level'
        pSX2 = pSX;
    }

    if( pSX2 == null )
      result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSURFACES );

    if( result == 0 ) {
      level = MAXDOUBLE; /* Workaround for the PC */
      Point p;
      result = pTopology->FindSurfaceOriginPointEx( pSZ->Zone(), level, &p.x, &p.y );

      if( result == 0 ) {
        PointArray pl;
        pl.push_back( pGPS->XPS()->XPT()->Center() );
        pl.push_back( p );

        result = PolyLinesIntersect( pl, *pSX2->Line(), &level, null );
        if( result != 0 )
          result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_PEAK );
      }
    }
    if( result == 0 ) {

      zfprintf( _pFile, "  %e\n", level );
    }
  }
  return result;
}


int FluxModel::Carre_CheckGridPoints( IComponentList& _rCreators_GPX ) const
{
  SENDER_NAME( "Carre_CheckGridPoints" );
  int result = 0;

  CreatorId crid;
  _rCreators_GPX.clear();
  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
    if( !pGPX->CoordsOk() )
      continue;

    FOREACHPTRCONST( GridPointExPtr, pGPX2, _rCreators_GPX )
      if( pGPX2->Zone() == pGPX->Zone() )
        break;

    if( pGPX2 == null ) {
      _rCreators_GPX.push_back( pGPX );
      crid = pGPX->GetCreatorId();
      if( !crid.IsUnchanged() || !crid.IsGood() || crid.CarreFlag() == 0 ||
          CountGridPoints( pGPX->Zone() ) != crid.Count() )
        result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_GPOINTS );
    }
    else if( pGPX->GetCreatorId() != crid )
      result = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_GPOINTS );
  }
  return result;
}


void FluxModel::Carre_WriteGridPoints( FILE* _pFile, const IComponentList& _rCreators_GPX ) const
{
  ulong table_size = 0;
  std::vector< double > vDelta1;
  std::vector< double > vDelta2;
  std::vector< int > vCount;

  FOREACHPTRCONST( GridPointSegPtr, pGPS, gpSegments_sorted ) {
    if( !pGPS->HasFlag( GPSF::USED ) )
      continue;
    FOREACHPTRCONST( GridPointExPtr, pGPX, _rCreators_GPX )
      if( pGPX->Zone() == pGPS->Zone() )
        break;
    CreatorId crid;
    if( pGPX != null )
      crid = pGPX->GetCreatorId();

    if( crid.IsGood() ) {
      vDelta1.push_back( crid.Alpha1() * pGPS->LineLength() );
      vDelta2.push_back( crid.Alpha2() * pGPS->LineLength() );
      vCount.push_back( crid.Count() + 2 );
      table_size++;
    }
  }

  zfprintf( _pFile, "dltp1\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %e\n", vDelta1[c] ); //1409 index fixed
  zfprintf( _pFile, "dltpn\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %e\n", vDelta2[c] ); //1409
  zfprintf( _pFile, "nptseg\n" );
  for( ulong c = 0; c < table_size; c++ )
    zfprintf( _pFile, "  %d\n", vCount[c] ); //1409
}
