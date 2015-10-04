#include "ViewHighlightedRectItem.h"

QRectF ViewHighlightedRectItem::boundingRect() const
{
  QRectF full_boundingRect = rect.boundingRect();
  return QRectF( full_boundingRect.topLeft() - QPointF( 1., 1. ),
                 full_boundingRect.bottomRight() + QPointF( 1., 1. ) );
}

void ViewHighlightedRectItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( pen );
  painter->drawPolygon( rect );
}

void ViewHighlightedRectItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewHighlightedRectItem::UpdateGeometry()
{
  prepareGeometryChange();
  UpdateComments();
}

void ViewHighlightedRectItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewHighlightedRectItem::UpdateStyle()
{
  pen = *pPrefs->visual.pens[ STYLE::TOOL ];
}
