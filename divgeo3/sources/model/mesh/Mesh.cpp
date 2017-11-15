#include "Mesh.h"
#include "../core/Model.h"

std::string Mesh::Description() const
{ return pModel->GetStr( STR::SONNET ); }

std::string Mesh::ShortInfo() const
{ return std::string( "\"" ) + fileName + "\""; }

std::string Mesh::DetailedInfo() const
{ return Description() + " \"" + fileName + "\" " + size.ToString( NPoint::SIZE ); }

Mesh::Mesh( ModelPtr _pModel ):
  IComponent(OT::MESH, _pModel ),
  sender_name( "Mesh" )
{ DG_PROPERTIES_INIT;
  DG_HIERARCHY_INIT; }

void Mesh::Delete()
{
  FOREACHPTRCONST( MeshElementPtr, pME, elements ) {
    pME->Mark( false );
    pME->DeleteContainers();
  }

  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    pMC->Mark( false );
    pMC->DeleteContainers();
  }

  SetFileName( "" );
  
  SetHeaderString( "" );

  Actions::ActDelMesh( pModel, this, DO_AT_ONCE );
}

int Mesh::ReadObjects( Token _token, const char* _cpStr,
                       FILE* _pFile, ulong& _rLine, int /*_mode*/ )
{
  char sToken[ 32 ];
  char sLine[ DG_LINE_LEN ];
  int id, id2;
  uint count;

  int errorFlags = 0;

  switch( _token ) {
  case TKN::MeshFingerprint112: {
    long fp;
    if( sscanf( _cpStr, "%s %lx", sToken, &fp ) == 2 )
      bMeshFingerprintOk = (fp == Fingerprint());
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::MeshPoint112: {
    int cell_eN, point_id;
    Point pos;
    if( sscanf( _cpStr, "%s %d %d "SCANFLT""SCANFLT,
                sToken, &cell_eN, &point_id, &pos.x, &pos.y ) == 5 ) {
      if( bMeshFingerprintOk ) {
        MeshCellPtr mc = FindMeshCellByNumber( cell_eN );
        if( mc != NULL )
          mc->MPoint( point_id )->Change( pos );
      }
      else
        errorFlags |= DGFE::MESH_FP;
    }
    else return DGFE::SYNTAX;
    break;
  }
  case TKN::MarkedMeshElements112:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d %d", &id, &id2 ) == 2 )
        {
          if( bMeshFingerprintOk )
          {
            MeshElementPtr pME = FindMeshElementById( id, id2 );
            if( pME != NULL )
              pME->Mark();
          }
          else
            errorFlags |= DGFE::MESH_FP;
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::MarkedMeshCells112:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d", &id ) == 1 )
        {
          if( bMeshFingerprintOk )
          {
            MeshCellPtr pMC = FindMeshCellByNumber( id );
            if( pMC != NULL )
              pMC->Mark();
          }
          else
            errorFlags |= DGFE::MESH_FP;
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::MeshFileHeader113:
    if( sscanf( _cpStr, "%s", sToken ) == 1 ) {
      std::string sText = Model::ReadText( _pFile, _rLine );
      SetHeaderString( sText );
    }
    else return DGFE::SYNTAX;

  default:
    errorFlags |= DGFE::SYNTAX;
    break;
  }

  return 0;
}

int Mesh::WriteObjects(Token _token, FILE* _pFile, int /*_mode*/ ) const
{
  const char* sToken = TKN::names[ _token ].s;

  switch( _token ) {
  case TKN::Mesh112:
    fprintf( _pFile, "%s %s\n", sToken, fileName.c_str() );
    break;
  case TKN::MeshFingerprint112:
    fprintf( _pFile, "%s %lx\n", sToken, fingerprint );
    break;
  case TKN::MeshPoint112: {
    FOREACHPTRCONST( MeshPointPtr, pMP, points ) {
      if( pMP->IsChanged() ) {
        MeshCellPtr pMC = null;
        for( int j = 0; j < 4; j++ )
          if( (pMC = pMP->Cell(j)) != NULL )
            break;
        if( pMC == NULL )
          continue;
        int j;
        for( j = 0; j < 4; j++ )
          if( pMC->MPoint(j) == pMP )
            break;
        assert( j < 4 );
        fprintf( _pFile, "%s %d %d %e %e\n", sToken, pMC->eNumber(), j, pMP->X(), pMP->Y() );
      }
    }
    break;
  }
  case TKN::MeshFileHeader113:
    if( alt != 0 && !headerString.empty() ) {
      fprintf( _pFile, "%s\n", sToken );
      WriteText( headerString, _pFile );
    }
    break;
  case TKN::MarkedMeshElements112: {
    IComponentList markedList_copy = pModel->MarkedObjects();
    RestrictToType( markedList_copy, OT::MESHELEMENT );
    if( !markedList_copy.empty() ) {
      fprintf( _pFile, "%s %d\n", sToken, (int)markedList_copy.size() );
      FOREACHPTRCONST( MeshElementPtr, pME, markedList_copy )
        fprintf( _pFile, "%d %d\n", pME->GetID_1(), pME->GetID_2() );
    }
    markedList_copy.clear();
    break;
  }
  case TKN::MarkedMeshCells112: {
    IComponentList markedList_copy = pModel->MarkedObjects();
    RestrictToType( markedList_copy, OT::MESHCELL );
    if( !markedList_copy.empty() ) {
      fprintf( _pFile, "%s %d\n", sToken, (int)markedList_copy.size() );
      FOREACHPTRCONST( MeshCellPtr, pMC, markedList_copy )
        fprintf( _pFile, "%d\n", pMC->eNumber() );
    }
    markedList_copy.clear();
    break;
  }
  default:
    break;
  }
  return 0;
}

void Mesh::SetFileName( const std::string& fName )
{
  Actions::ActChangeValue act( pModel, this, this, p_file_name, fName, NO_REDRAW, DO_AT_ONCE );
}

void Mesh::SetHeaderString( const std::string& hs )
{
  Actions::ActChangeValue act1( pModel, this, this, p_header_string, hs, NO_REDRAW, DO_AT_ONCE );
  SetAlt( alt+1 );
}

void Mesh::SetAlt( int _alt )
{
  Actions::ActChangeValue act( pModel, this, this, p_alt, _alt, NO_REDRAW, DO_AT_ONCE );
}

void Mesh::SetFingerprint( long _fingerprint )
{
  Actions::ActChangeValue act( pModel, this, this, p_fingerprint, _fingerprint, NO_REDRAW, DO_AT_ONCE );
}

int Mesh::CheckHeaderString( const std::string& hs )
{
  SENDER_NAME( "Mesh::CheckHeaderString" );
  bool bIDString = false;

  if( !hs.empty() && hs[ hs.length() - 1 ] != '\n' )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::MESH_HDR_NO_LF );

  std::vector< std::string > tokens;
  std::stringstream ss( hs );
  std::string token;
  while( std::getline( ss, token, '\n' ) )
    tokens.push_back( token );

  for( ulong i = 0; i < tokens.size(); i++ ) {
    if( tokens[i].compare( "   Element output:" ) == 0 && i < 10 )
      bIDString = true;
  }

  if( !bIDString )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::MESH_HDR_NO_ID );

  return 0;
}

int Mesh::RecalculateCenters()
{
  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    if( !pMC->IsChanged() )
      continue;

    int k = pMC->RecalculateCenter();
    if( k )
      return k;
  }

  return 0;
}


MeshCellPtr Mesh::FindMeshCellByNumber( int eN )
{
  if( eN >= cellCount )
    return null;

  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    if( pMC->eNumber() == eN )
      return pMC;
  }

  return NULL;
}

MeshElementPtr Mesh::FindMeshElementById( int id1, int id2)
{
  MeshCellPtr pMC = FindMeshCellByNumber( id1 );

  if( pMC == NULL )
    return NULL;

  return pMC->Element( id2 );
}

std::string Mesh::GetIrregularCellsList() const
{
  std::stringstream ss;
  bool first = true;
  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    if( pMC->IsIrregular() ) {
      if( first )
        first = false;
      else
        ss << ",  ";
      ss << pMC->eNumber() << " (" << pMC->Nx() << ", " << pMC->Ny() << ")";
    }
  }
  return ss.str();
}


int Mesh::SaveMesh( std::string fileName )
{
  SENDER_NAME( "Mesh::SaveMesh" );

  pModel->SendMessage( WND_PROCESS, SENDER,
                       DG3::SAVING_MESH_FILE, ARGS( fileName ) );

  int i = RecalculateCenters();
  if( i != 0 )
    return pModel->SendMessage( WND_ERROR, SENDER, i );

  pModel->SendProgress( 5 );

  assert( !fileName.empty() );

  FILE* file = fopen( fileName.c_str(), "wb" );
  if( file == null )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::FWRITE, ARGS( fileName ) );

  SetFileName( fileName );

  if( headerString.empty() ) {
    fprintf( file,
      "\n"
      "   Element output:\n"
      "\n"
      "   ========================================================================================\n"
    );
  }
  else
    fputs( headerString.c_str(), file );

  pModel->SendProgress( 10 );

  ulong fp = 0,  c_no = 0,  l_no = 0;
  char s[2048];
  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    sprintf( s,
             "   Element%5d = (%3d,%3d): (% 17.10E,% 17.10E)      (% 17.10E,% 17.10E)\n"
             "   Field ratio  = % 17.10E             (% 17.10E,% 17.10E)\n"
             "                             (% 17.10E,% 17.10E)      (% 17.10E,% 17.10E)\n",
             pMC->eNumber(), pMC->Nx(), pMC->Ny(),
             pMC->MPoint(2)->X() / SONNET_FACTOR, pMC->MPoint(2)->Y() / SONNET_FACTOR,
             pMC->MPoint(3)->X() / SONNET_FACTOR, pMC->MPoint(3)->Y() / SONNET_FACTOR,
             pMC->Ratio(),
             pMC->Center().x / SONNET_FACTOR, pMC->Center().y / SONNET_FACTOR,
             pMC->MPoint(0)->X() / SONNET_FACTOR, pMC->MPoint(0)->Y() / SONNET_FACTOR,
             pMC->MPoint(1)->X() / SONNET_FACTOR, pMC->MPoint(1)->Y() / SONNET_FACTOR
      );
    fputs( s, file );
    fputs("   ----------------------------------------------------------------------------------------\n", file );

    // Some instructions from hell:
    ulong fpl = 0;
    for( ulong k = 0; s[k] != 0; k++ ) {
      if( isgraph( s[k] ) )
        fpl = fpl ^ (((unsigned)s[k] & 0xff) * (1 + c_no++ %255));
    }
    fp = fp ^ fpl * (1 + l_no++ %32766);
  }

  pModel->SendProgress( 80 );

  fclose( file );

  SetFingerprint( fp );

  FOREACHPTRCONST( MeshPointPtr, pMP2, points )
    pMP2->SetBackupCoords( pMP2->Position() );

  FOREACHPTRCONST( MeshCellPtr, pMC2, cells )
    pMC2->SetBackups( pMC2->Center(), pMC2->Ratio() );

  SetAlt( 0 );

  pModel->SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return 0;
}

int Mesh::CreateTopology( MeshCellDataArray* pMCDs )
{
  SENDER_NAME( "Mesh::CreateTopology" );
  MeshCellDataArray pMCDs_2D;
  int eNm = 0;
  NPoint em;

  /* Find out mesh dimensions */
  FOREACH_CONST( MeshCellDataItemConst, itMCD, (*pMCDs) ) {
    em = Point( dg_max( em.x, (*itMCD)->index.x + 1 ), dg_max( em.y, (*itMCD)->index.y + 1 ) );
    eNm = dg_max( eNm, (*itMCD)->eN + 1 );
  }

  /* Verify mesh dimensions */
  if( em.x == 0 || em.y == 0 )
    return pModel->SendMessage( WND_ERROR, SENDER,
                                ERR::BADFILE, ARGS( fileName ) );

  /* Make sure every mesh cell number is unique */
  std::vector<int> iarray(eNm);

  for( int i = 0; i < eNm; i++ )
    iarray[i] = 0;

  FOREACH_CONST( MeshCellDataItemConst, itMCD, (*pMCDs) )
  {
    iarray[ (*itMCD)->eN ]++;
    if( iarray[ (*itMCD)->eN ] > 1 )
      return pModel->SendMessage( WND_ERROR, SENDER, ERR::DUP_MESH_ELNO );
  }
  iarray.clear(); // iarray won't be ever used


  /* Create a 2-D array of mesh cell infos */
  pMCDs_2D.resize( em.x * em.y );
  for( int i = 0; i < em.x * em.y; i++ )
    pMCDs_2D[i] = NULL;
  FOREACH_CONST( MeshCellDataItemConst, itMCD, (*pMCDs) )
    pMCDs_2D[ (*itMCD)->index.y * em.x + (*itMCD)->index.x ] = (*itMCD);

  /* Make sure every mesh cell in a rectangle exists */
  for( int i = 0; i < em.y; i++ )
    for( int j = 0; j < em.x; j++ )
    {
      MeshCellDataPtr pMCD = pMCDs_2D[ i * em.x + j ];
      if( pMCD == NULL )
      {
        FOREACH_CONST( MeshCellDataItemConst, itMCD, (*pMCDs) )
          delete *itMCD;
        delete pMCDs;
        return pModel->SendMessage( WND_ERROR, SENDER,
                                    ERR::BADFILE, ARGS( fileName ) );
      }
    }

  /* Detect neighboring cells */
  for( int i = 0; i < em.y; i++ )
    for( int j = 0; j < em.x; j++ ) {
      MeshCellDataPtr pMCD = pMCDs_2D[ i * em.x + j ];
      for( int k = MCN_START; k < MCN_END; k++ ) {
        pMCD->neighbours[k] = NULL;
        pMCD->cutFlags[k] = 0;
      }

      /* Above */
      if( i > 0 ) {
        MeshCellDataPtr pMCD_tmp = pMCDs_2D[ (i - 1) * em.x + j ];
        if( pMCD->p1 == pMCD_tmp->p3 && 
            pMCD->p2 == pMCD_tmp->p4 )
          pMCD->neighbours[ MCN_TOP ] = pMCD_tmp;
        else
          for( int k = 0; k < em.x * em.y; k++ ) {
            pMCD_tmp = pMCDs_2D[k];
            if( pMCD_tmp == pMCD )
              continue;
            if( pMCD->p1 == pMCD_tmp->p3 && 
                pMCD->p2 == pMCD_tmp->p4 ) {
              pMCD->neighbours[ MCN_TOP ] = pMCD_tmp;
              pMCD->cutFlags[ MCN_TOP ] = 1;
            }
          }
      }

      /* Below */
      if( i < em.y - 1 ) {
        MeshCellDataPtr pMCD_tmp = pMCDs_2D[ (i + 1) * em.x + j ];
        if( pMCD->p3 == pMCD_tmp->p1 && 
            pMCD->p4 == pMCD_tmp->p2 )
          pMCD->neighbours[ MCN_BOTTOM ] = pMCD_tmp;
        else
          for( int k = 0; k < em.x * em.y; k++ ) {
            pMCD_tmp = pMCDs_2D[k];
            if( pMCD_tmp == pMCD )
              continue;
            if( pMCD->p3 == pMCD_tmp->p1 &&
                pMCD->p4 == pMCD_tmp->p2 ) {
              pMCD->neighbours[ MCN_BOTTOM ] = pMCD_tmp;
              pMCD->cutFlags[ MCN_BOTTOM ] = 1;
            }
          }
      }

      /* Left */

      if( j > 0 ) {
        MeshCellDataPtr pMCD_tmp = pMCDs_2D[ i * em.x + j - 1];
        if( pMCD->p1 == pMCD_tmp->p2 &&
            pMCD->p3 == pMCD_tmp->p4 )
          pMCD->neighbours[ MCN_LEFT ] = pMCD_tmp;
        else 
          for( int k = 0; k < em.x * em.y; k++ ) {
            pMCD_tmp = pMCDs_2D[k];
            if( pMCD_tmp == pMCD )
              continue;
            if( pMCD->p1 == pMCD_tmp->p2 &&
                pMCD->p3 == pMCD_tmp->p4 ) {
              pMCD->neighbours[ MCN_LEFT ] = pMCD_tmp;
              pMCD->cutFlags[ MCN_LEFT ] = 1;
            }
          }
      }

      /* Right */

      if( j < em.x - 1 ) {
        MeshCellDataPtr pMCD_tmp = pMCDs_2D[ i * em.x + j + 1 ];
        if( pMCD->p2 == pMCD_tmp->p1 &&
            pMCD->p4 == pMCD_tmp->p3 )
          pMCD->neighbours[ MCN_RIGHT ] = pMCD_tmp;
        else 
          for( int k = 0; k < em.x * em.y; k++ ) {
            pMCD_tmp = pMCDs_2D[k];
            if( pMCD_tmp == pMCD )
              continue;
            if( pMCD->p2 == pMCD_tmp->p1 &&
                pMCD->p4 == pMCD_tmp->p3 ) {
              pMCD->neighbours[ MCN_RIGHT ] = pMCD_tmp;
              pMCD->cutFlags[ MCN_RIGHT ] = 1;
            }
          }
      }
  }


  /* Reset Mesh fields */
  locks = 0;
  alt = 0;
  fingerprint = 0;
  bComplete = 0;
  size = em;
  fileName = "";
  headerString = "";

  //TODO: need rezerve?
  cellCount = pointCount = elementCount = 0;

  /* Create mesh cells and points */
  for( int i = 0; i < em.y; i++ )
    for( int j = 0; j < em.x; j++ ) {

      MeshCellDataPtr pMCD = pMCDs_2D[ i * em.x + j ];

      MeshPointPtr pMP1 = null, pMP2 = null, pMP3 = null, pMP4 = null;

      /* Use points from already created neighboring cells if possible */
      if( pMCD->neighbours[MCN_TOP] != NULL && 
          pMCD->neighbours[MCN_TOP]->pMC != NULL ) {
        pMP1 = pMCD->neighbours[MCN_TOP]->pMC->MPoint(2);
        pMP2 = pMCD->neighbours[MCN_TOP]->pMC->MPoint(3);
      }

      if( pMCD->neighbours[MCN_BOTTOM] != NULL && 
          pMCD->neighbours[MCN_BOTTOM]->pMC != NULL ) {
        pMP3 = pMCD->neighbours[MCN_BOTTOM]->pMC->MPoint(0);
        pMP4 = pMCD->neighbours[MCN_BOTTOM]->pMC->MPoint(1);
      }

      if( pMCD->neighbours[MCN_LEFT] != NULL && 
          pMCD->neighbours[MCN_LEFT]->pMC != NULL) {
        pMP1 = pMCD->neighbours[MCN_LEFT]->pMC->MPoint(1);
        pMP3 = pMCD->neighbours[MCN_LEFT]->pMC->MPoint(3);
      }

      if( pMCD->neighbours[MCN_RIGHT] != NULL && 
          pMCD->neighbours[MCN_RIGHT]->pMC != NULL) {
        pMP2 = pMCD->neighbours[MCN_RIGHT]->pMC->MPoint(0);
        pMP4 = pMCD->neighbours[MCN_RIGHT]->pMC->MPoint(2);
      }

      /* Create the remaining points */
      if( pMP1 == NULL )
        pMP1 = AddMeshPoint( pMCD->p1 );
      if( pMP2 == NULL )
        pMP2 = AddMeshPoint( pMCD->p2 );
      if( pMP3 == NULL )
        pMP3 = AddMeshPoint( pMCD->p3 );
      if( pMP4 == NULL )
        pMP4 = AddMeshPoint( pMCD->p4 );

      /* Create the mesh cell */

      MeshCellPtr pMC = AddMeshCell( pMCD->eN, pMCD->index, pMP1, pMP2, pMP3, pMP4, pMCD->center, pMCD->ratio);
      pMCD->pMC = pMC;
      assert( pMC != NULL );
      pMP1->SetCell( pMC, 3 );
      pMP2->SetCell( pMC, 2 );
      pMP3->SetCell( pMC, 1 );
      pMP4->SetCell( pMC, 0 );

  }

  /* Set cut flags & neighbours, create mesh elements */
  for( int i = 0; i < em.y; i++ )
    for( int j = 0; j < em.x; j++ ) {

    MeshCellDataPtr pMCD = pMCDs_2D[ i * em.x + j ];
    MeshCellPtr pMC = pMCD->pMC;

    /* Set cut flags and neighbours */

    for( int k = MCN_START; k < MCN_END; k++ ) {
      pMC->SetCutFlag( pMCD->cutFlags[k], k);
      if( pMCD->neighbours[k] != NULL )
        pMC->SetNeighbor( pMCD->neighbours[k]->pMC, k );
    }

    /* Create mesh elements */

    if( pMC->Neighbour( MCN_TOP ) == NULL )
    {
      MeshElementPtr pME = AddMeshElement( pMC->MPoint(0), pMC->MPoint(1), false );
      pME->SetCutFlag( pMC->CutFlag( MCN_TOP ) );
      pME->MPoint(0)->SetElement( pME, MCN_RIGHT );
      pME->MPoint(1)->SetElement( pME, MCN_LEFT );
      pME->SetCell( pMC, 0 );
      pMC->SetElement( pME, MCN_TOP );
    }
    else
      pMC->SetElement( pMC->Neighbour( MCN_TOP )->Element( MCN_BOTTOM ), MCN_TOP );

    if( pMC->Neighbour( MCN_LEFT ) == NULL )
    {
      MeshElementPtr pME = AddMeshElement( pMC->MPoint(0), pMC->MPoint(2), true );
      pME->SetCutFlag( pMC->CutFlag( MCN_LEFT ) );
      pME->MPoint(0)->SetElement( pME, MCN_BOTTOM );
      pME->MPoint(1)->SetElement( pME, MCN_TOP );
      pME->SetCell( pMC, 0 );
      pMC->SetElement( pME, MCN_LEFT );
    }
    else
      pMC->SetElement( pMC->Neighbour( MCN_LEFT )->Element( MCN_RIGHT ), MCN_LEFT );

    MeshElementPtr pME = AddMeshElement( pMC->MPoint(1), pMC->MPoint(3), true );
    pME->SetCutFlag( pMC->CutFlag( MCN_RIGHT ) );
    pME->MPoint(0)->SetElement( pME, MCN_BOTTOM );
    pME->MPoint(1)->SetElement( pME, MCN_TOP );
    pME->SetCell( pMC, 0 );
    if( pMC->Neighbour( MCN_RIGHT ) != NULL )
    {
      pME->SetCell( pMC->Neighbour( MCN_RIGHT ), 1 );
      pMC->Neighbour( MCN_RIGHT )->SetElement( pME, MCN_LEFT );
    }
    pMC->SetElement( pME, MCN_RIGHT );

    pME = AddMeshElement( pMC->MPoint(2), pMC->MPoint(3), false );
    pME->SetCutFlag( pMC->CutFlag( MCN_BOTTOM ) );
    pME->MPoint(0)->SetElement( pME, MCN_RIGHT );
    pME->MPoint(1)->SetElement( pME, MCN_LEFT );
    pME->SetCell( pMC, 0 );
    if( pMC->Neighbour( MCN_BOTTOM ) != NULL )
    {
      pME->SetCell( pMC->Neighbour( MCN_BOTTOM ), 1);
      pMC->Neighbour( MCN_BOTTOM )->SetElement( pME, MCN_TOP );
    }
    pMC->SetElement( pME, MCN_BOTTOM );
  }

  /* Set cut flags for mesh points */
  FOREACHPTRCONST( MeshPointPtr, pMP, points ) {
    for( int j = MCN_START; j < MCN_END; j++ ) {
      MeshElementPtr pME = pMP->Element(j);
      if( pME == NULL )
        continue;
      if( pME->CutFlag() ) {
        pMP->SetCutFlag(1);
        break;
      }
    }
  }

  /* Set neighbours and numbers for mesh points */
  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    pMC->MPoint(0)->SetIndex( pMC->Index() );
    pMC->MPoint(1)->SetIndex( pMC->Index() + NPoint( 1, 0 ) );
    pMC->MPoint(2)->SetIndex( pMC->Index() + NPoint( 0, 1 ) );
    pMC->MPoint(3)->SetIndex( pMC->Index() + NPoint( 1, 1 ) );

    if( !pMC->MPoint(0)->CutFlag() ) {
      pMC->MPoint(0)->SetNeighbor( pMC->MPoint(2), MCN_BOTTOM );
      pMC->MPoint(0)->SetNeighbor( pMC->MPoint(1), MCN_RIGHT );
    }

    if( !pMC->MPoint(1)->CutFlag() ) {
      pMC->MPoint(1)->SetNeighbor( pMC->MPoint(3), MCN_BOTTOM );
      pMC->MPoint(1)->SetNeighbor( pMC->MPoint(0), MCN_LEFT );
    }

    if( !pMC->MPoint(2)->CutFlag() ) {
      pMC->MPoint(2)->SetNeighbor( pMC->MPoint(0), MCN_TOP );
      pMC->MPoint(2)->SetNeighbor( pMC->MPoint(3), MCN_RIGHT );
    }

    if( !pMC->MPoint(3)->CutFlag() ) {
      pMC->MPoint(3)->SetNeighbor( pMC->MPoint(1), MCN_TOP );
      pMC->MPoint(3)->SetNeighbor( pMC->MPoint(2), MCN_LEFT );
    }
  }

  bComplete = true;

  return DGMSG::OK;
}

MeshCellPtr Mesh::AddMeshCell( int eN, NPoint _index,
                            MeshPointPtr pt1, MeshPointPtr pt2, MeshPointPtr pt3, MeshPointPtr pt4,
                            Point _center, double ratio)
{
  assert( !bComplete );
  assert( cellCount < size.x * size.y );

  MeshCellPtr pMC =new MeshCell( pModel, this, eN, _index, pt1, pt2, pt3, pt4, _center, ratio );
  cells.push_back( pMC->GetPtr() );
  cellCount++;

  return pMC;
}

MeshElementPtr Mesh::AddMeshElement( MeshPointPtr pt1, MeshPointPtr pt2, bool bV )
{
  assert( !bComplete );
  assert( elementCount < size.x * size.y * 4 );

  MeshElementPtr pME = new MeshElement( pModel, this, pt1, pt2, bV );
  elements.push_back( pME->GetPtr() );
  elementCount++;

  return pME;
}

MeshPointPtr Mesh::AddMeshPoint( Point _position )
{
  assert( !bComplete );
  assert( pointCount < size.x * size.y * 4 );

  MeshPointPtr pMP = new MeshPoint( pModel, this, _position );
  points.push_back( pMP->GetPtr() );
  pointCount++;

  return pMP;
}

void Mesh::FreeVarContainings()
{
  FOREACHPTRCONST( MeshCellPtr, pMC, cells ) {
    assert( !pMC->HasContainers() );
    pMC->FreeContainers();
  }

  FOREACHPTRCONST( MeshElementPtr, pME, elements ) {
    assert( !pME->HasContainers() );
    pME->FreeContainers();
  }
}


void Mesh::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  FOREACHPTRCONST( MeshPointPtr, pMP, points )
    pMP->CalcExtens( _pMin, _pMax );
}

void Mesh::GetMinMaxPointX( double* pMin, double* pMax )
{
  double minX = MAXDOUBLE;
  double maxX = -MAXDOUBLE;
  FOREACHPTRCONST( MeshPointPtr, pMP, points ) {
    double r = pMP->X();
	  if( r < minX ) 
      minX = r;
	  if( r > maxX ) 
      maxX = r;
	}
  if( pMin != null )
    *pMin = minX;
  if( pMax != null )
    *pMax = maxX;
}


double Mesh::SlidingThreshold() const
{ return pModel->MeshSlidingThreshold(); }
void Mesh::SetSlidingThreshold( double _val )
{ pModel->SetMeshSlidingThreshold( _val ); }

bool Mesh::HasDoubleBorder() const
{ return pModel->HasDoubleMeshBorder(); }
void Mesh::SetDoubleBorderFlag( bool _flag )
{ pModel->SetDoubleMeshBorderFlag( _flag ); }

bool Mesh::CheckSlidingMode( ulong _mode ) const
{ return pModel->CheckMeshSlidingMode( _mode ); }
void Mesh::SetSlidingMode( ulong _mode )
{ pModel->SetMeshSlidingMode( _mode ); }
