#include "MainWnd.h"
#include "Console.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QStringList args = QCoreApplication::arguments();

  Console* pC = new Console();

  StringsManager sm( pC );

  HelpManager hm( pC );

  CMainWnd mw( args, pC, &sm, &hm );
  mw.show();
  /* mw will delete the console in dtor */

  /* To fix problem of decimal comma */
  setlocale(LC_ALL, "C");
#ifdef Q_OS_WIN
  /* Use POSIX's format 0.0E-00 (not MS's 0.0E-000 -- this is C++ Standard violation). Build with -posix option */
  // _putenv( "PRINTF_EXPONENT_DIGITS=2" ); -- not works for now
  _set_output_format( _TWO_DIGIT_EXPONENT ); //1409
#endif

  return a.exec();
}
