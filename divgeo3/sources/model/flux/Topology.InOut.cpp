#include "Topology.h"
#include "../core/Model.h"

int Topology::ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode )
{
  char sLine[ DG_LINE_LEN ];
  char sToken[ 32 ];
  Point pos;
  uint count;
  int id;
  double level;
  char sFlags[ DG_STR_LEN ], sName[ DG_STR_LEN ], sDescr[ DG_STR_LEN ];
  ulong flags;

  int errorFlags = 0;

  switch( _token ) {
  case TKN::EquilHints114:
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT""SCANFLT""SCANFLT,
                sToken, &ehint1.x, &ehint1.y, &ehint2.x, &ehint2.y ) == 5 ) {
      /* Ignore */
    }
    else
      return DGFE::SYNTAX;
    break;
  case TKN::XPointsEx114:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      if( _mode == 2 ) /* Import mode with X-points detection */
        DetectXPoints();

      Point P1 = pEquil != null ? pEquil->PointFirst() : 0.;
      Point P2 = pEquil != null ? pEquil->PointLast() : 0;

      IComponentList xpoints;
      NPoint eqid1, eqid2;
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;

        switch( _mode ) {
        case 0: /* Project mode */
          if( sscanf( sLine, "%d%d%d%d%d"SCANFLT,
                      &eqid1.x, &eqid1.y, &eqid2.x, &eqid2.y, &id, &level ) == 6 ) {
            if( pEquil == null ||
                AddXPointTest( eqid1, eqid2, level, id ) == null )
              errorFlags |= DGFE::NEQUIL;
          }
          else
            errorFlags |= DGFE::SYNTAX;
          break;

        case 1: /* Import mode */
        case 2:
          if( sscanf( sLine, "%d%d%d%d%d"SCANFLT""SCANFLT""SCANFLT,
                      &eqid1.x, &eqid1.y, &eqid2.x, &eqid2.y, &id, &level, &pos.x, &pos.y ) == 8 ) {
            /* Scale topology's XPT coordinates to current equil coordinates */
            pos = P1 + (pos - ehint1) / (ehint2 - ehint1) * (P2 - P1);
            /* Find nearest real XPT */
            double dMin = MAXDOUBLE;
            XPointTestPtr pXPT_min = null; //TODO: separate
            FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests ) {
              Point h = pXPT->Center() - pos;
              double d = hypot( h.x, h.y );
              if( d < dMin ) {
                dMin = d;
                pXPT_min = pXPT;
              }
            }
            /* And change XPT's id */
            if( pXPT_min != null ) {
              ActChangeValue( pModel, pXPT_min, pXPT_min, XPointTest::p_id, id, REDRAW, DO_AT_ONCE );
              xpoints.push_back( pXPT_min );
            }
          }
        } /* End mod difference */
      }

      if( _mode != 0 ) { /* Import mode */
        /* Remove all real XPTs not defined in topology */
        IComponentList vXPT_copy = xPointTests;
        FOREACH_CONST( IComponentIter, itXPT, vXPT_copy ) {
          if( !Contains( xpoints, *itXPT ) )
            (*itXPT)->Delete();
        }
        vXPT_copy.clear();
        xpoints.clear();
      }
    }
    else
      return DGFE::SYNTAX;
    break;
  case TKN::GridPointSegs114:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      if( _mode != 0 )
        DetectXPointSegs();

      IComponentList segments;

      int idXPT, idXPS;
      double startPos, endPos, gps_angle, length;
      int zone, dir, dir_target, tmp;

      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        int r = sscanf( sLine, "%d%d"SCANFLT""SCANFLT"%d%d%d%d%d"SCANFLT""SCANFLT"%s",
                    &idXPT, &idXPS, &startPos, &endPos, &zone, &dir,
                    &dir_target, &id, &tmp, &gps_angle, &length, sFlags );

        if( r < 12 )
          strcpy( sFlags, "" );

        if( r >= 7 )
        {
          fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
          fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
          if( pEquil == null )
            break;
          XPointTestPtr pXPT = FindXPointTest( idXPT );
          XPointSegPtr pXPS = null;
          if( pXPT == NULL )
            errorFlags |= DGFE::NEQUIL;
          else {
            switch( _mode ) {
            case 0: /* Project mode */
              pXPS = AddXPointSeg( pXPT, idXPS, startPos, endPos );
              break;
            case 1: /* Import mode */
            case 2: {
              if( r < 11 ) {
                errorFlags |= DGFE::NEQUIL;
                break;
              }
              if( dir <= 0 )
                dir = 1.; /* Bugfix for DG files made with beta */

              /* Find segment by angle near its XPT */
              double dMin = MAXDOUBLE;
              const IComponentList& crXptSegments = pXPT->Segments();
              FOREACHPTRCONST( XPointSegPtr, pXPS_selected, crXptSegments ) {
                double angle = pXPS_selected->CalcAngle( pXPT );
                double d = fabs( angle - gps_angle );
                if( d > 180 )
                  d = fabs( d - 360 ); /* Correct for "-179 vs +179" */
                if( d < dMin ) {
                  dMin = d;
                  pXPS = pXPS_selected;
                }
              }

              if( pXPS != null ) {
                XPointTestPtr pXPT_min = pXPS->XPT();
                int number_min = pXPS->Number();

                dMin = MAXDOUBLE;

                FOREACHPTRCONST( XPointSegPtr, pXPS_selected, crXptSegments ) {
                  if( pXPS_selected->Number() != number_min ||
                      pXPS_selected->XPT() != pXPT_min )
                    continue;
                  double d = fabs( startPos / length - pXPS_selected->StartPos() / pXPS_selected->LineLength() );
                  if( d < dMin ) {
                    dMin = d;
                    pXPS = pXPS_selected;
                  }
                }
              }
              break;
            }
            } /* End mode difference */


            if( pXPS == NULL )
              errorFlags |= DGFE::NEQUIL;
            else {
              GridPointSegPtr pGPS = FindGridPointSegBySegment( pXPS );
              assert( pGPS != NULL );

              pGPS->SetZone( zone );
              pGPS->SetDir( dir );
              pGPS->SetTargetDir( dir_target );
              pGPS->SetShortName( sName );
              pGPS->SetLongName( sDescr );

              if( _mode != 0 && /* Import mode only */
                  pXPT != pGPS->XPS()->XPT() ) {
                pGPS->SetDir( -pGPS->Dir() );
                pGPS->SetTargetDir( -pGPS->TargetDir() );
              }

              flags = 0;
              Str2Flags( sFlags, &flags, GPSF::flags );
              pGPS->SetFlags( flags );

              segments.push_back( pGPS->XPS() );

              /*xps=AddXPointSeg(a,xpt,i2,f1,f2);
              if (xps==NULL) {*ef|=DGFE::NEQUIL;goto bad;} */
            }
          }
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }

      if( _mode != 0 ) { /* Import mode */
        IComponentList vXPS_copy = xPointSegs;
        FOREACH_CONST( IComponentIter, itXPS, vXPS_copy )
          if( !Contains( segments, *itXPS ) )
            (*itXPS)->Delete();
        vXPS_copy.clear();
        segments.clear();
      }
    }
    else
      return DGFE::SYNTAX;
    break;

  case TKN::SurfaceZones114:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {

      if( _mode != 0 ) { /* Import mode only */
        IComponentList vSZ_copy = surfaceZones;
        FOREACH_CONST( IComponentIter, itSZ, vSZ_copy )
          (*itSZ)->Delete();
        vSZ_copy.clear();
      }

      int idGPS1, idGPS2;
      int orient, idElem;
      REPEAT( count ) {
        idElem = -1;
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++; // szNo2==-1 -> unlimited
        int r = sscanf( sLine, "%d%d%d%d%s%d",
                        &id, &idGPS1, &idGPS2, &orient, sFlags, &idElem );
        if( r == 4 ) {
          strcpy( sFlags, "" );
          r++;
        }
        if( r == 5 ) {
          if( sscanf( sFlags, "%d", &idElem ) != 0 )
            strcpy( sFlags, "" );
          r++;
        }
        if( r == 6 ) {
          fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
          fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;

          if( pEquil != null ) {
            ElementPtr pElem = null;
            if( _mode == 0 ) { /* Project mode only */
              pElem = pModel->Struct()->FindElementByID( idElem );
              //if( pEquil == null ) { errorFlags |= DGFE::NEQUIL; break; }
            }
            SurfaceZonePtr pSZ = AddSurfaceZone( id, idGPS1, idGPS2, orient, pElem );
            if( pSZ == NULL )
              errorFlags |= DGFE::NEQUIL;
            else {
              pSZ->SetShortName( sName );
              pSZ->SetLongName( sDescr );
              flags = 0;
              Str2Flags( sFlags, &flags, SurfaceZoneFlags::flags );
              pSZ->SetFlags( flags );
            }
          }
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else
      return DGFE::SYNTAX;
    break;
  case TKN::XPoint100:
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT""SCANFLT""SCANFLT,
                sToken, &ehint1.x, &ehint1.y, &ehint2.x, &ehint2.y ) == 5 ) {
      //if (a->equil!=NULL) i1=AddXPoint(a,f1,f2,f3,f4);  -- obsolete
      errorFlags |= DGFE::OLDTOPO;
    }
    else
      return DGFE::SYNTAX;
    break;

  case TKN::TopoName114:
    if( sscanf( _cpStr, "%s %s", sToken, sLine ) == 2 )
      pModel->SetTopologyName( sLine );
    else
      return DGFE::SYNTAX;
    break;

  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }

  return errorFlags;
}

int Topology::WriteObjects(Token _token, FILE* _pFile, int /*_mode*/ ) const
{
  const char* sToken = TKN::names[ _token ].s;

  switch( _token ) {
  case TKN::TopoName114:
    fprintf( _pFile, "%s %s\n", sToken, pModel->TopologyName().c_str() );
    break;
  case TKN::XPointsEx114: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)xPointTests.size() );
    FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests ) {
      NPoint c1 = pXPT->EndPos(1);
      NPoint c2 = pXPT->EndPos(2);
      Point center = pXPT->Center();
      fprintf( _pFile, "%d %d %d %d %d %e %e %e\n", c1.x, c1.y, c2.x, c2.y, pXPT->ID(), pXPT->Level(), center.x, center.y );
    }
    break;
  }
  case TKN::GridPointSegs114: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)gridPointSegs.size() );
    FOREACHPTRCONST( GridPointSegPtr, pGPS, gridPointSegs ) {
      XPointTestList xpts;
      xpts.push_back( pGPS->XPS()->XPT() );
      FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests )
        if( pXPT->HasXPointSeg( pGPS->XPS() ) )
          if( !Contains( xpts, pXPT ) )
            xpts.push_back( pXPT );

      int id2 = -1;
      int id1;
      double angle;
      double length = 0.;
      if( xpts.empty() ) {
        id1 = -1;
        angle = XPS_ANGLE_BAD;
      }
      else {
        pXPT = pGPS->XPS()->XPT();
        id1 = pXPT->ID();
        angle = pGPS->XPS()->CalcAngle();
        length = CalcLineLength( *pGPS->XPS()->Line() );
        if( xpts.size() > 1 ) {
          FOREACH_CONST( XPointTestIterConst, itXPT, xpts )
            if( *itXPT != pXPT )
              id2 = (*itXPT)->ID();
        }
      }

      XPointSegPtr pXPS = pGPS->XPS();
      fprintf( _pFile, "%d %d %e %e %d %d %d %d %d %e %e %s\n",
        pXPS->XPT()->ID(), pXPS->Number(), pXPS->StartPos(), pXPS->EndPos(),
        pGPS->Zone(), pGPS->Dir(), pGPS->TargetDir(), id1, id2, angle, length,
        Flags2Str( pGPS->Flags(), GridPointSegFlags::flags ) );
      fprintf( _pFile, "%s\n", pGPS->ShortName().c_str() );
      fprintf( _pFile, "%s\n", pGPS->LongName().c_str() );

      xpts.clear();
    }
    break;
  }
  case TKN::SurfaceZones114: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)surfaceZones.size() );
    FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones ) {
      fprintf( _pFile, "%d %d %d %d %s %d\n", pSZ->Zone(), pSZ->GPZone(1), pSZ->GPZone(2), pSZ->Orient(),
      Flags2Str( pSZ->Flags(), SurfaceZoneFlags::flags ), pSZ->HasInnermost() ? pSZ->Innermost()->Id() : -1 );
      fprintf( _pFile, "%s\n", pSZ->ShortName().c_str() );
      fprintf( _pFile, "%s\n", pSZ->LongName().c_str() );
    }
    break;
  }
  default:
    break;
  }

  return 0;
}



int Topology::Import( const std::string& _crsFileName, bool _detectXPoints,
                            int* _pErrFlags )
{
  SENDER_NAME( "LoadTopology" );
  pModel->SendMessage( WND_PROCESS, SENDER, DG3::LOADING_TOPOLOGY_FILE, ARGS( _crsFileName ) );

  int efPos = 0; // error flags
  int* ef = (_pErrFlags == null) ? &efPos : _pErrFlags;

  FILE* pFile = fopen( _crsFileName.c_str(), "rb" );
  if( pFile == null )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::FILENOTFOUND, ARGS( _crsFileName ) );

  ulong lines = 0;
  char str_buffer[512];
  int version = 0;
  while( fgets( str_buffer, sizeof( str_buffer ) - 1, pFile ) != null ) {
    lines++;
    if( sscanf( str_buffer, "DgFile%d", &version ) == 1 )
      break;
  }

  pModel->SendProgress( 10 ); //TODO: progress by lines

  if( version == 0 )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADFILE, ARGS( _crsFileName ) );

  if( version < 114 || version > 115 )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::OLD_FILE_NO_TOPOLOGY );

  fgets( str_buffer, sizeof( str_buffer ) - 1, pFile ); lines++;
  fgets( str_buffer, sizeof( str_buffer ) - 1, pFile ); lines++;

  int mode = _detectXPoints ? 2 : 1;

  while( fgets( str_buffer, sizeof( str_buffer ) - 1, pFile ) != null ) {
    lines++;
    RemoveLF( str_buffer );

    std::string sLine( str_buffer );
    char* psToken = strtok( str_buffer, " " );
    int token = Name2Int( psToken, TKN::names );
    if( token < 0 ) {
      /*pModel->SendMessage( LOG_WIP_WARN, SENDER, DG3::DG_FILE_SYNTAX_ERROR,
                           ARG( lines ) + "?" );*/
      continue;
    }
    int result = ReadObjects( (Token)token, sLine.c_str(), pFile, lines, mode );
    *ef |= result;
    if( result != 0 && result != DGFE::UNKNOWN_TOKEN )
      pModel->SendMessage( LOG_WIP_WARN, SENDER, DG3::DG_FILE_SYNTAX_ERROR,
                           ARG( lines ) + psToken );
  }

  fclose( pFile );

  int result = 0;
  RecalcCache( &result );

  sFileName = _crsFileName;

  pModel->SendMessage( LOG_RESULT, SENDER, result == 0 ? DG3::DONE : DG3::FAIL );

  return result;
}

void Topology::OutputPoints( FILE* _pFile ) const
{
  /*  if (a->xpoint!=NULL) {
    XY xy;
    int px,py,nx,ny; */

  /* Output X Points */

  zfprintf( _pFile, "xptcntr\n" );
  FOREACHPTRCONST( XPointTestPtr, pXPT, xPointTests )
    zfprintf( _pFile, "  %e, %e, %e\n", pXPT->Center().x, pXPT->Center().y, 0. );

  /* Output O Points */

  zfprintf( _pFile, "xlpcntr\n" );
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, surfaceZones ) {
    if( !pSZ->HasFlag( SZF::LIMITBYSURFACE ) )
      continue;
    double lvl = MAXDOUBLE; /* Workaround for the PC */
    Point p;
    if( FindSurfaceOriginPointEx( pSZ->Zone(), lvl, &p.x, &p.y )!=0)
      continue;
    if( pEquil->GetLevel( p, &lvl, null ) )
      continue;
    zfprintf( _pFile, "  %e, %e, %e\n", p.x, p.y, lvl );
  }
}
