#include "DlgGluePoints.h"

DlgGluePoints::DlgGluePoints( StringsManager* _pSM, Console* _pConsole,
                              QWidget *parent):
  QDialog(parent),
  sender_name( "DlgGluePoints" ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::GLUEPOINTS::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  /* Main controls */
  QHBoxLayout* pLoFirst = new QHBoxLayout( this );
  pLoMain->addLayout( pLoFirst );

  pLbDist = new QLabel( SM_DLG( DLG::GLUEPOINTS::LBL::MAX_DIST ) );
  pLbDist->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoFirst->addWidget( pLbDist );

  pSbDist = new QDoubleSpinBox();
  pSbDist->setDecimals( 5 );
  pSbDist->setRange( 1e-7, 1e7 );
  pSbDist->setSingleStep( 1. );
  pLoFirst->addWidget( pSbDist );

  pLbMM = new QLabel( SM_DLG( DLG::GLUEPOINTS::LBL::MILLIMETERS ) );
  pLbMM->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoFirst->addWidget( pLbMM );

  pChMarked = new QCheckBox( SM_DLG( DLG::GLUEPOINTS::LBL::MARKED_ONLY ) );
  pLoMain->addWidget( pChMarked );

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

void DlgGluePoints::slotDone()
{
  SENDER_NAME_Q( "Done" );
  if( pSbDist->value() == MAXDOUBLE ||
      pSbDist->value() <= 0. ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::GLUEPOINTS::ERR::BAD_MAX_DIST );
    return;
  }
  accept();
}
