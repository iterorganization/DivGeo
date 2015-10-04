#ifndef DLGPROGRESS_H
#define DLGPROGRESS_H

#include <QDialog>
#include <QList>
#include <QString>

#include <QLayout>
#include <QLabel>
#include <QProgressBar>

class DlgProgress: public QDialog
{
  QList< QLabel* > labels;
  QList< QProgressBar* > bars;

  QVBoxLayout* pLoMain;

  int level; // 0 - no progresses

public:
  DlgProgress( const QString& _crsWndTitle, QWidget* _pParent ):
    QDialog( _pParent ),
    level( 0 )
  {
    setWindowTitle( _crsWndTitle );
    setModal( true );
    setWindowModality( Qt::ApplicationModal );
    pLoMain = new QVBoxLayout();
    setLayout( pLoMain );
  }

  bool isInProcess() const { return level != 0; }

  QString LastProcessText() const {
    return (level == 0) ? "" : labels[level-1]->text();
  }

  void start( const QString& _crsTitle, int _maxValue = 100 ) {
    QLabel* pLabel;
    QProgressBar* pBar;
    if( labels.count() <= level ) {
      pLabel = new QLabel( _crsTitle );
      labels.push_back( pLabel );
      pLoMain->addWidget( pLabel );
      pBar = new QProgressBar();
      pBar->setMinimum( 0 );
      bars.push_back( pBar );
      pLoMain->addWidget( pBar );
    }
    else {
      pLabel = labels[ level ];
      pBar = bars[ level ];
    }
    pLabel->setText( _crsTitle );
    pBar->setMaximum( _maxValue );
    pBar->setValue( 0 );
    pLabel->show();
    pBar->show();
    level++;
    show();
  }

  void finish() {
    level--;
    QLabel* pLabel = labels[ level ];
    pLabel->hide();
    QProgressBar* pBar = bars[ level ];
    pBar->hide();
    pBar->setValue( 0 );
    adjustSize();
    if( level == 0 )
      hide();
  }

  void progress( int _value ) {
    if( level < 1 )
      return;
      //start( "Please, wait..." );
    QProgressBar* pBar = bars[ level-1 ];
    if( _value >= pBar->maximum() )
      finish();
    else
      pBar->setValue( _value );
  }
};

#endif // DLGPROGRESS_H
