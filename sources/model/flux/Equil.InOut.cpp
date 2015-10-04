#include "Equil.h"
#include "../core/Model.h"

int Equil::Load()
{
  SENDER_NAME( "Load" );
  int result = 0;
  switch( DetectFileType( fileName ) ) {
  case FT_DG_EQUIL: return LoadDgFile();
  case FT_1_EQUIL:  return Load1File();
  case FT_NOTFOUND: result = ERR::FILENOTFOUND; break;
  default:          result = ERR::BADFILETYPE; break;
  }
  return pModel->SendMessage( LOG_WIP_WARN, FULL_SENDER, result, ARGS( fileName ) );
}

int Equil::LoadDgFile()
{
  SENDER_NAME( "LoadDgFile" );

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );

  char s[256];
  fgets( s, sizeof(s), file );
  if( fscanf( file, "Width=%d\nHeight=%d", &size.x, &size.y ) != 2 || size.x == 0 || size.y == 0 ) {
    fclose( file );
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  pModel->SendProgress( 10 );

  x = new double[size.x];
  y = new double[size.y];
  val = new double[size.x*size.y];

  for( int i = 0; i < size.x; i++ )
    if( fscanf( file, SCANFLT, &x[i] ) != 1 ) {
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }

  pModel->SendProgress( 40 );

  for( int i = 0; i < size.y; i++ )
    if( fscanf( file, SCANFLT, &y[i] ) != 1 ) {
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }

  pModel->SendProgress( 70 );

  for( int i = 0; i < size.x*size.y; i++ )
    if( fscanf( file, SCANFLT, &val[i] ) != 1 ) {
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }

  fclose( file );
  return 0;
}

int Equil::Load1File()
{
  SENDER_NAME( "Load1File" );

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );
  size = 0;

  pModel->SendProgress( 5 );

  char c;
  char s[256];
  double psib;
  bool scanRows = false;
  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    sscanf( s, "     jm    =  %d", &size.x);
    sscanf( s, "     km    =  %d", &size.y);
    sscanf( s, "     psib  = "SCANFLT, &psib);
    if( sscanf( s, "     r(1:jm)%c", &c ) == 1 ) {
      scanRows = true;
      break;
    }
  }
  if( !scanRows || size.x == 0 || size.y == 0 ) {
    fclose( file );
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  pModel->SendProgress( 25 );

  x = new double[size.x];
  y = new double[size.y];
  val = new double[size.x*size.y];

  for( int i = 0; i < size.x; i++ ) {
    if( fscanf( file, SCANFLT, &x[i] ) != 1 ) {
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }
    x[i] *= 1e3;
  }

  pModel->SendProgress( 40 );

  bool scanCols = false;
  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    if( sscanf( s, "     z(1:km)%c", &c ) == 1 ) {
      scanCols = true;
      break;
    }
  }
  if( !scanCols ) {
    fclose( file );
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  pModel->SendProgress( 55 );

  for( int i = 0; i < size.y; i++ ) {
    if( fscanf( file, SCANFLT, &y[i] ) != 1 ) {
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }
    y[i] *= 1e3;
  }

  pModel->SendProgress( 70 );

  bool scanMain = false;
  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    if( sscanf( s, "     ((psi(j,k)-psib,j=1,jm),k=1,km%c", &c ) == 1 ) {
      scanMain = true;
      break;
    }
  }
  if( !scanMain ) {
    fclose( file );
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  pModel->SendProgress( 85 );

  for( int i = 0; i < size.x*size.y; i++ )
    if( fscanf( file, SCANFLT, &val[i] ) != 1 ){
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }

  fclose( file );
  return 0;
}


int Equil::WriteDg( const std::string& _fName, NPoint _size ) const
{
  SENDER_NAME( "WriteDg" );
  pModel->SendMessage( WND_PROCESS, FULL_SENDER,
                       DG3::WRITING_EQUIL_FILE, ARGS( _fName ) );

  FILE* file = fopen( _fName.c_str(), "wb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FWRITE, ARGS( _fName ) );

  pModel->SendProgress( 5 );

  fprintf( file, "DivGeo equilibrium file>>\nWidth=%d\nHeight=%d\n", _size.x, _size.y );
  for( int i = 0; i < _size.x; i++ )
    fprintf( file, "%e\n", x[i * size.x/_size.x] );
  fprintf( file, "\n" );

  pModel->SendProgress( 35 );

  for( int i = 0; i < _size.y; i++ )
    fprintf( file, "%e\n", y[i * size.y/_size.y] );
  fprintf( file, "\n" );

  pModel->SendProgress( 70 );

  for( int i = 0; i < _size.y; i++ )
    for( int j = 0; j < _size.x; j++ )
      fprintf( file, "%e\n", Cell( NPoint( j * size.x/_size.x, i * size.y/_size.y ) ) );

  fclose( file );
  pModel->SendMessage( LOG_RESULT, FULL_SENDER, DG3::DONE );
  return 0;
}

int Equil::WriteSpline( const std::string& _fName, NPoint _size ) const
{
  SENDER_NAME( "WriteSpline" );
  pModel->SendMessage( WND_PROCESS, FULL_SENDER,
                       DG3::WRITING_SPLINE_FILE, ARGS( _fName ) );

  /*if( pSurface == NULL )
    CalcSplines();*/ // TODO: MB: must not be in this const method

  assert( pSurface != NULL );

  SurfaceSplinePtr pSS = pSurface;

  /* si=CreateSurfaceSplineInfo(eq->sx,eq->sy,eq->x,eq->y,eq->val);
  assert(si!=NULL); */

  FILE* file = fopen( _fName.c_str(), "wb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FWRITE, ARGS( _fName ) );

  pModel->SendProgress( 5 );

  fprintf( file, "DivGeo equilibrium file>>\nWidth=%d\nHeight=%d\n", _size.x, _size.y );
  for( int i = 0; i < _size.x; i++ )
    fprintf( file, "%e\n", x[0] + (x[size.x-1] - x[0]) * i / _size.x);

  pModel->SendProgress( 30 );

  fprintf( file, "\n" );
  for( int i = 0; i < _size.y; i++ )
    fprintf( file, "%e\n", y[0] + (y[size.y-1] - y[0]) * i / _size.y);
  fprintf( file, "\n" );

  pModel->SendProgress( 60 );

  double z = 0.;
  for( int i = 0; i < _size.y; i++ )
    for( int j = 0; j < _size.x; j++ ) {
      assert( pSS->CalcValue(
      x[0] + (x[size.x-1] - x[0]) * j / _size.x,
      y[0] + (y[size.y-1] - y[0]) * i / _size.y,
      &z, NULL, NULL ) == 0 );
    fprintf( file, "%e\n", z );
  }

  /* FreeSurfaceSplineInfo(si); */
  fclose( file );

  pModel->SendMessage( LOG_RESULT, FULL_SENDER, DG3::DONE );
  return 0;
}

