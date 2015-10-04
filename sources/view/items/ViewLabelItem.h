#ifndef VIEWLABELITEM_H
#define VIEWLABELITEM_H

#include "IViewItem.h"

// SHW::LABELS
typedef class ViewLabelItem:
  public IViewItem, public QGraphicsItem
{
  Point pos;
  Point offset;
  QString text;

  QFont font;
  QRectF fullBoundingRect;
  QPen pen;

public:
  ViewLabelItem( IComponentPtr _pObject, const QString& _text ):
    IViewItem( _pObject ),
    text( _text )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewLabelItemPtr;

typedef QList< ViewLabelItemPtr > ViewLabelsList;

#endif // VIEWLABELITEM_H
