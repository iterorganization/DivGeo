#include "Model.h"
#include "../../common/Version.h"
#include <time.h>

int Model::Save( const std::string& fName, FileMode mode ) const
{
  SENDER_NAME( "Save" );
  SendMessage( MT::WINDOW, AL::PROCESS, SENDER,
               DG3::SAVING_DG_FILE, ARGS( fName ) );

  FILE* file = fopen( fName.c_str(), "wb" );
  if( file == null )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER,
                        ERR::FWRITE, ARGS( fName ) );

  switch( mode ) {
  case DGFM::APP:     fprintf( file, "DivGeo data file>>\n" ); break;
  case DGFM::CONFIG:  fprintf( file, "DivGeo config file>>\n" ); break;
  case DGFM::TMP:     fprintf( file, "DivGeo temporary file>>\n" ); break;
  default:
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER,
                        DG3::BAD_FILE_MODE, ARGS( fName ) );
  }

  SendProgress( 5 );

  time_t timer;
  fprintf( file,
    "\n"
    "This file should not be modified manually.\n"
    "\n"
    "DG version:             %s\n"
    "File format version:    %d\n"
    "Creation time:          %s\n"
    "Last Modified time:     %s\n"
    "Original filename:      %s\n",
    DG_VERSION_STR_FULL,
    FILE_VERSION,
    creationTime.empty() ? "---" : creationTime.c_str(),
    asctime(localtime((time(&timer),&timer))),
    (const char*)fName.c_str() );

  if( pFlux->HasEquil() )
    fprintf( file, "Equilibrium file:       %s\n", pFlux->GetEquil()->FileName().c_str() );
  if( pTemplate != null )
    fprintf( file, "Template file:          %s\n", pTemplate->FileName().c_str() );
  if( pMesh != null && !pMesh->FileName().empty() )
    fprintf( file, "Mesh:                   %s\n\n", pMesh->FileName().c_str() );

  if( !pStruct->Elements().empty() )
    fprintf( file, "Elements:               %u\n", (unsigned)pStruct->Elements().size() );
  if( pFlux->HasEquil() ) {
    if( pFlux->GetTopology()->HasSurfaceZones() )
      fprintf( file, "Radial cells:           %s\n", pFlux->GetSurfacesStatsStr().c_str() );
    if( pFlux->GetTopology()->HasGridPointSegs() )
      fprintf( file, "Poloidal Cells:         %s\n", pFlux->GetGridPointStatsStr().c_str() );
  }
  if( !pStruct->Separators().empty() )
    fprintf( file, "Separators:             %u\n", (unsigned)pStruct->Separators().size() );
  if( !sources.empty() )
    fprintf( file, "Sources:                %u\n", (unsigned)sources.size() );

  fprintf( file,
    "\n------------------------------------------------------------------------\n\n");

  SendProgress( 10 );

  WriteApp_File( file );
  fclose( file );

  if( pFlux->HasTopology() )
    pFlux->GetTopology()->SetFileName( fName );

  SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return 0;
}

int Model::WriteObjects( Token _token, FILE* _pFile, int _mode ) const
{
  const char* sToken = TKN::names[ _token ].s;
  unsigned markedNumber = 0;
  IComponentPtr pObj = null;

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
    return pFlux->WriteObjects( _token, _pFile, _mode );


  case TKN::Mesh112:
  case TKN::MeshFingerprint112:
  case TKN::MeshPoint112:
  case TKN::MarkedMeshElements112:
  case TKN::MarkedMeshCells112:
  case TKN::MeshFileHeader113:
    if( pMesh != null )
      return pMesh->WriteObjects( _token, _pFile, _mode );
    break;


  case TKN::VarSetDefs100:
  case TKN::VarDefs100:
  case TKN::VarSetDefs101:
  case TKN::VarDefs101:
  case TKN::VarDefs102:
  case TKN::VarSets100:
    return pVars->WriteObjects( _token, _pFile, _mode );


  case TKN::Template100: break; /* obsolete */
  case TKN::Template110:
    if( pTemplate != NULL ) {
      TemplateParams tp = pTemplate->GetParameters();
      fprintf( _pFile, "%s %e %e %e %e %s\n", sToken,
               tp.incr.x, tp.incr.y, tp.angle, tp.scale, pTemplate->FileName().c_str() );
    }
    break;

  case TKN::Elems100:
  case TKN::Nodes100:
  case TKN::MarkedElems100:
  case TKN::Separators101:
  case TKN::MarkedSeparators104:
  case TKN::Chords106:
  case TKN::Chords115:
    return pStruct->WriteObjects( _token, _pFile, _mode );

  case TKN::SonnetData100: break; /* obsolete */

  case TKN::Sources104: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)sources.size() );
    FOREACHPTRCONST( SourcePtr, pSrc, sources )
      fprintf( _pFile, "%e %e\n", pSrc->X(), pSrc->Y() );
    break;
  }

  case TKN::MarkedSources104:
    markedNumber = 0;
    FOREACHOBJCONST( pObj, markedList )
      if( pObj->Type() == OT::SOURCE )
        markedNumber++;
    fprintf( _pFile, "%s %d\n", sToken, markedNumber );
    FOREACHOBJCONST( pObj, markedList )
      if( pObj->Type() == OT::SOURCE )
        fprintf( _pFile, "%u\n", (uint)IndexOf( sources, pObj ) );
    break;

  case TKN::MaxElemId101:
    fprintf( _pFile, "%s %d\n", sToken, maxElemId );
    break;
  case TKN::CreaTime101:
    fprintf( _pFile, "%s\n%s\n", sToken, creationTime.empty() ? "---" : creationTime.c_str() );
    break;
  case TKN::OutputMode109:
    fprintf( _pFile, "%s %s\n", sToken, Int2Name( outputMode, OutputModes::names ) );
    break;
  case TKN::ViewAttr103: break; /* obsolete */
  case TKN::ViewAttr105: break; /* obsolete */
  case TKN::ViewAttr115:
    fprintf( _pFile, "%s %e %e %e %e %e %s\n", sToken,
      min_position.x, min_position.y, max_position.x, max_position.y,
      xyAngle, Flags2Str( showFlags, ShowFlags::flags ) );
    break;
  case TKN::OutputFlags111:
    fprintf( _pFile, "%s %s\n", sToken, Flags2Str( outputFlags, OutputFlags::flags ) );
    break;

  case TKN::MeshSlidingOptions112:
    fprintf( _pFile, "%s %s %e %d\n", sToken,
      Int2Name( meshSlidingMode, MeshSlidingModes::names ),
      meshSlidingThreshold, doubleMeshBorder );
    break;

  case TKN::Comments316: {
    fprintf( _pFile, "%s %u %s\n", sToken, (uint)comments.size(),
             (HasAnyFlag( showFlags, SHW::COMMENTS ) ? scsShow : scsHide).c_str() );
    FOREACHPTRCONST( CommentPtr, pComment, comments ) {
      fprintf( _pFile, "%s (%e, %e) (%e, %e) %d %u\n",
               Int2Name( pComment->ObjType(), OT::names ),
               pComment->PosEnd().x, pComment->PosEnd().y,
               pComment->Position().x, pComment->Position().y,
               pComment->ParentScene(), (uint)pComment->Text().length() );
      short aData[1024];
      const wchar_t* cwsText = pComment->Text().c_str();
      size_t textSize = pComment->Text().length();
      for( size_t c = 0; c <= textSize; c++ )
        aData[c] = (short)cwsText[c];
      fwrite( aData, sizeof( short ), textSize, _pFile );
      fprintf( _pFile, "\n" );
    }
    break;
  }
  case TKN::Properties316:
    fprintf( _pFile, "%s 1\n", sToken );
    fprintf( _pFile, "SurfaceZoneRestriction = %s\n", Int2Name( szRestriction, SZR::names ) );
    break;
  default:
    break;
  }


  return 0;
}

void Model::WriteApp_File( FILE* _pFile ) const
{
  /* dg3 section */
  fprintf( _pFile, "Dg3File %d\n", FILE3_VERSION );
  /* end dg3 section */

  fprintf( _pFile, "DgFile %d {\n; For compatibility\n}\n", FILE_VERSION );

  WriteObjects( TKN::Equil100, _pFile );
  WriteObjects( TKN::EquilHints114, _pFile );

  WriteObjects( TKN::Template110, _pFile );

  SendProgress( 15 );

  WriteObjects( TKN::Mesh112, _pFile );
  WriteObjects( TKN::MeshFingerprint112, _pFile );
  WriteObjects( TKN::MeshPoint112, _pFile );
  WriteObjects( TKN::MeshFileHeader113, _pFile );

  SendProgress( 30 );

  WriteObjects( TKN::Nodes100, _pFile );
  WriteObjects( TKN::Elems100, _pFile );

  WriteObjects( TKN::XPointsEx114, _pFile );
  WriteObjects( TKN::GridPointSegs114, _pFile );
  WriteObjects( TKN::SurfaceZones114, _pFile );

  WriteObjects( TKN::SurfacesEx114, _pFile );
  WriteObjects( TKN::GridPointsEx114, _pFile );

  WriteObjects( TKN::Separators101, _pFile );
  WriteObjects( TKN::Sources104, _pFile );
  WriteObjects( TKN::Chords115, _pFile );

  SendProgress( 45 );

  WriteObjects( TKN::MarkedElems100, _pFile );
  WriteObjects( TKN::MarkedSeparators104, _pFile );
  WriteObjects( TKN::MarkedSources104, _pFile );
  WriteObjects( TKN::MarkedChords110, _pFile );

  SendProgress( 60 );

  WriteObjects( TKN::MarkedMeshElements112, _pFile );
  WriteObjects( TKN::MarkedMeshCells112, _pFile );

  SendProgress( 75 );

  WriteObjects( TKN::VarSetDefs101, _pFile );
  WriteObjects( TKN::VarSets100, _pFile );

  SendProgress( 90 );

  WriteObjects( TKN::MaxElemId101, _pFile );
  WriteObjects( TKN::CreaTime101, _pFile );
  WriteObjects( TKN::TopoName114, _pFile );
  WriteObjects( TKN::OutputMode109, _pFile );
  WriteObjects( TKN::OutputFlags111, _pFile );
  WriteObjects( TKN::MeshSlidingOptions112, _pFile );
  WriteObjects( TKN::ViewAttr115, _pFile );
  WriteObjects( TKN::Comments316, _pFile );
  WriteObjects( TKN::Properties316, _pFile );
}

int Model::WriteTargetsFile( const std::string& _fileName ) const
{
  SENDER_NAME( "WriteTargetsFile" );
  SendMessage( MT::WINDOW, AL::PROCESS, SENDER,
               DG3::WRITING_TARGETS_FILE, ARGS( _fileName ) );
  int result = 0;

  /* Check for virtual surfaces, if needed */
  if( HasOutputFlag( OF::SURFACES ) && pFlux->HasEquil() ) {
    if( pFlux->HasVirtualSurfaces() )
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::SURFACE_XY );
  }

  /* Open the output file, if asked to */
  FILE* file = null;
  if( !_fileName.empty() ) {
    file = fopen( _fileName.c_str(), "wb" );
    if( file == null )
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FWRITE );
  }

  /* Output the equilibrium filename */
  if( pFlux->HasEquil() ) //1410
    zfprintf( file, "# equil %s\n", pFlux->GetEquil()->FileName().c_str() );//1410 equil filename

  /* Output the mesh filename */
  if( HasMesh() )
    zfprintf( file, "# mesh %s\n", pMesh->FileName().c_str() );//1410 mesh filename from dg2.1.1

  /* Output the topology name */
  zfprintf( file, "# topo %s\n", TopologyName().c_str() );

  SendProgress( 10 );

  /* Create a group with all target vars */
  VarItemListContainerPtrArray targets;
  pVars->FindTargetsEx( targets );

  SendProgress( 50 );

  if( pFlux->HasEquil() ) {
    IComponentList segments = pFlux->GetTopology()->GridPointSegs();
    segments.sort( GridPointSegCompare );

    /* Write targets */
    FOREACHPTRCONST( GridPointSegPtr, pGPS, segments ) {
      if( !pGPS->IsUsed() )
        continue;
      IVarItemListPtr pTarget = pVars->GetIntersectedTarget( pGPS->XPS()->Line() );
      if( pTarget == null )
        continue;

      if( Contains( targets, pTarget ) ) {
        WriteTarget( file, pTarget, pGPS );
        Remove( targets, pTarget );
      }
      else
        result = SendMessage( LOG_WIP_WARN, SENDER, ERR::TARGET_CROSSED_2X );
    }

    if( result == 0 && !targets.empty() )
      result = SendMessage( LOG_WIP_WARN, SENDER, ERR::TARGET_NOT_CROSSED );

    pFlux->WriteTargetsFile( file );
  }

FreeContainersArray( targets, false );
  if( result != 0 ) {
    if( file != null )
      fclose( file );
    return SendMessage( LOG_RESULT, SENDER, DG3::FAIL ), result;
  }

  SendProgress( 90 );

  /* End of carre part */

  bool bCheck = !HasOutputFlag( OF::STRUCTURE );

  IVarItemListPtr pStructure = pVars->GetStructure();

  if( pStructure != null ) {
    IVarItemList elemsTargets, elemsClosed, elemsBroken;
    result = Element::OrderStructureElems( pStructure, &elemsTargets, &elemsClosed, &elemsBroken, null, CarreOutputMode() );
    int count = elemsTargets.size() + elemsClosed.size();
    FreeContainerList( &elemsTargets );
    FreeContainerList( &elemsClosed );
    FreeContainerList( &elemsBroken );
    if( result != 0 ) {
      SendMessage( WND_ERROR, "Element::OrderStructureElems", result );
      if( bCheck ) {
        if( file != null )
          fclose( file );
        return result;
      }
    }

    zfprintf( file, "clstruct %d\n", count );
  }

  if( file != null )
    fclose( file );

  SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return 0;
}

void Model::WriteTarget( FILE* _pFile, IVarItemListPtr _pTarget, GridPointSegPtr _pGPS ) const
{
  SENDER_NAME( "WriteTarget" );

  zfprintf( _pFile, "target %d\nline\n", _pGPS->Zone() );
  unsigned char offset = 0;
  if( _pGPS->HasFlag( GPSF::TARGET_CW ) ) {
    _pTarget->reverse(); // Reverse...
    offset = 1;
  }

  ElementPtr pElem = dgtype_cast< ElementPtr >( _pTarget->front() );
  NodePtr pNode = pElem->Node(1+offset);
  zfprintf( _pFile, "  %e , %e\n", pNode->X(), pNode->Y() );
  for( IVarItemIter dg__it__ = (*_pTarget).begin(), dg__it_end__ = (*_pTarget).end();//1410 ++begin removed
       dg__it__ != dg__it_end__; //TODO: need MACRO!
       ++dg__it__, pElem = (dg__it__ != dg__it_end__ ) ? dgtype_cast< ElementPtr >( *dg__it__ ) : null ) {
    pNode = pElem->Node(2-offset);
    zfprintf( _pFile, "  %e , %e\n", pNode->X(), pNode->Y() );
  }

  if( _pGPS->HasFlag( GPSF::TARGET_CW ) )
    _pTarget->reverse(); // ...and restore
}

int Model::WriteElemsAsDgTemplate( const std::string& _fileName, bool _bMarked ) const
{
  SENDER_NAME( "WriteElemsAsDgTemplate" );
  SendMessage( MT::WINDOW, AL::PROCESS, SENDER,
               DG3::WRITING_TEMPLATE_FILE, ARGS( _fileName ) );
  FILE* file = fopen( _fileName.c_str(), "wb" );
  if( file == null )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FWRITE, ARGS( _fileName ) );

  fprintf( file, "DivGeo template file>>\n" );

  const IComponentList* objects = _bMarked ? &markedList : &pStruct->Elements();
  if( objects->empty() )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::NO_MARKED_ELEMS );

  SendProgress( 10 );

  FOREACH_CONST( IComponentIterConst, it, *objects ) {
    if( (*it)->Type() != OT::ELEMENT )
      continue; // TODO: set label. really need?
    ElementPtr pElem = dgtype_cast< ElementPtr >( *it );
    Point p1 = pElem->Node(1)->Position();
    Point p2 = pElem->Node(2)->Position();
    fprintf( file, "%e %e\n%e %e\n", p1.x, p1.y, p2.x, p2.y );
  }

  SendMessage( LOG_RESULT, SENDER, DG3::DONE );
  return 0;
}


int Model::WriteOutputFile( const std::string& _fileName ) const
{
  SENDER_NAME( "WriteOutputFile" );
  SendMessage( MT::WINDOW, AL::PROCESS, SENDER,
               DG3::WRITING_OUTPUT_FILE, ARGS( _fileName ) );

  /* Determine the maximum element number actually used */
  int maxId = 0;
  for( int id = 0; id <= maxElemId; id++ ) {
    if( pStruct->FindObject( id ) == null )
      continue;
    maxId = id;
  }

  FILE* file = null;
  if( !_fileName.empty() ) {
    file = fopen( _fileName.c_str(), "wb" );
    if( file == null )
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FWRITE, ARGS( _fileName ) );
  }

  zfprintf( file, "userdata\n" );
  zfprintf( file, "p1\n" );

  SendProgress( 5 );

  for( int id = 0; id <= maxId; id++ ) {
    IVarOriginPtr pObj = pStruct->FindObject( id );
    if( pObj == null )
      zfprintf( file, " 0,0,0\n" );
    else {
      switch( pObj->Type() ) {
      case OT::ELEMENT: {
        ElementPtr pElem = dgtype_cast< ElementPtr >( pObj );
        Point pt = pElem->Node(1)->Position();
        zfprintf( file, "  %e, %e, %e\n", pt.x, pt.y, pt.z );
        break;
      }
      case OT::SEPARATOR: {
        SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObj );
        zfprintf( file, "  %e, %e, %e\n", pSep->X(), pSep->Y(), 0. );
        break;
      }
      default:
        SendMessage( MT::WINDOW, AL::ERROR, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
        if( file != null )
          fclose( file );
        return 1;
      }
    }
  }

  SendProgress( 20 );

  zfprintf( file, "p2\n" );
  for( int id = 0; id <= maxId; id++ ) {
    IVarOriginPtr pObj = pStruct->FindObject( id );
    if( pObj == null )
      zfprintf( file, " 1,1,1\n" );
    else {
      switch( pObj->Type() ) {
      case OT::ELEMENT: {
        ElementPtr pElem = dgtype_cast< ElementPtr >( pObj );
        Point pt = pElem->Node(2)->Position();
        zfprintf( file, "  %e, %e, %e\n", pt.x, pt.y, pt.z );
        break;
      }
      case OT::SEPARATOR: {
        SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObj );
        Point pt = pSep->Node()->Position();
        zfprintf( file, "  %e, %e, %e\n", pt.x, pt.y, pt.z );
        break;
      }
      default:
        SendMessage( MT::WINDOW, AL::ERROR, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
        if( file != null )
          fclose( file );
        return 1;
      }
    }
  }

  SendProgress( 35 );

  zfprintf( file, "misselem\n" );
  for( int id = 0; id <= maxId; id++ ) {
    if( pStruct->FindObject( id ) == null /*|| IsChordElem(obj)*/)
      zfprintf( file, "  %d\n", id );
  }

  zfprintf( file, "sprtrs\n" );
  for( int id = 0; id <= maxId; id++ ) {
    IVarOriginPtr pObj = pStruct->FindObject( id );
    if( pObj != null && pObj->Type() == OT::SEPARATOR )
      zfprintf( file, "  %d\n", id );
  }

  zfprintf( file, "chords\n" );
  FOREACHPTRCONST( ChordPtr, pCh, pStruct->Chords() ) {
    Point p1 = pCh->Point_1();
    Point p2 = pCh->Point_2();
    zfprintf( file, "  %e, %e, %e, %e\n", p1.x, p1.y, p2.x, p2.y );
  }

  zfprintf( file, "sources\n" );
  FOREACHPTRCONST( SourcePtr, pSrc, sources )
    zfprintf( file, "  %e, %e\n", pSrc->X(), pSrc->Y() );

  SendProgress( 50 );

  if( pSonnetData != null ) {
    ComponentListContainerPtrArray* ci = CreateCellsInfo( null, null );
    if( ci != null ) {
      int i = 0;
      FOREACH_CONST( ComponentListContainerPtrItemConst, itCont, *ci ) {
        if( (*itCont)->pList == null )
          break;
        zfprintf( file, "cells %d\n", i + 1 );

        FOREACH_CONST( IComponentIterConst, itObj, *(*itCont)->pList )
          zfprintf( file, "  %d\n", GetObjectId( *itObj ) );

        i++;
      }
      FreeContainersArray( *ci );
      delete ci;
    }
  }

  SendProgress( 60 );

  pVars->OutputVars( file, maxId );

  SendProgress( 80 );

  if( pFlux->HasEquil() )
    pFlux->GetTopology()->OutputPoints( file );

  zfprintf( file, "finish\n" );

  if( file != null )
    fclose( file );

  SendMessage( LOG_RESULT, SENDER, DG3::DONE );
  return 0;
}



int Model::WriteStructureFile( const std::string& _fileName ) const
{
  SENDER_NAME( "WriteStructureFile" );
  SendMessage( WND_PROCESS, SENDER, DG3::WRITING_STRUCTURE_FILE, ARGS( _fileName ) );

  bool bCheck = !HasOutputFlag( OF::STRUCTURE );

  /* Do not output empty values */
  IVarItemListPtr pList = pVars->GetStructure( true );
  if( pList == null )
    return bCheck ? SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::BADSTRUCTURE ) : 0;

  /* Open output file */
  FILE* file = null;
  if( !_fileName.empty() ) {
    file = fopen( _fileName.c_str(), "wb" );
    if( file == null )
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FWRITE, ARGS( _fileName ) );
  }

  SendProgress( 5 );

  /* Calculate bounding rectangle dimensions */
  Point pmin( 0. ), pmax( -1., 0. );
  FOREACHVARITEMCONST( ElementPtr, pElem_i, *pList )
    pElem_i->CalcExtens( &pmin, &pmax );

  /* Calculate the number of rings */
  ulong num = 0;
  ElementPtr pElem0 = null; // 1409 and for{} to fix wrong number of rings
  FOREACHVARITEMCONST( ElementPtr, pElem, *pList ) {
    ElementPtr pElem1 = pElem->GetNextElem( pList, 1 );
    if( pElem1 != pElem0 || pElem1 == null )
      num++;
    pElem0 = pElem;
    /* if (GetNextElem(e,v->val,1)!=e0 ||) i++; */
  }

  /* Write into the file */
  zfprintf( file, "%d\n", num );

  SendProgress( 10 );

  /* ix will be used for ring separation and ix1 for element output */
  pElem = null; //1409
  IVarItemIter itElem = pList->begin();
  IVarItemIter itElem1 = pList->begin();
  pElem0 = dgtype_cast< ElementPtr >( pList->front() );
  ulong num_ring = 0; /* Number of the current ring, starting from 1 */

  /* Process all rings */
  while( pElem0 != null ) {
    num_ring++;
    pElem = pElem0;

    /* Follow the ring to figure out the number of elements in it */
    /* NOTE: This assumes a formatted structure; otherwise it will crash */
    int broken = 0;

    while( pElem->GetNextElem( pList, 2 ) != pElem0 ) {
      if( pElem->GetNextElem( pList, 2 ) == null ) {
        broken = 1;
        break;
      }
      ++itElem;
      pElem = (itElem == pList->end()) ? null : dgtype_cast< ElementPtr >( *itElem );
    }
    ulong index = IndexOf( *pList, pElem ) -
                  IndexOf( *pList, pElem0 ) + 1;
    ++itElem;
    pElem = (itElem == pList->end()) ? null : dgtype_cast< ElementPtr >( *itElem ); /* Will be NULL for the last ring */

    /* Write to file */
    zfprintf( file, "%d\n", (num_ring == 1 && not CarreOutputMode()) ? (index + 5) : (index + broken));//1410 not carre


    /* Write nodes */
    ElementPtr pElem00 = null;
    while( pElem0 != pElem && pElem0 != null ) {
      zfprintf( file, "  %e , %e\n", pElem0->Node(1)->X(), pElem0->Node(1)->Y() );
      pElem00 = pElem0;
      ++itElem1;
      pElem0 = (itElem1 == pList->end()) ? null : dgtype_cast< ElementPtr >( *itElem1 );
    }

    if( broken != 0 )
      zfprintf( file, "  %e , %e\n", pElem00->Node(2)->X(), pElem00->Node(2)->Y() );

    /* Write the bounding rectangle for the outermost ring */
    if( num_ring == 1 && not CarreOutputMode() ) { //1409: not carre
      pElem = dgtype_cast< ElementPtr >( pList->front() );
      zfprintf( file, "  %e , %e\n", pElem->Node(1)->X(), pElem->Node(1)->Y() );
      zfprintf( file, "  %e , %e\n", pmax.x, pmax.y );
      zfprintf( file, "  %e , %e\n", pmax.x, pmin.y );
      zfprintf( file, "  %e , %e\n", pmin.x, pmin.y);
      zfprintf( file, "  %e , %e\n", pmin.x, pmax.y );
    }
  }

  if( file != null )
    fclose( file );

  SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return 0;
}

ulong Model::GetOutputValidationFlags() const
{
  ulong valid_flags = AF::ALL;

  pVars->GetOutputValidationFlags( outputFlags, valid_flags );

  if( HasAnyFlag( outputFlags, OF::CELLS) &&
      !pStruct->Separators().empty() ) {
    ComponentListContainerPtrArray* info = CreateCellsInfo( null, null );
    if( info != null ) {
      FreeContainersArray( *info );
      delete info;
    }
    else
      valid_flags &= ~AF::CELLS;
  }

  return valid_flags;
}

int Model::SendValidationResult( ulong _flags, const std::string& _sender ) const
{
  if( !HasAnyFlag( _flags, AF::VARS ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::EMPTYVARS );
  if( !HasAnyFlag( _flags, AF::STRUCTURE ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::EMPTYSTRUCTURE );
  if( !HasAnyFlag( _flags, AF::TARGETS ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::EMPTYTARGETS );
  if( !HasAnyFlag( _flags, AF::SURFACES ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::BADSURFACES );
  if( !HasAnyFlag( _flags, AF::GRIDPOINTS ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::BADGRIDPOINTS );
  if( !HasAnyFlag( _flags, AF::CELLS ) )
    return SendMessage( MT::WINDOW, AL::ERROR, _sender, ERR::BADCELLS );
  return 0;
}
