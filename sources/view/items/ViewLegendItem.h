#ifndef VIEWLEGENDITEM_H
#define VIEWLEGENDITEM_H

#include "IViewItem.h"
#include <QGraphicsItem>

#include <QPainter>

typedef class ViewLegendItem:
    public IViewItem,
    public QGraphicsItem
{
  QPen pen;

  QVector< double > vLogs;
  int gradient_steps;
  int hue_delta;

  QPixmap pixmap;
  QPointF pixmapPos;

public:
  ViewLegendItem( IComponentPtr _pObject ):
    IViewItem( _pObject )
  { Init(); setZValue( 2. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
}* ViewLegendItemPtr;

#endif // VIEWLEGENDITEM_H
