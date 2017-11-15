#include "MainWnd.h"

// Options
void CMainWnd::slotProjectSettings()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::PROJECT_SETTINGS ) );
  ProjectSettings projSetts( pMV->CurrentModel() );
  DlgSettings* pDialog = new DlgSettings( pMV->CurrentModel(), pSM, &projSetts,
                                          SM_DLG( DLG::PRJSETTINGS::TITLE ), this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
  UpdateModelInfo( true );
  pMV->CurrentView()->slotUpdateView();//1408
}

/*void CMainWnd::slotOutputMode()
{
  ModelPtr pModel = pMV->CurrentModel();
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::OUTPUT_MODE ) );
  DlgOutputMode* pDialog = new DlgOutputMode( pModel, pSM, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
  pMV->CurrentProxy()->ChangeOutputModeForDialogs( pModel->OutputMode() );
  UpdateModelInfo( true );
}
void CMainWnd::slotMeshEditing()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::MESH_EDITING ) );
  ModelPtr pModel = pMV->CurrentModel();
  DlgMeshOptions* pDialog = new DlgMeshOptions( pModel->GetMesh(), pModel, pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
  UpdateModelInfo( true );
}*/

//void CMainWnd::slotDisplayMenu() { }
//void CMainWnd::slotDisplayToolbar() { }
//void CMainWnd::slotManualRefresh() { }
//void CMainWnd::slotSaveInterval() { }

void CMainWnd::slotSettings()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::SETTINGS ) );
  DlgSettings* pDialog = new DlgSettings( pMV->CurrentModel(), pSM, &userPrefs,
                                          SM_DLG( DLG::SETTINGS::TITLE ), this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  if( pDialog->exec() == QDialog::Accepted )
    pMV->UpdateAllViewsStyle();
  if( pMV->CurrentView() != null )
    pMV->CurrentView()->slotUpdateView();//1408
}

// Options > Setup
void CMainWnd::slotSetupVariables()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::SETUP::VARIABLES ) );
  if( QMessageBox::warning( this,
    SM_DLG( DLG::SETUPWARNING::TITLE ), SM_DLG( DLG::SETUPWARNING::MESSAGE ),
    SM_DLG( DLG::BTN::PROCEED ), SM_DLG( DLG::BTN::CANCEL ) ) == 1 )
    return;

  DlgVsdList* pDialog = new DlgVsdList( pMV->CurrentModel(), pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
  UpdateVarsMenu();//1411
}

void CMainWnd::slotSetupSave()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::OPTIONS::SETUP::SAVE ) );
  //TODO:
}
