#include "DlgInvalidVars.h"
#include "../main/MainWnd.h"

DlgInvalidVars::DlgInvalidVars(ModelPtr _pModel, CViewWndPtr _pView,
                               StringsManager* _pSM, Console* _pConsole,
                               QWidget *parent):
  QDialog( parent ),
  pModel( _pModel ),
  pView( _pView ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  this->setWindowTitle( SM_DLG( DLG::INVALIDVARS::TITLE ) );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pList = new QListWidget( this );
  pList->setSelectionMode( QAbstractItemView::SingleSelection );
  pLoMain->addWidget( pList );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnShow = new QPushButton( SM_DLG( DLG::INVALIDVARS::BTN::SHOW ) );
  pLoButtons->addWidget( pBtnShow );
  pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnShow,  SIGNAL(clicked()), this, SLOT(slotShowVar()) );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnHelp,  SIGNAL(clicked()), this, SLOT(slotHelp()) );

  RebuildList();
}

void DlgInvalidVars::slotShowVar()
{
  QListWidgetItem* pItem = pList->currentItem();
  if( pItem == null )
    return;
  QVariant qv_vd = pItem->data( Qt::UserRole );
  QVariant qv_vs = pItem->data( Qt::UserRole+1 );
  if( qv_vd.isNull() || qv_vd.userType() != QMetaType::type( "VarDefPtr" ) ||
      qv_vs.isNull() || qv_vs.userType() != QMetaType::type( "VarSetPtr" ) )
    return;
  VarDefPtr pVD = qv_vd.value< VarDefPtr >();
  VarSetPtr pVS = qv_vs.value< VarSetPtr >();

  int err = 0;
  IComponentList objects;
  if( pModel->Vars()->CheckVar( &objects, pVD, pVS, &err ) == 0 ) { /* Should never happen */
    objects.clear();
    RebuildList();
    return;
  }

  pModel->UnmarkAll();
  if( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) ) {
    IComponentList objects_val;
    ConvertVarItemList( pModel->Vars()->GetVar( pVS, pVD, pVS ).ListRef(), objects_val );
    pModel->MarkGroup( objects_val );
    if( !objects.empty() )
      pView->CurrentScene()->CreateLabelItem(
            objects.front(), SM_MSG( STR::ERRLABEL  ) );
  }
  else
    pModel->MarkGroup( objects );

  pModel->ActionStack().Complete( "Invalid:ShowVar" );

  //if( err != 0 )
  //  pConsole->Send( WND_ERROR, "DlgInvalidVars::slotShowVar", err );

  emit CreateVarsEditDlg( pVS );

  pModel->ActionStack().Complete( "Invalid:???" );
}

void DlgInvalidVars::RebuildList()
{
  pList->clear();

  VarPairList errPairs;
  pModel->Vars()->CheckAllVars( &errPairs );

  for( VarPairList::iterator it = errPairs.begin(), itEnd = errPairs.end();
       it != itEnd; ++it ) {
    QString name = QString::fromStdString( it->pVD->Descr() ) +
                   QString( "\n(%1)" ).arg( SM_MSG(it->error) );
    QListWidgetItem* pItem = new QListWidgetItem( name );
    pItem->setData( Qt::UserRole,   QVariant::fromValue( it->pVD ) );
    pItem->setData( Qt::UserRole+1, QVariant::fromValue( it->pVS ) );
    pList->addItem( pItem );
  }
  errPairs.clear();
  if( pList->count() > 0 )
    pList->setCurrentRow( 0 );
}
