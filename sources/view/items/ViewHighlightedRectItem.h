#ifndef VIEWHIGHLIGHTEDRECTITEM_H
#define VIEWHIGHLIGHTEDRECTITEM_H

#include "IViewItem.h"

typedef class ViewHighlightedRectItem:
  public IViewItem, public QGraphicsItem
{
  QPolygonF rect;
  QPen pen;

public:
  ViewHighlightedRectItem():
    IViewItem( null )
  { Init(); setZValue( 1. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

  QPolygonF Rect() { return rect; }
  void SetRect( const QPolygonF& _rect ) { rect = _rect; }
}* ViewHighlightedRectItemPtr;

#endif // VIEWHIGHLIGHTEDRECTITEM_H
