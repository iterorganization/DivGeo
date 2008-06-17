#ifndef _actions_h
#define _actions_h

/* Actions - reversible routines. All changes are made via actions
   to ensure that the Undo command can reverse the effect of any operation.
   An action routine is responsinble for creating corresponding undo record.
*/

/* Field id's for ActChangeObjString/Group/AppGroup:
   To reduce the number of actions and corresponding undo actions,
   there is only one action for all string operations that uses a field id
   as an additional parameter.
*/

/*
#define FI_NAME      1
#define FI_DESCR     2
#define FI_VAL       3
*/
/*
#define FI_MARK      4
#define FI_HIGHLIGHT 5
*/
/*
#define FI_GVAL      6
#define FI_FNAME     7
#define FI_HELP      8
#define FI_CREATIME  9
#define FI_ENUM      10
*/

/* Modes for ActChangeAppView:
   CVM_NEWWINDOW means another View has become active. Used only to produce
   undo information.
   CVM_OLDWINDOW is the undo operation for CVM_NEWWINDOW.
   CVM_ZOOM means the current View was changed by the Zoom or other command.
*/

#define CVM_NEWWINDOW 1
#define CVM_ZOOM      2
#define CVM_OLDWINDOW 3

/* General action record. All the Add... actions take ActRec as parameter.
   All the action records are compatible with ActRec. obj!=NULL means
   the action is going to create an object. When ActRec is used as an undo
   record, actProc should point to the corresponding action procedure.
   When an undo record containing obj!=NULL is freed, obj is also freed.
   If actProc==(ActProc)ActChangeObjGroup, obj is freed via FreeGroup()
   instead of Free().
*/
struct _ActRec {
  ActProc actProc;
  void* obj;
};

/* General action record for Del... actions. obj==NULL; delete points
   to the object to be deleted.
*/

typedef struct _DelRec {
  ActProc actProc;
  void* obj;
#ifndef __cplusplus
  void* delete;
#else
  void* objectToDelete;
#endif
}* DelRec;

/* UndoMarkRec is used as a separator between groups of undo actions.
   Each command the user executes creates one such group.
*/
typedef ActRec UndoMarkRec;

/* Any changes to Malloc'ed strings in objects are made via
   ChangeObjStringRec/ActChangeObjString. For a normal operation,
   the object to be changed should contain its type id as the first
   field in the object structure. fieldId specifies the field in the
   object structure to be changed; objChange points to the object; obj
   points to the string. obj==NULL means the current string should be
   deleted.
*/
typedef struct _ChangeObjStringRec {
  ActProc actProc;
  void* obj;
  void* objChange;
  int fieldOffset,bRedraw;
}* ChangeObjStringRec;

typedef struct _ChangeObjGroupRec {
  ActProc actProc;
  void* obj;
  void* objChange;
  void* member;
  int fieldOffset,bRedraw;
  int status;
}* ChangeObjGroupRec;

typedef struct _ChangeObjIntRec {
  ActProc actProc;
  void* obj;
  void* objChange;
  int fieldOffset,length,bRedraw;
  long value;
}* ChangeObjIntRec;

typedef struct _ChangeObjDoubleRec {
  ActProc actProc;
  void* obj;
  void* objChange;
  int fieldOffset,length,bRedraw;
  double value;
}* ChangeObjDoubleRec;

typedef struct _ChangeObjPointerRec {
  ActProc actProc;
  void* obj;
  void* objChange;
  int fieldOffset,length,bRedraw;
  void* value;
}* ChangeObjPointerRec;

/* ActChangeAppView is called whenever another View becomes active or
   the user changes zoom factor or showFlags. Its main purpose is to
   ensure the Undo command can reverse effects of such user actions.
*/
typedef struct _ChangeAppViewRec {
  ActProc actProc;
  void* obj;
  int mode;
  double minX,minY,maxX,maxY;
  unsigned long showFlags;
  double xyAngle;
}* ChangeAppViewRec;

/* Following are action records for various action routines
*/

typedef ActRec AddNodeRec;

typedef struct _DelNodeRec {
  ActProc actProc;
  void* obj;
  Node n;
}* DelNodeRec;

typedef struct _ChangeNodeRec {
  ActProc actProc;
  void* obj;
  Node n;
  double x,y,z;
}* ChangeNodeRec;

typedef ActRec AddElemRec;

typedef struct _DelElemRec {
  ActProc actProc;
  void* obj;
  Elem e;
}* DelElemRec;

typedef struct _ChangeElemRec {
  ActProc actProc;
  void* obj;
  Elem e;
  Node n1,n2;
  int id;
}* ChangeElemRec;

typedef ActRec AddEquilRec;

typedef struct _DelEquilRec {
  ActProc actProc;
  void* obj;
  Equil eq;
}* DelEquilRec;

typedef ActRec AddTemplateRec;

typedef struct _DelTemplateRec {
  ActProc actProc;
  void* obj;
  Template t;
}* DelTemplateRec;

typedef struct _ChangeTemplateRec {
  ActProc actProc;
  void* obj;
  Template t;
  double xIncr,yIncr,angle,scale;
}* ChangeTemplateRec;

typedef ActRec AddVarSetDefRec;
typedef DelRec DelVarSetDefRec;

typedef struct _ChangeVarSetDefRec {
  ActProc actProc;
  void* obj;
  VarSetDef vsd;
  int minVarSets,maxVarSets;
}* ChangeVarSetDefRec;

typedef ActRec AddVarDefRec;
typedef DelRec DelVarDefRec;

typedef struct _ChangeVarDefRec {
  ActProc actProc;
  void* obj;
  VarDef vd;
  VarSetDef vsd;
  int varType,flags,row,col;
}* ChangeVarDefRec;

typedef ActRec AddVarSetRec;
typedef DelRec DelVarSetRec;

typedef ActRec AddVarRec;
typedef DelRec DelVarRec;

typedef ActRec AddSonnetDataRec;
typedef DelRec DelSonnetDataRec;

typedef struct _LockAppUpdateRec {
  ActProc actProc;
  void* obj;
  int updateLocks;
  int bRepaintAfter;
}* LockAppUpdateRec;

typedef ActRec AddSeparatorRec;
typedef DelRec DelSeparatorRec;

typedef struct _ChangeSeparatorRec {
  ActProc actProc;
  void* obj;
  Separator sep;
  double x,y;
  Node n;
  int id;
}* ChangeSeparatorRec;

typedef ActRec AddSourceRec;
typedef DelRec DelSourceRec;
typedef struct _ChangeSourceRec {
  ActProc actProc;
  void* obj;
  Source src;
  double x,y;
}* ChangeSourceRec;

int ActUndoMark(App a,ActRec ar);

/* Operations with nodes - locked nodes cause fatal errors
   Nodes with elements or separators connected cannot be removed and cause
   fatal errors.
*/
int ActAddNode(App a,AddNodeRec ar);
int ActDelNode(App a,DelNodeRec ar);
int ActChangeNode(App a,ChangeNodeRec ar);

/* Operations with elements - locked elements cause fatal errors.
   Exception: the id of a locked element can be changed.
*/
int ActAddElem(App a,AddElemRec ar);
int ActDelElem(App a,DelElemRec ar);
int ActChangeElem(App a,ChangeElemRec ar);

/* Equilibrium
*/
int ActAddEquil(App a,AddEquilRec ar);
int ActDelEquil(App a,DelEquilRec ar);

/* Template. ChangeTemplate change only template position, not filename.
   ActAddTemplate returns error when problems with file I/O occur.
*/
int ActAddTemplate(App a,AddTemplateRec ar);
int ActDelTemplate(App a,DelTemplateRec ar);
int ActChangeTemplate(App a,ChangeTemplateRec ar);

/* Change strings/groups in objects
*/
int ActChangeObjString(App a,ChangeObjStringRec ar);
int ActChangeObjGroup(App a,ChangeObjGroupRec ar);
int ActChangeObjInt(App a,ChangeObjIntRec ar);
int ActChangeObjDouble(App a,ChangeObjDoubleRec ar);
int ActChangeObjPointer(App a,ChangeObjPointerRec ar);

/* Manipulates VarSet(layer) definitions. Locked VarSetDefs cause fatal errors.
   No error checking for minVarSets/maxVarSets.
*/
int ActAddVarSetDef(App a,AddVarSetDefRec ar);
int ActDelVarSetDef(App a,DelVarSetDefRec ar);
int ActChangeVarSetDef(App a,ChangeVarSetDefRec ar);

/* Manipulates variable definitions. Locked VarDefs cause fatal errors.
   Moves variables between VarSets/-Defs when necessary
*/
int ActAddVarDef(App a,AddVarDefRec ar);
int ActDelVarDef(App a,DelVarDefRec ar);
int ActChangeVarDef(App a,ChangeVarDefRec ar);

/* Manipulates VarSet definitions
*/
int ActAddVarSet(App a,AddVarSetRec ar);
int ActDelVarSet(App a,DelVarSetRec ar);

/* Manipulates variables
*/
int ActAddVar(App a,AddVarRec ar);
int ActDelVar(App a,DelVarRec ar);

int ActChangeAppView(App a,ChangeAppViewRec ar);

int ActAddSonnetData(App a,AddSonnetDataRec ar);
int ActDelSonnetData(App a,DelSonnetDataRec ar);

int ActLockAppUpdate(App a,LockAppUpdateRec ar);

int ActAddSeparator(App a,AddSeparatorRec ar);
int ActDelSeparator(App a,DelSeparatorRec ar);
int ActChangeSeparator(App a,ChangeSeparatorRec ar);

int ActAddSource(App a,AddSourceRec ar);
int ActDelSource(App a,DelSourceRec ar);
int ActChangeSource(App a,ChangeSourceRec ar);

#endif
