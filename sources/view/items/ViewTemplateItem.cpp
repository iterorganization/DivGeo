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

  PointArray points = pTempl->GetTransformedPoints();
  for( PointArray::const_iterator it = points.begin(), it_end = points.end(); it != it_end; it += 2 ) {
    lines.push_back( QLineFromPoints( *it, *(it+1) ) );

    if( firstIteration ) {
      BOUND_INIT( *it );
      firstIteration = false;
    }
    else
      BOUND_POINT( *it );

    BOUND_POINT( *(it+1) );
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
