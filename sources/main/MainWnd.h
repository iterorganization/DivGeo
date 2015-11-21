#ifndef MAINWND_H
#define MAINWND_H

#include "time.h"

#include <qglobal.h>
#include <QWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QGraphicsScene>
//#include <QMainWindowLayout.h>

#include <qnamespace.h>

#include <QStringList>

#include <QList>
#include <QMap>
#include <QFile>

#include "../common/Version.h"
#include "../common/Metatypes.h"
#include "../view/ViewWnd.h"
#include "../view/ViewScene.h"
#include "../view/ViewTopScene.h"
#include "../dialogs/DialogsDecl.h"

#include "Console.h"
#include "UserPreferences.h"
#include "ProjectSettings.h"
#include "HelpManager.h"
#include "../dialogs/Assistant.h"
#include "ModelInfo.h"
#include "ActionStackInfo.h"
#include "ModelViewManager.h"
#include "ToolsPanel.h"

#define GET_CURRENT_VIEW_RET( view, ret ) \
  QMdiSubWindow* dg__pSubWnd__ = pMdiArea->currentSubWindow(); \
  if( dg__pSubWnd__ == null ) return ret; \
  CViewWnd* (view) = qobject_cast< CViewWnd* >( dg__pSubWnd__->widget() ); \

#define GET_CURRENT_VIEW_AND_MODEL_RET( view, model, ret ) \
  GET_CURRENT_VIEW_RET( view, ret ) \
  ModelPtr model = view->GetModel(); \

#define GET_CURRENT_VIEW_AND_MODEL_BOOL( view, model ) \
  GET_CURRENT_VIEW_AND_MODEL_RET( view, model, false )
#define GET_CURRENT_VIEW_BOOL( view ) \
  GET_CURRENT_VIEW_RET( view, false )

#define GET_CURRENT_VIEW_AND_MODEL( view, model ) \
  GET_CURRENT_VIEW_AND_MODEL_RET( view, model, )
#define GET_CURRENT_VIEW( view ) \
  GET_CURRENT_VIEW_RET( view, )

#define FOREACH_VIEW( name ) foreach( CViewWndPtr name, pMV->Views( pModel ) )


struct LastPath {
  QString sPathProject;
  QString sPathTemplate;
  QString sPathEquil;
  QString sPathTopology;
  QString sPathMesh;
};

class CMainWnd : public QMainWindow
{
  Q_OBJECT

private:

  QString sender_name;

  QString version;

  QPixmap pngButtons;
  static const int ico_size = 24;

  // Objects
  Console* pConsole;
  StringsManager* pSM;
  HelpManager* pHelp;

  ModelInfo* pModelInfo;
  ActionStackInfo* pActInfo;
  ModelViewManager* pMV;
  Assistant* pAssistant;
  DlgVarHelp* pDlgVarHelp;
  QDockWidget *pToolBar_old;
  ToolsPanel* pToolBar_Tools;
  QToolBar* pToolBar_View;
  QToolBar* pToolBar_Edit;

  QString sProfileDir;
  UserPreferences userPrefs;

  mutable LastPath lastPath;

  int ErrorCode;
  int highlightLocks;
  bool highlightMode;

  QMdiArea *pMdiArea;

  QMenuBar *pMenuBar;
  QStatusBar *pStatusBar;

  QPushButton *pButton_L;
  QPushButton *pButton_M;
  QPushButton *pButton_R;

  /* Tools panel menu type, map and mapper */
  QSignalMapper *pSmToolSelect;
  typedef QPair< QAction*, const char* > ToolActionPair;
  typedef QMap< ulong, ToolActionPair > MapTools;
  MapTools mapTools;

  /* View > Display menu type, map and mapper */
  QSignalMapper *pSmViewOptions;
  typedef QMap< ShowFlag, QAction* > MapViewOptions;
  MapViewOptions mapViewOptions;

  /* Menu and menu actions types and map */
  typedef QMap< ulong, QAction* > MapMenuActions;
  MapMenuActions mapMenuActions;
  typedef QMap< ulong, QMenu* > MapMenus;
  MapMenus mapMenus;

  /* Variables menu types, maps and mappers */
  struct SVarActions { enum EVarActions { ADD, REMOVE, EDIT }; };
  typedef SVarActions VA;
  typedef SVarActions::EVarActions VarAction;
  QSignalMapper smVarsAdd;
  QSignalMapper smVarsRemove;
  QSignalMapper smVarsEdit;
  typedef QMap< QString, QAction* > MapVarMenuActions;
  MapVarMenuActions mapVarsAdd;
  MapVarMenuActions mapVarsRemove;
  MapVarMenuActions mapVarsEdit;

  /* RecentList */
  static const char* scpLstFileName;
  QList< QString > lstRecentFiles;
  QSignalMapper smRecentList;

  /* Config */
  static const char* scpDefaultDgcFileName;
  QString sDgcFileName;
  bool useConfig;

 /**********************************************
   MENU BUILDERS
  **********************************************/
private:

  /* Create menu action for display option specified by showFlag. Connected via viewOptionsMapper.*/
  QAction* CreateAction(QMenu* pMenu, ulong menuId, ShowFlag showFlag,
                        int _img_x = -1, int _img_y = -1 );

  /* Create standart menu action connected to slotMethod */
  QAction* CreateAction(QMenu* pMenu, ulong menuId, const char* slotMethod,
                         bool checkable = false, int _img_x = -1, int _img_y = -1,
                        const QKeySequence &shortcuts = QKeySequence() );

  /* Create menu action for mouse tool specified by toolid. Connected to slotMethod. */
  QAction* CreateAction( QMenu* pMenu, Qt::MouseButton button,
                         ToolId toolId, const char* slotMethod, bool checkable = false,
                         const QKeySequence &shortcuts = QKeySequence() );

  /* Create menu action or submenu with actions for 'Variables' menu */
  QAction* CreateAction( QMenu* _pMenu, VarSetDefPtr _pVSD, VarAction _va );

  /* Create menu action for 'Variables' menu */
  QAction* CreateAction( QMenu* _pMenu, const QString& _crsName,
                         const QString& _crsDescr, VarAction _va );

  /* Create sub menu */
  QMenu* CreateMenu(ulong menuId );

  /* Create dg3-panels buttons */
  QToolButton* CreateModeButton( QLayout* _pLo, const QString& _crsTitle,
                                 const QString& _crsToolTip, bool _isChecked);

  /* Create popup tool button */
  QPushButton* CreatePopupToolButton( QLayout* _pLo, const QString& _crsLabel,
                                      QMenu* _pMenu );

  QAction* CreateSeparator();

  int CreateActions();
  int CreateMenuBar();
  void CreateOldToolBar();
  void CreateToolBars();
  void UpdateVarsMenu();

  void LoadRecentList();
  void SaveRecentList() const;
public:
  void AddToRecentList( const QString& _crsFilePath );

  /**********************************************
   CONSTRUCTORS AND OTHER
  **********************************************/
public:
  CMainWnd( const QStringList& _crArgs, Console *_pConsoleWindow,
            StringsManager* _pSM, HelpManager* _pHM,
            QWidget *parent = 0, Qt::WindowFlags flags = 0 );
  ~CMainWnd();

  void closeEvent( QCloseEvent* ev );
  bool HasActiveView( CViewWnd* pV );
  bool DeleteObject(IComponentPtr _pObject , bool _single = false );
  QString SelectModelFile( ModelPtr _pModel ) const;
  QStringList GetLoadErrFlagsDescription( int errFlags );

public slots:
  void slotChangeShowFlag( ShowFlag sf, bool enable );
  void slotViewClosed( CViewWnd* _pView );
  void UpdateModelInfo() { pModelInfo->UpdateContentModel( pMV->CurrentModel() );
                           pModelInfo->UpdateContentSelected( null ); }
  void UpdateObjectInfo( IComponentPtr _pObject ) { pModelInfo->UpdateContentSelected( _pObject ); }
  void UpdateActionsInfo() { pActInfo->UpdateContent( pMV->CurrentModel() ); }
  void slotEnableUndoStack( bool enabled );
  void slotNewView( CViewWndPtr _pView );
  void slotAtStart();
  void slotSelect( int id );
  void slotDialogHelp(int _code );
  void slotVarHelp( const QString& _crsTitle, const QString& _crsText );

  /**********************************************
   MENU SLOTS
  **********************************************/
private slots:

  void slotActiveSubWndChanged( QMdiSubWindow* pSubWnd );
  void slotActiveViewChanged( CViewWndPtr _pView );

  /* menu */

  // File
  void slotNew();
  void slotOpen( QString _crsFilePath = "" );
  void slotReload();
  void slotSave();
  void slotSaveAs();
  void slotSaveCopy();
  void slotImportTemplate();
  void slotImportEquilibrium();
  void slotImportMesh();
  void slotImportTopology();
  //void slotImportSonnetGrid();
  void slotExportMesh();
  void slotExportElements();
  void slotOutput();
  void slotPrint();
  void slotExit();

  // Edit
  void slotUndo();
  void slotRedo();
  void slotRedoAll();
  void slotClearActionStack();
  void slotMarkAllElements();
  void slotMarkAllSeparators();
  void slotMarkAllChords();
  void slotUnmarkAll();
  // Edit > Create
  void slotCreatePoint();
  void slotCreateSource();
  void slotCreateChord();
  void slotCreateSurface();
  void slotMarkBoundingElem( int area );
  void slotCreateGridPoints();
/*void slotCreateSeparators();*/ /*2.0*/
  // Edit > Delete
  void slotDeleteMarkedObjects();
  void slotDeleteUnusedPoints();
  void slotDeleteVirtualSurfaces();
  void slotDeleteElements();
  void slotDeleteSources();
  void slotDeleteChords();
  void slotDeleteSurfaces();
  void slotDeleteGridPoints();
  void slotDeleteSeparators();
  void slotDeleteEquilibrium();
  void slotDeleteTemplate();
  void slotDeleteMesh();
/*void slotDeleteXPoint();*/ /*2.0*/
  void slotDeleteSonnetGrid();
  // Edit
  void slotExtendChords();
  void slotMoveRotate();

  // View
  void slotRefresh();
  void slotZoomIn();
  void slotZoomOut();
  void slotPictureView();
  void slotSelectionView();
  // View > Rotate
  void slotRotateIncreaseAngle();
  void slotRotateDecreaseAngle();
  void slotRotateSetAngle();
  void slotRotateRectify();
  // View > Stretch/Shrink
  void slotStretchHorizontal();
  void slotStretchVertical();
  void slotShrinkHorizontal();
  void slotShrinkVertical();
  void slotStretch();
  void slotJustify();
  // View
  void slotPreviousView();
  void slotRemoveLabels();
  // View > Mode
  void slotModeStretch( bool enable );
  void slotModeTopDownView( bool enable );
  void slotModeRadialGrid( bool enable );
  void slotModeColoredEquil( bool enable );
  // View > Display
  void slotDisplay( int value );
  // View
  void slotMeshHeader();

  // Commands
  // Commands > _C
  void slotConvertTemplate2Elements();
  void slotConvertElements2Chords();
  void slotConvertChords2Elements();
  // Commands > Simplify
  void slotSimplifyFillGaps();
  void slotSimplifyLineUpAllNormals();
  void slotSimplifyLineUpMarkedNormals();
  void slotSimplifyMergeSplitElements();
  // Commands
  void slotRenumberElements();
  void slotCheckVariables();
  void slotRebuildCarreObjects();
  void slotFindXPoints();
  void slotFindXPointSegs();
  void slotEditTopology();
  void slotUpdateTopology();

  // Variables
  void slotAddLayer( const QString& _crsVarName );
  void slotRemoveLayer( const QString& _crsVarName );
  void slotDisplayLayer( const QString& _crsVarName );

  // Options
  void slotProjectSettings();
/*void slotOutputMode();*/      /* since 3.0 moved to settings dialog */
/*void slotMeshEditing();*/     /* since 3.0 moved to settings dialog */
/*void slotDisplayMenu();*/     /* removed in 3.0 */
/*void slotDisplayToolbar();*/  /* removed in 3.0 */
/*void slotManualRefresh();*/   /* removed in 3.0 */
/*void slotSaveInterval();*/    /* since 3.0 moved to settings dialog */
  void slotSettings();
  // Options > Setup
  void slotSetupVariables();
  void slotSetupSave();

  // Window
/*void slotHideMenu();*/ /*removed in 3.0*/
  void slotNewView();
  void slotClose();
  void slotStatistics();
/*void slotToolbox();*/ /*use slotShowToolPanel() instead*/
  // New panels in 3.0
  void slotShowModelInfoPanel( bool _show ) { pModelInfo->setVisible( _show ); }
  void slotShowActionInfoPanel( bool _show ){ pActInfo->setVisible( _show ); }
  void slotShowViewsPanel( bool _show )     { pMV->setVisible( _show ); }
  void slotShowConsole( bool _show )        { pConsole->setVisible( _show ); }
  void slotShowOldToolBar( bool _show )     { pToolBar_old->setVisible( _show ); }
  void slotShowToolPanel( bool _show )      { pToolBar_Tools->setVisible( _show ); }
  void slotShowToolBar_View( bool _show )   { pToolBar_View->setVisible( _show ); }
  void slotShowToolBar_Edit( bool _show )   { pToolBar_Edit->setVisible( _show ); }

  // Help
  void slotWhatsNew();
  void slotManual();
  void slotUsingHelp();
  void slotOnMainWindow();
  void slotAbout();
};

#endif // MAINWND_H
