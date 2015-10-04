#ifndef VIEWELEMENTRADIALTOPITEM_H
#define VIEWELEMENTRADIALTOPITEM_H

#include "IViewItem.h"

// SHW::ELEMS
typedef class ViewElementRadialTopItem:
  public IViewItem, public QGraphicsItem
{
  QLineF topLine; //UNUSED
  QList< QLineF > lines;
  QPen penLine;
  QRectF fullBoundingRect;

public:
  ViewElementRadialTopItem( IComponentPtr _pObject ):
    IViewItem( _pObject ) { isRadial = true; Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewElementRadialTopItemPtr;

#endif // VIEWELEMENTRADIALTOPITEM_H
