#include "SonnetData.h"
#include "../core/Model.h"

std::string SonnetData::Description() const
{ return pModel->GetStr( STR::SONNET ); }

std::string SonnetData::ShortInfo() const
{ return std::string( "\"" ) + fName + "\""; }

std::string SonnetData::DetailedInfo() const
{ return Description() + " \"" + fName + "\" " + size.ToString( NPoint::SIZE ); }

int SonnetData::LoadSonnetFile()
{
  SENDER_NAME( "SonnetData::LoadSonnetFile" );
  Point p1, p2 /*, p3, p4*/;
  char s[DG_FNAME_LEN*2];

  FILE* file = fopen( fName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::FILENOTFOUND, ARGS( fName ) );

  points.clear();
  points.resize( 1 );
  int eN = 0, i = 0, j = 0;
  NPoint em;
  unsigned pos = 0;

  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    if( sscanf( s, "   Element    %d = (  %d,  %d): ( "SCANFLT","SCANFLT
        ")      ( "SCANFLT","SCANFLT")", &eN, &i, &j, &p1.x, &p1.y, &p2.x, &p2.y ) != 7 )
      continue;

    em.x = dg_max( em.x, i );
    em.y = dg_max( em.y, j );
    fgets( s, sizeof( s ) - 1, file );
    fgets( s, sizeof( s ) - 1, file );
    if( pos + 4 >= points.size() )
      points.resize( pos + 100 );
    points[pos++] = p1; 
    points[pos++] = p2;

    if( sscanf( s, "                             ( "SCANFLT
        ","SCANFLT")      ( "SCANFLT","SCANFLT, &p1.x, &p1.y, &p2.x, &p2.y ) != 4 ) {
      points.clear();
      return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADFILE, ARGS( fName ) );
    }

    points[pos++] = p1; 
    points[pos++] = p2;
  }

  if( em != NPoint() ) {
    points.clear();
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::BADFILE, ARGS( fName ) );
  }

  for( unsigned i = 0; i < pos; i++ ) {
    points[i] *= SONNET_FACTOR;
  }

  size = em + 1;

  points.resize( size.x * size.y * 4 );

  return 0;
}

void SonnetData::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMax = *_pMin = Point( points[0].x, points[0].y );

  for( int i = size.x*size.y*4-1; i > 0; i-- ) {
    Point xy = points[i];
    *_pMin = Point( dg_min( _pMin->x, xy.x ), dg_min( _pMin->y, xy.y ) );
    *_pMax = Point( dg_max( _pMax->x, xy.x ), dg_max( _pMax->y, xy.y ) );
  }
}
