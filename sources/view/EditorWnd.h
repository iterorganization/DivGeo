#ifndef EDITORWND_H
#define EDITORWND_H

#include <QWidget>
namespace dm { class DgDataModel; }
class QTreeView;
class QCloseEvent;
class QFocusEvent;

class EditorWnd: public QWidget {
  Q_OBJECT

  QTreeView* ptw;
  QString filename;
  dm::DgDataModel* pmodel;

signals:
  void EditorClosed();
  void EditorInFocus( EditorWnd* );

public:
  explicit EditorWnd( const QString& _crFilename, QWidget* _pParent = 0 );
  ~EditorWnd();

  void closeEvent( QCloseEvent* _pe );
  void focusInEvent( QFocusEvent* _pe );
};

typedef EditorWnd* EditorWndPtr;

#endif // EDITORWND_H
