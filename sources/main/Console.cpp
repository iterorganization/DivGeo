#include "Console.h"

Console::~Console()
{
  /* QFile file( "last.log" );
  file.open( QFile::WriteOnly );
  file.write( pLog->toPlainText().toAscii() );
  file.close(); */
}

void Console::setup( StringsManager* _pSM, QMainWindow* _pWnd )
{
  pSM = _pSM; pWnd = _pWnd;
  setParent( _pWnd );
  QWidget* titleWidget = new QWidget(pWnd);
  setTitleBarWidget( titleWidget );
}

Console::Console( QWidget *parent ):
  QDockWidget( parent ),
  mode( LM::USUAL ),
  isSaved( false ),
  waitWnd( "Please, wait..." , parent ),
  prev_process( false ),
  fail( false )
{
  setWindowTitle( "DivGeo: Console" );
  //setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint );

  setFeatures( QDockWidget::DockWidgetMovable );
  setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

  dockWidgetContents = new ResizableWidget();
  dockWidgetContents->SetCustomHeight( 72 );
  dockWidgetContents->setMinimumHeight( 56 );
  dockWidgetContents->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  QVBoxLayout* pMainLayout = new QVBoxLayout( dockWidgetContents );
  pMainLayout->setMargin( 1 );
  pMainLayout->setSpacing( 1 );

  pLog = new QTextEdit();
  pLog->setReadOnly( true );
  pLog->setWordWrapMode( QTextOption::WordWrap );
  pLog->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pLog->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
  pMainLayout->addWidget( pLog );

  QHBoxLayout* pButtonsLayout = new QHBoxLayout();
  pButtonsLayout->setMargin( 1 );
  pButtonsLayout->setSpacing( 1 );
  pMainLayout->addLayout( pButtonsLayout );

  QPushButton* pBtnClear = new QPushButton( "Clear");
  pBtnClear->setFixedSize( 40, 24 );
  pButtonsLayout->addWidget( pBtnClear );
  connect( pBtnClear, SIGNAL(clicked()), this, SLOT(slotClear()) );

  QPushButton* pBtnSave = new QPushButton( "Save" );
  pBtnSave->setFixedSize( 40, 24 );
  pButtonsLayout->addWidget( pBtnSave );
  connect( pBtnSave, SIGNAL(clicked()), this, SLOT(slotSave()) );

  pCommand = new QLineEdit();
  pCommand->setFixedHeight( 24 );
  pCommand->setEnabled( false );
  pButtonsLayout->addWidget( pCommand );
  setWidget(dockWidgetContents);

  Send( MT::LOG, AL::INFO, "DivGeo started from", QApplication::applicationDirPath() );
  QFile file( QApplication::applicationDirPath() + "/" + "last.log" );
  file.open( QFile::WriteOnly );
  file.write( "" );
  file.close();
}

void Console::slotClear()
{
  if( !isSaved && QMessageBox::question( this, "DivGeo: Console - clear",
                         "All history data will be removed. Do you want to save it before?",
                         QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {
    slotSave();
  }

  pLog->clear();
}

void Console::slotSave()
{
  QString fileName = QFileDialog::getSaveFileName( this, "Save log", "", "Text log files (*.log)" );
  if( fileName.isEmpty() )
    return;

  QFile file( fileName );
  file.open( QFile::WriteOnly );
  file.write( pLog->toPlainText().toLatin1() );
  file.close();

  isSaved = true;

  Send( MT::LOG, AL::INFO, "Console", QString( "log saved to %1\n" ).arg( fileName ) );
}

void Console::Progress( ulong pct )
{
  waitWnd.progress( pct );
  qApp->processEvents();
}

ulong Console::Send( MessageType mt, AlarmLevel al, const QString& _sender, ulong msg_code, const QStringList& args )
{
  QString text = SM_MSG( msg_code );
  if( text.left(4) == "msg:" )
    text = SM_DLG( msg_code );
  if( msg_code == DG3::FAIL )
    fail = true;
  foreach( const QString& arg, args )
    text = text.arg( arg );

  Send( mt, al, _sender, text );
  return msg_code;
}

void Console::Send( MessageType mt, AlarmLevel al, const QString& _sender, const QString& _text )
{
  if( al == AL::RESULT && !waitWnd.isInProcess() )
    return;

  /* Interrupt process with error message */
  if( waitWnd.isInProcess() && (al == AL::ERROR || al == AL::FATAL ) ) {
    fail = true;
    Send( mt, AL::RESULT, "", SM_MSG( DG3::FAIL ) );
  }

  if( mode != LM::IGNORE || fail ||
      al == AL::RESULT || al == AL::PROCESS ||
      al == AL::WIP_INFO || al == AL::WIP_WARN ) {
    /* Prepare message text */
    QString text, record;
    QString sTime = QTime::currentTime().toString( "HH:mm:ss.zzz" );
    if( al == AL::RESULT ) {
      if( !prev_process ) {
        record = QString( "%1 - Finished: " ).arg( sTime );
        record.append( waitWnd.LastProcessText() );
      }
      record.append( QString( " [ %1 ]\n" ).arg( _text ) );
    }
    else {
      if( prev_process )
        record = '\n';
      text = (!_sender.isEmpty() ? QString( "%1: %2" ).arg( _sender ).arg( _text ) : _text);
      record.append( QString( "%1 - %2" ).arg( sTime ).arg( text ) );
      if( al != AL::PROCESS )
        record.append( '\n' );
    }
    prev_process = (al == AL::PROCESS);

    if( al == AL::FATAL )
      record += "\n\n!!! Fatal error !!!\n";

    /* Log */
    if( !_text.isEmpty() && mt != MT::DYNAMIC ) {
      /* Cursor position */
      QTextCursor cursor;
      if( mode == LM::COLLECT && !fail )
        cursor = QTextCursor( &collectedText );
      else
        cursor = pLog->textCursor();
      cursor.movePosition( QTextCursor::End );

      /* Text format */
      QTextCharFormat format;
      if( al == AL::FATAL ) format.setFontWeight( QFont::Bold );
      else                  format.setFontWeight( QFont::Normal );

      switch( al ) {
      case AL::ERROR:
      case AL::FATAL:     format.setForeground( Qt::red ); break;
      case AL::WARNING:
      case AL::WIP_WARN:  format.setForeground( Qt::darkRed ); break;
      case AL::RESULT:    format.setForeground( fail ? Qt::red : Qt::darkGreen ); break;
      default:            format.setForeground( Qt::black ); break;
      }

      /* Append document */
      cursor.insertText( record, format );
      if( mode != LM::COLLECT ) {
        pLog->ensureCursorVisible();
        isSaved = false;
      }
    } /* End Log */

    if( mode != LM::COLLECT && mt != MT::DYNAMIC ) {
      /* File */
      QFile file( QApplication::applicationDirPath() + "/" + "last.log" );
      file.open( QFile::Append );
      file.write( record.toLatin1() );
      file.close();

      /* Window */
      if( mt == MT::WINDOW || mt == MT::ALL || al == AL::RESULT ) {
        switch( al ) {
        case AL::INFO:
        case AL::WIP_INFO:  QMessageBox::information( pWnd, "Information", text ); break;
        case AL::WARNING:
        case AL::WIP_WARN:  QMessageBox::warning( pWnd, "Warning!", text ); break;
        case AL::ERROR:     QMessageBox::critical( pWnd, "Error!", text ); break;
        case AL::FATAL:     QMessageBox::critical( pWnd, "Fatal error!", text ); break;
        case AL::PROCESS: {
          waitWnd.start( _text );
          qApp->processEvents();
          mode = LM::IGNORE;
          break;
        }
        case AL::RESULT:
          waitWnd.finish();
          qApp->processEvents();
          if( !waitWnd.isInProcess() )
            mode = LM::USUAL;
          return;
        } /* End AL switch */
      } /* End window */
    } /* End not COLLECT mode */
  } /* End not IGNORE mode */

  /* Status bar */
  if( mt == MT::STATUS || mt == MT::DYNAMIC || mt == MT::ALL )
    pWnd->statusBar()->showMessage( _text );

  fail = false;

  /* Crash */
  if( al == AL::FATAL )
    exit( -1 );
}

void Console::OutputCollection()
{
  QTextCursor cursor( &collectedText );
  cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
  pLog->textCursor().insertFragment( cursor.selection() );
  pLog->ensureCursorVisible();
  isSaved = false;
}

void Console::ClearCollection()
{
  collectedText.clear();
}
