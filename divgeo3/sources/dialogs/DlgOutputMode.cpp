#include "DlgOutputMode.h"

DlgOutputMode::DlgOutputMode( ModelPtr _pModel, StringsManager* _pSM,
                              QWidget *parent) :
  QDialog(parent),
  pModel( _pModel ),
  pSM( _pSM )
{
  this->setWindowTitle( SM_DLG( DLG::OUTPUTMODE::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QHBoxLayout* pLoOutput = new QHBoxLayout();
  pLoMain->addLayout( pLoOutput );

  pRbSonnet = new QRadioButton( SM_DLG( DLG::OUTPUTMODE::LBL::SONNET ) );
  pLoOutput->addWidget( pRbSonnet );
  pRbCarre = new QRadioButton( SM_DLG( DLG::OUTPUTMODE::LBL::CARRE ) );
  pLoOutput->addWidget( pRbCarre );

  pChVariables = new QCheckBox( SM_DLG( DLG::OUTPUTMODE::LBL::VARS ) );
  pLoMain->addWidget( pChVariables );
  pChTargets = new QCheckBox( SM_DLG( DLG::OUTPUTMODE::LBL::TARGETS ) );
  pLoMain->addWidget( pChTargets );
  pChStructure = new QCheckBox( SM_DLG( DLG::OUTPUTMODE::LBL::STRUCTURE ) );
  pLoMain->addWidget( pChStructure );
  pChSurfaces = new QCheckBox( SM_DLG( DLG::OUTPUTMODE::LBL::SURFACES ) );
  pLoMain->addWidget( pChSurfaces );
  pChGridPoints = new QCheckBox( SM_DLG( DLG::OUTPUTMODE::LBL::GRIDPOINTS ) );
  pLoMain->addWidget( pChGridPoints );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnOk = new QPushButton( SM_DLG( DLG::BTN::OK ) );
  pLoButtons->addWidget( pBtnOk );
  QPushButton* pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnOk,      SIGNAL(clicked()), this, SLOT(slotAccept()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );

  ResetValues();
}

void DlgOutputMode::slotAccept()
{
  ulong flags = 0;
  if( pChVariables->isChecked() )   flags |= OF::VARS;
  if( pChStructure->isChecked() )   flags |= OF::STRUCTURE;
  if( pChTargets->isChecked() )     flags |= OF::TARGETS;
  if( pChSurfaces->isChecked() )    flags |= OF::SURFACES;
  if( pChGridPoints->isChecked() )  flags |= OF::GRIDPOINTS;
  pModel->SetOutputFlags( flags );

  pModel->SetOutputMode( pRbCarre->isChecked() ? OUTPUTMODE::CARRE : OUTPUTMODE::SONNET );

  pModel->ActionStack().Complete( "OutputMode:Accept" );
  accept();
}

void DlgOutputMode::ResetValues()
{
  pRbSonnet->setChecked( pModel->OutputMode() == OUTPUTMODE::SONNET );
  pRbCarre->setChecked( pModel->OutputMode() == OUTPUTMODE::CARRE );

  pChVariables->setChecked( pModel->HasOutputFlag( OF::VARS ) );
  pChStructure->setChecked( pModel->HasOutputFlag( OF::STRUCTURE ) );
  pChTargets->setChecked( pModel->HasOutputFlag( OF::TARGETS ) );
  pChSurfaces->setChecked( pModel->HasOutputFlag( OF::SURFACES ) );
  pChGridPoints->setChecked( pModel->HasOutputFlag( OF::GRIDPOINTS ) );
}
