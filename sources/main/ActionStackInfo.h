#ifndef UNDOSTACKINFO_H
#define UNDOSTACKINFO_H

#include "ModelTree.h"

#include <QDockWidget>

#include "ResizableWidget.h"

class ActionStackInfo:
    public QDockWidget,
    public ModelTree
{
  Q_OBJECT

  QTreeWidget* pTree;

  ResizableWidget* dockWidgetContents;

  QTreeWidgetItem* pUndoBranch;
  QTreeWidgetItem* pRedoBranch;

public:
  explicit ActionStackInfo( StringsManager* _pSM, QWidget* _pParent = 0 );

public slots:
  void UpdateContentUndo( ModelPtr _pModel );
  void UpdateContentRedo( ModelPtr _pModel );
  void UpdateContent( ModelPtr _pModel );

private:
  void NullModel();

private slots:
  void UpdateItem( QTreeWidgetItem* _pItem ) { UpdateTreeItem( _pItem ); }
};


#endif // UNDOSTACKINFO_H
