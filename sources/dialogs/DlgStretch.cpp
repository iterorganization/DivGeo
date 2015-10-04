#include "DlgStretch.h"

DlgStretch::DlgStretch( const Point& _current, double _min, double _max,
                        StringsManager* _pSM, Console* _pConsole,
                        QWidget *parent):
  QDialog(parent),
  sender_name( "DlgMoveRotate" ),
  pSM( _pSM ),
  pConsole( _pConsole ),
  current( _current ),
  minScale( _min ),
  maxScale( _max )
{
  this->setWindowTitle( SM_DLG( DLG::ROTMOVE::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QGridLayout* pLoGrid = new QGridLayout( this );
  pLoMain->addLayout( pLoGrid );

  pSbX = new QDoubleSpinBox();
  pSbX->setDecimals( 5 );
  pSbX->setRange( minScale, maxScale );
  pSbX->setSingleStep( 1. );
  pLoGrid->addWidget( pSbX, 0, 1, 1, 1 );
  pLbX = new QLabel( SM_DLG( DLG::STRETCH::LBL::X ) );
  pLbX->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbX, 0, 0, 1, 1 );

  pSbY = new QDoubleSpinBox();
  pSbY->setDecimals( 5 );
  pSbY->setRange( minScale, maxScale );
  pSbY->setSingleStep( 1. );
  pLoGrid->addWidget( pSbY, 1, 1, 1, 1 );
  pLbY = new QLabel( SM_DLG( DLG::STRETCH::LBL::Y ) );
  pLbY->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbY, 1, 0, 1, 1 );

  pSbX->setValue( current.x );
  pSbY->setValue( current.y );

  QSpacerItem* pSpByPoint = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoGrid->addItem( pSpByPoint, 3, 0, 1, 2 );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnOk = new QPushButton( SM_DLG( DLG::BTN::OK ) );
  pLoButtons->addWidget( pBtnOk );
  pBtnReset = new QPushButton( SM_DLG( DLG::STRETCH::BTN::RESET ) );
  pLoButtons->addWidget( pBtnReset );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnOk,      SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnReset,   SIGNAL(clicked()), this, SLOT(slotReset()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );

}
