#include "ActionStackInfo.h"

ActionStackInfo::ActionStackInfo( StringsManager* _pSM, QWidget* _pParent):
  QDockWidget( _pParent ),
  ModelTree( _pSM )
{
  setFeatures( QDockWidget::DockWidgetMovable );
  setMinimumWidth( 220 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QWidget* titleWidget = new QWidget( _pParent );
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

  pUndoBranch = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::ACTIONS::UNDOSTACK ) ) );
  pUndoBranch->setFont( 0, treeFont );
  pRedoBranch = new QTreeWidgetItem( QStringList( SM_GUI( GUI::MODEL_INFO::ACTIONS::REDOSTACK ) ) );
  pRedoBranch->setFont( 0, treeFont );

  pTree->addTopLevelItem( pUndoBranch );
  pTree->addTopLevelItem( pRedoBranch );

  UpdateContent( pModel );

  pTree->connect( pTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
           this, SLOT(UpdateItem(QTreeWidgetItem*)) );
}

void ActionStackInfo::UpdateContent( ModelPtr _pModel )
{
  UpdateContentUndo( _pModel );
  UpdateContentRedo( _pModel );
}

void ActionStackInfo::NullModel()
{
  pUndoBranch->setText( 1, "0" );
  pUndoBranch->setDisabled( true );
  pUndoBranch->setExpanded( false );
  pRedoBranch->setText( 1, "0" );
  pRedoBranch->setDisabled( true );
  pRedoBranch->setExpanded( false );

  Clear( pUndoBranch );
  Clear( pRedoBranch );
}

void ActionStackInfo::UpdateContentUndo( ModelPtr _pModel )
{
  pModel = _pModel;
  if( _pModel == null )
    return NullModel();

  pUndoBranch->setText( 1, "" );
  pUndoBranch->setDisabled( false );
  pUndoBranch->setExpanded( true );

  AddBranchForStack( pUndoBranch, pModel->ActionStack().UndoStack(), 1 );
}

void ActionStackInfo::UpdateContentRedo( ModelPtr _pModel )
{
  pModel = _pModel;
  if( _pModel == null )
    return NullModel();

  pRedoBranch->setText( 1, "" );
  pRedoBranch->setDisabled( false );
  pRedoBranch->setExpanded( true );

  AddBranchForStack( pRedoBranch, pModel->ActionStack().RedoStack(), 1 );
}
