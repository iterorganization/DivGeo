#ifndef EDITORWND_H
#define EDITORWND_H

#include <QWidget>

class QTreeView;

class EditorWnd: public QWidget {
  Q_OBJECT

  QTreeView* ptw;
  QString filename;

signals:
  void EditorClosed();

public:
  explicit EditorWnd( const QString& _crFilename, QWidget* _pParent = 0 );

  void closeEvent( QCloseEvent* pe );
};

#endif // EDITORWND_H
