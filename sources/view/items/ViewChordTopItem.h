#ifndef VIEWCHORDTOPITEM_H
#define VIEWCHORDTOPITEM_H

#include "IViewItem.h"

// SHW::CHORDS | SHW::CHORDS3D | SHW::TOPVIEW
typedef class ViewChordTopItem:
  public IViewItem, public QGraphicsItem
{
  Point p1, p2;

  QLineF topLine;
  QPen penLine;

public:
  ViewChordTopItem( IComponentPtr _pObject ):
    IViewItem( _pObject ) { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

}* ViewChordTopItemPtr;

#endif // VIEWCHORDTOPITEM_H
