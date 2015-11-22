#ifndef _VIEWWND_H
#define _VIEWWND_H

#include <QVector>

#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QColor>
#include <QRgb>
#include <QMap>
#include <QPoint>
#include <QTimer>

#include <QMenuBar>
#include <QAction>

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>

#include "../common/PointConverters.h"
#include "../common/UpdateInfo.h"
#include "../main/StringsManager.h"
#include "../main/SessionData.h"
#include "ViewScene.h"
#include "ToolData.h"


class CMainWnd;
class DlgTopology;  //#include "../dialogs/DlgTopology.h"


static const double MIN_RECTW = 1e-4;
static const double MIN_RECTH = 1e-4;

class CViewWnd: public QGraphicsView
{
  Q_OBJECT

private:

  QString sender_name;  // Name shown in messages
  QString title_name;   // Name shown in title

  // Objects
  Console* pConsole;          // History log (global)
  StringsManager* pSM;      // Resource manager (global)
  CMainWnd* pMainWnd;         // Main window (global)
  const UserPreferences* pPrefs; // All settings (global)
  ModelPtr pModel;            // Current model (global)
  IViewScene* pScene;         // Current scene (global, changeable)
  IToolDataPtr pTool;         // Current tool (local, changeable)
  SViewState* pState;         // View state (local)
  QMap< SceneType, IViewScene* > mapScenes; // Table of scenes (global)
  DlgTopology* pTopologyDialog;// Topology dialog (global, changeable)

  SurfaceExPtr pLastExaminedSX;
  GridPointExPtr pLastExaminedGPX;

  QTimer upTimer;

  // Flags
  ulong showFlags; // ShowFlags register
  ulong updateFlag; // TODO: ?
  bool bHrShown;

  // Tools state
  ToolId toolId_left;
  ToolId toolId_middle;
  ToolId toolId_right;
  Qt::MouseButton currentButton;

  // Mouse state
  QTimer doubleClickTimer;
  QMouseEvent* pLastEvent;
  QPoint pressPosition;
  bool bReleaseQueued;
  bool bDoubleClickProcceed;

  // World in view state
  SViewState state;

public:
  CViewWnd(ModelPtr _pModel, const QString &_name,
           Console* _pConsole,StringsManager* _pSM,
           CMainWnd* _pMainWnd, const UserPreferences* _pPrefs );
  ~CViewWnd();

  void LoadSessionData( const SessionViewRecord& _crSVR );
  SessionViewRecord SaveSessionData() const;

  void SetTopologyDialog( DlgTopology* _pDlg ) { pTopologyDialog = _pDlg; }

  /* Add new scene to view */
  void AddScene( SceneType _sceneType, IViewScenePtr _pScene );
  void SelectCurrentScene( SceneType _sceneType );

  void UpdateSenderName() { sender_name = QString( "View-%1 (Model-%2)" ).arg( title_name ).arg( QString::fromStdString( pModel->Name() ) ); }
  void UpdateWindowTitle() { setWindowTitle( QString( "View-%1 (%2 / %3)" ).arg( title_name ).arg( QString::fromStdString( pModel->Name() ) ).arg( pScene->Name() ) ); }
  /* Set View message for examining objects */
  void SetExamineMsg(IComponentPtr _pObject, bool toLog = false );

signals:
  /* Request to change show flag via View/Display menu */
  void ChangeShowFlag( ShowFlag, bool );
  /* Request to update information in model tree widget */
  void UpdateModelInfo();
  void UpdateObjectInfo( IComponentPtr _pObject );
  /* View transformation matrix has been changed */
  void UpdateViewState( CViewWnd* pView );
  /* Request to change UndoStack state */
  void UndoStackEnabled( bool enabled );
  /* View is closing */
  void ViewClosed( CViewWnd* pView );
  /* Model object has been selected */
  void ObjectSelected( IComponentPtr _pObject );
  /* Request to update all views of this model */
  void UpdateViews( const UpdateInfo& _crUI = UpdateInfo(), bool _force = true );

  /*********************************************
   MOUSE TOOLS 
  *********************************************/

private:
  virtual void mousePressEvent      ( QMouseEvent* me );
  virtual void mouseReleaseEvent    ( QMouseEvent* me );
  virtual void mouseMoveEvent       ( QMouseEvent* me );
  virtual void mouseDoubleClickEvent( QMouseEvent* me );
  virtual void wheelEvent           ( QWheelEvent *pe );
  virtual void resizeEvent          ( QResizeEvent *pe );
  virtual void paintEvent           ( QPaintEvent* pe );
  virtual void closeEvent           ( QCloseEvent* pe );

private slots:
  void mousePress();

signals:
  void signalMouseLeftButton  ( QPoint position, EEventID eventid );
  void signalMouseMiddleButton( QPoint position, EEventID eventid );
  void signalMouseRightButton ( QPoint position, EEventID eventid );

  /**********************************************
   TOOLS
  **********************************************/

public slots:

  void slotZoomPan        ( QPoint position, EEventID eventid );
  void slotMark           ( QPoint position, EEventID eventid );
  void slotExamine        ( QPoint position, EEventID eventid );
  void slotRotate         ( QPoint position, EEventID eventid );
  void slotStretch        ( QPoint position, EEventID eventid );
  void slotMove           ( QPoint position, EEventID eventid );
  void slotDelete         ( QPoint position, EEventID eventid );
  void slotAddElement     ( QPoint position, EEventID eventid );
  void slotSplitElement   ( QPoint position, EEventID eventid );
  void slotJoinElements   ( QPoint position, EEventID eventid );
  void slotConnectPoints  ( QPoint position, EEventID eventid );
  void slotReposition     ( QPoint position, EEventID eventid );
  void slotReverseNormals ( QPoint position, EEventID eventid );
  void slotAddSources     ( QPoint position, EEventID eventid );
  void slotAddChords      ( QPoint position, EEventID eventid );
  void slotAddGridPoint   ( QPoint position, EEventID eventid );
  void slotAddSurface     ( QPoint position, EEventID eventid );
  void slotMoveMeshPoint  ( QPoint position, EEventID eventid );
  void slotAdjustChord    ( QPoint position, EEventID eventid );
  void slotExtendChord    ( QPoint position, EEventID eventid );
  void slotAddComment     ( QPoint position, EEventID eventid );

  /**********************************************
   DRAW
  **********************************************/
  void slotUpdateView();

private:

  /* Adjust view_point to scene borders */
  QPoint NearestScenePosition(const QPoint &view_point );

  /* For MoveMeshPoint tool: */
  /* -- Collect information about neighbours */
  bool SetupSlideMeshPoint(ToolMoveMeshPointPtr pToolMove );
  /* -- Process mesh geometry changing */
  void DoSlideMeshPoint(ToolMoveMeshPointPtr pToolMove, Point position );
  /* -- Highlight mesh point and connected geometry */
  ViewItemsSet MeshPointDragHighlightWithBorder( MeshPointPtr _pPoint, bool include = true );
  ViewItemsSet MeshPointDragHighlightError( MeshPointPtr _pPoint, bool include = true );
  ViewItemsSet MeshPointDragHighlightUpdate(MeshPointPtr _pPoint, bool include = true );
  /* -- Change model data and update view items geometry */
  void MeshPointChangeGeometry(MeshPointPtr _pPoint, Point _position, bool _freeMotion = false );
  /* -- Get view items for all geometry connected to processed points */
  ViewItemsSet MeshPointsViewItems( IComponentList& processed_points );

  void CreateAutosaveInfo();

  /* For slotDelete() */
  void SelectItem();

public slots:
  /* Remove items associated with object from each scene and delete them */
  void RemoveObjectItem( IComponentPtr _pObject );
  /* Remove items for each object in list */
  void RemoveObjectListItems( const IComponentList& _pObjects );
  /* Add items associated with object to each scene, check if existed */
  void CreateObjectItem( IComponentPtr _pObject );
  /* Add items for each object in list, check if existed */
  void CreateObjectListItems( const IComponentList& _pObjects );
  /* Update geometry of items associated with object in each scene */
  void UpdateObjectItem( IComponentPtr _pObject );
  /* Update geometry of each item in list */
  void UpdateObjectListItems( const IComponentList& _pObjects );

public:

  /* new staff */
  void IncludeShowFlag( ShowFlag flag ) { showFlags |= flag; pScene->UpdateVisibility( flag, true ); viewport()->update(); }
  void ExcludeShowFlag( ShowFlag flag ) { showFlags &= ~flag; pScene->UpdateVisibility( flag, false ); viewport()->update(); }
  ulong PrepareHitFlags( ulong hitFlags );

  void SetUpdateFlag( ulong flag ) { updateFlag = flag; }

  void SetupViewState();

  typedef QVector< QPair< Qt::MouseButton, ToolId > > ToolState;
  ToolState GetToolState();
  void SetToolState( Qt::MouseButton mb, ToolId toolid );

  ulong ShowFlags() { return showFlags; }
  bool HasShowFlag( ulong showFlag ) { return HasAnyFlag( showFlags, showFlag ); }
  void UpdateShowFlag( ShowFlag showFlag, bool value);

  void SetEditTopology( bool enable ) { pState->editTopology = enable; }

  /* Draw */
  void SetStretchMode( bool enable );
  void SetRadialGridMode( bool _enable );
  void SetColoredEquilMode( bool _enable );
  void SetAngle(double angle_rad, bool saveState = true );
  void RotateCW( double angle_rad = DEG2RAD );
  void RotateCCW( double angle_rad = DEG2RAD );
  void ResetAspectRatio( bool updateView = false );
  void Scale( const Point& _crK, bool saveState = true );
  void SetScale( const Point& _crScale, bool saveState = true );
  void FitToModel();
  void FitToSelected();
  void FitToSceneRect( QRectF _rect );
  void ApplyTransform();
  void SaveState();
  void SetCenter( const QPointF& _crCenter );

  void SetPreviousViewState();
  void SetNextViewState();

  void UnhighlightAll();

  /* Getters */
  const QString& TitleName() const { return title_name; }
  const QPointF& GetCenter() const { return state.center; }
  const Point& GetScale() const { return state.scale; }
  double GetAngleRad() const { return state.xyAngle; }
  double GetAngleDeg() const { return state.xyAngle * RAD2DEG; }
  bool RadialGrid() const { return state.radialGrid; }
  bool ColoredEquil() const { return state.coloredEquil; }

  bool HasPreviousViewStates() const { return pScene != null && pScene->HasUndoStateRecords(); }
  bool HasNextViewStates() const { return pScene != null && pScene->HasRedoStateRecords(); }

  ModelPtr GetModel() const { return pModel; }
  IViewScene* CurrentScene() const { return pScene; }
  DlgTopology* TopologyDialog() const { return pTopologyDialog; }

  SurfaceExPtr* LastExaminedSX() { return &pLastExaminedSX; }
  GridPointExPtr* LastExaminedGPX() { return &pLastExaminedGPX; }
};

typedef CViewWnd* CViewWndPtr;
typedef QList< CViewWndPtr > ViewList;

#endif
