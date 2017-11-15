#include "MainWnd.h"

//#define SET_DISPLAY_STATE(k) { QAction *pSender = (QAction*)sender(); SetDisplayState( k, pSender->isChecked() ); }

// Window
void CMainWnd::slotNewView()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::WINDOW::NEW_VIEW ) );
  pMV->CurrentProxy()->CreateView();
}

void CMainWnd::slotClose()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::WINDOW::CLOSE ) );
  pMV->CurrentView()->close();
  CViewWndPtr pView = pMV->CurrentView();
  if( pView == null ) {
    slotActiveViewChanged( null );
    return;
  }
  foreach( QMdiSubWindow* pSubWnd, pMdiArea->subWindowList() ) {
    CViewWnd* pSubView = qobject_cast< CViewWnd* >( pSubWnd->widget() );
    if( pSubView == pView )
      pMdiArea->setActiveSubWindow( pSubWnd );
  }
}

void CMainWnd::slotStatistics()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::WINDOW::STATISTICS ) );
  pMV->CurrentProxy()->OpenStatisticsDlg();//1408
}
//void CMainWnd::slotToolbox() { }

// Help
void CMainWnd::slotWhatsNew()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::HELP::WHATS_NEW ) );
  pAssistant->slotShowAt( MENU::HELP::WHATS_NEW, "menu" );
}

void CMainWnd::slotManual()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::HELP::MANUAL ) );
  pAssistant->slotShowAt( MENU::HELP::MANUAL, "menu" );
}

void CMainWnd::slotUsingHelp()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::HELP::USING_HELP ) );
  pAssistant->slotShowAt( DLG::DGHELP::DIALOG, "dlg" );
}

void CMainWnd::slotOnMainWindow()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::HELP::ON_MAIN_WINDOW ) );
  pAssistant->slotShowAt( HELP::DG2_HELP::MAIN_WINDOW, "main" );
}

void CMainWnd::slotAbout()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::HELP::ABOUT ) );
  DlgAbout* pDialog = new DlgAbout( pSM, version, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
}
