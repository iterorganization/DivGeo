#ifndef VIEWSURFACEEXITEM_H
#define VIEWSURFACEEXITEM_H

#include "IViewItem.h"

// SHW::SURFACES
typedef class ViewSurfaceExItem:
  public IViewItem, public QGraphicsItem
{
  QVector< QLineF > lines;
  QPen pen;
  QRectF fullBoundingRect;
  bool showError;

public:
  ViewSurfaceExItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    showError( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

  virtual void SetFlag( ParamID id, bool enable )
  { switch( id ) {
    case p_showError: showError = enable; break;
    default: break;
  } }

}* ViewSurfaceExItemPtr;

#endif // VIEWSURFACEEXITEM_H
