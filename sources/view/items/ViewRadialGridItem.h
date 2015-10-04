#ifndef VIEWRADIALGRIDITEM_H
#define VIEWRADIALGRIDITEM_H

#include "IViewItem.h"

// SHW::GRID
typedef class ViewRadialGridItem:
  public IViewItem, public QGraphicsItem
{
  QPen penLine, penText;
  QFont font;
  QRectF sceneRect;
  Point gridCellMax;

public:
  ViewRadialGridItem():
    IViewItem( null )
  { Init(); font = QFont( "Arial", 10 ); setZValue( -1. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
}* ViewRadialGridItemPtr;

#endif // VIEWRADIALGRIDITEM_H
