#include "ViewSurfaceExItem.h"

QRectF ViewSurfaceExItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewSurfaceExItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( pen );
  painter->drawLines( lines );

  //painter->drawRect( boundingRect() );
}

void ViewSurfaceExItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewSurfaceExItem::UpdateGeometry()
{
  lines.clear();

  SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
  if( !pSX->CoordsOk() )
    return;
  if( !pSX->HasLine() )
    return;

  const PointArray* pLine = pSX->Line();
  BOUND_DECLARE;
  BOUND_INIT( pLine->front() );
  Point p2, p1;
  FOREACHPAIRCONST( p1, p2, *pLine ) {
    lines.push_back( QLineFromPoints( p1, p2 ) );
    BOUND_POINT( p2 );
  }
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewSurfaceExItem::UpdateDependentGeometry()
{
  // No independent geometry
}

void ViewSurfaceExItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : -0.1 );

  pen = *pPrefs->visual.pens[ showError ? STYLE::ERROR :
                         (isHighlighted ? STYLE::HIGHLIGHT : STYLE::SURFACE) ];
}

double ViewSurfaceExItem::DistToPoint( QPointF _point ) const
{
  double distHit = MAXDOUBLE;
  foreach( const QLineF& line, lines ) {
    double dist = Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C( _point ), NULL, NULL );
    if( dist < distHit )
      distHit = dist;
  }

  return distHit;
}
