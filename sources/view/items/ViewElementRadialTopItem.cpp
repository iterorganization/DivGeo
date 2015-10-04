#include "ViewElementRadialTopItem.h"

QRectF ViewElementRadialTopItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewElementRadialTopItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( penLine );
  foreach( const QLineF& line, lines )
    painter->drawLine( line );
}

void ViewElementRadialTopItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewElementRadialTopItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::ELEM ];
}

void ViewElementRadialTopItem::UpdateGeometry()
{
  ElementPtr pElem = dgtype_cast< ElementPtr >( pObject );
  double x1 = pElem->Node(1)->X();
  double x2 = pElem->Node(2)->X();

  BuildTopProjection( x1, x2, lines, fullBoundingRect );

  UpdateComments();
}

void ViewElementRadialTopItem::UpdateDependentGeometry()
{
  // No dependent geometry
}
