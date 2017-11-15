#include "DlgMeshOptions.h"

DlgMeshOptions::DlgMeshOptions( MeshPtr _pMesh, ModelPtr _pModel,
                                StringsManager* _pSM, Console* _pConsole,
                                QWidget *parent) :
  QDialog(parent),
  pMesh( _pMesh ),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::MESHOPTIONS::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QFormLayout* pLoProp = new QFormLayout();
  pLoMain->addLayout( pLoProp );

  pCbSlide = new QComboBox();
  pCbSlide->addItems( QStringList()
                      <<SM_DLG( DLG::MESHOPTIONS::MODE::SPLINES )
                      <<SM_DLG( DLG::MESHOPTIONS::MODE::SURFACES ) );
  pLoProp->addRow( SM_DLG( DLG::MESHOPTIONS::MODE::TITLE ), pCbSlide );

  pLeMargin = new QLineEdit();
  pLoProp->addRow( SM_DLG( DLG::MESHOPTIONS::LBL::SLIDE ), pLeMargin );

  pChDouble = new QCheckBox();
  pLoProp->addRow( SM_DLG( DLG::MESHOPTIONS::LBL::BORDER ), pChDouble );

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

void DlgMeshOptions::slotAccept()
{
  SENDER_NAME_Q( "DlgMeshOptions::slotAccept" );
  bool isOk = true;
  double td = pLeMargin->text().toDouble( &isOk );
  if( !isOk || td <= 0 ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, SENDER, DLG::MESHOPTIONS::ERR::BAD_SLIDING_TRESHOLD );
    return;
  }

  pMesh->SetSlidingThreshold( td );
  pMesh->SetDoubleBorderFlag( pChDouble->isChecked() );
  pMesh->SetSlidingMode( pCbSlide->currentIndex() == 0 ? MSM::SPLINE : MSM::SURFACE );

  pModel->ActionStack().Complete( "MeshOptions:Accept" );
  accept();
}

void DlgMeshOptions::ResetValues()
{
  pCbSlide->setCurrentIndex( pMesh->CheckSlidingMode( MSM::SPLINE ) ? 0 : 1 );
  pLeMargin->setText( QString( "%1" ).arg( pMesh->SlidingThreshold() ) );
  pChDouble->setChecked( pMesh->HasDoubleBorder() );
}
