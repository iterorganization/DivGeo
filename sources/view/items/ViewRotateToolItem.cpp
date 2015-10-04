#include "ViewRotateToolItem.h"
#include <QGraphicsScene>

QRectF ViewRotateToolItem::boundingRect() const
{
  return rect;
}

void ViewRotateToolItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  center = pState->center;
  QPointF centerT = painter->worldMatrix().map( center );
  QPointF end_0( 0., -50. );
  QMatrix mx;
  mx.rotate( -pState->xyAngle * RAD2DEG );
  QPointF end = mx.map( end_0 );
  end += centerT;
  end_0 += centerT;

  QPointF halfRect( 50., 50. );

  painter->setPen( pen );

  painter->setWorldMatrixEnabled( false );
  painter->drawEllipse( QRectF( centerT - halfRect, centerT + halfRect ) );
  painter->drawLine( centerT, end );
  painter->drawLine( centerT, end_0 );
  painter->setWorldMatrixEnabled( true );

  rect = scene()->sceneRect(); //TODO: optimize
}

void ViewRotateToolItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewRotateToolItem::UpdateGeometry()
{
  prepareGeometryChange();
  UpdateComments();
}

void ViewRotateToolItem::UpdateDependentGeometry()
{
  rect = scene()->sceneRect();
}

void ViewRotateToolItem::UpdateStyle()
{
  pen = *pPrefs->visual.pens[ STYLE::TOOL ];
}
