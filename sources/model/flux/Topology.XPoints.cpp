#include "Topology.h"
#include "../core/Model.h"

XPointTestPtr Topology::AddXPointTest( const NPoint& _crC1, const NPoint& _crC2, double _level, int _id )
{
  XPointTestPtr pXPT_tmp = new XPointTest( pModel, _crC1, _crC2, _level, _id );

  if( CheckXPointRect( pXPT_tmp ) ) {
    delete pXPT_tmp;
    return NULL;
  }

  if( FindXPointCenter( pXPT_tmp ) ) {
    delete pXPT_tmp;
    return NULL;
  }

  ActAddXPointTest actAddXPT( pModel, pXPT_tmp );
  int result = actAddXPT.Do();

  assert( result == 0 );

  return pXPT_tmp;
}


int Topology::FindXPointCenter( XPointTestPtr _pXPT ) const
{
  Point s[4];

  NPoint c1 = _pXPT->EndPos(1);
  NPoint c2 = _pXPT->EndPos(2);
  NPoint p = _pXPT->EndPos(1);
  double plvl = _pXPT->Level();

  int n = 0;
  while( n < 4 ) {
    double lvl = pEquil->CorrCell( p, plvl );
    NPoint op = p;

    if( p.y == c1.y ) p.x == c2.x ? p.y++ : p.x++; else
    if( p.x == c2.x ) p.y == c2.y ? p.x-- : p.y++; else
    if( p.y == c2.y ) p.x == c1.x ? p.y-- : p.x--; else
    if( p.x == c1.x ) p.y == c1.y ? p.x++ : p.y--; else
    assert( 0 );

    double lvl1 = pEquil->CorrCell( p, plvl );

    if( (lvl - plvl)*(lvl1 - plvl) < 0 ) {
      s[n] = pEquil->CenterOffset( op, p, (plvl - lvl) / (lvl1 - lvl) );
      /* AddSource(a,xs[n],ys[n]); */
      n++;
    }

    if( p == c1 && n == 0)
      break;
  }

  if( n < 4 )
    return -1;

  double r;
  if( VIntersect( s[0], s[2], s[1], s[3], &r, NULL ) )
    return -1;

  _pXPT->SetCenter( Point( s[0].x + (s[2].x - s[0].x) * r, s[0].y + (s[2].y - s[0].y) * r ) );

  /* AddSource(a,xpt->centerX,xpt->centerY); */

  return 0;
}


XPointTestPtr Topology::FindXPointTest( int _id ) const
{
  if( !xPointTests.empty() ) {
    FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests )
      if( pXPT->ID() == _id )
        return pXPT;
  }

  return null;
}


int Topology::CheckXPointRect( XPointTestPtr _pXPT ) const
{
  XPointMinMax mm[6];

  NPoint c1 = _pXPT->EndPos(1);
  NPoint c2 = _pXPT->EndPos(2);

  assert( c1 < c2 );
  assert( c1 >= 0 );
  assert( c2 < pEquil->Size() );

  /* Detect extrema */
  NPoint p = c1;
  int d = 1;
  int n = 0;

  while( n < 6 ) {
    double lvl = pEquil->Cell( p );
    NPoint op = p;

    if( p.y == c1.y ) p.x == c2.x ? p.y++ : p.x++; else
    if( p.x == c2.x ) p.y == c2.y ? p.x-- : p.y++; else
    if( p.y == c2.y ) p.x == c1.x ? p.y-- : p.x--; else
    if( p.x == c1.x ) p.y == c1.y ? p.x++ : p.y--; else
    assert( 0 );

    double lvl1 = pEquil->Cell( p );

    if( d == 1 && lvl1 < lvl ) {
      mm[n] = XPointMinMax( op, d, lvl );
      n++;
      d = -1;
    }
    else if(d == -1 && lvl1 > lvl ) {
      mm[n] = XPointMinMax( op, d, lvl );
      n++;
      d = 1;
    }

    if( p == c1 && n == 0 ) break;
  }

  /* No extrema - error */
  if( n == 0 )
    return -1;

  assert( n == 6 );

  /* Check for exactly 4 extrema */
  for( int i = 2; i < n - 1; i++ )
    if( mm[i].p == mm[1].p )
      return -1; /* Too few */

  if( mm[n - 1].p != mm[1].p )
    return -1; /* Too many */

  /* Make sure a minimum is 1st */
  if( mm[1].t != -1) for( int i = 1; i < 4; i++ ) {
    mm[0] = mm[i];
    mm[i] = mm[i+1];
    mm[i+1] = mm[0];
  }

  /* Shift values to the left */
  for( int i = 0; i < 4; i++ )
    mm[i] = mm[i+1];
  n = 4;

  /* Check for each minimum < each maximum */
  if( mm[0].lvl >= mm[1].lvl ||
      mm[0].lvl >= mm[3].lvl )
    return -1;
  if( mm[2].lvl >= mm[1].lvl ||
      mm[2].lvl >= mm[3].lvl )
    return -1;

  /* Check for an 'X'-intersection */
  for( int i = 0; i < 4; i++ ) {
    if( CheckXPointLevels( c1, c2, mm[i].p, mm[i].t ) != 0 )
      return -1;
  }

  _pXPT->SetMinMax( mm );

  /*
  for (i=0;i<4;i++) {
    MarkObject(a,AddSource(a,eq->x[p[i].x],eq->y[p[i].y]),i&1);
  }
  */

  return 0;
}


int Topology::CheckXPointLevels( const NPoint& _crC1, const NPoint& _crC2,
                                 const NPoint& _crP0, int _bMinMax ) const
{
  if( ( _crP0.x == _crC1.x || _crP0.x == _crC2.x) &&
      ( _crP0.y == _crC1.y || _crP0.y == _crC2.y) ) {
    /* AddSource(a,eq->x[x0],eq->y[y0]); */
    return 0;
  }

  NPoint p = _crP0;

  int d;
  if( p.x == _crC1.x ) { d = 1; p.x++; } else
  if( p.x == _crC2.x ) { d = 3; p.x--; } else
  if( p.y == _crC1.y ) { d = 2; p.y++; } else
  if( p.y == _crC2.y ) { d = 0; p.y--; } else
  assert(0) ;

  double expr = pEquil->Cell( _crP0 ) - pEquil->Cell( p );
  if( expr * _bMinMax > 0)
    return 0;

  NPoint np;
  do {
    for( int nd = d - 1; nd <= d + 2; nd++ ) {
      np = p;
      switch( nd & 3 ) {
        case 0: np.y = p.y - 1; break;
        case 1: np.x = p.x + 1; break;
        case 2: np.y = p.y + 1; break;
        case 3: np.x = p.x - 1; break;
        default: assert( 0 );
      }

      expr = pEquil->Cell( _crP0 ) - pEquil->Cell( np );
      if( expr * _bMinMax <= 0 ) {
        d = nd;
        break;
      }
    }

    p = np;
    /*printf("%d %d    ",x,y);*/
  } while( np.x != _crC1.x && np.x != _crC2.x && np.y != _crC1.y && np.y != _crC2.y );

  /* if (nx==x0 && ny==y0) puts("Yau!"),AddSource(a,eq->x[x0],eq->y[y0]);*/
  /* else puts("AAA"); */
  return np != _crP0;
}


int Topology::DetectXPoints()
{
  /* Delete all pre-existing xpoints */
  IComponentList vXPT_copy = xPointTests;
  FOREACHPTRCONST( XPointTestPtr, pXPT, vXPT_copy )
    pXPT->Delete();
  vXPT_copy.clear();

  /* Find all xpoints, calculate levels and add to model */
  XPointTestList xpts = FindXPointRects();

  int i = 0;
  FOREACH_CONST( XPointTestIterConst, itXPT, xpts ) {
    XPointTestPtr pXPT_tmp = *itXPT;
    pXPT_tmp->CalcLevel();
    AddXPointTest( pXPT_tmp->EndPos(1), pXPT_tmp->EndPos(2), pXPT_tmp->Level(), i++ );
    delete pXPT_tmp;
  }

  /* Add X-Point segments */
  return DetectXPointSegs();
}


XPointTestList Topology::FindXPointRects() const
{
  XPointTestList xpts;

  NPoint size = pEquil->Size();

  for( int s = 1; s < 6; s++ ) {
    for( int i = 1; i < size.x - s - 1; i++ ) {
      for( int j = 1; j < size.y - s - 1; j++ ) {
        XPointTestPtr pXPT_tmp = new XPointTest( pModel, NPoint( i, j ), NPoint( i+s, j+s ), 0, 0 );

        FOREACH_CONST( XPointTestIterConst, itXPT, xpts ) {
          XPointTestPtr pXPT = *itXPT;
          if( pXPT->EndPos(2) >= pXPT_tmp->EndPos(1) &&
              pXPT->EndPos(1) <= pXPT_tmp->EndPos(2) ) {
            delete pXPT_tmp;
            goto skip; // Good GOTO: Continue j-loop
          }
        }

        if( CheckXPointRect( pXPT_tmp ) != 0 ) {
          delete pXPT_tmp;
          continue;
        }

        xpts.push_back( pXPT_tmp );
        skip:;
      }
    }
  }

  return xpts;
}


void Topology::CalcXPointGradients( XPointTestPtr _pXPT ) const
{
  for( int i = 0; i < 4; i++ )
    _pXPT->SetGradient( i, pEquil->CalcGradientLine( _pXPT->MinMax(i), 10, i % 2 * 2 - 1 ) );
}

