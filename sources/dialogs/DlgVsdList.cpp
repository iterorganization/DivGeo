#include "DlgVsdList.h"

DlgVsdList::DlgVsdList( ModelPtr _pModel, StringsManager* _pSM,
                        Console* _pConsole, QWidget* parent):
  QDialog(parent),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::VARSETDEFLIST::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  pLwVars = new QListWidget();
  pLwVars->setSelectionMode( QAbstractItemView::SingleSelection );
  pLoMain->addWidget( pLwVars );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnAdd = new QPushButton( SM_DLG( DLG::VARSETDEFLIST::BTN::ADD ) );
  pLoButtons->addWidget( pBtnAdd );
  pBtnModify = new QPushButton( SM_DLG( DLG::VARSETDEFLIST::BTN::MODIFY ) );
  pLoButtons->addWidget( pBtnModify );
  pBtnRemove = new QPushButton( SM_DLG( DLG::VARSETDEFLIST::BTN::REMOVE ) );
  pLoButtons->addWidget( pBtnRemove );
  pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnAdd, SIGNAL(clicked()), this, SLOT(slotAdd()) );
  connect( pBtnModify, SIGNAL(clicked()), this, SLOT(slotModify()) );
  connect( pBtnRemove, SIGNAL(clicked()), this, SLOT(slotRemove()) );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );

  RebuildList();

  connect( &smEditDialogs, SIGNAL(mapped(QString)),
           this, SLOT(slotCloseEditDialog(QString)) );
}

void DlgVsdList::RebuildList()
{
  pLwVars->clear();
  FOREACHPTRCONST( VarSetDefPtr, pVSD, pModel->Vars()->VarSetDefs() ) {
    QString sName = QString::fromStdString( pVSD->Name() );
    QListWidgetItem* pItem = new QListWidgetItem( sName );
    pItem->setData( Qt::UserRole, QVariant::fromValue( pVSD ) );
    pLwVars->addItem( pItem );
  }
  pLwVars->setCurrentRow( 0 );
}

void DlgVsdList::slotAdd()
{
  SENDER_NAME_Q( "DlgVsdList::slotAdd" );
  DlgCreateName* pDialog = new DlgCreateName( DLG::VARSETDEFCREATE::TITLE, pSM, this );
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
    FOREACHPTRCONST( VarSetDefPtr, pVSD, pModel->Vars()->VarSetDefs() ) {
      QString sName = QString::fromStdString( pVSD->Name() );
      if( sNewName == sName ) {
        pConsole->Send( WND_ERROR, SENDER, ERR::DUPNAME );
        isDuplicated = true;
        break;
      }
    }

    if( isDuplicated )
      continue;

    VarSetDefPtr pVSD_new = pModel->Vars()->AddVarSetDef( sNewName.toStdString(), "CLASSIFIED", 0, 1 );
    pModel->ActionStack().Complete( "VsdList:Add" );

    QListWidgetItem* pItem = new QListWidgetItem( sNewName );
    pItem->setData( Qt::UserRole, QVariant::fromValue( pVSD_new ) );
    pLwVars->addItem( pItem );
    break;
  }
}

void DlgVsdList::slotModify()
{
  VarSetDefPtr pVSD = RetrieveVSD();
  if( pVSD == null )
    return;
  QString sName = QString::fromStdString( pVSD->Name() );
  DlgVsdEdit* pDialog = mapEditDialogs.value( sName, null );
  if( pDialog != null ) {
    pDialog->show();
    pDialog->raise();
    pDialog->setFocus();
  }
  else {
    pDialog = new DlgVsdEdit( pVSD, pModel, pSM, pConsole, this );
    connect( pDialog, SIGNAL(sgnlHelp(int)), this, SIGNAL(sgnlHelp(int)) );
    mapEditDialogs.insert( sName, pDialog );
    smEditDialogs.setMapping( pDialog, sName );
    connect( pDialog, SIGNAL(finished(int)), &smEditDialogs, SLOT(map()) );
    pDialog->show();
  }
}

void DlgVsdList::slotRemove()
{
  VarSetDefPtr pVSD = RetrieveVSD();
  pVSD->Delete();
  pLwVars->removeItemWidget( pLwVars->currentItem() );
  pModel->ActionStack().Complete( "VsdList:Remove" );
}

VarSetDefPtr DlgVsdList::RetrieveVSD()
{
  SENDER_NAME_Q( "DlgVsdList::RetrieveVSD" );
  QListWidgetItem* pItem = pLwVars->currentItem();
  if( pItem == null )
    return null;

  QVariant qv = pItem->data( Qt::UserRole );
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarSetDefPtr" ) )
    return null;

  VarSetDefPtr pVSD = qv.value< VarSetDefPtr >();
  if( pVSD == null )
    return null;

  if( pVSD->IsLocked() ) {
    QString sArg = QString::fromStdString( pVSD->GetLockingObject()->Description() );
    pConsole->Send( WND_ERROR, SENDER, ERR::USEDBY, QStringList( sArg ) );
    return null;
  }

  return pVSD;
}

void DlgVsdList::slotCloseEditDialog( const QString& _crsName )
{
  mapEditDialogs.remove( _crsName );
}
