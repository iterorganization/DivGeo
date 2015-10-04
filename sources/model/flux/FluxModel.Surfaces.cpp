#include "FluxModel.h"

SurfaceExPtr FluxModel::AddSurfaceEx( int _zone, double _level, int* _pErr )
{
  int pErrBackup;

  if( _pErr == NULL )
    _pErr = &pErrBackup;
  if( _pErr != NULL )
    *_pErr = 0;

  SurfaceExPtr pSX = new SurfaceEx( pModel, _zone, _level, Point() );

  ActAddSurfaceEx( pModel, pSX, DO_AT_ONCE );
  return pSX;
}



SurfaceExPtr FluxModel::AddSurfaceEx( const Point& _crPnt, int* _pErr, bool _bByXY )
{
  int pErrBackup;

  if( _pErr == NULL )
    _pErr = &pErrBackup;
  if( _pErr != NULL )
    *_pErr = 0;

  SurfaceExPtr pSX = null;

  if( _bByXY ) {
    /* MB: Extracted from Equil::GetSurfaceZoneByXY() const */
    pTopology->RecalcCache();

    double level = 0;
    double zone = pTopology->GetSurfaceZoneByXY( _crPnt, &level, _pErr );
    if( zone >= 0 )
      pSX = new SurfaceEx( pModel, zone, level, Point() );
  }

  if( pSX == null )
    pSX = new SurfaceEx( pModel, SZN_XY, 0, _crPnt );

  ActAddSurfaceEx( pModel, pSX, DO_AT_ONCE );
  return pSX;

}



int FluxModel::CountSurfaces( int _area ) const
{
  int i = 0;

  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx )
    if( pSX->CoordsOk() && pSX->Zone() == _area )
      i++;

  return i;
}



/* Area==-1 means recursively process all areas */
int FluxModel::RebuildCarreSurfaces( int _area )
{
  SENDER_NAME( "RebuildCarreSurfaces" );
  if( !pModel->CarreOutputMode() )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::CARRE_MODE_NEEDED );

  if( _area < 0 ) {
    FOREACHPTRCONST( SurfaceZonePtr, pSZ, pTopology->SurfaceZones() ) {
      int r = RebuildCarreSurfaces( pSZ->Zone() );
      if( r != 0 )
        return pModel->SendMessage( WND_ERROR, SENDER, r );
    }
    return 0;
  }

  if( !CountSurfaces( _area ) )
    return 0;  /* $$ - maybe some errorcode? */

  CreatorId crid;
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
    if( pSX->Zone() != _area )
      continue;
    crid = pSX->GetCreatorId();
    if( !crid.IsGood() )
      continue;
    if( crid.CarreFlag() != 0 )
      break;
  }

  if( pSX == null )
    return 0;  /* $$ - maybe some errorcode? */

  return DistributeSurfaces( crid, null );
}


int FluxModel::DistributeSurfaces( const CreatorId& _crCrId, CreatorId* _pOldCrid )
{
  SENDER_NAME( "DistributeSurfaces" );
  int bosr;

  CreatorId crid_fixed = _crCrId;
  crid_fixed.ResetCarreFlag();

  /* MB: Extracted from Equil::GetSurfaceZoneByXY() const
         used by FindCarreMinMaxSurfaceLevel() */
  pTopology->RecalcCache();


  /* In Carre mode, determine the levels automatically */
  if( crid_fixed.CarreFlag() ) {
    int r = FindCarreMinMaxSurfaceLevel( crid_fixed.Area(),
      &crid_fixed.level1, &crid_fixed.level2, &bosr );
    if( r != 0)
      return ( r == ERR::OUTOFEQUIL ) ?
        pModel->SendMessage( WND_ERROR, SENDER, ERR::CARRE_OUTOFEQUIL ) : r;
  }

  /* Delete existing surfaces in Carre mode. Cannot be done earlier due to level calculations in main plasma */
  if( crid_fixed.CarreFlag() ) {
    IComponentList vSX_copy = surfacesEx;
    FOREACHPTRCONST( SurfaceExPtr, pSX, vSX_copy ) {
      if( pSX->Zone() == crid_fixed.Area() )
        pSX->Delete();
    }
    vSX_copy.clear();
  }

  /* Create surfaces */
  int cnt = crid_fixed.Count();

  /* Carre mode: one extra bounding surface */
  if( crid_fixed.CarreFlag() && bosr )
    cnt++;

  for( int i = 0; i < cnt; i++ ) {
    double v = crid_fixed.CalcValue( (i + 1) / (double)(crid_fixed.Count() + 1) );
    v = dg_max( v, 0 );
    v = dg_min( v, 1 );

    /* -- Created problems in the (count+1)th point in Carre mode --
    if (v<0 || v>1) {
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,
        GetResourceString(dlg->dg.wLaw,"errBadLaw",NULL,NULL));
      return;
    } --- */

    double t = crid_fixed.level1 + (crid_fixed.level2 - crid_fixed.level1) * v;

    int r;
    SurfaceExPtr pSX = AddSurfaceEx( crid_fixed.Area(), t, &r );
    if( pSX == NULL )
      return r;
    pSX->ChangeCreatorId( crid_fixed );
  }

  if( _pOldCrid != NULL )
    *_pOldCrid = crid_fixed;

  return 0;
}


int FluxModel::FindCarreMinMaxSurfaceLevel( int _nArea, double* _pL1, double* _pL2,
                                            int* _bOuterSurfaceNeeded ) const
{
  SENDER_NAME( "FindCarreMinMaxSurfaceLevel" );
  ElementPtr pInnermost = null, pOutermost = null;

  bool bAllFound = false;

  assert( pModel->CarreOutputMode() );

  int bosnBuf;
  if( _bOuterSurfaceNeeded == NULL )
    _bOuterSurfaceNeeded=&bosnBuf;
  *_bOuterSurfaceNeeded = 1;

  /* Make sure requirements are met */
  /* if (!a->equil->signInside) return ERR::NOCLOSEDSURFS; */

  /* Find the surface zone and the base separatrix segment */
  SurfaceZonePtr pSZ = pTopology->FindSurfaceZone( _nArea );
  assert( pSZ != NULL );
  GridPointSegPtr pGPS = pTopology->FindGridPointSeg( pSZ->GPZone(1) );
  assert( pGPS != NULL);
  GridPointSegPtr pGPS2;
  if( pSZ->GPZone(2) >= 0 ) {
    pGPS2 = pTopology->FindGridPointSeg( pSZ->GPZone(2) );
    assert( pGPS2 != NULL );
  }
  else pGPS2 = NULL;

  /* Do special processing for "between"-type zones */
  double lMin, // Nearest to separatrix
         lMax; // Farthest from separatrix
  if( pGPS2 != NULL ) {
    lMin = pGPS->Level();
    lMax = pGPS2->Level();
    pSZ->SetInnermost( NULL );
    *_bOuterSurfaceNeeded = 0;
    bAllFound = true;
  }

  /* Do special processing for "main plasma"-type zones */
  if ( !bAllFound && pSZ->HasFlag( SZF_LIMITBYSURFACE ) ) {
    lMax = lMin = pGPS->Level();
    pSZ->SetInnermost( NULL );
    FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
      if( pSX->Zone() == _nArea && fabs( pSX->Level() - lMin ) > fabs( lMax - lMin ) )
        lMax = pSX->Level();
    }
    if( lMax == lMin )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::CARRE_AREA1SURFACES );
    bAllFound = true;
  }

  if( !bAllFound ) {
    int signMinMax = pSZ->Sign();
    assert( signMinMax == 1 || signMinMax == -1 );

    /* Preset "min" and "max" levels */
    lMin = pGPS->Level();
    if( pGPS2 != NULL)
      lMax = pGPS2->Level(); /* Should never happen */
    else
      lMax = signMinMax * MAXDOUBLE;

    /* Fetch & split structure, fetch targets */
    IVarItemListPtr pStructure = pModel->Vars()->GetStructure();
    if( pStructure == null )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::NOSTRUCTURE );

    IVarItemList gSE, // Target elements
        gST, // Target chains
        gSO, // Closed chains
        gSC; // Broken chains
    pModel->Vars()->FindTargets( &gSE );
    gST = gSE; // Copy targets container to use it as in-parameter (will be overwritten)

    int r = Element::OrderStructureElems( pStructure, &gST, &gSO, &gSC, NULL, pModel->CarreOutputMode() );
    if( r != 0 )
      return pModel->SendMessage( LOG_WIP_WARN, "Element::OrderStructureElems", r );

    int restriction = pModel->SZRestriction();

    /* Since v3.0: detect zone limits as end points of targets */
    if( restriction == SZR::TPOINTS ) {
      FOREACHVARITEMCONST( VarItemListContainerPtr, pList, gSE ) {
        IVarItemList target = *pList->pList;
        ElementPtr pElem1 = null, pElem2 = null;
        if( Element::OrderTargetElems( &target, null, &pElem1, &pElem2 ) != 0 )
          continue;

        double level = 0.;
        ElementPtr pElem_selected = pElem1;
        Point pos_selected = pElem1->Node(1)->Position();
        int zone = pTopology->GetSurfaceZoneByXY( pos_selected, NULL, &r );
        if( zone != pSZ->Zone() ) {
          pElem_selected = pElem2;
          pos_selected = pElem2->Node(2)->Position();
          zone = pTopology->GetSurfaceZoneByXY( pos_selected, NULL, &r );
          if( zone != pSZ->Zone() )
            continue;
        }

        pEquil->GetLevel( pos_selected, &level, null );
        if( (level - lMax) * signMinMax < 0 ) {
          lMax = level;
          pInnermost = pElem_selected;
        }
      }
    }
    else {
      /* Since v3.0: use full target chains (old algorithm)
       * or marked target elements (new one) */
      IVarItemList& rgSR = (restriction == SZR::TARGETS) ? gSE : gST;
      /* Make sure all surfaces pass through structures containing targets */
      FOREACHVARITEMCONST( VarItemListContainerPtr, pList, rgSR ) {
        double l1 = -signMinMax * MAXDOUBLE;
        double l2 = signMinMax * MAXDOUBLE;

        /* Find extrema in this part of structure */
        int ne = 0;
        FOREACHVARITEMCONST( ElementPtr, pElem, *pList->pList ) {
          int zone = pTopology->GetSurfaceZoneByXY( pElem->Node(1)->Position(), NULL, &r );
          int zone2 = pTopology->GetSurfaceZoneByXY( pElem->Node(2)->Position(), NULL, &r );

          /* Ignore elements that do not belong to the zone in question */
          if( zone != pSZ->Zone() && zone2 != pSZ->Zone() )
            continue;
          ne++;

          /* Get min/max level values on the segment */
          double l;
          if( FindMinMaxSegment( signMinMax, pElem->Node(1)->Position(), pElem->Node(2)->Position(), &l ) == 0 )
             if( (l - l1) * signMinMax > 0 ) {
              l1 = l;
              pOutermost = pElem;
            }  /* l ">" l1 */
          if( FindMinMaxSegment( -signMinMax, pElem->Node(1)->Position(), pElem->Node(2)->Position(), &l ) == 0 )
            if( (l - l2) * signMinMax < 0)
              l2 = l;  /* l "<" l2 */
        }

        /* If >0 segments in the right zone, adjust min/max values */
        if( ne && (l1 - lMax) * signMinMax < 0 ) {
          lMax = l1;
          pInnermost = pOutermost;
        }
        if( ne && (l2 - lMin) * signMinMax > 0 )
          lMin = l2;
      }
    }

    /* Make sure they are restricted by other structures */
    IVarItemList gOther = gSC;
    gOther.insert( gOther.end(), gSO.begin(), gSO.end() );

    FOREACHVARITEMCONST( VarItemListContainerPtr, pList2, gOther ) {
      FOREACHVARITEMCONST( ElementPtr, pElem, *pList2->pList ) {

        /* Process only elements from the same zone */
        int r = 0;
        int zone = pTopology->GetSurfaceZoneByXY( pElem->Node(1)->Position(), NULL, &r );
        if( zone < 0 )
          continue;
        if( zone != _nArea )
          continue; /* Not from this zone */
        double l;
        if( FindMinMaxSegment( -signMinMax, pElem->Node(1)->Position(), pElem->Node(2)->Position(), &l)) continue;
        if( (l - lMin) * signMinMax <= 0 )
          continue; /* Skip intersecting elements */
        if( (l - lMax) * signMinMax < 0) {
          lMax = l;
          pInnermost = pElem;
        }
      }
    }

    /* Clean up */
    FreeContainerList( &gST );
    FreeContainerList( &gSC );
    FreeContainerList( &gSO );

    if( fabs( lMax ) == MAXDOUBLE )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::OUTOFEQUIL, ARGS( "?" ) );  /* $ - risky */
    assert( fabs( lMin ) != MAXDOUBLE );

    pSZ->SetInnermost( pInnermost );
  }

  if( _pL1 != NULL)
    *_pL1 = lMin;
  if( _pL2 != NULL)
    *_pL2 = lMax;

  return 0;
}


int FluxModel::FindMinMaxSegment( int _signMinMax, Point _p1, Point _p2, double* _pLevel ) const
{
  SENDER_NAME( "FindMinMaxSegment" );
  const double MIN_STEPS_IN_EQ_CELL = 20;

  int stepCount = hypot( _p1.x - _p2.x, _p1.y - _p2.y ) * MIN_STEPS_IN_EQ_CELL /
                  dg_min( pEquil->AverageCellWidth(), pEquil->AverageCellHeight() );
  stepCount = dg_max( MIN_STEPS_IN_EQ_CELL, stepCount );

  double l, lMax = -_signMinMax * MAXDOUBLE;
  Point xy, xyMax;

  for( int i = 0; i <= stepCount; i++ ) {
    xy = _p1 + (_p2 - _p1) * i/stepCount;
    if( pEquil->GetLevel( xy, &l, null ) )
      continue;
    if( (l - lMax) * _signMinMax > 0 ) {
      lMax = l;
      xyMax = xy;
    }
  }

  if( fabs( lMax ) == MAXDOUBLE )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::OUTOFEQUIL, ARGS( "?" ) );
  if( _pLevel != NULL )
    *_pLevel = lMax;

  return 0;
}


void FluxModel::ConvertOldDgSurfaces( int* _pErrorFlags )
{
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx )
    if( pSX->Zone() == SZN_XY && pSX->IsClosed() )
      break;

  int signInside = 0;
  if( pSX != NULL )
    signInside = pSX->Level() > 0 ? 1 : -1;

  IComponentList surfacesEx_tmp = surfacesEx;
  FOREACHPTRCONST( SurfaceExPtr, pSXt, surfacesEx_tmp ) {
    if( pSXt->Zone() != SZN_XY )
      continue;
    double level = pSXt->Level();

    int zone;
    if( pSXt->IsClosed() )
      zone = 1;
    else if( pSXt->Level() * signInside < 0 )
      zone = 2;
    else
      zone = 3;

    pSXt->Delete();
    if( signInside )
      AddSurfaceEx( zone, level );
    else *_pErrorFlags |= DGFE::OLD_SURFACES_LOST;
  }
}


std::string FluxModel::GetSurfacesStatsStr() const
{
  std::string str;
  char buf[2048];
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, pTopology->SurfaceZones() ) {
    ulong num = 0;
    FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
      if( pSX->CoordsOk() && pSX->Zone() == pSZ->Zone() )
        num++;
    }
    /* Add one extra cell if limited by segments from both sides */
    if( pSZ->GPZone(2) >= 0 )
      num++;
    sprintf( buf, "%d%s ", (int)num, pSZ->ShortName().c_str() );
    str += buf;
  }
  return str;
}


SurfaceExPtr FluxModel::FindSurface( int _area, double _level ) const
{
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
    if( pSX->Zone() == _area && pSX->Level() == _level )
      return pSX;
  }
  return null;
}

SurfaceExPtr FluxModel::FindSurface( const Point& _crOrigin ) const
{
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
    if( pSX->IsDrawn() && pSX->Origin() == _crOrigin )
      return pSX;
  }
  return null;
}


IComponentList FluxModel::RemoveVirtualSurfaces( SurfaceExPtr _pSX_NonVirtual )
{
  IComponentList surfaces_deleted;
  IComponentList surfaces_copy = surfacesEx;
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfaces_copy ) {
    if( pSX != _pSX_NonVirtual && pSX->Zone() == _pSX_NonVirtual->Zone() ) {
      pSX->Delete();
      surfaces_deleted.push_back( pSX );
    }
  }
  surfaces_copy.clear();
  return surfaces_deleted;
}


int FluxModel::RemoveSurfacesByCreatorId( const CreatorId& _crCrId, IComponentListPtr _vSX_removed )
{
  int i = 0;
  if( _vSX_removed != null )
    *_vSX_removed = IComponentList();
  IComponentList surfacesEx_copy = surfacesEx;
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx_copy ) {
    if( pSX->GetCreatorId().IsEqualTo( _crCrId, true ) ) {
      pSX->Delete();
      if( _vSX_removed != null )
        _vSX_removed->push_back( pSX );
      i++;
    }
  }
  return i;
}


bool FluxModel::HasVirtualSurfaces() const
{
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx )
    if( pSX->IsVirtual() )
      return true;
  return false;
}


const IComponentList& FluxModel::VirtualSurfaces() const
{
  surfacesEx_virtual.clear();
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx )
    if( pSX->IsVirtual() )
      surfacesEx_virtual.push_back( pSX );
  return surfacesEx_virtual;
}


const IComponentList& FluxModel::SurfacesEx( int _zone ) const
{
  surfacesEx_selected.clear();
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx )
    if( pSX->CoordsOk() && pSX->Zone() == _zone )
      surfacesEx_selected.push_back( pSX );
  return surfacesEx_selected;
}
