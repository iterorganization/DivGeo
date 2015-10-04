#ifndef VIEWELEMENTITEM_H
#define VIEWELEMENTITEM_H

#include "IViewItem.h"

// SHW::ELEMS | SHW::NUMBERS | SHW::NORMALS
typedef class ViewElementItem:
  public IViewItem, public QGraphicsItem
{
  QRectF textBoundingRect;
  QLineF line;
  QPointF pos, normalEnd;
  QString number;
  QPen penNormal, penMark, penLine, penNumber;
  bool isMarked, showNumbers, showNormals;
  double normalLength;

public:
  ViewElementItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false ),
    showNumbers( false ),
    showNormals( false ),
    normalLength( 0. )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;
  virtual bool InRectangle( const QRect& _rect, const QTransform& _transform ) const;

  virtual void SetFlag( ParamID id, bool enable )
  { switch( id ) {
    case p_showNormals: showNormals = enable; break;
    case p_showNumbers: showNumbers = enable; break;
    default: break;
  } }

}* ViewElementItemPtr;

#endif // VIEWELEMENTITEM_H
