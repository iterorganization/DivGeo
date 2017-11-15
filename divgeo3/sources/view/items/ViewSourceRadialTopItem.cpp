#include "ViewSourceRadialTopItem.h"

QRectF ViewSourceRadialTopItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewSourceRadialTopItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( penLine );
  painter->drawLines( lines );
}

void ViewSourceRadialTopItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewSourceRadialTopItem::UpdateGeometry()
{
  // No independent geometry
  UpdateComments();
}

void ViewSourceRadialTopItem::UpdateDependentGeometry()
{
  SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
  Point position = pSrc->Position();

  double radius = pPrefs->visual.sourceRadius;

  lines.clear();

  Point rp( (int)(0.886 * radius), (int)(0.500 * radius) );
  Point r0( 0., radius );

  for( double phi = 0.; phi < M_2XPI; phi += PHI_STEP ) {
    Point position_t = Point( cos( phi ), sin( phi ) ) * position.x;

    lines.push_back( QLineFromPoints( position_t - r0, position_t + r0 ) );
    lines.push_back( QLineFromPoints( position_t - rp, position_t + rp ) );
    rp.x = -rp.x;
    lines.push_back( QLineFromPoints( position_t - rp, position_t + rp ) );
  }

  fullBoundingRect.setCoords( -position.x - radius, position.x + radius, position.x + radius, -position.x - radius );
}


void ViewSourceRadialTopItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::SOURCE ];
}
