#ifndef VIEWCOMMENTITEM_H
#define VIEWCOMMENTITEM_H

#include "IViewItem.h"

// SHW::LABELS
typedef class ViewCommentItem:
  public IViewItem, public QGraphicsItem
{
  IComponentPtr pLinkedObject;
  Point position;
  Point pos_end;
  QString sText;

  QFont font;
  QRectF fullBoundingRect;
  QRectF textRect;
  QPolygonF poly;
  QPen pen;

public:
  ViewCommentItem( IComponentPtr _pCommentObject, IComponentPtr _pLinkedObject ):
    IViewItem( _pCommentObject ),
    pLinkedObject( DG_SHARE( _pLinkedObject ) )
  { Init(); }

  virtual ~ViewCommentItem() {
    ReleaseObject( &pObject );
    ReleaseComments();
  }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;

  void SetLinkedObject( IComponentPtr _pLinkedObject );
  IComponentPtr GetLinkedObject() const { return pLinkedObject; }
  void UpdateEndPosition() {
    if( pLinkedObject != null ) {
      CommentPtr pComment = dgtype_cast< CommentPtr >( pObject );
      Point pos_end = pLinkedObject->CentralPoint();
      pComment->ChangePosEnd( pos_end );
    }
  }

}* ViewCommentItemPtr;

typedef QList< ViewCommentItemPtr > ViewCommentsList;

#endif // VIEWCOMMENTITEM_H
