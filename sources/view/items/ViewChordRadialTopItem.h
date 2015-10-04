#ifndef VIEWCHORDRADIALTOPITEM_H
#define VIEWCHORDRADIALTOPITEM_H

#include "IViewItem.h"

// SHW::CHORDS | SHW::CHORDS3D | SHW::TOPVIEW
typedef class ViewChordRadialTopItem:
  public IViewItem, public QGraphicsItem
{
  Point p1, p2;

  QList< QLineF > lines;
  QPen penLine;

public:
  ViewChordRadialTopItem( IComponentPtr _pObject ):
    IViewItem( _pObject ) { isRadial = true; Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

}* ViewChordRadialTopItemPtr;

#endif // VIEWCHORDRADIALTOPITEM_H
