#include "ViewSeparatorItem.h"

QRectF ViewSeparatorItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewSeparatorItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  if( isMarked ) {
    painter->setPen( penMark );
    painter->drawLine( line );
  }

  painter->setPen( penLine );
  painter->drawLine( line );

  const QMatrix& mx = painter->worldMatrix();
  QPointF begin = mx.map( pos );
  QMatrix mxInv = mx.inverted();

  if( showNumbers ) {
    painter->setPen( penNumber );
    painter->setFont( font );
    painter->setWorldMatrixEnabled( false );
    painter->drawText( begin, number );
    painter->setWorldMatrixEnabled( true );
  }

  QFontMetricsF fm = QFontMetricsF( painter->font() );
  QRectF textBoundingRect = fm.boundingRect( showNumbers ? number : "" );
  textBoundingRect.moveTo( begin + textBoundingRect.topLeft() );
  textBoundingRect = mxInv.mapRect( textBoundingRect );

  Point points[] = {
    _C( textBoundingRect.topLeft() ),
    _C( textBoundingRect.bottomRight() ),
    _C( line.p1() ),
    _C( line.p2() ) };
  BOUND_DECLARE;
  BOUND_INIT( points[0] );
  for( unsigned i = 1; i < 4; i++ )
    BOUND_POINT( points[i] );
  BOUND_RECT( fullBoundingRect );
}

void ViewSeparatorItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewSeparatorItem::UpdateGeometry()
{
  SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObject );
  line = QLineFromPoints( pSep->Position(), pSep->Node()->Position() );

  pos = (line.p1() + line.p2()) / 2.;
  number = QString( "%1" ).arg( pSep->Id() );

  UpdateComments();
}

void ViewSeparatorItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewSeparatorItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  isMarked = pObject->IsMarked();

  penMark   = *pPrefs->visual.pens[ STYLE::MARK ];
  penLine   = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::SEPARATOR ];
  penNumber = *pPrefs->visual.pens[ STYLE::TEXT ];
  font = QFont( "Arial", penNumber.width() );
}


double ViewSeparatorItem::DistToPoint( QPointF _point ) const
{
  return Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C( _point ), null, null );
}

bool ViewSeparatorItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QLineF line_t = _transform.map( line );
  return VectorInRectangle( _C( line_t.p1() ), _C( line_t.p2() ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
