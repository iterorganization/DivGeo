#include "DlgTopology.h"

DlgTopology::DlgTopology( ModelPtr _pModel, StringsManager* _pSM, Console* _pConsole,
                          QWidget *parent ) :
  QDialog(parent),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole ),
  pObject( null )
{
  this->setWindowTitle( SM_DLG( DLG::TOPOLOGY::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pTabs = new QTabWidget( this );
  pLoMain->addWidget( pTabs );

  /* Tab XPoints */
  QWidget* pTabXPoints = new QWidget();
  pTabs->addTab( pTabXPoints, SM_DLG( DLG::TOPOLOGY::XPOINTS::TITLE ) );
  QVBoxLayout* pLoXPoints = new QVBoxLayout( pTabXPoints );

  const QString& crsBtnDAXP = SM_DLG( DLG::TOPOLOGY::XPOINTS::BTN::DETECT_ALL );
  QPushButton* pBtnDetectAllXPoints = new QPushButton( crsBtnDAXP );
  pLoXPoints->addWidget( pBtnDetectAllXPoints );
  connect( pBtnDetectAllXPoints,  SIGNAL(clicked()),
           this,                  SLOT(slotDetectAllXPoints()) );

  QHBoxLayout* pLoXPoints_2 = new QHBoxLayout();
  pLoXPoints->addLayout( pLoXPoints_2 );

  const QString& crsLblLevel = SM_DLG( DLG::TOPOLOGY::XPOINTS::LBL::XPT_LEVEL );
  QLabel* pLblLevel = new QLabel( crsLblLevel );
  pLoXPoints_2->addWidget( pLblLevel );

  pLeXPLevel = new QLineEdit();
  pLoXPoints_2->addWidget( pLeXPLevel );

  const QString& crsBtnChangeXP = SM_DLG( DLG::TOPOLOGY::XPOINTS::BTN::CHANGE );
  pBtnChangeXPoint = new QPushButton( crsBtnChangeXP );
  pBtnChangeXPoint->setDisabled( true );
  pLoXPoints_2->addWidget( pBtnChangeXPoint );
  connect( pBtnChangeXPoint,  SIGNAL(clicked()),
           this,              SLOT(slotChangeXPoint()) );

  pLoXPoints->addSpacerItem(
        new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );


  /* Tab Sections */
  QWidget* pTabSections = new QWidget();
  pTabs->addTab( pTabSections, SM_DLG( DLG::TOPOLOGY::SECTIONS::TITLE ) );
  QVBoxLayout* pLoSections = new QVBoxLayout( pTabSections );

  const QString& crsBtnDAS = SM_DLG( DLG::TOPOLOGY::SECTIONS::BTN::DETECT_ALL );
  QPushButton* pBtnDetectAllSections = new QPushButton( crsBtnDAS );
  pLoSections->addWidget( pBtnDetectAllSections );
  connect( pBtnDetectAllSections, SIGNAL(clicked()),
           this,                  SLOT(slotDetectAllSections()) );

  QLabel* pLblSx = new QLabel( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::TITLE ) );
  pLoSections->addWidget( pLblSx );

  QFormLayout* pLoSectProp = new QFormLayout();
  pLoSections->addLayout( pLoSectProp );

  pLeSNumber = new QLineEdit();
  pLoSectProp->addRow( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::ID ), pLeSNumber );
  pLeSName = new QLineEdit();
  pLoSectProp->addRow( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::SHORT ), pLeSName );
  pLeSDescr = new QLineEdit();
  pLoSectProp->addRow( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::LONG ), pLeSDescr );

  pChRevTarget = new QCheckBox( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::TARGETCW ) );
  pChHasGP = new QCheckBox( SM_DLG( DLG::TOPOLOGY::SECTIONS::LBL::USED ) );
  pLoSectProp->addRow( pChRevTarget, pChHasGP );

  const QString& crsBtnChSect = SM_DLG( DLG::TOPOLOGY::SECTIONS::BTN::CHANGE );
  pBtnChangeSection = new QPushButton( crsBtnChSect );
  pLoSections->addWidget( pBtnChangeSection );
  connect( pBtnChangeSection, SIGNAL(clicked()),
           this,              SLOT(slotChangeSection()) );

  const QString& crsBtnRevert = SM_DLG( DLG::TOPOLOGY::SECTIONS::BTN::REVERT );
  pBtnChangeDirection = new QPushButton( crsBtnRevert );
  pBtnChangeDirection->setDisabled( true );
  pLoSections->addWidget( pBtnChangeDirection );
  connect( pBtnChangeDirection, SIGNAL(clicked()),
           this,                SLOT(slotRevertSection()) );

  pLoSections->addSpacerItem(
        new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );


  /* Tab Zones */
  QWidget* pTabZones = new QWidget();
  pTabs->addTab( pTabZones, SM_DLG( DLG::TOPOLOGY::ZONES::TITLE ) );
  QVBoxLayout* pLoZones = new QVBoxLayout( pTabZones );

  pCbZones = new QComboBox();
  pLoZones->addWidget( pCbZones );
  connect( pCbZones,  SIGNAL(currentIndexChanged(int)),
           this,      SLOT(slotZoneSelectionChanged(int)) );

  QLabel* pLblZones = new QLabel( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::TITLE ) );
  pLoZones->addWidget( pLblZones );

  QFormLayout* pLoZoneProp = new QFormLayout();
  pLoZones->addLayout( pLoZoneProp );

  pLeZNumber = new QLineEdit();
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::ID ), pLeZNumber );
  pLeZName = new QLineEdit();
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::SHORT ), pLeZName );
  pLeZDescr = new QLineEdit();
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::LONG ), pLeZDescr );
  pLeZBaseSect = new QLineEdit();
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::GPS1 ), pLeZBaseSect );
  pLeZLimSect = new QLineEdit();
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::GPS2 ), pLeZLimSect );

  pCbZOrient = new QComboBox();
  pCbZOrient->addItems( QStringList()
                        << SM_DLG( DLG::TOPOLOGY::ZONES::LBL::LEFT )
                        << SM_DLG( DLG::TOPOLOGY::ZONES::LBL::RIGHT ) );
  pLoZoneProp->addRow( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::ORIENT ), pCbZOrient );
  pChClosed = new QCheckBox( SM_DLG( DLG::TOPOLOGY::ZONES::LBL::CLOSED ) );
  pLoZoneProp->addRow( null, pChClosed );

  const QString& crsBtnSet = SM_DLG( DLG::TOPOLOGY::ZONES::BTN::SET );
  QPushButton* pBtnSet = new QPushButton( crsBtnSet );
  pLoZones->addWidget( pBtnSet );
  connect( pBtnSet, SIGNAL(clicked()),
           this,    SLOT(slotZoneSet()) );

  const QString& crsBtnDelete = SM_DLG( DLG::TOPOLOGY::ZONES::BTN::DELETE );
  pBtnDelete = new QPushButton( crsBtnDelete );
  pLoZones->addWidget( pBtnDelete );
  connect( pBtnDelete, SIGNAL(clicked()),
           this,       SLOT(slotZoneDelete()) );

  pLoZones->addSpacerItem(
        new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

  RebuildZoneList();

  /* Tab Name */
  QWidget* pTabName = new QWidget();
  pTabs->addTab( pTabName, SM_DLG( DLG::TOPOLOGY::NAME::TITLE ) );
  QVBoxLayout* pLoName = new QVBoxLayout( pTabName );

  QHBoxLayout* pLoName_2 = new QHBoxLayout();
  pLoName->addLayout( pLoName_2 );

  const QString& crsLblName = SM_DLG( DLG::TOPOLOGY::NAME::LBL::TITLE );
  QLabel* pLblName = new QLabel( crsLblName );
  pLoName_2->addWidget( pLblName );

  pLeName = new QLineEdit( QString::fromStdString( pModel->TopologyName() ) );
  pLoName_2->addWidget( pLeName );
  connect( pLeName, SIGNAL(textChanged(QString)),
           this,    SLOT(slotNameChanged()) );

  const QString& crsBtnSetName = SM_DLG( DLG::TOPOLOGY::NAME::BTN::SET );
  pBtnSetName = new QPushButton( crsBtnSetName );
  pBtnSetName->setDisabled( true );
  pLoName_2->addWidget( pBtnSetName );
  connect( pBtnSetName,  SIGNAL(clicked()),
           this,         SLOT(slotSetTopoName()) );


  pLoName->addSpacerItem(
        new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );

  /* Adjust window width */

  int contents_width = 0;
  for( int index = 0; index < pTabs->tabBar()->count(); index++ )
    contents_width += pTabs->tabBar()->tabRect( index ).width();
  setFixedWidth( contents_width + 24 );

}


void DlgTopology::LoadSelectedObject( IComponentPtr _pObject )
{
  pObject = _pObject;
  pBtnChangeXPoint->setDisabled( true );
  pBtnChangeDirection->setDisabled( true );

  switch( pObject->Type() ) {
  case OT::XPOINTTEST: {
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );
    pBtnChangeXPoint->setEnabled( true );
    pTabs->setCurrentIndex( 0 );
    pLeXPLevel->setText( QString( "%1" ).arg( pXPT->Level() ) );
    break;
  }
  case OT::XPOINTSEG: {
    XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );
    pBtnChangeDirection->setEnabled( true );
    pBtnChangeSection->setEnabled( true );
    pTabs->setCurrentIndex( 1 );
    GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSegBySegment( pXPS );
    assert( pGPS != null );
    pLeSNumber->setText( QString( "%1" ).arg( pGPS->Zone() ) );
    pLeSName->setText( QString::fromStdString( pGPS->ShortName() ) );
    pLeSDescr->setText( QString::fromStdString( pGPS->LongName() ) );
    pChHasGP->setChecked( pGPS->IsUsed() );
    pChRevTarget->setChecked( HasAnyFlag( pGPS->Flags(), GPSF::TARGET_CW ) );
    break;
  }
  default:
    break;
  }
}

void DlgTopology::slotDetectAllXPoints()
{
  TopologyPtr pTopology = pModel->GetFluxModel()->GetTopology();
  pTopology->DetectXPoints();
  pTopology->RecalcAllGridPointSegSLines();
  pTopology->RecalcCache();
  pModel->ActionStack().Complete( "Topology:DetectXPoints" );
}

void DlgTopology::slotChangeXPoint()
{
  SENDER_NAME_Q( "DlgTopology::slotChangeXPoint" );
  if( pObject == null || pObject->Type() != OT::XPOINTTEST ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
    return;
  }
  XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );

  bool isOk = false;
  double level = pLeXPLevel->text().toDouble( &isOk );
  if( !isOk ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::BAD_XPT_LEVEL );
    return;
  }

  NPoint c1 = pXPT->EndPos(1);
  NPoint c2 = pXPT->EndPos(2);
  int id = pXPT->ID();

  pXPT->Delete();
  pXPT = pModel->GetFluxModel()->GetTopology()->AddXPointTest( c1, c2, level, id );

  if( pXPT == null ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::BAD_NEW_LEVEL );
    pModel->ActionStack().Cancel();
    return;
  }

  pModel->ActionStack().Complete( "Topology:ChangeXPoint" );
  pBtnChangeXPoint->setDisabled( true );
  pConsole->Send( LOG_INFO, SENDER, DLG::TOPOLOGY::MSG::XPT_LEVEL_ADJUSTED );
}

void DlgTopology::slotDetectAllSections()
{
  TopologyPtr pTopology = pModel->GetFluxModel()->GetTopology();
  pTopology->DetectXPointSegs();
  pTopology->RecalcAllGridPointSegSLines();
  pModel->ActionStack().Complete( "Topology:DetectSections" );
}

void DlgTopology::slotChangeSection()
{
  SENDER_NAME_Q( "DlgTopology::slotChangeSection" );
  if( pObject == null || pObject->Type() != OT::XPOINTSEG ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
    return;
  }
  XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );
  GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSegBySegment( pXPS );
  assert( pGPS != null );

  bool isOk = false;
  int zone = pLeSNumber->text().toInt( &isOk );
  if( !isOk ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::BAD_XPS_ZONE );
    return;
  }

  if( pModel->GetFluxModel()->GetTopology()->HasDuplicatedSegments( zone, pGPS ) ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::DUP_XPS_ZONE );
    return;
  }

  pGPS->SetZone( zone );
  std::string name = pLeSName->text().toStdString();
  pGPS->SetShortName( name );
  std::string descr = pLeSDescr->text().toStdString();
  pGPS->SetLongName( descr );

  long flags = pGPS->Flags() & ~GPSF::USED & ~ GPSF::TARGET_CW;
  if( pChHasGP->isChecked() )
    flags |= GPSF::USED;
  if( pChRevTarget->isChecked() )
    flags |= GPSF::TARGET_CW;
  pGPS->SetFlags( flags );

  if( !HasAnyFlag( flags, GPSF::USED ) )
    pModel->GetFluxModel()->RemoveGridPointsByArea( zone );

  pModel->ActionStack().Complete( "Topology:ChangeSection" );
  pBtnChangeSection->setDisabled( true );
  pConsole->Send( LOG_INFO, SENDER, DLG::TOPOLOGY::MSG::XSEG_CHANGED );
}

void DlgTopology::slotRevertSection()
{
  SENDER_NAME_Q( "DlgTopology::slotRevertSection" );
  if( pObject == null || pObject->Type() != OT::XPOINTSEG ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
    return;
  }
  XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );
  GridPointSegPtr pGPS = pModel->GetFluxModel()->GetTopology()->FindGridPointSegBySegment( pXPS );
  assert( pGPS != null );

  pGPS->SetDir( -pGPS->Dir() );

  pModel->ActionStack().Complete( "Topology:RevertSection" );
  pConsole->Send( LOG_INFO, SENDER, DLG::TOPOLOGY::MSG::XSEG_REVERTED );
}

void DlgTopology::RebuildZoneList()
{
  pCbZones->clear();
  pCbZones->addItem( SM_DLG( DLG::TOPOLOGY::ZONES::BTN::ADD ) );
  FOREACHPTRCONST( SurfaceZonePtr, pSZ, pModel->GetFluxModel()->GetTopology()->SurfaceZones() ) {
    QString title = QString( "%1 (%2)" ).arg( pSZ->Zone() )
                    .arg( QString::fromStdString( pSZ->LongName() ) );
    pCbZones->addItem( title, QVariant::fromValue( pSZ ) );
  }
  pCbZones->setCurrentIndex( 0 );
}

void DlgTopology::slotZoneSelectionChanged( int _index )
{
  bool createNew = false;
  if( _index != 0 ) {
    QVariant qv = pCbZones->itemData( _index, Qt::UserRole );
    if( qv.isNull() || qv.userType() != QMetaType::type( "SurfaceZonePtr" ) )
      createNew = true;
    else {
      SurfaceZonePtr pSZ = qv.value< SurfaceZonePtr >();
      pLeZNumber->setText( QString( "%1" ).arg( pSZ->Zone() ) );
      pLeZName->setText( QString::fromStdString( pSZ->ShortName() ) );
      pLeZDescr->setText( QString::fromStdString( pSZ->LongName() ) );
      pLeZBaseSect->setText( QString( "%1" ).arg( pSZ->GPZone(1) ) );
      if( pSZ->GPZone(2) < 0 )
        pLeZLimSect->clear();
      else
        pLeZLimSect->setText( QString( "%1" ).arg( pSZ->GPZone(2) ) );
      pCbZOrient->setCurrentIndex( pSZ->Orient() > 0 ? 0 : 1 );
      pChClosed->setChecked( pSZ->IsLimited() );
      pObject = pSZ;
    }
  }
  else
    createNew = true;

  if( createNew ) {
    pObject = null;
    pLeZNumber->clear();
    pLeZName->clear();
    pLeZDescr->clear();
    pLeZBaseSect->clear();
    pLeZLimSect->clear();
  }

  pBtnDelete->setEnabled( _index > 0 );
}

void DlgTopology::slotZoneSet()
{
  SENDER_NAME_Q( "DlgTopology::slotZoneSet" );

  bool createNew = (pCbZones->currentIndex() == 0);
  if( !createNew && pObject->Type() != OT::SURFACEZONE ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
    return;
  }

  bool isOk = false;
  int zone = pLeZNumber->text().toInt( &isOk );
  if( !isOk || zone < 0 ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::BAD_SZONE );
    return;
  }

  TopologyPtr pTopology = pModel->GetFluxModel()->GetTopology();

  SurfaceZonePtr pSZ = createNew ? null : dgtype_cast< SurfaceZonePtr >( pObject );
  if( pTopology->HasDuplicatedZones( zone, pSZ ) ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::DUP_SZONE );
    return;
  }

  bool isOk1 = true, isOk2 = true;
  int gps1_id = pLeZBaseSect->text().toInt( &isOk1 );
  int gps2_id = pLeZLimSect->text().isEmpty() ? -1 : pLeZLimSect->text().toInt( &isOk2 );

  int orient = pCbZOrient->currentIndex() == 0 ? 1 : -1;

  if( !isOk1 || !isOk2 || gps2_id < -1 ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::SZ_BAD_GP_ZONE );
    return;
  }

  if( pTopology->FindGridPointSeg( gps1_id ) == null ||
      ( gps2_id > 0 && pTopology->FindGridPointSeg( gps2_id ) == null ) ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::SZ_NOT_FOIND_GP_ZONE);
    return;
  }

  long flags = 0;
  if( pChClosed->isChecked() )
    flags |= SZF::LIMITBYSURFACE;

  if( pSZ != null )
    pSZ->Delete();

  pSZ = pTopology->AddSurfaceZone( zone, gps1_id, gps2_id, orient, null );
  pSZ->SetFlags( flags );
  pSZ->SetShortName( pLeZName->text().toStdString() );
  pSZ->SetLongName( pLeZDescr->text().toStdString() );

  RebuildZoneList();

  pModel->ActionStack().Complete( "Topology:ZoneSet" );
  pConsole->Send( LOG_INFO, SENDER, DLG::TOPOLOGY::MSG::SZ_CHANGED );
}

void DlgTopology::slotZoneDelete()
{
  SENDER_NAME_Q( "DlgTopology::slotZoneDelete" );
  if( pObject == null || pObject->Type() != OT::SURFACEZONE ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
    return;
  }
  pObject->Delete();

  RebuildZoneList();

  pModel->ActionStack().Complete( "Topology:ZoneDelete" );
}

void DlgTopology::slotNameChanged()
{
  pBtnSetName->setDisabled( !pLeName->text().isEmpty() );
}

void DlgTopology::slotSetTopoName()
{
  SENDER_NAME_Q( "DlgTopology::slotSetTopoName" );
  QString sName = pLeName->text();
  char aName[4096];

  if( sscanf( sName.toUtf8().data(), "%s%s", aName, aName ) != 1 ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::TOPOLOGY::ERR::BAD_TOPO_NAME );
    return;
  }

  pModel->SetTopologyName( sName.toStdString() );
  pModel->ActionStack().Complete( "Topology:SetTopoName" );
  pConsole->Send( LOG_INFO, SENDER, DLG::TOPOLOGY::MSG::TOPO_NAME_CHANGED );
}
