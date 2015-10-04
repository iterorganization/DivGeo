#include "Template.h"
#include "../core/Model.h"

Template::Template( ModelPtr _pModel, const std::string& _fName, const TemplateParams &_tp ):
  IComponent( OT::TEMPLATE, _pModel ),
  sender_name( "Template" ),
  fileName( _fName ),
  incr( _tp.incr ),
  angle( _tp.angle ),
  scale( _tp.scale ),
  dragStatus( 0 ),
  drag( 0 )
{ DG_PROPERTIES_INIT; }

std::string Template::Description() const
{ return pModel->GetStr( STR::TEMPLATE ); }

std::string Template::ShortInfo() const
{ return std::string( "\"" ) + fileName + "\""; }

std::string Template::DetailedInfo() const
{ return Description() + " \"" + fileName + "\" " +
      incr.ToString() + " x" + scale + ", " + angle*RAD2DEG + "deg"; }

void Template::SetDragStatus( int _dragStatus, Point _drag )
{
  Actions::ActChangeValue act1( pModel, this, this, p_drag_status, _dragStatus, NO_REDRAW, DO_AT_ONCE );
  Actions::ActChangeValue act2( pModel, this, this, p_drag_value, _drag, NO_REDRAW, DO_AT_ONCE );
}

void Template::Change( const TemplateParams& _tp )
{
  ValidatePtr( pModel, "ChangeTemplate" );

  ActChangeTemplate act( pModel, this, _tp, DO_AT_ONCE );
}

void Template::Delete()
{
  ValidatePtr( pModel, "DelTemplate" );

  ActDelTemplate act( pModel, this, DO_AT_ONCE );
}


int Template::LoadOgrTemplateFile()
{
  SENDER_NAME( "LoadOgrTemplateFile" );
  OgrNodePtr pOgrNode;
  OgrNodeList ogrNodes;

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );

  pModel->SendProgress( 10 );

  char s[500];
  bool err = false;
  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    int it = 0;
    if( sscanf( s, "%d", &it ) != 1 ) {
      err = true;
      break;
    }
      
    if( it == 99 ) // ENDFILE
      break;

    int id = 0, iv = 0, kc = 0;
    if( sscanf( s, "%d%d%d%d", &it, &id, &iv, &kc ) != 4 ) {
      err = true;
      break;
    }

    switch( it ) {
    case 1: //NODEDATA:
      {
        fgets( s, sizeof( s ) - 1, file );
        kc--;
        double x = 0., y = 0.;
        if( sscanf( s, SCANFLT""SCANFLT, &x, &y ) != 2 ) {
          err = true;
          break;
        }
        pOgrNode = new OgrNode( id, x, y );
        ogrNodes.push_back( pOgrNode );
        break;
      }
    case 2: //ELEMDATA:
      {
        fgets( s, sizeof( s ) - 1, file );
        kc--;
        int n1 = 0;
        if( sscanf( s, "%d", &n1 ) != 1 && n1 != 2 ) {
          err = true;
          break;
        }

        fgets( s, sizeof( s ) - 1, file );
        kc--;
        int n2 = 0;
        if( sscanf( s, "%d%d", &n1, &n2 ) != 2 ) {
          err = true;
          break;
        }

        OgrNodePtr pON = null;
        FOREACH_CONST( OgrNodeIter, itON, ogrNodes )
          if( (*itON)->id == n1 ) {
            pON = *itON;
            break;
          }
          
        if( pON == NULL) {
          err = true;
          break;
        }

        points.push_back( Point( pON->x, pON->y ) );

        FOREACH_CONST( OgrNodeIter, itON, ogrNodes )
          if( (*itON)->id == n2 ) {
            pON = *itON;
            break;
          }

        if( pON == NULL) {
          err = true;
          break;
        }

        points.push_back( Point( pON->x, pON->y ) );
        break;
      }
    default:
      break;
    }
    if( kc < 0 ) {
      err = true;
      break;
    }
    while( kc-- )
      fgets( s, sizeof( s ) - 1, file );
  }

  pModel->SendProgress( 90 );

  FOREACH_CONST( OgrNodeIter, itON, ogrNodes )
    if( *itON != null )
      delete *itON;
  fclose( file );

  if( err ) {
    points.clear();
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  return 0;
}


int Template::LoadDgTemplateFile()
{
  SENDER_NAME( "LoadDgTemplateFile" );

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );

  char s[500];
  fgets( s, sizeof( s ) - 1, file );

  pModel->SendProgress( 10 );

  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    double x = 0, y = 0;
    if( sscanf( s, SCANFLT""SCANFLT, &x, &y ) != 2 ) {
      points.clear();
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }
    points.push_back( Point( x, y ) );
  }
  fclose( file );

  if( (points.size() & 1) == 1 ) { // check for oddness
    points.clear();
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );
  }

  return 0;
}

int Template::Load2ColumnsTemplateFile()
{
  SENDER_NAME( "Template::Load2ColumnsTemplateFile" );

  char s[500], s1[500];

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );

  pModel->SendProgress( 10 );

  bool bOld = false;
  double xOld = 0, yOld = 0;
  while( fgets( s, sizeof( s ) - 1, file ) != NULL ) {
    double x = 0, y = 0;
    if( sscanf( s, SCANFLT""SCANFLT, &x, &y ) != 2 ) {
      if( sscanf( s, "%s", s1 ) != 1 ) {
        bOld = false;
        continue;
      }
      points.clear();
      fclose( file );
      return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                  ERR::BADFILE, ARGS( fileName ) );
    }

    if( bOld ) {
      points.push_back( Point( xOld, yOld ) );
      points.push_back( Point( x, y ) );
    }
    xOld = x;
    yOld = y;
    bOld = true;
  }

  fclose( file );

  assert( (points.size() & 1) != 1 );

  return 0;
}


int Template::LoadHpglTemplateFile()
{
  SENDER_NAME( "Template::LoadHpglTemplateFile" );

  FILE* file = fopen( fileName.c_str(), "rb" );
  if( file == NULL )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::FILENOTFOUND, ARGS( fileName ) );

  pModel->SendProgress( 10 );

  double xOld = 0, yOld = 0;

  char* s;
  while( ( s = ReadHpglCommand( file ) ) != NULL ) {
    double x = 0, y = 0;
    if( sscanf( s, "pu"SCANFLT","SCANFLT, &x, &y ) == 2 ) {
      xOld = x;
      yOld = y;
    } 
    else if( sscanf( s, "pd"SCANFLT","SCANFLT, &x, &y ) == 2 ) {
      points.push_back( Point( xOld, yOld ) );
      points.push_back( Point( x, y ) );
      xOld = x;
      yOld = y;
    }
  }

  fclose( file );

  if( points.size() == 0 )
    return pModel->SendMessage( WND_ERROR, FULL_SENDER,
                                ERR::BADFILE, ARGS( fileName ) );

  assert( (points.size() & 1) != 1 );

  return 0;
}

Point Template::Points_Next( bool bTransformed )
{
  Point pnt = *itPoint_current;
  itPoint_current++;
  return bTransformed ? TransformPoint( pnt ) : pnt;
}

Point Template::TransformPoint( Point _pnt ) const
{
  return Point( (_pnt.x * cos( angle ) - _pnt.y * sin( angle )) * scale + incr.x,
                (_pnt.x * sin( angle ) + _pnt.y * cos( angle )) * scale + incr.y );

  /* ... some conflicting code from 2.1 */
}

void Template::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  Point xy = points.front();
  if( _pMin->x > _pMax->x )
    *_pMax = *_pMin = Point( xy.x, xy.y );

  for( unsigned i = 1; i < points.size(); i++ ) {
    Point xy1 = TransformPoint( points[i] );
    *_pMin = Point( dg_min( _pMin->x, xy1.x ), dg_min( _pMin->y, xy1.y ) );
    *_pMax = Point( dg_max( _pMax->x, xy1.x ), dg_max( _pMax->y, xy1.y ) );
  }
}

int Template::GetNearestPoint( const Point& pos, Point* pt, Point* ps ) const
{
  double distMin = MAXDOUBLE;
  Point pntMin;

  for( size_t i = 0; i < points.size(); i++ ) {
    Point pnt_dist = TransformPoint( points[i] ) - pos;
    double dist = hypot( pnt_dist.x, pnt_dist.y );
    if( dist < distMin ) {
      pntMin = points[i];
      distMin = dist;
    }
  }

  if( pntMin == 0 )
    return -1;
  if( pt != null )
    *pt = pntMin;
  if( ps != null )
    *ps = TransformPoint( pntMin );

  return 0;
}


int Template::PlaceByHandles( const Point& pa, const Point& p1, const Point& pb, const Point& p2 )
{
  Template fakeT( pModel, "fake", TemplateParams( Point(), 0., 1. ) );

  if( pa != pb || p1 != p2 ) { /* Two handles */
    double hab = hypot( pa.x - pb.x, pa.y - pb.y );
    double h12 = hypot( p1.x - p2.x, p1.y - p2.y );

    if( hab < MINDOUBLE )
      return ERR::REPOS_TEMPLATE_1;
    if( h12 < MINDOUBLE )
      return ERR::REPOS_TEMPLATE_2;

    fakeT.scale = h12 / hab;

    double angle1 = acos( (pb.x - pa.x) / hab );
    if( pb.y - pa.y < 0 )
      angle1 = -angle1;

    double angle2 = acos( (p2.x - p1.x) / h12 );
    if( p2.y - p1.y < 0 )
      angle2 = -angle2;

    fakeT.angle = angle2 - angle1;
  }
  else {
    fakeT.angle = angle;
    fakeT.scale = scale;
  }

  fakeT.incr = 0.;
  Point xy = pa;

  Change( TemplateParams( p1 - fakeT.TransformPoint( xy ), fakeT.angle, fakeT.scale ) );

  return 0;
}
