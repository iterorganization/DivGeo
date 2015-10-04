#include "ViewChordTopItem.h"


QRectF ViewChordTopItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewChordTopItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( penLine );
  painter->drawLine( topLine );
  //painter->drawRect( boundingRect() );
}

void ViewChordTopItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewChordTopItem::UpdateGeometry()
{
  ChordPtr pChord = dgtype_cast< ChordPtr >( pObject );

  p1 = pChord->Point_1();
  p2 = pChord->Point_2();

  topLine = QLineFromPoints( p1.SwapYZ(), p2.SwapYZ() );

  UpdateComments();
}

void ViewChordTopItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewChordTopItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::CHORD ];
}


double ViewChordTopItem::DistToPoint( QPointF _point ) const
{
  return Point2VectorDist( p1.SwapYZ(), p2.SwapYZ(), _C(_point), NULL, NULL );
}
