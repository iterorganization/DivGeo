#include "Model.h"

std::string Model::ReadText( FILE* _pFile, ulong& _rLines )
{
  char line[ DG_FNAME_LEN * 2 ];
  _rLines++;
  if( fgets( line, sizeof( line ) - 1, _pFile ) == NULL )
    return "";
  int lines_num = 0;
  if( sscanf( line, "text101 %d", &lines_num ) != 1 )
    return "";

  std::string text;
  for( int i = 0; i < lines_num; i++ ) {
    if( fgets( line, sizeof( line ) - 1, _pFile ) == NULL )
      break;
    text += line;
    _rLines++;
  }
  return text;
}



int Model::LoadSonnetMeshFile( const std::string& _fName, MeshCellDataArray* pMCDs, StdStringArray* pStrs /*CMT*/, long* pFingerprint )
{
  double x1,y1,x2,y2,x3,y3,x4,y4,ratio,cX,cY;
  char s[DG_FNAME_LEN*2];
  int eN,i,j;
  bool bBeg = true;
  ulong fp=0,fpl=0,c_no=0,l_no=0;

  FILE* f = fopen( _fName.c_str(), "rb" );
  if( f == NULL )
    return ERR::FILENOTFOUND;

  while( fgets( s, sizeof(s) - 1, f ) != NULL)
  {
    if( sscanf( s, "   Element    %d = (  %d,  %d): ( "SCANFLT","SCANFLT")  ( "SCANFLT","SCANFLT")",
      &eN, &i, &j, &x1, &y1, &x2, &y2 ) !=7 ) {
      if( bBeg )
        pStrs->push_back( std::string( s ) );
      continue;
    }

    bBeg = false;

    if( eN < 0 || i < 0 || j < 0)
    {
      fclose(f);
      delete pMCDs;
      delete pStrs;
      return ERR::BADFILE;
    }

    fpl = 0;
    for( int k = 0; s[k]; k++ )
      if( isgraph( s[k] ) )
        fpl = fpl ^ ( ( (unsigned)s[k] & 0xff ) * ( 1 + c_no++ % 255 ) );

    assert( fgets( s, sizeof(s) - 1, f ) != NULL );
    if( sscanf( s, "   Field ratio  =  "SCANFLT"             ( "SCANFLT","SCANFLT")", &ratio, &cX, &cY ) != 3 )
    {
      fclose(f);
      delete pMCDs;
      delete pStrs;
      return ERR::BADFILE;
    }

    for( int k = 0; s[k]; k++ )
      if( isgraph( s[k] ) )
        fpl = fpl ^ ( ( (unsigned)s[k] & 0xff ) * ( 1 + c_no++ % 255 ) );

    assert( fgets( s, sizeof(s) - 1, f ) != NULL );
    if( sscanf( s, "                             ( "SCANFLT","SCANFLT")      ( "SCANFLT","SCANFLT")", &x3, &y3, &x4, &y4 ) != 4 )
    {
      fclose(f);
      delete pMCDs;
      delete pStrs;
      return ERR::BADFILE;
    }

    for( int k = 0; s[k]; k++ )
      if( isgraph( s[k] ) )
        fpl = fpl ^ ( ( (unsigned)s[k] & 0xff ) * ( 1 + c_no++ % 255 ) );
    fp = fp ^ fpl * ( 1 + l_no++ % 32766 );

    if( j < 0 || i < 0 )
    {
      fclose( f );
      delete pMCDs;
      delete pStrs;
      return ERR::BADFILE;
    }

    MeshCellDataPtr pMCD = new MeshCellData( eN, NPoint( i, j ), 
      Point( x3, y3 ) * SONNET_FACTOR, Point( x4, y4 ) * SONNET_FACTOR, 
      Point( x1, y1 ) * SONNET_FACTOR, Point( x2, y2 ) * SONNET_FACTOR, 
      Point( cX, cY ) * SONNET_FACTOR, ratio );
    
    pMCDs->push_back( pMCD );
  }

  fclose( f );

  *pFingerprint = fp;

  SendProgress( 50 );

  return 0;
}


int Model::ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode )
{
  char sFileName[ DG_FNAME_LEN ];
  char sToken[ 32 ];
  char sLine[ DG_LINE_LEN ];
  char sFlags[ DG_STR_LEN ];
  Point incr, pos, pos2;
  uint count;
  int id;

  int errorFlags = 0;

  switch( _token ) {
  case TKN::Equil100:
  case TKN::EquilHints114:
  case TKN::Surfaces100:
  case TKN::Surfaces108:
  case TKN::GridPoints100:
  case TKN::GridPoints108:
  case TKN::XPointsEx114:
  case TKN::GridPointSegs114:
  case TKN::SurfaceZones114:
  case TKN::SurfacesEx114:
  case TKN::GridPointsEx114:
  case TKN::XPoint100:
  case TKN::TopoName114:
    return pFlux->ReadObjects( _token, _cpStr, _pFile, _rLine, _mode );


  case TKN::MeshFingerprint112:
  case TKN::MeshPoint112:
  case TKN::MarkedMeshElements112:
  case TKN::MarkedMeshCells112:
  case TKN::MeshFileHeader113:
    if( pMesh == null )
      errorFlags |= DGFE::MESH_FP;
    else
      return pMesh->ReadObjects( _token, _cpStr, _pFile, _rLine, _mode );
    break;


  case TKN::VarSetDefs100:
  case TKN::VarDefs100:
  case TKN::VarSetDefs101:
  case TKN::VarDefs101:
  case TKN::VarDefs102:
  case TKN::VarSets100:
    return pVars->ReadObjects( _token, _cpStr, _pFile, _rLine, _mode );

  case TKN::Elems100:
  case TKN::Nodes100:
  case TKN::MarkedElems100:
  case TKN::Separators101:
  case TKN::MarkedSeparators104:
  case TKN::Chords106:
  case TKN::Chords115:
  case TKN::MarkedChords110:
    return pStruct->ReadObjects( _token, _cpStr, _pFile, _rLine, _mode );

  case TKN::Template100: {
    int angle;
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT"%d%[^\r\n]",//1408
                sToken, &incr.x, &incr.y, &angle, sFileName ) == 5 ) {
      if( AddTemplate( sFileName, incr, M_PI * angle / 2, 1 ) != 0 )
        errorFlags |= DGFE::NOTEMPL;
    }
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::Template110: {
    double angle, scale;
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT""SCANFLT""SCANFLT"%[^\r\n]",//1408
                sToken, &incr.x, &incr.y, &angle, &scale, sFileName ) == 6 ) {
      if( AddTemplate( sFileName, incr, angle, scale ) != 0 )
        errorFlags |= DGFE::NOTEMPL;
    }
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::SonnetData100:
  case TKN::Mesh112:
    if( sscanf( _cpStr, "%s %s", sToken, sFileName ) == 2 ) {
      if( AddMesh( sFileName ) != 0 )
        errorFlags |= DGFE::NOSONNET;
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Sources104:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT, &pos.x, &pos.y ) == 2 )
          AddSource( pos );
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;

  case TKN::MarkedSources104:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%u", &id ) == 1 )
          MarkObject( At( sources, id ) );
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;

  case TKN::MaxElemId101:
    if( sscanf( _cpStr, "%s %d", sToken, &id ) == 2 ) {
      if( id > maxElemId ) // MB: to prevent overwriting of real max id
        SetMaxElemId( id );
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::CreaTime101:
   if( sscanf( _cpStr, "%s", sToken ) == 1 ) {
      fgets( sLine, DG_LINE_LEN - 1, _pFile ); RemoveLF( sLine ); _rLine++;
      SetCreationTime( sLine );
   }
   else return DGFE::SYNTAX;
   break;
  case TKN::OutputMode109:
    if( sscanf( _cpStr, "%s %s", sToken, sFlags ) == 2 )
      SetOutputMode( Name2Int( sFlags, OutputModes::names ) );
    else return DGFE::SYNTAX;
    break;
  case TKN::ViewAttr103:
    // ViewAttr103 may contain invalid flags due to errors in some versions of DG
    break;
  case TKN::ViewAttr105:
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
                sToken, &pos.x, &pos.y, &pos2.x, &pos2.y, sFlags ) == 6 ) {
      Str2Flags( sFlags, &showFlags, ShowFlags::flags );
      min_position = pos;
      max_position = pos2;
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::ViewAttr115: {
    double angle;
    if( sscanf( _cpStr, "%s "SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
                     sToken, &pos.x, &pos.y, &pos2.x, &pos2.y, &angle, sFlags ) == 7 ) {
      Str2Flags( sFlags, &showFlags, ShowFlags::flags );
      min_position = pos;
      max_position = pos2;
      xyAngle = angle;
    }
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::OutputFlags111:
    if( sscanf( _cpStr, "%s %s", sToken, sFlags ) == 2 )
      Str2Flags( sFlags, &outputFlags, OutputFlags::flags );
    else return DGFE::SYNTAX;
    break;
  case TKN::MeshSlidingOptions112: {
    double threshold;
    int dborderFlag;
    if( sscanf( _cpStr, "%s %s"SCANFLT"%d",
                sToken, sFlags, &threshold, &dborderFlag ) == 4 ) {
      SetMeshSlidingMode( Name2Int( sFlags, MeshSlidingModes::names ) );
      SetMeshSlidingThreshold( threshold );
      SetDoubleMeshBorderFlag( dborderFlag != 0 );
    }
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::Comments316:
    if( sscanf( _cpStr, "%s %u %s", sToken, &count, sFlags ) == 3 ) {
      SETFLAG( showFlags, SHW::COMMENTS, scsShow.compare( sFlags ) == 0 );
      REPEAT( count ) {
        Point pos, end;
        int textSize, sceneType;
        int result = 0;
        while( result != 7 ) {
          fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
          result = sscanf( sLine, "%s ("SCANFLT", "SCANFLT") ("SCANFLT", "SCANFLT") %d %d",
                           sFlags, &end.x, &end.y, &pos.x, &pos.y, &sceneType, &textSize );
        }

        short aData[1024];
        fread( aData, sizeof(short), textSize, _pFile );
        aData[textSize] = 0;
        wchar_t wsText[1024]; // For compatibility of Windows and Linux files
        for( int c = 0; c <= textSize; c++ )
          wsText[c] = (wchar_t)aData[c];

        ObjectType ot = (ObjectType)Name2Int( sFlags, OT::names );
        AddComment( ot, end, pos, wsText, sceneType );
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Properties316:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      if( count < 1 ) {
        errorFlags |= DGFE::SYNTAX;
        break;
      }
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "SurfaceZoneRestriction = %s", sFlags ) == 1 )
          SetSZRestriction( Name2Int( sFlags, SZR::names ) );
        else {
          errorFlags |= DGFE::SYNTAX;
          continue;
        }
      }
    }
    else return DGFE::SYNTAX;
    break;
  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }


  return errorFlags;
}


int Model::ReadApp_File(FILE* f, const std::string& _fName, int* ef )
{
  SENDER_NAME( "ReadApp_File" );

  char s[ DG_LINE_LEN ];
  *ef = 0;

  // Count lines
  ulong lines = 0, linesExt = 0;
  while( fgets( s, sizeof(s)-1, f ) != NULL )
    lines++;
  fseek( f, 0, SEEK_SET );
  ulong deltaLines = 50;
  ulong deltaPercent = deltaLines / lines;
  if( deltaPercent < 10 ) {
    deltaPercent = 10;
    deltaLines = deltaPercent * lines / 100;
  }
  lines = 0;
  ulong percents = 0;

  // Detect file version
  int version = 0;
  while( fgets( s, sizeof(s)-1, f ) != NULL ) {
    lines++;
    if( sscanf( s, "DgFile%d", &version ) == 1 )
      break;
  }

  if( version == 0 )
    return SendMessage( WND_ERROR, SENDER, ERR::BADFILE, ARGS( std::string( _fName ) ) );

  if( version < 100 || version > 115 )
    return SendMessage( WND_ERROR, SENDER, ERR::BADFILEVERSION, ARGS( std::string( _fName ) ) );

  fgets( s, sizeof(s) - 1, f );
  fgets( s, sizeof(s) - 1, f );
  lines += 2;

  while( fgets( s, sizeof(s) - 1, f ) != NULL )
  {
    lines++;
    while( lines >= deltaLines ) {
      lines -= deltaLines;
      linesExt += deltaLines;
      percents += deltaPercent;
      if( percents >= 100 )
        percents = 99;
      SendProgress( percents );
    }

    RemoveLF(s);

    if( s[0] == 0 )
      continue;

    std::string sLine( s );
    char* psToken = strtok( s, " " );
    int token = Name2Int( psToken, TKN::names );
    if( token < 0 ) {
      /*SendMessage( LOG_WIP_WARN, SENDER, DG3::DG_FILE_SYNTAX_ERROR,
                   ARG( lines + linesExt ) + "?" );*/
      continue;
    }
    int result = ReadObjects( (Token)token, sLine.c_str(), f, lines );
    *ef |= result;
    if( result != 0 )
      SendMessage( LOG_WIP_WARN, SENDER, DG3::DG_FILE_SYNTAX_ERROR,
                   ARG( lines + linesExt) + psToken );
  }

  if( pFlux->HasTopology() )
    pFlux->GetTopology()->SetFileName( _fName );

  if( *ef & DGFE::OLDTOPO )
    pFlux->ConvertOldDgSurfaces( ef );

  return 0;
}
