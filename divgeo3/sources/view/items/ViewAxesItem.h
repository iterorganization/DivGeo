#ifndef VIEWAXESITEM_H
#define VIEWAXESITEM_H

#include "IViewItem.h"

// SHW::AXES
typedef class ViewAxesItem:
 public IViewItem, public QGraphicsItem
{
 QLineF aLines[3];
 QPen pen;
 bool abscissa;

public:
 ViewAxesItem( bool _abscissa ):
   IViewItem( null ),
   abscissa( _abscissa )
 { Init(); setZValue( -0.5 ); }

 virtual void SetVisible( bool visible ) { setVisible( visible ); }

 virtual QRectF boundingRect() const;
 virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
 virtual void WritePostScriptData( QFile* );

 virtual void UpdateGeometry();
 virtual void UpdateDependentGeometry();
 virtual void UpdateStyle();

}* ViewAxesItemPtr;

#endif // VIEWAXESITEM_H
