#include "ViewChordItem.h"

QRectF ViewChordItem::boundingRect() const
{
  QRectF br_result;
  if( showZProjection )
    br_result = br_result.united( boundingRect3D );  
  if( showXProjection )
    br_result = br_result.united( fullBoundingRect );

  return br_result;
}

void ViewChordItem::Build3DGeometry()
{
  lines3D.clear();

  BOUND_DECLARE;
  const double CHORD_RES = 20.;
  QPointF p1t( hypot( p1.x, p1.z ), p1.y );
  BOUND_INIT( _C( p1t ) );

  for( double i = 1. / CHORD_RES; i < 1; i += 1. / CHORD_RES ) {
    Point tmp = p1 + i * (p2 - p1);
    Point p2t( hypot( tmp.x, tmp.z ), tmp.y );
    lines3D.push_back( QLineF( p1t, _C( p2t ) ) );

    BOUND_POINT( p2t );
    p1t = _C( p2t );
  }
  Point p2t( hypot( p2.x, p2.z ), p2.y );
  lines3D.push_back( QLineF( p1t, _C( p2t ) ) );

  BOUND_POINT( p2t );
  BOUND_RECT( boundingRect3D );

}

void ViewChordItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  if( showZProjection ) {
    if( p1.z != 0. || p2.z != 0. ) {
      if( isMarked ) {
        painter->setPen( penMark );
        foreach( const QLineF& line, lines3D )
          painter->drawLine( line );
      }

      painter->setPen( penLine );
      foreach( const QLineF& line, lines3D )
        painter->drawLine( line );
    }
  }

  if( showXProjection ){
    if( isMarked ) {
      painter->setPen( penMark );
      painter->drawLine( line );
    }

    BOUND_DECLARE;
    BOUND_QINIT( line.p1() );
    BOUND_QPOINT( line.p2() );

    painter->setPen( penLine );
    painter->drawLine( line );

    if( showNormals ) {
      painter->setPen( penNormal );
      const QMatrix& mx = painter->worldMatrix();
      QLineF lineT = mx.map( line );
      QPointF invdif( lineT.y1() - lineT.y2(), lineT.x2() - lineT.x1() );
      QPointF begin = mx.map( pos );

      QPointF end = begin - invdif * normalLength;

      painter->setWorldMatrixEnabled( false );
      painter->drawLine( QLineF( begin, end ) );
      painter->setWorldMatrixEnabled( true );

      QMatrix mxInv = mx.inverted();
      normalEnd = mxInv.map( end );
    }
    else
      normalEnd = pos;

    BOUND_QPOINT( normalEnd );
    BOUND_RECT( fullBoundingRect );
  }

  //painter->drawRect( boundingRect() );
}

void ViewChordItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewChordItem::UpdateGeometry()
{
  ChordPtr pChord = dgtype_cast< ChordPtr >( pObject );

  p1 = pChord->Point_1();
  p2 = pChord->Point_2();

  line = QLineFromPoints( p1, p2 );
  pos = _C( (p1 + p2) / 2. );

  Build3DGeometry();

  BOUND_DECLARE;
  BOUND_QINIT( line.p1() );
  BOUND_QPOINT( line.p2() );
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewChordItem::UpdateDependentGeometry()
{
  double dist = Point2PointDist( p1, p2 );
  double normalLength_min = pPrefs->visual.normalLen;
  if( dist > normalLength_min / 1e6 )
    normalLength = normalLength_min / dist;
  else
    normalLength = 0.;
}

void ViewChordItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  isMarked = pObject->IsMarked() && !pObject->IsHighlighted();

  penMark = *pPrefs->visual.pens[ STYLE::MARK ];
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::CHORD ];
  penNormal = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::NORMAL ];
}


double ViewChordItem::DistToPoint( QPointF _point ) const
{
  return Point2VectorDist( p1, p2, _C(_point), NULL, NULL);

  //TODO:???
  /*if( show3D && (p1.z != 0 || p2.z != 0) ) {
    double distHit = MAXDOUBLE;
    foreach( QLineF line, lines3D ) {
      double dist = Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C(_point), NULL, NULL );
      if( dist < distHit )
        distHit = dist;
    }

    return distHit;
  }*/
}

bool ViewChordItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QLineF line_t = _transform.map( line );
  return VectorInRectangle( _C( line_t.p1() ), _C( line_t.p2() ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
