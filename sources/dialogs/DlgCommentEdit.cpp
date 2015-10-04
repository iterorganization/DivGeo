#include "DlgCommentEdit.h"

DlgCommentEdit::DlgCommentEdit(CommentPtr _pComment, IComponentPtr _pLinkedObject,
                               StringsManager* _pSM, QWidget *parent) :
  QDialog(parent),
  pComment( _pComment ),
  pLinkedObject( _pLinkedObject ),
  pSM( _pSM )
{
  QString sTitle = SM_DLG( DLG::COMMENTEDIT::TITLE );
  this->setWindowTitle( sTitle );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QHBoxLayout* pLoLinkedObj = new QHBoxLayout();
  pLoMain->addLayout( pLoLinkedObj );

  if( pLinkedObject != null ) {
    QLabel* pLblLinkedObj = new QLabel( SM_DLG( DLG::COMMENTEDIT::LINKED_OBJECT ) );
    pLoLinkedObj->addWidget( pLblLinkedObj );
    QString sText = QString::fromStdString( pLinkedObject->DetailedInfo() );
    QLabel* pLblObjDescr = new QLabel( sText );
    pLoLinkedObj->addWidget( pLblObjDescr );
    QPushButton* pBtnInserDescr = new QPushButton( SM_DLG( DLG::COMMENTEDIT::INSERT_DESCR ) );
    pLoLinkedObj->addWidget( pBtnInserDescr );
    connect( pBtnInserDescr, SIGNAL(clicked()), this, SLOT(slotInsert()) );
  }

  QString sText = QString::fromStdWString( pComment->Text() );
  pTeHelp = new QTextEdit();
  pTeHelp->setPlainText( sText );
  pTeHelp->setWordWrapMode( QTextOption::WordWrap );
  pTeHelp->setLineWrapMode( QTextEdit::WidgetWidth );
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

void DlgCommentEdit::slotAccept()
{
  std::wstring wsText = pTeHelp->toPlainText().toStdWString();
  pComment->ChangeText( wsText );
  accept();
}

void DlgCommentEdit::slotInsert()
{
  QString sText = QString::fromStdString( pLinkedObject->DetailedInfo() );
  pTeHelp->insertPlainText( sText );
}
