#include "Actions.h"
#include "Model.h"

namespace Actions
{
  int ActHighLevel::Do()
  {
    FOREACH_CONST( ActionIterConst, it, actions )
      ((IAction*)*it)->Do();
    return 0;
  }


  ActHighLevel::~ActHighLevel()
  {
    FOREACH_CONST( ActionIterConst, it, actions )
      delete *it;
  }


  bool ActHighLevel::AddAction( IAction* _pAct )
  {
    // Discard redundant actions:

    // the opposite ChangeGroup (with original one)
    if( _pAct->Type() == ACT::CHANGE_OBJGROUP ||
        _pAct->Type() == ACT::HIGHLIGHT_OBJECT ||
        _pAct->Type() == ACT::MARK_OBJECT ) {
      FOREACH( ActionIter, itAct, actions ) {
        if( _pAct->IsOppositeTo( *itAct ) ) {
          delete _pAct;
          delete (*itAct);
          actions.erase( itAct );
          return false;
        }
      }
    }

    // ChangeObject of the same object before any other actions
    if( _pAct->Type() == ACT::CHANGE_NODE ||
        _pAct->Type() == ACT::CHANGE_CHORD ||
        _pAct->Type() == ACT::CHANGE_ELEMENT ||
        _pAct->Type() == ACT::CHANGE_GRIDPOINTEX ||
        _pAct->Type() == ACT::CHANGE_SEPARATOR ||
        _pAct->Type() == ACT::CHANGE_SOURCE ||
        _pAct->Type() == ACT::CHANGE_SURFACEEX ||
        _pAct->Type() == ACT::CHANGE_MESHPOINT ||
        _pAct->Type() == ACT::CHANGE_TEMPLATE ||
        _pAct->Type() == ACT::CHANGE_VARDEF ||
        _pAct->Type() == ACT::CHANGE_VARSETDEF ||
        _pAct->Type() == ACT::CHANGE_VALUE ) {
      FOREACH_CONST( ActionIterConst, itAct, actions )
        if( _pAct->IsEqualTo( *itAct ) ) {
          delete _pAct;
          return false;
        }
    }

    // Else add the action
    actions.push_front( _pAct );

    return true;
  }



  int ActAddMesh::Do()
  {
    if( pModel->IsLocked() )
      return 1;

    MeshPtr pM = dgtype_cast< MeshPtr >( pObject );
    ActDelMesh* pADM = new ActDelMesh( pModel, pM );
    assert( pM->IsComplete() );
    pModel->SetMesh( pM );
    pModel->ActionStack().AddUndoRec( pADM );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }



  int ActDelMesh::Do()
  {
    if( pModel->IsLocked() )
      return 1;
   
    MeshPtr pM = dgtype_cast< MeshPtr >( pObject );
    ActAddMesh* pAAM = new ActAddMesh( pModel, pM );
    assert( pModel->HasMesh( pM ) );
    assert( !pM->IsLocked() );
    pModel->SetMesh( null );
    pM->FreeVarContainings();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAM );
    return 0;
  }


  int ActChangeValue::Do()
  {
    if( pModel->IsLocked() )
      return 1;

    UPtr ps = pHolder->GetValue( id );
    if( ps.IsEqualByValue( value ) )
      return 0;
    pHolder->SetValue( id, value );

    if( pHolder != pModel ) {
      switch( pObject->Type() ) {
      case OT::VAR: {
        VarPtr pV = dgtype_cast< VarPtr >( pObject );
        if( HasAnyFlag( pV->VD()->VarDefType(), VTF::TARGET ) )
          pModel->SetChangeFlag( CHF::TARGETS );
        break;
      }
      default: break;
      }
    }

    ActChangeValue* pACV = new ActChangeValue( pModel, pObject, pHolder, id, ps, bRedraw );
    pModel->ActionStack().AddUndoRec( pACV );

    std::stringstream ss;
    ss << pHolder->GetName(id) << ": " << ps.ToString() << " -> " << value.ToString();
    LogMessage( DG3::ACT_CHANGED, ARGS( ss.str() ) );
    return 0;
  }



  int ActChangeMeshPoint::Do()
  {
    if( pModel->IsLocked() )
      return 1;

    MeshPointPtr pMPoint = dgtype_cast< MeshPointPtr >( pObject );
    assert( !pMPoint->Locks() );
    Point oldPosition = pMPoint->Position();
    ActChangeMeshPoint* pACMP = new ActChangeMeshPoint( pModel, pMPoint, oldPosition );
    pMPoint->SetPosition( position );
    pModel->ActionStack().AddUndoRec( pACMP );

    const IComponentList& crCells = pMPoint->ConnectedCells();
    FOREACHPTRCONST( MeshCellPtr, pMC, crCells ) {
      pModel->Agent()->AddToUpdate( pMC, UT::UPDATE );
      pModel->Agent()->AddToUpdate( pMC->ConnectedElements(), UT::UPDATE );
    }

    std::StringList args;
    if( oldPosition != position ) {
      std::stringstream ss;
      ss << "position" << ": " << oldPosition.ToString() << " -> " << position.ToString();
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }

  int ActUndoMark::Do()
  {
    if( pModel->IsLocked() )
      return 1;

    ActUndoMark* pActUM = new ActUndoMark( pModel );
    pModel->ActionStack().AddUndoRec( pActUM );
    LogMessage( DG3::ACT_UNDOMARK );
    return 0;
  }




  int ActMarkObject::Do()
  {
    if( pModel->HasMarked( pObject ) == include )
      return 1;

    if( pModel->IsLocked() )
      return 1;

    ActMarkObject* pActMO = new ActMarkObject( pModel, pObject, !include );

    include ? 
      pModel->IncludeMarked( pObject ) :
      pModel->ExcludeMarked( pObject );

    pModel->ActionStack().AddUndoRec( pActMO );

    std::string sInclude = pModel->GetStr( include ? DG3::ACT_MARK : DG3::ACT_UNMARK );
    LogMessage( DG3::ACT_MARKED, ARGS( sInclude ) );
    return 0;
  }


  int ActHighlightObject::Do()
  {
    if( pModel->HasHighlighted( pObject ) == include )
      return 1;

    if( pModel->IsLocked() )
      return 1;

    ActHighlightObject* pActHO = new ActHighlightObject( pModel, pObject, !include );

    include ? 
      pModel->IncludeHighlighted( pObject ) :
      pModel->ExcludeHighlighted( pObject );

    pModel->ActionStack().AddUndoRec( pActHO );

    std::string sInclude = pModel->GetStr( include ? DG3::ACT_HIGHLIGHT : DG3::ACT_UNHIGHLIGHT );
    LogMessage( DG3::ACT_HIGHLIGHTED, ARGS( sInclude ) );
    return 0;
  }


  int ActAddSource::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
    ActDelSource* pADS = new ActDelSource( pModel, pSrc );
    pModel->IncludeObject( pObject );
    pModel->ActionStack().AddUndoRec( pADS );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelSource::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( !pObject->IsLocked() );
    SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
    assert( !pSrc->HasVars() );
    ActAddSource* pAAS = new ActAddSource( pModel, pSrc );
    pModel->ExcludeObject( pObject );
    pSrc->FreeVars();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAS );
    return 0;
  }

  int ActChangeSource::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( !pObject->IsLocked() );

    SourcePtr pSrc = dgtype_cast< SourcePtr >( pObject );
    Point oldPosition = pSrc->Position();
    ActChangeSource* pACS = new ActChangeSource( pModel, pSrc, oldPosition );
    pSrc->SetPosition( position );
    pModel->ActionStack().AddUndoRec( pACS );

    std::StringList args;
    if( oldPosition != position ) {
      std::stringstream ss;
      ss << "position" << ": " << oldPosition.ToString() << " -> " << position.ToString();
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }


  int ActAddChord::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    ChordPtr pCh = dgtype_cast< ChordPtr >( pObject );
    ActDelChord* pADC = new ActDelChord( pModel, pCh );
    pModel->IncludeObject( pObject );
    pModel->ActionStack().AddUndoRec( pADC );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelChord::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( !pObject->IsLocked() );
    ChordPtr pCh = dgtype_cast< ChordPtr >( pObject );
    assert( !pCh->HasVars() && !pCh->HasContainers()  );
    ActAddChord* pAAC = new ActAddChord( pModel, pCh );
    pModel->ExcludeObject( pObject );
    pCh->FreeVars();
    pCh->FreeContainers();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAC );
    return 0;
  }

  int ActChangeChord::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( !pObject->IsLocked() );
    ChordPtr pCh = dgtype_cast< ChordPtr >( pObject );
    Point p1_old = pCh->Point_1();
    Point p2_old = pCh->Point_2();
    ActChangeChord* pACC = new ActChangeChord( pModel, pCh, p1_old, p2_old );
    pCh->SetPoints( p1, p2 );
    pModel->ActionStack().AddUndoRec( pACC );

    std::StringList args;
    if( p1_old != p1 ) {
      std::stringstream ss;
      ss << "point 1" << ": " << p1_old.ToString() << " -> " << p1.ToString();
      args.push_back( ss.str() );
    }
    if( p2_old != p2 ) {
      std::stringstream ss;
      ss << "point 2" << ": " << p2_old.ToString() << " -> " << p2.ToString();
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }

  int ActAddSonnetData::Do()
  {
    SENDER_NAME( "ActAddSonnetData" );
    if( pModel->IsLocked() )
      return 0;

    if( pModel->HasSonnetData() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::ALREADY_HAS_SONNETDATA );

    SonnetDataPtr pSD = dgtype_cast< SonnetDataPtr >( pObject );

    int retCode = 0;
    switch( DetectFileType( pSD->FileName().c_str() ) ) {
    case FT_SONNET:
      retCode = pSD->LoadSonnetFile();
      if( retCode != 0 )
        return retCode;
      break;
    case FT_NOTFOUND:
      return pModel->SendMessage( LOG_WARNING, SENDER,
                                  ERR::FILENOTFOUND, ARGS( pSD->FileName() ) );
    default:
      return pModel->SendMessage( LOG_WARNING, SENDER,
                                  ERR::BADFILETYPE, ARGS( pSD->FileName() ) );
    }

    ActDelSonnetData* pADSD = new ActDelSonnetData( pModel, pSD );
    pModel->SetSonnetData( pSD );
    pModel->ActionStack().AddUndoRec( pADSD );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelSonnetData::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( !pObject->IsLocked() );

    SonnetDataPtr pSD = dgtype_cast< SonnetDataPtr>( pObject );
    ActAddSonnetData* pAASD = new ActAddSonnetData( pModel, pSD );
    pModel->SetSonnetData( null );
    pSD->FreePoints();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAASD );
    return 0;
  }

  int ActAddSeparator::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    if( !pModel->HasSonnetData() )
      ValidatePtr( null,"ActAddSeparator.sonnetData");

    SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObject );
    ActDelSeparator* pADS = new ActDelSeparator( pModel, pSep );
    assert( pSep->Id() <= pModel->MaxElemId() );
    pSep->IncludeToNode();
    pModel->IncludeObject( pObject );
    /*LockObject(a->sonnetData,1);*/
    /* a->maxElemId=dg_max(a->maxElemId,sep->id); -- obsolete */
    pModel->ActionStack().AddUndoRec( pADS );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelSeparator::Do()
  {
    SENDER_NAME( "ActDelSeparator::Do" );
    if( pModel->IsLocked() )
      return 0;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObject );
    assert( !pSep->HasVars() );

    ActAddSeparator* pAAS = new ActAddSeparator( pModel, pSep );
    /*LockObject(a->sonnetData,-1);*/
    pModel->ExcludeObject( pObject );
    pSep->ExcludeFromNode();
    pSep->FreeVars();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAS );
    return 0;
  }

  int ActChangeSeparator::Do()
  {
    SENDER_NAME( "ActChangeSeparator::Do" );
    if( pModel->IsLocked() )
      return 0;

    SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( pObject );
    Point pos_old = pSep->Position();
    NodePtr pN_old = pSep->Node();
    int id_old = pSep->Id();

    if( pObject->IsLocked() &&
        !(pos_old == position && pN_old == pNode && id_old == id ) )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    assert( id <= pModel->MaxElemId() );
    ActChangeSeparator* pACS = new ActChangeSeparator( pModel, pSep, pos_old, pN_old, id_old );
    pSep->ExcludeFromNode();
    pSep->SetProperties( position, pNode, id );
    pSep->IncludeToNode();
    /* a->maxElemId=dg_max(a->maxElemId,ar->sep->id); -- obsolete */
    pModel->ActionStack().AddUndoRec( pACS );

    std::StringList args;
    if( pN_old != pNode  ) {
      std::stringstream ss;
      ss << "node" << ": " << pN_old->Position().ToString()
                 << " -> " << pNode->Position().ToString();
      args.push_back( ss.str() );
    }
    if( pos_old != position ) {
      std::stringstream ss;
      ss << "position" << ": " << pos_old.ToString()
                    << " -> " << position.ToString();
      args.push_back( ss.str() );
    }
    if( id_old != id ) {
      std::stringstream ss;
      ss << "id" << ": " << id_old << " -> " << id;
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }

  int ActAddComment::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    CommentPtr pComment = dgtype_cast< CommentPtr >( pObject );
    ActDelComment* pActDelComment = new ActDelComment( pModel, pComment );
    pModel->IncludeObject( pComment );
    /* LockObject(a->equil,1); */
    pModel->ActionStack().AddUndoRec( pActDelComment );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelComment::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    CommentPtr pComment = dgtype_cast< CommentPtr >( pObject );
    ActAddComment* pActAddComment = new ActAddComment( pModel, pComment );
    pModel->ExcludeObject( pComment );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddComment );
    return 0;
  }

} // Actions

