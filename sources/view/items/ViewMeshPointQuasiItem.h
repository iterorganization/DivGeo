#ifndef VIEWMESHPOINTQUASIITEM_H
#define VIEWMESHPOINTQUASIITEM_H

#include "IViewItem.h"

typedef class ViewMeshPointQuasiItem:
  public IViewItem
{
  QPointF center;

public:
  ViewMeshPointQuasiItem( IComponentPtr _pObject ):
    IViewItem( _pObject )
  { Init(); }

  virtual void SetVisible( bool /*visible*/ ) {}
  virtual void UpdateGeometry()
  { UpdateComments();
    MeshPointPtr pMP = dgtype_cast< MeshPointPtr >( pObject );
    center = _C( pMP->Position() );
  }
  virtual void UpdateDependentGeometry() {}
  virtual void UpdateStyle() {}
  virtual void WritePostScriptData( QFile* /*_pFile*/ )
  { /*TODO: PostScript*/ }
  virtual double DistToPoint( QPointF _point ) const
  { return hypot( _point.x() - center.x(), _point.y() - center.y() ); }
  virtual bool InRectangle( const QRect& /*_rect*/, const QTransform& /*_transform*/ ) const
  { return false; }

}* ViewMeshPointQuasiItemPtr;

#endif // VIEWMESHPOINTQUASIITEM_H
