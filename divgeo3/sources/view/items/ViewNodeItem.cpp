#include "ViewNodeItem.h"

QRectF ViewNodeItem::boundingRect() const
{
  return nodeBoundingRect;
}

void ViewNodeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  if( !show )
    return;
  const QMatrix& mx = painter->worldMatrix();
  QPointF centerT = mx.map( center );

  painter->setPen( pen );
  nodeBoundingRect = QRectF( centerT - nodeRadius, centerT + nodeRadius );
  painter->setWorldMatrixEnabled( false );
  if( drawRect )
    painter->drawRect( nodeBoundingRect );
  else
    painter->drawEllipse( nodeBoundingRect );
  painter->setWorldMatrixEnabled( true );

  QMatrix mxInv = mx.inverted();
  nodeBoundingRect = mxInv.mapRect( nodeBoundingRect );
}

void ViewNodeItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewNodeItem::UpdateGeometry()
{
  NodePtr pNode = dgtype_cast< NodePtr >( pObject );
  center = _C( pNode->Position() );
  // Temporary rect for first paint
  nodeBoundingRect = QRectF( center - 10, center + 10 );

  UpdateComments();
}

void ViewNodeItem::UpdateDependentGeometry()
{
  nodeRadius = Point( pPrefs->visual.nodeRadius );
}

void ViewNodeItem::UpdateStyle()
{
  NodePtr pNode = dgtype_cast< NodePtr >( pObject );
  drawRect = pPrefs->visual.useSquares;
  bool isIrregular = !pNode->IsRegular( null );
  show = ( showAll || (isIrregular && showIrregular ) );
  setVisible( show );
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  pen = *pPrefs->visual.pens[ pObject->IsHighlighted() ? STYLE::HIGHLIGHT : STYLE::NODE ];
}

double ViewNodeItem::DistToPoint( QPointF _point ) const
{
  return hypot( _point.x() - center.x(), _point.y() - center.y() );
}
