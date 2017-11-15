#include "DlgAbout.h"

DlgAbout::DlgAbout( StringsManager* _pSM, const QString& _crsVersion, QWidget *parent):
  QDialog(parent),
  pSM( _pSM )
{
  this->setWindowTitle( SM_DLG( DLG::ABOUT::TITLE ) );
  this->setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  /* About */
  QHBoxLayout* pLoAbout = new QHBoxLayout();
  pLoMain->addLayout( pLoAbout );

  QLabel* pLbIcon = new QLabel();
  QPixmap icon( ":/main/icon.png" );
  pLbIcon->setPixmap( icon );
  pLbIcon->setFixedSize( icon.size() );
  pLoAbout->addWidget( pLbIcon );
  pLoAbout->setAlignment( pLbIcon, Qt::AlignTop );

  QVBoxLayout* pLoText = new QVBoxLayout();
  pLoAbout->addLayout( pLoText );

  QLabel* pLbTitle = new QLabel( _crsVersion );
  pLbTitle->setFont( QFont( "Arial", 12, QFont::Bold ) );
  pLoText->addWidget( pLbTitle );

  QString sText = QString( "Written by Mikhail Buinevich\n"
                           "Based on dg v2.1 by Konstantin Kukushkin\n\n"
                           "Built %4 at %5\n"
                           "Using Qt %1\n"
                           "Compiled with %2 %3\n" )
                  .arg( QT_VERSION_STR )
                  .arg( COMPILER_NAME ).arg( COMPILER_VERSION )
                  .arg( __DATE__ ).arg( __TIME__ );

  QLabel* pLbInfo = new QLabel( sText );
  pLbInfo->setFont( QFont( "Arial", 10 ) );
  pLoText->addWidget( pLbInfo );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnClose,  SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}
