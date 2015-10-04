#include "ViewSonnetDataItem.h"

QRectF ViewSonnetDataItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewSonnetDataItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  painter->setPen( pen );
  painter->drawLines( lines );
}

void ViewSonnetDataItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewSonnetDataItem::UpdateGeometry()
{
  SonnetDataPtr pSD = dgtype_cast< SonnetDataPtr >( pObject );

  lines.clear();

  BOUND_DECLARE;
  bool firstIteration = true;

  for( int i = 0; i < pSD->Size().y; i++ ) {
    for( int j = 0; j < pSD->Size().x; j++ ) {
      int pos = (i * pSD->Size().x + j) * 4;
      Point p0 = _C( pSD->GetPoint( pos ) );
      Point p1 = _C( pSD->GetPoint( pos + 1 ) );
      Point p2 = _C( pSD->GetPoint( pos + 2 ) );
      Point p3 = _C( pSD->GetPoint( pos + 3 ) );

      lines.push_back( QLineFromPoints( p0, p1 ) );
      lines.push_back( QLineFromPoints( p0, p2 ) );
      lines.push_back( QLineFromPoints( p3, p1 ) );
      lines.push_back( QLineFromPoints( p3, p2 ) );

      if( firstIteration ) {
        BOUND_INIT( p0 );
        firstIteration = false;
      }
      else
        BOUND_POINT( p0 );

      BOUND_POINT( p1 );
      BOUND_POINT( p2 );
      BOUND_POINT( p3 );
    }
  }
  BOUND_RECT( fullBoundingRect );

  UpdateComments();
}

void ViewSonnetDataItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewSonnetDataItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  pen = *pPrefs->visual.pens[ STYLE::SONNETDATA ];
}
