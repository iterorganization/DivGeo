#ifndef VIEWNODEITEM_H
#define VIEWNODEITEM_H

#include "IViewItem.h"

// SHW::NODES
typedef class ViewNodeItem:
  public IViewItem, public QGraphicsItem
{
  QRectF nodeBoundingRect;
  Point nodeRadius;
  QPointF center;
  QPen pen;
  bool drawRect, show;
  bool showAll, showIrregular;

public:
  ViewNodeItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    drawRect( false ),
    show( false ),
    showAll( false ),
    showIrregular( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

  virtual void SetFlag( ParamID id, bool enable ) {
    switch( id ) {
    case p_showIrregular: showIrregular = enable; break;
    case p_showAll:       showAll       = enable; break;
    default: break;
  } }

}* ViewNodeItemPtr;

#endif // VIEWNODEITEM_H
