#include "dg.h"

Node AddNode(App a,double x,double y) {
  Node n;
  struct _ActRec ar;

  ValidatePtr(a,"AddNode");

  n=Malloc(sizeof(*n));
  n->type=T_NODE;
  n->x=x;n->y=y;n->z=0;
  n->locks=0;

  ar.obj=n;
  ActAddNode(a,&ar);

  return n;
}

void* DelNode(App a,Node n) {
  struct _DelNodeRec ar;

  ValidatePtr(a,"DelNode");
  ValidatePtr(n,"DelNode_");
  if (IsHighlighted(a,n)) FatalError("DelNode()-highlight: fatal error 1");

  ar.n=n;
  ActDelNode(a,&ar);

  return NULL;
}

void ChangeNode(App a,Node n,double x,double y) {
  struct _ChangeNodeRec ar;

  ValidatePtr(a,"ChangeNode");
  ValidatePtr(n,"ChangeNode_");

  ar.n=n;
  ar.x=x;
  ar.y=y;
  ar.z=0;
  ActChangeNode(a,&ar);
}


Elem AddElem(App a,Node n1,Node n2) {
  Elem e;
  struct _ActRec ar;

  ValidatePtr(a,"AddElem");
  ValidatePtr(n1,"AddElem_");
  ValidatePtr(n2,"AddElem__");
  if (n1==n2) FatalError("AddElem()-sameNodes: fatal error 1");
  if (NodesConnected(n1,n2)) FatalError("AddElem()-connect: fatal error !");

  e=Malloc(sizeof(*e));
  e->type=T_ELEM;
  e->app=a;
  e->n[0]=NULL;
  e->n[1]=n1;
  e->n[2]=n2;
  e->id=GetNextElemId(a);
  e->locks=0;

  ar.obj=e;
  ActAddElem(a,&ar);

  return e;
}

void* DelElem(App a,Elem e) {
  struct _DelElemRec ar;
  Node n1,n2;
  Var v;
  Index ix;
  int i,id;

  ValidatePtr(a,"DelElem");
  ValidatePtr(e,"DelElem_");
  if (IsHighlighted(a,e)) FatalError("DelElem()-highlight: fatal error 1");
  MarkObject(a,e,0);

  for (v=ElemVar1st(e,&ix);v!=NULL;v=Next(&ix))
    SetVar(a,e,v->def,v->varSet,NULL);

  for (v=Group1st(e->varsContaining,&ix);v!=NULL;v=Next(&ix))
    ChangeVarEx(a,v,e,0);

  n1=e->n[1];
  n2=e->n[2];
  id=e->id;
  ar.e=e;
  i=ActDelElem(a,&ar);
  if (IsEmptyNode(n1)) DelNode(a,n1);
  if (IsEmptyNode(n2)) DelNode(a,n2);

  return NULL;
}

void ChangeElem(App a,Elem e,int ix,Node n) {
  struct _ChangeElemRec ar;
  Node n1;

  ValidatePtr(a,"ChangeElem");
  ValidatePtr(e,"ChangeElem_");
  ValidatePtr(n,"ChangeElem___");
  if (ix<1 || ix>2) FatalError("ChangeElem()-index: fatal error 1");
  if (e->n[3-ix]==n) FatalError("ChangeElem()-sameNodes: fatal error 2");
  assert(!NodesConnected(e->n[3-ix],n));

  n1=e->n[ix];

  ar.e=e;
  ar.id=e->id;
  ar.n1=e->n[1];
  ar.n2=e->n[2];
  if (ix==1) ar.n1=n; else ar.n2=n;
  ActChangeElem(a,&ar);

  if (IsEmptyNode(n1)) DelNode(a,n1);
}

void RevertElem(App a,Elem e) {
  struct _ChangeElemRec ar;

  ValidatePtr(a,"ChangeElem");
  ValidatePtr(e,"RevertElem_");

  ar.e=e;
  ar.id=e->id;
  ar.n2=e->n[1];
  ar.n1=e->n[2];
  ActChangeElem(a,&ar);
}

void ChangeElemId(App a,Elem e,int id) {
  struct _ChangeElemRec ar;

  ValidatePtr(a,"ChangeElemId");
  ValidatePtr(e,"ChangeElemId_");

  if (id>a->maxElemId) SetMaxElemId(a,id);

  ar.e=e;
  ar.id=id;
  ar.n1=e->n[1];
  ar.n2=e->n[2];
  ActChangeElem(a,&ar);
}

int AddTemplate(App a,char* fName,double xIncr,double yIncr,double angle,
    double scale) {
  struct _ActRec ar;
  Template t;
  int i;

  ValidatePtr(a,"AddTemplate");
  ValidatePtr(fName,"AddTemplate_");

  if (a->template!=NULL) {
    /*i=*/DelTemplate(a);
    /*if (i) return i;*/
  }

  t=Malloc(sizeof(*t));
  t->type=T_TEMPLATE;
  t->locks=0;
  t->xIncr=xIncr;
  t->yIncr=yIncr;
  t->angle=angle;
  t->scale=scale;
  t->dragStatus=0;
  t->dragX=t->dragY=0;

  ar.obj=t;

  strncpy(t->fName,fName,sizeof(t->fName)-1);
  i=ActAddTemplate(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

  if (a->fName!=NULL) {
    strncpy(t->fName,a->fName,sizeof(t->fName)-1);
    strcat(GetFilePath(t->fName),GetShortFName(fName));
    i=ActAddTemplate(a,&ar);
    if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
  }

  strncpy(t->fName,GetShortFName(fName),sizeof(t->fName)-1);
  i=ActAddTemplate(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

/*  strncpy(t->fName,a->xapp->argv[0],sizeof(t->fName)-1);
  strcat(GetFilePath(t->fName),GetShortFName(fName));
  i=ActAddTemplate(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
*/
  if (ar.obj!=NULL) ar.obj=Free(ar.obj);

  return i;
}

void ChangeTemplate(App a,double xIncr,double yIncr,double angle,double
    scale) {
  struct _ChangeTemplateRec ar;

  ValidatePtr(a,"ChangeTemplate");
  ValidatePtr(a->template,"ChangeTemplate.template");

  ar.t=a->template;
  ar.xIncr=xIncr;
  ar.yIncr=yIncr;
  ar.angle=angle;
  ar.scale=scale;

  ActChangeTemplate(a,&ar);
}

void DelTemplate(App a) {
  struct _DelTemplateRec ar;

  ValidatePtr(a,"DelTemplate");
  ValidatePtr(a->template,"DelTemplate.template");

  ar.t=a->template;
  ActDelTemplate(a,&ar);
}

void SetTemplateDragStatus(App a,int dragStatus,double dragX,double dragY) {
  SetObjInt(a,a->template,GetOffset(Template,dragStatus),
      sizeof(a->template->dragStatus),dragStatus,0);
  SetObjDouble(a,a->template,GetOffset(Template,dragX),dragX,0);
  SetObjDouble(a,a->template,GetOffset(Template,dragY),dragY,0);
}

void MarkObject(App a,void* object,int status) {
  ValidatePtr(a,"MarkObject");
  ValidatePtr(object,"MarkObject_");

  switch(GetObjType(object)) {
    case T_ELEM:
    case T_SEPARATOR:
    case T_SOURCE:
    case T_CHORD:
    case T_MESHELEMENT:
    case T_MESHCELL:
      break;
    default:
      FatalError("MarkObject()-type%d: fatal error 1",GetObjType(object));
  }

  /*if (!!IsMarked(a,object)==!!status) return; - Done in COG */

  ChangeObjGroup(a,a,GetOffset(App,mark),object,status,1);
}

void MarkGroup(App a,Group g,int status) {
  void* p;
  Index ix;

  if (status==10) MarkGroup(a,a->mark,0);
  if (g==NULL) return;
  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix))
    MarkObject(a,p,!!status);
}

/* Highlight the node and all attached elements and separators
   flag is passed to Highlight() as parameter
*/

void HighlightDragNode(View w,Node n,int flag) {
  Elem e;
  Separator sep;
  Index ix;

  Highlight(w,n,flag);
  for (e=NodeElem1st(n,&ix);e!=NULL;e=Next(&ix))
    Highlight(w,e,flag);
  for (sep=Group1st(n->separators,&ix);sep!=NULL;sep=Next(&ix))
    Highlight(w,sep,flag);
}

/* Return True if no elements or separators attached
*/

int IsEmptyNode(Node n) {
  return IsEmptyGroup(n->elems) && IsEmptyGroup(n->separators);
}

/* Return 1 if there is exactly 1 element and no separators attached
   g!=NULL: only members of g are considered
*/

int IsEndNode(Node n,Group g) {
  int i;
  Index ix;
  Elem e;
  Separator sep;

  ValidatePtr(g,"IsEndNode");
  for (i=0,e=NodeElem1st(n,&ix);e!=NULL;e=Next(&ix))
    if (InGroup(g,e)) i++;

  for (sep=Group1st(n->separators,&ix);sep!=NULL;sep=Next(&ix))
    if (InGroup(g,sep)) return 0;

  return i==1 && (g!=NULL || IsEmptyGroup(n->separators));
}

/* Check for irregular node:
     Exactly 2 elements from the group or from the app must be attached
     The end of one of then must be the beginning of the other
     No separators must be attached
   g!=NULL: only members of g (elems & separators) are considered
   Return: 0 = regular node
           ERR_xxx: error code showing why it is irregular
*/

int IsIrregularNode(Node n,Group g) {
  Elem e,e1 = NULL;
  Separator sep;
  Index ix;
  int i,j;

  for (j=i=0,e=NodeElem1st(n,&ix);e!=NULL;e=Next(&ix)) {
    if (g!=NULL && !InGroup(g,e)) continue;
    switch(i++) {
      case 0:
        e1=e;
        break;
      case 1:
        if (e1->n[1]==e->n[2]) j=1;
        if (e1->n[2]==e->n[1]) j=2;
        break;
      case 2:
        return STR_IRRTOOMANY;
      default:
        FatalError("IsRegularNode()-i: fatal error 1");
    }
  }
  if (i<2) return STR_IRRTOOFEW;

  for (sep=Group1st(n->separators,&ix);sep!=NULL;sep=Next(&ix))
    if (g==NULL || InGroup(g,sep)) return STR_IRRSEPARATORS;

  return j? 0 : STR_IRRNORMALS;
}

/* Count: # of intermediate points to produce */

Node SplitElem(App a,Elem e,int count,int* pErr)  {
  Node n0,n=NULL;
  int i,isMarked;
  Elem e1;

  if (IsLocked(e)) {
    if (pErr!=NULL) *pErr=ERR_LOCKED;
    return NULL;
  }

  assert(count>0);
  isMarked=IsMarked(a,e);

  for (n0=e->n[1],i=1;i<count;i++) {
    n=AddNode(a,e->n[1]->x+(e->n[2]->x-e->n[1]->x)*(double)i/count,
        e->n[1]->y+(e->n[2]->y-e->n[1]->y)*(double)i/count);
    assert(n!=NULL);
    e1=AddElem(a,n0,n);
    MarkObject(a,e1,isMarked);
    n0=n;
  }

  e1=AddElem(a,n0,e->n[2]);
  MarkObject(a,e1,isMarked);

  /*i=*/DelElem(a,e);

  if (pErr!=NULL) *pErr=i;
  return n0;
}

/* a==NULL: check for join; return !NULL if can be performed
   a!=NULL: perform join
   pErr!=NULL: store error code in *pErr
*/

Elem JoinElems(App a,Node n,int* pErr) {
  int i,errBuf;
  Elem e[3];
  Index ix;

  ValidatePtr(n,"JoinElems_");
  if (pErr==NULL) pErr=&errBuf;

  if ((*pErr=IsIrregularNode(n,NULL))!=0) if (a==NULL) return NULL;
  else FatalError("JoinElems()-!regular: fatal error 1");

  e[0]=NodeElem1st(n,&ix);
  e[1]=Next(&ix);
  i= e[0]->n[1]==e[1]->n[2] ? 0:1;
  if (NodesConnected(e[i]->n[2],e[1-i]->n[1])) if (a==NULL) {
    *pErr=ERR_JOINCONNECTED;
    return NULL;
  } else FatalError("JoinElems()-connect: fatal error 2");

  if (IsLocked(e[0]) || IsLocked(e[1]) || IsLocked(e[0]->n[1]))
    if (a==NULL) {*pErr=ERR_LOCKED;return NULL;} else
      FatalError("JoinElems()-locks: fatal error 1");

  if (a==NULL) return e[0]; /* !NULL means join is possible */

  e[2]=AddElem(a,e[1-i]->n[1],e[i]->n[2]);
  if (IsMarked(a,e[0]) || IsMarked(a,e[1])) MarkObject(a,e[2],1);
  DelElem(a,e[0]);
  DelElem(a,e[1]);

  return e[2];
}

/* Check for an element attached to both nodes
*/

int NodesConnected(Node n1,Node n2) {
  Elem e;
  Index ix;

  for (e=NodeElem1st(n1,&ix);e!=NULL;e=Next(&ix))
    if (InGroup(n2->elems,e)) return 1;

  return 0;
}

/* Check for a node to which both elements are attached
   Return 1 if connected >>; 2 if connected <> or ><
*/
int ElemsConnected(Elem e1,Elem e2) {
  if (e1->n[1]==e2->n[2] || e1->n[2]==e2->n[1]) return 1;
  if (e1->n[1]==e2->n[1] || e1->n[2]==e2->n[2]) return 2;

  return 0;
}

/* Return another element attached to a regular node
   g!=NULL: only members of g are considered
*/

Elem GetNextElem(Elem e,Group g,int node) {
  Index ix;
  int i;
  Elem e1,e2 = NULL;

  if (node<1 || node>2)
    FatalError("GetNextElem()-node%d: fatal error 1",node);

  for (i=0,e1=NodeElem1st(e->n[node],&ix);e1!=NULL;e1=Next(&ix)) {
    if (g!=NULL && !InGroup(g,e1)) continue;
    if (e1!=e) {e2=e1;i++;}
  }
  if (i!=1) return NULL;

  return e2;
}

/* Return group containing chain of elements connected by regular nodes
   g!=NULL: only members of g are considered
*/

#define CHAIN_LIMIT 10000 /* Prevent bugs from locking up the program */

Group GetElemChain(Elem e,Group gElems,Group gCheckRegular) {
  Group gRes;
  Elem e0;

  e0=e;
  while (IsRegularNode(e->n[1],gElems) &&
      (gCheckRegular==gElems || IsRegularNode(e->n[1],gCheckRegular))) {
    e=GetNextElem(e,gElems,1);
    if (e==e0) break;
  }

  gRes=CreateGroup();
  e0=e;

  do {
    GroupAdd(gRes,e);
    if (!IsRegularNode(e->n[2],gElems) ||
       (gCheckRegular!=gElems && !IsRegularNode(e->n[2],gCheckRegular)))
     break;

    e=GetNextElem(e,gElems,2);
    if (GroupCount(gRes)>CHAIN_LIMIT) {
      fprintf(stderr,"Warning - GetElemChain() - Limit crossed.\n");
      return gRes;
    }
  } while (e!=e0);

  return gRes;
}

/* Order elements for a Target variable

   Return: error code or 0 for success
   errObj!=NULL: in case of error *errObj points to an object causing the
     error
*/

int OrderTargetElems(Group g,void** errObj) {
  Elem e;
  Group g1;
  int i;
  void* errBuf;

  ValidatePtr(g,"OrderTargetElems");

  if (errObj==NULL) errObj=&errBuf;
  *errObj=NULL;

  if (GroupCount(g)<2) return ERR_MINTARGETELEMS;

  e=Group1st(g,NULL);
  if (e==NULL) return ERR_NOELEMS;

  for (i=GroupCount(g);i && IsRegularNode(e->n[1],g);i--)
    e=GetNextElem(e,g,1);
  if (!i) return ERR_CLOSED;
  if (!IsEndNode(e->n[1],g)) {*errObj=e->n[1];return ERR_IRREGULAR;}

  g1=CreateGroup();
  while (IsRegularNode(e->n[2],g)) {
    GroupAdd(g1,e);
    e=GetNextElem(e,g,2);
  }
  GroupAdd(g1,e);
  if (GroupCount(g1)!=GroupCount(g)) {
    *errObj=e->n[2];
    g1=FreeGroup(g1);
    return IsEndNode(e->n[2],g) ? ERR_BROKEN : ERR_IRREGULAR;
  }

  CopyGroup(g1,g);
  FreeGroup(g1);

  return 0;
}

/* Order elements for a Structure variable */
/* Arguments: */
/*   g0      unformatted group of elements */
/*   errObj  pointer to pointer to the object causing an error */
/* Returns 0 if no error, otherwise error code */
/*   errObj  points to the object causing an error */
/*   g0      contains formatted group */

int OrderStructureElems(App a,Group g0,Group* pggTargets,Group* pggClosed,
    Group* pggBroken,void** errObj) {
  Elem e,e0=NULL;
  Node breakNode=NULL;
  Group g,g1,gt,gt1;
  int i,r,bClosed,bTarget,bT;
  double yMax=0;
  void* errBuf;
  Index ix,ixt;
  Var v;
  VarDef vd;
  VarSet vs;

  assert(a->type==T_APP);

  r=0;
  if (errObj==NULL) errObj=&errBuf;
  *errObj=NULL;

  /* No elements means no structure */

  if (IsEmptyGroup(g0)) return ERR_NOELEMS;

  /* Create temporary groups */

  g=CreateGroup();
  CopyGroup(g0,g);
  g1=CreateGroup();

  /* Create result groups */

  *pggTargets=CreateGroup();
  *pggClosed=CreateGroup();
  *pggBroken=CreateGroup();

  /* Find targets if any. Targets are used only in Carre mode */

  gt=CreateGroup();
  for (vs=AppVarSet1st(a,&ix);vs!=NULL;vs=Next(&ix))
    for (vd=Group1st(vs->def->varDefs,&ixt);vd!=NULL;vd=Next(&ixt)) {
      if (!(vd->varType & VTF_TARGET)) continue;
      gt1=GetVar(vs,vd,NULL);
      if (GroupCount(gt1)>0) GroupAdd(gt,gt1);
    }

/*  v=GetVarPtrByType(a,VT_TARGET1);
  if (v==NULL || v->val==NULL || IsEmptyGroup(v->val)) t1=NULL;
  else t1=v->val;

  v=GetVarPtrByType(a,VT_TARGET2);
  if (v==NULL || v->val==NULL || IsEmptyGroup(v->val)) t2=NULL;
  else t2=v->val; */

/* Order all structure rings starting from the outermost one */

  while (!IsEmptyGroup(g)) {

    /* Find the element with the outermost origin */
    /* It always belongs to the outermost ring */

    for (e0=NULL,e=Group1st(g,&ix);e!=NULL;e=Next(&ix)) {
      if (e0==NULL || e->n[1]->y>yMax) {
        yMax=e->n[1]->y;
        e0=e;
      }
    }
    assert(e0!=NULL);

    /* Trace the structure ring connected to the found element */

    bClosed=1;
    for (e=e0,i=1;;i++) {
      if (IsEndNode(e->n[2],g)) {
        breakNode=e->n[2];
        if (a->outputMode==OUTPUTMODE_CARRE) {bClosed=0;break;}
        *errObj=e->n[2];FreeGroup(g);FreeGroup(g1);FreeGroup(gt);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_BROKEN;
      }
      if (!IsRegularNode(e->n[2],g)) {
        *errObj=e->n[2];FreeGroup(g);FreeGroup(g1);FreeGroup(gt);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_IRREGULAR;
      }
      e=GetNextElem(e,g,2);
      if (e==e0) break;
    }

    /* Broken chain in Carre mode: find the beginning */

    if (!bClosed) {
      assert(a->outputMode==OUTPUTMODE_CARRE);
      for (i=1;IsRegularNode(e->n[1],g);i++)
        e=GetNextElem(e,g,1);
      if (!IsEndNode(e->n[1],g)) {
        *errObj=e->n[1];FreeGroup(g);FreeGroup(g1);FreeGroup(gt);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_IRREGULAR;
      }
    }

    /* Copy the ring into g1 and remove all its elements from g */

    bTarget=0;
    for (;i;i--) {
/*      if (t1!=NULL && InGroup(t1,e)) bT1=bTarget=1;
      if (t2!=NULL && InGroup(t2,e)) bT2=bTarget=1; */

      GroupAdd(g1,e);
      e0=e;
      e=GetNextElem(e,g,2);
      GroupDel(g,e0);
    }

    /* Additional tests */

    for (gt1=Group1st(gt,&ixt);gt1!=NULL;gt1=Next(&ixt)) {
      bT=0;
      for (e=Group1st(g1,&ix);e!=NULL;e=Next(&ix))
        if (InGroup(gt1,e)) bT=bTarget=1;

      if (a->outputMode==OUTPUTMODE_CARRE) {
        if (bT & !GroupInGroup(gt1,g1)) {
          /* *errObj=e0; */ FreeGroup(g);FreeGroup(g1);FreeGroup(gt);
          FreeStructureInfo(pggTargets,pggClosed,pggBroken);
          return ERR_CARRE_TARGETXSTRUCTURE;
        }
      }
    }

    if (a->outputMode==OUTPUTMODE_CARRE) {
      if (bTarget && !bClosed) {
        *errObj=breakNode;FreeGroup(g);FreeGroup(g1);FreeGroup(gt);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_CARRE_BROKENTARGET;
      }
    }

/*      if (bT1 && !GroupInGroup(t1,g1)) { */
        /* *errObj=e0; */ /*FreeGroup(g);FreeGroup(g1);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_CARRE_TARGETXSTRUCTURE;
      }

      if (bT2 && !GroupInGroup(t2,g1)) {  */
        /* *errObj=e0 */ /*;FreeGroup(g);FreeGroup(g1);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR_CARRE_TARGETXSTRUCTURE;
      } */

    /* Add g1 to one of the result groups */

    if (bTarget) GroupAdd(*pggTargets,g1);
    else if (bClosed) GroupAdd(*pggClosed,g1);
    else {GroupAdd(*pggBroken,g1);}

    g1=CreateGroup(); /* Create a new g1 */
  }

  /* Free temporary groups */

  FreeGroup(g1);
  FreeGroup(g);
  FreeGroup(gt);

  return 0;
}

void FreeStructureInfo(Group* pggTargets,Group* pggClosed,Group* pggBrokn) {
  Group g;
  Index ix;

  for (g=Group1st(*pggTargets,&ix);g!=NULL;g=Next(&ix)) FreeGroup(g);
  *pggTargets=FreeGroup(*pggTargets);
  for (g=Group1st(*pggClosed,&ix);g!=NULL;g=Next(&ix)) FreeGroup(g);
  *pggClosed=FreeGroup(*pggClosed);
  for (g=Group1st(*pggBrokn,&ix);g!=NULL;g=Next(&ix)) FreeGroup(g);
  *pggBrokn=FreeGroup(*pggBrokn);
}

/* Check if gPart is the part of gStructure
*/

int CheckStructurePart(Group gStructure,Group gPart,void** pErrObj) {
  Elem e;
  Index ix;

  for (e=Group1st(gPart,&ix);e!=NULL;e=Next(&ix))
    if (!InGroup(gStructure,e)) {
      if (pErrObj!=NULL) *pErrObj=e;
      return ERR_NOTSTRUCTPART;
    }

  return 0;
}

/* Renumber all elements and separators
   Separators are renumbered first
*/

void RenumberElems(App a) {
  Group g;
  Elem e,e0;
  Separator sep;
  Index ix;
  int i=1;

/* Renumber separators first */

  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    ChangeSeparatorId(a,sep,i++);

/* Renumbers non-chord elements */

  g=CopyGroup(a->elems,NULL);

/*
  g=CreateGroup();

  for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix))
    if (!IsChordElem(e)) GroupAdd(g,e);
*/

  while (!IsEmptyGroup(g)) {
    e0=e=Group1st(g,NULL);
    while (IsRegularNode(e->n[1],g)) {
      e=GetNextElem(e,g,1);
      if (e==e0) break;
    }
    while (e!=NULL) {
      ChangeElemId(a,e,i++);
      e0=e;
      e=GetNextElem(e0,g,2);
      GroupDel(g,e0);
    }
  }
  g=FreeGroup(g);

  SetMaxElemId(a,i-1);
}

/* Get X value of a template point - rotate and shift if necessary
*/

double TemplateXY2X(Template t,XY xy) {
  return (xy->x*cos(t->angle)-xy->y*sin(t->angle))*t->scale+t->xIncr;

  /*
  switch(t->rotate&3) {
    case 0: return xy->x+t->xIncr;
    case 1: return -xy->y+t->xIncr;
    case 2: return -xy->x+t->xIncr;
    case 3: return xy->y+t->xIncr;
    default: FatalError("TemplateXY2X()-rotate: fatal error 1");return 0;
  }
  */
}

/* Get Y value of a template point - rotate and shift if necessary
*/

double TemplateXY2Y(Template t,XY xy) {
  return (xy->x*sin(t->angle)+xy->y*cos(t->angle))*t->scale+t->yIncr;

/*
  switch(t->rotate&3) {
    case 0: return xy->y+t->yIncr;
    case 1: return xy->x+t->yIncr;
    case 2: return -xy->y+t->yIncr;
    case 3: return -xy->x+t->yIncr;
    default: FatalError("TemplateXY2Y()-rotate: fatal error 1");return 0;
  }
*/
}

/* Add node only if no node exists with the same coordinates
*/

static Node AddNodeAt(App a,double x,double y) {
  Index ix;
  Node n;

  for (n=AppNode1st(a,&ix);n!=NULL;n=Next(&ix))
    if (n->x==x && n->y==y) return n;

  return AddNode(a,x,y);
}

/* Append template to the geometry
*/

int AppendTemplate(App a) {
  XY xy;
  Index ix;
  Node n1,n2;
  int i;

  ValidatePtr(a,"AppendTemplate");
  ValidatePtr(a->template,"AppendTemplate.template");

  LockAppUpdate(a,1);
  for (i=0,xy=TemplatePoint1st(a->template,&ix);xy!=NULL;xy=Next(&ix)) {
    n1=AddNodeAt(a,TemplateXY2X(a->template,xy),TemplateXY2Y(a->template,xy));
    xy=Next(&ix);
    n2=AddNodeAt(a,TemplateXY2X(a->template,xy),TemplateXY2Y(a->template,xy));
    if (!NodesConnected(n1,n2) && n1!=n2) {
      AddElem(a,n1,n2);
      i++;
    } else {
      if (IsEmptyNode(n1)) DelNode(a,n1);
      if (n1!=n2 && IsEmptyNode(n2)) DelNode(a,n2);
    }
  }
  LockAppUpdate(a,-1);
  return i;
}

/* Produce DG template file from elements
*/

int WriteElemsAsDgTemplate(Group gElems,char* fName) {
  FILE* f;
  Index ix;
  Elem e;

  f=fopen(fName,"w");
  if (f==NULL) return ERR_FWRITE;

  fprintf(f,"DivGeo template file>>\n");
  for (e=Group1st(gElems,&ix);e!=NULL;e=Next(&ix))
    fprintf(f,"%e %e\n%e %e\n",e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y);
  fclose(f);
  return 0;
}

/* Private functions
*/

#define NODEDATA 1
#define ELEMDATA 2
#define ENDFILE 99
int LoadOgrTemplateFile(Template t) {
  FILE* f;
  struct {
    int id;
    double x,y;
  }* ogrNode;
  Group ogrNodes,g;
  Index ix;
  char s[500];
  int it,id,iv,kc,n1,n2;
  double x,y;

  f=fopen(t->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  ogrNodes=CreateGroup();
  g=CreateGroup();

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"%d",&it)!=1) goto err;
    if (it==ENDFILE) break;
    if (sscanf(s,"%d%d%d%d",&it,&id,&iv,&kc)!=4) goto err;
    switch(it) {
      case NODEDATA:
        fgets(s,sizeof(s)-1,f);kc--;
        if (sscanf(s,SCANFLT""SCANFLT,&x,&y)!=2) goto err;
        ogrNode=Malloc(sizeof(*ogrNode));
        ogrNode->id=id;
        ogrNode->x=x;
        ogrNode->y=y;
        GroupAdd(ogrNodes,ogrNode);
        break;
      case ELEMDATA:
        fgets(s,sizeof(s)-1,f);kc--;
        if (sscanf(s,"%d",&n1)!=1) goto err;
        if (n1!=2) goto err;
        fgets(s,sizeof(s)-1,f);kc--;
        if (sscanf(s,"%d%d",&n1,&n2)!=2) goto err;

        for (ogrNode=Group1st(ogrNodes,&ix);ogrNode!=NULL;ogrNode=Next(&ix))
          if (ogrNode->id==n1) break;
        if (ogrNode==NULL) goto err;
        AddXY(g,ogrNode->x,ogrNode->y);

        for (ogrNode=Group1st(ogrNodes,&ix);ogrNode!=NULL;ogrNode=Next(&ix))
          if (ogrNode->id==n2) break;
        if (ogrNode==NULL) goto err;
        AddXY(g,ogrNode->x,ogrNode->y);
        break;
      default:
        break;
    }
    if (kc<0) goto err;
    while (kc--) fgets(s,sizeof(s)-1,f);
  }
  fclose(f);
  ogrNodes=FreeMallocedGroup(ogrNodes);
  t->points=g;
  return 0;
  err:
  g=FreeMallocedGroup(g);
  ogrNodes=FreeMallocedGroup(ogrNodes);
  fclose(f);
  return ERR_BADFILE;
}
#undef NODEDATA
#undef ELEMDATA
#undef ENDFILE

int LoadDgTemplateFile(Template t) {
  FILE* f;
  char s[500];
  double x,y;
  Group g;

  f=fopen(t->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  fgets(s,sizeof(s)-1,f);

  g=CreateGroup();
  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,SCANFLT""SCANFLT,&x,&y)!=2) {
      g=FreeMallocedGroup(g);
      fclose(f);
      return ERR_BADFILE;
    }
    AddXY(g,x,y);
  }
  fclose(f);
  if (GroupCount(g)&1) return ERR_BADFILE;

  t->points=g;
  return 0;
}

int Load2ColumnsTemplateFile(Template t) {
  FILE* f;
  char s[500],s1[500];
  double x,y,xOld=0,yOld=0;
  int bOld;
  Group g;

  f=fopen(t->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  g=CreateGroup();
  bOld=0;

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,SCANFLT""SCANFLT,&x,&y)!=2) {
      if (sscanf(s,"%s",s1)!=1) {
        bOld=0;
        continue;
      }
      g=FreeMallocedGroup(g);
      fclose(f);
      return ERR_BADFILE;
    }
    if (bOld) {
      AddXY(g,xOld,yOld);
      AddXY(g,x,y);
    }
    xOld=x;
    yOld=y;
    bOld=1;
  }

  fclose(f);

  assert(!(GroupCount(g)&1));

  t->points=g;

  return 0;
}

/* Reads one HPGL command, finished by a ';' or EOF */
/* Ignores blanks/line ends */

#define HPGL_COMMAND_MAX 256

static char* ReadHpglCommand(FILE* f) {
  static char buf[HPGL_COMMAND_MAX];
  int i=0,c;

  while (1) {
    c=fgetc(f);
    if (c==EOF) if (!i) return NULL; else break;
    if (isspace(c)) continue;
    if (c==';') break;
    buf[i++]=(char)tolower(c);
    if (i>=sizeof(buf)-1) break;
  }
  buf[i]=0;

  return buf;
}

#define CHECK_MAXCOMMANDS 100

int CheckForHpglFile(FILE* f) {
  int i;
  char* s;
  double x,y;

  for (i=0;i<CHECK_MAXCOMMANDS;i++) {
    s=ReadHpglCommand(f);
    if (s==NULL) break;
    if (sscanf(s,"pd"SCANFLT","SCANFLT,&x,&y)==2) return 0;
  }

  return -1;
}

int LoadHpglTemplateFile(Template t) {
  FILE* f;
  char* s;
  double x,y,xOld,yOld;
  Group g;

  f=fopen(t->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  g=CreateGroup();
  xOld=yOld=0;

  while ((s=ReadHpglCommand(f))!=NULL) {
    if (sscanf(s,"pu"SCANFLT","SCANFLT,&x,&y)==2) {
      xOld=x;yOld=y;
    } else if (sscanf(s,"pd"SCANFLT","SCANFLT,&x,&y)==2) {
      AddXY(g,xOld,yOld);
      AddXY(g,x,y);
      xOld=x;
      yOld=y;
    }
  }

  fclose(f);

  if (GroupCount(g)==0) {
    FreeMallocedGroup(g);
    return ERR_BADFILE;
  }

  assert(!(GroupCount(g)&1));

  t->points=g;

  return 0;
}


void LockTarget(Var v,int incr) {
  Elem e;
  Index ix;

  LockObject(v,incr);
  for (e=Group1st(v->val,&ix);e!=NULL;e=Next(&ix)) {
    LockObject(e,incr);
    LockObject(e->n[1],incr);
    LockObject(e->n[2],incr);
  }
}


/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  Routines for simplifying the geometry                       // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

/* Fill gaps in the structure /////////////////////////////////////// */

/* Aux structures/functions */

typedef struct _NearestNode {
  Node n1,n2;
  double dist;
}* NearestNode;

static NearestNode FindNearestNode(Node n0,Group g,double maxDist) {
  Node n,nMin;
  double d,dMin;
  NearestNode nn;
  Index ix;

  dMin=MAXDOUBLE;
  nMin=NULL;
  for (n=Group1st(g,&ix);n!=NULL;n=Next(&ix)) {
    if (n==n0 || NodesConnected(n,n0)) continue;
    assert(GroupCount(n->elems)==1);
    d=hypot(n->x-n0->x,n->y-n0->y);
    if (d<=maxDist && d<dMin) {
      dMin=d;
      nMin=n;
    }
  }

  if (nMin==NULL) return NULL;

  nn=Malloc(sizeof(*nn));
  nn->n1=n0;
  nn->n2=nMin;
  nn->dist=dMin;

  return nn;
}

static int GlueNodes_Compare(NearestNode n1,NearestNode n2,void* p) {
  return sign(n1->dist-n2->dist);
}

int GlueNodes(App a,Group g0Elems,double maxDist,int* prCount) {
  Group g,gE,gN;
  Node n0,n,nMin;
  Elem e;
  int i,ctCount;
  double x,y,d,dMin;
  Index ix,ixe,ix1;
  NearestNode nn,nn1,nn2;

  /* Get all end nodes of elements in g0Elems or App */

  gE=CopyGroup(g0Elems==NULL? a->elems : g0Elems,NULL);
  RestrictGroupToType(gE,T_ELEM);

  g=CreateGroup();
  for (e=Group1st(gE,&ix);e!=NULL;e=Next(&ix)) {
    if (GroupCount(e->n[1]->elems)==1 && !InGroup(g,e->n[1]))
      GroupAdd(g,e->n[1]);
    if (GroupCount(e->n[2]->elems)==1 && !InGroup(g,e->n[2]))
      GroupAdd(g,e->n[2]);
  }
  FreeGroup(gE);

  ctCount=0;

  /* For each node in g, find its nearest node, then sort the array */

  gN=CreateGroup();
  for (n=Group1st(g,&ix);n!=NULL;n=Next(&ix)) {
    nn=FindNearestNode(n,g,maxDist);
    if (nn!=NULL) GroupAdd(gN,nn); else GroupDel(g,n);
  }

  GroupQSort(gN,(GroupSortProc)GlueNodes_Compare,NULL);

  /* Create elements between closest pairs in the array, eventually
     recalculating pairs */

  while (!IsEmptyGroup(gN)) {
    nn=Group1st(gN,NULL);
/*    if (nn->dist>maxDist) break; */

    e=AddElem(a,nn->n1,nn->n2);
    if (IsIrregularNode(nn->n1,NULL)==STR_IRRNORMALS) RevertElem(a,e);
    ctCount++;
    GroupDel(gN,nn);
    GroupDel(g,nn->n1);
    GroupDel(g,nn->n2);

    for (nn1=Group1st(gN,&ix);nn1!=NULL;nn1=Next(&ix)) {
      if (nn1->n1==nn->n2) {GroupDel(gN,nn1);nn1=Free(nn1);}
      else if (nn1->n2==nn->n1 || nn1->n2==nn->n2) {
        GroupDel(gN,nn1);
        nn2=FindNearestNode(nn1->n1,g,maxDist);
        nn1=Free(nn1);
        nn1=nn2;
        if (nn1!=NULL) {
          for (nn2=Group1st(gN,&ix1);nn2!=NULL;nn2=Next(&ix1))
            if (nn2->dist>nn1->dist) break;
          nn2=Prev(&ix1);
          GroupAddAt(gN,nn2,nn1);
        }
      }
    }
    Free(nn);
  }

  assert(IsEmptyGroup(gN));
  FreeMallocedGroup(gN);
  FreeGroup(g);

  if (prCount!=NULL) *prCount=ctCount;

  return 0;
}

/* Merge elements forming an almost straight line /////////////////// */

int GlueElems(App a,Group g0,double maxDist,double maxLen,int bCutLonger,
    int* prCount) {
  Group g,g1;
  Elem e0,e,e1,eMax;
  double x0,y0,h;
  int joinedCount;
  Index ix,ix1;

  assert(maxLen>0);

  /* The group of unprocessed elements */

  g=CopyGroup(g0==NULL? a->elems : g0,NULL);
  RestrictGroupToType(g,T_ELEM);
  joinedCount=0;

  /* Process every chain */

  while (!IsEmptyGroup(g)) {
    g1=GetElemChain(Group1st(g,NULL),g,NULL);
    assert(!IsEmptyGroup(g1));

    /* Remove the chain from the group of unprocessed elements */

    for (e=Group1st(g1,&ix);e!=NULL;e=Next(&ix)) GroupDel(g,e);

    /* Follow the whole chain */

    while (GroupCount(g1)>=1) {

      /* Find out how many elements at the beginning can be glued.
         The last one to be glued is stored in eMax, or NULL */

      eMax=NULL;
      e0=e=Group1st(g1,&ix);
      x0=e0->n[1]->x;
      y0=e0->n[1]->y;
      while ((e=Next(&ix))!=NULL) {
        if (hypot(x0-e->n[2]->x,y0-e->n[2]->y)>maxLen) goto TooFarAway;
        for (e1=Group1st(g1,&ix1);e1!=e;e1=Next(&ix1)) {
          if (Point2VectorDist(x0,y0,e->n[2]->x,e->n[2]->y,
              e1->n[2]->x,e1->n[2]->y,NULL,NULL)>maxDist) goto TooFarAway;
        }
        eMax=e;
        TooFarAway:;
      }

      if (eMax!=NULL) {

        /* Glue elements */

        for (e=Group1st(g1,&ix);e!=eMax;e=Next(&ix)) {
          GroupDel(g1,e);
          if (JoinElems(NULL,e->n[2],NULL)!=NULL) {
            JoinElems(a,e->n[2],NULL);
            joinedCount++;
          }
        }
        GroupDel(g1,eMax);
      } else {

        /* Remove the 1st element from the group and try to glue the rest */

        GroupDel(g1,e0);

        /* Cut the only remaining element, if necessary */

        h=hypot(e0->n[1]->x-e0->n[2]->x,e0->n[1]->y-e0->n[2]->y);
        if (bCutLonger && h>maxLen) SplitElem(a,e0,(int)(h/maxLen),NULL);
      }
    }

    /* Free what remained from the chain */

    g1=FreeGroup(g1);
  }

  g=FreeGroup(g);
  if (prCount!=NULL) *prCount=joinedCount;

  return 0;
}

/* Force external normals of all adjacend elements into one dir ///// */

int GlueNormals(App a,Group g0,int* prCount) {
  Elem e,e0,e1;
  int i,j,revCnt;
  Group g,g1;
  Index ix;

  /* Create a group holding all unprocessed elements */

  g=CopyGroup(g0==NULL? a->elems : g0,NULL);
  RestrictGroupToType(g,T_ELEM);

  revCnt=0;
  while (!IsEmptyGroup(g)) {

    /* Pick up some element and follow its chain back to the beginning */

    e=e0=Group1st(g,NULL);
    i=1;
    while (1) {
      e1=GetNextElem(e,g,i);
      if (e1==NULL) break;
      if (e1->n[i]==e->n[i]) i=3-i;
      if (e1==e0) break;
      e=e1;
    }

    /* Now follow the chain towards its other end, reversing normals that
       point to a wrong directions and counting their number */

    e0=e;
    i=3-i;
    g1=CreateGroup();
    j=0;
    while (1) {
      e1=GetNextElem(e,g,i);
      GroupAdd(g1,e);
      if (e1==NULL || e1==e0) break;
      if (e1->n[i]==e->n[i]) {RevertElem(a,e1);j++;}
      if (e1->n[i]==e->n[i]) i=3-i;
      e=e1;
    }

    /* If more than 1/2 of all normals were reversed, reverse all normals
       once more, so that they point to the MAJOR direction */

    if (j>GroupCount(g1)/2) {
      for (e=Group1st(g1,&ix);e!=NULL;e=Next(&ix)) RevertElem(a,e);
      j=GroupCount(g1)-j;
    }

    /* Remove elements from the list of unprocessed ones and clean up */

    for (e=Group1st(g1,&ix);e!=NULL;e=Next(&ix)) GroupDel(g,e);
    revCnt+=j;
    g1=FreeGroup(g1);
  }

  g=FreeGroup(g);

  if (prCount!=NULL) *prCount=revCnt;
  return 0;
}

int ConvertChordsToElems(App a,Group chords,void** pErrObj) {
  Chord ch;
  Node n1,n2;
  Elem e;
  int i;
  Index ix;

  if (pErrObj!=NULL) *pErrObj=NULL;

  for (ch=Group1st(chords,&ix);ch!=NULL;ch=Next(&ix)) {
    n1=AddNodeAt(a,ch->x1,ch->y1);
    n2=AddNodeAt(a,ch->x2,ch->y2);
    if (!NodesConnected(n1,n2) && n1!=n2) {
      i=IsMarked(a,ch);
      DelChord(a,ch);ch=NULL;
      e=AddElem(a,n1,n2);
      if (e==NULL) continue;
      MarkObject(a,e,i);
    } else {
      if (IsEmptyNode(n1)) DelNode(a,n1);
      if (IsEmptyNode(n2)) DelNode(a,n2);
    }
  }

  return 0;
}

int GetNearestTemplatePoint(Template t,double x,double y,
    double* ptx,double* pty,double* psx,double* psy) {
  XY xy;
  Index ix;

  double d,dMin=MAXDOUBLE;
  XY xyMin=NULL;

  for (xy=Group1st(t->points,&ix);xy!=NULL;xy=Next(&ix)) {
    d=hypot(TemplateXY2X(t,xy)-x,TemplateXY2Y(t,xy)-y);
    if (d<dMin) {
      xyMin=xy;
      dMin=d;
    }
  }

  if (xyMin==NULL) return -1;
  if (ptx!=NULL) *ptx=xyMin->x;
  if (pty!=NULL) *pty=xyMin->y;
  if (psx!=NULL) *psx=TemplateXY2X(t,xyMin);
  if (psy!=NULL) *psy=TemplateXY2Y(t,xyMin);

  return 0;
}

int PlaceTemplateByHandles(App a,double xa,double ya,double x1,double y1,
    double xb,double yb,double x2,double y2) {
  double h12,hab,angle1,angle2;
  struct _Template fakeT;
  struct _XY xy;

  if (a->template==NULL) return ERR_NOTEMPLATE;

  if (xa!=xb || ya!=yb || x1!=x2 || y1!=y2) { /* Two handles */
    hab=hypot(xa-xb,ya-yb);
    h12=hypot(x1-x2,y1-y2);

    if (hab<MINDOUBLE) return ERR_REPOS_TEMPLATE_1;
    if (h12<MINDOUBLE) return ERR_REPOS_TEMPLATE_2;

    fakeT.scale=h12/hab;

    angle1=acos((xb-xa)/hab);
    if (yb-ya<0) angle1=-angle1;
    angle2=acos((x2-x1)/h12);
    if (y2-y1<0) angle2=-angle2;
    fakeT.angle=angle2-angle1;
  } else {
    fakeT.angle=a->template->angle;
    fakeT.scale=a->template->scale;
  }

  fakeT.xIncr=fakeT.yIncr=0;

  xy.x=xa;
  xy.y=ya;

  ChangeTemplate(a,
        -TemplateXY2X(&fakeT,&xy)+x1,
        -TemplateXY2Y(&fakeT,&xy)+y1,
        fakeT.angle,fakeT.scale);

  return 0;
}

void SetMaxElemId(App a,int id) {
  SetObjInt(a,a,GetOffset(App,maxElemId),sizeof(a->maxElemId),id,0);
}

int GetNextElemId(App a) {
  Elem e;
  Separator sep;
  int i;
  int* bUsed;
  Index ix;

  if (GetUnusedNumberCount(a)) {
    bUsed=Malloc(sizeof(*bUsed)*(a->maxElemId+1));
    for (i=0;i<=a->maxElemId;i++) bUsed[i]=0;

    for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix)) {
      assert(e->id>=0 && e->id<=a->maxElemId);
      bUsed[e->id]=1;
    }
    for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix)) {
      assert(sep->id>=0 && sep->id<=a->maxElemId);
      bUsed[sep->id]=1;
    }

    for (i=1;i<=a->maxElemId;i++) if (!bUsed[i]) {
      Free(bUsed);
      return i;
    }

    Free(bUsed);
    assert(0);
  }

  i=a->maxElemId+1;
  SetMaxElemId(a,i);

  return i;
}

int ElementInTarget(Elem e) {
  Var v;
  Index ix;

  assert(e->type==T_ELEM);
  for (v=Group1st(e->varsContaining,&ix);v!=NULL;v=Next(&ix))
    if (v->def->varType & VTF_TARGET) return 1;

  return 0;
}

int PointInTarget(Node n) {
  Elem e;
  Index ix;

  assert(n->type==T_NODE);
  for (e=Group1st(n->elems,&ix);e!=NULL;e=Next(&ix))
    if (ElementInTarget(e)) return 1;

  return 0;
}

