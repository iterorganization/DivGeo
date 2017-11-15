#ifndef ACTIONSTACK_H
#define ACTIONSTACK_H

#include "Actions.h"
using namespace Actions;

enum UndoMode
{
  UM_NORM = 1,
  UM_UNDO = 2,
  UM_REDO = 3,
  UM_CANCEL = 4
};

enum StackType
{
  ST_UNDO,
  ST_REDO
};

class CActionStack
{
  std::string sender_name;

  ModelPtr pModel;

  int undoMode;
  int cancelToolFlag;
  bool bNonDestructiveCancel;

  ActionList undoStack;
  ActionList redoStack;

private:

  void FreeList( StackType st );
  void ProcessUndo();

public:

  CActionStack( ModelPtr _pModel ):
    sender_name( "UndoStack" ),
    pModel( _pModel ),
    undoMode( UM_NORM ),
    cancelToolFlag( 0 ),
    bNonDestructiveCancel( false )
  { }

  ~CActionStack() { FreeList( ST_UNDO ); FreeList( ST_REDO ); }

  void AddUndoRec( IAction* pAct );
  void FreeUndoInfo();
  void Undo();
  void Redo();
  void RedoAll();
  void Cancel();
  void Complete( const std::string& _crsDescr = "" );

  bool HasUndoRecords() const;

  void EnableUndo() { undoMode = UM_NORM; FreeUndoInfo(); }
  void DisableUndo() { undoMode = UM_CANCEL; FreeUndoInfo(); }

  const ActionList& UndoStack() const { return undoStack; }
  const ActionList& RedoStack() const { return redoStack; }
};

#endif
