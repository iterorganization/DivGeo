#ifndef VIEWCHORDITEM_H
#define VIEWCHORDITEM_H

#include "IViewItem.h"

// SHW::CHORDS | SHW::CHORDS3D | SHW::NORMALS
typedef class ViewChordItem:
  public IViewItem, public QGraphicsItem
{
  Point p1, p2;
  QLineF line;
  QList< QLineF > lines3D;
  QPointF pos, normalEnd;
  QRectF boundingRect3D;
  QPen penNormal, penLine, penMark;
  bool isMarked, showNormals;
  double normalLength;
  bool showXProjection;
  bool showZProjection;

public:
  ViewChordItem( IComponentPtr _pObject ):
    IViewItem( _pObject ),
    isMarked( false ),
    showNormals( false ),
    normalLength( 0. ),
    showXProjection( false ),
    showZProjection( false )
  { Init(); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* );
  virtual void WritePostScriptData( QFile* );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();
  virtual double DistToPoint( QPointF _point ) const;
  virtual bool InRectangle( const QRect& _rect, const QTransform& _transform ) const;

  virtual void SetFlag( ParamID id, bool enable )
  { switch( id ) {
    case p_showNormals:     showNormals = enable; break;
    case p_showXProjection: showXProjection = enable; break;
    case p_showZProjection: showZProjection = enable; break;
    default: break;
  } }


private:
  void Build3DGeometry();
}* ViewChordItemPtr;

#endif // VIEWCHORDITEM_H
