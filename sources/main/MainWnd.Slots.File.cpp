#include "MainWnd.h"
#include <QMessageBox>

const char* CMainWnd::scpDefaultDgcFileName = "config.dgc";

void CMainWnd::slotNew()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::NEW ) );

  QString sPath;
  QDir dir( QApplication::applicationDirPath() );
  QFile file( dir.absolutePath() + "/" + scpDefaultDgcFileName );
  if( !file.exists() ) {
    dir.cdUp();
    file.setFileName( dir.absolutePath() + "/" + scpDefaultDgcFileName );
  }
  if( file.exists() )
    sPath = file.fileName();

  DlgNewModel* pDialog = new DlgNewModel( pSM, pConsole, sPath, this );
  if( pDialog->exec() == QDialog::Accepted ) {
    sPath = pDialog->Path();
    pMV->NewModel( sPath );
    UpdateModelInfo();
    pMV->CurrentModel()->Agent()->Clear();
  }
}

void CMainWnd::slotOpen( QString _crsFilePath )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::OPEN ) );

  ModelViewProxyPtr pProxy = pMV->OpenModel( _crsFilePath );
  if( pProxy != null ) {
    ModelPtr pModel = pProxy->GetModel();
    CViewWndPtr pView = pProxy->CurrentView();
    /* setup show options */
    ulong showFlags = pModel->ShowFlags();
    for( QMap< ShowFlag, QAction* >::iterator it = mapViewOptions.begin(),
                                           itEnd = mapViewOptions.end();
         it != itEnd; it++ ) {
      bool isChecked = HasAnyFlag( showFlags, it.key() );
      it.value()->setChecked( isChecked );
      pView->UpdateShowFlag( it.key(), isChecked );
    }
    pView->FitToModel();
    AddToRecentList( ToQString( pModel->FileName() ) );
  }

  UpdateModelInfo();
}

QString CMainWnd::SelectModelFile( ModelPtr _pModel ) const
{
  const QString& sTitle = SM_DLG( DLG::FILESAVE::TITLE );
  const QString& sFilter = SM_DLG( DLG::FILESAVE::FILTER )
                           .arg( userPrefs.common.sExtFilterModel ) //1409
                           .arg( userPrefs.common.sExtFilterTopology ); //1409
  QString sPath = lastPath.sPathProject.isEmpty() ? QDir::currentPath() : lastPath.sPathProject;

  QString sFileName;
  forever {
    sFileName = QFileDialog::getSaveFileName( (QWidget*)this, sTitle, sPath, sFilter, null,
                                              QFileDialog::DontConfirmOverwrite );
    if( sFileName.isEmpty() )
      return "";

    QFileInfo fileInfo( sFileName );
    lastPath.sPathProject = fileInfo.absolutePath();

    if( fileInfo.suffix().isEmpty() )
      sFileName += ".dg";
    /*else if( fileInfo.suffix() != "dg" ) {
      pConsole->Send( WND_ERROR, sender_name, ERR::WRONGTYPE );
      return "";
    }        MB: v3.0.1406 */

    if( DetectFileType( sFileName.toStdString() ) == FT_DG_CONFIG ) {
      pConsole->Send( WND_ERROR, sender_name, ERR::NOCONFIG );
      return "";
    }

    if( QFileInfo( sFileName ).exists() ) {
      if( pMV->ModelIsLoaded( sFileName ) ) {
        if( QMessageBox::information( (QWidget*)this,
            SM_DLG( DLG::MODEL_OPEN::TITLE ), SM_DLG( DLG::MODEL_OPEN::MESSAGE ),
            QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel )
          return "";
        sFileName = pMV->SelectAnotherPath( sFileName );
      }

      QString sDialogText = SM_DLG( DLG::FILE_OVERWRITE::TITLE )
                            .arg( ToQString( _pModel->Name() ) );
      int retcode = QMessageBox::information(
                      (QWidget*)this, QString( "DivGeo" ), sDialogText,
                      SM_DLG( DLG::FILE_OVERWRITE::OVERWRITE ),
                      SM_DLG( DLG::FILE_OVERWRITE::SELECTANOTHER ),
                      SM_DLG( DLG::BTN::CANCEL ) );

      switch( retcode ) {
      case 1: continue;
      case 2: return "";
      }
    }

    break;
  }

  return sFileName;
}


void CMainWnd::slotReload()
{
  //TODO:
}

void CMainWnd::slotSave()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::SAVE ) );

  pMV->CurrentProxy()->SaveModel();
  QString sFilePath = QString::fromStdString( pMV->CurrentModel()->FileName() );
  if( !sFilePath.isEmpty() )
    AddToRecentList( sFilePath );
}

void CMainWnd::slotSaveAs()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::SAVEAS ) );

  QString sFileName = SelectModelFile( pMV->CurrentProxy()->GetModel() );
  if( not sFileName.isEmpty() ) //1409
    pMV->CurrentProxy()->SaveModelAs( sFileName );
}

void CMainWnd::slotSaveCopy()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::SAVECOPY ) );

  QString sFileName = SelectModelFile( pMV->CurrentProxy()->GetModel() );
  if( not sFileName.isEmpty() ) //1409
    pMV->CurrentProxy()->SaveModelAs( sFileName, true );
}

void CMainWnd::slotImportTemplate()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::IMPORT::TEMPLATE ) );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr pModel = pProxy->GetModel();

  if( pModel->HasTemplate() && pModel->GetTemplate()->IsLocked() ) {
    pModel->ShowLockReasonOf( pModel->GetTemplate() );
    return;
  }

  const QString& title = SM_DLG( DLG::TEMPLATE::TITLE );
  const QString& filter = SM_DLG( DLG::TEMPLATE::FILTER ).arg( userPrefs.common.sExtFilterTemplate );
  QString sPath = lastPath.sPathTemplate.isEmpty() ? QDir::currentPath() : lastPath.sPathTemplate;//1408

  QString fileName = QFileDialog::getOpenFileName( this, title, sPath, filter );
  if( fileName.isEmpty() || !QFile::exists( fileName ) )
    return;
  lastPath.sPathTemplate = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != "ogr" )
    return;      MB: v3.0.1406 */

  ulong result = (ulong)pModel->AddTemplate(
                   fileName.toStdString(), Point(), 0, 1 );
  if( result != 0 ) {
    pConsole->Send( WND_ERROR, sender_name, result,
                    QStringList( QFileInfo( fileName ).completeBaseName() ) );
    pModel->ActionStack().Cancel();
  }
  else {
    pProxy->UpdateScenes( SHW::TEMPLATE );
    slotChangeShowFlag( SHW::TEMPLATE, true );
    pConsole->Send( LOG_INFO, sender_name, DGMSG::TEMPLATELOADED );
    pModel->ActionStack().Complete( "ImportTemplate" );
  }
}

void CMainWnd::slotImportEquilibrium()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::IMPORT::EQUILIBRIUM ) );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr pModel = pProxy->GetModel();
  EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();

  if( pEquil != null && pEquil->IsLocked() ) {
    pModel->ShowLockReasonOf( pEquil );
    return;
  }

  const QString& title = SM_DLG( DLG::EQUIL::TITLE );
  const QString& filter = SM_DLG( DLG::EQUIL::FILTER ).arg( userPrefs.common.sExtFilterEquil );
  QString sPath = lastPath.sPathEquil.isEmpty() ? QDir::currentPath() : lastPath.sPathEquil;//1408

  QString fileName = QFileDialog::getOpenFileName( this, title, sPath, filter );
  if( fileName.isEmpty() || !QFile::exists( fileName ) )
    return;

  lastPath.sPathEquil = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != "equ" )
    return;   MB: v3.0.1406 */

  ulong result = (ulong)pModel->GetFluxModel()->AddEquil( fileName.toStdString().c_str() );

  if( result != 0 ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, sender_name, result,
                    QStringList( QFileInfo( fileName ).completeBaseName() ) );
  }
  else {
    pProxy->UpdateScenes( SHW::EQUIL );
    slotChangeShowFlag( SHW::EQUIL, true );
    pConsole->Send( LOG_INFO, sender_name, DGMSG::EQUILLOADED );
    pModel->ActionStack().Complete( "ImportEquilibrium" );
  }

  bool equilExists = pModel->GetFluxModel()->HasEquil();
  mapMenuActions.value( MENU::FILES::IMPORT::TOPOLOGY )->setEnabled( equilExists );
  mapMenuActions.value( MENU::EDIT::CREATE::GRIDPOINTS )->setEnabled( equilExists );
  mapMenuActions.value( MENU::EDIT::CREATE::SURFACE )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::EDIT_TOPOLOGY )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTS )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTSEGS )->setEnabled( equilExists );
}



//void CMainWnd::slotImportSonnetGrid() { }
void CMainWnd::slotImportMesh()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::IMPORT::MESH ) );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr pModel = pProxy->GetModel();

  if( pModel->HasMesh() && pModel->GetMesh()->IsLocked() ) {
    pModel->ShowLockReasonOf( pModel->GetMesh() );
    return;
  }

  const QString& title = SM_DLG( DLG::SONNET::TITLE );
  const QString& filter = SM_DLG( DLG::SONNET::FILTER ).arg( userPrefs.common.sExtFilterMesh );
  QString sPath = lastPath.sPathMesh.isEmpty() ? QDir::currentPath() : lastPath.sPathMesh;//1408

  QString fileName = QFileDialog::getOpenFileName( this, title, sPath, filter );
  if( fileName.isEmpty() || !QFile::exists( fileName ) )
    return;

  lastPath.sPathMesh = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != "sno" )
    return;      MB: v3.0.1406 */

  ulong result = (ulong)pModel->AddMesh( fileName.toStdString().c_str() );
  if( result != 0 ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, sender_name, result,
                    QStringList( QFileInfo( fileName ).completeBaseName() ) );
  }
  else {
    pProxy->UpdateScenes( SHW::MESH | SHW::MESHDETAILS );
    slotChangeShowFlag( SHW::MESH, true );
    pConsole->Send( LOG_INFO, sender_name, DGMSG::SONNETLOADED );
    pModel->ActionStack().Complete( "ImportMesh" );
  }

  mapMenuActions.value( MENU::VIEW::MESH_HEADER )->setEnabled( pModel->HasMesh() );
  //mapMenuActions.value( MENU::OPTIONS::MESH_EDITING )->setEnabled( pModel->HasMesh() );

  UpdateModelInfo();
}



void CMainWnd::slotImportTopology()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::IMPORT::TOPOLOGY ) );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr pModel = pProxy->GetModel();

  if( !pModel->GetFluxModel()->HasEquil() ) {
    pConsole->Send( WND_ERROR, sender_name, ERR::NOEQUIL );
    return;
  }

  QString sPath = lastPath.sPathTopology.isEmpty() ? QDir::currentPath() : lastPath.sPathTopology;//1408
  DlgImportTopology* pDialog = new DlgImportTopology( pSM, userPrefs.common.sExtFilterModel, userPrefs.common.sExtFilterTopology, sPath, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  QString fileName;

  if( pDialog->exec() == QDialog::Accepted )
    fileName = pDialog->selectedFiles().first();
  else
    return;
  if( fileName.isEmpty() || !QFile::exists( fileName ) )
    return;

  lastPath.sPathTopology = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != "dg" )
    return;         MB: v3.0.1406: SPV ask to make possible loading files without ext */

  FluxModelPtr pFlux = pProxy->GetModel()->GetFluxModel();
  ulong result = (ulong)pFlux->AddTopology( fileName.toStdString(), pDialog->DetectXPoint() );

  if( result != 0 ) {
    pModel->ActionStack().Cancel();
    /*QStringList errMsgList = GetLoadErrFlagsDescription( errFlags );
    if( errMsgList.empty() && result == DGMSG::OK )
      pConsole->Send( LOG_INFO, sender_name, DG3::TOPOLOGY_LD_SUCCESSFULLY );
    else if( !errMsgList.empty() ) {
      static const char* scList = " > ";
      QString errText = SM_MSG( DG3::TOPOLOGY_LD_WITH_ERRORS );
      errText.append( "\n" );
      foreach( const QString& str, errMsgList ) {
        errText.append( scList );
        errText.append( str );
        errText.append( "\n" );
      }
      errText.chop( 1 );
      pConsole->Send( WND_ERROR, sender_name, errText );
    }*/
  }
  else {
    pProxy->UpdateScenes( SHW::GRIDPOINTS | SHW::XPOINT | SHW::XPOINTTESTS | SHW::SURFACES );
    slotChangeShowFlag( SHW::GRIDPOINTS, true );
    slotChangeShowFlag( SHW::XPOINTTESTS, true );
    slotChangeShowFlag( SHW::SURFACES, true );

    pModel->Agent()->Update();

    // Fix segments length cut by targets.
    pFlux->GetTopology()->RecalcAllGridPointSegSLines();

    // Dark legacy of dg2.
    // Complete() calls some topology processing with surfaces and GP rebuild
    //TODO: Make explicit call of all calculation methods.
    pModel->ActionStack().Complete( "ImportTopology" );

    pConsole->Send( LOG_INFO, sender_name, SM_DLG( DLG::IMPORTTOPO::MESSAGE ) );
  }

  pFlux->GetTopology()->RecalcCache();
}



void CMainWnd::slotExportMesh()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::EXPORT::MESH ) );

  ModelPtr pModel = pMV->CurrentModel();
  if( !pModel->HasMesh() ) {
    pConsole->Send( WND_ERROR, sender_name, ERR::NOSONNET );
    return;
  }

  QString sIrregularCells = QString::fromStdString(
                             pModel->GetMesh()->GetIrregularCellsList() );
  if( !sIrregularCells.isEmpty() ) {
    QString sText = "Some mesh cells are irregular: \n" +
                    sIrregularCells + "\n\n"
                    "Continue?";
    if( QMessageBox::question(
          this, "Irregular cells", sText,
          QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel ) {
      pConsole->Send( LOG_WARNING, sender_name, "Saving mesh aborted by user" );
      return;
    }
  }

  const QString& title = SM_DLG( DLG::EXPORT_MESH::TITLE );
  const QString& filter = SM_DLG( DLG::EXPORT_MESH::FILTER ).arg( userPrefs.common.sExtFilterMesh );
  QString sPath = lastPath.sPathMesh.isEmpty() ? QDir::currentPath() : lastPath.sPathMesh;//1408
  //const char* csSuffix = "sno";

  QFileDialog* pDialog = new QFileDialog( this, title, sPath, filter );
  //pDialog->setDefaultSuffix( csSuffix );
  if( pDialog->exec() != QDialog::Accepted )
    return;
  QStringList fileNames = pDialog->selectedFiles();
  if( fileNames.isEmpty() )
    return;
  QString fileName = fileNames.first();
  if( fileName.isEmpty() )
    return;

  lastPath.sPathMesh = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != csSuffix )
    return;     MB: v3.0.1406 */

  ulong result = (ulong)pModel->GetMesh()->SaveMesh( fileName.toStdString() );
  if( result != 0 ) {
    pModel->ActionStack().Cancel();
  }
  else {
    pConsole->Send( LOG_INFO, sender_name, DLG::EXPORT_MESH::MESSAGE );
    pModel->ActionStack().Complete( "ExportMesh" );
  }

  UpdateModelInfo();
}



void CMainWnd::slotExportElements()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::EXPORT::ELEMENT ) );

  ModelPtr pModel = pMV->CurrentModel();
  QString sPath = lastPath.sPathTemplate.isEmpty() ? QDir::currentPath() : lastPath.sPathTemplate;//1408
  DlgExportElements* pDialog = new DlgExportElements( pSM, userPrefs.common.sExtFilterTemplate, sPath, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  QString fileName;
  if( pDialog->exec() == QDialog::Accepted )
    fileName = pDialog->selectedFiles().first();
  else
    return;
  if( fileName.isEmpty() )
    return;

  lastPath.sPathTemplate = QFileInfo( fileName ).absolutePath();//1408

  /*if( QFileInfo( fileName ).suffix() != "ogr" )
    return;      MB: v3.0.1406 */

  ulong result = (ulong)pModel->WriteElemsAsDgTemplate( fileName.toStdString(), pDialog->Marked() );
  if( result != 0 ) {
    pModel->ActionStack().Cancel();
  }
  else {
    pConsole->Send( LOG_INFO, sender_name, DLG::EXPORT_ELEMS_TEMPLATE::MESSAGE );
    pModel->ActionStack().Complete( "ExportElements" );
  }

  UpdateModelInfo();
}



void CMainWnd::slotOutput()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::OUTPUT ) );

  ModelPtr pModel = pMV->CurrentModel();
  bool result_valid  = pModel->ValidateOutputData();
  int result_output, result_struct, result_target;
  if( result_valid ) {
    if( pModel->GetFluxModel()->HasEquil() ) {
      /* MB: Extracted from Equil::GetSurfaceZoneByXY() const
             used by Model::WriteTargetsFile(...)
                     Equil::WriteTargetsFile(...)
                     Equil::Carre_CheckSurfaces(...) */
      pModel->GetFluxModel()->GetTopology()->RecalcCache();
    }

    result_output = pModel->WriteOutputFile( "" );
    result_struct = pModel->WriteStructureFile( "" );
    result_target = pModel->WriteTargetsFile( "" );
  }
  else
    return;

  if( result_output != 0 && result_struct != 0 && result_target != 0 )
    return;

  if( pModel->FileName().empty() ) {
    pConsole->Send( WND_ERROR, sender_name, ERR::NOFNAME );
    return;
  }

  QFileInfo fi( QString::fromStdString( pModel->FileName() ) );
  QString sName = fi.completeBaseName();
  QString sPath = fi.absolutePath();
  sName = sPath + "/" + sName;
  QString fname_output = sName + SM_MSG( FSTR::OUTPUTEXT );
  QString fname_struct = sName + SM_MSG( FSTR::STRUCTUREEXT );
  QString fname_target = sName + SM_MSG( FSTR::TARGETSEXT );
  QString sText = QString( "Output mode: %1\n\n"
                  "Output files will be created at %2/\n\n" )
                  .arg( pModel->CarreOutputMode() ? "Carre" : "Sonnet" )
                  .arg( fi.absolutePath() );
  if( result_output == 0 )
    sText += SM_MSG( STR::OUTPUTFNAME ) + ": " + fname_output + "\n";
  if( result_struct == 0 )
    sText += SM_MSG( STR::STRUCTUREFNAME ) + ": " + fname_struct + "\n";
  if( result_target == 0 )
    sText += SM_MSG( STR::TARGETSFNAME ) + ": " + fname_target + "\n";

  sText += "\nContinue?";

  if( QMessageBox::question(
        this, "Output files", sText,
        QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel ) {
    pConsole->Send( LOG_WARNING, sender_name, "Data output aborted by user" );
    return;
  }

  if( result_output == 0 )
    result_output = pModel->WriteOutputFile( fname_output.toStdString() );
  if( result_struct == 0 )
    result_struct = pModel->WriteStructureFile( fname_struct.toStdString() );
  if( result_target == 0 )
    result_target = pModel->WriteTargetsFile( fname_target.toStdString() );

  if( result_output == 0 || result_struct == 0 || result_target == 0 )
    pConsole->Send( LOG_INFO, sender_name, DGMSG::OUTPUT );
}



void CMainWnd::slotPrint()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::PRINT ) );
}



void CMainWnd::slotExit()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::FILE::EXIT ) );
  this->close();
}
