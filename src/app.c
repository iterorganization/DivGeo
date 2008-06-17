#include "dg.h"
/*#pragma hdrstop*/

App CreateApp() {
  App a;

  a=Malloc(sizeof(*a));
  a->type=T_APP;

  a->undoStack=CreateStack();
  a->redoStack=CreateStack();
  a->undoMode=UM_NORM;
  a->bNonDestructiveCancel=0;
  a->alt=0;
  a->prevViewLastRec=a->prevViewLastUsedRec=NULL;

  a->views=CreateGroup();
  a->activeAppView=NULL;
  a->cancelToolFlag=0;
  a->updateLocks=0;
  a->highlightMode=0;
  a->highlightLocks=0;

  a->bStrict=0;
  a->errorCode=0;
  a->errorObject=a->errorCausedBy=0;

  a->minX=a->minY=-1;
  a->maxX=a->maxY=1;
  a->xyAngle=0;
  a->showFlags=SHW_AXES|SHW_ELEMS|SHW_NODES|SHW_IRRNODES|SHW_TEMPLATE|
    SHW_SEPARATORS|SHW_MENU|SHW_TOOLBAR|SHW_CHORDS|SHW_3DCHORDS|
    SHW_EQUIL|SHW_SURFACES|SHW_GRIDPOINTS|SHW_GRID|SHW_LABELS|SHW_MESH|
    SHW_XPOINTTESTS;
  a->outputFlags=0;
  a->chFlags=0;

  a->meshSlidingMode=MSM_SURFACE;
  a->meshSlidingThreshold=1;
  a->bDoubleMeshBorder=1;

  a->highlight=CreateGroup();
  a->mark=CreateGroup();

  a->nodes=CreateGroup();
  a->elems=CreateGroup();
  a->separators=CreateGroup();
  a->sources=CreateGroup();
  a->chords=CreateGroup();
  a->xpointTests=CreateGroup();
  a->xPointSegs=CreateGroup();
  a->gridPointSegs=CreateGroup();
  a->gridPointsEx=CreateGroup();
  a->surfaceZones=CreateGroup();
  a->surfacesEx=CreateGroup();

  a->varSetDefs=CreateGroup();
  a->varSets=CreateGroup();
  a->varDefs=CreateGroup();
  a->equil=NULL;
  a->template=NULL;
  a->sonnetData=NULL;
  a->mesh=NULL;

  a->maxElemId=0;
  a->outputMode=OUTPUTMODE_SONNET;
  a->fName=NULL;
  a->creationTime=NULL;
  a->topologyName=NULL;

  return a;
}

void* FreeApp(App a) {
  Node n;
  Elem e;
  VarSetDef vsd;
  VarSet vs;
  Separator sep;
  Source src;
  Chord ch;
  XPointTest xpt;
  GridPointSeg gps;
  GridPointEx gpx;
  SurfaceZone sz;
  SurfaceEx sx;
  Index ix;

  ValidatePtr(a,"FreeApp");
  if (GetObjType(a)!=T_APP) FatalError("FreeApp()-type: fatal error 1");

  EnableUndo(a,0);

  SetAppFName(a,NULL);
  SetAppCreationTime(a,NULL);
  SetTopologyName(a,NULL);

  if (!IsEmptyGroup(a->views))
    FatalError("FreeApp()-views: fatal error 1");

  if (a->sonnetData!=NULL) DelSonnetData(a);
  for (vsd=AppVarSetDef1st(a,&ix);vsd!=NULL;vsd=Next(&ix))
    ChangeVarSetDef(a,vsd,vsd->name,vsd->descr,0,vsd->maxVarSets,NULL);
  for (vs=AppVarSet1st(a,&ix);vs!=NULL;vs=Next(&ix))
    DelVarSet(a,vs);
  for (vsd=AppVarSetDef1st(a,&ix);vsd!=NULL;vsd=Next(&ix))
    DelVarSetDef(a,vsd);

  for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix))
    DelGridPointEx(gpx);
  for (gps=Group1st(a->gridPointSegs,&ix);gps!=NULL;gps=Next(&ix))
    DelGridPointSeg(gps);
  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    DelXPointTest(a,xpt);
  for (ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix))
    DelChord(a,ch);
  for (src=AppSource1st(a,&ix);src!=NULL;src=Next(&ix))
    DelSource(a,src);
  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    DelSeparator(a,sep);
  for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix)) DelSurfaceEx(sx);
  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix)) DelSurfaceZone(sz);

  for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix)) DelElem(a,e);
  for (n=AppNode1st(a,&ix);n!=NULL;n=Next(&ix)) DelNode(a,n);
  if (a->equil!=NULL) DelEquil(a);
  if (a->template!=NULL) DelTemplate(a);
  if (a->mesh!=NULL) DelMesh(a->mesh);

  assert(IsEmptyGroup(a->xPointSegs));
  assert(IsEmptyGroup(a->gridPointSegs));

  FreeUndoInfo(a);

  FreeGroup(a->gridPointSegs);
  FreeGroup(a->xPointSegs);
  FreeGroup(a->xpointTests);
  FreeGroup(a->nodes);
  FreeGroup(a->elems);
  FreeGroup(a->surfacesEx);
  FreeGroup(a->gridPointsEx);
  FreeGroup(a->separators);
  FreeGroup(a->sources);
  FreeGroup(a->chords);
  FreeGroup(a->surfaceZones);

  FreeGroup(a->varSets);
  FreeGroup(a->varDefs);
  FreeGroup(a->varSetDefs);

  FreeGroup(a->highlight);
  FreeGroup(a->mark);

  FreeGroup(a->undoStack);
  FreeGroup(a->redoStack);
  FreeGroup(a->views);

  Free(a);

  return NULL;
}

App AddApp(XApp xap) {
  App a;
  int err;
  char s[500];
  char* msg=NULL;
  time_t timer;

  if (xap->config==NULL || xap->config->fName==NULL) a=CreateApp();
  else {
    a=LoadApp(xap,xap->config->fName,&msg,&err);
    if (msg!=NULL) Free(msg);
  }
  if (a!=NULL) {
    strcpy(s,asctime(localtime((time(&timer),&timer))));
    if (*s && s[strlen(s)-1]=='\n') s[strlen(s)-1]=0;
    SetAppFName(a,NULL);
    SetAppCreationTime(a,s);
    SetTopologyName(a,NULL);
    FreeUndoInfo(a);
  }
  return a;
}

void* CreateActRec(size_t size,ActProc ap) {
  ActRec ar;

/*  ValidatePtr((void*)ap,"CreateActRec");
*/
  ar=Malloc(size);
  ar->actProc=ap;
  ar->obj=NULL;

  return ar;
}

void* FreeActRec(void* ar) {
  ValidatePtr(ar,"FreeActRec");

  if (((ActRec)ar)->obj!=NULL) Free(((ActRec)ar)->obj);

  Free(ar);

  return NULL;
}

static void FreeUndoList(Group list) {
  Index ix;
  ActRec ar;

  ValidatePtr(list,"FreeUndoList");

  for (ar=Group1st(list,&ix);ar!=NULL;ar=Next(&ix)) {
    GroupDel(list,ar);
    FreeActRec(ar);
  }
}

void FreeUndoInfo(App a) {
  ValidatePtr(a,"FreeUndoInfo");

  FreeUndoList(a->undoStack);
  FreeUndoList(a->redoStack);
  NotifyAlt(a);
}

void AddUndoRec(App a,ActRec rec) {
  Group g;
  Index ix;
  ChangeNodeRec cnr;
  ChangeObjGroupRec cogr;
  ChangeObjDoubleRec codr;
  ChangeObjIntRec coir;
  ChangeElemRec cer;
  ChangeSeparatorRec csepr;
  ChangeSourceRec csrcr;
  ChangeMeshPointRec cmptr;
  ChangeSurfaceExRec cser,cser1;
  ChangeGridPointExRec cgxr;
  UndoMarkRec umr;

  ValidatePtr(a,"AddUndoRec");
  ValidatePtr(rec,"AddUndoRec_");

  if (a->undoMode==UM_NORM || a->undoMode==UM_REDO) g=a->undoStack; else
      if (a->undoMode==UM_UNDO) g=a->redoStack; else {
    FreeActRec(rec);
    return;
  }

/* Discard redundant UndoMark's
*/
  if (rec->actProc==(ActProc)ActUndoMark)
    if (umr=Group1st(g,&ix),umr!=NULL && umr->actProc==rec->actProc)
      {FreeActRec(rec);return;}

/* Discard redundant ChangeObjIntRec's
*/
  if (!IsEmptyGroup(a->highlight) &&
      rec->actProc==(ActProc)ActChangeObjInt)
    for (coir=Group1st(g,&ix);!IsUndoMark(coir);coir=Next(&ix))
      if (coir->actProc==rec->actProc &&
          coir->objChange==((ChangeObjIntRec)rec)->objChange &&
          coir->fieldOffset==((ChangeObjIntRec)rec)->fieldOffset &&
          coir->length==((ChangeObjIntRec)rec)->length &&
          coir->bRedraw==((ChangeObjIntRec)rec)->bRedraw)
        {FreeActRec(rec);return;}

/* Discard redundant ChangeObjDoubleRec's
*/
  if (!IsEmptyGroup(a->highlight) &&
      rec->actProc==(ActProc)ActChangeObjDouble)
    for (codr=Group1st(g,&ix);!IsUndoMark(codr);codr=Next(&ix))
      if (codr->actProc==rec->actProc &&
          codr->objChange==((ChangeObjDoubleRec)rec)->objChange &&
          codr->fieldOffset==((ChangeObjDoubleRec)rec)->fieldOffset &&
          codr->length==((ChangeObjDoubleRec)rec)->length &&
          codr->bRedraw==((ChangeObjDoubleRec)rec)->bRedraw)
        {FreeActRec(rec);return;}

/* Discard redundant ChangeObjGroup's
*/
  if (rec->actProc==(ActProc)ActChangeObjGroup)
    for (cogr=Group1st(g,&ix);!IsUndoMark(cogr);cogr=Next(&ix))
      if (cogr->actProc==rec->actProc &&
          cogr->objChange==((ChangeObjGroupRec)rec)->objChange &&
          cogr->member==((ChangeObjGroupRec)rec)->member &&
          cogr->fieldOffset==((ChangeObjGroupRec)rec)->fieldOffset &&
          !!cogr->status==!((ChangeObjGroupRec)rec)->status)
        {FreeActRec(rec);GroupDel(g,cogr);FreeActRec(cogr);return;}

/* Discard redundant ChangeNode's
*/
  if (rec->actProc==(ActProc)ActChangeNode)
    for (cnr=Group1st(g,&ix);cnr!=NULL && cnr->actProc==rec->actProc;
        cnr=Next(&ix))
      if (cnr->n==((ChangeNodeRec)rec)->n) {FreeActRec(rec);return;}

/* Discard redundant ChangeElem's
*/
  if (rec->actProc==(ActProc)ActChangeElem)
    for (cer=Group1st(g,&ix);cer!=NULL && cer->actProc==rec->actProc;
        cer=Next(&ix))
      if (cer->e==((ChangeElemRec)rec)->e) {FreeActRec(rec);return;}

/* Discard redundant ChangeGridPointEx's
*/
  if (rec->actProc==(ActProc)ActChangeGridPointEx)
    for (cgxr=Group1st(g,&ix);cgxr!=NULL && cgxr->actProc==rec->actProc;
        cgxr=Next(&ix))
      if (cgxr->gpx==((ChangeGridPointExRec)rec)->gpx) {FreeActRec(rec);return;}

/* Discard redundant ChangeSeparators's
*/
  if (rec->actProc==(ActProc)ActChangeSeparator)
    for (csepr=Group1st(g,&ix);csepr!=NULL && csepr->actProc==rec->actProc;
        csepr=Next(&ix))
      if(csepr->sep==((ChangeSeparatorRec)rec)->sep){FreeActRec(rec);return;}

/* Discard redundant ChangeSource's
*/
  if (rec->actProc==(ActProc)ActChangeSource)
    for (csrcr=Group1st(g,&ix);csrcr!=NULL && csrcr->actProc==rec->actProc;
        csrcr=Next(&ix))
      if (csrcr->src==((ChangeSourceRec)rec)->src) {FreeActRec(rec);return;}

/* Discard redundant ChangeSurfaceEx's
*/
  if (rec->actProc==(ActProc)ActChangeSurfaceEx)
    for (cser=Group1st(g,&ix);cser!=NULL && cser->actProc==rec->actProc;
        cser=Next(&ix))
      if (cser->sx==((ChangeSurfaceExRec)rec)->sx) {FreeActRec(rec);return;}

/* Discard redundant ChangeMeshPoint's
*/
  if (rec->actProc==(ActProc)ActChangeMeshPoint)
    for (cmptr=Group1st(g,&ix);cmptr!=NULL && !IsUndoMark(cmptr);
        cmptr=Next(&ix))
      if (cmptr->mpt==((ChangeMeshPointRec)rec)->mpt) {
        FreeActRec(rec);return;
      }

  GroupAdd(g,rec);

  if (rec->actProc==(ActProc)ActUndoMark) {
    switch(a->undoMode) {
      case UM_NORM:
        if (a->alt<0) a->alt=GroupCount(a->undoStack);
        a->alt++;
        break;
      case UM_REDO:
        a->alt++;
        break;
      case UM_UNDO:
        a->alt--;
        break;
    }
    NotifyAlt(a);
  }
}

static void ProcessUndo(App a) {
  ActRec ar;
  Stack g;
  Index ix;
  int b,oldMode;

  if (a->cancelToolFlag>1) return;
  a->cancelToolFlag++;

  if (!a->bNonDestructiveCancel) {
    if (a->activeAppView!=NULL && a->activeAppView->activeTool!=NULL) {
      CallToolProc(a->activeAppView,a->activeAppView->activeTool,
        TL_CANCEL,0,0);
      if (a->activeAppView!=NULL && a->activeAppView->activeTool!=NULL)
        FatalError("ProcessUndo()-cancelTool: fatal error 2");
    }
  }
  a->cancelToolFlag--;

  if (a->undoMode==UM_NORM) FatalError("ProcessUndo()-mode: fatal error 1");
  ar=Group1st(g=a->undoMode==UM_REDO ? a->redoStack : a->undoStack,&ix);
  if (ar==NULL) return;
  b= ar->actProc==(ActProc)ActUndoMark;
  if (b && a->undoMode==UM_CANCEL) return;
  oldMode=a->undoMode;

  do {
    GroupDel(g,ar);
    if (!b) a->undoMode=UM_CANCEL;
    if (ar->actProc!=(ActProc)ActUndoMark) ar->actProc(a,ar);
    if (!b) a->undoMode=oldMode;
    FreeActRec(ar);
    ar=Next(&ix);
  } while (!IsUndoMark(ar));

  if (b) UndoMark(a);
  NotifyAlt(a);
}

void Undo(App a) {
  int b;

  ValidatePtr(a,"Undo");
  assert(!a->bNonDestructiveCancel);

  b=a->undoMode;
  a->undoMode=UM_UNDO;
  ProcessUndo(a);
  a->undoMode=b;
}

void Redo(App a) {
  int b;

  ValidatePtr(a,"Redo");
  assert(!a->bNonDestructiveCancel);

  b=a->undoMode;
  a->undoMode=UM_REDO;
  ProcessUndo(a);
  a->undoMode=b;
}

void Cancel(App a) {
  int b;

  ValidatePtr(a,"Cancel");
  assert(!a->bNonDestructiveCancel);

  b=a->undoMode;
  a->undoMode=UM_CANCEL;
  ProcessUndo(a);
  a->undoMode=b;
}

void CancelNonDestructive(App a) {
  int b,b1;

  b=a->undoMode;
  b1=a->bNonDestructiveCancel;

  a->undoMode=UM_CANCEL;
  a->bNonDestructiveCancel=1;

  ProcessUndo(a);

  a->bNonDestructiveCancel=b1;
  a->undoMode=b;
}

void UndoMark(App a) {
  struct _ActRec ar;

  ValidatePtr(a,"UndoMark");
  ActUndoMark(a,&ar);
}

void EnableUndo(App a,int bEnable) {
  a->undoMode=bEnable? UM_NORM : UM_CANCEL;
  FreeUndoInfo(a);
}

void DrawAppObject(App a,void* obj,int mode) {
  Index ix;
  View w;

  ValidatePtr(a,"DrawAppObject");
  ValidatePtr(obj,"DrawAppObject_");

  for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix))
    DrawObject(w,obj,mode);
}

void SetHighlightMode(App a,int mode) {
  if (mode==a->highlightMode) return;

  DrawAppHighlight(a,DRAW_OFF);

  a->highlightMode=mode;

  DrawAppHighlight(a,DRAW_ON);
}

void DrawAppHighlight(App a,int mode) {
  View w;
  Index ix;
  void* p;

  ValidatePtr(a,"DrawAppHighlight");

  switch(mode) {
    case DRAW_ON:
       assert(a->highlightLocks);
       if (a->highlightLocks<0) a->highlightLocks++; else {
         if (!--a->highlightLocks)
           for (p=AppHighlight1st(a,&ix);p!=NULL;p=Next(&ix))
             DrawAppObject(a,p,mode);
       }
       for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix)) {
         DrawHighlightRect(w,DRAW_ON);
         DrawAllViewShapes(w,DRAW_ON);
       }
       break;
    case DRAWHI_UNLOCK:
      assert(a->highlightLocks);
      a->highlightLocks--;
      break;
    case DRAWHI_LOCK:
      if (a->highlightLocks<0) DrawAppHighlight(a,DRAW_OFF);
      else a->highlightLocks++;
      break;
    case DRAWHI_CNDOFF:
      if (a->highlightLocks<=0) a->highlightLocks--;
      else a->highlightLocks++;
      break;
    case DRAW_OFF:
      if (a->highlightLocks<=0) {
        for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix)) {
          DrawHighlightRect(w,DRAW_OFF);
          DrawAllViewShapes(w,DRAW_OFF);
        }
        for (p=AppHighlight1st(a,&ix);p!=NULL;p=Next(&ix))
          DrawAppObject(a,p,mode);
        a->highlightLocks=-a->highlightLocks;
      }
      a->highlightLocks++;
      break;
    default:
      assert(0);
  }
}

void AddAppUpdate(App a) {
  View w;
  Index ix;

  ValidatePtr(a,"AddAppUpdate");

  for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix))
    if (w->app==NULL || !w->app->updateLocks) w->updateFlag=UPDATE_MAX;
}

void LockAppUpdate(App a,int incr) {
  struct _LockAppUpdateRec ar;

  ValidatePtr(a,"LockAppUpdate");

  ar.bRepaintAfter=1;
  ar.updateLocks=a->updateLocks+incr;
  if (ar.updateLocks<0) FatalError("LockAppUpdate()-<0: fatal error 1");


  ActLockAppUpdate(a,&ar);
}

void DisableAppUpdate(App a,int incr) {
  struct _LockAppUpdateRec ar;

  ar.bRepaintAfter=0;
  ar.updateLocks=a->updateLocks+incr;
  if (ar.updateLocks<0) FatalError("EnableAppUpdate()-<0: fatal error 1");

  ActLockAppUpdate(a,&ar);
}

void SetAppFName(App a,char* fName) {
  struct _ChangeObjStringRec ar;

  ValidatePtr(a,"SetAppFName");

  if (fName!=NULL && *fName) fName=ExpandFilename(fName);

  if (fName==a->fName) return;
  ar.obj=fName==NULL ? NULL : MallocString(fName);
  ar.objChange=a;
  ar.fieldOffset=GetOffset(App,fName);
  ar.bRedraw=0;

  ActChangeObjString(a,&ar);
}

void SetAppCreationTime(App a,char* creaTime) {
  struct _ChangeObjStringRec ar;

  ValidatePtr(a,"SetAppCreationTime");

  if (a->creationTime==creaTime) return;
  ar.obj=creaTime==NULL ? NULL : MallocString(creaTime);
  ar.objChange=a;
  ar.fieldOffset=GetOffset(App,creationTime);
  ar.bRedraw=0;

  ActChangeObjString(a,&ar);
}

void* DelObject(App a,void* obj) {
  ValidatePtr(a,"DelObject");
  ValidatePtr(obj,"DelObject_");

  switch(GetObjType(obj)) {
    case T_NODE:
      return DelNode(a,obj);
    case T_ELEM:
      return DelElem(a,obj);
    case T_SURFACEEX:
      return DelSurfaceEx(obj);
    case T_GRIDPOINTEX:
      return DelGridPointEx(obj);
    case T_SEPARATOR:
      RemoveAllSeparators(a);
      return NULL;
    case T_SOURCE:
      DelSource(a,obj);
      return NULL;
    case T_CHORD:
      DelChord(a,obj);
      return NULL;
    case T_TEMPLATE:
      DelTemplate(a);return NULL;
    case T_EQUIL:
      DelEquil(a);return NULL;
    case T_XPOINTTEST:
      DelXPointTest(a,obj);return NULL;
    case T_XPOINTSEG:
      DelXPointSeg(a,obj);return NULL;
    case T_GRIDPOINTSEG:
      return DelGridPointSeg(obj);
    case T_SURFACEZONE:
      return DelSurfaceZone(obj);
    case T_SONNET:
      DelSonnetData(a);return NULL;
    case T_MESH:
      DelMesh(obj);return NULL;
    case T_VAR:
      SetVar(a,((Var)obj)->origin,((Var)obj)->def,((Var)obj)->varSet,NULL);
      return NULL;
    case T_VARDEF:
      return DelVarDef(a,obj);
    case T_VARSET:
      return DelVarSet(a,obj);
    case T_VARSETDEF:
      return DelVarSetDef(a,obj);
    default:
      FatalError("DelObject()-type%d: fatal error 1",GetObjType(obj));
      return NULL;
  }
}

void NotifyAppViews(App a,unsigned type,void* object) {
  View w;
  GridPointSeg gps;
  SurfaceZone sz;
  Index ix;
  Var v;

  /* Pre-processing of certain events */

  /* Update chFlags */
  if (type!=N_EXAMINE && object!=NULL) {
    switch(GetObjType(object)) {
      case  T_ELEM:
        a->chFlags |= CHF_GEOMETRY;
        if ((type==N_ADDED || type==N_CHANGED || type==N_DEL) &&
            ElementInTarget((Elem)object))
          a->chFlags |= CHF_TARGETS;
        break;
      case  T_NODE:
        a->chFlags |= CHF_GEOMETRY;
        if (type==N_CHANGED && PointInTarget((Node)object))
          a->chFlags |= CHF_TARGETS;
        break;
      case  T_SEPARATOR:
      case  T_SOURCE:
      case  T_CHORD:
      case  T_GRIDPOINTEX:
      case  T_SURFACEEX:
        a->chFlags |= CHF_GEOMETRY;
        break;
      case  T_EQUIL:
      case  T_TEMPLATE:
      case  T_SONNET:
        a->chFlags |= CHF_FILES;
        break;
      case  T_VAR:
        a->chFlags |= CHF_VARS;
        v=(Var)object;
        if (type==N_CHANGED &&
            GetObjType(((Var)object)->origin)==T_VARSETDEF)
          a->chFlags |= CHF_VARDEFS;
        if (v->def->varType & VTF_TARGET) a->chFlags |= CHF_TARGETS;
        break;
      case  T_VARSETDEF:
      case  T_VARDEF:
      case  T_VARSET:
        a->chFlags |= CHF_VARDEFS/*|CHF_VARSMENU*//*CHF_TARGETS*/;
        break;
      case  T_APP:
      case  T_VIEW:
        /* a->chFlags |= CHF_APPVIEW; */
        break;
      case T_XPOINTTEST:
      case T_XPOINTSEG:
      case T_GRIDPOINTSEG:
      case T_SURFACEZONE:
        a->chFlags |= CHF_TOPOLOGY;
        InvalidateTopologyCache(a);
        break;
      case T_MESH:
      case T_MESHCELL:
      case T_MESHELEMENT:
      case T_MESHPOINT:
        a->chFlags |= CHF_MESH;
        break;
      default:
        assert(0);
    }
  }

  /* Special processing before notifying views */

  if (type==N_ALT) {
    if (a->chFlags & (CHF_TARGETS|CHF_TOPOLOGY)) {
      for (gps=AppGridPointSeg1st(a,&ix);gps!=NULL;gps=Next(&ix))
        RecalcGridPointSegLine(gps,NULL,NULL);
    }
    if (a->chFlags & CHF_TOPOLOGY) {
      for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
        RecalcSurfaceZoneSign(sz);
      GroupQSort(a->surfaceZones,SurfaceZoneSortProc,NULL);
      GroupQSort(a->gridPointSegs,GridPointSegSortProc,NULL);
      UpdateAfterTopologyChange(a);
    }
  }
  /* Pass the message to views */

  for (w=AppView1st(a,&ix);w!=NULL;w=Next(&ix)) NotifyView(w,type,object);

  /* Zero out chFlags after N_ALT */

  if (type==N_ALT) {
    a->chFlags=0;
  }
}

int IsAppUnsaved(App a) {
  ActRec ar;
  Index iar;

  if (!a->alt) return 0;

  for (ar=Group1st(a->undoStack,&iar);ar!=NULL;ar=Next(&iar)) {
    if (ar->actProc==(ActProc)ActChangeAppView) continue;
    if (ar->actProc==(ActProc)ActUndoMark) continue;
    return 1;
  }

  return 0;
}

int RaiseChangeAppViewUndoRec(App a,int bTestOnly) {
  ActRec ar,ar1;
  Index iar;

  for (ar=Group1st(a->undoStack,&iar);ar!=NULL;ar=Next(&iar)) {
    if (ar->actProc==(ActProc)ActChangeAppView) break;
  }

  if (ar==NULL) return -1;

  if (bTestOnly) return 0;

  ar1=Next(&iar);
  if (ar1==NULL || ar->actProc==(ActProc)ActUndoMark) {
    ar1=Prev(&iar);
    ar1=Prev(&iar);
    if (ar1!=NULL && ar1->actProc==(ActProc)ActUndoMark) {
      GroupDel(a->undoStack,ar1);
      ar1=FreeActRec(ar1);
    }
  }

  GroupDel(a->undoStack,ar);
  GroupAdd(a->undoStack,ar);
  UndoMark(a);

  return 0;
}

int GetPrevViewInfo(App a,double* pMinX,double* pMinY,double* pMaxX,double*
    pMaxY,double* pXyAngle,int bCheckOnly) {
  int bStopNext;
  ChangeAppViewRec car;
  ActRec ar;
  Index iar;

  for (ar=Group1st(a->undoStack,&iar);ar!=NULL;ar=Next(&iar)) {
    if (ar->actProc==(ActProc)ActChangeAppView) break;
  }

  if (ar==NULL) return -1;

  if (ar!=a->prevViewLastRec) {
    if (bCheckOnly) return 0;
    goto Found;
  }

  bStopNext=0;
  while ((ar=Next(&iar))!=NULL) {
    if (ar->actProc!=(ActProc)ActChangeAppView) continue;
    if (bStopNext) break;
    if (ar==a->prevViewLastUsedRec) bStopNext=1;
  }
  if (ar==NULL) return -1;

  Found:

  if (!bCheckOnly) {
    a->prevViewLastUsedRec=ar;
    a->prevViewLastRec=NULL;
  }
  car=(ChangeAppViewRec)ar;

  if (pMinX!=NULL) *pMinX=car->minX;
  if (pMinY!=NULL) *pMinY=car->minY;
  if (pMaxX!=NULL) *pMaxX=car->maxX;
  if (pMaxY!=NULL) *pMaxY=car->maxY;
  if (pXyAngle!=NULL) *pXyAngle=car->xyAngle;

  return 0;
}

void MarkPrevViewDone(App a) {
  ActRec ar;
  Index iar;

  a->prevViewLastRec=NULL;
  for (ar=Group1st(a->undoStack,&iar);ar!=NULL;ar=Next(&iar)) {
    if (ar->actProc==(ActProc)ActChangeAppView) {
      a->prevViewLastRec=ar;
      break;
    }
  }
}

void SetAppOutputMode(App a,int newMode) {
  switch (newMode) {
    case OUTPUTMODE_SONNET:
    case OUTPUTMODE_CARRE:
      break;
    default:
      assert(0);
  }
  SetObjInt(a,a,GetOffset(App,outputMode),sizeof(a->outputMode),newMode,0);
}

void SetOutputFlags(App a,long flags) {
  SetObjInt(a,a,GetOffset(App,outputFlags),sizeof(a->outputFlags),flags,0);
}

void SetMeshSlidingMode(App a,int mode) {
  SetObjInt(a,a,GetOffset(App,meshSlidingMode),sizeof(a->meshSlidingMode),
      mode,0);
}

void SetMeshSlidingThreshold(App a,double value) {
  SetObjDouble(a,a,GetOffset(App,meshSlidingThreshold),value,0);
}

void SetDoubleMeshBorderFlag(App a,int flag) {
  SetObjInt(a,a,GetOffset(App,bDoubleMeshBorder),sizeof(a->bDoubleMeshBorder),
      flag,0);
}

void SetTopologyName(App a,char* name) {
  SetObjString(a,a,GetOffset(App,topologyName),name,0);
}

char* GetTopologyName(App a) {
  return a->topologyName!=NULL ? a->topologyName : "*";
}
