#include "MainWnd.h"

// Commands
// Commands > Convert
void CMainWnd::slotConvertTemplate2Elements()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::CONVERT::TEMPLATE_TO_ELEMENTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  if( !pModel->HasTemplate() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NOTEMPLATE );
    return;
  }

  pModel->GetTemplate()->ConvertToElements( pModel->Struct() );
  slotChangeShowFlag( SHW::ELEMS, true );
  slotChangeShowFlag( SHW::IRRNODES, true );
  slotChangeShowFlag( SHW::NORMALS, true );
  pModel->ActionStack().Complete( "ConvertTemplate2Elements" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::TEMPLAPPENDED );
}

void CMainWnd::slotConvertElements2Chords()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::CONVERT::ELEMENTS_TO_CHORDS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  IComponentList marked = pModel->MarkedObjects();
  RestrictToType( marked, OT::ELEMENT );
  if( marked.empty() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NO_MARKED_ELEMS );
    return;
  }
  IComponentPtr pElem_err = null;
  int r = pModel->Struct()->ConvertElemsToChords( marked, &pElem_err );
  if( r != 0 ) {
    pModel->ActionStack().Cancel();
    if( pElem_err != null )
      pMV->CurrentView()->CurrentScene()->CreateLabelItem( pElem_err, SM_MSG( STR::ERRLABEL ) );
    if( r == ERR::LOCKED )
      pModel->ShowLockReasonOf( pElem_err );
    else
      pConsole->Send( WND_ERROR, FULL_SENDER, r );
  }
  else
    pModel->ActionStack().Complete( "ConvertElements2Chords" );
}

void CMainWnd::slotConvertChords2Elements()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::CONVERT::CHORDS_TO_ELEMENTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  IComponentList marked = pModel->MarkedObjects();
  RestrictToType( marked, OT::CHORD );
  if( marked.empty() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NO_MARKED_CHORDS );
    return;
  }
  IComponentPtr pCh_err = null;
  int r = pModel->Struct()->ConvertChordsToElems( marked, &pCh_err );
  if( r != 0 ) {
    pModel->ActionStack().Cancel();
    if( pCh_err != null )
      pMV->CurrentView()->CurrentScene()->CreateLabelItem( pCh_err, SM_MSG( STR::ERRLABEL ) );
    if( r == ERR::LOCKED )
      pModel->ShowLockReasonOf( pCh_err );
    else
      pConsole->Send( WND_ERROR, FULL_SENDER, r );
  }
  else
    pModel->ActionStack().Complete( "ConvertChords2Elements" );
}

// Commands > Symplify
void CMainWnd::slotSimplifyFillGaps()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::SIMPLIFY::FILL_GAPS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  DlgGluePoints* pDialog = new DlgGluePoints( pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  if( pDialog->exec() == QDialog::Rejected )
    return;

  int count = 0;
  pModel->Struct()->GlueNodes( pDialog->Dist(), pDialog->MarkedOnly(), &count );
  pModel->ActionStack().Complete( "SimplifyFillGaps" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DLG::GLUEPOINTS::MSG::ELEMS_ADDED, Arg( count ) ) ;
}
void CMainWnd::slotSimplifyLineUpAllNormals()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::ALL ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  int count = 0;
  pModel->Struct()->GlueNormals( false /*difference*/, &count );
  pModel->ActionStack().Complete( "SimplifyLineUpAllNormals" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DLG::MSG::NORMALS_GROUPED, Arg( count ) ) ;
}
void CMainWnd::slotSimplifyLineUpMarkedNormals()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::MARKED ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  int count = 0;
  pModel->Struct()->GlueNormals( true /*difference*/, &count );
  pModel->ActionStack().Complete( "SimplifyLineUpMarkedNormals" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DLG::MSG::NORMALS_GROUPED, Arg( count ) ) ;
}
void CMainWnd::slotSimplifyMergeSplitElements()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::SIMPLIFY::MERGE_SPLIT_ELEMENTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();

  DlgGlueElems* pDialog = new DlgGlueElems( pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  if( pDialog->exec() == QDialog::Rejected )
    return;

  int count = 0;
  pModel->Struct()->GlueElems( pDialog->MaxDev(), pDialog->LenLimit(),
                                       pDialog->SplitLonger(), pDialog->MarkedOnly(), &count );
  pModel->ActionStack().Complete( "SimplifyMergeSplitElements" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, SM_DLG( DLG::GLUEELEMS::MSG::ELEMS_REMOVED ).arg( count ) ) ;
}

// Commands
void CMainWnd::slotRenumberElements()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::RENUMBER_ELEMENTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  pModel->Struct()->RenumberElements();
  pModel->ActionStack().Complete( "RenumberElements" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DG3::ELEMENTS_RENUMBERED ) ;
}

void CMainWnd::slotCheckVariables()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::CHECK_VARIABLES ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();

  VarPairList errPairs;
  if( pProxy->GetModel()->Vars()->CheckAllVars( &errPairs ) == 0 )
    pConsole->Send( MT::WINDOW, AL::INFO, FULL_SENDER, DGMSG::ALLVARSVALID );
  else {
    DlgInvalidVars* pDialog = new DlgInvalidVars( pProxy->GetModel(),
                                  pProxy->CurrentView(), pSM, pConsole, this );
    connect( pDialog, SIGNAL(CreateVarsEditDlg(VarSetPtr)),
             (QObject*)pProxy, SLOT(slotCreateVarsEditDlg(VarSetPtr)) );
    connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
    pDialog->show();
  }
}

void CMainWnd::slotRebuildCarreObjects()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::COMMANDS::REBUILD_CARRE_OBJECTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  FluxModelPtr pFlux = pModel->GetFluxModel();

  int r = pFlux->RebuildCarreSurfaces( -1 );
  if( r != 0 ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, FULL_SENDER, r );
    return;
  }

  r = pFlux->RebuildCarreGridPoints( -1 );
  if( r != 0 ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, FULL_SENDER, r );
    return;
  }

  pModel->ActionStack().Complete( "RebuildCarreObjects" );
}

void CMainWnd::slotFindXPoints()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::COMMANDS::FIND_XPOINTS ) );

  ModelPtr pModel = pMV->CurrentModel();
  FluxModelPtr pFlux = pModel->GetFluxModel();

  pFlux->GetTopology()->DetectXPoints();
  pModel->ActionStack().Complete( "FindXPoints" );
}

void CMainWnd::slotFindXPointSegs()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::COMMANDS::FIND_XPOINTSEGS ) );

  ModelPtr pModel = pMV->CurrentModel();
  FluxModelPtr pFlux = pModel->GetFluxModel();

  pFlux->GetTopology()->DetectXPointSegs();
  pModel->ActionStack().Complete( "FindXPointSegs" );
}

void CMainWnd::slotEditTopology()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::COMMANDS::EDIT_TOPOLOGY ) );
  pMV->CurrentProxy()->OpenTopologyDialog();
}

void CMainWnd::slotUpdateTopology()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::COMMANDS::UPDATE_TOPOLOGY ) );
  pMV->CurrentModel()->GetFluxModel()->UpdateAfterTopologyChange();
  pMV->CurrentModel()->ActionStack().Complete( "UpdateTopology" );
}
