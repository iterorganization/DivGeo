#include "ActionStack.h"
#include "Model.h"

void CActionStack::FreeList( StackType st )
{
  ActionList* pStack = null;
  switch( st ) {
  case ST_UNDO: pStack = &undoStack; break;
  case ST_REDO: pStack = &redoStack; break;
  default: throw "wrong stack type"; break;
  }

  if( pStack->empty() )
    return;

  FOREACH_CONST( ActionIterConst, it, *pStack ) {
    if( (*it) != NULL )
      delete (*it);
  }

  pStack->clear();
}

void CActionStack::FreeUndoInfo()
{
  FreeList( ST_UNDO );
  FreeList( ST_REDO );
}

void CActionStack::AddUndoRec( IAction* pAct )
{
  ActionList* pStack;
  ValidatePtr(pAct,"AddUndoRec_");

  // Update object
  if( HasAnyFlag( pAct->Type(), ACT::MASK_GROUP ) ) {
    switch( pAct->Type()&ACT::MASK_ACTION ) {
    case ACT::DEL_ACTION: pModel->Agent()->AddToUpdate( pAct->Object(), UT::CREATE ); break;
    case ACT::ADD_ACTION: pModel->Agent()->AddToUpdate( pAct->Object(), UT::REMOVE ); break;
    case ACT::CHANGE_ACTION: pModel->Agent()->AddToUpdate( pAct->Object(), UT::UPDATE ); break;
    default: break;
    }
  }
  else {
    if( pAct->Type() == ACT::MARK_OBJECT ||
        pAct->Type() == ACT::HIGHLIGHT_OBJECT )
      pModel->Agent()->AddToUpdate( pAct->Object(), UT::USTYLE );
  }

  // Add action
  if( undoMode == UM_NORM && !redoStack.empty() )
    FreeList( ST_REDO );

  if( undoMode == UM_NORM || undoMode == UM_REDO ) {
    pStack = &undoStack;
  }
  else if ( undoMode == UM_UNDO )
    pStack = &redoStack;
  else // UM_CANCEL
    return delete pAct;

  // obsolete UndoMark
  if( pAct->Type() == ACT::UNDO_MARK )
    return delete pAct;

  ActHighLevel* pHiAct = static_cast< ActHighLevel* >( pStack->front() );
  if( pStack->empty() || pHiAct->IsComplete() ) {
    pHiAct = new ActHighLevel( pModel );
    pStack->push_front( pHiAct );
  }
  pHiAct->AddAction( pAct );
}

void CActionStack::ProcessUndo()
{
  SENDER_NAME( "UndoStack::ProcessUndo" );
  ActionList* pStack = null;

  if( cancelToolFlag > 1 )
    return;
  cancelToolFlag++;

  if( !bNonDestructiveCancel ) {
    /* In v2 there was calling a TL_CANCEL part of tools.
     * In v3 undo stack can not check the tool state,
     * so you need call Cancel() or ProcessUndo() after
     * tool has been deleted */
  }

  cancelToolFlag--;

  // Find stack
  switch( undoMode ) {
  case UM_NORM:
    pModel->SendMessage( FATAL_ERROR, FULL_SENDER, DG3::WRONG_UNDO_MODE );
    break;
  case UM_UNDO:
  case UM_CANCEL: pStack = &undoStack; break;
  case UM_REDO:   pStack = &redoStack; break;
  default:
    break;
  }

  // Find action
  IAction* pAct = pStack->empty() ? null : pStack->front();
  if( pAct == null || pAct->Type() != ACT::HIGH_LEVEL )
    return;
  ActHighLevel* pHiAct = static_cast< ActHighLevel* >( pAct );

  // Process action
  if( pHiAct->IsComplete() && undoMode == UM_CANCEL )
    return;
  int oldMode = undoMode;

  if( !pHiAct->IsComplete() )
    undoMode = UM_CANCEL;

  pHiAct->Do();
  pStack->pop_front();

  if( undoMode != UM_CANCEL ) {
    std::string sDescr = pHiAct->Descr();

    // Switch stack
    if( pStack == &undoStack )
      pStack = &redoStack;
    else
      pStack = &undoStack;

    // Set opposite action completed
    ActHighLevel* pHiActOposite = static_cast< ActHighLevel* >( pStack->front() );
    pHiActOposite->SetComplete( sDescr );
  }

  // Ouput
  pModel->SendMessage( LOG_WIP_INFO, FULL_SENDER,
                       DG3::ACTIONS_REVERTED, ARG( pHiAct->Size() ) );

  if( !pHiAct->IsComplete() )
    undoMode = oldMode;

  // Remove action
  delete pAct;

  // Additional changes
  pModel->ProcessChanges();
  pModel->Agent()->Update();
}

void CActionStack::Undo()
{
  assert( !bNonDestructiveCancel );

  int tmpUm = undoMode;
  undoMode = UM_UNDO;
  ProcessUndo();
  undoMode = tmpUm;
}

void CActionStack::Redo()
{
  assert( !bNonDestructiveCancel );

  int tmpUm = undoMode;
  undoMode = UM_REDO;
  ProcessUndo();
  undoMode = tmpUm;
}

void CActionStack::RedoAll()
{
  while( !redoStack.empty() )
    Redo();
}

void CActionStack::Cancel()
{
  assert( !bNonDestructiveCancel );

  int tmpUm = undoMode;
  undoMode = UM_CANCEL;
  pModel->Agent()->SetLoggingMode( LM::IGNORE );
  ProcessUndo();
  pModel->Agent()->SetLoggingMode( LM::USUAL );
  undoMode = tmpUm;
}

void CActionStack::Complete( const std::string& _crsDescr )
{
  ActHighLevel* pAct = static_cast< ActHighLevel* >( undoStack.front() );
  if( pAct == null )
    pAct = new ActHighLevel( pModel, _crsDescr );
  pAct->SetComplete( _crsDescr );
  pModel->ProcessChanges();
  pModel->Agent()->Update();
}

bool CActionStack::HasUndoRecords() const
{
  return !undoStack.empty();
}
