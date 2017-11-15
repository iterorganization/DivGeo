#include "DlgVdHelpEdit.h"

DlgVdHelpEdit::DlgVdHelpEdit( VarDefPtr _pVD, StringsManager* _pSM,
                              QWidget *parent) :
  QDialog( parent, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint ), //1411
  pVD( _pVD ),
  pSM( _pSM )
{
  QString sTitle = SM_DLG( DLG::HELPEDIT::TITLE_EX )
                   .arg( QString::fromStdString( pVD->Name() ) );
  this->setWindowTitle( sTitle );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  pTeHelp = new QTextEdit( QString::fromStdString( pVD->Help() ) );
  pLoMain->addWidget( pTeHelp );

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
  connect( pBtnOk, SIGNAL(clicked()), this, SLOT(slotAccept()) );
  connect( pBtnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
}

void DlgVdHelpEdit::slotAccept()
{
  std::string sText = pTeHelp->toPlainText().toStdString();
  pVD->ChangeHelpString( sText );
  accept();
}
