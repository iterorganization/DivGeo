#include "dg.h"

/* Low-level action routines. No error checking
*/

#ifdef __TURBOC__
#pragma argsused
#endif
int ActUndoMark(App a,ActRec ar) {
  ActRec ur;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActUndoMark);
  AddUndoRec(a,ur);
  return 0;
}

int ActAddNode(App a,AddNodeRec ar) {
  DelNodeRec ur;
  Node n;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelNode);
  ur->n=ar->obj;

  n=ar->obj;
  ar->obj=NULL;

  NotifyAdd(a,n);
  DrawAppHighlight(a,DRAW_OFF);

  n->elems=CreateGroup();
  n->separators=CreateGroup();
  GroupAdd(a->nodes,n);

  DrawAppObject(a,n,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,n);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelNode(App a,DelNodeRec ar) {
  ActRec ur;

  if (AppLocked(a)) return 0;

  if (ar->n->locks) FatalError("ActDelNode()-locks: fatal error 2");
  if (!IsEmptyGroup(ar->n->elems))
    FatalError("ActDelNode()-elems: fatal error 1");
  if (!IsEmptyGroup(ar->n->separators))
    FatalError("ActDelNode()-separators: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddNode);
  ur->obj=ar->n;

  NotifyDel(a,ar->n);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->n,DRAW_ERASE);

  ar->n->separators=FreeGroup(ar->n->separators);
  ar->n->elems=FreeGroup(ar->n->elems);
  GroupDel(a->nodes,ar->n);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->n);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int ActChangeNode(App a,ChangeNodeRec ar) {
  ChangeNodeRec ur;
  Index ix;
  Elem e;
  Separator sep;

  if (AppLocked(a)) return 0;

  if (ar->n->locks) FatalError("ActChangeNode()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeNode);
  ur->n=ar->n;
  ur->x=ar->n->x;
  ur->y=ar->n->y;
  ur->z=ar->n->z;

  NotifyChange(a,ar->n);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->n,DRAW_OFF);
  for (sep=Group1st(ar->n->separators,&ix);sep!=NULL;sep=Next(&ix))
    DrawAppObject(a,sep,DRAW_OFF);
  for (e=NodeElem1st(ar->n,&ix);e!=NULL;e=Next(&ix))
    DrawAppObject(a,e,DRAW_OFF);

  ar->n->x=ar->x;
  ar->n->y=ar->y;
  ar->n->z=ar->z;

  for (e=NodeElem1st(ar->n,&ix);e!=NULL;e=Next(&ix))
    DrawAppObject(a,e,DRAW_ON);
  for (sep=Group1st(ar->n->separators,&ix);sep!=NULL;sep=Next(&ix))
    DrawAppObject(a,sep,DRAW_ON);
  DrawAppObject(a,ar->n,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->n);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddElem(App a,AddElemRec ar) {
  DelElemRec ur;
  Elem e;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelElem);
  ur->e=ar->obj;

  e=ar->obj;
  ar->obj=NULL;

  assert(e->app==a);
  assert(e->id<=a->maxElemId);

  NotifyAdd(a,e);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,e->n[1],DRAW_OFF);
  DrawAppObject(a,e->n[2],DRAW_OFF);

  e->vars=CreateGroup();
  e->varsContaining=CreateGroup();

  GroupAdd(e->n[1]->elems,e);
  GroupAdd(e->n[2]->elems,e);
  GroupAdd(a->elems,e);
  /*a->maxElemId=max(a->maxElemId,e->id); -- obsolete */

  DrawAppObject(a,e,DRAW_ON);
  DrawAppObject(a,e->n[2],DRAW_ON);
  DrawAppObject(a,e->n[1],DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,e);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelElem(App a,DelElemRec ar) {
  AddElemRec ur;

  if (AppLocked(a)) return 0;

  if (ar->e->locks)
    FatalError("ActDelElem()-locks: fatal error 1");
  assert(IsEmptyGroup(ar->e->vars));
  assert(IsEmptyGroup(ar->e->varsContaining));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddElem);
  ur->obj=ar->e;

  NotifyDel(a,ar->e);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->e->n[1],DRAW_OFF);
  DrawAppObject(a,ar->e->n[2],DRAW_OFF);
  DrawAppObject(a,ar->e,DRAW_ERASE);

  GroupDel(a->elems,ar->e);
  GroupDel(ar->e->n[2]->elems,ar->e);
  GroupDel(ar->e->n[1]->elems,ar->e);

  ar->e->varsContaining=FreeGroup(ar->e->varsContaining);
  ar->e->vars=FreeGroup(ar->e->vars);

  DrawAppObject(a,ar->e->n[2],DRAW_ON);
  DrawAppObject(a,ar->e->n[1],DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->e);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int ActChangeElem(App a,ChangeElemRec ar) {
  ChangeElemRec ur;

  if (AppLocked(a)) return 0;

  if (ar->e->locks && (ar->e->n[1] != ar->n1 || ar->e->n[2] != ar->n2))
    FatalError("ActChangeElem()-locks: fatal error 1");
  assert(ar->id<=a->maxElemId);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeElem);
  ur->e=ar->e;
  ur->n1=ar->e->n[1];
  ur->n2=ar->e->n[2];
  ur->id=ar->e->id;

  NotifyChange(a,ar->e);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->e->n[1],DRAW_OFF);
  DrawAppObject(a,ar->e->n[2],DRAW_OFF);
  if (ar->n1!=ar->e->n[1] && ar->n1!=ar->e->n[2])
    DrawAppObject(a,ar->n1,DRAW_OFF);
  if (ar->n2!=ar->e->n[1] && ar->n2!=ar->e->n[2])
    DrawAppObject(a,ar->n2,DRAW_OFF);
  DrawAppObject(a,ar->e,DRAW_OFF);

  GroupDel(ar->e->n[1]->elems,ar->e);
  GroupDel(ar->e->n[2]->elems,ar->e);
  ar->e->n[1]=ar->n1;
  ar->e->n[2]=ar->n2;
  ar->e->id=ar->id;
  GroupAdd(ar->e->n[1]->elems,ar->e);
  GroupAdd(ar->e->n[2]->elems,ar->e);
  /* a->maxElemId=max(a->maxElemId,ar->e->id); -- obsolete */

  DrawAppObject(a,ar->e,DRAW_ON);
  if (ur->n2!=ar->e->n[1] && ur->n2!=ar->e->n[2])
    DrawAppObject(a,ur->n2,DRAW_ON);
  if (ur->n1!=ar->e->n[1] && ur->n1!=ar->e->n[2])
    DrawAppObject(a,ur->n1,DRAW_ON);
  DrawAppObject(a,ar->e->n[2],DRAW_ON);
  DrawAppObject(a,ar->e->n[1],DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->e);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddEquil(App a,AddEquilRec ar) {
  DelEquilRec ur;
  Equil eq;

  if (AppLocked(a)) return 0;

  assert(a->equil==NULL);

  eq=ar->obj;
  eq->x=eq->y=eq->val=NULL;

/* Do not create the UndoAction until sure that no error occured
*/
  switch (DetectFileType(eq->fName)) {
    case FT_DG_EQUIL:
      if (LoadDgEquilFile(eq)) return ERR_BADFILE;
      break;
    case FT_1_EQUIL:
      if (Load1EquilFile(eq)) return ERR_BADFILE;
      break;
    case FT_NOTFOUND:
      return ERR_FILENOTFOUND;
    default:
      return ERR_BADFILETYPE;
  }
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelEquil);
  ur->eq=eq;

  NotifyAdd(a,eq);
  DrawAppHighlight(a,DRAW_OFF);

  CalcEquilValues(eq);
  a->equil=eq;

  DrawAppObject(a,a->equil,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,eq);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelEquil(App a,DelEquilRec ar) {
  AddEquilRec ur;

  if (AppLocked(a)) return 0;

  if (ar->eq->locks) FatalError("ActDelEquil()-locks: fatal error 1");

  if (ar->eq->sspline!=NULL || ar->eq->hSplines!=NULL || ar->eq->vSplines!=NULL)
    FreeEquilSplines(ar->eq);

  if (ar->eq->cache!=NULL) FreeEquilCache(ar->eq);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddEquil);
  ur->obj=ar->eq;

  NotifyDel(a,ar->eq);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->eq,DRAW_ERASE);

  a->equil=NULL;
  ar->eq->x=Free(ar->eq->x);
  ar->eq->y=Free(ar->eq->y);
  ar->eq->val=Free(ar->eq->val);
  ar->eq->sx=ar->eq->sy=0;

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->eq);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddTemplate(App a,AddTemplateRec ar) {
  Template t;
  DelTemplateRec ur;
  int i;

  if (AppLocked(a)) return 0;

  assert(a->template==NULL);

  t=ar->obj;

  switch(DetectFileType(t->fName)) {
    case FT_DG_TEMPLATE:
      i=LoadDgTemplateFile(t);
      if (i) return i;
      break;
    case FT_2COLUMNS:
      i=Load2ColumnsTemplateFile(t);
      if (i) return i;
      break;
    case FT_OGR_TEMPLATE:
      i=LoadOgrTemplateFile(t);
      if (i) return i;
      break;
    case FT_HPGL:
      i=LoadHpglTemplateFile(t);
      if (i) return i;
      break;
    case FT_NOTFOUND:
      return ERR_FILENOTFOUND;
    default:
      return ERR_BADFILETYPE;
  }
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelTemplate);
  ur->t=t;

  NotifyAdd(a,t);
  DrawAppHighlight(a,DRAW_OFF);

  a->template=t;

  DrawAppObject(a,a->template,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,t);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelTemplate(App a,DelTemplateRec ar) {
  AddTemplateRec ur;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddTemplate);
  ur->obj=ar->t;

  NotifyDel(a,ar->t);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->t,DRAW_ERASE);

  a->template=NULL;
  ar->t->points=FreeMallocedGroup(ar->t->points);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->t);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int ActChangeTemplate(App a,ChangeTemplateRec ar) {
  ChangeTemplateRec ur;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeTemplate);
  ur->t=ar->t;
  ur->xIncr=ar->t->xIncr;
  ur->yIncr=ar->t->yIncr;
  ur->angle=ar->t->angle;
  ur->scale=ar->t->scale;

  NotifyChange(a,ar->t);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->t,DRAW_OFF);

  ar->t->xIncr=ar->xIncr;
  ar->t->yIncr=ar->yIncr;
  ar->t->angle=ar->angle;
  ar->t->scale=ar->scale;

  DrawAppObject(a,ar->t,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->t);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddVarSetDef(App a,AddVarSetDefRec ar) {
  VarSetDef vsd;
  DelVarSetDefRec ur;

  if (AppLocked(a)) return 0;

  vsd=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelVarSetDef);
  ur->delete=vsd;

  NotifyAdd(a,vsd);
  DrawAppHighlight(a,DRAW_OFF);

  vsd->varDefs=CreateGroup();
  vsd->vars=CreateGroup();
  vsd->varSets=CreateGroup();
  GroupAdd(a->varSetDefs,vsd);

  DrawAppObject(a,vsd,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,vsd);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelVarSetDef(App a,DelVarSetDefRec ar) {
  VarSetDef vsd;
  AddVarSetDefRec ur;

  if (AppLocked(a)) return 0;

  vsd=ar->delete;
  if (vsd->locks)
    FatalError("ActDelVarSetDef()-locks: fatal error 1");
  assert(IsEmptyGroup(vsd->varSets));
  assert(IsEmptyGroup(vsd->varDefs));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddVarSetDef);
  ur->obj=vsd;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,vsd,DRAW_ERASE);

  GroupDel(a->varSetDefs,vsd);
  vsd->varSets=FreeGroup(vsd->varSets);
  vsd->vars=FreeGroup(vsd->vars);
  vsd->varDefs=FreeGroup(vsd->varDefs);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeVarSetDef(App a,ChangeVarSetDefRec ar) {
  ChangeVarSetDefRec ur;

  if (AppLocked(a)) return 0;

  if (ar->vsd->locks)
    FatalError("ActChangeVarSetDef()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeVarSetDef);
  ur->vsd=ar->vsd;
  ur->minVarSets=ar->vsd->minVarSets;
  ur->maxVarSets=ar->vsd->maxVarSets;

  NotifyChange(a,ar->vsd);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->vsd,DRAW_OFF);

  ar->vsd->minVarSets=ar->minVarSets;
  ar->vsd->maxVarSets=ar->maxVarSets;

  DrawAppObject(a,ar->vsd,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->vsd);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}


int ActAddVarDef(App a,AddVarDefRec ar) {
  VarDef vd;
  DelVarDefRec ur;

  if (AppLocked(a)) return 0;

  vd=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelVarDef);
  ur->delete=vd;

  NotifyAdd(a,vd);
  DrawAppHighlight(a,DRAW_OFF);

  vd->name=NULL;
  vd->descr=NULL;
  vd->vars=CreateGroup();
  GroupAdd(vd->varSetDef->varDefs,vd);
  GroupAdd(a->varDefs,vd);

  DrawAppObject(a,vd,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,vd);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelVarDef(App a,DelVarDefRec ar) {
  VarDef vd;
  AddVarDefRec ur;

  if (AppLocked(a)) return 0;

  vd=ar->delete;
  if (vd->locks)
    FatalError("ActDelVarDef()-locks: fatal error 1");
  assert(IsEmptyGroup(vd->vars));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddVarDef);
  ur->obj=vd;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,vd,DRAW_ERASE);

  GroupDel(a->varDefs,vd);
  GroupDel(vd->varSetDef->varDefs,vd);
  vd->vars=FreeGroup(vd->vars);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeVarDef(App a,ChangeVarDefRec ar) {
  ChangeVarDefRec ur;

  if (AppLocked(a)) return 0;

  if (ar->vd->locks)
    FatalError("ActChangeVarDef()-locks: fatal error 1");

  assert(!((ar->vd->varType^ar->varType) & VTM_HASGROUP) ||
      IsEmptyGroup(ar->vd->vars));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeVarDef);
  ur->vd=ar->vd;
  ur->vsd=ar->vd->varSetDef;
  ur->varType=ar->vd->varType;
  ur->flags=ar->vd->flags;
  ur->col=ar->vd->col;
  ur->row=ar->vd->row;

  NotifyChange(a,ar->vd);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->vd,DRAW_OFF);

  GroupDel(ar->vd->varSetDef->varDefs,ar->vd);
  ar->vd->varSetDef=ar->vsd;
  ar->vd->varType=ar->varType;
  ar->vd->flags=ar->flags;
  ar->vd->col=ar->col;
  ar->vd->row=ar->row;
  GroupAdd(ar->vd->varSetDef->varDefs,ar->vd);

  DrawAppObject(a,ar->vd,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->vd);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}


int ActAddVarSet(App a,AddVarSetRec ar) {
  VarSet vs;
  DelVarSetRec ur;

  if (AppLocked(a)) return 0;

  vs=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelVarSet);
  ur->delete=vs;

  NotifyAdd(a,vs);
  DrawAppHighlight(a,DRAW_OFF);

  vs->vars=CreateGroup();
  GroupAdd(vs->def->varSets,vs);
  GroupAdd(a->varSets,vs);

  DrawAppObject(a,vs,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,vs);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelVarSet(App a,DelVarSetRec ar) {
  VarSet vs;
  AddVarSetRec ur;

  if (AppLocked(a)) return 0;

  vs=ar->delete;
  if (vs->locks)
    FatalError("ActDelVarSet()-locks: fatal error 1");
  assert(IsEmptyGroup(vs->vars));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddVarSet);
  ur->obj=vs;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,vs,DRAW_ERASE);

  GroupDel(a->varSets,vs);
  GroupDel(vs->def->varSets,vs);
  vs->vars=FreeGroup(vs->vars);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddVar(App a,AddVarRec ar) {
  DelVarRec ur;
  Var v;

  if (AppLocked(a)) return 0;

  v=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelVar);
  ur->delete=v;

  NotifyAdd(a,v);
  DrawAppHighlight(a,DRAW_OFF);

  if (v->def->varType & VTM_HASGROUP) v->val=CreateGroup();
  GroupAdd(v->def->vars,v);
  switch(GetObjType(v->origin)) {
    case T_VARSETDEF:
      GroupAdd(((VarSetDef)v->origin)->vars,v);
      break;
    case T_VARSET:
      GroupAdd(((VarSet)v->origin)->vars,v);
      break;
    case T_ELEM:
      GroupAdd(((Elem)v->origin)->vars,v);
      break;
    case T_SEPARATOR:
      GroupAdd(((Separator)v->origin)->vars,v);
      break;
    case T_SOURCE:
      GroupAdd(((Source)v->origin)->vars,v);
      break;
    case T_CHORD:
      GroupAdd(((Chord)v->origin)->vars,v);
      break;
    default:
      FatalError("ActAddVar()-type%d: fatal error 1",*(int*)v->origin);
  }

  DrawAppObject(a,v,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,v);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelVar(App a,DelVarRec ar) {
  AddVarRec ur;
  Var v;

  if (AppLocked(a)) return 0;

  v=ar->delete;
  if (v->locks)
    FatalError("ActDelVar()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddVar);
  ur->obj=v;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,v,DRAW_ERASE);

  switch(GetObjType(v->origin)) {
    case T_VARSETDEF:
      GroupDel(((VarSetDef)v->origin)->vars,v);
      break;
    case T_VARSET:
      GroupDel(((VarSet)v->origin)->vars,v);
      break;
    case T_ELEM:
      GroupDel(((Elem)v->origin)->vars,v);
      break;
    case T_SEPARATOR:
      GroupDel(((Separator)v->origin)->vars,v);
      break;
    case T_SOURCE:
      GroupDel(((Source)v->origin)->vars,v);
      break;
    case T_CHORD:
      GroupDel(((Chord)v->origin)->vars,v);
      break;
    default:
      FatalError("ActDelVar()-type%d: fatal error 1",*(int*)v->origin);
  }
  GroupDel(v->def->vars,v);
  if (v->def->varType & VTM_HASGROUP) {
    assert(IsEmptyGroup(v->val));
    v->val=FreeGroup(v->val);
  }

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeObjString(App a,ChangeObjStringRec ar) {
  ChangeObjStringRec ur;
  VarSetDef vsd;
  VarDef vd;
  Var v;
  char** ps;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeObjString);
  ur->objChange=ar->objChange;
  ur->fieldOffset=ar->fieldOffset;
  ur->bRedraw=ar->bRedraw;

  NotifyChange(a,ar->objChange);

  if (ar->bRedraw) {
    DrawAppHighlight(a,DRAW_OFF);
    DrawAppObject(a,ar->objChange,DRAW_OFF);
  }

  ps=(char**)((char*)ar->objChange+ar->fieldOffset);

  ur->obj=*ps;
  *ps=ar->obj;
  ar->obj=NULL;

  if (ar->bRedraw) {
    DrawAppObject(a,ar->objChange,DRAW_ON);
    DrawAppHighlight(a,DRAW_ON);
    AddAppUpdate(a);
  }

  NotifyChanged(a,ar->objChange);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeObjGroup(App a,ChangeObjGroupRec ar) {
  ChangeObjGroupRec ur;
  Var v;
  void* p;
  Elem e;
  Index ix;
  Group* pg;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeObjGroup);
  ur->objChange=ar->objChange;
  ur->member=ar->member;
  ur->fieldOffset=ar->fieldOffset;
  ur->status=!ar->status;
  ur->bRedraw=ar->bRedraw;

  if (a!=ar->objChange) NotifyChange(a,ar->objChange);


  if (ar->bRedraw) {
    DrawAppHighlight(a,DRAW_OFF);
    DrawAppObject(a,ar->objChange,DRAW_OFF);
    DrawAppObject(a,ar->member,DRAW_OFF);
  }

  pg=(Group*)((char*)ar->objChange+ar->fieldOffset);

  assert(pg!=NULL);

  ar->status ? GroupAdd(*pg,ar->member) : GroupDel(*pg,ar->member);

  if (ar->bRedraw) {
    DrawAppObject(a,ar->member,DRAW_ON);
    DrawAppObject(a,ar->objChange,DRAW_ON);
    DrawAppHighlight(a,DRAW_ON);
    AddAppUpdate(a);
  }

  if (a!=ar->objChange)NotifyChanged(a,ar->objChange);
  else if (ar->fieldOffset==GetOffset(App,mark)) NotifyMarked(a,ar->member);

  if (a==ar->objChange) AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeObjInt(App a,ChangeObjIntRec ar) {
  ChangeObjIntRec ur;
  void* pv;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeObjInt);
  ur->objChange=ar->objChange;
  ur->fieldOffset=ar->fieldOffset;
  ur->length=ar->length;
  ur->bRedraw=ar->bRedraw;

  NotifyChange(a,ar->objChange);

  if (ar->bRedraw) {
    DrawAppHighlight(a,DRAW_OFF);
    DrawAppObject(a,ar->objChange,DRAW_OFF);
  }

  pv=(void*)((char*)ar->objChange+ar->fieldOffset);

  switch (ar->length) {
    case sizeof(char):
      ur->value=*(char*)pv;
      *(char*)pv=(char)ar->value;
      break;
    case sizeof(int):
      ur->value=*(int*)pv;
      *(int*)pv=(int)ar->value;
      break;
/*    case sizeof(long):
      ur->value=*(long*)pv;
      *(long*)pv=ar->value;
      break; */
    default:
      assert(0);
  }

  if (ar->bRedraw) {
    DrawAppObject(a,ar->objChange,DRAW_ON);
    DrawAppHighlight(a,DRAW_ON);
    AddAppUpdate(a);
  }

  NotifyChanged(a,ar->objChange);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeObjPointer(App a,ChangeObjPointerRec ar) {
  ChangeObjPointerRec ur;
  void** pv;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeObjPointer);
  ur->objChange=ar->objChange;
  ur->fieldOffset=ar->fieldOffset;
  ur->length=ar->length;
  ur->bRedraw=ar->bRedraw;

  NotifyChange(a,ar->objChange);

  if (ar->bRedraw) {
    DrawAppHighlight(a,DRAW_OFF);
    DrawAppObject(a,ar->objChange,DRAW_OFF);
  }

  pv=(void**)((char*)ar->objChange+ar->fieldOffset);

  switch (ar->length) {
    case sizeof(void*):
      ur->value=*pv;
      *pv=ar->value;
      break;
    default:
      assert(0);
  }

  if (ar->bRedraw) {
    DrawAppObject(a,ar->objChange,DRAW_ON);
    DrawAppHighlight(a,DRAW_ON);
    AddAppUpdate(a);
  }

  NotifyChanged(a,ar->objChange);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeObjDouble(App a,ChangeObjDoubleRec ar) {
  ChangeObjDoubleRec ur;
  void* pv;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeObjDouble);
  ur->objChange=ar->objChange;
  ur->fieldOffset=ar->fieldOffset;
  ur->length=ar->length;
  ur->bRedraw=ar->bRedraw;

  NotifyChange(a,ar->objChange);

  if (ar->bRedraw) {
    DrawAppHighlight(a,DRAW_OFF);
    DrawAppObject(a,ar->objChange,DRAW_OFF);
  }

  pv=(void*)((char*)ar->objChange+ar->fieldOffset);

  switch (ar->length) {
    case sizeof(double):
      ur->value=*(double*)pv;
      *(double*)pv=ar->value;
      break;
    default:
      assert(0);
  }

  if (ar->bRedraw) {
    DrawAppObject(a,ar->objChange,DRAW_ON);
    DrawAppHighlight(a,DRAW_ON);
    AddAppUpdate(a);
  }

  NotifyChanged(a,ar->objChange);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

#ifdef OLDEQ
int ActAddXPoint(App a,AddXPointRec ar) {
  DelXPointRec ur;
  XPoint xpt;
  int i,cx1,cy1,cx2,cy2;
  Var t1,t2;
  GridPoint gp;
  Index ix;

  assert(0);

  if (AppLocked(a)) return 0;

  if (a->xpoint!=NULL) FatalError("ActAddXPoint()-!null: fatal error 1");
  ValidatePtr(a->equil,"ActAddXPoint.equil");

  xpt=ar->obj;
  if (GetEquilCell(a->equil,xpt->x1,xpt->y1,&cx1,&cy1))
    return ERR_OUTOFEQUIL;
  if (GetEquilCell(a->equil,xpt->x2,xpt->y2,&cx2,&cy2))
    return ERR_OUTOFEQUIL;
  t1=GetVarPtrByType(a,VT_TARGET1);
  t2=GetVarPtrByType(a,VT_TARGET2);
  if (CheckValue(a,t1->val,VT_TARGET1,NULL) ||
      CheckValue(a,t2->val,VT_TARGET2,NULL))
    return ERR_BADTARGETS;
  i=CalcXPointLine(a->equil,xpt,cx1,cy1,cx2,cy2,t1->val,t2->val);
  if (i) return i;
  LockTarget(t1,1);
  LockTarget(t2,1);
  /*LockObject(a->equil,1);*/
  ar->obj=NULL;
  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelXPoint);
  ur->delete=xpt;

  NotifyAdd(a,xpt);
  DrawAppHighlight(a,DRAW_OFF);

  a->xpoint=xpt;

  DrawAppObject(a,xpt,DRAW_ON);

  for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix)) {
    CalcGridPointCoords(xpt,gp);
    DrawAppObject(a,gp,DRAW_ON);
  }

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,xpt);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelXPoint(App a,DelXPointRec ar) {
  AddXPointRec ur;
  int i;
  Var t1,t2;
  GridPoint gp;
  Index ix;

  assert(0);

  if (AppLocked(a)) return 0;

  if (a->xpoint->locks)
    FatalError("ActDelXPoint()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddXPoint);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix))
    DrawAppObject(a,gp,DRAW_OFF);
  DrawAppObject(a,ar->delete,DRAW_ERASE);

  for (i=0;i<3;i++) a->xpoint->line[i]=FreeMallocedGroup(a->xpoint->line[i]);
  a->xpoint=NULL;
  /*LockObject(a->equil,-1);*/
  t1=GetVarPtrByType(a,VT_TARGET1);
  t2=GetVarPtrByType(a,VT_TARGET2);
  assert(!CheckValue(a,t1->val,VT_TARGET1,NULL));
  assert(!CheckValue(a,t2->val,VT_TARGET2,NULL));

  LockTarget(t1,-1);
  LockTarget(t2,-1);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}


int ActAddGridPoint(App a,AddGridPointRec ar) {
  GridPoint gp,gp1;
  DelGridPointRec ur;
  Index ix;

  if (AppLocked(a)) return 0;

  gp=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelGridPoint);
  ur->delete=gp;

  NotifyAdd(a,gp);
  DrawAppHighlight(a,DRAW_OFF);

  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>gp->area ||(gp1->area==gp->area && gp1->value>gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix);
  GroupAddAt(a->gridPoints,gp1,gp);

  if (a->xpoint!=NULL) {
    CalcGridPointCoords(a->xpoint,gp);
    DrawAppObject(a,gp,DRAW_ON);
  }

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,gp);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeGridPoint(App a,ChangeGridPointRec ar) {
  GridPoint gp1;
  ChangeGridPointRec ur;
  Index ix;

  if (AppLocked(a)) return 0;

  if (ar->gp->locks)
    FatalError("ActChangeGridPoint()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeGridPoint);
  ur->gp=ar->gp;
  ur->area=ar->gp->area;
  ur->value=ar->gp->value;

  NotifyChange(a,ar->gp);
  DrawAppHighlight(a,DRAW_OFF);

  if (a->xpoint!=NULL) DrawAppObject(a,ar->gp,DRAW_OFF);

  GroupDel(a->gridPoints,ar->gp);
  ar->gp->area=ar->area;
  ar->gp->value=ar->value;

  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>ar->gp->area ||
        (gp1->area==ar->gp->area && gp1->value>ar->gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix);
  GroupAddAt(a->gridPoints,gp1,ar->gp);

  if (a->xpoint!=NULL) {
    CalcGridPointCoords(a->xpoint,ar->gp);
    DrawAppObject(a,ar->gp,DRAW_ON);
  }

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->gp);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelGridPoint(App a,DelGridPointRec ar) {
  AddGridPointRec ur;
  GridPoint gp;

  if (AppLocked(a)) return 0;

  gp=ar->delete;

  if (gp->locks) FatalError("ActDelGridPoint()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddGridPoint);
  ur->obj=gp;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  if (a->xpoint!=NULL) DrawAppObject(a,gp,DRAW_ERASE);

  GroupDel(a->gridPoints,gp);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}
#endif

/* ChangeAppView: change zoom factor/flags
   Also used when switching between windows */

int ActChangeAppView(App a,ChangeAppViewRec ar) {
  ChangeAppViewRec ur=NULL;
  View w=a->activeAppView;

  if (AppLocked(a)) return 0;

  /* Preserve the undo information */

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeAppView);
  ur->mode= ar->mode==CVM_NEWWINDOW ? CVM_OLDWINDOW : ar->mode;
  ur->minX=a->minX;
  ur->minY=a->minY;
  ur->maxX=a->maxX;
  ur->maxY=a->maxY;
  ur->showFlags=a->showFlags;
  ur->xyAngle=a->xyAngle;

  if (w!=NULL) NotifyChange(a,w);
  DrawAppHighlight(a,DRAW_OFF);

  /* If a view is active, affect the current view.
     CVM_NEWWINDOW: in this case, the (new) view is already set up */

  if (ar->mode!=CVM_NEWWINDOW && w!=NULL && w->width && w->height) {

    /* Adjust arguments */

    if (ar->minX==ar->maxX || ar->minY==ar->maxY)
      FatalError("ActChangeAppView()-minmax: fatal error 1");
    if (ar->maxX<ar->minX) swap(ar->maxX,ar->minX);
    if (ar->maxY<ar->minY) swap(ar->maxY,ar->minY);

    /* Adjust the active view */

    w->centerX=(ar->minX+ar->maxX)/2;
    w->centerY=(ar->minY+ar->maxY)/2;
    w->zoomX=w->width/(ar->maxX-ar->minX);
    w->zoomY=w->height/(ar->maxY-ar->minY);
    if (!(ar->showFlags & SHW_STRETCH)) AdjustViewAspectRatio(w);
    a->minX=w->minX=w->centerX-w->width/2/w->zoomX;
    a->minY=w->minY=w->centerY-w->height/2/w->zoomY;
    a->maxX=w->maxX=w->centerX+w->width/2/w->zoomX;
    a->maxY=w->maxY=w->centerY+w->height/2/w->zoomY;
    a->showFlags=w->showFlags=ar->showFlags;
    a->xyAngle=w->xyAngle=ar->xyAngle;
    /* Repaint the active view */

    ClearView(w);
    RepaintView(w);
  } else {

    /* No view modifications - simply adjust values in App */

    a->minX=ar->minX;
    a->minY=ar->minY;
    a->maxX=ar->maxX;
    a->maxY=ar->maxY;
    a->showFlags=ar->showFlags;
    a->xyAngle=ar->xyAngle;
  }

  DrawAppHighlight(a,DRAW_ON);
  if (w!=NULL) NotifyChanged(a,w);

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddSonnetData(App a,AddSonnetDataRec ar) {
  SonnetData sd;
  DelSonnetDataRec ur;
  int i;

  if (AppLocked(a)) return 0;

  if (a->sonnetData!=NULL)
    FatalError("ActAddSonnetData()-!null: fatal error 1");

  assert(IsEmptyGroup(a->separators));

  sd=ar->obj;

  switch(DetectFileType(sd->fName)) {
    case FT_SONNET:
      i=LoadSonnetFile(sd);
      if (i) return i;
      break;
    case FT_NOTFOUND:
      return ERR_FILENOTFOUND;
    default:
      return ERR_BADFILETYPE;
  }
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelSonnetData);
  ur->delete=sd;

  NotifyAdd(a,sd);
  DrawAppHighlight(a,DRAW_OFF);

  a->sonnetData=sd;

  DrawAppObject(a,a->sonnetData,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,sd);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelSonnetData(App a,DelSonnetDataRec ar) {
  AddSonnetDataRec ur;
  SonnetData sd;

  if (AppLocked(a)) return 0;

  sd=ar->delete;
  assert(!sd->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddSonnetData);
  ur->obj=sd;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,sd,DRAW_ERASE);

  a->sonnetData=NULL;
  sd->points=Free(sd->points);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);

  AddUndoRec(a,ur);
  return 0;
}

int ActLockAppUpdate(App a,LockAppUpdateRec ar) {
  LockAppUpdateRec ur;
  View w;
  Index ix;

  if (AppLocked(a)) return 0;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActLockAppUpdate);
  ur->updateLocks=a->updateLocks;
  ur->bRepaintAfter=ar->bRepaintAfter;

  if (!ar->updateLocks) a->updateLocks=ar->updateLocks;

  if (ar->bRepaintAfter) for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix)) {
    if (ar->updateLocks && !ur->updateLocks) {
      ClearView(w);FlushView(w);
    }
    if (!ar->updateLocks && ur->updateLocks)  {
      RepaintView(w);
    }
  }
  if (ar->updateLocks) a->updateLocks=ar->updateLocks;

  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddSeparator(App a,AddSeparatorRec ar) {
  DelSeparatorRec ur;
  Separator sep;

  if (AppLocked(a)) return 0;

  ValidatePtr(a->sonnetData,"ActAddSeparator.sonnetData");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelSeparator);
  ur->delete=ar->obj;

  sep=ar->obj;
  ar->obj=NULL;

  assert(sep->id<=a->maxElemId);

  NotifyAdd(a,sep);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,sep->n,DRAW_OFF);

  sep->vars=CreateGroup();
  GroupAdd(sep->n->separators,sep);
  GroupAdd(a->separators,sep);
  /*LockObject(a->sonnetData,1);*/
  /* a->maxElemId=max(a->maxElemId,sep->id); -- obsolete */

  DrawAppObject(a,sep,DRAW_ON);
  DrawAppObject(a,sep->n,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,sep);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelSeparator(App a,DelSeparatorRec ar) {
  AddSeparatorRec ur;
  Separator sep;

  if (AppLocked(a)) return 0;

  sep=ar->delete;
  if (sep->locks)
    FatalError("ActDelSeparator()-locks: fatal error 1");
  assert(IsEmptyGroup(sep->vars));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddSeparator);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,sep->n,DRAW_OFF);
  DrawAppObject(a,sep,DRAW_ERASE);

  /*LockObject(a->sonnetData,-1);*/
  GroupDel(a->separators,sep);
  GroupDel(sep->n->separators,sep);
  sep->vars=FreeGroup(sep->vars);

  DrawAppObject(a,sep->n,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int ActChangeSeparator(App a,ChangeSeparatorRec ar) {
  ChangeSeparatorRec ur;

  if (AppLocked(a)) return 0;

  if (ar->sep->locks && (ar->x!=ar->sep->x || ar->y!=ar->sep->y ||
      ar->n!=ar->sep->n))
    FatalError("ActChangeSeparator()-locks: fatal error 1");

  assert(ar->id<=a->maxElemId);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeSeparator);
  ur->sep=ar->sep;

  ur->x=ar->sep->x;
  ur->y=ar->sep->y;
  ur->n=ar->sep->n;
  ur->id=ar->sep->id;

  NotifyChange(a,ar->sep);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->sep->n,DRAW_OFF);
  if (ar->n!=ar->sep->n)
    DrawAppObject(a,ar->n,DRAW_OFF);
  DrawAppObject(a,ar->sep,DRAW_OFF);

  GroupDel(ar->sep->n->separators,ar->sep);
  ar->sep->x=ar->x;
  ar->sep->y=ar->y;
  ar->sep->n=ar->n;
  ar->sep->id=ar->id;
  GroupAdd(ar->sep->n->separators,ar->sep);
  /* a->maxElemId=max(a->maxElemId,ar->sep->id); -- obsolete */

  DrawAppObject(a,ar->sep,DRAW_ON);
  if (ur->n!=ar->sep->n)
    DrawAppObject(a,ur->n,DRAW_ON);
  DrawAppObject(a,ar->sep->n,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->sep);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActAddSource(App a,AddSourceRec ar) {
  DelSourceRec ur;
  Source src;

  if (AppLocked(a)) return 0;

  src=ar->obj;
  ar->obj=NULL;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelSource);
  ur->delete=src;

  NotifyAdd(a,src);
  DrawAppHighlight(a,DRAW_OFF);

  src->vars=CreateGroup();
  GroupAdd(a->sources,src);

  DrawAppObject(a,src,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,src);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}


int ActDelSource(App a,DelSourceRec ar) {
  Source src;
  AddSourceRec ur;

  if (AppLocked(a)) return 0;

  src=ar->delete;
  assert(!src->locks);
  assert(IsEmptyGroup(src->vars));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddSource);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,src,DRAW_ERASE);

  GroupDel(a->sources,src);
  src->vars=FreeGroup(src->vars);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}


int ActChangeSource(App a,ChangeSourceRec ar) {
  ChangeSourceRec ur;

  if (AppLocked(a)) return 0;

  assert(!ar->src->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeSource);
  ur->src=ar->src;

  ur->x=ar->src->x;
  ur->y=ar->src->y;

  NotifyChange(a,ar->src);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->src,DRAW_OFF);

  ar->src->x=ar->x;
  ar->src->y=ar->y;

  DrawAppObject(a,ar->src,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->src);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

