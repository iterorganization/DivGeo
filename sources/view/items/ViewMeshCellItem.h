#ifndef VIEWMESHCELLITEM_H
#define VIEWMESHCELLITEM_H

#include "IViewItem.h"

// SHW::MESH | SHW::MESHDETAILS
typedef class ViewMeshCellItem:
  public IViewItem, public QGraphicsItem
{
  QRectF markBoundingRect;
  QPointF center;
  QPen penMark, penLine;
  bool isMarked, isError;
  double centerLength;

public:
  ViewMeshCellItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false ),
    isError( false ),
    centerLength( 0. )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
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

}* ViewMeshCellItemPtr;

#endif // VIEWMESHCELLITEM_H
