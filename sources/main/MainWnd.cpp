#include "MainWnd.h"
#include "../view/EditorWnd.h"

CMainWnd::CMainWnd( const QStringList& _crArgs, Console* _pConsoleWindow,
                   StringsManager* _pSM, HelpManager* _pHM,
                   QWidget *parent, Qt::WindowFlags flags):
  QMainWindow(parent, flags),
  sender_name( "DG" ),
  pngButtons( ":/main/buttons.png" ),
  pConsole( _pConsoleWindow ),
  pSM( _pSM ), pHelp( _pHM ),
  sProfileDir( QDir::homePath() + "/.dg3/" ),
  userPrefs( sProfileDir, "settings" ),
  useConfig( true )
  //TODO: initialize by command line arguments
{

#ifdef Q_OS_LINUX
  QString sPlatform = "Linux";
#else //Q_OS_WIN
  QString sPlatform = "Windows";
#endif

  /* Setup MainWindow */
  version = QString( "DivGeo %1 for %2 - build %3 (beta)" )
            .arg( DG_VERSION_STR_SHORT )
            .arg( sPlatform )
            .arg( DG_VERSION_STR_BUILD );
  this->setWindowTitle( version );

  this->setWindowIcon( QIcon( ":/main/icon.png" ) );
  ErrorCode = 0;

  /* Setup MenuBar */
  CreateMenuBar();
  LoadRecentList();

  /* Setup StatusBar */
  pStatusBar = new QStatusBar();
  this->setStatusBar(pStatusBar);
  pStatusBar->setFont( QFont( "Courier", 9 ) );

  /* Setup tools panel */
  pToolBar_Tools = new ToolsPanel( pSM, this );
  pToolBar_Tools->setVisible( false );
  this->addDockWidget( Qt::LeftDockWidgetArea, pToolBar_Tools );
  connect( pToolBar_Tools, SIGNAL(toolSelected(int)), this, SLOT(slotSelect(int)) );

  /* Start setup MDI */
  pMdiArea = new QMdiArea;
  pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setCentralWidget( pMdiArea );

  /* Setup Console */
  pConsole->setup( pSM, this );
  pConsole->setVisible( false );
  this->addDockWidget( Qt::BottomDockWidgetArea, pConsole );
  pConsole->UpdateCursor();

  /* Setup ModelInfoTree */
  pModelInfo = new ModelInfo( pSM, this );
  pModelInfo->setVisible( false );
  this->addDockWidget( Qt::RightDockWidgetArea, pModelInfo );

  pActInfo = new ActionStackInfo( pSM, this );
  pActInfo->setVisible( false );
  this->addDockWidget( Qt::RightDockWidgetArea, pActInfo );

  /* Setup ModelViewManager */
  pMV = new ModelViewManager( this, &userPrefs, pConsole, pSM, this );
  pMV->setVisible( false );
  this->addDockWidget( Qt::RightDockWidgetArea, pMV );
  connect( pMV, SIGNAL(ViewSelected(CViewWndPtr)),
           this, SLOT(slotActiveViewChanged(CViewWndPtr)) );
  connect( pMV, SIGNAL(EditorSelected(EditorWnd*)),
           this, SLOT(slotActiveEditorChanged(EditorWnd*)) );
  connect( pMV, SIGNAL(ViewCreated(CViewWndPtr)),
           this, SLOT(slotNewView(CViewWndPtr)) );

  /* Setup ToolBar */
  CreateOldToolBar(); // mdi-area is necessary to setup subwindows' tools

  /* Setup toolbars */
  CreateToolBars(); // Menu is necessary

  /* Finish setup MDI */
  this->setCentralWidget(pMdiArea);
  connect( pMdiArea,  SIGNAL(subWindowActivated(QMdiSubWindow*)),
           this,      SLOT(slotActiveSubWndChanged(QMdiSubWindow*)) );

  /* Setup Vars menu connections for dynamic update */
  connect( &smVarsAdd,    SIGNAL(mapped(QString)),
           this,          SLOT(slotAddLayer(QString)) );
  connect( &smVarsRemove, SIGNAL(mapped(QString)),
           this,          SLOT(slotRemoveLayer(QString)) );
  connect( &smVarsEdit,   SIGNAL(mapped(QString)),
           this,          SLOT(slotDisplayLayer(QString)) );

  /* Refresh menu state */
  slotActiveViewChanged( null );

  /* Setup Help system */
  pAssistant = new Assistant( pHelp, this );
  pDlgVarHelp = new DlgVarHelp( "", "", this );

  /* Ready! */
  pConsole->Send( LOG_INFO, sender_name, "Ready" );

  if( _crArgs.length() > 1 )
      pMV->SkipSessionBut( _crArgs );

  QTimer::singleShot( 100, this, SLOT(slotAtStart()) );
}

void CMainWnd::closeEvent( QCloseEvent* ev )
{
  quint32 panels = 0;
  if( pToolBar_old  ->isVisible() ) panels |= SessionData::PF::OLDTOOLS;
  if( pModelInfo    ->isVisible() ) panels |= SessionData::PF::MODELINFO;
  if( pMV           ->isVisible() ) panels |= SessionData::PF::VIEWINFO;
  if( pConsole      ->isVisible() ) panels |= SessionData::PF::CONSOLE;
  if( pToolBar_Tools->isVisible() ) panels |= SessionData::PF::TOOLS;
  if( pToolBar_View ->isVisible() ) panels |= SessionData::PF::VIEWBAR;
  if( pToolBar_Edit ->isVisible() ) panels |= SessionData::PF::EDITBAR;
  pMV->GetSessionData()->panels = panels;
  pMV->Delete();
  QMainWindow::closeEvent( ev );
  //QApplication::exit();
}

void CMainWnd::slotAtStart()
{
  pMV->LoadSessionData();
  quint32 panels = pMV->GetSessionData()->panels;
  bool flag;
  flag = HasAnyFlag( panels, SessionData::PF::OLDTOOLS );
  pToolBar_old  ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::OLDTOOLS ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::MODELINFO );
  pModelInfo    ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::MODEL_INFO ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::VIEWINFO );
  pMV           ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::VIEWS ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::CONSOLE );
  pConsole      ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::CONSOLE ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::TOOLS );
  pToolBar_Tools->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::TOOLS ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::VIEWBAR );
  pToolBar_View ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::TB_VIEW ]->setChecked( flag );
  flag = HasAnyFlag( panels, SessionData::PF::EDITBAR );
  pToolBar_Edit ->setVisible( flag );
  mapMenuActions[ MENU::WINDOW::PANELS::TB_EDIT ]->setChecked( flag );

  pMV->UpdateAllViewsGeometry();

  UpdateModelInfo( true );
}

void CMainWnd::slotDialogHelp( int _code )
{
  pAssistant->slotShowAt( (ulong)_code, "dlg" );
}

void CMainWnd::slotVarHelp( const QString& _crsTitle, const QString& _crsText )
{
  pDlgVarHelp->slotShowWith( _crsTitle, _crsText );
}

bool CMainWnd::HasActiveView( CViewWnd* pV )
{
  QMdiSubWindow* pSubWnd = pMdiArea->activeSubWindow();
  if( pSubWnd == null )
    return false;

  CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
  if( pSubView != pV )
    return false;

  return true;
}

void CMainWnd::slotViewClosed( CViewWnd* _pView )
{
  ModelViewProxyPtr pProxy = pMV->GetProxy( _pView );
  pProxy->RemoveView( _pView );
  if( pMdiArea->subWindowList().size() == 0 )
    slotActiveViewChanged( null );
}

void CMainWnd::slotNewView( CViewWndPtr _pView )
{
  /* Management */
  connect( _pView,  SIGNAL(ChangeShowFlag(ShowFlag,bool)),
           this,    SLOT(slotChangeShowFlag(ShowFlag,bool)) );
  connect( _pView,  SIGNAL(ViewClosed(CViewWnd*)),
           this,    SLOT(slotViewClosed(CViewWnd*)) );
  connect( _pView,  SIGNAL(ViewInFocus(CViewWnd*)),
           this,    SLOT(slotActiveViewChangedP(CViewWnd*)) );

  /* Update info */
  connect( _pView,  SIGNAL(UpdateModelInfo()),
           this,    SLOT(UpdateModelInfo()) );
  connect( _pView,  SIGNAL(UpdateObjectInfo(IComponentPtr)),
           this,    SLOT(UpdateObjectInfo(IComponentPtr)) );
  connect( _pView,  SIGNAL(UndoStackEnabled(bool)),
           this,    SLOT(slotEnableUndoStack(bool)) );

  QMdiSubWindow* pSubWnd = pMdiArea->addSubWindow( (QWidget*)_pView );
  UpdateMenu( _pView ); //1411

  pSubWnd->activateWindow();
  pSubWnd->setWindowState( Qt::WindowMaximized );
  pSubWnd->show();

  /* default show flags */
  slotChangeShowFlag( SHW::GRID, true );
  slotChangeShowFlag( SHW::AXES, true );

  _pView->FitToModel();
}

void CMainWnd::slotNewEditor( EditorWnd* _pEditor ) {
  connect( _pEditor,  SIGNAL(EditorInFocus(EditorWnd*)),
           this,      SLOT(slotActiveEditorChanged(EditorWnd*)) );

  QMdiSubWindow* pSubWnd = pMdiArea->addSubWindow( (QWidget*)_pEditor );
  pSubWnd->activateWindow();
  pSubWnd->setWindowState( Qt::WindowMaximized );
  pSubWnd->show();
}

CMainWnd::~CMainWnd()
{

}


int CMainWnd::CreateMenuBar()
{
  using namespace Qt;

  QMenu *pMenu,*pSubMenu,*pSubSubMenu;

  pMenuBar = new QMenuBar(this);
  pSmViewOptions = new QSignalMapper( pMenuBar );
  connect( pSmViewOptions, SIGNAL(mapped(int)), this, SLOT(slotDisplay(int)) );

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::FILES::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::FILES::NEW,                        SLOT(slotNew()) );
  CreateAction( pMenu,     MENU::FILES::OPEN,                       SLOT(slotOpen()) );
  pMenu->addMenu( CreateMenu( MENU::FILES::RECENT::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::FILES::SAVE,                       SLOT(slotSave()) );
  CreateAction( pMenu,     MENU::FILES::SAVEAS,                     SLOT(slotSaveAs()) );
  CreateAction( pMenu,     MENU::FILES::SAVECOPY,                   SLOT(slotSaveCopy()) );
  pMenu->addSeparator();
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::FILES::EXPORT::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::FILES::EXPORT::MESH,               SLOT(slotExportMesh()) );/*2.1*/
  CreateAction( pSubMenu,  MENU::FILES::EXPORT::ELEMENT,            SLOT(slotExportElements()) );/*2.1*/
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::FILES::IMPORT::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::FILES::IMPORT::TEMPLATE,           SLOT(slotImportTemplate()) );
  CreateAction( pSubMenu,  MENU::FILES::IMPORT::EQUILIBRIUM,        SLOT(slotImportEquilibrium()) );
  CreateAction( pSubMenu,  MENU::FILES::IMPORT::MESH,               SLOT(slotImportMesh()) );/*2.1*/
  CreateAction( pSubMenu,  MENU::FILES::IMPORT::TOPOLOGY,           SLOT(slotImportTopology()) );/*2.1*/
  //CreateAction( pSubMenu,  MENU::FILES::IMPORT::SONNETGRID,         SLOT(slotImportSonnetGrid()) );/*2.0 the same as ImportMesh?*/
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::FILES::EDITOR,                     SLOT(slotEditor()) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::FILES::OUTPUT,                     SLOT(slotOutput()) )->setEnabled( false );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::FILES::PRINT,                      SLOT(slotPrint()) )->setEnabled( false );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::FILES::EXIT,                       SLOT(slotExit()),                      false, -1, -1, QKeySequence(CTRL+Key_Q) );
  
  pMenu = pMenuBar->addMenu( CreateMenu( MENU::EDIT::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::EDIT::UNDO,                        SLOT(slotUndo()),                      false, 0, 4, QKeySequence(CTRL+Key_Z) );
  CreateAction( pMenu,     MENU::EDIT::REDO,                        SLOT(slotRedo()),                      false, 1, 4, QKeySequence(CTRL+SHIFT+Key_Z) );
  CreateAction( pMenu,     MENU::EDIT::REDO_ALL,                    SLOT(slotRedoAll()) );
  CreateAction( pMenu,     MENU::EDIT::CLEAR_ASTACK,                    SLOT(slotClearActionStack()) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::EDIT::MARK_ALL_ELEMENTS,           SLOT(slotMarkAllElements()),           false, -1, -1, QKeySequence(CTRL+Key_A) );
  CreateAction( pMenu,     MENU::EDIT::MARK_ALL_SEPARATORS,         SLOT(slotMarkAllSeparators()) );/*2.0*/
  CreateAction( pMenu,     MENU::EDIT::MARK_ALL_CHORDS,             SLOT(slotMarkAllChords()) );/*2.1*/
  CreateAction( pMenu,     MENU::EDIT::UNMARK_ALL,                  SLOT(slotUnmarkAll()),                 false, -1, -1, QKeySequence(CTRL+Key_U) );
  pMenu->addSeparator();
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::EDIT::CREATE::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::EDIT::CREATE::POINT,               SLOT(slotCreatePoint()) );
  CreateAction( pSubMenu,  MENU::EDIT::CREATE::SOURCE,              SLOT(slotCreateSource()) );
  CreateAction( pSubMenu,  MENU::EDIT::CREATE::CHORD,               SLOT(slotCreateChord()) );
  CreateAction( pSubMenu,  MENU::EDIT::CREATE::SURFACE,             SLOT(slotCreateSurface()) );
  CreateAction( pSubMenu,  MENU::EDIT::CREATE::GRIDPOINTS,          SLOT(slotCreateGridPoints()) );
/*CreateAction( pSubMenu,  MENU::EDIT::CREATE::SEPARATORS,          SLOT(slotCreateSeparators()) );*//*2.0*/
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::EDIT::DELETE::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::MARKED_OBJECTS,      SLOT(slotDeleteMarkedObjects()),       false, -1, -1, QKeySequence(CTRL+Key_D) );/*2.1*/
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::UNUSED_POINTS,       SLOT(slotDeleteUnusedPoints()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::VIRTUAL_SURFACES,    SLOT(slotDeleteVirtualSurfaces()) );/*2.1*/
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::ELEMENTS,            SLOT(slotDeleteElements()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::SOURCES,             SLOT(slotDeleteSources()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::CHORDS,              SLOT(slotDeleteChords()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::SURFACES,            SLOT(slotDeleteSurfaces()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::GRIDPOINTS,          SLOT(slotDeleteGridPoints()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::SEPARATORS,          SLOT(slotDeleteSeparators()) );
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::EQUILIBRIUM,         SLOT(slotDeleteEquilibrium()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::TEMPLATE,            SLOT(slotDeleteTemplate()) );
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::MESH,                SLOT(slotDeleteMesh()) );/*2.1*/
/*CreateAction( pSubMenu,  MENU::EDIT::DELETE::XPOINT,              SLOT(slotDeleteXPoint()) );*/ /*2.0*/
  CreateAction( pSubMenu,  MENU::EDIT::DELETE::SONNETGRID,          SLOT(slotDeleteSonnetGrid()) );/*2.1*/
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::EDIT::EXTEND_CHORDS,               SLOT(slotExtendChords()) );/*2.1*/
  CreateAction( pMenu,     MENU::EDIT::MOVE_ROTATE,                 SLOT(slotMoveRotate()) );

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::VIEW::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::VIEW::REFRESH,                     SLOT(slotRefresh()),                    false );
  CreateAction( pMenu,     MENU::VIEW::ZOOM_IN,                     SLOT(slotZoomIn()),                     false, -1, -1, QKeySequence(CTRL+Key_I) );
  CreateAction( pMenu,     MENU::VIEW::ZOOM_OUT,                    SLOT(slotZoomOut()),                    false, -1, -1, QKeySequence(CTRL+Key_O) );
  CreateAction( pMenu,     MENU::VIEW::PICTURE_VIEW,                SLOT(slotPictureView()),                false, 3, 4, QKeySequence(CTRL+Key_P) );
  CreateAction( pMenu,     MENU::VIEW::SELECTION_VIEW,              SLOT(slotSelectionView()),              false, 2, 4, QKeySequence(CTRL+Key_S) );
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::VIEW::ROTATE::TITLE ) )->menu();/*2.1*/
  CreateAction( pSubMenu,  MENU::VIEW::ROTATE::INCREASE_ANGLE,      SLOT(slotRotateIncreaseAngle()),        false, -1, -1, QKeySequence(CTRL+Key_BracketLeft) );
  CreateAction( pSubMenu,  MENU::VIEW::ROTATE::DECREASE_ANGLE,      SLOT(slotRotateDecreaseAngle()),        false, -1, -1, QKeySequence(CTRL+Key_BracketRight) );
  CreateAction( pSubMenu,  MENU::VIEW::ROTATE::SET_ANGLE,           SLOT(slotRotateSetAngle()) );
  CreateAction( pSubMenu,  MENU::VIEW::ROTATE::RECTIFY,             SLOT(slotRotateRectify()),              false, -1, -1, QKeySequence(CTRL+Key_C) );
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::VIEW::STR_SHR::TITLE ) )->menu();/*2.1*/
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::STRETCH_HORIZONTAL, SLOT(slotStretchHorizontal()),          false, -1, -1, QKeySequence(CTRL+SHIFT+Key_H) );
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::STRETCH_VERTICAL,   SLOT(slotStretchVertical()),            false, -1, -1, QKeySequence(CTRL+SHIFT+Key_V) );
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::SHRINK_HORIZONTAL,  SLOT(slotShrinkHorizontal()),           false, -1, -1, QKeySequence(CTRL+Key_V) );
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::SHRINK_VERTICAL,    SLOT(slotShrinkVertical()),             false, -1, -1, QKeySequence(CTRL+Key_V) );
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::STRETCH,            SLOT(slotStretch()) );
  CreateAction( pSubMenu,  MENU::VIEW::STR_SHR::JUSTIFY,            SLOT(slotJustify()),                    false, -1, -1, QKeySequence(CTRL+Key_J) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::VIEW::PREVIOUS_VIEW,               SLOT(slotPreviousView()),               false, -1, -1, QKeySequence(CTRL+Key_R) );/*2.1*/
  CreateAction( pMenu,     MENU::VIEW::REMOVE_LABELS,               SLOT(slotRemoveLabels()) )->setEnabled( false );/*2.1*/
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::VIEW::MODE::TITLE ) )->menu();/*2.1*/
  CreateAction( pSubMenu,  MENU::VIEW::MODE::STRETCH,               SLOT(slotModeStretch(bool)),              true );
  CreateAction( pSubMenu,  MENU::VIEW::MODE::TOPDOWN_VIEW,          SLOT(slotModeTopDownView(bool)),          true );
  CreateAction( pSubMenu,  MENU::VIEW::MODE::RADIAL_GRID,           SLOT(slotModeRadialGrid(bool)),           true );
  CreateAction( pSubMenu,  MENU::VIEW::MODE::COLORED_EQUIL,         SLOT(slotModeColoredEquil(bool)),         true );
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::VIEW::DISPLAY::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::NODES,       SHW::NODES,        0, 0 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::IRRNODES,    SHW::IRRNODES,     1, 0 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::ELEMENTS,    SHW::ELEMS,        2, 0 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::SOURCES,     SHW::SOURCES,      3, 0 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::CHORDS,      SHW::CHORDS,       4, 1 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::CHORDS3D,    SHW::CHORDS3D,     4, 0 );/*2.1*/
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::SEPARATORS,  SHW::SEPARATORS,   0, 1 );
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::NORMALS,     SHW::NORMALS,      1, 1 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::NUMBERS,     SHW::NUMBERS,      2, 1 );
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::SURFACES,    SHW::SURFACES,     3, 1 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::GRIDPOINTS,  SHW::GRIDPOINTS,   2, 3 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::XPOINTS,     SHW::XPOINTTESTS,  3, 3 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::MESH,        SHW::MESH ,        0, 2 );/*2.1*/
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::MESHDETAILS, SHW::MESHDETAILS,  1, 2 );/*2.1*/
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::EQUILIBRIUM, SHW::EQUIL,        2, 2 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::TEMPLATE,    SHW::TEMPLATE,     3, 2 );
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::GRID,        SHW::GRID,         0, 3 );
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::AXES,        SHW::AXES,         1, 3 );
  pSubMenu->addSeparator();
  CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::COMMENTS,    SHW::COMMENTS,     4, 2 );/*3.0*/
  //CreateAction( pSubMenu,  MENU::VIEW::DISPLAY::SONNETGRID,         0 /*SHW::MESH*/ );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::VIEW::MESH_HEADER, SLOT(slotMeshHeader()), false, -1, -1, QKeySequence(Key_X) );

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::COMMANDS::TITLE ) )->menu();
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::COMMANDS::CONVERT::TITLE ) )->menu();
  CreateAction( pSubMenu,     MENU::COMMANDS::CONVERT::TEMPLATE_TO_ELEMENTS,    SLOT(slotConvertTemplate2Elements()) );
  CreateAction( pSubMenu,     MENU::COMMANDS::CONVERT::ELEMENTS_TO_CHORDS,      SLOT(slotConvertElements2Chords()) );
  CreateAction( pSubMenu,     MENU::COMMANDS::CONVERT::CHORDS_TO_ELEMENTS,      SLOT(slotConvertChords2Elements()) );
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::COMMANDS::SIMPLIFY::TITLE ) )->menu();
  CreateAction( pSubMenu,     MENU::COMMANDS::SIMPLIFY::FILL_GAPS,              SLOT(slotSimplifyFillGaps()) );
  pSubSubMenu = pSubMenu->addMenu( CreateMenu( MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::TITLE ) )->menu();
  CreateAction( pSubSubMenu,  MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::ALL,    SLOT(slotSimplifyLineUpAllNormals()) );
  CreateAction( pSubSubMenu,  MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::MARKED, SLOT(slotSimplifyLineUpMarkedNormals()) );
  CreateAction( pSubMenu,     MENU::COMMANDS::SIMPLIFY::MERGE_SPLIT_ELEMENTS,   SLOT(slotSimplifyMergeSplitElements()) );
  CreateAction( pMenu,        MENU::COMMANDS::RENUMBER_ELEMENTS,                SLOT(slotRenumberElements()) );
  CreateAction( pMenu,        MENU::COMMANDS::CHECK_VARIABLES,                  SLOT(slotCheckVariables()) );
  CreateAction( pMenu,        MENU::COMMANDS::REBUILD_CARRE_OBJECTS,            SLOT(slotRebuildCarreObjects()) );
  pMenu->addSeparator();
  CreateAction( pMenu,        MENU::COMMANDS::FIND_XPOINTS,                     SLOT(slotFindXPoints()),     false, -1, -1, QKeySequence(CTRL+Key_X) );
  CreateAction( pMenu,        MENU::COMMANDS::FIND_XPOINTSEGS,                  SLOT(slotFindXPointSegs())/*,  false, -1, -1, QKeySequence(CTRL+Key_X)*/);
  CreateAction( pMenu,        MENU::COMMANDS::EDIT_TOPOLOGY,                    SLOT(slotEditTopology())/*,    false, -1, -1, QKeySequence(Key_X)*/);
  //CreateAction( pMenu,        MENU::COMMANDS::UPDATE_TOPOLOGY,                  SLOT(slotUpdateTopology())/*,  false, -1, -1, QKeySequence(Key_X)*/);

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::VARIABLES::TITLE ) )->menu();

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::OPTIONS::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::OPTIONS::PROJECT_SETTINGS,         SLOT(slotProjectSettings()) );
  //CreateAction( pMenu,     MENU::OPTIONS::OUTPUT_MODE,              SLOT(slotOutputMode()) );
  //CreateAction( pMenu,     MENU::OPTIONS::MESH_EDITING,             SLOT(slotMeshEditing()) );
  //pMenu->addSeparator();
  //CreateAction( pMenu,     MENU::OPTIONS::DISPLAY_MENU,             SLOT(slotDisplayMenu()),    true ); /*removed in 3.0*/
  //CreateAction( pMenu,     MENU::OPTIONS::DISPLAY_TOOLBAR,          SLOT(slotDisplayToolbar()), true ); /*removed in 3.0*/
  //CreateAction( pMenu,     MENU::OPTIONS::MANUAL_REFRESH,           SLOT(slotManualRefresh()),  true ); /*removed in 3.0*/
  //pMenu->addSeparator();
  //CreateAction( pMenu,     MENU::OPTIONS::SAVE_INTERVAL,            SLOT(slotSaveInterval()) )       ->setEnabled( false );
  CreateAction( pMenu,     MENU::OPTIONS::SETTINGS,                 SLOT(slotSettings()) );
  pMenu->addSeparator();
  pSubMenu = pMenu->addMenu( CreateMenu( MENU::OPTIONS::SETUP::TITLE ) )->menu();
  CreateAction( pSubMenu,  MENU::OPTIONS::SETUP::VARIABLES,         SLOT(slotSetupVariables()) );
  CreateAction( pSubMenu,  MENU::OPTIONS::SETUP::SAVE,              SLOT(slotSetupSave()) )->setEnabled( false );

  pMenu = pMenuBar->addMenu( CreateMenu( MENU::WINDOW::TITLE ) )->menu();
  //CreateAction( pMenu,     MENU::WINDOW::HIDE_MENU,                 SLOT(slotHideMenu()),  false, -1, -1, QKeySequence(Key_Backspace) )->setEnabled( false );
  CreateAction( pMenu,     MENU::WINDOW::NEW_VIEW,                  SLOT(slotNewView()) );
  CreateAction( pMenu,     MENU::WINDOW::CLOSE,                     SLOT(slotClose()) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::WINDOW::STATISTICS,                SLOT(slotStatistics()) );
  //CreateAction( pMenu,     MENU::WINDOW::TOOLBOX,                   SLOT(slotToolbox()) )       ->setEnabled( false );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::WINDOW::PANELS::ACT_INFO,    SLOT(slotShowActionInfoPanel(bool)),true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::MODEL_INFO,  SLOT(slotShowModelInfoPanel(bool)), true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::VIEWS,       SLOT(slotShowViewsPanel(bool)),     true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::CONSOLE,     SLOT(slotShowConsole(bool)),        true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::OLDTOOLS,    SLOT(slotShowOldToolBar(bool)),     true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::TOOLS,       SLOT(slotShowToolPanel(bool)),      true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::TB_VIEW,     SLOT(slotShowToolBar_View(bool)),   true );
  CreateAction( pMenu,     MENU::WINDOW::PANELS::TB_EDIT,     SLOT(slotShowToolBar_Edit(bool)),   true );


  pMenu = pMenuBar->addMenu( CreateMenu( MENU::HELP::TITLE ) )->menu();
  CreateAction( pMenu,     MENU::HELP::WHATS_NEW,                   SLOT(slotWhatsNew()) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::HELP::MANUAL,                      SLOT(slotManual()) );
  CreateAction( pMenu,     MENU::HELP::USING_HELP,                  SLOT(slotUsingHelp()) );
  CreateAction( pMenu,     MENU::HELP::ON_MAIN_WINDOW,              SLOT(slotOnMainWindow()) );
  pMenu->addSeparator();
  CreateAction( pMenu,     MENU::HELP::ABOUT,                       SLOT(slotAbout()) );

  this->setMenuBar( pMenuBar );

  mapViewOptions.insert( SHW::STRETCH, mapMenuActions.value( MENU::VIEW::MODE::STRETCH ) );
  mapViewOptions.insert( SHW::TOPVIEW, mapMenuActions.value( MENU::VIEW::MODE::TOPDOWN_VIEW ) );

  mapMenus.value( MENU::EDIT::TITLE )->setEnabled( false );
  mapMenus.value( MENU::VIEW::TITLE )->setEnabled( false );
  mapMenus.value( MENU::COMMANDS::TITLE )->setEnabled( false );
  mapMenus.value( MENU::VARIABLES::TITLE )->setEnabled( false );
  //mapMenus.value( MENU::OPTIONS::TITLE )->setEnabled( false );
  mapMenus.value( MENU::OPTIONS::SETUP::TITLE )->setEnabled( false );
  //mapMenus.value( MENU::WINDOW::TITLE )->setEnabled( false );
  //mapMenus.value( MENU::HELP::TITLE )->setEnabled( false );

  mapMenuActions.value( MENU::WINDOW::PANELS::MODEL_INFO )->setChecked( true );
  mapMenuActions.value( MENU::WINDOW::PANELS::VIEWS )->setChecked( true );
  mapMenuActions.value( MENU::WINDOW::PANELS::CONSOLE )->setChecked( true );
  mapMenuActions.value( MENU::WINDOW::PANELS::OLDTOOLS )->setChecked( false );
  mapMenuActions.value( MENU::WINDOW::PANELS::TOOLS)->setChecked( true );
  mapMenuActions.value( MENU::WINDOW::PANELS::TB_VIEW )->setChecked( true );
  mapMenuActions.value( MENU::WINDOW::PANELS::TB_EDIT )->setChecked( true );

  mapMenuActions.value( MENU::VIEW::MODE::RADIAL_GRID )->setChecked( true );
  mapMenuActions.value( MENU::VIEW::MODE::COLORED_EQUIL )->setChecked( false );
  return 0;
}

QMenu* CMainWnd::CreateMenu( ulong menuId )
{
  QStringList menu_strings = SM_MENU( menuId );
  QMenu* pNewMenu = new QMenu( menu_strings.first() );
  if( menu_strings.size() > 1 )
    pNewMenu->setStatusTip( menu_strings.at( 1 ) );
  mapMenus.insert( menuId, pNewMenu );
  return pNewMenu;
}

QAction* CMainWnd::CreateAction( QMenu* pMenu, ulong menuId, ShowFlag showFlag,
                                 int _img_x, int _img_y )
{
  QStringList strings = SM_MENU( menuId );
  if( strings.size() < 2 )
    strings.append( strings.first() );
  QAction*pAction = pMenu->addAction( strings[0] );
  pAction->setStatusTip( strings[1] );
  pAction->setCheckable( true );
  if( _img_x >= 0 && _img_y >= 0 ) {
    QIcon ico( pngButtons.copy( _img_x * ico_size, _img_y * ico_size, ico_size, ico_size ) );
    pAction->setIcon( ico );
  }
  pSmViewOptions->setMapping( pAction, showFlag );
  connect(pAction, SIGNAL(triggered()), pSmViewOptions, SLOT(map()) );
  mapViewOptions.insert( showFlag, pAction );
  mapMenuActions.insert( menuId, pAction );
  return pAction;
}


QAction* CMainWnd::CreateAction(
  QMenu* pMenu,
  ulong menuId,
  const char* slotMethod,
  bool  checkable,
  int _img_x, int _img_y,
  const QKeySequence &shortcuts )
{
  QAction*pAction = null;
  QStringList strings = SM_MENU( menuId );
  if( strings.size() < 2 )
    strings.append( strings.first() );

  if( checkable ) {
    pAction = pMenu->addAction( strings[0] );
    pAction->setCheckable( true );
    connect( pAction, SIGNAL(triggered(bool)), this, slotMethod );
  }
  else
    pAction = pMenu->addAction( strings[0], this, slotMethod );

  if( _img_x >= 0 && _img_y >= 0 ) {
    QIcon ico( pngButtons.copy( _img_x * ico_size, _img_y * ico_size, ico_size, ico_size ) );
    pAction->setIcon( ico );
  }

  QList<QKeySequence> keyList;
  keyList += shortcuts;
  if( !shortcuts.isEmpty() )
    pAction->setShortcuts( keyList );
  pAction->setStatusTip( strings[1] );
  mapMenuActions.insert( menuId, pAction );
  return pAction;
}

int CMainWnd::CreateActions()
{
  return 0;
}

void CMainWnd::slotActiveSubWndChanged( QMdiSubWindow* _pSubWnd )
{
  CViewWndPtr pView = ( _pSubWnd == null ) ? null :
                         qobject_cast< CViewWndPtr >( _pSubWnd->widget() );
  slotActiveViewChanged( pView );
}


class CbFindSubWindow {
  QWidget* pWgt;
public:
  CbFindSubWindow( QWidget* _pWgt ):
    pWgt( _pWgt ) {}

  bool operator()( QMdiSubWindow* _pWnd ) const {
    return _pWnd->widget() == pWgt;
  }
};

typedef QList< QMdiSubWindow* > WndList;

void CMainWnd::slotActiveViewChanged( CViewWndPtr _pView )
{
  if( _pView == null )
      UpdateMenu( _pView );
  else {
    // Find subwindow
    WndList wnds = pMdiArea->subWindowList();
    WndList::iterator it = std::find_if( wnds.begin(), wnds.end(), CbFindSubWindow( _pView ) );
    if( it == wnds.end() )
      return;
    QMdiSubWindow* pSubWindow = *it;
    if( pSubWindow == null )
      return;

    QMdiSubWindow* pCurrentSubWindow = pMdiArea->activeSubWindow();
    // Check if the subwindow is in focus
    if( pCurrentSubWindow != pSubWindow ) { // 1411  {}
      pMV->SelectCurrentView( _pView );
      pMdiArea->setActiveSubWindow( pSubWindow );

      // Update MV-manager tree
      if( pMV->CurrentView() != _pView ) {
        QString current_str = QString( "Current model: %1, view: %2" )
                              .arg( ToQString( _pView->GetModel()->Name() ) )
                              .arg( _pView->TitleName() );

        pConsole->Send( LOG_INFO, sender_name, current_str );
      }
      UpdateModelInfo( true );
      UpdateActionsInfo();
    } //1411 {}
    UpdateMenu( _pView );
  }
}


void CMainWnd::slotActiveEditorChanged( EditorWnd* _pEditor ) {
  if( _pEditor == null )
    return;

  WndList wnds = pMdiArea->subWindowList();
  WndList::iterator it = std::find_if( wnds.begin(), wnds.end(), CbFindSubWindow( _pEditor ) );
  if( it == wnds.end() )
    return;
  QMdiSubWindow* pSubWindow = *it;
  if( pSubWindow == null )
    return;

  if( pSubWindow != pMdiArea->activeSubWindow() )
    pMdiArea->setActiveSubWindow( pSubWindow );
}

void CMainWnd::UpdateMenu( CViewWndPtr _pView ) {
  if( _pView == null ) {
    pButton_L->setEnabled( false );
    pButton_M->setEnabled( false );
    pButton_R->setEnabled( false );

    mapMenus.value( MENU::FILES::IMPORT::TITLE )->setEnabled( false );
    mapMenus.value( MENU::FILES::EXPORT::TITLE )->setEnabled( false );
    mapMenuActions.value( MENU::FILES::SAVE )->setEnabled( false );
    mapMenuActions.value( MENU::FILES::OUTPUT )->setEnabled( false );
    mapMenus.value( MENU::VIEW::TITLE )->setEnabled( false );
    mapMenus.value( MENU::EDIT::TITLE )->setEnabled( false );
    mapMenus.value( MENU::COMMANDS::TITLE )->setEnabled( false );
    mapMenus.value( MENU::VARIABLES::TITLE )->setEnabled( false );
    //mapMenus.value( MENU::OPTIONS::TITLE )->setEnabled( false );
    mapMenuActions.value( MENU::OPTIONS::PROJECT_SETTINGS )->setEnabled( false );
    //mapMenuActions.value( MENU::OPTIONS::MESH_EDITING )->setEnabled( false );
    //mapMenuActions.value( MENU::OPTIONS::OUTPUT_MODE )->setEnabled( false );
    mapMenus.value( MENU::OPTIONS::SETUP::TITLE )->setEnabled( false );
    //mapMenus.value( MENU::WINDOW::TITLE )->setEnabled( false );
    mapMenuActions.value( MENU::WINDOW::NEW_VIEW )->setEnabled( false );
    mapMenuActions.value( MENU::WINDOW::CLOSE )->setEnabled( false );
    mapMenuActions.value( MENU::WINDOW::STATISTICS )->setEnabled( false );

    pToolBar_Tools->setEnabled( false );
    pToolBar_View->setEnabled( false );
    pToolBar_Edit->setEnabled( false );
  }
  else {
    // Enable or disable menus, tools and flags
    pButton_L->setEnabled( true );
    pButton_M->setEnabled( true );
    pButton_R->setEnabled( true );

    mapMenus.value( MENU::FILES::IMPORT::TITLE )->setEnabled( true );
    mapMenus.value( MENU::FILES::EXPORT::TITLE )->setEnabled( true );
    mapMenuActions.value( MENU::FILES::SAVE )->setEnabled( true );
    mapMenuActions.value( MENU::FILES::OUTPUT )->setEnabled( true );
    mapMenus.value( MENU::VIEW::TITLE )->setEnabled( true );
    mapMenus.value( MENU::EDIT::TITLE )->setEnabled( true );
    mapMenus.value( MENU::COMMANDS::TITLE )->setEnabled( true );
    mapMenus.value( MENU::VARIABLES::TITLE )->setEnabled( true );
    //mapMenus.value( MENU::OPTIONS::TITLE )->setEnabled( true );
    mapMenuActions.value( MENU::OPTIONS::PROJECT_SETTINGS )->setEnabled( true );
    //mapMenuActions.value( MENU::OPTIONS::MESH_EDITING )->setEnabled( true );
    //mapMenuActions.value( MENU::OPTIONS::OUTPUT_MODE )->setEnabled( true );
    mapMenus.value( MENU::OPTIONS::SETUP::TITLE )->setEnabled( true );
    //mapMenus.value( MENU::WINDOW::TITLE )->setEnabled( true );
    mapMenuActions.value( MENU::WINDOW::NEW_VIEW )->setEnabled( true );
    mapMenuActions.value( MENU::WINDOW::CLOSE )->setEnabled( true );
    mapMenuActions.value( MENU::WINDOW::STATISTICS )->setEnabled( true );
    UpdateVarsMenu();

    bool equilExists = _pView->GetModel()->GetFluxModel()->HasEquil();
    mapMenuActions.value( MENU::FILES::IMPORT::TOPOLOGY )->setEnabled( equilExists );
    mapMenuActions.value( MENU::EDIT::CREATE::GRIDPOINTS )->setEnabled( equilExists );
    mapMenuActions.value( MENU::EDIT::CREATE::SURFACE )->setEnabled( equilExists );
    mapMenuActions.value( MENU::COMMANDS::EDIT_TOPOLOGY )->setEnabled( equilExists );
    mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTS )->setEnabled( equilExists );
    mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTSEGS )->setEnabled( equilExists );

    bool meshExists = _pView->GetModel()->HasMesh();
    mapMenuActions.value( MENU::VIEW::MESH_HEADER )->setEnabled( meshExists );
    //mapMenuActions.value( MENU::OPTIONS::MESH_EDITING )->setEnabled( meshExists );

    for( QMap< ShowFlag, QAction* >::iterator it = mapViewOptions.begin(),
         itEnd = mapViewOptions.end();
         it != itEnd; ++it )
      it.value()->setChecked( _pView->HasShowFlag( it.key() ) );

    bool stretchMode = _pView->HasShowFlag( SHW::STRETCH );
    mapMenus.value( MENU::VIEW::STR_SHR::TITLE )->setEnabled( stretchMode );
    mapTools.value( TOOLS::STRETCH | Qt::LeftButton ).first->setEnabled( stretchMode );
    mapTools.value( TOOLS::STRETCH | Qt::RightButton ).first->setEnabled( stretchMode );
    mapTools.value( TOOLS::STRETCH | Qt::MidButton ).first->setEnabled( stretchMode );

    bool fullToolSet = (_pView->CurrentScene()->Type() == ST::SECTION);

    for( ulong btn = 1; btn <= 4; btn *= 2 ) {
      mapTools.value( btn | TOOLS::ADDELEMENT     ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::ADDGRIDPOINT   ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::ADDSOURCES     ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::ADDSURFACE     ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::MOVEMESHPOINT  ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::SPLITELEMENT   ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::CONNECTPOINTS  ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::JOINELEMENTS   ).first->setEnabled( fullToolSet );
      mapTools.value( btn | TOOLS::REPOSITION     ).first->setEnabled( fullToolSet );
    }

    pToolBar_Tools->EnableButton( TOOLS::ADDELEMENT,    fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::ADDGRIDPOINT,  fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::ADDSOURCES,    fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::ADDSURFACE,    fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::MOVEMESHPOINT, fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::SPLITELEMENT,  fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::CONNECTPOINTS, fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::JOINELEMENTS,  fullToolSet );
    pToolBar_Tools->EnableButton( TOOLS::REPOSITION,    fullToolSet );


    CViewWnd::ToolState ts = _pView->GetToolState();
    for( ulong i = 0; i < 3; i++ )
      slotSelect( ts[i].first | ts[i].second );

    mapMenuActions.value( MENU::VIEW::PREVIOUS_VIEW )->setEnabled(
          _pView->HasPreviousViewStates() );

    mapMenuActions.value( MENU::VIEW::MODE::RADIAL_GRID )->setChecked(
          _pView->RadialGrid() );

    mapMenuActions.value( MENU::VIEW::MODE::COLORED_EQUIL )->setChecked(
          _pView->ColoredEquil() );

    pToolBar_Tools->setEnabled( true );
    pToolBar_View->setEnabled( true );
    pToolBar_Edit->setEnabled( true );
  }
}

QStringList CMainWnd::GetLoadErrFlagsDescription( int errFlags )
{
  QStringList msgList;

  if( HasAnyFlag( errFlags, DGFE::OLDTOPO ) )
    msgList.push_back( SM_MSG( WRN::OLDTOPO ) );
  if( HasAnyFlag( errFlags, DGFE::OLD_SURFACES_LOST) )
    msgList.push_back( SM_MSG( WRN::OLD_SURFACES_LOST ) );
  if( HasAnyFlag( errFlags, DGFE::NOEQUIL ) )
    msgList.push_back( SM_MSG( WRN::NOEQUIL ) );
  if( HasAnyFlag( errFlags, DGFE::NOTEMPL) )
    msgList.push_back( SM_MSG( WRN::NOTEMPL ) );
  if( HasAnyFlag( errFlags, DGFE::BADTYPE) )
    msgList.push_back( SM_MSG( WRN::BADTYPE ) );
  if( HasAnyFlag( errFlags, DGFE::SYNTAX) )
    msgList.push_back( SM_MSG( WRN::SYNTAX ) );
  if( HasAnyFlag( errFlags, DGFE::NOSONNET) )
    msgList.push_back( SM_MSG( WRN::NOSONNET ) );
  if( HasAnyFlag( errFlags, DGFE::BADTARG1) )
    msgList.push_back( SM_MSG( WRN::BADTARG1 ) );
  if( HasAnyFlag( errFlags, DGFE::BADTARG2) )
    msgList.push_back( SM_MSG( WRN::BADTARG2 ) );
  if( HasAnyFlag( errFlags, DGFE::BADSTRUC) )
    msgList.push_back( SM_MSG( WRN::BADSTRUC ) );
  if( HasAnyFlag( errFlags, DGFE::BADXPT) )
    msgList.push_back( SM_MSG( WRN::BADXPT ) );
  if( HasAnyFlag( errFlags, DGFE::MESH_FP) )
    msgList.push_back( SM_MSG( WRN::MESH_FP ) );

  return msgList;
}

const char* CMainWnd::scpLstFileName = "recent.lst";

void CMainWnd::LoadRecentList()
{
  if( !QFile( sProfileDir + scpLstFileName ).exists() )
    return;
  QFile file( sProfileDir + scpLstFileName );
  file.open( QFile::ReadOnly );
  connect( &smRecentList, SIGNAL(mapped(QString)),
           this,          SLOT(slotOpen(QString)) );
  lstRecentFiles.clear();
  QByteArray line;
  QMenu* pMenu = mapMenus[ MENU::FILES::RECENT::TITLE ];
  pMenu->clear();
  while( file.bytesAvailable() != 0 ) {
    line = file.readLine();
    QString sPath( line.trimmed() );
    lstRecentFiles.append( sPath );
    QAction* pAction = pMenu->addAction( sPath, &smRecentList, SLOT(map()) );
    smRecentList.setMapping( pAction, sPath );
  }
  file.close();
}

void CMainWnd::SaveRecentList() const
{
  QFile file( sProfileDir + scpLstFileName );
  file.open( QFile::WriteOnly );
  foreach( const QString& _crsPath, lstRecentFiles ) {
    file.write( _crsPath.toUtf8().data() );
    file.write( "\n", 1 );
  }
  file.close();
}

void CMainWnd::AddToRecentList( const QString& _crsFilePath )
{
  QMenu* pMenu = mapMenus[ MENU::FILES::RECENT::TITLE ];
  int c = 0;
  for( QList< QString>::iterator it = lstRecentFiles.begin();
       it != lstRecentFiles.end(); it++, c++ ) {
    if( QFileInfo( _crsFilePath ) == QFileInfo( *it ) ) {
      it = lstRecentFiles.erase( it ) - 1;
      pMenu->removeAction( pMenu->actions().at( c ) );
    }
  }
  lstRecentFiles.push_front( _crsFilePath );
  QAction* pAction = new QAction( _crsFilePath, this );
  connect( pAction, SIGNAL(triggered()), &smRecentList, SLOT(map()) );
  smRecentList.setMapping( pAction, _crsFilePath );
  pMenu->insertAction( pMenu->actions().isEmpty() ?
                         null : pMenu->actions().first(), pAction );
  if( lstRecentFiles.size() > (int)userPrefs.common.recentList_len ) {
    lstRecentFiles.pop_back();
    pMenu->removeAction( pMenu->actions().last() );
  }
  SaveRecentList();
}

void CMainWnd::slotChangeShowFlag( ShowFlag sf, bool enable )
{
  mapViewOptions.value( sf )->setChecked( enable );
  slotDisplay( sf );
  ModelViewProxy* pProxy = pMV->CurrentProxy();
  pProxy->GetModel()->SetViewAttributes( 0, 0, 0, pProxy->CurrentView()->ShowFlags() );
}
