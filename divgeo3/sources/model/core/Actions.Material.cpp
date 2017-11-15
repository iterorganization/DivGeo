#include "Actions.h"
#include "Model.h"

namespace Actions
{
  
  int ActAddNode::Do()
  {
    if( pModel->IsLocked() )
      return 1;

    NodePtr pNode = dgtype_cast< NodePtr >( pObject );
    ActDelNode* pADN = new ActDelNode( pModel, pNode );
    pModel->IncludeObject( pObject );
    pModel->ActionStack().AddUndoRec( pADN );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelNode::Do()
  {
    SENDER_NAME( "ActDelNode::Do" );
    if( pModel->IsLocked() )
      return 1;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    NodePtr pNode = dgtype_cast< NodePtr >( pObject );

    if( pNode->HasElements() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::NODE_HAS_ELEMENTS );

    if( pNode->HasSeparators() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::NODE_HAS_SEPARATORS );

    ActAddNode* pADN = new ActAddNode( pModel, pNode );
    pModel->ExcludeObject( pObject );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( (IAction*)pADN );
    return 0;
  }

  int ActChangeNode::Do()
  {
    SENDER_NAME( "ActChangeNode::Do" );
    if( pModel->IsLocked() )
      return 0;
  
    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    NodePtr pNode = dgtype_cast< NodePtr >( pObject );
    Point oldPos = pNode->Position();
    ActChangeNode* pACN = new ActChangeNode( pModel, pNode, oldPos );
    pNode->SetXYZ( position );
    if( pNode->IsInTarget() )
      pModel->SetChangeFlag( CHF::TARGETS );
    pModel->ActionStack().AddUndoRec( pACN );

    const IComponentList& crElements = pNode->ConnectedElements();
    IComponentPtr FOREACHOBJCONST( pObj, crElements )
      pModel->Agent()->AddToUpdate( pObj, UT::UPDATE );
    const IComponentList& crSeparators = pNode->ConnectedSeparators();
    FOREACHOBJCONST( pObj, crSeparators )
      pModel->Agent()->AddToUpdate( pObj, UT::UPDATE );

    std::StringList args;
    if( oldPos != position ) {
      std::stringstream ss;
      ss << "position" << ": " << oldPos.ToString( Point::POS3D )
                     << " -> " << position.ToString( Point::POS3D );
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }

  int ActAddElem::Do()
  {  
    if( pModel->IsLocked() )
      return 0;

    ElementPtr pE = dgtype_cast< ElementPtr >( pObject );
    ActDelElem* pADE = new ActDelElem( pModel, pE );
    assert( pObject->HasModel( pModel ) );
    assert( pE->Id() <= pModel->MaxElemId() );
    pE->Node( 1 )->IncludeElement( pE );
    pE->Node( 2 )->IncludeElement( pE );
    pModel->IncludeObject( pObject );
    /*a->maxElemId=dg_max(a->maxElemId,e->id); -- obsolete */
    pModel->ActionStack().AddUndoRec( pADE );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelElem::Do()
  {
    SENDER_NAME( "ActDelElem::Do" );
    if( pModel->IsLocked() )
      return 0;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    ElementPtr pE = dgtype_cast< ElementPtr >( pObject );
    assert( !pE->HasVars() );
    ActAddElem* pAAE = new ActAddElem( pModel, pE );
    pModel->ExcludeObject( pObject );
    pE->Node( 2 )->ExcludeElement( pE );
    pE->Node( 1 )->ExcludeElement( pE );
    pE->FreeVars();
    if( pE->IsInTarget() )
      pModel->SetChangeFlag( CHF::TARGETS );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAE );
    return 0;
  }

  int ActChangeElem::Do()
  {
    SENDER_NAME( "ActChangeElem::Do" );
    if( pModel->IsLocked() )
      return 0;

    ElementPtr pE = dgtype_cast< ElementPtr >( pObject );

    if( pE->IsLocked() && ( pE->Node(1) != n1 || pE->Node(2) != n2 ) )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    assert( id <= pModel->MaxElemId() );
    NodePtr pOldNode1 = pE->Node( 1 );
    NodePtr pOldNode2 = pE->Node( 2 );
    int oldId = pE->Id();
    ActChangeElem* pACE = new ActChangeElem( pModel, pE, pOldNode1, pOldNode2, oldId );
    pE->Node(1)->ExcludeElement( pE );
    pE->Node(2)->ExcludeElement( pE );
    pE->SetParams( n1, n2, id );
    pE->Node(1)->IncludeElement( pE );
    pE->Node(2)->IncludeElement( pE );
    /* a->maxElemId=dg_max(a->maxElemId,ar->e->id); -- obsolete */
    if( pE->IsInTarget() )
      pModel->SetChangeFlag( CHF::TARGETS );
    pModel->ActionStack().AddUndoRec( pACE );

    pModel->Agent()->AddToUpdate( pE->Node(1), UT::UPDATE );
    pModel->Agent()->AddToUpdate( pE->Node(2), UT::UPDATE );

    std::StringList args;
    if( pOldNode1 != n1 ) {
      std::stringstream ss;
      ss << "node 1" << ": " << pOldNode1->Position().ToString()
                   << " -> " << n1->Position().ToString();
      args.push_back( ss.str() );
    }
    if( pOldNode2 != n2 ) {
      std::stringstream ss;
      ss << "node 2" << ": " << pOldNode2->Position().ToString()
                   << " -> " << n2->Position().ToString();
      args.push_back( ss.str() );
    }
    if( oldId != id ) {
      std::stringstream ss;
      ss << "id" << ": " << oldId << " -> " << id;
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );

    return 0;
  }

  int ActAddTemplate::Do()
  {
    SENDER_NAME( "ActAddTemplate" );
    if( pModel->IsLocked() )
      return 0;

    assert( !pModel->HasTemplate() );
    TemplatePtr pT = dgtype_cast< TemplatePtr >( pObject );

    int retCode = 0;
    switch( DetectFileType( pT->FileName() ) ) {
    case FT_DG_TEMPLATE:
      retCode = pT->LoadDgTemplateFile();
      if( retCode != 0 )
        return retCode;
      break;
    case FT_2COLUMNS:
      retCode = pT->Load2ColumnsTemplateFile();
      if( retCode != 0 )
        return retCode;
      break;
    case FT_OGR_TEMPLATE:
      retCode = pT->LoadOgrTemplateFile();
      if( retCode != 0 )
        return retCode;
      break;
    case FT_HPGL:
      retCode = pT->LoadHpglTemplateFile();
      if( retCode != 0 )
        return retCode;
      break;
    case FT_NOTFOUND:
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::FILENOTFOUND, ARGS( pT->FileName() ) );
    default:
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADFILETYPE, ARGS( pT->FileName() ) );
    }

    ActDelTemplate* pADT = new ActDelTemplate( pModel, pT );
    pModel->SetTemplate( pT );
    pModel->ActionStack().AddUndoRec( pADT );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  ActChangeTemplate::ActChangeTemplate( ModelPtr _pModel, TemplatePtr _pTemplate, const TemplateParams& _tp, bool doAtOnce ):
    IAction( _pModel, (IComponentPtr)_pTemplate, ACT::CHANGE_TEMPLATE )
    { pTP = new TemplateParams( _tp ); AT_ONCE }

  ActChangeTemplate::~ActChangeTemplate()
  { delete pTP; }

  int ActChangeTemplate::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    TemplatePtr pT = dgtype_cast< TemplatePtr >( pObject );
    TemplateParams oldTP = pT->GetParameters();
    ActChangeTemplate* pACT = new ActChangeTemplate( pModel, pT, oldTP );
    pT->SetParameters( *pTP );
    pModel->ActionStack().AddUndoRec( pACT );

    std::StringList args;
    if( oldTP.incr != pTP->incr ) {
      std::stringstream ss;
      ss << "incr" << ": " << oldTP.incr.ToString()
                 << " -> " << pTP->incr.ToString();
      args.push_back( ss.str() );
    }
    if( oldTP.scale != pTP->scale ) {
      std::stringstream ss;
      ss << "scale" << ": " << oldTP.scale << " -> " << pTP->scale;
      args.push_back( ss.str() );
    }
    if( oldTP.angle != pTP->angle ) {
      std::stringstream ss;
      ss << "angle" << ": " << oldTP.angle << " -> " << pTP->angle;
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );

    return 0;
  }

  int ActDelTemplate::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    TemplatePtr pT = dgtype_cast< TemplatePtr >( pObject );
    ActAddTemplate* pAAT = new ActAddTemplate( pModel, pT );
    pModel->SetTemplate( null );
    pT->FreePoints();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAT );
    return 0;
  }
}
