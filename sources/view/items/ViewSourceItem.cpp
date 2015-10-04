#include "ViewSourceItem.h"

QRectF ViewSourceItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewSourceItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  if( isMarked ) {
    painter->setPen( penMark );
    painter->drawEllipse( markRect );
  }

  painter->setPen( penLine );
  painter->drawLines( lines, 5 );
}

void ViewSourceItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewSourceItem::UpdateGeometry()
{
  // No independent geometry
  UpdateComments();
}

void ViewSourceItem::UpdateDependentGeometry()
{
  SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
  position = pSrc->Position();

  double radius = pPrefs->visual.sourceRadius;
  markRect = QRectF( _C( position - radius ), _C( position + radius ) );

  Point rp( (int)(0.886 * radius), (int)(0.500 * radius) );
  Point r0( 0., radius );

  lines[0] = QLineFromPoints( position - r0, position + r0 );
  lines[1] = QLineFromPoints( position - rp, position + rp );
  rp.x = -rp.x;
  lines[2] = QLineFromPoints( position - rp, position + rp );

  fullBoundingRect.setCoords( position.x - rp.x, position.y - radius,
                              position.x + rp.x, position.y + radius );
}

void ViewSourceItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  isMarked = pObject->IsMarked();

  penMark = *pPrefs->visual.pens[ STYLE::MARK ];
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::SOURCE ];
}

double ViewSourceItem::DistToPoint( QPointF _point ) const
{
  return hypot( _point.x() - position.x, _point.y() - position.y );
}

bool ViewSourceItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QPointF position_q = _C( position );
  QPointF position_t = _transform.map( position_q );
  return PointInRectangle( _C( position_t ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
