#include "Topology.h"
#include "../core/Model.h"

#include <iostream>

Topology::Topology( ModelPtr _pModel ):
  IComponent( OT::TOPOLOGY, _pModel ),
  pEquil( null )
{
  DG_HIERARCHY_INIT;
}

Topology::~Topology()
{
  ReleaseObject( &pEquil );
  Release();
}

void Topology::SetEquil( EquilPtr _pEquil )
{ 
  ReleaseObject( &pEquil );
  pEquil = DG_SHARE( _pEquil );

  if( pEquil != null && !xPointSegs.empty() ) {
    FOREACHPTRCONST( XPointSegPtr, pXPS, xPointSegs )
      pXPS->SetEquilParams( (pEquil->PointFirst().y - pEquil->PointLast().y),
                            (pEquil->AverageCellWidth() + pEquil->AverageCellHeight()) / 2. );
  }
}

void Topology::Delete()
{
  ActDelTopology( pModel, this, DO_AT_ONCE );
}

IComponentPtr Topology::GetLockingObject()
{
  if( !xPointTests.empty() ) {
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( xPointTests.front() );
    return pXPT->IsLocked() ? pXPT->GetLockingObject() : pXPT;
  }

  if( !xPointSegs.empty() ) {
    IComponentPtr pXPS = xPointSegs.front();
    return pXPS->IsLocked() ? throw "XPointSeg can not be locked" : pXPS;
  }
  return null;
}


void Topology::InvalidateCache()
{
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones ) {
    if( pSZ->HasBounds() )
      pSZ->FreeBounds();
  }
}


void Topology::RecalcCache( int* _pErr )
{
  SENDER_NAME( "RecalcTopologyCache" );
  int errBuf;
  if( _pErr == null )
    _pErr = &errBuf;
  *_pErr = 0;

  /* Free all existing bounds */
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones )
    pSZ->FreeBounds();

  /**** Generate all separatrix lines, store in "lines" */
  std::vector< PointArray > lines;

  int n = 0;
  FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests ) {
    for( int i = 0; i < 4; i++ ) {

      XPointSeg xps( pModel, pXPT, i, -1, -1 );
      int result = xps.CalcLine();
      if( result != 0 )
        continue;

      lines.push_back( *xps.Line() );
      n++;
    }
  }

  /* Detect and sort all equilibrium edge intersections */
  std::vector< EquilBorderSort > borders( 2 * n + 1 );
  n = 0;

  std::vector< PointArray > lines_reversed;
  FOREACH_CONST( std::vector< PointArray >::const_iterator, itLine, lines ) {
    /* Is the "other" end of the line at the equilibrium edge? */
    const PointArray& line = *itLine;
    assert( !line.empty() );
    Point xy = line.back();

    borders[n].b = pEquil->XYToBorder( xy );

    /* No - if it is not a loop, skip it */
    if( borders[n].b == MAXDOUBLE ) {
      Point xyt = line[0];
      if( xyt != xy )
        continue;
    }
    /* Yes - add to the intersection array */
    else {
      borders[n].p = xy;
      n++;
    }

    /* Add the reversed line to the lines array */
    PointArray line1( line.size() ); // CreateStack()
    std::reverse_copy( line.begin(), line.end(), line1.begin() );

    lines_reversed.push_back( line1 );
  }
  lines.insert( lines.end(), lines_reversed.begin(), lines_reversed.end() );
  lines_reversed.clear();

  /* Sort eq.edge intersections in the CW order */
  std::vector< EquilBorderSort >::iterator border_it = borders.begin() + n;
  std::sort( borders.begin(), border_it, EquilBorderCompare );

  /* Connect equilibrium edge intersections with addl lines */
  Point p1 = pEquil->PointFirst();
  Point p2 = pEquil->PointLast();

  if( p1.x > p2.x ) { double t = p1.x; p1.x = p2.x; p2.x = t; }
  if( p1.y > p2.y ) { double t = p1.y; p1.y = p2.y; p2.y = t; }

  for( int i = 0; i < n; i++ ) {
    int j1 = (i + 1) % n;
    PointArray line; /* For the logical CCW line: Y1->X2->Y2->X1 */
    Point t = borders[i].p;
    line.push_back( t );

    do {
      if( t.y == p1.y && t.x < p2.x )
        t.x = (t.y == borders[j1].p.y && t.x < borders[j1].p.x) ? borders[j1].p.x : p2.x;
      else if ( t.x == p2.x && t.y < p2.y )
        t.y = (t.x == borders[j1].p.x && t.y < borders[j1].p.y) ? borders[j1].p.y : p2.y;
      else if ( t.y == p2.y && t.x > p1.x )
        t.x = (t.y == borders[j1].p.y && t.x > borders[j1].p.x) ? borders[j1].p.x : p1.x;
      else if ( t.x == p1.x && t.y > p1.y )
        t.y = (t.x == borders[j1].p.x && t.y > borders[j1].p.y) ? borders[j1].p.y : p1.y;

      line.push_back( t );
    } while( t != borders[j1].p );

    /* Create a reverse line in CW order */
    PointArray line1( line.size() ); // CreateStack()
    std::reverse_copy( line.begin(), line.end(), line1.begin() );

    lines.push_back( line );
    lines.push_back( line1 );
  }

  borders.clear();


  /**** Now, generate bounds for each SurfaceZone */

  FOREACHPTRCONST( SurfaceZonePtr, pSZ1, surfaceZones ) {
    GridPointSegPtr pGpsStart= FindGridPointSeg( pSZ1->GPZone(1) );
    if( pGpsStart == NULL ) {
      *_pErr = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADGPZONENUMBER,
                                   ARG( pSZ1->Zone() )_ARG( pSZ1->GPZone(1) ) );
      continue;
    }

    /* Check the level to the "left" side */
    unsigned size = pGpsStart->LineSize();
    if( size < 3 ) {
      *_pErr = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSURFACEZONE1 );
      continue;
    } /* $ - change error msg */

    unsigned halfSize = size / 2;
    unsigned halfSize1 = halfSize + 1;
    if( halfSize1 < size - 1 )
      halfSize1++;

    Point xy = pGpsStart->GetPoint( halfSize );

    Point grad;
    double level;
    if( pEquil->GetLevel( xy, &level, &grad ) != 0 ) {
      *_pErr = pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSURFACEZONE1 );  /* $ - change error msg */
      continue;
    }

    /* Determine the initial direction */
    Point xy1 = pGpsStart->GetPoint( halfSize1 );
    double sina = pSZ1->Sign() * (grad.y * (xy1.x - xy.x) - grad.x * (xy1.y - xy.y));
    bool bReverse = (sina < 0);

    /* Generate the first line */
    XPointSeg xps( pModel, pGpsStart->XPS()->XPT(), pGpsStart->XPS()->Number(), -1., -1. );
    int result = xps.CalcLine();

    if( result != 0 ) {
      /*  puts("Unable to recreate xps"); */
      continue;
    }

    PointArray line = *xps.Line();
    if( bReverse )
      std::reverse( line.begin(), line.end() );

    /* Store the "first" point coords - we will return here */
    Point Start = line[0];
    Point Start2 = line[1];

    /* Start walking from the original line */
    const PointArray* pLine = &line;
    PointArray lresult;

    /* To prevent dead loops, limit the number of steps */
    for( unsigned i = 0; i < lines.size(); i++ ) {

      /* Follow the line, add points except the last one to the result */
      for( PointIterConst itP = pLine->begin() + 1, itPEnd = pLine->end();
           itP != itPEnd; ++itP )
        lresult.push_back( *(itP - 1) );

      /* Figure out where to go next */

      /* Detect the "leftmost" outgoung segment from this point */
      Point t = pLine->back();
      Point d = *(pLine->end() - 2) - t;

      const PointArray* pLineHit = null;
      double angleHit = MAXDOUBLE;
      FOREACH_CONST( std::vector< PointArray >::const_iterator, itLine, lines ) {
        pLine = &(*itLine);
        Point xy = pLine->front();
        if( xy != t  )
          continue;
        Point xy1 = *(pLine->begin() + 1);
        double angle = AngleBetween( xy1 - xy, d );
        if( angle < angleHit && angle > ANGLE_0_THRESHOLD ) {
          pLineHit = pLine;
          angleHit = angle;
        }
      }

      /* Select the next line to follow */
      if( pLineHit == NULL )
        break;
      pLine = pLineHit;

      /* Back at the beginning? */
      Point xy = pLine->front();
      Point xy1 = *(pLine->begin() + 1);
      /*src=AddSource(a,xy->x,xy->y);*/
      if( xy == Start && xy1 == Start2 ) {
        /*MarkObject(a,src,1);*/
        break;
      }
      continue;
    }

    pSZ1->SetBounds( lresult );
    line.clear();
  }

  /* Free the "lines" group */
  // self destructed

  return;
}
