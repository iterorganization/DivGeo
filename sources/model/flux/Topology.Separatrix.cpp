#include "Topology.h"
#include "../core/Model.h"

XPointSegPtr Topology::AddXPointSeg( XPointTestPtr _pXPT, int _number, double _sp, double _ep )
{
  XPointSegPtr pXPS = new XPointSeg( pModel, _pXPT, _number, _sp, _ep );

  ActAddXPointSeg actAddXPS( pModel, pXPS );
  int result = actAddXPS.Do();

  if( result != 0 )
    return NULL;

  AddGridPointSeg( pXPS, NextGridPointSegNumber() );

  return pXPS;
}


void Topology::DeleteXPointSeg( XPointSegPtr _pXPS )
{
  IComponentList vGPS_copy = gridPointSegs;
  FOREACHPTRCONST( GridPointSegPtr, pGPS, vGPS_copy )
    if( _pXPS == pGPS->XPS() )
      pGPS->Delete();
  vGPS_copy.clear();

  ActDelXPointSeg actDelXPS( pModel, _pXPS );
  int result = actDelXPS.Do();
  assert( result == 0 );
}


int Topology::DetectXPointSegs()
{
  const int MAX_INS_N = 100;
  double pos[MAX_INS_N];

  /* Delete all existing XPointSegs */
  IComponentList vXPS_copy = xPointSegs;
  FOREACHPTRCONST( XPointSegPtr, pXPS, vXPS_copy )
    pXPS->Delete();
  vXPS_copy.clear();

  /* Add xpoint segments and delete duplicates */
  FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests ) {
    for( int i = 0; i < 4; i++ )
      AddXPointSeg( pXPT, i, -1, -1 );
  }

  DeleteDuplicateXPointSegs();

  /* Cut xpoint segments using gradients */
  IComponentList xpss = xPointSegs; /* Do not interfere with added segs */

  FOREACHPTRCONST( XPointSegPtr, pXPS1, xpss ) {
    int n = 0;
    if( pXPS1->Line() == NULL )
      continue;

    FOREACHPTRCONST( XPointTestPtr, pXPT1, xPointTests ) {
      bool bigBreak = false;
      for( int i = 0; i < 4; i++ ) {
        if( PolyLinesIntersect( *pXPS1->Line(), *pXPT1->Gradient(i), &pos[n], NULL ) )
          continue;
        if( ++n >= MAX_INS_N ) {
          bigBreak = true; /* should never happen */
          break;
        }
      }
      if( bigBreak )
        break;
    }

    if( n == 0 )
      continue;

    /* Sort intersection points */
    for( int i = 0; i < n; i++ )
      for( int j = i + 1; j < n; j++ )
        if( pos[j] < pos[i] )
          dg_swap( pos[j], pos[i] );

    /* Add segments between intersections */
    if( pos[0] > 0 )
      AddXPointSeg( pXPS1->XPT(), pXPS1->Number(), -1, pos[0] );

    for( int i = 0; i < n - 1; i++ )
      if( pos[i+1] > pos[i] )
        AddXPointSeg( pXPS1->XPT(), pXPS1->Number(), pos[i], pos[i+1] );

    if( pos[n-1] < pXPS1->LineLength() )
      AddXPointSeg( pXPS1->XPT(), pXPS1->Number(), pos[n-1], -1 );

    /* Delete the original segment */
    //pXPS1->Delete();
    vXPS_copy.push_back( pXPS1 );
  }

  FOREACHPTRCONST( XPointSegPtr, pXPS2, vXPS_copy )
    pXPS2->Delete();

  return 0;
}


void Topology::DeleteDuplicateXPointSegs()
{
  int k = 0;

  IComponentList vXPS_del;
  FOREACHPTRCONST( XPointSegPtr, pXPS1, xPointSegs ) {
    if( pXPS1->Line() == null )
      continue;

    FOREACHPTRCONST( XPointSegPtr, pXPS2, xPointSegs ) {
      if( pXPS2 == pXPS1 || Contains( vXPS_del, (IComponentPtr)pXPS2 ) )
        break;
      if( pXPS2->Line() == null )
        continue;
      k++;
      if (!CompPolyLinesEx( *pXPS1->Line(), *pXPS2->Line(), pEquil->AverageCellWidth() * DEL_DUP_MAX_ERROR ) ) {
        /*puts("Deleted!"); */
        //pXPS2->Delete();
        vXPS_del.push_back( pXPS2 );
        /* break; */
      }
    }
  }

  FOREACHPTRCONST( XPointSegPtr, pXPS, vXPS_del )
    pXPS->Delete();
  /* printf("Comparisions:%d\n",k); */
}


void Topology::CalcSeparatrixLine( XPointTestPtr _pXPT, PointArray* _pLine, int _idx ) const
{
  // in: XPointTestPtr pXPT
  // out: PointArray* pLine

  NPoint c1 = _pXPT->EndPos(1);
  NPoint c2 = _pXPT->EndPos(2);

  /* Make sure that end cells are not out of equilibrium */
  assert( c1 > 0 );
  assert( c2 < pEquil->Size() - 1 );
  assert( c1 < c2 );

  double xptLevel = _pXPT->Level();

  /* Find a cell that contains the proper "surface" */
  NPoint cell;
  if( FindSurfaceCell( c1, c2, xptLevel, _idx, cell ) < 0 ) {
    _pLine->clear();
    return;
  }

  /* Unknown code
  AddChord(w->app,eq->x[x],eq->y[y],eq->x[x+1],eq->y[y+1]);
  AddChord(w->app,eq->x[x+1],eq->y[y],eq->x[x],eq->y[y+1]);
  */

  /* Create a spline with CalcSurfaceLine() method and and additional points */
  bool isClosed;
  if( CreateSurfaceLine( cell, xptLevel, _pLine, isClosed ) < 0 )
    return;

  /* Detect the direction of the line and cut it if needed */

  /* Unknown code
  xy=GroupAt(g,0);
  if(xy!=NULL)
    AddViewLabel(w,xy->x,xy->y,"00",0);
  return g;
  */

  PointIter itXY;
  if( isClosed ) {
    Point xy0 = _pLine->front();
    Point xy1 = xy0;
    itXY = _pLine->begin();
    while( true ) {
      ++itXY;
      /* Unknown code
      AddSource(w->app,(EqX(eq,x)+EqX(eq,x+1))/2,(EqY(eq,y)+EqY(eq,y+1))/2);
      */
      if( pEquil->In( xy1, cell, cell+1 ) &&    // first point in cell
          !pEquil->In( *itXY, cell, cell+1 ) && // second point out of cell
          pEquil->In( *itXY, c1, c2 ) ) {       // but in limits

        std::reverse( _pLine->begin(), _pLine->end() ); /* Cell -> XPointRect - reverse */
        for( itXY = _pLine->begin(); *itXY != xy1; ++itXY ); /* Find it again */

        /* Unknown code
        printf("a:%d\n",GroupIndex(g,xy));
        AddSource(a,xy->x,xy->y);
        */
        break;
      }

      /* Rotate the group */
      itXY--;
      itXY = _pLine->erase( itXY );

      _pLine->push_back( xy1 );
      Point xy = *itXY;

      /* And check again. MB: nothing must be changed */
      if( pEquil->In( xy,cell, cell+1 ) &&    // first point in cell
          !pEquil->In( xy1, cell, cell+1 ) && // end point out of cell
          pEquil->In( xy1, c1, c2 ) ) {       // but in limits

        /* Unknown code
        printf("b:%d\n",GroupIndex(g,xy));
        XPointRec -> Cell - found
        AddSource(a,xy->x,xy->y);
        */
        break;
      }

      xy1 = *itXY;
      if( xy1 == xy0 )
        break;
    } // end while
  } // end if( isClosed )
  else { /* Non-closed surface */
    /* Kill the part that passes through the X point rectangle and beyond */
    Point xy, xy1;
    FOREACHPAIR( xy1, xy, *_pLine ) {
      /* Trace the surface line */

      /* Unknown code
      if (!EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
          EqIn(eq,xy->x,xy->y,x,y,x+1,y+1))
      AddViewLabel(w,xy->x,xy->y,"Enter",0);

      if (EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
          !EqIn(eq,xy->x,xy->y,x,y,x+1,y+1))
      AddViewLabel(w,xy1->x,xy1->y,"Leave",0);
      */
      if( pEquil->In( xy1, cell, cell+1 ) &&
          !pEquil->In( xy, cell, cell+1 ) &&
          pEquil->In( xy, c1, c2 ) ) {
        /* Unknown code
        puts("c");
        if (xy1!=NULL)
          AddViewLabel(w,xy->x,xy->y,"Direct",0);
        */
        itXY = CURRENTITER;
        break; /* Cell -> XPointRect - found */
      }

      if( pEquil->In( xy, cell, cell+1 ) &&
          !pEquil->In( xy1, cell, cell+1 ) &&
          pEquil->In( xy1, c1, c2 ) ) {
        /* Unknown code
        if (xy!=NULL)
          AddViewLabel(w,xy1->x,xy1->y,"Reverse",0);
        */
        std::reverse( _pLine->begin(), _pLine->end() ); /* XPointRec -> Cell - reverse */

        for( CURRENTITER = _pLine->begin(); *CURRENTITER != xy1; ++CURRENTITER ); /* Find it again */
        /*puts("d");*/
        itXY = CURRENTITER;
        break; /* found */
      }
    }
    /* if (xy==NULL) AddViewLabel(w,xy1->x,xy1->y,"NotFound",0); */

    _pLine->erase( itXY, _pLine->end() );
    std::reverse( _pLine->begin(), _pLine->end() );
  }

  /* Unknown code
  xy=GroupAt(g,0);
  if (xy!=NULL)
    AddViewLabel(w,xy->x,xy->y,"0",0);
  xy=GroupAt(g,1);
  if (xy!=NULL)
    AddViewLabel(w,xy->x,xy->y,"1",0);

  // Obsolete
  printf("GC=%d\n",GroupCount(g));   -- XPointTest -- obsolete --
  xy=GroupAt(g,1);
  if (EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2))
    puts("!"),RevertGroup(g);      -- XPointTest -- obsolete --
  */

  /* Make sure we are not immediately entering the XptRect again */

  Point xy = _pLine->at( 1 );
  if( pEquil->In( xy, c1, c2 ) ) {
    /* assert(0); */
    /* Unknown code
    AddSource(a,xy->x,xy->y);
    xy=GroupAt(g,0);
    AddSource(a,xy->x,xy->y);
    xy=GroupAt(g,2);
    AddSource(a,xy->x,xy->y);
    */
    //delete pLine;
    //pLine = null;
    _pLine->clear();
    /*puts("3");*/ /* XPointTest */
    return;
  }

  /* Cut the line at the 1st x-point met */
  IComponentIterConst ENDITER = xPointTests.end();
  IComponentIterConst itXPT;
  XPointTestPtr pXPT_1 = null;

  itXY = _pLine->begin();
  while( itXY != _pLine->end() ) {
    bool fullbreak = false;
    FOREACH_NODECL_CONST( itXPT, xPointTests ) {
      pXPT_1 = dgtype_cast< XPointTestPtr >( *itXPT );
      NPoint c1 = pXPT_1->EndPos(1);
      NPoint c2 = pXPT_1->EndPos(2);
      if( pEquil->InS( *itXY, c1, c2 ) ) {
        fullbreak = true;
        break;
      }
    }
    if( fullbreak )
      break;
    ++itXY;
  }

  /* if (xy!=NULL) printf("GI=%d\n",GroupIndex(g,xy)); */

  _pLine->erase( itXY, _pLine->end() );

  /* Add X point center */

  if( itXPT != xPointTests.end() )
    _pLine->push_back( pXPT_1->Center() ); /* Closed separatrix */

  std::reverse( _pLine->begin(), _pLine->end() );
  _pLine->push_back( _pXPT->Center() );
  std::reverse( _pLine->begin(), _pLine->end() );

  /* Kill duplicate points - bugfix for closed surfaces */
  Point xy1;
  FOREACHPAIR( xy1, xy, *_pLine ) {
    if( xy1 == xy ) {
      /* AddSource(a,xy->x,xy->y); */
      CURRENTITER--;
      _pLine->erase( CURRENTITER + 1 );
      xy = xy1;
    }
  }

  /* Make sure loops are in CCW order */
  if( itXPT != xPointTests.end() && pXPT_1 == _pXPT ) {
    Point xy = _pLine->at( 1 );
    Point xy1 = _pLine->at( _pLine->size() - 2 );
    Point center = _pXPT->Center();
    if( (xy.x - center.x)*(xy1.y - center.y) - (xy1.x - center.x)*(xy.y - center.y) < 0 )
      std::reverse( _pLine->begin(), _pLine->end() );
  }

  return;
}


int Topology::FindSurfaceCell( const NPoint& _crC1, const NPoint& _crC2,
                               double _level, int _idx, NPoint& _rCell ) const
{
  int n = 0;
  NPoint c = _crC1; // Current cell
  NPoint c0;     // Previous cell

  while( true ) {
    c0 = c;

    /* Go cell by cell ccw around rectangle
     * given by c1 & c2 cells in corners. Start in c1 */
         if( c.y == _crC1.y && c.x != _crC2.x ) c.x++; // Right
    else if( c.x == _crC2.x && c.y != _crC2.y ) c.y++; // Up to c2
    else if( c.y == _crC2.y && c.x != _crC1.x ) c.x--; // Left
    else if( c.x == _crC1.x && c.y != _crC1.y ) c.y--; // Down to c1
    else
      assert( 0 );

    /* DG 2.1 algorithm */
    /*     if( c.y == c1.y )  c.x == c2.x ? c.y++ : c.x++;
    else if( c.x == c2.x )  c.y == c2.y ? c.x-- : c.y++;
    else if( c.y == c2.y )  c.x == c1.x ? c.y-- : c.x--;
    else if( c.x == c1.x )  c.y == c1.y ? c.x++ : c.y--;
    else
      assert( 0 );*/

    /* Check if xpt level is between previous and current positions */
    double cellLevel = pEquil->CorrCell( c, _level );
    double cellLevel_0 = pEquil->CorrCell( c0, _level );
    if( dg_inrange_s( _level, cellLevel_0, cellLevel ) )
      if( n++ == _idx )
        break;

    /* Unknown code
    CalcSurfData(eq,x,y,xpt->level,&sc);
      if (sc.n>1) if (n++==idx) break;

      if (y==xpt->cy1-1) x==xpt->cx2-1 ? x++,y++ : x++; else
      if (x==xpt->cx2)   y==xpt->cy2-1 ? x--,y++ : y++; else
      if (y==xpt->cy2)   x==xpt->cx1   ? x--,y-- : x--; else
      if (x==xpt->cx1-1) y==xpt->cy1   ? x++,y-- : y--; else
      assert(0);
    */

    if( c.x == _crC1.x && c.y == _crC1.y ) {
      /* puts("1"); */
      return -1; // Proper cell not found
    }
  }

  /* Make current cell less than previous one */
  if( c.x > c0.x ) dg_swap( c.x, c0.x );
  if( c.y > c0.y ) dg_swap( c.y, c0.y );

  /* If previous and current cells are one the same line with c1
   * than shift current one down or left */
  if( c.x == c0.x ) {
    if( c.x == _crC1.x )
      c.x--;
  }
  else if( c.y == c0.y ) {
    if( c.y == _crC1.y )
      c.y--;
  }
  else
    assert( 0 );

  _rCell = c;
  return 0;
}


int Topology::CreateSurfaceLine( const NPoint& _crCell, double _level,
                                 PointArray* _pLine, bool& _rIsClosed ) const
{
  /* Calculate a line */
  int i = pEquil->CalcSurfaceLine( _crCell, _level, _pLine, pEquil->Size() );
  _rIsClosed = (i == 1);
  if( i < 0 ) {
    /*puts("F");*/
    _pLine->clear();
    return -1;
  } /* XPointTest */

  if( _pLine->size() < 3 ) {
    _pLine->clear();
    /*puts("2");*/
    return -2;
  } /* XPointTest$*/

  /* Order the line */
  Point xy = _pLine->front(); // at [0]
  Point xy1 = _pLine->back();

  if( xy.x < xy1.x || ( xy.x == xy1.x && xy.y < xy1.y ) )
    std::reverse( _pLine->begin(), _pLine->end() );

  /* Create a double number of points */
  PointArray* pLine_t = new PointArray();

  FOREACHPAIRCONST( xy1, xy, *_pLine ) {
    pLine_t->push_back( xy1 );
    pLine_t->push_back( (xy1 + xy) / 2. );
  }

  /* And additional points to the line ends */
  xy1 = _pLine->back();
  pLine_t->push_back( xy1 );

  if( _rIsClosed ) {
    xy = _pLine->front();
    pLine_t->push_back( (xy1 + xy) / 2. );
  }

  //delete pLine;
  *_pLine = *pLine_t;

  return 0;
}


int Topology::CalcXPointSegNumber( XPointSegPtr _pXPS, XPointTestPtr _pXPT ) const
{
  int nTotal = 0;
  int nBefore = 0;
  FOREACHPTRCONST( XPointSegPtr, pXPS1, xPointSegs ) {
    if( pXPS1->XPT() == _pXPS->XPT() && pXPS1->Number() == _pXPS->Number() ) {
      nTotal++;
      if( pXPS1->StartPos() < _pXPS->StartPos() )
        nBefore++;
    }
  }

  /* Invert the number if the opposite xpoint */
  if( _pXPT != _pXPS->XPT() )
    nBefore = nTotal - nBefore - 1;
  return nBefore;
}



bool Topology::HasDuplicatedSegments( int _zone, GridPointSegPtr _pGPS ) const
{
  int count = 0;
  FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs ) {
    if( pGPS->Zone() == _zone && _pGPS != pGPS )
      count++;
  }
  return count > 1;
}
