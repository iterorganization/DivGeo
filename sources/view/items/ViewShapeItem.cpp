#include "ViewShapeItem.h"

QRectF ViewShapeItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewShapeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( pen );

  switch( shape ) {
  case LINE:  painter->drawLine( QLineF( _C( p1 ), _C( p2 ) ) ); break;
  case RECT:  painter->drawRect( QRectF( _C( p1 ), _C( p2 ) ) ); break;
  case POINT: {
    const QMatrix& mx = painter->worldMatrix();
    QPointF centerT = mx.map( _C( p1 ).toQPointF() );
    painter->setWorldMatrixEnabled( false );
    fullBoundingRect = QRectF( _C( centerT - nodeRadius - 1 ), _C( centerT + nodeRadius + 1 ) );
    painter->drawRect( QRectF( _C( centerT - nodeRadius ), _C( centerT + nodeRadius ) ) );
    painter->setWorldMatrixEnabled( true );
    QMatrix mxInv = mx.inverted();
    fullBoundingRect = mxInv.mapRect( fullBoundingRect );
    break;
  }
  }
}

void ViewShapeItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewShapeItem::UpdateGeometry()
{
  BOUND_DECLARE;
  BOUND_INIT( p1 );
  BOUND_POINT( p2 );
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewShapeItem::UpdateDependentGeometry()
{
  nodeRadius = Point( pPrefs->visual.nodeRadius );
}

void ViewShapeItem::UpdateStyle()
{
  pen = *pPrefs->visual.pens[ STYLE::TOOL ];
}
