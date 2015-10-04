#include "MainWnd.h"

// Edit
void CMainWnd::slotUndo()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::UNDO ) );
  pMV->CurrentModel()->ActionStack().Undo();
}

void CMainWnd::slotRedo()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::REDO ) );
  pMV->CurrentModel()->ActionStack().Redo();
}

void CMainWnd::slotRedoAll()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::REDO_ALL ) );
  pMV->CurrentModel()->ActionStack().RedoAll();
}

void CMainWnd::slotClearActionStack()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::CLEAR_ASTACK ) );
  pMV->CurrentModel()->ActionStack().FreeUndoInfo();
  UpdateActionsInfo();
}

void CMainWnd::slotMarkAllElements()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::MARK_ALL_ELEMENTS ) );
  ModelPtr pModel = pMV->CurrentModel();
  pModel->MarkGroup( OT::ELEMENT );
  slotChangeShowFlag( SHW::ELEMS, true );
  pModel->ActionStack().Complete( "MarkAllElements" );
 }

void CMainWnd::slotMarkAllSeparators()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::MARK_ALL_SEPARATORS ) );
  ModelPtr pModel = pMV->CurrentModel();
  pModel->MarkGroup( OT::SEPARATOR );
  slotChangeShowFlag( SHW::SEPARATORS, true );
  pModel->ActionStack().Complete( "MarkAllSEparators" );
}

void CMainWnd::slotMarkAllChords()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::MARK_ALL_CHORDS ) );
  ModelPtr pModel = pMV->CurrentModel();
  pModel->MarkGroup( OT::CHORD );
  slotChangeShowFlag( SHW::CHORDS, true );
  pModel->ActionStack().Complete( "MarkAllChords" );
}

void CMainWnd::slotUnmarkAll()
{
  pConsole->Send( LOG_INFO, sender_name, SM_LOG( LOG::MENU::EDIT::UNMARK_ALL ) );
  ModelPtr pModel = pMV->CurrentModel();
  pModel->UnmarkAll();
  pModel->ActionStack().Complete( "UnmarkAll" );
}

// Edit > Create
void CMainWnd::slotCreatePoint()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::CREATE::POINT ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr    pModel = pMV->CurrentModel();
  CViewWndPtr pView = pMV->CurrentView();

  DlgCreatePoint* pDialog = new DlgCreatePoint( pSM, SM_DLG( DLG::CREATE_NODE::TITLE ), this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );

  forever {
    if( pDialog->exec() != QDialog::Accepted )
      break;
    IComponentPtr pNode = pModel->FindNode( pDialog->Position() );
    if( pNode != null ) {
      pView->CurrentScene()->CreateLabelItem( pNode, SM_MSG( STR::ERRLABEL ) );
      pModel->Agent()->Update();
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                QStringList( QString::fromStdString( pNode->DetailedInfo() ) ) );
      continue; // try again
    }
    pNode = pModel->AddNode( pDialog->Position() );
    pView->CurrentScene()->CreateLabelItem( pNode, SM_MSG( STR::NEWLABEL ) );
    slotChangeShowFlag( SHW::NODES, true );
    slotChangeShowFlag( SHW::IRRNODES, true );
    pModel->ActionStack().Complete( "CreatePoint" );
    UpdateObjectInfo( pNode );
    break;
  }
}

void CMainWnd::slotCreateSource()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::CREATE::SOURCE ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr    pModel = pMV->CurrentModel();
  CViewWndPtr pView = pMV->CurrentView();

  DlgCreatePoint dialog( pSM, SM_DLG( DLG::CREATE_SOURCE::TITLE ), this );

  forever {
    if( dialog.exec() != QDialog::Accepted )
      break;
    IComponentPtr pSrc = pModel->FindSource( dialog.Position() );
    if( pSrc != null ) {
      pView->CurrentScene()->CreateLabelItem( pSrc, SM_MSG( STR::ERRLABEL ) );
      pModel->Agent()->Update();
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                QStringList( QString::fromStdString( pSrc->DetailedInfo() ) ) );
      continue; // try again
    }
    pSrc = pModel->AddNode( dialog.Position() );
    pView->CurrentScene()->CreateLabelItem( pSrc, SM_MSG( STR::NEWLABEL ) );
    slotChangeShowFlag( SHW::SOURCES, true );
    pModel->ActionStack().Complete( "CreateSource" );
    UpdateObjectInfo( pSrc );
    break;
  }
}

void CMainWnd::slotCreateChord()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::CREATE::CHORD ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr    pModel = pMV->CurrentModel();
  CViewWndPtr pView = pMV->CurrentView();

  DlgCreateChord* pDialog = new DlgCreateChord( pSM, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );

  forever {
    if( pDialog->exec() != QDialog::Accepted )
      break;
    Point p1 = pDialog->Position1();
    Point p2 = pDialog->Position2();

    IComponentPtr pCh = pModel->FindChord( p1, p2, true );
    if( pCh != null ) {
      pView->CurrentScene()->CreateLabelItem( pCh, SM_MSG( STR::ERRLABEL ) );
      pModel->Agent()->Update();
      pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                QStringList( QString::fromStdString( pCh->DetailedInfo() ) ));
      continue; // try again
    }
    pCh = pModel->AddChord( p1, p2, true );
    pView->CurrentScene()->CreateLabelItem( pCh, SM_MSG( STR::NEWLABEL ) );
    slotChangeShowFlag( SHW::CHORDS, true );
    pModel->ActionStack().Complete( "CreateChord" );
    UpdateObjectInfo( pCh );
    break;
  }
}

void CMainWnd::slotMarkBoundingElem( int area )
{
  SENDER_NAME_Q( "MarkBoundingElements" );

  ModelPtr pModel = pMV->CurrentModel();
  SurfaceZonePtr pSZ = pModel->GetFluxModel()->GetTopology()->FindSurfaceZone( area );
  if( pSZ != null && pSZ->Innermost() != null ) {
    pModel->UnmarkAll();
    pSZ->Innermost()->Mark();
    slotChangeShowFlag( SHW::ELEMS, true );
    pModel->ActionStack().Complete( "MarkBoundingElem" );
  }
  else
    pConsole->Send( LOG_WARNING, SENDER, DGMSG::NOBOUNDINGELEMS );
}

void CMainWnd::slotCreateSurface()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::CREATE::SURFACE ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr     pModel = pMV->CurrentModel();
  CViewWndPtr  pView = pMV->CurrentView();
  FluxModelPtr pFlux = pModel->GetFluxModel();

  if( !pFlux->HasEquil() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NOEQUIL );
    return;
  }
  if( pFlux->GetTopology()->SurfaceZones().size() == 0 ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, DG3::NO_TOPOLOGY );
    return;
  }
  DlgCreateSurface* pDialog = pMV->CurrentProxy()->OpenCreateSurfacesDlg();

  forever {
    if( pDialog->exec() == QDialog::Accepted ) {
      int area = pDialog->SingleArea();
      double level = pDialog->SingleLevel();
      if( pDialog->IsSingleByLevel() ) {
        /* Check for duplicate surfaces */
        SurfaceExPtr pSX = pFlux->FindSurface( area, level );
        if( pSX != null ) {
          pView->CurrentScene()->CreateLabelItem( pSX, SM_MSG( STR::ERRLABEL ) );
          pModel->Agent()->Update();
          pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                    QStringList( QString::fromStdString( pSX->DetailedInfo() ) ) );
          continue; // try again
        }
        /* Try adding the surface */
        int r = 0;
        pFlux->AddSurfaceEx( area, level, &r );
        if( pSX == null ) {
          pConsole->Send( WND_ERROR, FULL_SENDER, r );
          continue; // try again
        }
        pView->CurrentScene()->CreateLabelItem( pSX, SM_MSG( STR::NEWLABEL ) );
      }
      else if( pDialog->IsSingleByPoint() ) {
        /* Check for duplicate surfaces */
        SurfaceExPtr pSX = pFlux->FindSurface( pDialog->SinglePoint() );
        if( pSX != null ) {
          pView->CurrentScene()->CreateLabelItem( pSX, SM_MSG( STR::ERRLABEL ) );
          pModel->Agent()->Update();
          pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                    QStringList( QString::fromStdString( pSX->DetailedInfo() ) ) );
          continue; // try again
        }
        /* Try adding the surface */
        int r = 0;
        pSX = pFlux->AddSurfaceEx( area, level, &r );
        if( pSX == null ) {
          pConsole->Send( WND_ERROR, FULL_SENDER, r );
          continue; // try again
        }
        pView->CurrentScene()->CreateLabelItem( pSX, SM_MSG( STR::NEWLABEL ) );
        pModel->ActionStack().Complete( "CreateSurface" );
        UpdateObjectInfo( pSX );
      }
      else { // dialog.IsMultiple()

        /* if (level2<level1) swap(level1,level2); -- Why did I do this?? */

        /* Remove old surfaces */
        bool removeOld = pDialog->RemoveOldOption();
        if( pModel->CarreOutputMode() )
          removeOld = true;

        if( removeOld ) {
          if( !pModel->CarreOutputMode() ) {
            /* Sonnet mode: delete surfaces with matching creatorId */
            IComponentList vSX_removed;
            int i = pFlux->RemoveSurfacesByCreatorId( pDialog->OldCreatorId(), &vSX_removed );
            if( i == 0 ) {
              pModel->ActionStack().Cancel();
              pConsole->Send( WND_ERROR, FULL_SENDER,
                   DLG::CREATE_SURFACE::ERR::OLD_SURFACES_NOT_FOUND );
              continue; // try again
            }
          }
          else {
            /* Carre mode: delete all surfaces in the area */
            /* -- Now done in DistributeSurfaces, because of level calculation
            for (s=AppSurface1st(dlg->w->app,&ix);s!=NULL;s=Next(&ix))
              if (GetSurfaceArea(dlg->w->app,s)==area)
                DelSurface(dlg->w->app,s); */
          }
        }
        else if( pModel->CarreOutputMode() ) {
          pModel->ActionStack().Cancel();
          pConsole->Send( WND_ERROR, FULL_SENDER,
                        DLG::CREATE_SURFACE::ERR::CARRE_OLD_SURFACES );
          continue; // try again
        }

        /* Create surfaces */
        CreatorId crid_old;
        int r = pFlux->DistributeSurfaces( pDialog->MultipleCreator(), &crid_old );

        if( r != 0 ) {
          pModel->ActionStack().Cancel();
          pConsole->Send( WND_ERROR, FULL_SENDER, r );
          continue; // try again
        }

        pModel->ActionStack().Complete( "CreateSurface" );

        /* MB: what does it do before closing dialog?
        strcpy(dlg->oldCreatorId,creatorId);
        if (!carreMode) {
          SetSensitiveEx(dlg->wSwRemoveOld,!!dlg->dg.count);
          XmToggleButtonSetState(dlg->wSwRemoveOld,!!dlg->dg.count,True);
        }
        */
      }

      slotChangeShowFlag( SHW::SURFACES, true );
      break;
    }
    else
      break;
  }
}

void CMainWnd::slotCreateGridPoints()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::CREATE::GRIDPOINTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr     pModel = pMV->CurrentModel();
  CViewWndPtr  pView = pMV->CurrentView();
  FluxModelPtr pFlux = pModel->GetFluxModel();

  if( !pFlux->HasEquil() ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, ERR::NOEQUIL );
    return;
  }
  if( pFlux->GetTopology()->GridPointSegs().size() == 0 ) {
    pConsole->Send( WND_ERROR, FULL_SENDER, DG3::NO_TOPOLOGY );
    return;
  }
  DlgCreateGridPoint* pDialog = pMV->CurrentProxy()->OpenCreateGridPointsDlg();

  forever {
    if( pDialog->exec() == QDialog::Accepted ) {
      if( pDialog->IsSingle() ) {
        int area = pDialog->SingleArea();
        double value = pDialog->SingleValue();
        GridPointExPtr pGPX = pFlux->FindGridPoint( area, value );
        if( pGPX != null ) {
          pView->CurrentScene()->CreateLabelItem( pGPX, SM_MSG( STR::ERRLABEL ) );
          pModel->Agent()->Update();
          pConsole->Send( WND_ERROR, FULL_SENDER, ERR::ALREADYEXISTS,
                    QStringList( QString::fromStdString( pGPX->DetailedInfo() ) ) );
          continue; // try again
        }
        pGPX = pFlux->AddGridPointEx( area, value );
        if( pGPX == null ) {
          pConsole->Send( WND_ERROR, FULL_SENDER, ERR::INTERNALGP );
          continue; // TODO: or return for internal errors?
        }
        pView->CurrentScene()->CreateLabelItem( pGPX, SM_MSG( STR::ERRLABEL ) );
        pModel->ActionStack().Complete( "CreateGridPoints" );
        UpdateObjectInfo( pGPX );
      }
      else { // dialog.isMultiple()
        int r = pFlux->DistributeGridPoints( pDialog->MultipleCreator() );
        if( r != 0 ) {
          pModel->ActionStack().Cancel();
          pConsole->Send( WND_ERROR, FULL_SENDER, r );
          continue; // try again
        }
        pModel->ActionStack().Complete( "CreateGridPoints" );
      }
      slotChangeShowFlag( SHW::GRIDPOINTS, true );
      break; // success
    }
    else
      break;
  }
}

// Edit > Delete

bool CMainWnd::DeleteObject( IComponentPtr _pObject, bool _single )
{
  if( _pObject == null )
    return false;
  ModelPtr pModel = pMV->CurrentModel();
  if( _pObject->IsLocked() ) {
    if( !_single ) {
      pModel->ActionStack().Cancel();
      pMV->CurrentView()->CurrentScene()->CreateLabelItem( _pObject, SM_MSG( STR::ERRLABEL ) );
    }
    pModel->ShowLockReasonOf( _pObject );
    return false;
  }

  _pObject->Delete();
  return true;
}

void CMainWnd::slotDeleteMarkedObjects()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::MARKED_OBJECTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr    pModel = pMV->CurrentModel();
  CViewWndPtr pView = pMV->CurrentView();
  const IComponentList& objects = pModel->MarkedObjects();
  foreach( IComponentPtr pObject, objects ) {
    switch( pObject->Type() ) {
    case OT::ELEMENT:
    case OT::SOURCE:
    case OT::CHORD: {
      if( !DeleteObject( pObject ) )
        return;
      break;
    }
    default:
      pModel->ActionStack().Cancel();
      pView->CurrentScene()->CreateLabelItem( pObject, SM_MSG( STR::ERRLABEL ) );
      pMV->CurrentProxy()->UpdateViews();
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::DEL_MARKED_OBJECT_TYPE );
      return;
    }
  }
  pModel->ActionStack().Complete( "DeleteMarkedObjects" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::MARKED_REMOVED );
}

void CMainWnd::slotDeleteUnusedPoints()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::UNUSED_POINTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );
  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = pModel->UnusedNodes();
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteUnusedPoints" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::EMPTYNODESREMOVED );
}

void CMainWnd::slotDeleteVirtualSurfaces()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::VIRTUAL_SURFACES ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = pModel->GetFluxModel()->VirtualSurfaces();
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteVirtualSurfaces" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::SURFACESREMOVED );
}

void CMainWnd::slotDeleteElements()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::ELEMENTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& crObjects = pModel->Elements();
  foreach( IComponentPtr pObject, crObjects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteElements" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::ELEMSREMOVED );
}

void CMainWnd::slotDeleteSources()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::SOURCES ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = pModel->Sources();
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteSources" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::SOURCESREMOVED );
}

void CMainWnd::slotDeleteChords()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::CHORDS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = pModel->Chords();
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteChords" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::CHORDSREMOVED );
}

void CMainWnd::slotDeleteSurfaces()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::SURFACES ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = *pModel->SelectList( OT::SURFACEEX );
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteSurfaces" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::SURFACESREMOVED );
}

void CMainWnd::slotDeleteGridPoints()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::GRIDPOINTS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = *pModel->SelectList( OT::GRIDPOINTEX );
  foreach( IComponentPtr pObject, objects ) {
    if( !DeleteObject( pObject ) )
      return;
  }
  pModel->ActionStack().Complete( "DeleteGridPoints" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::GPOINTSREMOVED );
}

void CMainWnd::slotDeleteSeparators()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::SEPARATORS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );
  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& objects = pModel->Separators();
  foreach( IComponentPtr pObject, objects )
    pObject->Delete(); // Can not be locked as DeleteObject()
  pModel->ActionStack().Complete( "DeleteSeparators" );
  pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::CHORDSREMOVED );
}

void CMainWnd::slotDeleteEquilibrium()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::EQUILIBRIUM ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  if( DeleteObject( pModel->GetFluxModel()->GetEquil(), false ) )
    pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::EQUILREMOVED );

  bool equilExists = pModel->GetFluxModel()->HasEquil();
  mapMenuActions.value( MENU::FILES::IMPORT::TOPOLOGY )->setEnabled( equilExists );
  mapMenuActions.value( MENU::EDIT::CREATE::GRIDPOINTS )->setEnabled( equilExists );
  mapMenuActions.value( MENU::EDIT::CREATE::SURFACE )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::EDIT_TOPOLOGY )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTS )->setEnabled( equilExists );
  mapMenuActions.value( MENU::COMMANDS::FIND_XPOINTSEGS )->setEnabled( equilExists );

  pModel->ActionStack().Complete( "DeleteEquilibrium" );
}

void CMainWnd::slotDeleteTemplate()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::TEMPLATE ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  if( DeleteObject( pModel->GetTemplate(), false ) )
    pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::TEMPLATEREMOVED );

  pModel->ActionStack().Complete( "DeleteTemplate" );
}

void CMainWnd::slotDeleteMesh()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::MESH ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  if( DeleteObject( pModel->GetMesh(), false ) )
    pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::SONNETREMOVED );

  bool meshExists = pModel->HasMesh();
  mapMenuActions.value( MENU::VIEW::MESH_HEADER )->setEnabled( meshExists );
  //mapMenuActions.value( MENU::OPTIONS::MESH_EDITING )->setEnabled( meshExists );

  pModel->ActionStack().Complete( "DeleteMesh" );
}

void CMainWnd::slotDeleteSonnetGrid()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::DELETE::SONNETGRID ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  if( DeleteObject( pModel->GetSonnetData(), false ) )
    pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::SONNETREMOVED );

  pModel->ActionStack().Complete( "DeleteSonnetGrid" );
}

// Edit
void CMainWnd::slotExtendChords()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::EXTEND_CHORDS ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelPtr pModel = pMV->CurrentModel();
  const IComponentList& marked = pModel->MarkedObjects();
  bool done = false;

  FOREACH_CONST( IComponentIterConst, it, marked ) {
    if( (*it)->Type() == OT::CHORD ) {
      ChordPtr pCh = dgtype_cast< ChordPtr >( *it );
      pCh->Extend( pMV->CurrentView()->CurrentScene()->IsTopView() );
      done = true;
    }
  }

  if( done )
    pModel->ActionStack().Complete( "ExtendChords" );
  else {
    pModel->ActionStack().Cancel();
    pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::NOMARKEDCHORDS );
  }
}

void CMainWnd::slotMoveRotate()
{
  SENDER_NAME_Q( SM_LOG( LOG::MENU::EDIT::MOVE_ROTATE ) );
  pConsole->Send( LOG_INFO, sender_name, SENDER );

  ModelViewProxyPtr pProxy = pMV->CurrentProxy();
  ModelPtr          pModel = pProxy->GetModel();

  DlgMoveRotate* pDialog = new DlgMoveRotate( pSM, pConsole, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SLOT(slotDialogHelp(int)) );
  forever {
    if( pDialog->exec() == QDialog::Rejected )
      break;

    int r = pDialog->exec();
    if( r == QDialog::Rejected )
      return;

    if( !pDialog->ChangeElements() && !pDialog->ChangeTemplate() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOTHINGTODO );
      continue; // try again
    }

    if( pDialog->ChangeTemplate() ) {
      TemplatePtr pT = pModel->GetTemplate();
      if( pT != null ) {
        if( r == DlgMoveRotate::r_move )
          pT->Change( TemplateParams( pT->Incr()*pDialog->Scale()+pDialog->Offset(),
                                      pT->Angle(), pT->Scale()*pDialog->Scale() ) );
        else if( r == DlgMoveRotate::r_rotate ) {
          double angle = pT->Angle() + M_PI_2;
          if( angle >= M_2_PI )
            angle -= M_2_PI;
          pT->Change( TemplateParams( pT->Incr(), angle, pT->Scale() ) );
        }
        slotChangeShowFlag( SHW::TEMPLATE, true );
        pModel->ActionStack().Complete( "MoveRotate" );
        UpdateObjectInfo( pT );
      }
    }
    if( pDialog->ChangeElements() ) {
      IComponentPtr pN = pModel->LockedNode();
      if( pN != null ) {
        pModel->ActionStack().Cancel();
        pModel->ShowLockReasonOf( pN );
        return; // leave dialog: smth is wrong in model
      }
      const IComponentList& nodes = pModel->Nodes();
      if( !nodes.empty() ) {
        FOREACHPTRCONST( NodePtr, pN, nodes ) {
          if( r == DlgMoveRotate::r_move )
            pN->Change( pN->Position() * pDialog->Scale() + pDialog->Offset() );
          else if( r == DlgMoveRotate::r_rotate )
            pN->Change( Point( -pN->Y(), pN->X() ) );
        }
        slotChangeShowFlag( SHW::ELEMS, true );
      }
      pModel->ActionStack().Complete( "MoveRotate" );
    }

    break;
  }

}
