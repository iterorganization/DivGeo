#include "MainWnd.h"

void CMainWnd::slotSelect( int id )
{
  Qt::MouseButton button = (Qt::MouseButton)(id & TOOLS::MASK_BUTTON);
  ToolId toolid = (ToolId)(id & TOOLS::MASK_TOOL);

  QMdiSubWindow* pSubWnd = pMdiArea->activeSubWindow();
  if( pSubWnd == null )
    return;

  CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
  if( pSubView == null )
    return;

  pSubView->SetToolState( button, toolid  );
  ToolActionPair tool = mapTools.value( id, ToolActionPair( null, null ) );
  if( tool.first == null || tool.second == null )
    return;

  QString sMsgText = SM_LOG( LOG::TOOLS::TOOL ).arg( SM_LOG( toolid ) ) + " " +
                     SM_LOG( LOG::TOOLS::SELECTED );

  switch( button ) {
  case Qt::LeftButton:
    pSubView->disconnect( SIGNAL(signalMouseLeftButton(QPoint, EEventID)) );
    connect( pSubView, SIGNAL(signalMouseLeftButton(QPoint, EEventID)), pSubView, tool.second );
    pButton_L->setText( tool.first->text() );
    sMsgText = sMsgText.arg( SM_LOG( LOG::TOOLS::LEFT_BUTTON ) );
    break;
  case Qt::MiddleButton:
    pSubView->disconnect( SIGNAL(signalMouseMiddleButton(QPoint, EEventID)) );
    connect( pSubView, SIGNAL(signalMouseMiddleButton(QPoint, EEventID)), pSubView, tool.second );
    pButton_M->setText( tool.first->text() );
    sMsgText = sMsgText.arg( SM_LOG( LOG::TOOLS::MIDDLE_BUTTON ) );
    break;
  case Qt::RightButton:
    pSubView->disconnect( SIGNAL(signalMouseRightButton(QPoint, EEventID)) );
    connect( pSubView, SIGNAL(signalMouseRightButton(QPoint, EEventID)), pSubView, tool.second );
    pButton_R->setText( tool.first->text() );
    sMsgText = sMsgText.arg( SM_LOG( LOG::TOOLS::RIGHT_BUTTON ) );
    break;
  default:
    break;
  }

  pConsole->Send( LOG_INFO, sender_name, sMsgText );
  pToolBar_Tools->slotToolSelected( id );
}


QAction* CMainWnd::CreateAction(
  QMenu*pMenu,
  Qt::MouseButton button,
  ToolId toolId,
  const char* slotMethod,
  bool checkable,
  const QKeySequence &shortcuts )
{
  // configure action
  QStringList strings = SM_TOOL( toolId );
  QAction*pAction = pMenu->addAction( strings[0] );
  QList<QKeySequence> keyList;
  keyList += shortcuts;
  if( !shortcuts.isEmpty() )
    pAction->setShortcuts( keyList );
  if( strings.size() > 1 )
    pAction->setStatusTip( strings[1] );
  pAction->setCheckable( checkable );
  pAction->setEnabled( true );

  // connect to mapper
  int id = toolId | button;
  pSmToolSelect->setMapping( pAction, id );
  connect(pAction, SIGNAL(triggered()), pSmToolSelect, SLOT(map()) );
  mapTools.insert( id, qMakePair( pAction, slotMethod ) );
  return pAction;
}


void CMainWnd::CreateOldToolBar()
{
  pSmToolSelect = new QSignalMapper( this );
  connect( pSmToolSelect, SIGNAL(mapped(int)), this, SLOT(slotSelect(int)) );

  QMenu* Menus[3];
  for(unsigned c = 0; c < 3; c++)
  {
    Menus[c] = new QMenu();
    QMenu* pMenu = Menus[c];
    Qt::MouseButton button;
    switch( c ) {
    case 0: button = Qt::LeftButton; break;
    case 1: button = Qt::MiddleButton; break;
    case 2: button = Qt::RightButton; break;
    default: continue; }

    CreateAction(pMenu, button, TOOLS::ZOOMPAN,        SLOT(slotZoomPan       (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::MARK,           SLOT(slotMark          (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::EXAMINE,        SLOT(slotExamine       (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::ROTATE,         SLOT(slotRotate        (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::STRETCH,        SLOT(slotStretch       (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::MOVE,           SLOT(slotMove          (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::DELETE,         SLOT(slotDelete        (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::ADDELEMENT,     SLOT(slotAddElement    (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::ADDSOURCES,     SLOT(slotAddSources    (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::ADDCHORDS,      SLOT(slotAddChords     (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::ADDSURFACE,     SLOT(slotAddSurface    (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::ADDGRIDPOINT,   SLOT(slotAddGridPoint  (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::SPLITELEMENT,   SLOT(slotSplitElement  (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::JOINELEMENTS,   SLOT(slotJoinElements  (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::CONNECTPOINTS,  SLOT(slotConnectPoints (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::REPOSITION,     SLOT(slotReposition    (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::REVERSENORMALS, SLOT(slotReverseNormals(QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::MOVEMESHPOINT,  SLOT(slotMoveMeshPoint (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::ADJUSTCHORD,    SLOT(slotAdjustChord   (QPoint, EEventID)) );
    CreateAction(pMenu, button, TOOLS::EXTENDCHORD,    SLOT(slotExtendChord   (QPoint, EEventID)) );
    pMenu->addSeparator();
    CreateAction(pMenu, button, TOOLS::COMMENT,        SLOT(slotAddComment    (QPoint, EEventID)) );
  }

  pToolBar_old = new QDockWidget();
  QWidget* pTitleBar = pToolBar_old->titleBarWidget();  // Remove title widget
  QWidget* pEmptyWidget = new QWidget();            //
  pToolBar_old->setTitleBarWidget( pEmptyWidget );      //
  delete pTitleBar;                                 //
  //pToolBar->setFeatures( QDockWidget::NoDockWidgetFeatures );
  pToolBar_old->setFixedHeight(32);
  pToolBar_old->setMinimumWidth( 400 );
  pToolBar_old->setAllowedAreas( Qt::TopDockWidgetArea );
  this->addDockWidget( Qt::TopDockWidgetArea, pToolBar_old );

  QWidget *dockWidgetContents = new QWidget();
  dockWidgetContents ->setGeometry( QRect( 0, 0, 400, 30 ) );
  QHBoxLayout *pLoTools = new QHBoxLayout( dockWidgetContents );

  pButton_L = CreatePopupToolButton( pLoTools, "L:", Menus[0] );
  pButton_M = CreatePopupToolButton( pLoTools, "M:", Menus[1] );
  pButton_R = CreatePopupToolButton( pLoTools, "R:", Menus[2] );

  /* defaults */
  slotSelect( TOOLS::DEFAULT_LEFT_TOOL  | Qt::LeftButton );
  slotSelect( TOOLS::DEFAULT_MID_TOOL   | Qt::MiddleButton );
  slotSelect( TOOLS::DEFAULT_RIGHT_TOOL | Qt::RightButton );

  QSpacerItem *pSpacer = new QSpacerItem( 400, 30, QSizePolicy::Expanding, QSizePolicy::Minimum );
  pLoTools->addItem( pSpacer );

  pToolBar_old->setWidget( dockWidgetContents );
  pToolBar_old->setVisible( false );
}

QPushButton* CMainWnd::CreatePopupToolButton( QLayout* _pLo, const QString& _crsLabel,
                                    QMenu* _pMenu )
{
  QLabel* pLabel = new QLabel( _crsLabel );
  pLabel->setFont(QFont("Courier",12));
  pLabel->setFixedHeight(24);
  _pLo->addWidget( pLabel );

  QPushButton* pBtn = new QPushButton();
  pBtn->setFixedSize( 150, 24 );
  pBtn->setMenu( _pMenu );
  pBtn->setEnabled( false );
  _pLo->addWidget( pBtn );
  return pBtn;
}

QToolButton* CMainWnd::CreateModeButton( QLayout* _pLo, const QString& _crsTitle,
                               const QString& _crsToolTip, bool _isChecked )
{
  QToolButton* pBtn = new QToolButton();
  pBtn->setCheckable( true );
  pBtn->setChecked( _isChecked );
  pBtn->setFixedSize( 24, 24 );
  pBtn->setText( _crsTitle );
  pBtn->setToolTip( _crsToolTip );
  _pLo->addWidget( pBtn );
  return pBtn;
}

void CMainWnd::CreateToolBars()
{
  pToolBar_Edit = new QToolBar( "Edit", this );
  QList< QAction* > lstActions;
  lstActions.append( mapMenuActions[ MENU::EDIT::UNDO ] );
  lstActions.append( mapMenuActions[ MENU::EDIT::REDO ] );

  pToolBar_Edit->addActions( lstActions );
  pToolBar_Edit->setVisible( false );
  this->addToolBar( Qt::TopToolBarArea, pToolBar_Edit );

  foreach( QObject* pObj, pToolBar_Edit->children() ) {
    QToolButton* pBtn = qobject_cast< QToolButton* >( pObj );
    if( pBtn == null )
      continue;
    pBtn->setFixedSize( 27, 27 );
    pBtn->setIconSize( QSize( 24, 24 ) );
  }

  pToolBar_View = new QToolBar( "View", this );
  lstActions.clear();
  lstActions.append( mapMenuActions[ MENU::VIEW::PICTURE_VIEW ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::SELECTION_VIEW ] );
  lstActions.append( CreateSeparator() );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::NODES ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::IRRNODES ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::ELEMENTS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::SOURCES ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::CHORDS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::CHORDS3D ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::SEPARATORS ] );
  lstActions.append( CreateSeparator() );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::NORMALS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::NUMBERS ] );
  lstActions.append( CreateSeparator() );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::SURFACES ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::GRIDPOINTS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::XPOINTS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::MESH ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::MESHDETAILS ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::EQUILIBRIUM ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::TEMPLATE ] );
  lstActions.append( CreateSeparator() );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::GRID ] );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::AXES ] );
  lstActions.append( CreateSeparator() );
  lstActions.append( mapMenuActions[ MENU::VIEW::DISPLAY::COMMENTS ] );

  pToolBar_View->addActions( lstActions );
  pToolBar_View->setVisible( false );
  this->addToolBar( Qt::TopToolBarArea, pToolBar_View );

  foreach( QObject* pObj, pToolBar_View->children() ) {
    QToolButton* pBtn = qobject_cast< QToolButton* >( pObj );
    if( pBtn == null )
      continue;
    pBtn->setFixedSize( 27, 27 );
    pBtn->setIconSize( QSize( 24, 24 ) );
  }
}

QAction* CMainWnd::CreateSeparator()
{
  QAction* pAct = new QAction( this );
  pAct->setSeparator( true );
  return pAct;
}
