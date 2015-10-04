#include "FluxModel.h"

FluxModel::FluxModel( ModelPtr _pModel ):
  pModel( _pModel ),
  pEquil( null ),
  pTopology( null ),
  pTopology_native( null )
{
  DG_HIERARCHY_INIT;
}

FluxModel::~FluxModel()
{
  ReleaseObject( &pEquil );

  if( pTopology != pTopology_native &&
      pTopology_native != null )
    delete pTopology_native;

  ReleaseObject( &pTopology );

  Release();
}

int FluxModel::ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode )
{
  char sFileName[ DG_FNAME_LEN ];
  char sCreator[ DG_LINE_LEN ], sLine[ DG_LINE_LEN ];
  char sToken[ 32 ];
  uint count;
  Point pos;
  int zone;
  double value;

  int errorFlags = 0;

  switch( _token ) {
  case TKN::Equil100:
    if( sscanf( _cpStr, "%s %s", sToken, sFileName ) == 2 ) {
      if( AddEquil( sFileName ) != 0 )
        /*if( AddEquil( SplitFilePath( sFileName )[1].c_str() ) != 0 )*/
          errorFlags |= DGFE::NOEQUIL;
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Surfaces100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT, &pos.x, &pos.y ) == 2 &&
            pEquil != null ) {
           AddSurfaceEx( pos, NULL );
           errorFlags |= DGFE::OLDTOPO;
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Surfaces108:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT"%s", &pos.x, &pos.y, sCreator ) == 3 &&
            pEquil != null) {
          SurfaceExPtr pSX = AddSurfaceEx( pos );
          if( pSX != NULL ) {
            pSX->ChangeCreatorId( CreatorId( sCreator ) );
            errorFlags |= DGFE::OLDTOPO;
          }
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::GridPoints100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d"SCANFLT, &zone, &value ) == 2 &&
            pEquil != null )
          AddGridPointEx( zone, value );
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::GridPoints108:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d"SCANFLT"%s", &zone, &value, sCreator ) == 3 &&
            pEquil != null ) {
          GridPointExPtr pGPX = AddGridPointEx( zone, value );
          if( pGPX != NULL )
            pGPX->ChangeCreatorId( CreatorId( sCreator ) );
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::SurfacesEx114:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d"SCANFLT""SCANFLT""SCANFLT"%s",
                    &zone, &value, &pos.x, &pos.y, sCreator ) == 5 )
        {
          if( pEquil == null ) {
            errorFlags |= DGFE::NEQUIL;
            continue;
          }
          SurfaceExPtr pSX = null;
          if( zone != SZN_XY )
            pSX = AddSurfaceEx( zone, value );
          else
            pSX = AddSurfaceEx( pos );

          if( pSX == NULL )
            errorFlags |= DGFE::NEQUIL;
          else
            pSX->ChangeCreatorId( CreatorId( sCreator ) );
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::GridPointsEx114:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d"SCANFLT"%s",
                   &zone, &value, sCreator ) == 3 )
        {
          if( pEquil == null ) {
            errorFlags |= DGFE::NEQUIL;
            continue;
          }
          GridPointExPtr pGPX = AddGridPointEx( zone, value );
          if( pGPX == NULL )
            errorFlags |= DGFE::NEQUIL;
          else
            pGPX->ChangeCreatorId( CreatorId( sCreator ) );
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;

  case TKN::EquilHints114:
  case TKN::XPointsEx114:
  case TKN::GridPointSegs114:
  case TKN::SurfaceZones114:
  case TKN::XPoint100:
  case TKN::TopoName114:
    return pTopology->ReadObjects( _token, _cpStr, _pFile, _rLine, _mode );

  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }

  return errorFlags;
}

int FluxModel::WriteObjects(Token _token, FILE* _pFile, int _mode) const
{
  const char* sToken = TKN::names[ _token ].s;

  switch( _token ) {
  case TKN::Equil100:
    if( pEquil != NULL )
      fprintf( _pFile, "%s %s\n", sToken, pEquil->FileName().c_str() );
    break;
  case TKN::EquilHints114:
    if( pEquil != null ) {
      Point p1 = pEquil->PointFirst();
      Point p2 = pEquil->PointLast();
      fprintf( _pFile, "%s %e %e %e %e\n", sToken, p1.x, p1.y, p2.x, p2.y );
    }
    break;

  case TKN::Surfaces100: break; /* obsolete */
  case TKN::Surfaces108: break; /* obsolete */
  case TKN::GridPoints100: break; /* obsolete */
  case TKN::GridPoints108: break; /* obsolete */

  case TKN::XPoint100:
  case TKN::TopoName114:
  case TKN::XPointsEx114:
  case TKN::GridPointSegs114:
  case TKN::SurfaceZones114:
    if( pTopology != null )
      pTopology->WriteObjects( _token, _pFile, _mode );
    break;

  case TKN::SurfacesEx114: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)surfacesEx.size() );
    FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
      Point o = pSX->Origin();
      fprintf( _pFile, "%d %e %e %e %s\n", pSX->Zone(), pSX->Level(), o.x, o.y,
               pSX->GetCreatorId().ToStdString().c_str() );
    }
    break;
  }
  case TKN::GridPointsEx114: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)gridPointsEx.size() );
    FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
      fprintf( _pFile, "%d %e %s\n", pGPX->Zone(), pGPX->Value(),
               pGPX->GetCreatorId().ToStdString().c_str() );
    }
    break;
  }
  default:
    break;
  }

  return 0;
}


void FluxModel::ProcessChanges( ulong _changeFlags )
{
  if( HasAnyFlag( _changeFlags, CHF::TARGETS|CHF::TOPOLOGY ) && pEquil != null )
    pTopology->RecalcAllGridPointSegSLines();

  if( HasAnyFlag( _changeFlags, CHF::TOPOLOGY ) && pEquil != null ) {
    pTopology->RecalcAllSurfaceZonesSignes();
    pTopology->SortZonesAndSegments();
    UpdateAfterTopologyChange();
  }
}


void FluxModel::UpdateAfterTopologyChange()
{
  FOREACHPTRCONST( SurfaceExPtr, pSX, surfacesEx ) {
    pSX->RecalcPos();
    pModel->Agent()->AddToUpdate( pSX, UT::UPDATE );
  }

  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
    pGPX->RecalcPos();
    pModel->Agent()->AddToUpdate( pGPX, UT::UPDATE );
  }
}


ulong FluxModel::AddEquil( const char* _fName )
{
  SENDER_NAME( "AddEquil" );
  ValidatePtr( _fName, "AddEquil_" );

  if( pEquil != null ) {
    pEquil->Delete();
    //TODO:
    //if (i) return i;
  }

  const char* cpShortName = GetShortFName( _fName );

  pModel->SendMessage( WND_PROCESS, SENDER, DG3::LOADING_EQUIL_FILE, ARGS( cpShortName ) );

  EquilPtr pEquil_tmp = new Equil( pModel, _fName );
  ActAddEquil aae( pModel, pEquil_tmp );
  ulong result = aae.Do();

  if( result == ERR::FILENOTFOUND && !pModel->FileName().empty() ) {
    std::string fname_tmp = CombinateFilePath( pModel->FileName(), _fName );
    pEquil_tmp->SetFileName( fname_tmp );

    result = aae.Do();
    if( result == ERR::FILENOTFOUND ) {
      pEquil_tmp->SetFileName( SplitFilePath( _fName, 1 ) );
      result = aae.Do();
      if( result == ERR::FILENOTFOUND ) {
        pModel->SendMessage( WND_ERROR, SENDER, ERR::FILENOTFOUND, ARGS( cpShortName ) );
        pEquil_tmp = null;
      }
    }
  }

  if( pEquil_tmp != NULL && result == 0 ) {
    pEquil_tmp->CalcSplines();
    if( !pEquil_tmp->SplinesAreOk() ) {
      result = pModel->SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::EQUIL_SPLINE );
      pEquil_tmp->Delete();
    }
  }

  if( result == 0 )
    pModel->SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return result;
}


void FluxModel::SetEquil( EquilPtr _pEquil )
{
  ReleaseObject( (IComponentPtr*)&pEquil );
  pEquil = DG_SHARE( _pEquil );
}


int FluxModel::AddTopology( const std::string& _crsFileName, bool _detectXPoints )
{
  SENDER_NAME( "AddTopology" );

  TopologyPtr pTopo_tmp = new Topology( pModel );

  if( !_detectXPoints ) { // Clone existed XPTs omitting Actions
    FOREACHPTRCONST( XPointTestPtr, pXPT, pTopology->XPointTests() ) {
      XPointTestPtr pXPT_copy = new XPointTest( *pXPT );
      pTopo_tmp->IncludeObject( pXPT_copy );
    }
  }

  if( pTopology != null )
    pTopology->Delete();

  ActAddTopology( pModel, pTopo_tmp, DO_AT_ONCE );

  if( !_crsFileName.empty() )
    return pTopology->Import( _crsFileName, _detectXPoints, null );
  return 0;
}


void FluxModel::SetTopology( TopologyPtr _pTopology )
{
  ReleaseObject( (IComponentPtr*)&pTopology );
  pTopology = DG_SHARE( _pTopology );
  DG_HIERARCHY_INIT;
}
