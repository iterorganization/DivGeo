#include "DlgNewModel.h"

DlgNewModel::DlgNewModel( StringsManager* _pSM,
                          Console* _pConsole,
                          const QString& _crsDefaultConfig,
                          QWidget *parent) :
  QDialog(parent),
  pSM( _pSM ),
  pConsole( _pConsole ),
  useConfig( true )
{
  setWindowTitle( SM_DLG( DLG::NEW_MODEL::TITLE ) );
  setMinimumWidth( 400 );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QRadioButton* pRbNoConfig = new QRadioButton(
                                SM_DLG( DLG::NEW_MODEL::NO_CONFIG ) );
  pLoMain->addWidget( pRbNoConfig );

  QRadioButton* pRbUseConfig = new QRadioButton(
                                SM_DLG( DLG::NEW_MODEL::USE_CONFIG ) );
  pLoMain->addWidget( pRbUseConfig );
  pRbUseConfig->setChecked( true );

  connect( pRbNoConfig, SIGNAL(toggled(bool)), this, SLOT(slotNoCfgToggled(bool)) );

  QHBoxLayout* pLoPath = new QHBoxLayout();
  pLoMain->addLayout( pLoPath );

  pLePath = new QLineEdit( _crsDefaultConfig );
  pLoPath->addWidget( pLePath );

  pBtnBrowse = new QPushButton( SM_DLG( DLG::BTN::BROWSE ) );
  pBtnBrowse->setFixedWidth( 30 );
  pLoPath->addWidget( pBtnBrowse );
  connect( pBtnBrowse, SIGNAL(clicked()), this, SLOT(slotBrowse()) );

  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnCreate = new QPushButton( SM_DLG( DLG::BTN::CREATE ) );
  pLoButtons->addWidget( pBtnCreate );
  connect( pBtnCreate, SIGNAL(clicked()), this, SLOT(slotCreate()) );

  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(reject()) );

  pBrowseDlg = new QFileDialog( this );
  pBrowseDlg->setNameFilter( "DG config (*.dgc)" );
  pBrowseDlg->setViewMode( QFileDialog::Detail );
  pBrowseDlg->setDefaultSuffix( "dgc" );
  pBrowseDlg->setFileMode( QFileDialog::ExistingFile );
}

void DlgNewModel::slotNoCfgToggled( bool _nocfg )
{
  useConfig = !_nocfg;
  pLePath->setEnabled( useConfig );
  pBtnBrowse->setEnabled( useConfig );
}

void DlgNewModel::slotBrowse()
{
  if( QFile( pLePath->text() ).exists() )
    pBrowseDlg->selectFile( pLePath->text() );
  else
    pBrowseDlg->setDirectory( QApplication::applicationDirPath() );

  if( pBrowseDlg->exec() == QFileDialog::Accepted ) {
    QStringList fileNames = pBrowseDlg->selectedFiles();
    if( !fileNames.empty() )
      pLePath->setText( fileNames.first() );
  }
}

void DlgNewModel::slotCreate()
{
  if( useConfig && !QFile( pLePath->text() ).exists() ) {
    pConsole->Send( WND_ERROR, windowTitle(), ERR::FILENOTFOUND, QStringList( pLePath->text() ) );
    return;
  }
  accept();
}
