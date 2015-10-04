#include "MainWnd.h"

void CMainWnd::slotEnableUndoStack( bool enabled )
{
  mapMenuActions.value( MENU::VIEW::PREVIOUS_VIEW )->setEnabled( enabled );
}

// View
void CMainWnd::slotRefresh()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::REFRESH ) );
  pMV->CurrentView()->viewport()->update();
}

void CMainWnd::slotZoomIn()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ZOOM_IN ) );
  pMV->CurrentView()->Scale( 2 );
}

void CMainWnd::slotZoomOut()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ZOOM_OUT ) );
  pMV->CurrentView()->Scale( 0.5 );
}

void CMainWnd::slotPictureView()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::PICTURE_VIEW ) );
  pMV->CurrentView()->FitToModel();
}

void CMainWnd::slotSelectionView()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::SELECTION_VIEW ) );
  pMV->CurrentView()->FitToSelected();
}

// View > Rotate
void CMainWnd::slotRotateIncreaseAngle()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ROTATE::INCREASE_ANGLE ) );
  pMV->CurrentView()->RotateCW();
}

void CMainWnd::slotRotateDecreaseAngle()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ROTATE::DECREASE_ANGLE ) );
  pMV->CurrentView()->RotateCCW();
}
void CMainWnd::slotRotateSetAngle()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ROTATE::SET_ANGLE ) );
  CViewWndPtr pView = pMV->CurrentView();
  DlgSetAngle* pDialog = new DlgSetAngle( pView->GetAngleRad(), pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  if( pDialog->exec() == QDialog::Rejected )
    return;
  pView->SetAngle( pDialog->AngleRad() );
}
void CMainWnd::slotRotateRectify()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::ROTATE::RECTIFY ) );
  pMV->CurrentView()->SetAngle( 0. );
}
// View > Stretch/Shrink
void CMainWnd::slotStretchHorizontal()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::STRETCH_HORIZONTAL ) );
  pMV->CurrentView()->Scale( Point( 2., 1. ) );
}

void CMainWnd::slotStretchVertical()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::STRETCH_VERTICAL ) );
  pMV->CurrentView()->Scale( Point( 1., 2. ) );
}

void CMainWnd::slotShrinkHorizontal()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::SHRINK_HORIZONTAL ) );
  pMV->CurrentView()->Scale( Point( 0.5, 1. ) );
}

void CMainWnd::slotShrinkVertical()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::SHRINK_VERTICAL ) );
  pMV->CurrentView()->Scale( Point( 1., 0.5 ) );
}

void CMainWnd::slotStretch()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::STRETCH ) );
  CViewWndPtr pView = pMV->CurrentView();
  DlgStretch* pDialog = new DlgStretch( pView->GetScale(),
                     SViewState::min_scale, SViewState::max_scale,
                     pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  if( pDialog->exec() == QDialog::Rejected )
    return;
  pView->SetScale( pDialog->Scale() );
}

void CMainWnd::slotJustify()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::STR_SHR::JUSTIFY ) );
  pMV->CurrentView()->ResetAspectRatio( true );
}

// View
void CMainWnd::slotPreviousView()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::PREVIOUS_VIEW ) );
  pMV->CurrentView()->SetPreviousViewState();
}

void CMainWnd::slotRemoveLabels()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::REMOVE_LABELS ) );
}

// View > Mode
void CMainWnd::slotModeStretch( bool enable )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::MODE::STRETCH ) );
  pMV->CurrentView()->SetStretchMode( enable );
  mapMenus.value( MENU::VIEW::STR_SHR::TITLE )->setEnabled( enable );
  mapTools.value( TOOLS::STRETCH | Qt::LeftButton ).first->setEnabled( enable );
  mapTools.value( TOOLS::STRETCH | Qt::RightButton ).first->setEnabled( enable );
  mapTools.value( TOOLS::STRETCH | Qt::MidButton ).first->setEnabled( enable );
}

void CMainWnd::slotModeColoredEquil( bool enable )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::MODE::COLORED_EQUIL ) );
  CViewWndPtr pView = pMV->CurrentView();
  pView->SetColoredEquilMode( enable );
}

void CMainWnd::slotModeRadialGrid( bool enable )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::MODE::RADIAL_GRID ) );
  CViewWndPtr pView = pMV->CurrentView();
  pView->SetRadialGridMode( enable );
}

void CMainWnd::slotModeTopDownView( bool enable )
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::MODE::TOPDOWN_VIEW ) );
  CViewWndPtr pView = pMV->CurrentView();

  bool fullToolSet = true;

  if( enable ) {
    pView->SelectCurrentScene( ST::TOP );
    fullToolSet = false;
  }
  else {
    pView->SelectCurrentScene( ST::SECTION );
    fullToolSet = true;
  }

  for( ulong btn = 1; btn <= 4; btn *= 2 ) {
    mapTools.value( btn | TOOLS::ADDELEMENT     ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::ADDGRIDPOINT   ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::ADDSOURCES     ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::ADDSURFACE     ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::MOVEMESHPOINT  ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::SPLITELEMENT   ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::CONNECTPOINTS  ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::JOINELEMENTS   ).first->setEnabled( fullToolSet );
    mapTools.value( btn | TOOLS::REPOSITION     ).first->setEnabled( fullToolSet );
  }

  pToolBar_Tools->EnableButton( TOOLS::ADDELEMENT,    fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::ADDGRIDPOINT,  fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::ADDSOURCES,    fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::ADDSURFACE,    fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::MOVEMESHPOINT, fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::SPLITELEMENT,  fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::CONNECTPOINTS, fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::JOINELEMENTS,  fullToolSet );
  pToolBar_Tools->EnableButton( TOOLS::REPOSITION,    fullToolSet );

  CViewWnd::ToolState ts = pView->GetToolState();
  for( ulong i = 0; i < 3; i++ ) {
    if( !mapTools.value( ts[i].first | ts[i].second ).first->isEnabled() ) {
      ToolId toolId;
      switch( i ) {
      case 0: toolId = TOOLS::DEFAULT_LEFT_TOOL; break;
      case 1: toolId = TOOLS::DEFAULT_MID_TOOL; break;
      default:
      case 2: toolId = TOOLS::DEFAULT_RIGHT_TOOL; break;
      }
      slotSelect( ts[i].first | toolId );
    }
  }

}
// View > Display
void CMainWnd::slotDisplay( int value )
{
  QAction* pAction = mapViewOptions[ (ShowFlag)value ];

  bool enable = pAction->isChecked();

  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::DISPLAY )
    .arg( pAction->text() ).arg( SM_MSG( enable ? DG3::ENABLE : DG3::DISABLE ) ) );

  pMV->CurrentView()->UpdateShowFlag( (ShowFlag)value, enable );
}
// View
void CMainWnd::slotMeshHeader()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::VIEW::MESH_HEADER ) );
  ModelPtr pModel = pMV->CurrentModel();
  DlgMeshHeader* pDialog = new DlgMeshHeader( pModel->GetMesh(), pModel, pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  pDialog->exec();
}
