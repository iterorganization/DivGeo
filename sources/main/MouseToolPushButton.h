#ifndef MOUSETOOLPUSHBUTTON_H
#define MOUSETOOLPUSHBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QFont>

#include <QHBoxLayout>
#include <QLabel>

class MouseToolPushButton : public QPushButton
{
  Q_OBJECT

    QLabel* pLlText;

public:
  explicit MouseToolPushButton( const QIcon& _crIco, QWidget* _pParent = 0):
    QPushButton( _pParent ) {

    setFixedSize( 38, 27 );
    setCheckable( true );
    setChecked( false );

    QHBoxLayout* pLo = new QHBoxLayout();
    pLo->setMargin( 0 );
    pLo->setSpacing( 0 );

    QLabel* pLlIcon = new QLabel();
    pLlIcon->setPixmap( _crIco.pixmap( 24, 24 ) );
    pLlIcon->setMargin( 1 );
    pLlIcon->setIndent( 0 );
    pLo->addWidget( pLlIcon );

    QFont font;
    font.setFamily( this->font().family() );
    font.setPixelSize( 10 );

    pLlText = new QLabel();
    pLlText->setMargin( 0 );
    pLlText->setFont( font );
    pLlText->setIndent( 0 );
    pLo->addWidget( pLlText );

    setLayout( pLo );
  }

  void mouseReleaseEvent( QMouseEvent* _pEv ) {
    switch( _pEv->button() ) {
    case Qt::LeftButton:
      emit leftClicked();
      break;
    case Qt::MiddleButton:
      emit middleClicked();
      break;
    case  Qt::RightButton:
      emit rightClicked();
      break;
    default:
      QPushButton::mouseReleaseEvent( _pEv );
      break;
    }
  }

  void ChangeLetter( QChar _ch, bool _add ) {
    QString sText = pLlText->text();
    if( (sText.isEmpty() && !_add) ||
        (sText.contains( _ch ) == _add ) )
      return;

    bool hasL, hasM, hasR;
    if( _add ) {
      hasL = sText.contains( 'L' ) || (_ch == 'L');
      hasM = sText.contains( 'M' ) || (_ch == 'M');
      hasR = sText.contains( 'R' ) || (_ch == 'R');
    }
    else {
      hasL = sText.contains( 'L' ) && (_ch != 'L');
      hasM = sText.contains( 'M' ) && (_ch != 'M');
      hasR = sText.contains( 'R' ) && (_ch != 'R');
    }

    sText.clear();

    if( hasL ) {
      sText = "L";
      if( hasM || hasR )
        sText += "\n";
    }
    if( hasM ) {
      sText += "M";
      if( hasR )
        sText += "\n";
    }
    if( hasR )
      sText += "R";

    pLlText->setText( sText );
  }
  
signals:
  void leftClicked();
  void middleClicked();
  void rightClicked();
  
public slots:
  
};

#endif // MOUSETOOLPUSHBUTTON_H
