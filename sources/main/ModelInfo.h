#ifndef MODELINFO_H
#define MODELINFO_H

#include "ModelTree.h"

#include <QDockWidget>

#include "ResizableWidget.h"

class ModelInfo:
    public QDockWidget,
    public ModelTree
{
  Q_OBJECT

  IComponentPtr pObject;

  QTreeWidget* pTree;

  ResizableWidget* dockWidgetContents;

  QTreeWidgetItem* pSelectBranch;
  QTreeWidgetItem* pModelBranch;
  QTreeWidgetItem* pTopologyBranch;
  QTreeWidgetItem* pEquilBranch;
  QTreeWidgetItem* pMeshBranch;

public:
  explicit ModelInfo( StringsManager* _pSM, QWidget *parent = 0);

public slots:
  void UpdateContentSelected( IComponentPtr _pObject );
  void UpdateContentModel( ModelPtr _pModel );

private slots:
  void UpdateItem( QTreeWidgetItem* _pItem ) { UpdateTreeItem( _pItem ); }
};

#endif // MODELINFO_H
