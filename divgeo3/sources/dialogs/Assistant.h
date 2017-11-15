#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QDialog>
#include <QToolBar>
#include <QLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QHeaderView>
#include <QAction>
#include <QScrollBar>
#include <QCloseEvent>

#include <QIcon>
#include <QPixmap>
#include <QSize>

#include <QMetaType>

#include "../common/Resources.Help.h"
#include "../main/ResourcesManager.h"
#include "../main/HelpManager.h"

typedef const DataValue* DataValueCPtr;
Q_DECLARE_METATYPE( DataValueCPtr )

class Assistant : public QDialog
{
  Q_OBJECT

  static const int ico_size = 24;
  ResourcesManager* pRM;

  QTreeWidget* pTree;
  QTextBrowser* pBrowser;

  typedef QList< QTreeWidgetItem* > ItemsList;
  ItemsList stack_prev, stack_next;
  QTreeWidgetItem* pItem_current;

public:
  explicit Assistant( ResourcesManager* _pRM, QWidget *parent = 0 );

  void closeEvent( QCloseEvent *pEv );

public slots:

  void slotShowAt( ulong _code, const QString& _crsDict );
  
private:

  QIcon MakeIcon( const QPixmap& _crSrc, int _x, int _y ) const
  { return QIcon( _crSrc.copy( _x * ico_size, _y * ico_size, ico_size, ico_size ) ); }

  void RebuildTree();
  void BuildBranch( QTreeWidgetItem* _pParent, const TreeNode* _pNode );

  void OpenPage( QTreeWidgetItem* _pItem );
  void SetText( const QStringList& _crText );
  
private slots:
  void slotItemSelected( QTreeWidgetItem* _pItem, int _column );
  void slotPrevPage();
  void slotNextPage();
  void slotOptions();
  
};

#endif // ASSISTANT_H
