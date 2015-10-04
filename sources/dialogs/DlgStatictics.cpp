#include "DlgStatictics.h"

DlgStatictics::DlgStatictics( ModelPtr _pModel, StringsManager* _pSM,
                              QWidget *parent) :
  QDialog(parent),
  pModel( _pModel ),
  pSM( _pSM )
{
  this->setWindowTitle( SM_DLG( DLG::INFO::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QFormLayout* pLoProp = new QFormLayout();
  pLoMain->addLayout( pLoProp );

  pLblNodes = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::NODES ), pLblNodes );
  pLblElems = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::ELEMS ), pLblElems );
  pLblSources = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::SOURCES ), pLblSources );
  pLblUnusedNumbers = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::UNUSEDNUM ), pLblUnusedNumbers );
  pLblSurfaces = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::SURFACES ), pLblSurfaces );
  pLblGridPoints = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::GRIDPOINTS ), pLblGridPoints );
  pLblSeparators = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::SEPARATORS ), pLblSeparators );
  pLblChords = new QLabel();
  pLoProp->addRow( SM_DLG( DLG::INFO::LBL::CHORDS ), pLblChords );

  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(accept()) );
  pLoMain->addWidget( pBtnClose );

  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
  pLoMain->addWidget( pBtnHelp );

  UpdateInfo();
}

void DlgStatictics::UpdateInfo()
{
  pLblNodes->setText( ToQString( pModel->NodesNum() ) );
  pLblElems->setText( ToQString( pModel->ElementsNum() ) );
  pLblSources->setText( ToQString( pModel->SourcesNum() ) );
  pLblUnusedNumbers->setText( ToQString( (ulong)pModel->UnusedNodes().size() ) );
  FluxModelPtr pFlux = pModel->GetFluxModel();
  if( pFlux->HasEquil() ) {
    pLblSurfaces->setText( ToQString( pFlux->GetSurfacesStatsStr() ) );
    pLblGridPoints->setText( ToQString( pFlux->GetGridPointStatsStr() ) );
  }
  else {
    pLblSurfaces->setText( SM_MSG( ERR::NOEQUIL ) );
    pLblGridPoints->setText( SM_MSG( ERR::NOEQUIL ) );
  }
  pLblSeparators->setText( ToQString( pModel->SeparatorsNum() ) );
  pLblChords->setText( ToQString( pModel->ChordsNum() ) );
}
