#include "Assistant.h"

Assistant::Assistant( ResourcesManager* _pRM, QWidget *parent) :
  QDialog(parent),
  pRM( _pRM )
{
  setWindowTitle( parent->windowTitle() + " - Assistant" );
  resize( 800, 600 );

  QPixmap icons( ":/main/buttons.png" );

  QVBoxLayout* pLoMain = new QVBoxLayout();
  pLoMain->setMargin( 0 );
  pLoMain->setSpacing( 0 );
  setLayout( pLoMain );

  QToolBar* pBar = new QToolBar( this );
  pBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
  pBar->setIconSize( QSize( ico_size, ico_size ) );
  pLoMain->addWidget( pBar );

  QSplitter* pSplitter = new QSplitter( Qt::Horizontal );
  pLoMain->addWidget( pSplitter );

  pTree = new QTreeWidget();
  pTree->header()->hide();
  pSplitter->addWidget( pTree );

  pBrowser = new QTextBrowser();
  pSplitter->addWidget( pBrowser );

  pSplitter->setStretchFactor( 0, 1 );
  pSplitter->setStretchFactor( 1, 4 );

  pBar->addAction( MakeIcon( icons, 0, 4 ), "Previous page", this, SLOT(slotPrevPage()) );
  pBar->addAction( MakeIcon( icons, 1, 4 ), "Next page", this, SLOT(slotNextPage()) );
  pBar->addSeparator();
  pBar->addAction( MakeIcon( icons, 4, 3 ), "Options", this, SLOT(slotOptions()) );
  pBar->actions()[2]->setDisabled( true );

  RebuildTree();

  connect( pTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
           this,  SLOT(slotItemSelected(QTreeWidgetItem*,int)) );

  pItem_current = pRM->Value( "main.help_system" )->pItem;
  pTree->setCurrentItem( pItem_current );
  slotItemSelected( pItem_current, 0 );
}

void Assistant::closeEvent( QCloseEvent *pEv ) {
  pEv->accept();
}

void Assistant::slotShowAt( ulong _code, const QString& _crsDict )
{
  DataValue* pValue = pRM->Value(_code, _crsDict );
  if( pValue == null )
    return;
  pItem_current = pValue->pItem;
  pTree->setCurrentItem( pItem_current );
  slotItemSelected( pItem_current, 0 );
  show();
  raise();
}

void Assistant::RebuildTree()
{
  pTree->clear();
  DataValue* pValue = pRM->Value( "main.title" );
  QTreeWidgetItem* pItem = new QTreeWidgetItem(
                             pValue != null && !pValue->strings.isEmpty() ?
                             QStringList( pValue->strings.first() ) :
                             QStringList() );
  pTree->addTopLevelItem( pItem );

  if( pValue != null )
    pValue->pItem = pItem;

  const TreeNode::NodesList& crBranches = pRM->TopLevelNodes();
  BuildBranch( pItem, pRM->Node( "main" ) );
  pTree->expandItem( pItem );

  foreach( const TreeNode& crNode, crBranches ) {
    if( crNode.Key() == "main" )
      continue;

    if( pValue != null )
        pValue = crNode.Value( "title" );

    QTreeWidgetItem* pBranch = new QTreeWidgetItem(
                                 pValue != null && !pValue->strings.isEmpty() ?
                                 QStringList( pValue->strings.first() ) :
                                 QStringList() );
    pItem->addChild( pBranch );

    if( pValue != null )
        pValue->pItem = pBranch;

    BuildBranch( pBranch, &crNode );
    pTree->expandItem( pBranch );
  }
}

void Assistant::BuildBranch( QTreeWidgetItem* _pParent, const TreeNode* _pNode )
{
  foreach( const DataValue& crValue, _pNode->Values() ) {
    if( crValue.sKey == "title" )
      continue;
    QTreeWidgetItem* pItem = new QTreeWidgetItem( !crValue.strings.isEmpty() ?
                               QStringList( crValue.strings.first() ) :
                               QStringList() );
    pItem->setData( 0, Qt::UserRole, QVariant::fromValue( &crValue ) );
    _pParent->addChild( pItem );
    crValue.pItem = pItem;
  }

  foreach( const TreeNode& crNode, _pNode->Nodes() ) {
    DataValue* pValue = crNode.Value( "title" );
    QTreeWidgetItem* pBranch = new QTreeWidgetItem(
                                 pValue != null && !pValue->strings.isEmpty() ?
                                 QStringList( pValue->strings.first() ) :
                                 QStringList() );
    _pParent->addChild( pBranch );
    if( pValue != null )
        pValue->pItem = pBranch;

    BuildBranch( pBranch, &crNode );
  }
}

void Assistant::slotItemSelected( QTreeWidgetItem* _pItem, int _column )
{
  Q_UNUSED( _column );
  if( _pItem != pItem_current ) {
    stack_next.clear();
    stack_prev.push_back( pItem_current );
    pItem_current = _pItem;
  }

  OpenPage( _pItem );
}

void Assistant::OpenPage( QTreeWidgetItem* _pItem )
{
  QVariant data = _pItem->data( 0, Qt::UserRole );
  if( data.isNull() || data.userType() != QMetaType::type( "DataValueCPtr" ) ) {
    pBrowser->setText( "" );
    return;
  }
  DataValueCPtr cpValue = data.value< DataValueCPtr >();
  if( cpValue == null )
    return;
  if( cpValue->strings.size() < 2 )
    pBrowser->setText( HelpManager::scsNoHelp );
  else
    SetText( cpValue->strings );
}

void Assistant::SetText( const QStringList& _crText )
{
  QTextDocument* pDoc = pBrowser->document();
  pDoc->clear();

  QTextCursor cursor( pDoc );

  QTextBlockFormat block_format;
  block_format.setBottomMargin( 10. );
  block_format.setLeftMargin( 10. );
  block_format.setRightMargin( 10. );
  block_format.setTextIndent( 20. );
  cursor.insertBlock( block_format );

  QTextCharFormat char_format;
  char_format.setFont( QFont( "Arial", 14, QFont::Bold ) );
  cursor.insertText( _crText[0] + '\n', char_format );

  char_format.setFont( QFont( "Arial", 11 ) );
  cursor.insertText( _crText[1], char_format );

  cursor.movePosition( QTextCursor::Start );
  pBrowser->setTextCursor( cursor );
  pBrowser->ensureCursorVisible();
  //pBrowser->verticalScrollBar()->setValue( pBrowser->verticalScrollBar()->minimum() );
}

void Assistant::slotPrevPage()
{
  if( stack_prev.isEmpty() )
    return;
  stack_next.push_front( pItem_current );
  pItem_current = stack_prev.last();
  stack_prev.pop_back();

  pTree->setCurrentItem( pItem_current );
  OpenPage( pItem_current );
}

void Assistant::slotNextPage()
{
  if( stack_next.isEmpty() )
    return;
  stack_prev.push_back( pItem_current );
  pItem_current = stack_next.first();
  stack_next.pop_front();

  pTree->setCurrentItem( pItem_current );
  OpenPage( pItem_current );
}

void Assistant::slotOptions()
{
}
