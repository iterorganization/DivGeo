#include "DlgMeshHeader.h"

DlgMeshHeader::DlgMeshHeader(MeshPtr _pMesh, ModelPtr _pModel,
                              StringsManager* _pSM, Console* _pConsole,
                              QWidget *parent):
  QDialog(parent),
  pMesh( _pMesh ),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::EDIT_MESH_HEADER::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  pTeHeader = new QTextEdit();
  pLoMain->addWidget( pTeHeader );

  QString sHeader = QString::fromStdString( pMesh->HeaderString() );
  pTeHeader->setPlainText( sHeader );
  pTeHeader->setWordWrapMode( QTextOption::WordWrap );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnChange = new QPushButton( SM_DLG( DLG::EDIT_MESH_HEADER::BTN::CHANGE ) );
  pLoButtons->addWidget( pBtnChange );
  pBtnChange->setDisabled( true );
  QPushButton* pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnChange,  SIGNAL(clicked()), this, SLOT(slotAccept()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
  connect( pTeHeader,   SIGNAL(textChanged()), this, SLOT(slotChanged()) );
}

void DlgMeshHeader::slotAccept()
{
  SENDER_NAME_Q( "DlgMeshHeader::slotAccept" );
  std::string sHeader = pTeHeader->toPlainText().toStdString();
  int r = pMesh->CheckHeaderString( sHeader );
  if( r != 0 ) {
    pConsole->Send( WND_ERROR, SENDER, r );
    return;
  }
  pMesh->SetHeaderString( sHeader );
  pModel->ActionStack().Complete( "MeshHeader:Accept" );
  accept();
}

void DlgMeshHeader::slotChanged()
{
  pBtnChange->setEnabled( true );
}
