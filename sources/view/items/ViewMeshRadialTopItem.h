#ifndef VIEWMESHRADIALTOPITEM_H
#define VIEWMESHRADIALTOPITEM_H

#include "IViewItem.h"

// SHW::MESH
typedef class ViewMeshRadialTopItem:
  public IViewItem, public QGraphicsItem
{
  QList< QLineF > lines;
  QPen penLine;
  QRectF fullBoundingRect;

public:
  ViewMeshRadialTopItem( IComponentPtr _pObject ):
    IViewItem( _pObject ) { isRadial = true; Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewMeshRadialTopItemPtr;

#endif // VIEWMESHRADIALTOPITEM_H
