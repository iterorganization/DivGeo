#ifndef VIEWXPOINTSEGITEM_H
#define VIEWXPOINTSEGITEM_H

#include "IViewItem.h"
#include "../../model/flux/Topology.h"

// SHW::GRIDPOINTS | SHW::SURFACES
typedef class ViewXPointSegItem:
  public IViewItem, public QGraphicsItem
{
  QVector< QLineF > polyLines;
  QLineF lineEnd;
  QLineF lineBegin;
  QRectF fullBoundingRect;
  TopologyPtr pTopology;
  QPen penPoly, penEnd;
  bool showEnds;
  double separatrixSegEndLen;

public:
  ViewXPointSegItem( IComponentPtr _pObject, TopologyPtr _pTopology ):
    IViewItem( _pObject ),
    pTopology( _pTopology ),
    showEnds( false ),
    separatrixSegEndLen( 0. )
  { Init(); }
  ~ViewXPointSegItem() { ReleaseObject( (IComponentPtr*)&pObject );
                         ReleaseComments(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

}* ViewXPointSegItemPtr;

#endif // VIEWXPOINTSEGITEM_H
