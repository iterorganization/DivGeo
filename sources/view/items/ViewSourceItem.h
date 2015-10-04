#ifndef VIEWSOURCEITEM_H
#define VIEWSOURCEITEM_H

#include "IViewItem.h"

// SHW::SOURCES
typedef class ViewSourceItem:
  public IViewItem, public QGraphicsItem
{
  Point position;
  QRectF fullBoundingRect;
  QRectF markRect;
  QLineF lines[3];

  QPen penLine, penMark;
  bool isMarked;

public:
  ViewSourceItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;
  virtual bool InRectangle( const QRect& _rect, const QTransform& _transform ) const;
}* ViewSourceItemPtr;

#endif // VIEWSOURCEITEM_H
