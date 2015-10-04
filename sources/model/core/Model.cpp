#include "Model.h"

const std::string Model::topologyName_empty = "*";
const std::string Model::scsHide = "Hide";
const std::string Model::scsShow = "Show";

bool IComponent::IsHighlighted() { return pModel->HasHighlighted( this ); }
bool IComponent::IsMarked() { return pModel->HasMarked( this ); }
void IComponent::Mark( bool _enable ) { pModel->MarkObject( this, _enable ); }
void IComponent::Highlight( bool _enable ) { pModel->Highlight( this, _enable ); }

Model::Model(IModelAgentPtr _pAgent, const std::string& _crsName ):
  pAgent( _pAgent ),
  title_name( _crsName ),

  chFlags( 0 ),
  maxElemId( 0 ),
  outputMode( OUTPUTMODE::SONNET ),
  updateLocks( 0 ),

  errorCode( 0 ),
  bStrict( false ), //TODO: check default

  pMesh( NULL ),
  pTemplate( null ),
  pSonnetData( null ),

  creationTime( "" ),
  fileName( "" ),
  topologyName( "" ),

  actStack( this ),

  meshSlidingMode( MSM::SURFACE ),
  meshSlidingThreshold( 1. ),
  doubleMeshBorder( true ),
  outputFlags( 0 ),
  min_position( 0. ),
  max_position( 0. ),
  xyAngle( 0. ),
  showFlags( SHW::INIT2 ),
  szRestriction( SZR::B2EIRENE )
{
  DG_PROPERTIES_INIT;
  sender_name = "Model-";
  sender_name += title_name;

  pVars = new VarsManager( this );
  pFlux = new FluxModel( this );
  pFlux->AddTopology();

  DG_HIERARCHY_INIT;
}

Model::~Model()
{
  pAgent->Clear();
  Release();
  delete pVars;
  delete pFlux;
}

int Model::Load(const std::string &_fName, int* pErrFlags )
{
  SENDER_NAME( "Load" );
  SendMessage( WND_PROCESS, SENDER, DG3::LOADING_DG_FILE, ARGS( _fName ) );
  int errFlags = 0;
  if( pErrFlags == null )
    pErrFlags = &errFlags;
  int err = 0;

  switch( DetectFileType( _fName ) )
  {
    case FT_NOTFOUND:
      err = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FILENOTFOUND, ARGS( _fName ) );
      break;

    case FT_DG_APP:
    case FT_DG_CONFIG:
    case FT_DG_TMP:
      {
        FILE* f = fopen( _fName.c_str(), "rb" );
        if( f == NULL ) {
          err = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::FILENOTFOUND, ARGS( _fName ) );
          break;
        }

        SetFileName( _fName );

        actStack.DisableUndo();
        err = ReadApp_File( f, _fName, pErrFlags );
        actStack.EnableUndo();
        fclose( f );

        if( err != 0 )
          Release();
        break;
      }

    default:
      //TODO: old format
      //*err=ReadOldDgFile(a,fName,pErrFlags);
      if( err != 0 )
        Release();
      else
        SetFileName( _fName );
      break;
  }

  if( err == 0 )
    SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return err;
}

int Model::Release()
{
  actStack.FreeUndoInfo();

  fileName = "";
  creationTime = "";
  topologyName = "";

  if( pMesh != null ) delete pMesh;
  if( pTemplate != null ) delete pTemplate;
  if( pSonnetData != null ) delete pSonnetData;

  IComponentPtr pObj = null;
  FOREACHOBJCONST( pObj, nodes )       delete pObj;
  nodes.clear();
  FOREACHOBJCONST( pObj, elements )    delete pObj;
  elements.clear();
  FOREACHOBJCONST( pObj, separators )  delete pObj;
  separators.clear();
  FOREACHOBJCONST( pObj, sources )     delete pObj;
  sources.clear();
  FOREACHOBJCONST( pObj, chords )      delete pObj;
  chords.clear();
  FOREACHOBJCONST( pObj, comments )    delete pObj;
  comments.clear();

  pVars->Release();
  pFlux->Release();

  return 0;
}

int Model::LoadMesh( const std::string& _fName )
{
  SENDER_NAME( "LoadMesh" );
  long fp; // fingerprint

  int fileType = DetectFileType( _fName );
  int err;
  
  MeshCellDataArray mcds; // mcdG
  std::vector< std::string > strs; // cmtG

  switch( fileType )
  {
  case FT_SONNET:
    err = LoadSonnetMeshFile( _fName, &mcds, &strs, &fp );
    break;
  case FT_NOTFOUND: err = ERR::FILENOTFOUND; break;
  default:          err = ERR::BADFILETYPE; break;
  }

  if( err != 0 ) {
    SendMessage( LOG_WIP_WARN, SENDER, err, ARGS( _fName ) );
    return err;
  }

  MeshPtr pMesh_tmp = null;

  if( err == 0 ) {
    if( pMesh != NULL )
      pMesh->Delete();
    assert( pMesh == NULL );

    pMesh_tmp = new Mesh( this);
    err = pMesh_tmp->CreateTopology( &mcds );
  }

  SendProgress( 85 );

  if( err == 0 ) {    
    pMesh_tmp->SetFileFormat( 0 );

    Actions::ActAddMesh act( this, pMesh_tmp );
    err = act.Do();
  }
  else
    delete pMesh_tmp;

  SendProgress( 90 );

  if( err == 0 && !strs.empty() ) {
    std::string header;
    FOREACH_CONST( std::vector< std::string >::const_iterator, itS, strs )
      header.append( *itS );

    pMesh->SetHeaderString( header );
    pMesh->SetAlt( 0 );
  }

  SendProgress( 95 );

  if( err != 0 && pMesh != NULL )
    ReleaseObject( (IComponentPtr*)&pMesh );

  if( err == 0 ) {
    pMesh->SetFileName( _fName );
    pMesh->SetFingerprint( fp );
  }

  if( err != 0 )
    SendMessage( MT::WINDOW, AL::ERROR, SENDER, err );

  DG_HIERARCHY_INIT;

  return err;
}


void Model::ProcessChanges()
{
  if( HasAnyFlag( chFlags, CHF::TARGETS|CHF::TOPOLOGY ) )
    pFlux->ProcessChanges( chFlags );

  chFlags = 0;
}


void Model::SetOutputFlags( ulong flags )
{
  ActChangeValue( this, null, this, p_output_flags, flags, REDRAW, DO_AT_ONCE );
}

void Model::SetMeshSlidingMode( ulong mode )
{
  ActChangeValue( this, null, this, p_mesh_sliding_mode, mode, REDRAW, DO_AT_ONCE );
}

void Model::SetMeshSlidingThreshold( double value )
{
  ActChangeValue( this, null, this, p_mesh_sliding_threshold, value, REDRAW, DO_AT_ONCE );
}

void Model::SetDoubleMeshBorderFlag( bool flag )
{
  ActChangeValue( this, null, this, p_double_mesh_border, flag, REDRAW, DO_AT_ONCE );
}

void Model::SetTopologyName( const std::string& _crsName )
{
  ActChangeValue( this, null, this, p_topology_name, _crsName, REDRAW, DO_AT_ONCE );
}

void Model::SetFileName( const std::string& _crsName )
{
  //if( !name.empty() )
  //  name = ExpandFilename( name ); //TODO: obsolete

  if( _crsName == fileName ) return;
  ActChangeValue( this, null, this, p_file_name, _crsName, NO_REDRAW, DO_AT_ONCE );
}

void Model::SetCreationTime( const std::string& _crsCreaTime )
{
  if( _crsCreaTime == creationTime ) return;
  ActChangeValue( this, null, this, p_creation_time, _crsCreaTime, NO_REDRAW, DO_AT_ONCE );
}

void Model::SetOutputMode( ulong newMode )
{
  switch( newMode ) {
  case OUTPUTMODE::SONNET:
  case OUTPUTMODE::CARRE:
    break;
  default:
    assert( false );
  }
  ActChangeValue( this, null, this, p_output_mode, newMode, NO_REDRAW, DO_AT_ONCE );
}

void Model::SetSZRestriction( ulong _restriction )
{
  ActChangeValue( this, null, this, p_sz_restriction, _restriction, NO_REDRAW, DO_AT_ONCE );
}

bool Model::IsUnsaved() const
{
  return actStack.HasUndoRecords();
}

ulong Model::SendMessage( MessageType _mt, AlarmLevel _al, const std::string& _sender, ulong msg_code, const std::StringList& args ) const
{
  std::string sender = sender_name;
  sender += "/";
  sender += _sender;
  return pAgent->send( _mt, _al, sender, msg_code, args );
}

void Model::SendMessage(MessageType _mt, AlarmLevel _al, const std::string& _sender, const std::string& _message ) const
{
  std::string sender = sender_name;
  sender += "/";
  sender += _sender;
  pAgent->send( _mt, _al, sender, _message );
}
