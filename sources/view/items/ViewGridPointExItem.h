#ifndef VIEWGRIDPOINTEXITEM_H
#define VIEWGRIDPOINTEXITEM_H

#include "IViewItem.h"

// SHW::GRIDPOINTS
typedef class ViewGridPointExItem:
  public IViewItem, public QGraphicsItem
{
  Point position;
  QLineF line;
  QPen pen;
  bool show;

public:
  ViewGridPointExItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    show( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

}* ViewGridPointExItemPtr;

#endif // VIEWGRIDPOINTEXITEM_H
