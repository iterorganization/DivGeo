#include "ViewEquilItem.h"

ViewEquilItem::ViewEquilItem(const IComponentPtr _pObject, NPoint _blockSize ):
  IViewItem( _pObject )
{
  setZValue( -2. );

  EquilPtr pEquil = dgtype_cast< EquilPtr >( _pObject );
  NPoint equilSize = pEquil->Size();
  NPoint dimension = equilSize / _blockSize;
  NPoint restoredSize = dimension * _blockSize;
  if( equilSize.x > restoredSize.x )
    ++dimension.x;
  if( equilSize.y > restoredSize.y )
    ++dimension.y;

  NPoint index;
  for( index.y = 0; index.y < dimension.y; index.y++ ) {
    for( index.x = 0; index.x < dimension.x; index.x++ ) {
      NPoint begin = index * _blockSize;
      NPoint end = begin + _blockSize;
      if( equilSize.x < end.x )
        end.x = equilSize.x;
      if( equilSize.y < end.y )
        end.y = equilSize.y;

      ViewEquilBlockItemPtr pBlock = new ViewEquilBlockItem( _pObject, begin, end );
      pBlock->SetSettingsManager( pPrefs );
      blocks.push_back( pBlock );
      addToGroup( pBlock );
    }
  }
}

void ViewEquilItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewEquilItem::UpdateGeometry()
{
  foreach( ViewEquilBlockItemPtr block, blocks )
    block->UpdateGeometry();

  UpdateComments();
}

void ViewEquilItem::UpdateDependentGeometry()
{
  /* No dependent geometry */
}

void ViewEquilItem::UpdateStyle()
{
  foreach( ViewEquilBlockItemPtr block, blocks ) {
    block->SetViewState( pState );
    block->UpdateStyle();
  }
}


QRectF ViewEquilBlockItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewEquilBlockItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  int prevValueSign = +1.;
  painter->setPen( pen_pos );

  foreach( const Pair& pair, lines ) {

    if( !pState->coloredEquil ) {
      if( pair.value >= 0. ) {
        if( prevValueSign < 0 ) {
          painter->setPen( pen_pos );
          prevValueSign = +1;
        }
      }
      else {
        if( prevValueSign > 0 ) {
          painter->setPen( pen_neg );
          prevValueSign = -1;
        }
      }
    }
    else {
      int hue = 255;
      if( pair.value < 0. ) {
        for( int c = vLogs.size()/2; c >= 0; c-- ) {
          if( pair.value > vLogs[c] ) {
            hue = c * hue_delta;
            break;
          }
        }
      }
      else {
        for( int c = vLogs.size()/2; c < vLogs.size(); c++ ) {
          if( pair.value < vLogs[c] ) {
            hue = c * hue_delta;
            break;
          }
        }
      }
      QPen pen( QColor::fromHsv( hue, 255, 128 ), 1 );
      pen.setCosmetic( true );
      painter->setPen( pen );
    }

    painter->drawLine( pair.line );
  }
}

void ViewEquilBlockItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewEquilBlockItem::UpdateGeometry()
{
  EquilPtr pEquil = dgtype_cast< EquilPtr >( pObject );

  lines.clear();

  NPoint size = pEquil->Size();
  fullBoundingRect = QRectF( _C( pEquil->GetPoint( begin ) - boundOffset ), _C( pEquil->GetPoint( end - 1 ) + boundOffset2 ) );
  NPoint index;
  QPointF cache_x;
  for( index.y = begin.y /*0*/; index.y < end.y /*size.y*/ /*-1*/; index.y++ ) {
    index.x = begin.x /*0*/;
    cache_x = _C( pEquil->GetPoint( index ) );
    for( ; index.x < end.x /*size.x*/ /*-1*/; index.x++ ) {
      QPointF p1 = cache_x;
      //QPointF p1t = _C( pEquil->GetPoint( index ) );
      if( index.x < size.x - 1 ) {
        cache_x = _C( pEquil->GetPoint( index + NPoint( 1, 0 ) ) );
        lines.push_back( Pair( QLineF( p1, cache_x ), pEquil->Cell( index ) ) );
      }
      if( index.y < size.y - 1 )
        lines.push_back( Pair( QLineF( p1, _C( pEquil->GetPoint( index + NPoint( 0, 1 ) ) ) ), pEquil->Cell( index ) ) );
    }
  }

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

  UpdateComments();
}

void ViewEquilBlockItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewEquilBlockItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  if( isHighlighted )
    pen_pos = pen_neg = *pPrefs->visual.pens[ STYLE::HIGHLIGHT ];
  else {
    pen_pos = *pPrefs->visual.pens[ STYLE::EQUIL_POS ];
    pen_neg = *pPrefs->visual.pens[ STYLE::EQUIL_NEG ];
  }
}
