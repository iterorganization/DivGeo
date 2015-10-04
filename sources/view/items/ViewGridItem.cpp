#include "ViewGridItem.h"
#include <QGraphicsScene>

QRectF ViewGridItem::boundingRect() const
{
  return sceneRect;
}

void ViewGridItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  QFont font = painter->font();
  font.setPointSize( penText.width() );
  painter->setFont( font );

  painter->setPen( penLine );
  painter->drawLines( vVLines );
  painter->drawLines( vHLines );

  painter->setPen( penText );
  const QMatrix& mx = painter->worldMatrix();
  painter->setWorldMatrixEnabled( false );
  for( int c = 0; c < vVLabels_pos.size(); c++ )
    painter->drawText( (QPointF)mx.map( vVLabels_pos[c] ) + vVLabels_offset[c], vVLabels_text[c] );
  for( int c = 0; c < vHLabels_pos.size(); c++ )
    painter->drawText( (QPointF)mx.map( vHLabels_pos[c] ) + vHLabels_offset[c], vHLabels_text[c] );
  painter->setWorldMatrixEnabled( true );
}

void ViewGridItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewGridItem::UpdateGeometry()
{
  prepareGeometryChange();
}

void ViewGridItem::UpdateDependentGeometry()
{
  Point gridCellMax = pPrefs->visual.gridCellMax;
  sceneRect = scene()->sceneRect();

  /* Calculate maximal grid size */
  Point maxPos = _C( sceneRect.bottomRight() );
  Point minPos = _C( sceneRect.topLeft() );

  /* Calculate rounded-up grid size */
  Point size = maxPos - minPos;
  Point cs( pow( 10., floor( log10( size.x ) ) ),
            pow( 10., floor( log10( size.y ) ) ) );

  Point gridCellMax_scaled = gridCellMax / pState->scale / 2.;

  while( cs.x > gridCellMax_scaled.x ) cs.x /= 10;

  if( cs.x * 5 < gridCellMax_scaled.x )         cs.x *= 5;
  else if( cs.x * 2.5 < gridCellMax_scaled.x )  cs.x *= 2.5;

  while( cs.y > gridCellMax_scaled.y ) cs.y /= 10;

  if( cs.y * 5 < gridCellMax_scaled.y )         cs.y *= 5;
  else if( cs.y * 2.5 < gridCellMax_scaled.y )  cs.y *= 2.5;

  if( cs * 1e6 < size )
    return;

  QPolygonF viewPoly = pState->viewPoly;
  if( viewPoly.empty() )
    return;

  vVLines.clear();
  vVLabels_text.clear();
  vVLabels_pos.clear();
  vVLabels_offset.clear();

  vHLines.clear();
  vHLabels_text.clear();
  vHLabels_pos.clear();
  vHLabels_offset.clear();

  // VERTICAL
  vVLines.reserve( (int)((maxPos.x - minPos.x)/cs.x) + 1 );
  double t = cs.x * (floor( minPos.x / cs.x ) + 1.);
  for( ; t < maxPos.x; t += cs.x ) {
    QLineF line( QPointF( t, minPos.y ), QPointF( t, maxPos.y ) );
    vVLines += line;

    QPointF zero( t, 0. );
    if( viewPoly.containsPoint( zero, Qt::OddEvenFill ) ) {
      vVLabels_pos    += zero;
      vVLabels_offset += QPointF( 2., 10. );
      vVLabels_text   += QString( "x'%1" ).arg( t );
      continue;
    }

    QPointF isec;
    for( unsigned i = 0; i < (unsigned)viewPoly.size(); i++ ) {
      QLineF lineP( viewPoly[ (i == 0) ? 3 : (i - 1) ], viewPoly[i] );
      QLineF::IntersectType it = lineP.intersect( line, &isec );
      if( it == QLineF::NoIntersection || it == QLineF::UnboundedIntersection )
        continue;
      switch( i ) {
      case 0: vVLabels_offset += QPointF( 2., -2. ); break;   // Left
      case 1: vVLabels_offset += QPointF( 2., 10. ); break;   // Top
      case 2: vVLabels_offset += QPointF( -40., -2. ); break; // Right
      case 3: vVLabels_offset += QPointF( 2., -5. ); break;  // Bottom
      }
      vVLabels_pos += isec;
      vVLabels_text += QString( "x'%1" ).arg( t );
    }
  }

  // HORIZONTAL
  vHLines.reserve( (int)((maxPos.y - minPos.y)/cs.y) + 1 );
  t = cs.y * (floor( minPos.y / cs.y ) + 1.);
  for( ; t < maxPos.y; t += cs.y ) {
    QLineF line( QPointF( minPos.x, t ), QPointF( maxPos.x, t ) );
    vVLines += line;

    QPointF zero( 0., t );
    if( viewPoly.containsPoint( zero, Qt::OddEvenFill ) ) {
      vVLabels_pos    += zero;
      vVLabels_offset += QPointF( 2., -2. );
      vVLabels_text   += QString( "y'%1" ).arg( t );
      continue;
    }

    QPointF isec;
    for( int i = 3; i > -1; i-- ) {
      QLineF lineP( viewPoly[ (i == 3) ? 0 : (i + 1) ], viewPoly[i] );
      QLineF::IntersectType it = lineP.intersect( line, &isec );
      if( it == QLineF::NoIntersection || it == QLineF::UnboundedIntersection )
        continue;
      switch( i ) {
      case 0: vVLabels_offset += QPointF( 2., 10. ); break;   // Top
      case 1: vVLabels_offset += QPointF( -40., -2. ); break; // Right
      case 2: vVLabels_offset += QPointF( 2., -5. ); break;  // Bottom
      case 3: vVLabels_offset += QPointF( 2., -2. ); break;   // Left
      }
      vVLabels_pos += isec;
      vVLabels_text += QString( "y'%1" ).arg( t );
    }
  }
}

void ViewGridItem::UpdateStyle()
{
  penLine = *pPrefs->visual.pens[ STYLE::GRID ];
  penText = *pPrefs->visual.pens[ STYLE::GRIDTEXT ];
}
