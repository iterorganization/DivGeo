#include "XPointSeg.h"
#include "../core/Model.h"

XPointSeg::XPointSeg( ModelPtr _pModel, XPointTestPtr _pXPT, int _number, double _sp, double _ep ):
  IComponent( OT::XPOINTSEG, _pModel ),
  pXPT( DG_SHARE( _pXPT ) ),
  number( _number ),
  startPos( _sp ),
  endPos( _ep ),
  lineLength( 0 )
{}

XPointSeg::~XPointSeg()
{
  Lock();
  ReleaseObject( (IComponentPtr*)&pXPT );
  Unlock();
}

std::string XPointSeg::Description() const
{ return pModel->GetStr( STR::XPOINTSEG ); }

std::string XPointSeg::ShortInfo() const
{ return ToStdString( number ); }

std::string XPointSeg::DetailedInfo() const
{ return Description() << " [" << number << "] (" << startPos << " -> " << endPos << ")"; }

double XPointSeg::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{
  UNUSED( _pPosFlag );
  double distHit = MAXDOUBLE;
  PointIterConst itXY_second = line.begin() + 1;
  FOREACH_CONST( PointIterConst, itXY_2, line ) {
    if( itXY_2 == line.begin() )
      continue;
    Point xy1 = *(itXY_2 - 1);
    Point xy2 = *itXY_2;

    double dist = Point2VectorDist( xy1, xy2, _crPnt, NULL, NULL );
    if( itXY_2 == itXY_second || dist < distHit )
      distHit = dist;
  }
  return distHit;
}

double XPointSeg::GetXptLevel() const
{
  return pXPT->Level(); 
}

void XPointSeg::Register()
{
  pXPT->IncludeXPointSeg( this );
}

void XPointSeg::ExcludeFromXPT()
{
  pXPT->ExcludeXPointSeg( this );
}

int XPointSeg::CalcLine()
{
  SENDER_NAME( "XPointSeg::CalcLine" );
  /* Calculate the full line */

  if( !pModel->GetFluxModel()->HasEquil() )
    return pModel->SendMessage( LOG_ERROR, SENDER, ERR::NOEQUIL );

  //PointArray* pLine = new PointArray();
  line.clear();
  pModel->GetFluxModel()->GetTopology()->CalcSeparatrixLine( pXPT, &line, number );

  if( line.empty() )
    return pModel->SendMessage( LOG_WARNING, SENDER, ERR::BADXPOINTSEG );

  /* Cut the line if needed */
  if( endPos > 0)
    CutPolyLine( &line, endPos, 0 );
  if( startPos > 0)
    CutPolyLine( &line, startPos, 1 );

  /* Update the XPointSeg structure and display the result */

  //if( !line.empty() )
  //  line.clear();
  //line = *pLine;
  lineLength = CalcLineLength( line );

#ifdef QT_DEBUG
  /*std::stringstream ss;
  ss << "XPS:" << number << ", line:" <<  line.size() << "/" << lineLength << "\n";
  FOREACH_CONST( PointArray::const_iterator, it, line )
    ss << it->ToString() << "  ";
  pModel->SendMessage( LOG_INFO, SENDER, ss.str() );*/
#endif

  return 0;
}

void XPointSeg::Delete()
{
  pModel->GetFluxModel()->GetTopology()->DeleteXPointSeg( this );
}

double XPointSeg::CalcAngle( XPointTestPtr _pXPT ) const
{
  if( _pXPT == null )
    _pXPT = DG_SHARE( pXPT );

  PointArray* pPoints = new PointArray();
  pModel->GetFluxModel()->GetTopology()->CalcSeparatrixLine( _pXPT, pPoints, number );
  if( pPoints == NULL )
    return XPS_ANGLE_BAD;

  Point xy = pPoints->front();
  Point xy1 = pPoints->back();

  /* Make the line point away from xpt */

  if( hypot( xy1.x - _pXPT->Center().x, xy1.y - _pXPT->Center().y ) <
      hypot( xy.x - _pXPT->Center().x, xy.y - _pXPT->Center().y ) )
    std::reverse( pPoints->begin(), pPoints->end() );

  double minD = fabs( value_y_delta * XPS_ANGLE_FRACTION );
  minD = dg_max( minD, average_cell_size * XPS_ANGLE_CELLS_MIN );

  Point d;
  Point xy0 = pPoints->front();
  PointIter itXY, itXYEnd = pPoints->end();
  for( itXY = pPoints->begin() + 1; itXY != itXYEnd; ++itXY ) {
    d = *itXY - xy0;
    if( hypot( d.x, d.y ) > minD )
      break;
  }

  if( itXY == pPoints->end() ) {
    delete pPoints;
    return XPS_ANGLE_BAD;
  }

  double angle;
  if( d.x == 0 )
    angle = d.y < 0 ? -M_PI_2 : M_PI_2;
  else
    angle = atan( d.y / d.x );

  if( d.x < 0 )
    angle += M_PI;

  delete pPoints;
  return angle / M_PI * 180;
}
