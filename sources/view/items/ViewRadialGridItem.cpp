#include "ViewRadialGridItem.h"
#include <QGraphicsScene>

QRectF ViewRadialGridItem::boundingRect() const
{
  return sceneRect;
}

void ViewRadialGridItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  sceneRect = scene()->sceneRect();
  QPolygonF viewPoly = pState->viewPoly;
  if( viewPoly.empty() )
    return;

  QPointF zero( 0., 0. );
  double step = 100. / pState->scale.x;

  painter->setPen( penLine );

  QPolygonF viewPolyExt = viewPoly;
  static const double far = 100000;
  QLineF abcissa( -far, 0, far, 0 );
  QLineF ordinata( 0, -far, 0, far );
  for( unsigned i = 0; i < (unsigned)viewPoly.size(); i++ ) {
    QLineF lineP( viewPoly[ (i == 0) ? 3 : (i - 1) ], viewPoly[i] );
    QPointF isec;
    if( lineP.intersect( abcissa, &isec ) == QLineF::BoundedIntersection )
      viewPolyExt.append( isec );
    if( lineP.intersect( ordinata, &isec ) == QLineF::BoundedIntersection )
      viewPolyExt.append( isec );
  }

  double minDist = QLineF( zero, viewPolyExt[0] ).length();
  double maxDist = minDist;
  for( unsigned i = 1; i < (unsigned)viewPolyExt.size(); i++ ) {
    double dist = QLineF( zero, viewPolyExt[i] ).length();;
    if( dist < minDist )
      minDist = dist;
    else if( dist > maxDist )
      maxDist = dist;
  }

  bool fullRound = viewPoly.containsPoint( zero, Qt::OddEvenFill );
  double minAngle = -180., maxAngle = 180.;

  if( !fullRound ) {
    minAngle = atan2( viewPoly[0].y(),viewPoly[0].x() ) * RAD2DEG;
    if( pState->center.x() < 0. && minAngle < 0. )
      minAngle += 360.;
    maxAngle = minAngle;
    for( unsigned i = 1; i < (unsigned)viewPoly.size(); i++ ) {
      double angle = atan2( viewPoly[i].y(),viewPoly[i].x() ) * RAD2DEG;
      if( pState->center.x() < 0. && angle < 0. )
        angle += 360.;
      if( angle < minAngle )
        minAngle = angle;
      if( angle > maxAngle )
        maxAngle = angle;
    }
  }

  const QMatrix& mx = painter->worldMatrix();

  ulong maxCircle = 0;
  ulong minCircle = 0;
  double r = 0.;
  if( !fullRound ) {
    minCircle = floor( minDist / step );
    if( r < minDist )
      r = step * (minCircle + 1);
    maxCircle = minCircle;
  }
  for( ; r < maxDist; r += step, maxCircle++ ) {
    painter->setPen( penLine );
    painter->drawEllipse( zero, r, r );
    painter->setPen( penText );
    painter->setWorldMatrixEnabled( false );
    if( !fullRound ) {
      double midAngle_rad = (maxAngle + minAngle) * DEG2RAD / 2.;
      QPointF isec( r * cos( midAngle_rad ), r * sin( midAngle_rad ) );
      isec = mx.map( isec );
      painter->drawText( isec, QString( "r'%1" ).arg( r ) );
    }
    else {
      QPointF isecs[4] = { mx.map( QPointF( r, 0 ) ),
                           mx.map( QPointF( -r, 0 ) ),
                           mx.map( QPointF( 0, -r ) ),
                           mx.map( QPointF( 0, r ) ) };
      for( int c = 0; c < 4; c++ )
        painter->drawText( isecs[c], QString( "r'%1" ).arg( r ) );
    }
    painter->setWorldMatrixEnabled( true );
  }

  /* Find dirCoint for (arkLen == step) on (circle == minCircle) */
  uint dirCount = (int)(M_2XPI * minCircle) + 1;
  if( dirCount < 4 )
    dirCount = 4;

  for( uint circle = minCircle; circle < maxCircle; circle++ ) {
    double radius = circle * step;
    double angle_rad = M_2XPI / dirCount;
    double arkLen = radius * angle_rad;
    if( arkLen > step * 1.4 )
      dirCount *= 2;
    else if( circle != 0 && circle != minCircle )
      continue;
    double angle_deg = 360. / dirCount;

    int dirFrom = fullRound ? 0 : floor( (double)dirCount * minAngle / 360. );
    int dirTo = fullRound ? dirCount : floor( (double)dirCount * maxAngle / 360. ) + 1;

    for( int dir = dirFrom; dir < dirTo; dir++ ) {
      if( dir%2 == 0 && circle != minCircle && circle != 0 )
        continue;
      QLineF radiusLine( QPointF( (double)((circle < minCircle) ? minCircle - 1 : circle)*step, 0 ),
                         QPointF( (double)(maxCircle + 1)*step, 0 ) );
      QMatrix radmx;
      radmx.rotate( 360. * (double)dir / (double)dirCount );
      radiusLine = radmx.map( radiusLine );
      painter->setPen( penLine );
      painter->drawLine( radiusLine );

      painter->setPen( penText );
      painter->setWorldMatrixEnabled( false );
      QPointF isec;
      for( unsigned i = 0; i < (unsigned)viewPoly.size(); i++ ) {
        QLineF lineP( viewPoly[ (i == 0) ? 3 : (i - 1) ], viewPoly[i] );
        QLineF::IntersectType it = lineP.intersect( radiusLine, &isec );
        if( it != QLineF::BoundedIntersection )
          continue;
        isec = mx.map( isec );
        Point offset;
        switch( i ) {
        case 0: offset = Point( 2., -2. ); break;
        case 1: offset = Point( 2., 10. ); break;
        case 2: offset = Point( -55., -2. ); break;
        case 3: offset = Point( 2., -20. ); break;
        }
        painter->drawText( isec + offset, QString( "%1°" ).arg( dir*angle_deg ) );
      }
      painter->setWorldMatrixEnabled( true );
    }
  }
}

void ViewRadialGridItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewRadialGridItem::UpdateGeometry()
{
  prepareGeometryChange();
}

void ViewRadialGridItem::UpdateDependentGeometry()
{
  gridCellMax = pPrefs->visual.gridCellMax;
  sceneRect = scene()->sceneRect();

  UpdateComments();
}

void ViewRadialGridItem::UpdateStyle()
{
  penLine = *pPrefs->visual.pens[ STYLE::GRID ];
  penText = *pPrefs->visual.pens[ STYLE::GRIDTEXT ];
}
