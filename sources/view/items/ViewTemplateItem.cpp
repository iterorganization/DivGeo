#include "ViewTemplateItem.h"

QRectF ViewTemplateItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewTemplateItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( pen );
  painter->drawLines( lines );
}

void ViewTemplateItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewTemplateItem::UpdateGeometry()
{
  TemplatePtr pTempl = dgtype_cast< TemplatePtr >( pObject );
  lines.clear();

  BOUND_DECLARE;
  bool firstIteration = true;

  pTempl->Points_Reset();
  while( !pTempl->Points_End() ) {
    Point p1 = pTempl->TransformPoint( pTempl->Points_Next() );
    Point p2 = pTempl->TransformPoint( pTempl->Points_Next() );
    lines.push_back( QLineFromPoints( p1, p2 ) );

    if( firstIteration ) {
      BOUND_INIT( p1 );
      firstIteration = false;
    }
    else
      BOUND_POINT( p1 );

    BOUND_POINT( p2 );
  }
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewTemplateItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewTemplateItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  pen = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::TEMPLATE ];
}
