#ifndef VIEWTEMPLATEITEM_H
#define VIEWTEMPLATEITEM_H

#include "IViewItem.h"

// SHW::TEMPLATE
typedef class ViewTemplateItem:
  public IViewItem, public QGraphicsItem
{
  QRectF fullBoundingRect;
  QVector< QLineF > lines;
  QPen pen;

public:
  ViewTemplateItem( IComponentPtr _pObject ):
    IViewItem( _pObject )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewTemplateItemPtr;

#endif // VIEWTEMPLATEITEM_H
