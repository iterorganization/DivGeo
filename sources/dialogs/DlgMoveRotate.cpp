#include "DlgMoveRotate.h"

DlgMoveRotate::DlgMoveRotate( StringsManager* _pSM, Console* _pConsole,
                              QWidget *parent) :
  QDialog(parent),
  sender_name( "DlgMoveRotate" ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::ROTMOVE::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QGridLayout* pLoGrid = new QGridLayout( this );
  pLoMain->addLayout( pLoGrid );

  pSbX = new DoubleSpinBoxExt /*1409*/();
  pSbX->setDecimals( 5 );
  pSbX->setRange( -1e7, 1e7 );
  pSbX->setSingleStep( 1. );
  pLoGrid->addWidget( pSbX, 0, 1, 1, 1 );
  pLbX = new QLabel( SM_DLG( DLG::ROTMOVE::LBL::X ) );
  pLbX->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbX, 0, 0, 1, 1 );

  pSbY = new DoubleSpinBoxExt /*1409*/();
  pSbY->setDecimals( 5 );
  pSbY->setRange( -1e7, 1e7 );
  pSbY->setSingleStep( 1. );
  pLoGrid->addWidget( pSbY, 1, 1, 1, 1 );
  pLbY = new QLabel( SM_DLG( DLG::ROTMOVE::LBL::Y ) );
  pLbY->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbY, 1, 0, 1, 1 );

  pSbScale = new DoubleSpinBoxExt /*1409*/();
  pSbScale->setDecimals( 5 );
  pSbScale->setRange( 1e-5, 1e7 );
  pSbScale->setValue( 1. );
  pSbScale->setSingleStep( 1. );
  pLoGrid->addWidget( pSbScale, 2, 1, 1, 1 );
  pLbScale = new QLabel( SM_DLG( DLG::ROTMOVE::LBL::SCALE ) );
  pLbScale->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbScale, 2, 0, 1, 1 );

  QSpacerItem* pSpByPoint = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoGrid->addItem( pSpByPoint, 3, 0, 1, 2 );

  /* Checkboxes layer */
  QHBoxLayout* pLoCheckboxes = new QHBoxLayout();
  pLoMain->addLayout( pLoCheckboxes );

  pChElem = new QCheckBox( SM_DLG( DLG::ROTMOVE::LBL::ELEMS ) );
  pLoCheckboxes->addWidget( pChElem );
  pChTempl = new QCheckBox( SM_DLG( DLG::ROTMOVE::LBL::TEMPLATE ) );
  pLoCheckboxes->addWidget( pChTempl );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnMove = new QPushButton( SM_DLG( DLG::ROTMOVE::BTN::MOVE ) );
  pLoButtons->addWidget( pBtnMove );
  pBtnRotate = new QPushButton( SM_DLG( DLG::ROTMOVE::BTN::ROTATE ) );
  pLoButtons->addWidget( pBtnRotate );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnMove,    SIGNAL(clicked()), this, SLOT(slotMove()) );
  connect( pBtnRotate,  SIGNAL(clicked()), this, SLOT(slotRotate()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}

void DlgMoveRotate::Done( RetCode r )
{
  SENDER_NAME_Q( "Done" );
  if( pSbX->value() == MAXDOUBLE ||
      pSbY->value() == MAXDOUBLE ||
      pSbScale->value() == MAXDOUBLE ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
    return;
  }

  if( pSbScale->value() <= 0. ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::ROTMOVE::ERR::BAD_SCALE );
    return;
  }

  done( r );
}
