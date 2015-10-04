#ifndef VIEWGRIDITEM_H
#define VIEWGRIDITEM_H

#include "IViewItem.h"

// SHW::GRID
typedef class ViewGridItem:
  public IViewItem, public QGraphicsItem
{
  QPen penLine, penText;
  QFont font;
  QRectF sceneRect;

  QVector< QLineF > vHLines, vVLines;
  QVector< QPointF > vHLabels_pos, vVLabels_pos;
  QVector< QPointF > vHLabels_offset, vVLabels_offset;
  QVector< QString > vHLabels_text, vVLabels_text;

public:
  ViewGridItem():
    IViewItem( null )
  { Init(); font = QFont( "Arial", 10 ); setZValue( -1. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewGridItemPtr;

#endif // VIEWGRIDITEM_H
