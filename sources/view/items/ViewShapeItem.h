#ifndef VIEWSHAPEITEM_H
#define VIEWSHAPEITEM_H

#include "IViewItem.h"

typedef class ViewShapeItem:
  public IViewItem, public QGraphicsItem
{
public:
  enum Shape {
    LINE,
    RECT,
    POINT
  };

private:
  Shape shape;
  Point p1, p2;
  QPen pen;
  QRectF fullBoundingRect;
  Point nodeRadius;

public:
  ViewShapeItem( Shape _shape, const Point& _p1, const Point& _p2 = Point() ):
    IViewItem( null ),
    shape( _shape ),
    p1( _p1 ),
    p2( _p2 )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewShapeItemPtr;

typedef QList< ViewShapeItemPtr > ViewShapesList;

#endif // VIEWSHAPEITEM_H
