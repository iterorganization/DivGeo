#include "ViewGridPointSegItem.h"

QRectF ViewGridPointSegItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewGridPointSegItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  if( !pState->editTopology )
    return;

  painter->setPen( penLine );
  painter->drawLine( line1 );
  painter->drawLine( line2 );
  painter->drawLine( lineT );

  const QMatrix& mx = painter->worldMatrix();
  QPointF p2T = mx.map( lineT.p2() );

  painter->setPen( penText );
  painter->setFont( font );
  painter->setWorldMatrixEnabled( false );
  painter->drawText( p2T, text );
  painter->setWorldMatrixEnabled( true );

  QFontMetricsF fm = QFontMetricsF( font );
  QRectF textRect = fm.boundingRect( text );
  textRect.moveTo( lineT.p2() + textRect.topLeft() );
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

void ViewGridPointSegItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewGridPointSegItem::UpdateGeometry()
{
  // No independent geometry
  UpdateComments();
}

void ViewGridPointSegItem::UpdateDependentGeometry()
{
  GridPointSegPtr pGPS = dgtype_cast< GridPointSegPtr >( pObject );

  /* Find the central point */
  PointArray* pLine = pGPS->XPS()->Line();
  int i = 0;
  if( pLine->size() >= 4 )
    i = pLine->size() / 2.;

  Point p0 = pLine->at(i);
  Point p1 = pLine->at(i+1);
  Point dir = (p0 - p1) * pGPS->Dir();

  const double DGPS_ANGLE = 20.0 / 180.0 * M_PI;
  double arrowLength = pPrefs->visual.arrowSize.y;

  double hyp = hypot( dir.x, dir.y );
  if( hyp <= 0 )
    return;
  dir *= arrowLength / hyp;

  double cosa = cos( DGPS_ANGLE );
  double sina = sin( DGPS_ANGLE );

  Point p12( p1.x + dir.x*cosa - dir.y*sina, p1.y + dir.x*sina + dir.y*cosa );
  Point p22( p1.x + dir.x*cosa + dir.y*sina, p1.y - dir.x*sina + dir.y*cosa );
  Point pT2( p1 + arrowLength );

  line1 = QLineFromPoints( p1, p12 );
  line2 = QLineFromPoints( p1, p22 );
  lineT = QLineFromPoints( p1, pT2 );
  text = QString( "%1 (%2)" ).arg( pGPS->Zone() ).arg( QString::fromStdString( pGPS->ShortName() ) );

  BOUND_DECLARE;
  BOUND_INIT( p1 );
  BOUND_POINT( p12 );
  BOUND_POINT( p22 );
  BOUND_POINT( pT2 );
  BOUND_RECT( fullBoundingRect );
}

void ViewGridPointSegItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::GPOINT ];
  penText = *pPrefs->visual.pens[ STYLE::TEXT ];
  font = QFont( "Arial", penText.width() );
}
