#ifndef VIEWMESHELEMENTITEM_H
#define VIEWMESHELEMENTITEM_H

#include "IViewItem.h"

// SHW::MESH
typedef class ViewMeshElementItem:
  public IViewItem, public QGraphicsItem
{
  QLineF line;
  QPen penMark, penLine;
  bool isMarked, isError;

public:
  ViewMeshElementItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false ),
    isError( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;
  virtual bool InRectangle( const QRect& _rect, const QTransform& _transform ) const;

  virtual void SetFlag( ParamID id, bool enable )
  { switch( id ) {
    case p_showError: isError = enable; break;
    default: break;
  } }

}* ViewMeshElementItemPtr;

#endif // VIEWMESHELEMENTITEM_H
