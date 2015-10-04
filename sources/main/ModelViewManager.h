#ifndef VIEWSINFO_H
#define VIEWSINFO_H

#include <string>

#include <QMainWindow>
#include <QDockWidget>
#include <QLayout>
#include <QString>
#include <QTreeWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QSignalMapper>

#include "../common/Metatypes.h"
#include "../common/StringConverters.h"
#include "StringsManager.h"
#include "ResizableWidget.h"
#include "Console.h"
#include "SessionData.h"

#include "ModelViewProxy.h"
#include "ModelAgent.h"

class ModelViewManager : public QDockWidget
{
  Q_OBJECT

  QString sender_name;

  CMainWnd* pMainWnd;
  const UserPreferences* pPrefs;
  Console* pConsole;
  StringsManager* pSM;
  static const char* scpSessionFileName;

  mutable QFileDialog fileDialog;

  ResizableWidget *dockWidgetContents;
  QTreeWidget* pTree;

  QMap< ModelPtr, ModelViewProxyPtr > proxies;

  ulong id_model;

  ModelViewProxyPtr pCurrentProxy;

  QSignalMapper smButtons;

  SessionData session;
  QStringList sessionModels;

public:
  explicit ModelViewManager( CMainWnd* _pMainWnd, const UserPreferences* _pPrefs,
                             Console* _pConsole,
                             StringsManager *_pSM, QWidget *parent = 0);
  void Delete();
  void SaveAll();

  ModelViewProxyPtr NewModel( const QString& _crsPath = "", bool _createView = true );
  ModelViewProxyPtr OpenModel( const QString& _crsPath = "", bool _createView = true );
  QString SelectModelPath() const;
  QString SelectAnotherPath( const QString& _crsPath );
private:
  bool LoadModel( ModelPtr _pModel, const QString& _crsPath );
  ModelViewProxyPtr AddModel( ModelPtr _pModel, ModelAgent* _pAgent );
public:
  ModelViewProxyPtr ModelProxy( ModelPtr _pModel ) const
  { return proxies.value( _pModel, null ); }

  void SkipSessionBut( const QStringList &_crsPaths = QStringList() );

  int         GetNewModelId() { return id_model++; }
  bool        ModelIsLoaded( const QString& _crsPath ) const;
  CViewWndPtr CurrentView() const { return pCurrentProxy == null ? null : pCurrentProxy->CurrentView(); }
  ModelPtr    CurrentModel() const { return pCurrentProxy == null ? null : pCurrentProxy->GetModel(); }
  ModelViewProxyPtr CurrentProxy() const { return proxies.value( CurrentModel(), null ); }
  ModelViewProxyPtr GetProxy( CViewWndPtr _pView ) const;

  void UpdateAllViewsStyle();
  void UpdateAllViewsGeometry();
  void SaveSessionData();
  void LoadSessionData();
  SessionData* GetSessionData() { return &session; }

signals:
  void ViewSelected( CViewWndPtr pView );
  void ViewCreated( CViewWndPtr pView );
  
public slots:
  void RemoveModel( ModelViewProxy* _pProxy );
  void RemoveCurrentModel() { RemoveModel( pCurrentProxy ); }

  void SelectCurrentView(CViewWndPtr _pSelectedView );
  void SelectionChanged( QTreeWidgetItem* current, int );
};

#endif // VIEWSINFO_H
