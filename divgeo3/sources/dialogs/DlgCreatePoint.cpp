#include "DlgCreatePoint.h"

DlgCreatePoint::DlgCreatePoint( StringsManager* _pSM,
                          const QString& _title, QWidget *parent):
  QDialog(parent),
  pSM( _pSM )
{
  this->setWindowTitle( _title );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  /* Main controls */
  QGridLayout* pLoGrid = new QGridLayout( this );
  pLoMain->addLayout( pLoGrid );

  pSbX = new DoubleSpinBoxExt /*1409*/();
  pSbX->setDecimals( 5 );
  pSbX->setRange( -100000, 100000 );
  pSbX->setSingleStep( 1. );
  pLoGrid->addWidget( pSbX, 0, 1, 1, 1 );
  pLbX = new QLabel( SM_DLG( DLG::CREATE_NODE::LBL::X ) );
  pLbX->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbX, 0, 0, 1, 1 );

  pSbY = new DoubleSpinBoxExt /*1409*/();
  pSbY->setDecimals( 5 );
  pSbY->setRange( -100000, 100000 );
  pSbY->setSingleStep( 1. );
  pLoGrid->addWidget( pSbY, 1, 1, 1, 1 );
  pLbY = new QLabel( SM_DLG( DLG::CREATE_NODE::LBL::Y ) );
  pLbY->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoGrid->addWidget( pLbY, 1, 0, 1, 1 );

  QSpacerItem* pSpByPoint = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoGrid->addItem( pSpByPoint, 2, 0, 1, 2 );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnCreate = new QPushButton( SM_DLG( DLG::BTN::CREATE ) );
  pLoButtons->addWidget( pBtnCreate );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnCreate,  SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}
