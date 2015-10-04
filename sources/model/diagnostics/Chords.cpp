#include "Chords.h"
#include "../core/Model.h"

Chord::Chord( ModelPtr _pModel, Point _p1, Point _p2, bool _bActiveTopView ):
  IComponent( OT::CHORD, _pModel )
{
  if( _bActiveTopView ) {
    p1.x = _p1.x;
    p2.x = _p2.x;
    p1.y = p2.y = 0;
    p1.z = _p1.y;
    p2.z = _p2.y;
  } 
  else {
    p1 = _p1;
    p2 = _p2;
  }
}

std::string Chord::Description() const
{ return pModel->GetStr( STR::CHORD ); }

std::string Chord::DetailedInfo() const
{ return Description() + " " + ShortInfo(); }

std::string Chord::ShortInfo() const
{ return p1.ToString() + " - " + p2.ToString(); }


void Chord::Delete()
{
  assert( !pModel->HasHighlighted( this ) );
  Mark( false );

  DeleteContainers();
  DeleteVars();

  ActDelChord( pModel, this, DO_AT_ONCE );
}

double Chord::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{
  return DistToPoint( _crPnt, _pPosFlag,
                      !HasAnyFlag( pModel->ShowFlags(), SHW::TOPVIEW),
                      pModel->ShowFlags() );
}

double Chord::DistToPoint( const Point& _crPnt, int* _pPosFlag, bool _topView, ulong _showFlags ) const
{
  const double CHORD_RES = 20;
  if( !_topView ) {
    if( HasAnyFlag( _showFlags, SHW::CHORDS ) )
      return Point2VectorDist( p1, p2, _crPnt, _pPosFlag, NULL);

    if( HasAnyFlag( _showFlags, SHW::CHORDS3D ) && (p1.z != 0 || p2.z != 0) ) {
      Point p1t( hypot( p1.x, p1.z ), p1.y );
      double distHit = MAXDOUBLE;
      double dist = 0.;
      int posFlagHit = 0;
      int posFlagT = 0;
      bool firstIteration = true;
      for( double i = 1. / CHORD_RES; i < 1; i += 1. / CHORD_RES ) {
        Point p2t( hypot( p1.x + i * (p2.x - p1.x),
                          p1.z + i * (p2.z - p1.z ) ),
                   p1.y + i * (p2.y - p1.y) );
        dist = Point2VectorDist( p1t, p2t, _crPnt, &posFlagT, NULL );
        if( dist < distHit ) {
          distHit = dist;
          posFlagHit = posFlagT;
        }
        firstIteration = false;
        p1t = p2t;
      }
      dist = Point2VectorDist( p1t, Point( hypot( p2.x, p2.z ), p2.y ),
                               _crPnt, &posFlagT, NULL );
      if( firstIteration || dist < distHit ) {
        distHit = dist;
        posFlagHit = posFlagT;
      }
      if( _pPosFlag != null )
        *_pPosFlag = posFlagHit;
      return distHit;
    }
    else
      throw "unknown showflags combination";
  }
  else if( HasAnyFlag( _showFlags, SHW::CHORDS ) )
    return Point2VectorDist( p1.SwapYZ(), p2.SwapYZ(), _crPnt, _pPosFlag, NULL );
  else
    throw "unknown showflags combination";
  return MAXDOUBLE;
}


bool Chord::InRectangle( const Point& _p1, const Point& _p2 ) const
{
  return VectorInRectangle( p1, p2, _p1, _p2 );
}


void Chord::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMin = *_pMax = p1;
  *_pMin = Point( dg_min( _pMin->x, p1.x ), dg_min( _pMin->y, p1.y ) );
  *_pMax = Point( dg_max( _pMax->x, p1.x ), dg_max( _pMax->y, p1.y ) );
  *_pMin = Point( dg_min( _pMin->x, p2.x ), dg_min( _pMin->y, p2.y ) );
  *_pMax = Point( dg_max( _pMax->x, p2.x ), dg_max( _pMax->y, p2.y ) );
}


int Chord::Change( Point _p1, Point _p2 )
{
  ActChangeChord actChangeChord( pModel, this, _p1, _p2 );
  return actChangeChord.Do();
}

/* ignores horizontal elements. parallel case not yet tested. */
int Chord::Extend( bool /*_topView*/ )
{
  SENDER_NAME( "Chord::Extend" );
  IComponentList elements = pModel->GetMarkedElements();

  Point p0 = p1;
  Point K = p2 - p1;
  double &A = K.x, &B = K.y, &C = K.z;
  if( B == 0 ) {
    pModel->SendMessage( LOG_ERROR, SENDER, DG3::CANNOT_EXTEND_HRZ_CHORD );
    return 0;
  }

  double dy = MAXDOUBLE;
  FOREACHPTRCONST( ElementPtr, pElement, elements ) {
    Point pe = pElement->Node(1)->Position();
    double w = (pElement->Node(2)->X() - pElement->Node(1)->X()) /
               (pElement->Node(2)->Y() - pElement->Node(1)->Y());

    /* the constraints used to obtain y are:
       (x - x0)/A = (y - y0)/B = (z - z0)/C, from the chord line, and
       x^2 + z^2 = (xe + w*(y - ye))^2, for the element, along with
       the condition that y lie between e->n[1]->y and e->n[2]->y.
       x and z are eliminated, to be calculated by the line equation
       only after the final value of y has been determined.
     */

    double D = ((-C*C) * (p0.x + pe.x + w*(p0.y - pe.y)) * (p0.x - pe.x + w*(-p0.y + pe.y))
       + (B*w*p0.x - A*(pe.x + w*p0.y - w*pe.y))*(B*w*p0.x - A*(pe.x + w*p0.y - w*pe.y))
       + 2*C*(A*p0.x - B*w*(pe.x + w*p0.y - w*pe.y))*p0.z
       + ((-A*A) + B*B*w*w)*p0.z*p0.z);
    if( D < 0 )
      continue;
    D = B*sqrt( D );
    double yPos = ((-A)*B*p0.x + A*A*p0.y + C*C*p0.y + B*B*w*((pe.x - w*pe.y)) - B*C*p0.z + D)/
                  (A*A + C*C - B*B*w*w);
    double yNeg = ((-A)*B*p0.x + A*A*p0.y + C*C*p0.y + B*B*w*((pe.x - w*pe.y)) - B*C*p0.z - D)/
                  (A*A + C*C - B*B*w*w);

    double yMin = pElement->Node(1)->Y();
    double yMax = pElement->Node(2)->Y();
    if( yMin > yMax )
      dg_swap( yMin, yMax );
    double y = 0.;
    if( dg_inrange( yPos, pElement->Node(1)->Y(), pElement->Node(2)->Y() ) ) {
      if( dg_inrange( yNeg, pElement->Node(1)->Y(), pElement->Node(2)->Y() ) )
        y = fabs( yPos - p1.y ) < fabs( yNeg - p1.y ) ? yPos : yNeg;
      else
        y = yPos;
    }
    else if( dg_inrange( yNeg, pElement->Node(1)->Y(), pElement->Node(2)->Y() ) )
      y = yNeg;
    else
      continue;

    if( fabs( y - p1.y ) < dy && dg_sign( y - p1.y ) == dg_sign( B ) )
      dy = fabs( y - p1.y );
  }
  elements.clear();

  if( dy == MAXDOUBLE )
    return ERR::NOELEMSINPATH;

  double y2 = p1.y + dy / fabs( B ) * (p2.y - p1.y);
  Change( p1, Point( p1.x + A / B * (y2 - p1.y),      y2,
                        p1.z + C / B * (y2 - p1.y) ) );
  return 0;
}
