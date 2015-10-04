#include "../main/MainWnd.h"
#include "ViewWnd.h"

void CViewWnd::SelectItem()
{
  SENDER_NAME_Q( "slotDelete" );
  IComponentPtr pObject = pTool->pItem->ModelObject();
  pTool->isPossible = !pObject->IsLocked();
  if( pObject->Type() == OT::NODE ) {
    NodePtr pNode = dgtype_cast< NodePtr >( pObject );
    if( !pNode->IsEmpty() ) {
      pTool->isPossible = false;
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NONEMPTYNODE );
    }
  }
  /* Inhibit delete surfaces/grid points in Carre mode */

  else if( pModel->CarreOutputMode() ) {
    if( pObject->Type() == OT::SURFACEEX ) {
      SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
      if( !pSX->IsVirtual() ) {
        pTool->isPossible = false;
        pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::CARRE_INHIBITS );
      }
    }
    else if( pObject->Type() == OT::GRIDPOINTEX ) {
      pTool->isPossible = false;
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::CARRE_INHIBITS );
    }
  }

  pObject->Highlight();
  if( pTool->isPossible )
    SetExamineMsg( pObject );
}

void CViewWnd::slotDelete( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotDelete" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    pTool = new ToolBase();
    pTool->pItem = pScene->HitViewItem( mapToScene( position ), SHW::REMOVEABLE );
    if( pTool->pItem == null ) {
      delete pTool;
      pTool = null;
      break;
    }

    SelectItem();
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;

    if( !pTool->isShifted ) {
      IViewItemPtr pItem = pScene->HitViewItem( mapToScene( position ), SHW::REMOVEABLE );
      if( pItem == pTool->pItem )
        break;
      pTool->pItem->ModelObject()->Highlight( false );
      pTool->pItem = pItem;

      SelectItem();
      pModel->Agent()->Update();
    }
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    IComponentPtr pObject = pTool->pItem->ModelObject();
    pObject->Highlight( false );
    if( !pTool->isShifted ) {
      if( !pTool->isPossible ) {
        if( pObject->IsLocked() )
          pModel->ShowLockReasonOf( pObject );
        pModel->ActionStack().Cancel();
        emit UpdateObjectInfo( pTool->pItem->ModelObject() );
      }
      else {
        if( pObject->Type() == OT::ELEMENT ) {
          ElementPtr pElem = dgtype_cast< ElementPtr >( pObject );
          pElem->Delete();
        }
        else if( pObject->Type() == OT::COMMENT ) {
          ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pTool->pItem );
          if( pCommentItem->GetLinkedObject() != null ) {
            IViewItemPtr pLinkedItem = pScene->GetItem( pCommentItem->GetLinkedObject() );
            if( pLinkedItem != null )
              pLinkedItem->ExcludeComment( pCommentItem );
          }
          pObject->Delete();
        }
        else
          pObject->Delete();

        pModel->ActionStack().Complete( "Tool:Delete" );
        pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
      }
    }
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotAddElement( QPoint position, EEventID eventid )
{
  SENDER_NAME_Q( "slotAddElement" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;

    ToolAddElementPtr pToolAdd = new ToolAddElement( true );
    pTool = pToolAdd;

    emit ChangeShowFlag( SHW::ELEMS, true );
    NodePtr pNode = pModel->Struct()->AddNode( _C( mapToScene( position ) ) );
    pNode->Highlight();

    pModel->Agent()->Update();

    pToolAdd->pNodeFirstItem = pScene->GetItem( pNode );
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolAddElementPtr pToolAdd = new ToolAddElement();
    pTool = pToolAdd;

    pToolAdd->pNodeFirstItem = pScene->HitItemByType( SHW::NODES, _C( mapToScene( position ) ) );
    emit ChangeShowFlag( SHW::ELEMS, true );

    NodePtr pNode = null;
    if( pToolAdd->pNodeFirstItem == null ) {
      pNode = pModel->Struct()->AddNode( _C( mapToScene( position ) ) );
      pNode->Highlight();
      pModel->Agent()->Update();
      pToolAdd->pNodeFirstItem = pScene->GetItem( pNode );
    }
    else {
      pToolAdd->pNodeFirstItem->ModelObject()->Highlight();
      pModel->Agent()->Update();
    }

    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;
    ToolAddElementPtr pToolAdd = static_cast< ToolAddElementPtr >( pTool );
    if( !pTool->isMoved ) {
      pConsole->SetLoggingMode( LM::USUAL );
      pTool->SetMoved();
      NodePtr pNodeFirst = dgtype_cast< NodePtr >( pToolAdd->pNodeFirstItem->ModelObject() );
      NodePtr pNodeSecond = pModel->AddNode( _C( mapToScene( position ) ) );
      ElementPtr pElement = pModel->AddElem( pNodeFirst, pNodeSecond );

      pNodeSecond->Highlight();
      pElement->Highlight();

      pModel->Agent()->Update();
      pToolAdd->pNodeSecondItem = pScene->GetItem( pNodeSecond );
      pToolAdd->pElementItem = pScene->GetItem( pElement );
      pConsole->SetLoggingMode( LM::COLLECT );
    }
    else {
      NodePtr pNodeSecond = dgtype_cast< NodePtr >( pToolAdd->pNodeSecondItem->ModelObject() );
      pNodeSecond->Change( _C( mapToScene( position ) ) );
      pModel->Agent()->Update();
    }

    SetExamineMsg( pToolAdd->pElementItem->ModelObject() );
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    // Unhighlight all
    ToolAddElementPtr pToolAdd = static_cast< ToolAddElementPtr >( pTool );
    if( pToolAdd->pNodeFirstItem != null )
      pToolAdd->pNodeFirstItem->ModelObject()->Highlight( false );
    if( pToolAdd->pNodeSecondItem != null )
      pToolAdd->pNodeSecondItem->ModelObject()->Highlight( false );
    if( pToolAdd->pElementItem != null )
      pToolAdd->pElementItem->ModelObject()->Highlight( false );

    if( pTool->isMoved ) {
      pModel->ActionStack().Complete( "Tool:AddElement" );
      pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    }
    else
      pModel->ActionStack().Cancel();

    if( pToolAdd->pElementItem != null )
      emit UpdateObjectInfo( pToolAdd->pElementItem->ModelObject() );

    delete pTool;
    pTool = null;
    break;
  }
  }
}


void CViewWnd::slotAddSources( QPoint position, EEventID eventid )
{
  UNUSED(position);
  SENDER_NAME_Q( "slotAddSources" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;
    pTool = new ToolBase();
    SourcePtr pSource = pModel->AddSource( _C( mapToScene( position ) ) );
    emit ChangeShowFlag( SHW::SOURCES, true );
    pSource->Highlight();
    pModel->Agent()->Update();
    pTool->pItem = pScene->GetItem( pSource );
    SetExamineMsg( pSource );
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->isShifted )
      break;
    SourcePtr pSource = dgtype_cast< SourcePtr >( pTool->pItem->ModelObject() );
    pSource->Change( _C( mapToScene( position ) ) );
    pModel->Agent()->Update();
    SetExamineMsg( pSource );
    break;
  }
  case RELEASED:
  {
    if( pTool == null || pTool->isShifted )
      break;
    pTool->pItem->ModelObject()->Highlight( false );
    pModel->ActionStack().Complete( "Tool:AddSource" );
    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotAddChords( QPoint position, EEventID eventid )
{
  UNUSED(position);
  SENDER_NAME_Q( "slotAddChords" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;

    pTool = new ToolAddChord( _C( mapToScene( position ) ) );

    emit ChangeShowFlag( SHW::CHORDS, true );
    emit ChangeShowFlag( SHW::CHORDS3D, true );
    pModel->Agent()->Update();
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->isShifted )
      break;

    ToolAddChordPtr pToolAdd = static_cast< ToolAddChordPtr >( pTool );

    if( pTool->pItem == null ) {
      pConsole->SetLoggingMode( LM::USUAL );
      ChordPtr pChord = pModel->AddChord( pTool->position, _C( mapToScene( position ) ) );
      if( pChord == null ) {
        pConsole->SetLoggingMode( LM::COLLECT );
        break;
      }
      pChord->Highlight();
      pModel->Agent()->Update();
      pToolAdd->pItem = pScene->GetItem( pChord );
      pConsole->SetLoggingMode( LM::COLLECT );
    }
    else {
      ChordPtr pChord = dgtype_cast< ChordPtr >( pTool->pItem->ModelObject() );
      Point position_t = _C( mapToScene( position ) );
      if( !pScene->IsTopView() )
        pChord->Change( pChord->Point_1(), position_t );
      else
        pChord->Change( pChord->Point_1(), position_t.SwapYZ() );
      pModel->Agent()->Update();
    }
    SetExamineMsg( pTool->pItem->ModelObject() );
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;

    ToolAddChordPtr pToolAdd = static_cast< ToolAddChordPtr >( pTool );
    if( pToolAdd->pItem != null )
      pToolAdd->pItem->ModelObject()->Highlight( false );
    pModel->ActionStack().Complete( "Tool:AddChord" );
    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    if( pTool->pItem != null )
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotAddGridPoint( QPoint position, EEventID eventid )
{
  UNUSED(position);
  SENDER_NAME_Q( "slotAddGridPoint" );
  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
    break;
  case PRESSED:
  {
    if( pTool != null )
      break;
    if( !pModel->GetFluxModel()->HasEquil() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOEQUIL );
      break;
    }
    if( pModel->CarreOutputMode() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::CARRE_INHIBITS );
      break;
    }    
    pTool = new ToolBase();
    int zone = 0;
    double value = 0.;
    Point position_t = _C( mapToScene( position ) );
    if( pModel->GetFluxModel()->GetTopology()->HitGridPointPosition( position_t, &zone, &value ) != 0 )
      break; /* $ - error msg needed */
    emit ChangeShowFlag( SHW::GRIDPOINTS, true );
    GridPointExPtr pGPX = pModel->GetFluxModel()->AddGridPointEx( zone, value );
    pGPX->Highlight();
    pModel->Agent()->Update();
    pTool->pItem = pScene->GetItem( pGPX );
    SetExamineMsg( pGPX );
    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->pItem == null )
      break;
    int zone = 0;
    double value = 0.;
    Point position_t = _C( mapToScene( position ) );
    if( pModel->GetFluxModel()->GetTopology()->HitGridPointPosition( position_t, &zone, &value ) != 0 )
      break;
    GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pTool->pItem->ModelObject() );
    pGPX->Change( zone, value );
    SetExamineMsg( pGPX );
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    if( pTool->pItem != null )
      pTool->pItem->ModelObject()->Highlight( false );
    pModel->ActionStack().Complete( "Tool:AddGridPoint" );
    if( pTool->pItem != null )
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );
    pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotAddSurface( QPoint position, EEventID eventid )
{  
  UNUSED(position);
  SENDER_NAME_Q( "slotAddSurface" );

  FluxModelPtr pFlux = pModel->GetFluxModel();

  switch( eventid ) {
  case DOUBLECLICK:
    break;
  case SHIFTCLICK:
  {
    if( pTool != null )
      break;
    if( !pFlux->HasEquil() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOEQUIL );
      break;
    }
    emit ChangeShowFlag( SHW::SURFACES, true );

    pTool = new ToolBase( true );
    Point position_t = _C( mapToScene( position ) );
    int result = 0;
    SurfaceExPtr pSX = pFlux->AddSurfaceEx( position_t, &result );

    if( pSX == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, result );
      delete pTool;
      pTool = null;
      break;
    }
    pTool->isPossible = true;
    pSX->Highlight();
    pModel->Agent()->Update();
    pTool->pItem = pScene->GetItem( pSX );
    if( pTool->isPossible )
      SetExamineMsg( pSX );
    else /* Should never happen */
      pConsole->Send( STATUS_WARN, FULL_SENDER, result );
    break;
  }
  case PRESSED:
  {
    if( pTool != null )
      break;
    if( !pFlux->HasEquil() ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, ERR::NOEQUIL );
      break;
    }
    emit ChangeShowFlag( SHW::SURFACES, true );

    pTool = new ToolBase();
    Point position_t = _C( mapToScene( position ) );
    int result = 0;
    SurfaceExPtr pSX = pFlux->AddSurfaceEx( position_t, &result, true );

    if( pSX == null ) {
      pConsole->Send( STATUS_WARN, FULL_SENDER, result );
      delete pTool;
      pTool = null;
      break;
    }

    if( pTool == null )
      break; // It is possible if error dialog was created

    pTool->isPossible = (pSX->Zone() != SZN_XY && pSX->CoordsOk());

    if( pFlux->GetTopology()->SurfaceZoneBoundsCached() )
      pConsole->Send( STATUS_INFO, FULL_SENDER, DGMSG::CALCULATING_SZ_BOUNDS );

    SurfaceZonePtr pSZ = pFlux->GetTopology()->FindSurfaceZone( pSX->Zone() );
    if( pTool->isPossible &&
        pModel->CarreOutputMode() &&
        pSZ != null &&
        pFlux->CountSurfaces( pSX->Zone() ) > !!(pSZ->Flags() & SZF::LIMITBYSURFACE) ) {
      pTool->isPossible = false;
      result = ERR::CARRE_INHIBITS;
    }

    if( pTool->isPossible )
      SetExamineMsg( pSX );
    else
      pConsole->Send( STATUS_INFO, FULL_SENDER, result );

    pSX->Highlight();
    pModel->Agent()->Update();
    pTool->pItem = pScene->GetItem( pSX );

    if( !pTool->isPossible ) {
      pTool->pItem->SetFlag( IViewItem::p_showError, true );
      pTool->pItem->UpdateStyle();
    }

    break;
  }
  case MOVED:
  {
    if( pTool == null || pTool->pItem == null )
      break;
    SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pTool->pItem->ModelObject() );
    int result = 0;
    Point position_t = _C( mapToScene( position ) );
    if( pTool->isShifted ) {
      pSX->Change( position_t, &result );
      SetExamineMsg( pSX );
    }
    else {
      pSX->Change( position_t, &result, true );

      pTool->isPossible = (pSX->Zone() != SZN_XY && pSX->CoordsOk());
      SurfaceZonePtr pSZ = pFlux->GetTopology()->FindSurfaceZone( pSX->Zone() );
      if( pTool->isPossible &&
          pModel->CarreOutputMode() &&
          pSZ != null &&
          pFlux->CountSurfaces( pSX->Zone() ) > !!(pSZ->Flags() & SZF::LIMITBYSURFACE) ) {
        pTool->isPossible = false;
        result = ERR::CARRE_INHIBITS;
      }

      pTool->pItem->SetFlag( IViewItem::p_showError, !pTool->isPossible );

      if( pTool->isPossible )
        SetExamineMsg( pSX );
      else
        pConsole->Send( STATUS_INFO, FULL_SENDER, result );
    }
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;
    if( pTool->pItem != null )
      pTool->pItem->ModelObject()->Highlight( false );

    if( pTool->isPossible ) {
      pModel->ActionStack().Complete( "Tool:AddSurface" );
      pConsole->Send( STATUS_INFO, FULL_SENDER, "" );
    }
    else
      pModel->ActionStack().Cancel();

    if( pTool->isPossible && pTool->pItem != null )
      emit UpdateObjectInfo( pTool->pItem->ModelObject() );

    delete pTool;
    pTool = null;
    break;
  }
  }
}

void CViewWnd::slotAddComment( QPoint position, EEventID eventid )
{
  QPointF posT = mapToScene( position );
  SENDER_NAME_Q( "slotAddComment" );
  switch( eventid ) {
  case DOUBLECLICK: {
    const double hitRadius = 10. / state.scale.x;
    IViewItemPtr pItem = pScene->HitItemByType( SHW::COMMENTS, posT, null, hitRadius );
    if( pItem != null ) {
      ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pItem );
      CommentPtr pComment = dgtype_cast< CommentPtr >( pItem->ModelObject() );
      DlgCommentEdit* pDialog = new DlgCommentEdit( pComment, pCommentItem->GetLinkedObject(), pSM, this );
      if( pDialog->exec() == QDialog::Accepted )
        pModel->Agent()->AddToUpdate( pComment, UT::UPDATE );
      pModel->ActionStack().Complete( "Tool:Comment:Change" );
    }
    break;
  }
  case SHIFTCLICK:
  case PRESSED:
  {
    if( pTool != null )
      break;

    ToolCommentPtr pToolComment = new ToolComment( _C(posT), eventid == SHIFTCLICK );
    pTool = pToolComment;
    const double hitRadius = 10. / state.scale.x;
    pTool->pItem = pScene->HitItemByType( SHW::COMMENTS, posT, null, hitRadius );
    if( pTool->pItem == null ) {
      emit ChangeShowFlag( SHW::COMMENTS, true );
      CommentPtr pComment = pModel->AddComment(
        OT::UNKNOWN, _C(posT), _C(posT), QString( "Text" ).toStdWString(), pScene->Type() );
      pModel->Agent()->Update();
      pTool->pItem = pScene->GetItem( pComment );
      pToolComment->newComment = true;
    }
    else {
      pToolComment->newComment = false;
      ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pTool->pItem );
      CommentPtr pComment = dgtype_cast< CommentPtr >( pTool->pItem->ModelObject() );
      if( pComment->ObjType() != OT::UNKNOWN )
        pToolComment->pLinkedItem = pScene->GetItem( pCommentItem->GetLinkedObject() );
      pModel->Agent()->Update();
    }
    break;
  }
  case MOVED:
  {
    if( pTool == null )
      break;

    CommentPtr pComment = dgtype_cast< CommentPtr >( pTool->pItem->ModelObject() );
    ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pTool->pItem );
    ToolCommentPtr pToolComment = static_cast< ToolCommentPtr >( pTool );

    const double linkingRadius = 10. / state.scale.x;

    if( pToolComment->newComment == pTool->isShifted ) { /* Move textbox */
      pComment->ChangePosition( _C( posT ) );
      SetExamineMsg( null );
    }
    else { /* Move and link end with examinable object */
      if( pToolComment->pLinkedItem != null )
        pToolComment->pLinkedItem->ExcludeComment( pCommentItem );
      pToolComment->pLinkedItem = pScene->HitViewItem( posT, SHW::EXAMINABLE, linkingRadius );
      if( pCommentItem->GetLinkedObject() != null ) // Unhighlight previous object
        pCommentItem->GetLinkedObject()->Highlight( false );

      if( pToolComment->pLinkedItem != null ) { // Has object to link with
        IComponentPtr pLinkedObject = pToolComment->pLinkedItem->ModelObject();
        pLinkedObject->Highlight();

        pCommentItem->SetLinkedObject( pLinkedObject );
        pToolComment->pLinkedItem->IncludeComment( pCommentItem );
        pComment->ChangePosEnd( pLinkedObject->CentralPoint() ); //TODO: nearest point
        pComment->ChangeObjectType( pLinkedObject->Type() );
        SetExamineMsg( pLinkedObject );
      }
      else { // Free end position
        pCommentItem->SetLinkedObject( null );
        pComment->ChangePosEnd( _C( posT ) );
        pComment->ChangeObjectType( OT::UNKNOWN );
        SetExamineMsg( null );
      }
    }

    pModel->Agent()->AddToUpdate( pComment, UT::UPDATE );
    pModel->Agent()->Update();
    break;
  }
  case RELEASED:
  {
    if( pTool == null )
      break;

    CommentPtr pComment = dgtype_cast< CommentPtr >( pTool->pItem->ModelObject() );
    ViewCommentItemPtr pCommentItem = static_cast< ViewCommentItemPtr >( pTool->pItem );
    ToolCommentPtr pToolComment = static_cast< ToolCommentPtr >( pTool );

    if( pToolComment->newComment ) {
      DlgCommentEdit* pDialog = new DlgCommentEdit( pComment, pCommentItem->GetLinkedObject(), pSM, this );
      pDialog->exec();
    }

    IComponentPtr pLinkedObject = pCommentItem->GetLinkedObject();
    if( pLinkedObject != null )
      pLinkedObject->Highlight( false );

    pModel->ActionStack().Complete( "Tool:AddComment" );

    if( pLinkedObject != null )
      emit UpdateObjectInfo( pLinkedObject );

    delete pTool;
    pTool = null;
    break;
  }
  }
}
