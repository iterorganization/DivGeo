#include "ViewXPointSegItem.h"

QRectF ViewXPointSegItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewXPointSegItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( penPoly );
  painter->drawLines( polyLines );

  if( showEnds && pState->editTopology ) {
    painter->setPen( penEnd );
    painter->drawLine( lineBegin );
    painter->drawLine( lineEnd );
  }

  //painter->drawRect( boundingRect() );
}

void ViewXPointSegItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewXPointSegItem::UpdateGeometry()
{
  XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );

  if( pXPS->Line() == null )
    return;

  polyLines.clear();
  PointArray* pLine = pXPS->Line();
  double length = 0;
  double start = 0, end = pXPS->LineLength();

  BOUND_DECLARE;

  if( length >= start )
    BOUND_INIT( pLine->front() );

  Point p1, p2;  
  FOREACHPAIR( p1, p2, *pLine ) {
    double partLength = hypot( fabs( p2.x - p1.x ), fabs( p2.y - p1.y ) );

    /* Skip segments not displayed at all */
    if( length >= end || length + partLength <= start)
      continue;

    /* Change ending points if partial segment */
    if( length < start ) {
      p1 = p1 + (p2 - p1)*(start - length)/partLength;
      BOUND_INIT( p1 );
    }

    if( length + partLength > end )
      p2 = p1 + (p2 - p1)*(end - length)/partLength;

    polyLines.push_back( QLineFromPoints( p1, p2 ) );
    BOUND_POINT( p2 );
    length += partLength;
  }

  if( pXPS->StartPos() > 0 ) {
    Point p1 = pXPS->Line()->at(0);
    Point p2 = pXPS->Line()->at(2);
    Point pdif = p2 - p1;

    double ll = hypot( pdif.x, pdif.y );
    Point dinv( (p2.y - p1.y) / ll, (p1.x - p2.x) / ll );
    Point p1c = p2 + dinv * separatrixSegEndLen / 2.;
    Point p2c = p2 - dinv * separatrixSegEndLen / 2.;

    BOUND_POINT( p1c );
    BOUND_POINT( p2c );

    lineBegin = QLineFromPoints( p1c, p2c );
  }
  else
    lineBegin = QLineF();

  if( pXPS->EndPos() > 0 ) {
    int size = pXPS->Line()->size();
    Point p1 = pXPS->Line()->at(size - 1);
    Point p2 = pXPS->Line()->at(size - 3);
    Point pdif = p2 - p1;

    double ll = hypot( pdif.x, pdif.y );
    Point dinv( (p2.y - p1.y) / ll, (p1.x - p2.x) / ll );
    Point p1c = p2 + dinv * separatrixSegEndLen / 2.;
    Point p2c = p2 - dinv * separatrixSegEndLen / 2.;

    BOUND_POINT( p1c );
    BOUND_POINT( p2c );

    lineEnd = QLineFromPoints( p1c, p2c );
  }
  else
    lineEnd = QLineF();

  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewXPointSegItem::UpdateDependentGeometry()
{
  separatrixSegEndLen = pPrefs->visual.separatrixSegEndLen;
}

void ViewXPointSegItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  penPoly = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::XPOINT ];
  penEnd = *pPrefs->visual.pens[ STYLE::GPOINT ];

  XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );
  GridPointSegPtr pGPS = pTopology->FindGridPointSegBySegment( pXPS );

  /* Draw the "beginning" if the segment was split by a gradient line at the beginning/end */

  showEnds = ((pGPS != null && pGPS->HasFlag( GPSF::USED )) &&
              (!isHighlighted && pXPS->Line()->size() > 2));
}

double ViewXPointSegItem::DistToPoint( QPointF _point ) const
{
  double distHit = MAXDOUBLE;
  foreach( const QLineF& line, polyLines ) {
    double dist = Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C( _point ), NULL, NULL );
    if( dist < distHit )
      distHit = dist;
  }

  return distHit;
}
