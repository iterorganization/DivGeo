#include "ViewGridPointExItem.h"

QRectF ViewGridPointExItem::boundingRect() const
{
  return QRectF( line.p1(), line.p2() );
}

void ViewGridPointExItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  if( !show )
    return;

  painter->setPen( pen );
  painter->drawLine( line );

  //painter->drawRect( boundingRect() );
}

void ViewGridPointExItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewGridPointExItem::UpdateGeometry()
{
  // No independent geometry
  UpdateComments();
}

void ViewGridPointExItem::UpdateDependentGeometry()
{
  GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
  position = pGPX->Position();
  Point p = pGPX->Derivative().SwapXY() * Point( -1., 1. ) * pPrefs->visual.gridPointLen / 2.;
  line = QLineFromPoints( position + p, position - p );
}

void ViewGridPointExItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  pen = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::GPOINT ];

  GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
  show = pGPX->CoordsOk();
}

double ViewGridPointExItem::DistToPoint( QPointF _point ) const
{
  //return hypot( _point.x() - position.x, _point.y() - position.y );
  return dg_min( hypot( _point.x() - line.p1().x(), _point.y() - line.p1().y() ),
                 hypot( _point.x() - line.p2().x(), _point.y() - line.p2().y() ) );
}
