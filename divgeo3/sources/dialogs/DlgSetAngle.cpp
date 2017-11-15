#include "DlgSetAngle.h"

DlgSetAngle::DlgSetAngle( double currentRad, StringsManager* _pSM, Console* _pConsole,
                          QWidget* parent):
  QDialog(parent),
  sender_name( "DlgSetAngle" ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::SETANGLE::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  pLbAngle = new QLabel( SM_DLG( DLG::SETANGLE::LBL::ANGLE ) );
  pLbAngle->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  pLoMain->addWidget( pLbAngle );

  pSbAngle = new DoubleSpinBoxExt /*1409*/();
  pSbAngle->setDecimals( 3 );
  pSbAngle->setRange( -1e7, 1e7 );
  pSbAngle->setSingleStep( 1. );
  pSbAngle->setValue( currentRad*RAD2DEG );
  pLoMain->addWidget( pSbAngle );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnOk = new QPushButton( SM_DLG( DLG::BTN::OK ) );
  pLoButtons->addWidget( pBtnOk );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnOk,      SIGNAL(clicked()), this, SLOT(slotDone()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}

void DlgSetAngle::slotDone()
{
  SENDER_NAME_Q( "Done" );
  if( pSbAngle->value() == MAXDOUBLE ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
    return;
  }
  accept();
}

