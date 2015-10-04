#ifndef RESIZABLEWIDGET_H
#define RESIZABLEWIDGET_H

#include <QWidget>

class ResizableWidget : public QWidget
{
  Q_OBJECT

  QSize custom_size;
  bool hasCustomWidth;
  bool hasCustomHeight;

public:
  explicit ResizableWidget(QWidget *parent = 0):
    QWidget( parent ),
    hasCustomWidth( false ),
    hasCustomHeight( false ) { }

  virtual QSize sizeHint() const
  {
    QSize sh = QWidget::sizeHint();
    if( hasCustomHeight )
      sh.setHeight( custom_size.height() );
    if( hasCustomWidth )
      sh.setWidth( custom_size.width() );
    return sh;
  }

  void SetCustomHeight( int h )
  { custom_size.setHeight( h ); hasCustomHeight = true; }

  void SetCustomWidth( int w )
  { custom_size.setWidth( w ); hasCustomWidth = true; }

  void SetCustomSize( const QSize& s )
  { custom_size = s; hasCustomWidth = hasCustomHeight = true; }

  void SetCustomSize( int w, int h )
  { custom_size = QSize( w, h ); hasCustomWidth = hasCustomHeight = true; }

  void SetCustomHeightEnabled( bool en )
  { hasCustomHeight = en; }

  void SetCustomWidthEnabled( bool en )
  { hasCustomWidth = en; }

  void SetCustomSizeEnabled( bool en )
  { hasCustomHeight = hasCustomWidth = en; }
  
signals:
  
public slots:
  
};

#endif // RESIZABLEWIDGET_H
