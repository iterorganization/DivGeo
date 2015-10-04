#include "ViewXPointTestItem.h"

QRectF ViewXPointTestItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewXPointTestItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  /*if( !pState->editTopology )
    return;*/

  painter->setPen( penGrad );
  painter->drawLines( gradLines );

  painter->setPen( penBound );
  painter->drawLines( boundLines, 4 );

  //painter->drawRect( boundingRect() );
}

void ViewXPointTestItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewXPointTestItem::UpdateGeometry()
{
  XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );

  center = pXPT->Center();

  BOUND_DECLARE;
  bool firstIteration = true;

  gradLines.clear();
  for( unsigned i = 0; i < 4; i++ ) {
    PointArray* pGrad = pXPT->Gradient(i);
    if( pGrad != NULL && !pGrad->empty() ) {
      for( PointIter itP = pGrad->begin() + 1, itPEnd = pGrad->end();
           itP != itPEnd; ++itP ) {
        Point p1 = *(itP - 1);
        gradLines.push_back( QLineFromPoints( p1, *itP ) );

        if( firstIteration ) {
          BOUND_INIT( p1 );
          firstIteration = false;
        }
        else
          BOUND_POINT( p1 );
      }
      Point p2 = pGrad->back();
      BOUND_POINT( p2 );
    }
  }

  NPoint c1 = pXPT->EndPos(1);
  NPoint c2 = pXPT->EndPos(2);
  ps[0] = /*11*/ _C( pEquil->GetPoint( c1 ) );
  ps[1] = /*22*/ _C( pEquil->GetPoint( c2 ) );
  ps[2] = /*21*/ _C( pEquil->GetPoint( NPoint( c2.x, c1.y ) ) );
  ps[3] = /*12*/ _C( pEquil->GetPoint( NPoint( c1.x, c2.y ) ) );

  boundLines[0] = QLineFromPoints( ps[0], ps[2] );
  boundLines[1] = QLineFromPoints( ps[0], ps[3] );
  boundLines[2] = QLineFromPoints( ps[1], ps[2] );
  boundLines[3] = QLineFromPoints( ps[1], ps[3] );

  for( unsigned i = 0; i < 4; i++ )
    BOUND_POINT( ps[i] );

  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewXPointTestItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewXPointTestItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  penGrad = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::XPOINTTEST ];
  penBound = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::XPOINTTEST_BOUND ];
}

double ViewXPointTestItem::DistToPoint( QPointF _point ) const
{
  //dg2: return hypot( _point.x() - center.x, _point.y() - center.y );
  double min_dist = MAXDOUBLE;
  for( int c = 0; c < 4; c++ ) {
    double dist = hypot( _point.x() - ps[c].x, _point.y() - ps[c].y );
    if( dist < min_dist )
      min_dist = dist;
  }
  return min_dist;
}
