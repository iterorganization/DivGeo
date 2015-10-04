#include "ViewAxesItem.h"
#include <QGraphicsScene>

QRectF ViewAxesItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewAxesItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( pen );
  painter->drawLines( aLines, 3 );

  //painter->drawRect( boundingRect() );
}

void ViewAxesItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewAxesItem::UpdateGeometry()
{
  prepareGeometryChange();
}

void ViewAxesItem::UpdateDependentGeometry()
{
  QGraphicsScene* pScene_tmp = scene();
  if( pScene_tmp == null )
    return;
  QRectF sceneRect = pScene_tmp->sceneRect();
  Point arrowSize = _C( pPrefs->visual.arrowSize );

  // Detect begin and end points
  Point endPnt, beginPnt;
  double& rEndVal = abscissa ? endPnt.x : endPnt.y;
  double& rBeginVal = abscissa ? beginPnt.x : beginPnt.y;

  rEndVal = abscissa ? sceneRect.right() : sceneRect.bottom();
  rBeginVal = abscissa ? sceneRect.left() : sceneRect.top();

  if( rBeginVal > -arrowSize.x )
      rBeginVal = -arrowSize.x;
  if( rEndVal < arrowSize.x )
      rEndVal = arrowSize.x;

  // Axis line
  aLines[0] = QLineF( _C( beginPnt ), _C( endPnt ) );

  // Full bounding rect
  if( abscissa ) {
    fullBoundingRect = QRectF( QPointF( rEndVal, arrowSize.x ),
                               QPointF( rBeginVal, -arrowSize.x ) );
    arrowSize = arrowSize.SwapXY();
  }
  else
    fullBoundingRect = QRectF( QPointF( arrowSize.x, rEndVal ),
                               QPointF( -arrowSize.x, rBeginVal ) );

  // Arrow lines
  QPolygonF viewPoly = pState->viewPoly;
  QPointF isec_max = _C( beginPnt );
  for( unsigned i = 0; i < (unsigned)viewPoly.size(); i++ ) {
    QLineF lineP( viewPoly[ (i == 0) ? 3 : (i - 1) ], viewPoly[i] );
    QPointF isec;
    QLineF::IntersectType it = lineP.intersect( aLines[0], &isec );
    if( it == QLineF::NoIntersection || it == QLineF::UnboundedIntersection )
      continue;
    if( (abscissa && isec.x() > isec_max.x()) ||
        (!abscissa && isec.y() > isec_max.y()) )
      isec_max = isec;
  }
  Point modifier = abscissa ? Point( -1., 1. ) : Point( 1., -1. );
  Point arrowSizeT = arrowSize / pState->scale;
  aLines[1] = QLineF( isec_max, isec_max - arrowSizeT );
  aLines[2] = QLineF( isec_max, isec_max + arrowSizeT * modifier );
}

void ViewAxesItem::UpdateStyle()
{
  pen = *pPrefs->visual.pens[ STYLE::AXES ];
}
