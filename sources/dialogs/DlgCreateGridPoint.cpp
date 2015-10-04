#include "DlgCreateGridPoint.h"

DlgCreateGridPoint::DlgCreateGridPoint(StringsManager* _pSM, Console* _pConsole,
                 int _outputMode, GridPointExPtr* _ppGPX, const IComponentList& _crZones,
                                       QWidget *parent ):
  QDialog(parent),
  sender_name( "CreateGridPoint dialog" ),
  pSM( _pSM ),
  pConsole( _pConsole ),
  ppGPX( _ppGPX ),
  outputMode( _outputMode )
{
  this->setWindowTitle( SM_DLG( DLG::CREATE_GPOINT::TITLE ) );
  this->setModal( false );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pTabs = new QTabWidget( this );
  pLoMain->addWidget( pTabs );

  /* Single tab */
  pTabSingle = new QWidget();
  QGridLayout* pLoSingle = new QGridLayout( pTabSingle );
  pTabs->addTab( pTabSingle, SM_DLG( DLG::CREATE_GPOINT::SINGLE::TITLE ) );

  pCbZoneS = new QComboBox();
  pLoSingle->addWidget( pCbZoneS, 0, 1, 1, 1 );
  QLabel* pLbZoneS = new QLabel( SM_DLG( DLG::CREATE_GPOINT::AREA::TITLE ) );
  pLbZoneS->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoSingle->addWidget( pLbZoneS, 0, 0, 1, 1 );

  pSbValue = new QDoubleSpinBox();
  pSbValue->setDecimals( 5 );
  pSbValue->setRange( -1000, 1000 );
  pSbValue->setSingleStep( 0.001 );
  pLoSingle->addWidget( pSbValue, 1, 1, 1, 1 );
  QLabel* pLbValue = new QLabel( SM_DLG( DLG::CREATE_GPOINT::SINGLE::VALUE ) );
  pLbValue->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoSingle->addWidget( pLbValue, 1, 0, 1, 1 );

  QSpacerItem* pSpSingle = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoSingle->addItem( pSpSingle, 2, 0, 1, 2 );

  /* Multiple tab */
  pTabMult = new QWidget();
  QGridLayout* pLoMult = new QGridLayout( pTabMult );
  pTabs->addTab( pTabMult, SM_DLG( DLG::CREATE_GPOINT::MULTIPLE::TITLE ) );

  pDigr = new DistrGraph();
  pDigr->setMinimumSize( 200, 150 );
  pLoMult->addWidget( pDigr, 0, 0, 1, 3 );

  pCbZoneM = new QComboBox();
  pLoMult->addWidget( pCbZoneM, 1, 1, 1, 1 );
  pLbZoneM = new QLabel( SM_DLG( DLG::CREATE_GPOINT::AREA::TITLE ) );
  pLbZoneM->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbZoneM, 1, 0, 1, 1 );

  pSbCount = new QSpinBox();
  pSbCount->setRange( 1, 100 );
  pSbCount->setSingleStep( 1 );
  pSbCount->setValue( 1 );
  pLoMult->addWidget( pSbCount, 2, 1, 1, 1 );
  pLbCount = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::COUNT ) );
  pLbCount->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbCount, 2, 0, 1, 1 );

  pSbAlpha1 = new QDoubleSpinBox();
  pSbAlpha1->setDecimals( 5 );
  pSbAlpha1->setRange( 1e-05, 1000 );
  pSbAlpha1->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbAlpha1, 3, 1, 1, 1 );
  pLbAlpha1 = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::A1 ) );
  pLbAlpha1->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbAlpha1, 3, 0, 1, 1 );

  pSbAlpha2 = new QDoubleSpinBox();
  pSbAlpha2->setDecimals( 5 );
  pSbAlpha2->setRange( 1e-05, 1000 );
  pSbAlpha2->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbAlpha2, 4, 1, 1, 1 );
  pLbAlpha2 = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::A2 ) );
  pLbAlpha2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbAlpha2, 4, 0, 1, 1 );

  pCbLaw = new QComboBox();
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::NORM ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::FLIP ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::TWOA) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::SYMM ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::DELTA ) );
  pCbLaw->setCurrentIndex( 4 );
  pLoMult->addWidget( pCbLaw, 5, 1, 1, 1 );
  pLbLaw = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LAW::TITLE ) );
  pLbLaw->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbLaw, 5, 0, 1, 1 );

  pBtnReset = new QPushButton( SM_DLG( DLG::DISTR_GRAPH::BTN::RESET ) );
  pLoMult->addWidget( pBtnReset, 6, 1, 1, 1 );

  pBtnCopy = new QPushButton( SM_DLG( DLG::DISTR_GRAPH::BTN::COPY ) );
  pBtnCopy->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  pBtnCopy->setFixedWidth( 24 );
  pLoMult->addWidget( pBtnCopy, 1, 2, 6, 1 );

  /* Zones */
  FOREACHPTRCONST( GridPointSegPtr, pZone, _crZones ) {
    QString sText = QString::fromStdString( pZone->LongName() );
    if( sText.isEmpty() )
      sText = QString( "Zone %1" ).arg( pZone->Zone() );
    QVariant data = QVariant( pZone->Zone() );
    pCbZoneS->addItem( sText, data );
    pCbZoneM->addItem( sText, data );
  }
  pCbZoneS->setCurrentIndex( 0 );
  pCbZoneM->setCurrentIndex( 0 );


  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );
  pBtnCreate = new QPushButton( SM_DLG( DLG::BTN::CREATE ) );
  pLoButtons->addWidget( pBtnCreate );
  pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* connections */
  connect( pBtnCreate,  SIGNAL(clicked()), this, SLOT(slotCreate()) );
  connect( pBtnClose,   SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );

  connect( pCbLaw,      SIGNAL(currentIndexChanged(int)),
           this,        SLOT(slotLawChanged(int)) );
  connect( pSbCount,    SIGNAL(valueChanged(int)),
           this,        SLOT(slotCountChanged(int)) );
  connect( pSbAlpha1,   SIGNAL(valueChanged(double)),
           pDigr,       SLOT(slotChangeAlpha0(double)) );
  connect( pSbAlpha2,   SIGNAL(valueChanged(double)),
           pDigr,       SLOT(slotChangeAlpha1(double)) );
  connect( pDigr,       SIGNAL(signalChangedAlpha0(double)),
           pSbAlpha1,   SLOT(setValue(double)) );
  connect( pDigr,       SIGNAL(signalChangedAlpha1(double)),
           pSbAlpha2,   SLOT(setValue(double)) );

  connect( pBtnCopy,    SIGNAL(clicked()), this, SLOT(slotCopy()) );
  connect( pBtnReset,   SIGNAL(clicked()), this, SLOT(slotReset()) );

  /* distrgraph setup */

  pDigr->EnableUpdate( false );
  slotLawChanged( pCbLaw->currentIndex() );
  pDigr->slotChangeAlpha0( pSbAlpha1->value() );
  pDigr->slotChangeAlpha1( pSbAlpha2->value() );
  pDigr->EnableUpdate( true );
  pDigr->slotChangeCount( pSbCount->value() );
}

void DlgCreateGridPoint::slotCountChanged( int count )
{
  if( pDigr->Law() == DGLAW::DELTA ) {
    double deltas[2];
    pDigr->RecalcDeltas( count, deltas );
    pDigr->EnableUpdate( false );
    pSbAlpha1->setValue( deltas[0] );
    pSbAlpha2->setValue( deltas[1] );
    pDigr->EnableUpdate( true );
  }
  pDigr->slotChangeCount( count );
}

void DlgCreateGridPoint::slotLawChanged( int index )
{
  bool deltaLaw = (index == (int)DGLAW::DELTA);
  bool twoArgs = (index == (int)DGLAW::TWOALPHA || deltaLaw);
  pSbAlpha2->setEnabled( twoArgs );
  pLbAlpha2->setEnabled( twoArgs );
  if( deltaLaw ) {
    pLbAlpha1->setText( SM_DLG( DLG::DISTR_GRAPH::LBL::D1 ) );
    pLbAlpha2->setText( SM_DLG( DLG::DISTR_GRAPH::LBL::D2 ) );
  }
  else {
    pLbAlpha1->setText( SM_DLG( DLG::DISTR_GRAPH::LBL::A1 ) );
    pLbAlpha2->setText( SM_DLG( DLG::DISTR_GRAPH::LBL::A2 ) );
  }

  pDigr->slotChangeLaw( index );
}

void DlgCreateGridPoint::slotCreate()
{
  if( pTabs->currentWidget() == pTabSingle ) {
    SENDER_NAME_Q( "slotCreate:single" );
    double value = pSbValue->value();

    // Next checks can't be failed if controls are setup with right parameters
    if( value == MAXDOUBLE ) {
      pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
      return;
    }
    if( value < 0 && value > 1 ) {
      pConsole->Send( WND_ERROR, SENDER, ERR::INVAREA );
      return;
    }
  }
  else if( pTabs->currentWidget() == pTabMult ) {
    SENDER_NAME_Q( "slotCreate:multiple" );

    if( outputMode == OUTPUTMODE::CARRE &&
        pCbLaw->currentIndex() != (int)DGLAW::DELTA ) {
      pConsole->Send( WND_ERROR, SENDER,
                      SM_DLG( DLG::DISTR_GRAPH::ERR::WRONG_CARRE_LAW ) );
      return;
    }
  }

  accept();
}

void DlgCreateGridPoint::slotReset()
{
  if( pDigr->Law() == DGLAW::DELTA ) {
    pSbAlpha1->setValue( pSbAlpha1->minimum() );
    pSbAlpha2->setValue( pSbAlpha2->minimum() );
  }
  else {
    pSbAlpha1->setValue( 1. );
    pSbAlpha2->setValue( 1. );
  }
}

void DlgCreateGridPoint::slotCopy()
{
  SENDER_NAME_Q( "slotCopy" );
  if( ppGPX == null && (*ppGPX) == null ) {
    pConsole->Send( WND_ERROR, SENDER,
                    SM_DLG( DLG::CREATE_GPOINT::ERR::NO_REMEMBERED_GRIDPOINT ) );
    return;
  }

  CreatorId _crid = (*ppGPX)->GetCreatorId();
  if( !_crid.IsGood() ) {
    pConsole->Send( WND_ERROR, SENDER,
                    SM_DLG( DLG::CREATE_GPOINT::ERR::BAD_CREATOR_ID ) );
    return;
  }

  if( outputMode == OUTPUTMODE::CARRE && _crid.CarreFlag() == 0 ) {
    pConsole->Send( WND_ERROR, SENDER,
                    SM_DLG( DLG::CREATE_GPOINT::ERR::NON_CARRE_MODE ) );
    return;
  }

  pDigr->setUpdatesEnabled( false );
  for( int index = 0; index < pCbZoneM->count(); index++ ) {
    if( pCbZoneM->itemData( index ).toInt() == _crid.Area() ) {
      pCbZoneM->setCurrentIndex( index );
      break;
    }
  }
  pSbCount->setValue( _crid.Count() + 1 );
  pSbAlpha1->setValue( _crid.Alpha1() );
  pSbAlpha2->setValue( _crid.Alpha2() );
  pDigr->setUpdatesEnabled( true );
  pCbLaw->setCurrentIndex( _crid.Law() );
}


bool DlgCreateGridPoint::IsSingle() const
{
  return pTabs->currentWidget() == pTabSingle;
}

bool DlgCreateGridPoint::IsMultiple() const
{
  return pTabs->currentWidget() == pTabMult;
}

int DlgCreateGridPoint::SingleArea() const
{
  return pCbZoneS->itemData( pCbZoneS->currentIndex() ).toInt();
}

double DlgCreateGridPoint::SingleValue() const
{
  return pSbValue->value();
}

CreatorId DlgCreateGridPoint::MultipleCreator() const
{
  int zone = pCbZoneM->itemData( pCbZoneM->currentIndex() ).toInt();
  return CreatorId( zone, pSbCount->value() - 1,
                    pSbAlpha1->value(), pSbAlpha2->value(),
                    (DistrLaw)pCbLaw->currentIndex(),
                    (int)(outputMode == OUTPUTMODE::CARRE) );
}
