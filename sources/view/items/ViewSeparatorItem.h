#ifndef VIEWSEPARATORITEM_H
#define VIEWSEPARATORITEM_H

#include "IViewItem.h"

// SHW::SEPARATOR | SHW::NUMBERS
typedef class ViewSeparatorItem:
  public IViewItem, public QGraphicsItem
{
  QLineF line;
  QString number;
  QPointF pos;
  QRectF fullBoundingRect;
  QPen penMark, penLine, penNumber;
  QFont font;
  bool isMarked, showNumbers;

public:
  ViewSeparatorItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false ),
    showNumbers( false )
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

  virtual void SetFlag( ParamID id, bool enable ) {
    switch( id ) {
    case p_showNumbers: showNumbers = enable; break;
    default: break;
  } }

}* ViewSeparatorItemPtr;

#endif // VIEWSEPARATORITEM_H
