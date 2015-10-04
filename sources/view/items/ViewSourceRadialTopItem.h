#ifndef VIEWSOURCERADIALTOPITEM_H
#define VIEWSOURCERADIALTOPITEM_H

#include "IViewItem.h"

// SHW::SOURCES
typedef class ViewSourceRadialTopItem:
  public IViewItem, public QGraphicsItem
{
  QRectF fullBoundingRect;
  QVector< QLineF > lines;

  QPen penLine;

public:
  ViewSourceRadialTopItem( IComponentPtr _pObject ):
    IViewItem( _pObject ) { isRadial = true; Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
}* ViewSourceRadialTopItemPtr;

#endif // VIEWSOURCERADIALTOPITEM_H
