#include "ModelInfo.h"
#include "MainWnd.h"

ModelInfo::ModelInfo( StringsManager *_pSM, QWidget *parent ) :
  QDockWidget( parent ),
  ModelTree( _pSM ),
  pObject( null )
{
  setFeatures( QDockWidget::DockWidgetMovable );
  setMinimumWidth( 220 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QWidget* titleWidget = new QWidget( parent );
  setTitleBarWidget( titleWidget );

  dockWidgetContents = new ResizableWidget();
  dockWidgetContents->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  dockWidgetContents->SetCustomHeight( 1000 );
  setWidget(dockWidgetContents);
  QVBoxLayout *pVerticalLayout = new QVBoxLayout( dockWidgetContents );
  pVerticalLayout->setMargin( 1 );

  pTree = new QTreeWidget( this );
  pTree->setColumnCount( 2 );
  //pTree->header()->hide();
  pTree->setTextElideMode( Qt::ElideRight );
  pTree->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  pTree->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pTree->setHeaderLabels( QStringList()
                          << SM_GUI( GUI::MODEL_INFO::PROPERTY )
                          << SM_GUI( GUI::MODEL_INFO::VALUE ) );
  pVerticalLayout->addWidget( pTree );

  QFont treeFont = pTree->font();
  treeFont.setBold( true );

  pSelectBranch   = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::SELECTED ) ) );
  pSelectBranch->setFont( 0, treeFont );  
  pModelBranch    = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::TITLE ) ) );
  pModelBranch->setFont( 0, treeFont );
  pTopologyBranch = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::TOPOLOGY::TITLE ) ) );
  pTopologyBranch->setFont( 0, treeFont );
  pEquilBranch    = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::EQUIL::TITLE ) ) );
  pEquilBranch->setFont( 0, treeFont );
  pMeshBranch     = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::MESH::TITLE ) ) );
  pMeshBranch->setFont( 0, treeFont );

  pTree->addTopLevelItem( pSelectBranch   );
  pTree->addTopLevelItem( pModelBranch    );
  pTree->addTopLevelItem( pTopologyBranch );
  pTree->addTopLevelItem( pEquilBranch    );
  pTree->addTopLevelItem( pMeshBranch     );

  UpdateContentModel( pModel );
  UpdateContentSelected( pObject );

  pTree->connect( pTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
           this, SLOT(UpdateItem(QTreeWidgetItem*)) );
}




void ModelInfo::UpdateContentSelected( IComponentPtr _pObject )
{
  if( _pObject == null ) {
    pObject = _pObject;
    pSelectBranch->setText( 1, SM_GUI( GUI::MODEL_INFO::NO_SELECTED ) );
    pSelectBranch->setDisabled( true );
    pSelectBranch->setExpanded( false );
    Clear( pSelectBranch );
    return;
  }

  bool typeChanged = ( _pObject == null || pObject == null ||
                       _pObject->Type() != pObject->Type() );
  pObject = _pObject;

  QString str_type = AddObjectDescr( pSelectBranch, pObject, 2, typeChanged );

  pSelectBranch->setText( 1, str_type );
  pSelectBranch->setDisabled( false );
  pSelectBranch->setExpanded( true );
}

void ModelInfo::UpdateContentModel( ModelPtr _pModel )
{
  const QString& str_not_loaded = SM_GUI( GUI::MODEL_INFO::NOT_LOADED );
  if( _pModel == null ) {
    pModel = _pModel;
    pModelBranch->setText( 1, str_not_loaded );
    pModelBranch->setDisabled( true );
    pModelBranch->setExpanded( false );
    pTopologyBranch->setText( 1, str_not_loaded );
    pTopologyBranch->setDisabled( true );
    pTopologyBranch->setExpanded( false );
    pEquilBranch->setText( 1, str_not_loaded );
    pEquilBranch->setDisabled( true );
    pEquilBranch->setExpanded( false );
    pMeshBranch->setText( 1, str_not_loaded );
    pMeshBranch->setDisabled( true );
    pMeshBranch->setExpanded( false );

    Clear( pModelBranch );
    Clear( pTopologyBranch );
    Clear( pEquilBranch );
    Clear( pMeshBranch );

    return;
  }

  pModel = _pModel;

  pModelBranch->setText( 1, "" );
  pModelBranch->setDisabled( false );
  pModelBranch->setExpanded( true );

  AddBranchForModel( pModelBranch, pModel, 1 );

  if( !_pModel->GetFluxModel()->HasTopology() ) {
    pTopologyBranch->setText( 1, str_not_loaded );
    pTopologyBranch->setDisabled( true );
    pTopologyBranch->setExpanded( false );
    Clear( pTopologyBranch );
  }
  else {
    pTopologyBranch->setText( 1, "" );
    pTopologyBranch->setDisabled( false );
    pTopologyBranch->setExpanded( true );
    AddBranchForTopology( pTopologyBranch, pModel->GetFluxModel()->GetTopology(), 1 );
  }

  if( !pModel->GetFluxModel()->HasEquil() ) {
    pEquilBranch->setText( 1, str_not_loaded );
    pEquilBranch->setDisabled( true );
    pEquilBranch->setExpanded( false );
    Clear( pEquilBranch );
  }
  else {
    pEquilBranch->setText( 1, "" );
    pEquilBranch->setDisabled( false );
    pEquilBranch->setExpanded( true );
    AddBranchForEquil( pEquilBranch, pModel->GetFluxModel()->GetEquil(), 1 );
  }

  if( !_pModel->HasMesh() ) {
    pMeshBranch->setText( 1, str_not_loaded );
    pMeshBranch->setDisabled( true );
    pMeshBranch->setExpanded( false );
    Clear( pMeshBranch );
  }
  else {
    pMeshBranch->setText( 1, "" );
    pMeshBranch->setDisabled( false );
    pMeshBranch->setExpanded( true );
    AddBranchForMesh( pMeshBranch, pModel->GetMesh(), 1 );
  }
}
