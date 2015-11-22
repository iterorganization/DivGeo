#include "DlgVsdEdit.h"

DlgVsdEdit::DlgVsdEdit( VarSetDefPtr _pVSD, ModelPtr _pModel,
                        StringsManager* _pSM, Console* _pConsole,
                        QWidget *parent) :
  QDialog( parent, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint ), //1411
  pVSD( _pVSD ),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole )
{

  QString sTitle = SM_DLG( DLG::VARSETDEFEDIT::TITLE_EX )
                   .arg( QString::fromStdString( pVSD->Name() ) );
  this->setWindowTitle( sTitle );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QFormLayout* pLoProp = new QFormLayout();
  pLoMain->addLayout( pLoProp );
  pLeDescr = new QLineEdit( QString::fromStdString( pVSD->Descr() ) );
  pLoProp->addRow( SM_DLG( DLG::VARSETDEFEDIT::LBL::DESCR ), pLeDescr );
  pLeMinVS = new QLineEdit( QString( "%1" ).arg( pVSD->MinVarSets() ) );
  pLoProp->addRow( SM_DLG( DLG::VARSETDEFEDIT::LBL::MIN ), pLeMinVS );
  pLeMaxVS = new QLineEdit( QString( "%1" ).arg( pVSD->MaxVarSets() ) );
  pLoProp->addRow( SM_DLG( DLG::VARSETDEFEDIT::LBL::MAX ), pLeMaxVS );

  pTwVarDefs = new QTableWidget();
  pTwVarDefs->horizontalHeader()->hide();
  pTwVarDefs->verticalHeader()->hide();
  pTwVarDefs->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pTwVarDefs->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pLoMain->addWidget( pTwVarDefs );

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
  connect( pBtnOk, SIGNAL(clicked()), this, SLOT(slotAcceptChanges()) );
  connect( pBtnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );

  connect( &smVarDefs, SIGNAL(mapped(int)), this, SLOT(slotEdit(int)) );
  RebuildGrid();
}

void DlgVsdEdit::slotAcceptChanges()
{
  SENDER_NAME_Q( "DlgVsdEdit::slotAcceptChanges" );
  bool result;
  int minVsCount = pLeMinVS->text().toInt( &result );
  if( !result ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
    return;
  }
  int maxVsCount = pLeMaxVS->text().toInt( &result );
  if( !result ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
    return;
  }
  if( minVsCount > maxVsCount ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::INVNUMBERS );
    return;
  }
  if( maxVsCount > 1 ) {
    FOREACHPTRCONST( VarDefPtr, pVD, pVSD->VarDefs() ) {
      if( HasAnyFlag( pVD->VarDefType(), VTF::SINGLE ) ) {
        pConsole->Send( WND_ERROR, SENDER, ERR::SINGLE );
        return;
      }
    }
  }
  std::string sDescr = pLeDescr->text().toStdString();
  if( sDescr.empty() )
    sDescr = "CLASSIFIED";
  pVSD->Change( pVSD->Name(), sDescr, minVsCount, maxVsCount );
  pModel->ActionStack().Complete( "VSD:Accept" );
  accept();
}

void DlgVsdEdit::RebuildGrid()
{
  /* Calculate grid dimensions */
  int rowsCount = 0;
  int columnsCount = 0;
  FOREACHPTRCONST( VarDefPtr, pVD, pVSD->VarDefs() ) {
    rowsCount = dg_max( rowsCount, pVD->Row()+1 );
    columnsCount = dg_max( columnsCount, pVD->Column()+1 );
  }
  if( ++rowsCount < 3 ) rowsCount = 3;
  if( ++columnsCount < 3 ) columnsCount = 3;
  pTwVarDefs->clear();
  pTwVarDefs->setRowCount( rowsCount );
  pTwVarDefs->setColumnCount( columnsCount );

  /* Build buttons-grid */
  const QString& crsEmpty = SM_DLG( DLG::VARSETDEFEDIT::LBL::EMPTY );
  for( int row = 0; row < rowsCount; row++ ) {
    for( int column = 0; column < columnsCount; column++ ) {
      QTableWidgetItem* pItem = new QTableWidgetItem();
      pTwVarDefs->setItem( row, column, pItem );
      QPushButton* pBtn = new QPushButton( crsEmpty );
      pTwVarDefs->setCellWidget( row, column, pBtn );
      smVarDefs.setMapping( pBtn, (row << 16) + column );
      connect( pBtn, SIGNAL(clicked()), &smVarDefs, SLOT(map()) );

      VarDefPtr pVD = pVSD->GetVarDefByPosition( row, column );
      if( pVD != null ) {
        pBtn->setText( QString::fromStdString( pVD->Name() ) );
        pItem->setData( Qt::UserRole, QVariant::fromValue( pVD ) );
      }
    }
  }

  pTwVarDefs->resizeColumnsToContents();
  pTwVarDefs->resizeRowsToContents();

  int contents_width = 0;
  for( int c = 0; c < pTwVarDefs->columnCount(); c++ )
    contents_width += pTwVarDefs->columnWidth(c);
  int contents_heigh = pTwVarDefs->rowCount() * pTwVarDefs->rowHeight(0);
  setFixedSize( contents_width + 24, contents_heigh + 140 );
}

void DlgVsdEdit::slotEdit( int _position )
{
  SENDER_NAME_Q( "DlgVsdEdit::slotEdit" );
  VarDefPtr pVD = null;

  int row = _position >> 16;
  int column = _position & 0x0000ffff;
  QTableWidgetItem* pItem = pTwVarDefs->item( row, column );
  if( pItem == null )
    return;
  QVariant qv = pItem->data( Qt::UserRole );
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarDefPtr" ) ) {
    DlgCreateName* pDialog = new DlgCreateName( DLG::VARDEFCREATE::TITLE, pSM, this );
    connect( pDialog, SIGNAL(sgnlHelp(int)), this, SIGNAL(sgnlHelp(int)) );

    forever {
      if( pDialog->exec() == QDialog::Rejected )
        break;

      QString sNewName = pDialog->GetName();
      char name[256];
      if( sscanf( sNewName.toUtf8().data(), "%s%s", name, name ) != 1 ) {
        pConsole->Send( WND_ERROR, SENDER, ERR::BADNAME );
        continue;
      }

      bool isDuplicated = false;
      FOREACHPTRCONST( VarDefPtr, pVD1, pVSD->VarDefs() ) {
        QString sName = QString::fromStdString( pVD1->Name() );
        if( sNewName == sName ) {
          pConsole->Send( WND_ERROR, SENDER, ERR::DUPNAME );
          isDuplicated = true;
          break;
        }
      }

      if( isDuplicated )
        continue;

      pVD = pVSD->AddVarDef( sNewName.toStdString(), "CLASSIFIED",
                             VT::FLOAT, 0, NPoint( column, row ) );
      pModel->ActionStack().Complete( "VSD:Edit" );

      break;
    }
  }
  else
    pVD = qv.value< VarDefPtr >();

  if( pVD == null )
    return;
  DlgVdEdit* pDialog = new DlgVdEdit( pVD, pModel, pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SIGNAL(sgnlHelp(int)) );
  pDialog->exec();
  RebuildGrid();
}
