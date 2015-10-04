#ifndef VIEWXPOINTTESTITEM_H
#define VIEWXPOINTTESTITEM_H

#include "IViewItem.h"

// SHW::XPOINTTESTS
typedef class ViewXPointTestItem:
  public IViewItem, public QGraphicsItem
{
  Point center;
  QVector< QLineF > gradLines;
  QLineF boundLines[4];
  Point ps[4];
  QRectF fullBoundingRect;
  const EquilPtr pEquil;
  QPen penGrad, penBound;

public:
  ViewXPointTestItem( IComponentPtr _pObject, const EquilPtr _pEquil ):
    IViewItem( _pObject ),
    pEquil( DG_SHARE( _pEquil ) )
  { Init(); }
  ~ViewXPointTestItem() { ReleaseObject( (IComponentPtr*)&pObject );
                          ReleaseObject( (IComponentPtr*)&pEquil );
                          ReleaseComments(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

}* ViewXPointTestItemPtr;

#endif // VIEWXPOINTTESTITEM_H
