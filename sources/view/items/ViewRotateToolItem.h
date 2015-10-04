#ifndef VIEWROTATETOOLITEM_H
#define VIEWROTATETOOLITEM_H

#include "IViewItem.h"

typedef class ViewRotateToolItem:
  public IViewItem, public QGraphicsItem
{
  double angle;
  QPointF center;
  QRectF rect;
  QPen pen;

public:
  ViewRotateToolItem():
    IViewItem( null )
  { Init(); setZValue( 1. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
}* ViewRotateToolItemPtr;

#endif // VIEWROTATETOOLITEM_H
