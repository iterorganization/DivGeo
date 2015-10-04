#include "DlgGlueElems.h"

DlgGlueElems::DlgGlueElems( StringsManager* _pSM, Console* _pConsole,
                            QWidget *parent ):
  QDialog(parent),
  sender_name( "DlgGlueElems" ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::GLUEELEMS::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  /* Main controls */
  QGridLayout* pLoGrid = new QGridLayout( this );
  pLoMain->addLayout( pLoGrid );

  pLbMaxDev = new QLabel( SM_DLG( DLG::GLUEELEMS::LBL::TRESHOLD ) );
  pLbMaxDev->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbMaxDev, 0, 0, 1, 1 );

  pSbMaxDev = new QDoubleSpinBox();
  pSbMaxDev->setDecimals( 5 );
  pSbMaxDev->setRange( 1e-7, 1e7 );
  pSbMaxDev->setSingleStep( 1. );
  pSbMaxDev->setValue( 3. );
  pLoGrid->addWidget( pSbMaxDev, 0, 1, 1, 1 );

  pLbMM1 = new QLabel( SM_DLG( DLG::GLUEELEMS::LBL::MILLIMETERS ) );
  pLbMM1->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbMM1, 0, 2, 1, 1 );

  pLbLenLimit = new QLabel( SM_DLG( DLG::GLUEELEMS::LBL::MAX_LEN ) );
  pLbLenLimit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbLenLimit, 1, 0, 1, 1 );

  pSbLenLimit= new QDoubleSpinBox();
  pSbLenLimit->setDecimals( 5 );
  pSbLenLimit->setRange( 1e-7, 1e7 );
  pSbLenLimit->setSingleStep( 1. );
  pSbLenLimit->setValue( 10000. );
  pLoGrid->addWidget( pSbLenLimit, 1, 1, 1, 1 );

  pLbMM2 = new QLabel( SM_DLG( DLG::GLUEELEMS::LBL::MILLIMETERS ) );
  pLbMM2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbMM2, 1, 2, 1, 1 );

  pChSplitLonger = new QCheckBox( SM_DLG( DLG::GLUEELEMS::LBL::CUT_LONGER ) );
  pLoMain->addWidget( pChSplitLonger );

  pChMarked = new QCheckBox( SM_DLG( DLG::GLUEELEMS::LBL::MARKED_ONLY ) );
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

void DlgGlueElems::slotDone()
{
  SENDER_NAME_Q( "Done" );
  if( pSbMaxDev->value() == MAXDOUBLE ||
      pSbMaxDev->value() <= 0. ) {
    pConsole->Send( WND_ERROR, SENDER, SM_DLG( DLG::GLUEELEMS::ERR::BAD_MAX_DIST ) );
    return;
  }
  if( pSbLenLimit->value() == MAXDOUBLE ||
      pSbLenLimit->value() <= 0. ) {
    pConsole->Send( WND_ERROR, SENDER, SM_DLG( DLG::GLUEELEMS::ERR::BAD_MAX_LEN ) );
    return;
  }
  accept();
}
