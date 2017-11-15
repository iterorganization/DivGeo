#include "ViewChordRadialTopItem.h"

QRectF ViewChordRadialTopItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewChordRadialTopItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( penLine );
  //painter->drawLine( topLine );
  foreach( const QLineF& line, lines )
    painter->drawLine( line );

  //painter->drawRect( boundingRect() );
}

void ViewChordRadialTopItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewChordRadialTopItem::UpdateGeometry()
{
  ChordPtr pChord = dgtype_cast< ChordPtr >( pObject );

  p1 = pChord->Point_1();
  p2 = pChord->Point_2();

  //topLine = QLineFromPoints( p1.SwapYZ(), p2.SwapYZ() );

  BuildTopProjection( p1.x, p2.x, lines, fullBoundingRect );

  UpdateComments();
}

void ViewChordRadialTopItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewChordRadialTopItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::CHORD ];
}


double ViewChordRadialTopItem::DistToPoint( QPointF _point ) const
{
  return Point2VectorDist( p1.SwapYZ(), p2.SwapYZ(), _C(_point), NULL, NULL );
}
