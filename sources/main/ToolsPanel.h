#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include <QDockWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

#include "../common/Resources.Menu.h"
#include "MouseToolPushButton.h"
#include "../main/StringsManager.h"

#include <QMap>
#include <QPixmap>
#include <QFont>
#include <QSignalMapper>

class ToolsPanel : public QDockWidget
{
  Q_OBJECT

  StringsManager* pSM;

  //QPixmap pngTools32;
  QPixmap pngTools24;
  static const int ico_size = 24;

  typedef QMap< ToolId, MouseToolPushButton* > ButtonsMap;
  ButtonsMap mapButtons;
  MouseToolPushButton* pBtn_left;
  MouseToolPushButton* pBtn_middle;
  MouseToolPushButton* pBtn_right;

  QSignalMapper smLeftClick;
  QSignalMapper smMiddleClick;
  QSignalMapper smRightClick;

public:
  explicit ToolsPanel( StringsManager* _pSM, QWidget* _pParent );

  void EnableButton(ToolId _toolId, bool _enable );

private:
  MouseToolPushButton* AddButton(QVBoxLayout* _pLo, ToolId _id, int _img_x, int _img_y);

public slots:
  void slotToolSelected( int _id );

signals:
  void toolSelected( int _id );
  
};

#endif // TOOLSPANEL_H
