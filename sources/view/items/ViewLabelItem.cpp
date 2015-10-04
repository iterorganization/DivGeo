#include "ViewLabelItem.h"

QRectF ViewLabelItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewLabelItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  QLineF line = QLineFromPoints( pos, pos + offset );

  painter->setPen( pen );
  painter->drawLine( line );

  const QMatrix& mx = painter->worldMatrix();
  QPointF p2T = mx.map( line.p2() );

  painter->setFont( font );
  painter->setWorldMatrixEnabled( false );
  painter->drawText( p2T, text );
  painter->setWorldMatrixEnabled( true );

  QFontMetricsF fm = QFontMetricsF( font );
  QRectF textRect = fm.boundingRect( text );
  textRect.moveTo( line.p2() + textRect.topLeft() );
  //painter->drawRect( textRect );

  Point points[] =  { _C( fullBoundingRect.topLeft() ),
                      _C( fullBoundingRect.bottomRight() ),
                      _C( textRect.topLeft() ),
                      _C( textRect.bottomRight() ) };
  BOUND_DECLARE;
  BOUND_INIT( points[0] );
  BOUND_POINT( points[1] );
  BOUND_POINT( points[2] );
  BOUND_POINT( points[3] );
  BOUND_RECT( fullBoundingRect );

  //painter->drawRect( boundingRect() );
}

void ViewLabelItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewLabelItem::UpdateGeometry()
{
  pos = pObject->CentralPoint();
  UpdateComments();
}

void ViewLabelItem::UpdateDependentGeometry()
{
  offset = _C( pPrefs->visual.labelOffset );

  BOUND_DECLARE;
  BOUND_INIT( pos );
  BOUND_POINT( offset );
  BOUND_RECT( fullBoundingRect );
}

void ViewLabelItem::UpdateStyle()
{
  setZValue( 2. );
  pen = *pPrefs->visual.pens[ STYLE::LABEL ];
  font = QFont( "Arial", pPrefs->visual.labelTextSize );
}
