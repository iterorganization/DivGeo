#ifndef CONSOLEWND_H
#define CONSOLEWND_H

#include <string>

#include <QMainWindow>
#include <QStatusBar>
#include <QDockWidget>
#include <QLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QResizeEvent>
#include <QDialog>
#include <QProgressDialog>
#include <QLabel>
#include <QTimer>

#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QBrush>

#include "../common/IModelAgent.h"
#include "../dialogs/DlgProgress.h"
#include "StringsManager.h"
#include "ResizableWidget.h"
#define null NULL

#define SENDER_NAME_Q( str ) QString dg__sender_name__( str )

class Console:
  public QDockWidget
{
  Q_OBJECT

private:
  QTextEdit* pLog;
  QLineEdit* pCommand;
  QMainWindow* pWnd;
  StringsManager* pSM;
  ResizableWidget* dockWidgetContents;
  QTextDocument collectedText;

  LoggingMode mode;
  bool isSaved;         // Save flag is true if log text is saved to file
  DlgProgress waitWnd;  // Dialog with progress bars, contents current processes' messages
  bool prev_process;    // This flag is true if previous message was a process one.
                        // Used for better text formatting.
  bool fail;            // Fail flag is true if process was failed or interrupted.
                        // Used for text formatting and for detecting error reason.

public:
  Console( QWidget *parent = 0 );
  ~Console();
  void setup( StringsManager* _pSM, QMainWindow* _pWnd = null );
  void UpdateCursor() { pLog->ensureCursorVisible(); }

  void OutputCollection();
  void ClearCollection();

  void SetFailFlag() { fail = true; }
  void SetLoggingMode( LoggingMode _mode ) { mode = _mode; }
  void Progress( ulong pct );

public slots:
  void Send( MessageType mt, AlarmLevel al, const QString& _sender, const QString& _text );
  ulong Send( MessageType mt, AlarmLevel al, const QString& _sender, ulong msg_code, const QStringList& args = QStringList() );

private slots:
  void slotClear();
  void slotSave();

};

#endif // CONSOLEWND_H
