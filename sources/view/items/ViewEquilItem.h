#ifndef VIEWEQUILITEM_H
#define VIEWEQUILITEM_H

#include "IViewItem.h"

#include "QRgb"

typedef class ViewEquilBlockItem:
  public IViewItem, public QGraphicsItem
{
  struct Pair {
    QLineF line;
    double value;
    Pair( QLineF _line, double _value ):
      line( _line ), value( _value ) { }
    Pair(): line( QLineF() ), value( 0. ) { }
  };

  QRectF boundingRectFull;
  QVector< Pair > lines;
  QPen pen_pos, pen_neg;
  NPoint begin, end;
  QVector< double > vLogs;
  int gradient_steps;
  int hue_delta;

public:
  ViewEquilBlockItem( IComponentPtr _pObject, NPoint _begin, NPoint _end ):
    IViewItem( _pObject ),
    begin( _begin ),
    end( _end )
  { Init(); setZValue( -2. ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual QRectF boundingRect() const;
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

}* ViewEquilBlockItemPtr;


// SHW::EQUIL
typedef class ViewEquilItem:
  public QGraphicsItemGroup,
  public IViewItem
{
  QVector< ViewEquilBlockItemPtr > blocks;

public:
  ViewEquilItem( IComponentPtr _pObject, NPoint _blockSize );
  ~ViewEquilItem() { foreach( ViewEquilBlockItemPtr pBlock, blocks ) delete pBlock; ReleaseObject( &pObject ); }

  virtual void SetVisible( bool visible ) { setVisible( visible ); }

  virtual void WritePostScriptData( QFile* _pFile );

  virtual void UpdateGeometry();
  virtual void UpdateDependentGeometry();
  virtual void UpdateStyle();

  /*overload*/ void SetSettingsManager( const UserPreferences* _pPrefs ) {
    pPrefs = _pPrefs;
    foreach( ViewEquilBlockItemPtr pBlock, blocks )
      pBlock->SetSettingsManager( pPrefs );
  }

}* ViewEquilItemPtr;


#endif // VIEWEQUILITEM_H
