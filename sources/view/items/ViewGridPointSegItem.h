#ifndef VIEWGRIDPOINTSEGITEM_H
#define VIEWGRIDPOINTSEGITEM_H

#include "IViewItem.h"

// SHW::GRIDPOINTS
typedef class ViewGridPointSegItem:
  public IViewItem, public QGraphicsItem
{
  QLineF line1;
  QLineF line2;
  QLineF lineT;
  QString text;

  QFont font;
  QRectF fullBoundingRect;
  QPen penLine, penText;

public:
  ViewGridPointSegItem( IComponentPtr _pObject ):
    IViewItem( _pObject )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewGridPointSegItemPtr;

#endif // VIEWGRIDPOINTSEGITEM_H
