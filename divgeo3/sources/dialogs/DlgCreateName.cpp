#include "DlgCreateName.h"

DlgCreateName::DlgCreateName( ulong _titleCode,
                              StringsManager* _pSM, QWidget *parent) :
  QDialog(parent),
  pSM( _pSM )
{
  this->setWindowTitle( SM_DLG( _titleCode ) );
  QGridLayout* pLoMain = new QGridLayout( this );

  QLabel* pLblName = new QLabel( SM_DLG( DLG::VARSETDEFCREATE::LBL::NAME ) );
  pLoMain->addWidget( pLblName, 0, 0, 1, 1 );

  pLeName = new QLineEdit();
  pLoMain->addWidget( pLeName, 0, 1, 1, 2 );

  pBtnCreate = new QPushButton( SM_DLG( DLG::BTN::CREATE ) );
  pLoMain->addWidget( pBtnCreate, 1, 0, 1, 1 );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoMain->addWidget( pBtnCancel, 1, 1, 1, 1 );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoMain->addWidget( pBtnHelp, 1, 2, 1, 1 );

  /* Connections */
  connect( pBtnCreate, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}
