#include "DlgCreateSurface.h"

DlgCreateSurface::DlgCreateSurface(StringsManager* _pSM, Console* _pConsole,
             int _outputMode, SurfaceExPtr* _ppSX, const IComponentList& _crZones,
                                   QWidget* parent ):
  QDialog(parent),
  sender_name( "CreateSurface dialog" ),
  pSM( _pSM ),
  pConsole( _pConsole ),
  ppSX( _ppSX ),
  outputMode( _outputMode )
{
  this->setWindowTitle( SM_DLG( DLG::CREATE_SURFACE::TITLE ) );
  this->setModal( false );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pTabs = new QTabWidget( this );
  pLoMain->addWidget( pTabs );

  /* Single tab */
  pTabSingle = new QWidget();
  pTabs->addTab( pTabSingle, SM_DLG( DLG::CREATE_SURFACE::SINGLE::TITLE ) );

  QHBoxLayout* pLoSingle = new QHBoxLayout( pTabSingle );
  pTabsSingle = new QTabWidget( pTabSingle );
  pLoSingle->addWidget( pTabsSingle );

  /* ByLevel tab */
  pTabByLevel = new QWidget();
  pTabsSingle->addTab( pTabByLevel, SM_DLG( DLG::CREATE_SURFACE::SINGLE::BY_LEVEL ) );
  QGridLayout* pLoByLevel = new QGridLayout( pTabByLevel );

  pCbAreaS = new QComboBox();
  pLoByLevel->addWidget( pCbAreaS, 0, 1, 1, 1 );
  QLabel* pLbAreaS = new QLabel( SM_DLG( DLG::CREATE_SURFACE::AREA::TITLE ) );
  pLbAreaS->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoByLevel->addWidget( pLbAreaS, 0, 0, 1, 1 );

  pSbValue = new DoubleSpinBoxExt /*1409*/();
  pSbValue->setDecimals( 5 );
  pSbValue->setRange( -1000, 1000 );
  pSbValue->setSingleStep( 0.001 );
  pLoByLevel->addWidget( pSbValue, 1, 1, 1, 1 );
  QLabel* pLbValue = new QLabel( SM_DLG( DLG::CREATE_SURFACE::SINGLE::LEVEL ) );
  pLbValue->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoByLevel->addWidget( pLbValue, 1, 0, 1, 1 );

  QSpacerItem* pSpByLevel = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoByLevel->addItem( pSpByLevel, 2, 0, 1, 2 );

  /* ByPoint tab */
  pTabByPoint = new QWidget();
  pTabsSingle->addTab( pTabByPoint, SM_DLG( DLG::CREATE_SURFACE::SINGLE::BY_POINT ) );
  QGridLayout* pLoByPoint = new QGridLayout( pTabByPoint );

  pSbX = new DoubleSpinBoxExt /*1409*/();
  pSbX->setDecimals( 5 );
  pSbX->setRange( -100000, 100000 );
  pSbX->setSingleStep( 1. );
  pLoByPoint->addWidget( pSbX, 0, 1, 1, 1 );
  QLabel* pLbX = new QLabel( SM_DLG( DLG::CREATE_SURFACE::SINGLE::X ) );
  pLbX->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoByPoint->addWidget( pLbX, 0, 0, 1, 1 );

  pSbY = new DoubleSpinBoxExt /*1409*/();
  pSbY->setDecimals( 5 );
  pSbY->setRange( -100000, 100000 );
  pSbY->setSingleStep( 1. );
  pLoByPoint->addWidget( pSbY, 1, 1, 1, 1 );
  QLabel* pLbY = new QLabel( SM_DLG( DLG::CREATE_SURFACE::SINGLE::Y ) );
  pLbY->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoByPoint->addWidget( pLbY, 1, 0, 1, 1 );

  QSpacerItem* pSpByPoint = new QSpacerItem( 10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding );
  pLoByPoint->addItem( pSpByPoint, 2, 0, 1, 2 );


  /* Multiple tab */
  pTabMult = new QWidget();
  QGridLayout* pLoMult = new QGridLayout( pTabMult );
  pTabs->addTab( pTabMult, SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::TITLE ) );

  pDigr = new DistrGraph();
  pDigr->setMinimumSize( 200, 150 );
  pLoMult->addWidget( pDigr, 0, 0, 1, 4 );

  pCbAreaM = new QComboBox();
  pLoMult->addWidget( pCbAreaM, 1, 1, 1, 1 );
  pLbAreaM = new QLabel( SM_DLG( DLG::CREATE_SURFACE::AREA::TITLE ) );
  pLbAreaM->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbAreaM, 1, 0, 1, 1 );

  pSbLevel1 = new DoubleSpinBoxExt /*1409*/();
  pSbLevel1->setDecimals( 5 );
  pSbLevel1->setRange( -1000, 1000 );
  pSbLevel1->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbLevel1, 2, 1, 1, 1 );
  pLbLevel1 = new QLabel( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL1 ) );
  pLbLevel1->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbLevel1, 2, 0, 1, 1 );

  pBtnCopyL1 = new QPushButton( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::BTN::COPY ) );
  pBtnCopyL1->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  pBtnCopyL1->setFixedWidth( 24 );
  pLoMult->addWidget( pBtnCopyL1, 2, 2, 1, 1 );

  pSbLevel2 = new DoubleSpinBoxExt /*1409*/();
  pSbLevel2->setDecimals( 5 );
  pSbLevel2->setRange( -1000, 1000 );
  pSbLevel2->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbLevel2, 3, 1, 1, 1 );
  pLbLevel2 = new QLabel( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL2 ) );
  pLbLevel2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbLevel2, 3, 0, 1, 1 );

  pBtnCopyL2 = new QPushButton( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::BTN::COPY ) );
  pBtnCopyL2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  pBtnCopyL2->setFixedWidth( 24 );
  pLoMult->addWidget( pBtnCopyL2, 3, 2, 1, 1 );

  pSbCount = new QSpinBox();
  pSbCount->setRange( 1, 100 );
  pSbCount->setSingleStep( 1 );
  pSbCount->setValue( 1 );
  pLoMult->addWidget( pSbCount, 4, 1, 1, 1 );
  pLbCount = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::COUNT ) );
  pLbCount->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbCount, 4, 0, 1, 1 );

  pSbAlpha1 = new DoubleSpinBoxExt /*1409*/();
  pSbAlpha1->setDecimals( 5 );
  pSbAlpha1->setRange( 1e-05, 1000 );
  pSbAlpha1->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbAlpha1, 5, 1, 1, 1 );
  pLbAlpha1 = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::A1 ) );
  pLbAlpha1->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbAlpha1, 5, 0, 1, 1 );

  pSbAlpha2 = new DoubleSpinBoxExt /*1409*/();
  pSbAlpha2->setDecimals( 5 );
  pSbAlpha2->setRange( 1e-05, 1000 );
  pSbAlpha2->setSingleStep( 0.01 );
  pLoMult->addWidget( pSbAlpha2, 6, 1, 1, 1 );
  pLbAlpha2 = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LBL::A2 ) );
  pLbAlpha2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbAlpha2, 6, 0, 1, 1 );

  pCbLaw = new QComboBox();
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::NORM ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::FLIP ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::TWOA) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::SYMM ) );
  pCbLaw->addItem( SM_DLG( DLG::DISTR_GRAPH::LAW::DELTA ) );
  pCbLaw->setCurrentIndex( 4 );
  pLoMult->addWidget( pCbLaw, 7, 1, 1, 1 );
  pLbLaw = new QLabel( SM_DLG( DLG::DISTR_GRAPH::LAW::TITLE ) );
  pLbLaw->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  pLoMult->addWidget( pLbLaw, 7, 0, 1, 1 );

  pBtnMarkBoundElems = new QPushButton(
      SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::BTN::MARK_BOUND_ELEM ) );
  pLoMult->addWidget( pBtnMarkBoundElems, 8, 1, 1, 1 );

  pBtnReset = new QPushButton( SM_DLG( DLG::DISTR_GRAPH::BTN::RESET ) );
  pLoMult->addWidget( pBtnReset, 9, 0, 1, 1 );

  pChRemoveOld = new QCheckBox(
      SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::LBL::REMOVE_OLD ) );
  pChRemoveOld->setEnabled( false );
  pLoMult->addWidget( pChRemoveOld, 9, 1, 1, 2 );

  pBtnCopy = new QPushButton( SM_DLG( DLG::DISTR_GRAPH::BTN::COPY ) );
  pBtnCopy->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  pBtnCopy->setFixedWidth( 24 );
  pLoMult->addWidget( pBtnCopy, 1, 3, 6, 9 );

  /* Zones */
  FOREACHPTRCONST( SurfaceZonePtr, pZone, _crZones ) {
    QString sText = QString::fromStdString( pZone->LongName() );
    if( sText.isEmpty() )
      sText = QString( "Zone %1" ).arg( pZone->Zone() );
    QVariant data = QVariant( pZone->Zone() );
    pCbAreaS->addItem( sText, data );
    pCbAreaM->addItem( sText, data );
  }
  pCbAreaS->setCurrentIndex( 0 );
  pCbAreaM->setCurrentIndex( 0 );

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
  connect( pCbAreaM,    SIGNAL(currentIndexChanged(int)),
           this,        SLOT(slotAreaChanged(int)) ); // For multiple only
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
  connect( pBtnMarkBoundElems,  SIGNAL(clicked()),
           this,                SLOT(slotMarkBoundElems()) );

  QSignalMapper* pLevelCopyMapper = new QSignalMapper( pTabMult );
  connect( pLevelCopyMapper, SIGNAL(mapped(int)), this, SLOT(slotCopyLevel(int)) );
  pLevelCopyMapper->setMapping( pBtnCopyL1, 1 );
  pLevelCopyMapper->setMapping( pBtnCopyL2, 2 );
  connect( pBtnCopyL1,  SIGNAL(clicked()), pLevelCopyMapper, SLOT(map()) );
  connect( pBtnCopyL2,  SIGNAL(clicked()), pLevelCopyMapper, SLOT(map()) );

  /* distrgraph setup */

  pDigr->EnableUpdate( false );
  slotLawChanged( pCbLaw->currentIndex() );
  pDigr->slotChangeAlpha0( pSbAlpha1->value() );
  pDigr->slotChangeAlpha1( pSbAlpha2->value() );
  pDigr->EnableUpdate( true );
  pDigr->slotChangeCount( pSbCount->value() );

  setOutputMode( outputMode );
}

void DlgCreateSurface::slotLawChanged( int index )
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

void DlgCreateSurface::slotCopyLevel( int index )
{
  SENDER_NAME_Q( "slotCopyLevel" );
  if( outputMode == OUTPUTMODE::CARRE )
    return;

  if( ppSX == null || *ppSX == null ) {
    pConsole->Send( WND_ERROR, FULL_SENDER,
                        DLG::CREATE_SURFACE::ERR::NO_REMEMBERED_SURFACE );
    return;
  }

  if( (*ppSX)->Zone() < 0 ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NOSURFZONE );
    return;
  }

  switch( index ) {
  case 1: pSbLevel1->setValue( (*ppSX)->Level() ); break;
  case 2: pSbLevel2->setValue( (*ppSX)->Level() ); break;
  default: assert( false );
  }

  pChRemoveOld->setCheckState( Qt::Unchecked );
}

void DlgCreateSurface::slotCountChanged( int count )
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

void DlgCreateSurface::slotAreaChanged( int index )
{
  UNUSED( index );
  if( outputMode != OUTPUTMODE::CARRE ) {
    pChRemoveOld->setEnabled( false );
    pChRemoveOld->setCheckState( Qt::Unchecked );
  }
}

void DlgCreateSurface::slotMarkBoundElems()
{
  int zone = pCbAreaM->itemData( pCbAreaM->currentIndex() ).toInt();
  emit signalMarkBoundingElem( zone );
}

void DlgCreateSurface::slotCreate()
{
  if( pTabs->currentWidget() == pTabSingle ) {
    SENDER_NAME_Q( "slotCreate:single" );

    if( outputMode == OUTPUTMODE::CARRE ) {
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::CARRE_INHIBITS );
      return;
    }

    if( ( pTabsSingle->currentWidget() == pTabByLevel &&
          pSbValue->value() == MAXDOUBLE ) ||
        ( pTabsSingle->currentWidget() == pTabByPoint &&
          ( pSbX->value() == MAXDOUBLE || pSbY->value() == MAXDOUBLE ) ) ) {
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::INVNUMBERS );
      return;
    }
  }
  else if( pTabs->currentWidget() == pTabMult ) {
    SENDER_NAME_Q( "slotCreate:multiple" );

    if( outputMode != OUTPUTMODE::CARRE &&
        ( pSbLevel1->value() == MAXDOUBLE ||
          pSbLevel2->value() == MAXDOUBLE ) ) {
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::INVNUMBERS );
      return;
    }
    else if( outputMode == OUTPUTMODE::CARRE ) {
      if( pCbLaw->currentIndex() != (int)DGLAW::DELTA ) {
        pConsole->Send( WND_ERROR, FULL_SENDER,
                        SM_DLG( DLG::DISTR_GRAPH::ERR::WRONG_CARRE_LAW ) );
        return;
      }
      pSbLevel1->setValue( 0. );
      pSbLevel2->setValue( 1. );
    }

    if( pSbLevel1->value() == pSbLevel2->value() ) {
      pConsole->Send( WND_ERROR, FULL_SENDER,
                      SM_DLG( DLG::CREATE_SURFACE::ERR::EQUAL_LEVELS ) );
      return;
    }
  }

  accept();
}

void DlgCreateSurface::slotReset()
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

void DlgCreateSurface::slotCopy()
{
  SENDER_NAME_Q( "slotCopy" );
  if( ppSX == null || (*ppSX) == null ) {
    pConsole->Send( WND_ERROR, FULL_SENDER,
                    SM_DLG( DLG::CREATE_SURFACE::ERR::NO_REMEMBERED_SURFACE ) );
    return;
  }

  CreatorId _crid = (*ppSX)->GetCreatorId();
  if( !_crid.IsGood() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER,
                    SM_DLG( DLG::CREATE_SURFACE::ERR::BAD_CREATOR_ID ) );
    return;
  }

  if( outputMode == OUTPUTMODE::CARRE && _crid.CarreFlag() == 0 ) {
    pConsole->Send( WND_ERROR, FULL_SENDER,
                    SM_DLG( DLG::CREATE_SURFACE::ERR::NON_CARRE_MODE ) );
    return;
  }

  for( int index = 0; index < pCbAreaM->count(); index++ ) {
    if( pCbAreaM->itemData( index ).toInt() == _crid.Area() ) {
      pCbAreaM->setCurrentIndex( index );
      break;
    }
  }
  if( _crid.CarreFlag() == 0 ) {
    pSbLevel1->setValue( _crid.level1 );
    pSbLevel2->setValue( _crid.level2 );
  }
  pDigr->setUpdatesEnabled( false );
  pSbCount->setValue( _crid.Count() + 1 );
  pSbAlpha1->setValue( _crid.Alpha1() );
  pSbAlpha2->setValue( _crid.Alpha2() );
  pDigr->setUpdatesEnabled( true );
  pCbLaw->setCurrentIndex( _crid.Law() );

  crid = _crid;
  if( outputMode != OUTPUTMODE::CARRE ) {
    pChRemoveOld->setEnabled( true );
    pChRemoveOld->setCheckState( Qt::Checked );
  }
}


bool DlgCreateSurface::IsSingleByLevel() const
{
  return pTabs->currentWidget() == pTabSingle &&
      pTabsSingle->currentWidget() == pTabByLevel;
}

bool DlgCreateSurface::IsSingleByPoint() const
{
  return pTabs->currentWidget() == pTabSingle &&
      pTabsSingle->currentWidget() == pTabByPoint;
}

bool DlgCreateSurface::IsMultiple() const
{
  return pTabs->currentWidget() == pTabMult;
}

int DlgCreateSurface::SingleArea() const
{
  return pCbAreaS->itemData( pCbAreaS->currentIndex() ).toInt();
}

double DlgCreateSurface::SingleLevel() const
{
  return pSbValue->value();
}

Point DlgCreateSurface::SinglePoint() const
{
  return Point( pSbX->value(), pSbY->value() );
}

CreatorId DlgCreateSurface::MultipleCreator() const
{
  int zone = pCbAreaM->itemData( pCbAreaM->currentIndex() ).toInt();
  return CreatorId( zone, pSbCount->value() - 1,
                    pSbAlpha1->value(), pSbAlpha2->value(),
                    pSbLevel1->value(), pSbLevel2->value(),
                    (DistrLaw)pCbLaw->currentIndex(),
                    (int)(outputMode == OUTPUTMODE::CARRE) );
}

bool DlgCreateSurface::RemoveOldOption() const
{
  return pChRemoveOld->checkState() == Qt::Checked;
}

const CreatorId& DlgCreateSurface::OldCreatorId() const
{
  return crid;
}

void DlgCreateSurface::setOutputMode( int _outputMode )
{
  outputMode = _outputMode;
  bool carre = (outputMode == OUTPUTMODE::CARRE);
  pSbLevel1->setDisabled( carre );
  pLbLevel1->setDisabled( carre );
  pBtnCopyL1->setDisabled( carre );
  pSbLevel2->setDisabled( carre );
  pLbLevel2->setDisabled( carre );
  pBtnCopyL2->setDisabled( carre );
  pChRemoveOld->setDisabled( carre );

  pBtnMarkBoundElems->setEnabled( carre );
}
