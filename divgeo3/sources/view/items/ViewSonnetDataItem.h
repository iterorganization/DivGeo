#ifndef VIEWSONNETDATAITEM_H
#define VIEWSONNETDATAITEM_H

#include "IViewItem.h"

// SHW::MESH
typedef class ViewSonnetDataItem:
  public IViewItem, public QGraphicsItem
{
  QVector< QLineF > lines;
  QRectF fullBoundingRect;
  QPen pen;

public:
  ViewSonnetDataItem( IComponentPtr _pObject ):
    IViewItem( _pObject )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
}* ViewSonnetDataItemPtr;

#endif // VIEWSONNETDATAITEM_H
