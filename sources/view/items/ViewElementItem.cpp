#include "ViewElementItem.h"

QRectF ViewElementItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewElementItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  BOUND_DECLARE;
  BOUND_QINIT( line.p1() );
  BOUND_QPOINT( line.p2() );

  if( isMarked ) {
    painter->setPen( penMark );
    painter->drawLine( line );
  }

  painter->setPen( penLine );
  painter->drawLine( line );

  const QMatrix& mx = painter->worldMatrix();
  QPointF begin = mx.map( pos );
  QMatrix mxInv = mx.inverted();

  if( showNormals ) {
    painter->setPen( penNormal );
    QLineF lineT = mx.map( line );
    QPointF invdif( lineT.y1() - lineT.y2(), lineT.x2() - lineT.x1() );
    QPointF end = begin + invdif * normalLength;

    painter->setWorldMatrixEnabled( false );
    painter->drawLine( QLineF( begin, end ) );
    painter->setWorldMatrixEnabled( true );

    normalEnd = mxInv.map( end );
    BOUND_QPOINT( normalEnd );
  }
  else
    normalEnd = pos;

  BOUND_RECT( fullBoundingRect );

  if( showNumbers ) {
    painter->setPen( penNumber );
    painter->setFont( QFont( "Arial", penNumber.width() ) );
    painter->setWorldMatrixEnabled( false );
    painter->drawText( begin, number );
    painter->setWorldMatrixEnabled( true );

    QFontMetricsF fm = QFontMetricsF( painter->font() );
    textBoundingRect = fm.boundingRect( showNumbers ? number : "" );
    textBoundingRect.moveTo( begin + textBoundingRect.topLeft() );
    textBoundingRect = mxInv.mapRect( textBoundingRect );

    fullBoundingRect = fullBoundingRect.united( textBoundingRect );
  }
}

void ViewElementItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewElementItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  isMarked = pObject->IsMarked();

  penMark = *pPrefs->visual.pens[ STYLE::MARK ];
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::ELEM ];
  penNumber = *pPrefs->visual.pens[ STYLE::TEXT ];
  penNormal = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::NORMAL ];
}

void ViewElementItem::UpdateGeometry()
{
  ElementPtr pElem = dgtype_cast< ElementPtr >( pObject );
  Point p1 = pElem->Node(1)->Position();
  Point p2 = pElem->Node(2)->Position();
  line = QLineFromPoints( p1, p2 );
  pos = _C( (p1 + p2) / 2. );
  number = QString( "%1" ).arg( pElem->Id() );  

  BOUND_DECLARE;
  BOUND_QINIT( line.p1() );
  BOUND_QPOINT( line.p2() );
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewElementItem::UpdateDependentGeometry()
{
  double dist = Point2PointDist( _C( line.p1() ), _C( line.p2() ) );
  double normalLength_min = pPrefs->visual.normalLen;
  if( dist > normalLength_min / 1e6 )
    normalLength = normalLength_min / dist;
  else
    normalLength = 0.;
}

double ViewElementItem::DistToPoint( QPointF _point ) const
{
  /*Point central_dist = _C( (line.p1() + line.p2()) / 2. - _point );
  return hypot( central_dist );*/
  return Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C( _point ), null, null );
}

bool ViewElementItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QLineF line_t = _transform.map( line );
  return VectorInRectangle( _C( line_t.p1() ), _C( line_t.p2() ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
