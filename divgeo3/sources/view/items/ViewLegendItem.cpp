#include "ViewLegendItem.h"
#include <QGraphicsScene>
#include "../../model/flux/Equil.h"

QRectF ViewLegendItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewLegendItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setMatrixEnabled( false );
  painter->drawPixmap( pixmapPos, pixmap );
  painter->setMatrixEnabled( true );
}

void ViewLegendItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewLegendItem::UpdateGeometry()
{
  prepareGeometryChange();
  EquilPtr pEquil = dgtype_cast< EquilPtr >( pObject );

  gradient_steps = 9;
  hue_delta = 135/gradient_steps;

  double level_min = fabs( pEquil->MinValue() );
  double level_max = fabs( pEquil->MaxValue() );

  double pow_min = log10( level_min + 1 );
  double pow_max = log10( level_max + 1 );

  double pow_min_delta = pow_min / gradient_steps;
  double pow_max_delta = pow_max / gradient_steps;

  vLogs.clear();
  for( int c = -gradient_steps; c <= gradient_steps; c++ ) {
    if( c < 0 )       vLogs += -pow( 10., -pow_min_delta * c ) + 1;
    else if( c == 0 ) vLogs += 0.;
    else              vLogs += pow( 10., pow_max_delta * c ) - 1;
  }

  pixmap = QPixmap( QSize( 251, 51 ) );
  QPainter painter( &pixmap );

  pen = QPen( Qt::white );

  painter.setPen( pen );
  painter.setFont( QFont( "Arial", 8 ) );

  QRectF rect = pixmap.rect();
  rect.setSize( rect.size() - QSizeF( 1., 1. ) );
  painter.fillRect( rect, Qt::black );
  painter.drawRect( rect );

  QSizeF subRectSize( (rect.width() - 15.) / (vLogs.size() - 1), 10. );
  QRectF subRect( QPointF( 10., 5.), subRectSize );

  for( int c = 0, c_log = 0; c < vLogs.size()-1; c++, c_log++ ) {

    if( vLogs[c_log] == 0. ) {
      painter.save();
      painter.translate( subRect.bottomLeft() + QPointF( 0., 12. ) );
      painter.rotate( 270. );
      painter.drawText( -18., 3., QString( "0.0" ) );
      c_log++;
      painter.restore();
    }

    painter.save();
    QPointF textPos = ( c == c_log ) ? subRect.bottomLeft() : subRect.bottomRight();
    textPos += QPointF( 0., 12. );
    painter.translate( textPos );
    painter.rotate( 270. );
    char pStr[16];
    sprintf( pStr, "%3.3g", vLogs[ c_log ] );
    painter.drawText( -18., 3., pStr );

    painter.restore();

    painter.fillRect( subRect, QColor::fromHsv( c_log * hue_delta, 255, 255 ) );
    painter.drawRect( subRect );

    subRect.moveLeft( 10. + (c+1)*subRectSize.width() );
  }

}

void ViewLegendItem::UpdateDependentGeometry()
{
  fullBoundingRect = scene()->sceneRect();
  double rectMargin = 20.;
  pixmapPos = QPointF( pState->viewRect.right() - pixmap.size().width() - rectMargin,
                       pState->viewRect.bottom() - pixmap.size().height() - rectMargin );
}

void ViewLegendItem::UpdateStyle()
{
  pen = QPen( Qt::white );
}
