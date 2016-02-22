#ifndef MODELVIEWPROXY_H
#define MODELVIEWPROXY_H

#include <QObject>
#include <QMap>
#include <QTreeWidget>
#include <QPushButton>

#include "../view/ViewWnd.h"
#include "../view/ViewScene.h"
#include "../view/ViewTopScene.h"
#include "../dialogs/DlgVarsEdit.h"
#include "../dialogs/DlgVarHelp.h"
#include "../dialogs/DlgTopology.h"
#include "../dialogs/DlgCreateSurface.h"
#include "../dialogs/DlgCreateGridPoint.h"
#include "../dialogs/DlgStatictics.h"
#include "../common/StringConverters.h"
#include "SessionData.h"
#include "../common/IModelAgent.h"

typedef QMap< CViewWndPtr, QTreeWidgetItem* > ViewMap;

class ModelViewManager;
class EditorWnd;

class ModelViewProxy:
    public QObject
{
  Q_OBJECT

  /* global */
  QTreeWidget*            pTree;
  ModelViewManager*       pManager;
  CMainWnd*               pMainWnd;
  const UserPreferences*  pPrefs;
  Console*                pConsole;
  StringsManager*         pSM;

  /* local */
  ModelPtr            pModel;
  SceneList           scenes;
  ViewMap             mapViews;
  QTreeWidgetItem*    pModelItem;
  MapVarsEditDialogs  mapVarEditDlgs;
  CViewWndPtr         pTopologyView;
  CViewWndPtr         pCurrentView;
  ulong               id_view;
  QSignalMapper       smDialogs;

  EditorWnd*          pEditor;
  DlgCreateSurface*   pCreateSurfaceDlg;
  DlgCreateGridPoint* pCreateGridPointDlg;
  DlgStatictics*      pStatisticsDlg;//1408

  QTimer upTimer; //1409
  bool upEnabled;  //1409

  bool isSaved;

public:
  /* Constructors */
  ModelViewProxy(ModelPtr _pModel, QTreeWidget* _pTree, ModelViewManager* _pManager,
                  CMainWnd* _pMainWnd, const UserPreferences* _pPrefs,
                  Console* _pConsole, StringsManager *_pSM );
  ~ModelViewProxy();

  /* Getters */
  ModelPtr          GetModel() const { return pModel; }
  const SceneList&  Scenes() const { return scenes; }
  QTreeWidgetItem*  ViewItem( CViewWndPtr _pView ) const { return mapViews.value( _pView ); }
  QTreeWidgetItem*  ModelItem() const { return pModelItem; }
  CViewWnd*         CurrentView() const { return pCurrentView; }
  bool              HasView( CViewWndPtr _pView ) const { return mapViews.contains( _pView ); }

  /* Setters */
  bool SetCurrentView( CViewWndPtr _pView );
  void MarkSaved() { isSaved = true; }

  /* View actions */
  CViewWndPtr CreateView();
  void RemoveView( CViewWndPtr _pView );
  void UpdateScenes( ulong sfs = SHW::ALL );
  void UpdateStyle();
  void ShowAll( bool show );
  void UpdateModelName();
  void SaveModel( bool _ask = false );
  void SaveModelAs( const QString& _crsName, bool _bCopy = false );

  void LoadSessionData( const SessionModelRecord& _crSMR );
  SessionModelRecord SaveSessionData() const;
  void UpdateViewsGeometry();

  void OpenEditor();
  DlgCreateSurface* OpenCreateSurfacesDlg();
  DlgCreateGridPoint* OpenCreateGridPointsDlg();
  DlgStatictics* OpenStatisticsDlg();//1408
  void ChangeOutputModeForDialogs( int _om );

signals:
  void ViewSelected( CViewWndPtr _pView );
  void ViewCreated( CViewWndPtr _pView );
  void DeleteProxy( ModelViewProxy* _pProxy );

public slots:
  void UpdateViewBranch(CViewWnd* pView );
  void UpdateViews( const UpdateInfo& _crUI = UpdateInfo(), bool _force = true ); //1409 force
  void UpdateStatistics();

  void slotCreateView() { CreateView(); }
  void slotCreateVarsEditDlg( VarSetPtr _pVS );
  void slotCloseVarsEditDlg( QWidget* _pWgt );
  void OpenTopologyDialog();
  void slotCloseTopologyDlg();

private slots: //1409
  void slotEnableUpdate();
};
typedef ModelViewProxy* ModelViewProxyPtr;

#endif // MODELVIEWPROXY_H
