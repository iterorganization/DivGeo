#include "DlgSettings.h"

DlgSettings::DlgSettings( ModelPtr _pModel, StringsManager* _pSM,
                          SettingsManager* _pSettings,
                          const QString& _crstitle, QWidget *_pParent ):
  QDialog( _pParent ),
  pModel( _pModel ),
  pSM( _pSM ),
  pSettings( _pSettings ),
  settings_default( *_pSettings )
{
  this->setWindowTitle( _crstitle );
  this->resize( 500, 500 );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  pTree = new QTreeWidget( this );
  pTree->setSelectionMode( QAbstractItemView::NoSelection );
  pTree->setFocusPolicy( Qt::NoFocus );
  pTree->setColumnCount( 2 );
  pTree->setTextElideMode( Qt::ElideRight );
  pTree->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  pTree->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pTree->setColumnWidth( 0, 250 );
  pTree->header()->hide();
  pTree->setStyleSheet( QString( "QTreeWidget, QTableWidget {background-color:%1}" )
                        .arg( palette().window().color().name() ) );
  pLoMain->addWidget( pTree );

  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  connect( pBtnCancel, SIGNAL(clicked()), this, SLOT(slotCancel()) );
  pLoButtons->addWidget( pBtnCancel );

  QPushButton* pBtnApply = new QPushButton( SM_DLG( DLG::BTN::APPLY ) );
  connect( pBtnApply, SIGNAL(clicked()), this, SLOT(slotApply()) );
  pLoButtons->addWidget( pBtnApply );

  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
  pLoButtons->addWidget( pBtnHelp );

  pSettings->Retrieve();
  RebuildTree();
}

PropertyTableWidget* DlgSettings::MakePropertyTable( QTreeWidgetItem* _pBranch,
                                                     const QString& _crsName )
{
  QTreeWidgetItem* pItem = new QTreeWidgetItem( QStringList( _crsName ) );
  _pBranch->addChild( pItem );

  PropertyTableWidget* pTable = new PropertyTableWidget( pSM, pSettings );
  QTreeWidgetItem* pChild = new QTreeWidgetItem();
  pItem->addChild( pChild );
  pTree->setItemWidget( pChild, 0, pTable );
  return pTable;
}

void DlgSettings::RebuildTree()
{
  ulong depth = 0;
  PropertyTableWidget* pTable = null;
  QTreeWidgetItem* pBranch = null;
  PTW::PreviewType previewType = PREVIEW::UNKNOWN;

  foreach( const SettingsManager::PropertyRecord& rProperty, pSettings->Table() ) {
    QString sName = (rProperty.descrCode == 0) ? "" :
                    SM_DLG( rProperty.descrCode );
    switch( rProperty.type ) {
    case SettingsManager::PR::PT::BEGIN: {
      if( pBranch == null )
        pBranch = new QTreeWidgetItem( pTree, QStringList( SM_DLG( rProperty.descrCode ) ) );
      pTable = new PropertyTableWidget( pSM, pSettings, rProperty.updateAll );
      QTreeWidgetItem* pChild = new QTreeWidgetItem();
      if( !rProperty.vUserData.isEmpty() ) {
        previewType = (PTW::PreviewType)rProperty.vUserData[0];
        tables.append( pTable );
      }
      else
        previewType = PREVIEW::UNKNOWN;
      if( depth == 0 )
        pBranch->addChild( pChild );
      else if( depth == 1 ) {
        QTreeWidgetItem* pSubBranch = new QTreeWidgetItem( QStringList( sName ) );
        pBranch->addChild( pSubBranch );
        pSubBranch->addChild( pChild );
      }
      else {
        pBranch->setText( 0, "PropertiesTree code error!!!" );
        pBranch->setTextColor( 0, Qt::red );
      }
      pTree->setItemWidget( pChild, 0, pTable );
      depth++;
      break;
    }
    case SettingsManager::PR::PT::END: {
      depth--;
      if( previewType != PREVIEW::UNKNOWN )
        pTable->SetupPreview( previewType );
      if( depth == 0 )
        pBranch = null;
      break;
    }
    case SettingsManager::PR::PT::PEN: {
      QPen* pPen = rProperty.dataPtr.value< QPenPtr >();
      if( pPen == null ) break;
      pTable->AddPenProperty( sName, pPen,
         rProperty.descrCode == DLG::SETTINGS::VISUAL::EQUIL::POS );
      break;
    }
    case SettingsManager::PR::PT::FONT: {
      QPen* pPen = rProperty.dataPtr.value< QPenPtr >();
      if( pPen == null ) break;
      pTable->AddFontProperty( sName, pPen );
      break;
    }
    case SettingsManager::PR::PT::COLOR: {
      QColor* pColor = rProperty.dataPtr.value< QColorPtr >();
      if( pColor == null ) break;
      pTable->AddColorProperty( sName, pColor );
      break;
    }
    case SettingsManager::PR::PT::INT: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      pTable->AddRangedProperty( sName, pInt, rProperty.vUserData[0],
                                              rProperty.vUserData[1] );
      break;
    }
    case SettingsManager::PR::PT::FLOAT: {
      float* pFloat = rProperty.dataPtr.value< FloatPtr >();
      if( pFloat == null ) break;
      pTable->AddRangedProperty( sName, pFloat, (float)rProperty.vUserData[0],
                                                (float)rProperty.vUserData[1] );
      break;
    }
    case SettingsManager::PR::PT::BOOL: {
      bool* pBool = rProperty.dataPtr.value< BoolPtr >();
      if( pBool == null ) break;
      pTable->AddCheckProperty( sName, pBool );
      break;
    }
    case SettingsManager::PR::PT::NPOINT: {
      NPoint* pPoint = rProperty.dataPtr.value< NPointPtr >();
      if( pPoint == null ) break;
      pTable->AddVectorProperty( sName, pPoint, NPoint( rProperty.vUserData[0] ),
                                            NPoint( rProperty.vUserData[1] ) );
      break;
    }
    case SettingsManager::PR::PT::LIST: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      QStringList items;
      foreach( int code, rProperty.vUserData )
        items.append( ( code == 0 ) ? "" : SM_DLG( code ) );
      pTable->AddListProperty( sName, pInt, items );
      break;
    }
    case SettingsManager::PR::PT::FLAGS: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      QStringList items;
      foreach( int code, rProperty.vUserData )
        items.append( ( code == 0 ) ? "" : SM_DLG( code ) );
      pTable->AddFlagsProperty( sName, pInt, items );
      break;
    }
    case SettingsManager::PR::PT::UNKNOWN:
      break;
    }

    if( pTable != null && pTable->UpdateAll() )
      connect( pTable, SIGNAL(sgnlPropertyChanged()),
               this,   SLOT(slotUpdateAllPreviews()) );
  }

  pTree->expandAll();
}

void DlgSettings::slotUpdateAllPreviews()
{
  foreach ( PropertyTableWidget* pWgt, tables )
    pWgt->UpdatePreview();
}

void DlgSettings::slotCancel()
{
  *pSettings = settings_default;
  reject();
}

void DlgSettings::slotApply()
{
  pSettings->Apply( pModel );
  pSettings->Save( pSettings->IniPath() );
  accept();
}


