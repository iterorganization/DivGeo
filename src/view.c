#include "dg.h"

#define MAKESTREX_BUF 3000
#define MAKESTREX_CHR '\x1'
#define STATICSTR_LEN 4000
#define STATICSTR_CNT 20

#define MIN_RECTW     1e-4
#define MIN_RECTH     1e-4

static void VoidProc() {
}

static char* VoidGetStringProc() {
  return "Strings not supported";
}

/* Create a new View object
*/
View CreateAbstractView() {
  View w;
  typedef void(*VoidProc0)(View);

  w=Malloc(sizeof(*w));

  w->type=T_VIEW;
  w->x=NULL;
  w->UnconfigureProc=(void(*)(View))VoidProc;
  w->AdjustAspectRatioProc=(void(*)(View))VoidProc;
  w->SetModeProc=(void(*)(View,int))VoidProc;
  w->DrawLineProc=(void(*)(View,double,double,double,double))VoidProc;
  w->DrawRectProc=(void(*)(View,double,double,double,double))VoidProc;
  w->DrawCircleProc=(void(*)(View,double,double,double))VoidProc;
  w->DrawTextProc=(void(*)(View,double,double,char*))VoidProc;
  w->SetMsgProc=(void(*)(View,char*))VoidProc;
  w->ClearProc=(void(*)(View))VoidProc;
  w->FlushProc=(void(*)(View))VoidProc;
  w->NotifyProc=(void(*)(View,int,void*))VoidProc;
  w->GetStrProc=(char*(*)(View,int))VoidGetStringProc;

  w->app=NULL;
  w->activeTool=NULL;
  w->toolData=NULL;

  w->lastExaminedSurfaceEx=NULL;
  w->lastExaminedGridPointEx=NULL;

  w->labels=CreateGroup();
  w->shapes=CreateGroup();
  w->updateFlag=0;
  w->stretchPower=1;

  w->width=w->height=1;
  w->minX=w->minY=-1;
  w->maxX=w->maxY=1;
  w->centerX=w->centerY=0;
  w->zoomX=w->zoomY=1;
  w->showFlags=~0L;
  w->xyAngle=0;

  w->xScaleSign=1;
  w->yScaleSign=1;

  w->bEditTopology=0;

  w->hrMinX=w->hrMaxX=w->hrMinY=w->hrMaxY=w->bHrShown=0;

  return w;
}

/* Free a View object
*/
void* FreeView(View w) {
  if (w->app!=NULL)
    FatalError("FreeView()-app : fatal error 1");

  NotifyView(w,N_DESTROYVIEW,NULL);

  RemoveAllViewLabels(w);
  ClearViewShapes(w);
  w->UnconfigureProc(w);
  assert(w->x==NULL);
  FreeGroup(w->labels);

  assert(IsEmptyGroup(w->shapes));
  w->shapes=FreeGroup(w->shapes);

  return Free(w);
}

/* Conversions between real and window coordinates
*/

int ScreenX(View w,double x) {
  return (x-w->centerX)*w->zoomX*w->xScaleSign+w->width/2;
}

int ScreenY(View w,double y) {
  return (y-w->centerY)*w->zoomY*w->yScaleSign+w->height/2;
}

double FScreenX(View w,double x) {
  return (x-w->centerX)*w->zoomX*w->xScaleSign+w->width/2;
}

double FScreenY(View w,double y) {
  return (y-w->centerY)*w->zoomY*w->yScaleSign+w->height/2;
}

double RealX(View w,int x) {
  return (x-w->width/2)/w->zoomX/w->xScaleSign+w->centerX;
}

double RealY(View w,int y) {
  return (y-w->height/2)/w->zoomY/w->yScaleSign+w->centerY;
}

/* Set visible rectangle (zoom). The rectangle will be expanded to ensure
   proper aspect ratio
*/

void SetViewRect(View w,double x1,double y1,double x2,double y2) {
  struct _ChangeAppViewRec ar;

  ValidatePtr(w,"SetViewRect");

  assert(w->width>0);
  assert(w->height>0);

  if (x2<x1) swap(x1,x2);
  if (y2<y1) swap(y1,y2);

  if (x2-x1<MIN_RECTW || y2-y1<MIN_RECTH) {
    x1=(x1+x2)/2;
    x2=x1+MIN_RECTW/2;
    x1=2*x1-x2;
    y1=(y1+y2)/2;
    y2=y1+MIN_RECTH/2;
    y1=2*y1-y2;
  }

  if (w->app==NULL || w->app->activeAppView!=w) {
    w->centerX=(x1+x2)/2;
    w->centerY=(y1+y2)/2;
    w->zoomX=w->width/(x2-x1);
    w->zoomY=w->height/(y2-y1);
    if (!(w->showFlags & SHW_STRETCH)) AdjustViewAspectRatio(w);
    w->minX=w->centerX-w->width/w->zoomX/2.;
    w->minY=w->centerY-w->height/w->zoomY/2.;
    w->maxX=w->centerX+w->width/w->zoomX/2.;
    w->maxY=w->centerY+w->height/w->zoomY/2;
    w->xyAngle=w->xyAngle;
    ClearView(w);
    if (w->app!=NULL) RepaintView(w);
  } else {
    ar.mode=CVM_ZOOM;
    ar.minX=x1;
    ar.minY=y1;
    ar.maxX=x2;
    ar.maxY=y2;
    ar.xyAngle=w->xyAngle;
    ar.showFlags=w->showFlags;

    ActChangeAppView(w->app,&ar);
  }

  assert(w->minX<w->maxX);
  assert(w->minY<w->maxY);
}

/* Set zoom factor and origin for the center of the View window
*/

void SetViewFactor(View w,double cx,double cy,double zx,double zy) {
  SetViewRect(w,cx-w->width/2/zx,cy-w->height/2/zy,
    cx+w->width/2/zx,cy+w->height/2/zy);
}

void SetRotatedViewFactor(View w,double cx,double cy,double zx,double zy,
    double th) {
  SetView(w,cx-w->width/2/zx,cy-w->height/2/zy,
    cx+w->width/2/zx,cy+w->height/2/zy,th);
}

/* Set angle at which the view is displayed
*/
void SetViewAngle(View w,double th) {
  struct _ChangeAppViewRec ar;

  ValidatePtr(w,"SetViewXYAngle");
  th=fmod(th,2*M_PI);
  if (th<0) th+=2*M_PI;

  if (w->app==NULL || w->app->activeAppView!=w) {
    w->xyAngle=th;
    ClearView(w);
    RepaintView(w);
  } else {
    ar.mode=CVM_ZOOM;
    ar.minX=w->minX;
    ar.minY=w->minY;
    ar.maxX=w->maxX;
    ar.maxY=w->maxY;
    ar.xyAngle=th;
    ar.showFlags=w->showFlags;
    ActChangeAppView(w->app,&ar);
  }
}

void SetView(View w,double x1,double y1,double x2,double y2,double th) {
  struct _ChangeAppViewRec ar;

  ValidatePtr(w,"SetView");

  assert(w->width>0);
  assert(w->height>0);

  if (x2<x1) swap(x1,x2);
  if (y2<y1) swap(y1,y2);

  if (x2-x1<MIN_RECTW || y2-y1<MIN_RECTH) {
    x1=(x1+x2)/2;
    x2=x1+MIN_RECTW/2;
    x1=2*x1-x2;
    y1=(y1+y2)/2;
    y2=y1+MIN_RECTH/2;
    y1=2*y1-y2;
  }

  if (th!=0) th=fmod(th,2*M_PI);
  if (th<0) th+=2*M_PI;

  if (w->app==NULL || w->app->activeAppView!=w) {
    w->centerX=(x1+x2)/2;
    w->centerY=(y1+y2)/2;
    w->zoomX=w->width/(x2-x1);
    w->zoomY=w->height/(y2-y1);
    if (!(w->showFlags & SHW_STRETCH)) AdjustViewAspectRatio(w);
    w->minX=w->centerX-w->width/w->zoomX/2.;
    w->minY=w->centerY-w->height/w->zoomY/2.;
    w->maxX=w->centerX+w->width/w->zoomX/2.;
    w->maxY=w->centerY+w->height/w->zoomY/2;
    w->xyAngle=th;
    ClearView(w);
    if (w->app!=NULL) RepaintView(w);
  } else {
    ar.mode=CVM_ZOOM;
    ar.minX=x1;
    ar.minY=y1;
    ar.maxX=x2;
    ar.maxY=y2;
    ar.xyAngle=th;
    ar.showFlags=w->showFlags;

    ActChangeAppView(w->app,&ar);
  }

  assert(w->minX<w->maxX);
  assert(w->minY<w->maxY);
}

/* Set flags for displaying different kinds of objects
*/

void SetViewFlags(View w,unsigned long flags) {
  struct _ChangeAppViewRec ar;

  ValidatePtr(w,"SetViewFlags");
  if (w->showFlags==flags) return;

  if (w->app==NULL || w->app->activeAppView!=w) {
    w->showFlags=flags;
  } else {
    ar.mode=CVM_ZOOM;
    ar.minX=w->minX;
    ar.minY=w->minY;
    ar.maxX=w->maxX;
    ar.maxY=w->maxY;
    ar.xyAngle=w->xyAngle;
    ar.showFlags=flags;

    ActChangeAppView(w->app,&ar);
  }
}

/* Notify current window change for proper Undo/Redo operations
*/

static void ChangeActiveAppView(App a,View w) {
  struct _ChangeAppViewRec ar;

  if (w==a->activeAppView) return;

  a->activeAppView=w;

  if (w!=NULL) {
    ar.mode=CVM_NEWWINDOW;
    ar.minX=w->minX;
    ar.minY=w->minY;
    ar.maxX=w->maxX;
    ar.maxY=w->maxY;
    ar.xyAngle=w->xyAngle;
    ar.showFlags=w->showFlags;
    ActChangeAppView(a,&ar);
    UndoMark(a);
  }
}

/* Set App for a View
*/

void SetViewApp(View w,App a) {
/*  double minX,minY,maxX,maxY;
  unsigned long showFlags;*/

  ValidatePtr(w,"SetViewApp");

  NotifyNewApp(w);
  if (a==w->app) return;
  ClearView(w);
  RemoveAllViewLabels(w);
  if (w->app!=NULL) {
    if (w->activeTool!=NULL) Cancel(w->app);
    NotifyView(w,N_DEL,w->app);
    if (w->app->activeAppView==w) ChangeActiveAppView(w->app,NULL);
    GroupDel(w->app->views,w);
    NotifyView(w,N_DELETED,w->app);
    w->app=NULL;
  }

/*  NotifyNewApp(w); */

  if (a!=NULL) {
    SetViewFlags(w,a->showFlags);
    SetView(w,a->minX,a->minY,a->maxX,a->maxY,a->xyAngle);
    w->app=a;
    NotifyView(w,N_ADD,a);
    GroupAdd(w->app->views,w);

    if (w->app->activeAppView==NULL) ChangeActiveAppView(w->app,w);
    NotifyView(w,N_ADDED,a);

    RepaintView(w);
  }
  if (w->app!=NULL) NotifyNewApp(w);
}

/* Make a View active
*/

void SetActiveView(View w) {
  ValidatePtr(w,"SetActiveView");

  if (w->app!=NULL) Cancel(w->app);

  if (w->app!=NULL && w->app->activeAppView!=w)
    ChangeActiveAppView(w->app,w);

  return;
}

/* Change active tool for a View
*/

void SetActiveTool(View w,ToolProc tool) {
  if (w->activeTool==tool) return;
  if (tool!=NULL && w->activeTool!=NULL)
    FatalError("SetActiveTool()-tools: fatal error 1");
  w->activeTool=tool;
}

/* Call the tool procedure
*/

void CallToolProc(View w,ToolProc tool,int event,int mouseX,int mouseY) {
  double x,y;

  ValidatePtr(w,"CallToolProc");
  ValidatePtr((void*)tool,"CallToolProc_");

  if (w->app==NULL) return;
  if (w!=w->app->activeAppView) return;
  if (tool==NULL) tool=w->activeTool;
  if (tool==NULL) return;
  if (w->app->cancelToolFlag>1) return;

  w->app->cancelToolFlag++;
  if (event==TL_RELEASE || event==TL_CANCEL) SetActiveTool(w,NULL);
  DrawAppHighlight(w->app,DRAWHI_CNDOFF);
  x=RealX(w,mouseX);
  y=RealY(w,mouseY);
  ScreenRotate(w,-1,&x,&y);
  tool(w,event,x,y);
  DrawAppHighlight(w->app,DRAW_ON);
  SetActiveTool(w,w->toolData==NULL ? NULL : tool);
  w->app->cancelToolFlag--;
}

/* Draw a specified object
   Do not draw a highlighted
*/

void DrawObject(View w,void* obj,int mode) {

  ValidatePtr(w,"DrawObject");
  ValidatePtr(obj,"DrawObject_");
  assert(w->app!=NULL);

  if (IsHighlighted(w->app,obj) && w->app->highlightLocks>0) return;

  switch(GetObjType(obj)) {
    case T_NODE:
      DrawNode(w,obj,mode);
      break;
    case T_ELEM:
      DrawElem(w,obj,mode);
      break;
    case T_EQUIL:
      DrawEquil(w,obj,mode);
      break;
    case T_SURFACEEX:
      DrawSurfaceEx(w,obj,mode);
      break;
    case T_TEMPLATE:
      DrawTemplate(w,obj,mode);
      break;
    case T_XPOINTTEST:
      DrawXPointTest(w,obj,mode);
      break;
    case T_XPOINTSEG:
      DrawXPointSeg(w,obj,mode);
      break;
    case T_GRIDPOINTEX:
      DrawGridPointEx(w,obj,mode);
      break;
    case T_SONNET:
      DrawSonnetData(w,obj,mode);
      break;
    case T_VIEWLABEL:
      DrawViewLabel(w,obj,mode);
      break;
    case T_SEPARATOR:
      DrawSeparator(w,obj,mode);
      break;
    case T_SOURCE:
      DrawSource(w,obj,mode);
      break;
    case T_CHORD:
      DrawChord(w,obj,mode);
      break;
    case T_MESHELEMENT:
      DrawMeshElement(w,obj,mode);
      break;
    case T_MESHCELL:
      DrawMeshCell(w,obj,mode);
      break;
    case T_MESH:
      DrawWholeMesh(w,obj,mode);
      break;
    case T_MESHPOINT:
      break;
    case T_SURFACEZONE:
      DrawSurfaceZone(w,obj,mode);
      break;
    case T_GRIDPOINTSEG:
      DrawGridPointSeg(w,obj,mode);
    case T_VARSETDEF:
    case T_VARDEF:
    case T_VARSET:
    case T_VAR:
    case T_APP:
      break;
    default:
      FatalError("DrawObject()-type%d: fatal error 1",GetObjType(obj));
      break;
  }
}

/* Repaint window contents
*/

void RepaintView(View w) {
  Index ix;
  void* p;
  Node n;
  Elem e;
  ViewLabel vl;
  Separator sep;
  Source src;
  Chord ch;
  XPointTest xpt;
  XPointSeg xps;
  GridPointEx gpx;
  SurfaceZone sz;
  SurfaceEx sx;
  GridPointSeg gps;

  ValidatePtr(w,"RepaintView");
  if (w->app==NULL) return;

/* Hide highlight and lock highlight drawing
*/
  for (p=AppHighlight1st(w->app,&ix);p!=NULL;p=Next(&ix))
    DrawObject(w,p,DRAW_ERASE);
  DrawAppHighlight(w->app,DRAWHI_LOCK);

  DrawHighlightRect(w,DRAW_ERASE);
  DrawAllViewShapes(w,DRAW_ERASE);

/* Repaint objects
*/
  if (w->app->equil!=NULL) DrawObject(w,w->app->equil,DRAW_ON);
  DrawGrid(w);
  DrawAxes(w);
  if (w->showFlags & SHW_TOPVIEW) Draw3DObjects(w);
  else {
    if (w->app->template!=NULL) DrawObject(w,w->app->template,DRAW_ON);

    for (xpt=AppXPointTest1st(w->app,&ix);xpt!=NULL;xpt=Next(&ix))
      DrawObject(w,xpt,DRAW_ON);

    for (xps=AppXPointSeg1st(w->app,&ix);xps!=NULL;xps=Next(&ix))
      DrawObject(w,xps,DRAW_ON);

    for (gps=AppGridPointSeg1st(w->app,&ix);gps!=NULL;gps=Next(&ix))
      DrawObject(w,gps,DRAW_ON);

    for (sz=AppSurfaceZone1st(w->app,&ix);sz!=NULL;sz=Next(&ix))
      DrawObject(w,sz,DRAW_ON);

    for (sx=AppSurfaceEx1st(w->app,&ix);sx!=NULL;sx=Next(&ix))
      DrawObject(w,sx,DRAW_ON);

    for (gpx=AppGridPointEx1st(w->app,&ix);gpx!=NULL;gpx=Next(&ix))
      DrawObject(w,gpx,DRAW_ON);

    if (w->app->mesh!=NULL) DrawWholeMesh(w,w->app->mesh,DRAW_ON);
    if (w->app->sonnetData!=NULL) DrawObject(w,w->app->sonnetData,DRAW_ON);
    for (e=AppElem1st(w->app,&ix);e!=NULL;e=Next(&ix))
      DrawObject(w,e,DRAW_ON);
    for (sep=AppSeparator1st(w->app,&ix);sep!=NULL;sep=Next(&ix))
      DrawObject(w,sep,DRAW_ON);
    for (src=AppSource1st(w->app,&ix);src!=NULL;src=Next(&ix))
      DrawObject(w,src,DRAW_ON);
    for (n=AppNode1st(w->app,&ix);n!=NULL;n=Next(&ix))
      DrawObject(w,n,DRAW_ON);
    for (vl=Group1st(w->labels,&ix);vl!=NULL;vl=Next(&ix))
      DrawViewLabel(w,vl,DRAW_ON);
  }
  for (ch=AppChord1st(w->app,&ix);ch!=NULL;ch=Next(&ix))
    DrawObject(w,ch,DRAW_ON);

/* Unlock highlight drawing and repaint highlight
*/
  DrawAppHighlight(w->app,DRAWHI_UNLOCK);
  for (p=AppHighlight1st(w->app,&ix);p!=NULL;p=Next(&ix))
    DrawObject(w,p,DRAW_ON);

  DrawHighlightRect(w,DRAW_ON);
  DrawAllViewShapes(w,DRAW_ON);

  w->updateFlag=0;
}

/* Highlight object with specified status and remove all labels from
   the View window
*/

void Highlight(View w,void* obj,int status) {
  ValidatePtr(w,"Highlight");
  ValidatePtr(obj,"Highlight_");
  ValidatePtr(w->app,"Highlight.app");

  RemoveAllViewLabels(w);

  ChangeObjGroup(w->app,w->app,GetOffset(App,highlight),obj,status,1);
}

/* Unhighlight all objects
*/

void UnhighlightAll(View w) {
  void* obj;
  Index ix;

  ValidatePtr(w,"UnhighlightAll");
  ValidatePtr(w->app,"UnhighlightAll_");
  for (obj=AppHighlight1st(w->app,&ix);obj!=NULL;obj=Next(&ix))
    Highlight(w,obj,0);
  ClearHighlightRect(w);
  ClearViewShapes(w);
}

/* Set View message for examining objects
*/

void SetExamineMsg(View w,void* obj) {
  Node n;
  Elem e;
  SurfaceEx sx;
  GridPointEx gpx;
  Separator sep;
  Source src;
  Chord ch;
  MeshPoint mpt;
  MeshCell mc;
  MeshElement me;
  XPointTest xpt;
  XPointSeg xps;
  GridPointSeg gps;
  SurfaceZone sz;
  int i,r;
  double f;
  Index ix;

  switch(GetObjType(obj)) {
    case T_NODE:
      n=obj;
      i=IsIrregularNode(n,NULL);
      ViewMsgEx(w,MSG_EXAMNODE,
        "$(X)%g$(Y)%g$(Z)%g$(EX)%e$(EY)%e$(EZ)%e$(REGULAR)%s",
        n->x,n->y,n->z,n->x,n->y,n->z,
        GetStr(w,i? i : STR_REGULAR));
      break;
    case T_ELEM:
      e=obj;
      f=hypot(e->n[1]->x-e->n[2]->x,e->n[1]->y-e->n[2]->y);
      ViewMsgEx(w,MSG_EXAMELEM,
        "$(X1)%g$(Y1)%g$(X2)%g$(Y2)%g$(EX1)%e$(EY1)%e$(EX2)%e$(EY2)%e"
        "$(LEN)%g$(ELEN)%e$(ID)%d$(VARS1)%d",
        e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
        e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,f,f,e->id,
        GroupCount(e->varsContaining));
      break;
    case T_SURFACEEX:
      sx=obj;
      i=sx->zone; /*GetSurfaceArea(w->app,s),*/
      sz= i!=SZN_XY? FindSurfaceZone(sx->app,i) : NULL;
      ViewMsgEx(w,sx->zone==SZN_XY? MSG_EXAM_SURFACE_XY : MSG_EXAMSURFACE,
        "$(NAREA)%d$(AREA)%s$(LEVEL)%g$(ELEVEL)%e"
        "$(X)%g$(Y)%g$(EX)%e$(EY)%e$(CREATOR)%s",
        i,sz!=NULL? GetSurfaceZoneLongName(sz) : "---",
        sx->level,sx->level,
        sx->originX,sx->originY,sx->originX,sx->originY,
        GetSurfaceExCreatorId(sx));
      w->lastExaminedSurfaceEx=sx;
      break;
    case T_GRIDPOINTEX:
      gpx=obj;
      gps=FindGridPointSeg(w->app,gpx->zone);
      ViewMsgEx(w,MSG_EXAMGRIDPOINT,
        "$(NAREA)%d$(AREA)%s$(VALUE)%g$(EVALUE)%e$(NUMBER)%d$(CREATOR)%s",
        gpx->zone,
        gps!=NULL? GetGridPointSegLongName(gps) : "---",
        gpx->value,gpx->value,GetGridPointExNumber(w->app,gpx),
        GetGridPointExCreatorId(gpx));
      w->lastExaminedGridPointEx=gpx;
      break;
    case T_SEPARATOR:
      sep=obj;
      ViewMsgEx(w,MSG_EXAMSEPARATOR,
        "$(NUMBER)%d$(ID)%d",
        GroupIndex(w->app->separators,sep)+1,
        sep->id);
      break;
    case T_SOURCE:
      src=obj;
      ViewMsgEx(w,MSG_EXAMSOURCE,
        "$(X)%g$(Y)%g$(EX)%e$(EY)%e",
        src->x,src->y,src->x,src->y);
      break;
    case T_CHORD:
      ch=obj;
      ViewMsgEx(w,MSG_EXAMCHORD,
        "$(X1)%g$(Y1)%g$(X2)%g$(Y2)%g$(LEN)%g"
	"$(Z1)%g$(Z2)%g"
        "$(EX1)%e$(EY1)%e$(EX2)%e$(EY2)%e%(LEN)%e"
        "$(NUMBER)%d",
        ch->x1,ch->y1,ch->x2,ch->y2,hypot(ch->x2-ch->x1,ch->y2-ch->y1),
        ch->z1,ch->z2,
        ch->x1,ch->y1,ch->x2,ch->y2,hypot(ch->x2-ch->x1,ch->y2-ch->y1),
        GroupIndex(w->app->chords,ch)+1
      );
      break;
    case T_MESHCELL:
      mc=obj;
      i=IsIrregularMeshCell(mc);
      ViewMsgEx(w,MSG_EXAM_MESH_CELL,
        "$(CX)%g$(CY)%g$(RATIO)%g$(ECX)%g$(ECY)%g$(ERATIO)%g"
        "$(EN)%d$(COLUMN)%d$(ROW)%d$(OLD_RATIO)%g"
        "$(WHY_IRREGULAR)%s",
        mc->centerX,mc->centerY,mc->ratio,mc->centerX,mc->centerY,mc->ratio,
        mc->eN,mc->nx,mc->ny,mc->backupRatio,i? GetStr(w,i) : "");
      break;
    case T_MESHELEMENT:
      me=obj;
      ViewMsgEx(w,MSG_EXAM_MESH_ELEMENT,
        "$(X1)%g$(Y1)%g$(X2)%g$(Y2)%g$(EX1)%e$(EY1)%e$(EX2)%e$(EY2)%e"
        "$(IS_CUT)%s",
        me->points[0]->x,me->points[0]->y,me->points[1]->x,me->points[1]->y,
        me->points[0]->x,me->points[0]->y,me->points[1]->x,me->points[1]->y,
        me->cutFlag? GetStr(w,STR_MESH_ELEMENT_CUT):"");
      break;
    case T_MESHPOINT:
      mpt=obj;
      ViewMsgEx(w,MSG_EXAM_MESH_POINT,
        "$(X)%g$(Y)%g$(EX)%e$(EY)%e$(NX)%d$(NY)%d",
        mpt->x,mpt->y,mpt->x,mpt->y,mpt->nx,mpt->ny);
      break;
    case T_XPOINTTEST:
      xpt=obj;
      assert(w->app->equil!=NULL);
      ViewMsgEx(w,MSG_EXAM_XPOINTTEST,
        "$(X1)%g$(X2)%g$(Y1)%g$(Y2)%g""$(ID)%d$(LEVEL)%g"
        "$(LVLMIN)%g$(LVLMAX)%g""$(CENTERX)%g$(CENTERY)%g",
        EqX(w->app->equil,xpt->cx1),EqX(w->app->equil,xpt->cx1),
        EqX(w->app->equil,xpt->cx2),EqX(w->app->equil,xpt->cy2),
        xpt->id,xpt->level,xpt->lvlMin,xpt->lvlMax,xpt->centerX,
        xpt->centerY);
      break;
    case T_XPOINTSEG:
      xps=obj;
      gps=FindGridPointSegBySegment(w->app,xps);
      ViewMsgEx(w,MSG_EXAM_XPOINTSEG,
        "$(XPTID)%d$(XPSNUMBER)%d$(ZONE)%d$(DIR)%d$(TARGETDIR)%d"
        "$(LEVEL)%g$(LINELENGTH)%g$(NAME)%s$(DESCR)%s",
        xps->xpt->id,xps->number,gps->zone,gps->dir,gps->targetDir,
        gps->level,gps->lineLength,
        GetGridPointSegShortName(gps),GetGridPointSegLongName(gps));
      break;
    default:
      break;
  }
}

/* Set View rectangle to include all shown objects
*/

static void SetPictureViewRect(View w,double x1, double y1, double x2,
    double y2) {
  double zoom=min(w->width/(x2-x1),w->height/(y2-y1));

  SetRotatedViewFactor(w,(x1+x2)/2,(y1+y2)/2,zoom,zoom,0);
}

void ShowPicture(View w) {
  double minX,minY,maxX,maxY;
  Var v;
  Chord ch;
  Index ix;
/*  Index ix;
  Node n;
  Surface s;
  GridPoint gp;
  XY xy;
  int i;*/

  minX=minY=MAXDOUBLE;
  maxX=maxY=-MAXDOUBLE;

  if (w->app==NULL) return;
  if (w->showFlags & SHW_TOPVIEW) {
    if (w->showFlags & SHW_ELEMS) {
      v=GetVarPtrByType(w->app,VT_TOPVIEW);
      if (v!=NULL && v->val!=NULL && !IsEmptyGroup(v->val)) {
	CalcGroupExtents(v->val,&minX,&minY,&maxX,&maxY);
	minX=minY=-maxX;
	maxY=maxX;
      }
    }
    if (w->showFlags & SHW_CHORDS)
      for (ch=AppChord1st(w->app,&ix);ch!=NULL;ch=Next(&ix)) {	
	if (minX>maxX) {minX=maxX=ch->x1;minY=maxY=ch->y1;}
	minX=min(minX,ch->x1);maxX=max(maxX,ch->x1);
	minY=min(minY,ch->z1);maxY=max(maxY,ch->z1);
	minX=min(minX,ch->x2);maxX=max(maxX,ch->x2);
	minY=min(minY,ch->z2);maxY=max(maxY,ch->z2);
      }
  } else {
    if (w->showFlags & (SHW_NODES | SHW_IRRNODES | SHW_ELEMS | SHW_NORMALS
	| SHW_NUMBERS))
      CalcGroupExtents(w->app->nodes,&minX,&minY,&maxX,&maxY);
    if (w->showFlags & SHW_SEPARATORS)
      CalcGroupExtents(w->app->separators,&minX,&minY,&maxX,&maxY);
    if (w->showFlags & SHW_SOURCES)
      CalcGroupExtents(w->app->sources,&minX,&minY,&maxX,&maxY);
    if (w->showFlags & SHW_CHORDS)
      CalcGroupExtents(w->app->chords,&minX,&minY,&maxX,&maxY);
    if (w->showFlags & SHW_SURFACES)
      CalcGroupExtents(w->app->surfacesEx,&minX,&minY,&maxX,&maxY);
    if (w->showFlags & SHW_GRIDPOINTS)
      CalcGroupExtents(w->app->gridPointsEx,&minX,&minY,&maxX,&maxY);
    if (w->app->template!=NULL && w->showFlags & SHW_TEMPLATE)
      CalcObjExtents(w->app->template,&minX,&minY,&maxX,&maxY);
    if (w->app->sonnetData!=NULL && w->showFlags & SHW_MESH)
      CalcObjExtents(w->app->sonnetData,&minX,&minY,&maxX,&maxY);
    if ((w->app->equil!=NULL && w->showFlags & SHW_EQUIL))
      CalcObjExtents(w->app->equil,&minX,&minY,&maxX,&maxY);
    if ((w->app->mesh!=NULL && w->showFlags & SHW_MESH))
      CalcObjExtents(w->app->mesh,&minX,&minY,&maxX,&maxY);
    if (ViewShowTopology(w))
      CalcGroupExtents(w->app->gridPointSegs,&minX,&minY,&maxX,&maxY);
  }


  if (minX>maxX)
    SetPictureViewRect(w,-1,-1,1,1);
  else if (minX==maxX || minY==maxY)
    SetPictureViewRect(w,minX-1,minY-1,minX+1,minY+1);
  else SetPictureViewRect(w,minX-(maxX-minX)*w->shpIncr/100,
    minY-(maxY-minY)*w->shpIncr/100,
    maxX+(maxX-minX)*w->shpIncr/100,
    maxY+(maxY-minY)*w->shpIncr/100);
}

/* Add a label to the View window
*/

void AddViewLabel(View w,double x,double y,char* label,int bShowIt) {
  ViewLabel vl;

  if (bShowIt && (x<=w->minX || x>=w->maxX || y<=w->minY || y>=w->maxY)) {
    SetViewFactor(w,x,y,w->zoomX,w->zoomY);
  }

  vl=Malloc(sizeof(*vl));
  vl->type=T_VIEWLABEL;
  vl->x=x;
  vl->y=y;
  vl->label=MallocString(label);

  GroupAdd(w->labels,vl);
  DrawObject(w,vl,DRAW_ON);
}

/* Remove all labels from the View
*/

void RemoveAllViewLabels(View w) {
  ViewLabel vl;
  Index ix;

  for (vl=Group1st(w->labels,&ix);vl!=NULL;vl=Next(&ix)) {
    DrawObject(w,vl,DRAW_ERASE);
    vl->label=Free(vl->label);
    GroupDel(w->labels,vl);
    vl=Free(vl);
  }
}

/* Place a label on the specified object
*/

void LabelObject(View w,void* obj,char* label,int bShowIt) {
  Elem e;
  Node n;
  Separator sep;
  Source src;
  Chord ch;
  MeshElement me;
  MeshCell mc;
  SurfaceEx sex;
  GridPointEx gpx;

  switch(GetObjType(obj)) {
    case T_NODE:
      n=obj;
      AddViewLabel(w,n->x,n->y,label,bShowIt);
      break;
    case T_SOURCE:
      src=obj;
      AddViewLabel(w,src->x,src->y,label,bShowIt);
      break;
    case T_ELEM:
      e=obj;
      AddViewLabel(w,(e->n[1]->x+e->n[2]->x)/2,(e->n[1]->y+e->n[2]->y)/2,
        label,bShowIt);
      break;
    case T_SEPARATOR:
      sep=obj;
      AddViewLabel(w,(sep->n->x+sep->x)/2,(sep->n->y+sep->y)/2,
        label,bShowIt);
      break;
    case T_CHORD:
      ch=obj;
      AddViewLabel(w,(ch->x1+ch->x2)/2,(ch->y1+ch->y2)/2,
        label,bShowIt);
      break;
    case T_MESHELEMENT:
      me=obj;
      AddViewLabel(w,(me->points[0]->x+me->points[1]->x)/2,
          (me->points[0]->y+me->points[1]->y)/2,label,bShowIt);
      break;
    case T_MESHCELL:
      mc=obj;
      AddViewLabel(w,mc->centerX,mc->centerY,label,bShowIt);
      break;
    case T_SURFACEEX:
      sex=obj;
      if (sex->bCoordsOk) AddViewLabel(w,sex->originX,sex->originY,label,bShowIt);
      break;
    case T_GRIDPOINTEX:
      gpx=obj;
      if (gpx->bCoordsOk) AddViewLabel(w,gpx->x,gpx->y,label,bShowIt);
      break;
    default:
      FatalError("LabelObject()-type%d: fatal error 1",GetObjType(obj));
  }
}

/* Call XAppStrEx() and SetViewMsgEx() in sequence
*/

void ViewMsgEx(View w,int id,char* fmt,...) {
  va_list vl;

  va_start(vl,fmt);
  SetViewMsg(w,GetStrVaEx(w,id,fmt,vl));
  va_end(vl);
}

/* Returns an static string describing an object in a View */

char* GetObjDescription(View w,void* obj) {
    static char buf[DG_FNAME_LEN*2];
    VarSet vs;
    VarDef vd;
    Var v;
    char* s;

    switch(GetObjType(obj)) {
      case T_NODE:return GetStr(w,STR_NODE);
      case T_ELEM:return GetStr(w,STR_ELEM);
      case T_SEPARATOR:return GetStr(w,STR_SEPARATOR);
      case T_SOURCE:return GetStr(w,STR_SOURCE);
      case T_CHORD:return GetStr(w,STR_CHORD);
      case T_EQUIL:return GetStr(w,STR_EQUIL);
      case T_TEMPLATE:return GetStr(w,STR_TEMPLATE);
      case T_SONNET:return GetStr(w,STR_SONNET);
      case T_XPOINTTEST:return GetStr(w,STR_XPOINTTEST);
      case T_XPOINTSEG:return GetStr(w,STR_XPOINTSEG);
      case T_VARSETDEF:return ((VarSetDef)obj)->descr;
      case T_VARSET:
        vs=obj;
        strcpy(buf,vs->def->descr);

        /* Append index if necessary */

        s=GetIndexVarValue(vs);

        if (s!=NULL) {
          sprintf(buf+strlen(buf)," %s",s);
        } else if (GroupCount(vs->def->varSets)>1) {
          sprintf(buf+strlen(buf)," #%d",GroupIndex(vs->def->varSets,vs)+1);
        }

        return buf;
      case T_VARDEF:
        vd=obj;
        sprintf(buf,"%s.%s",vd->varSetDef->descr,vd->descr);
        return buf;
      case T_VAR:
        v=obj;
        obj= v->varSet!=NULL ? v->varSet : v->origin;
        strcpy(buf,GetObjDescription(w,obj));
        strcat(buf,".");
        strcat(buf,v->def->descr);
        return buf;
   }
   return "Internal error 15";
}

/* Returns a static string describing why an object is locked */

char* WhyLocked(View w,void* object) {
  static char buf[DG_FNAME_LEN*2];

  sprintf(buf,"%s %s",GetStr(w,ERR_USEDBY),
    GetObjDescription(w,GetLockingObject(w->app,object)));

  return buf;
}

static char* MakeStrVaEx(char* s,char* fmt,va_list params) {
  static char buf[MAKESTREX_BUF]="",b2[MAKESTREX_BUF]="";
  int i,j,k/*,n*/;
  char* ps,*ps1;

  strncpy(b2,fmt,sizeof(b2)-1);

  for (i=0;b2[i];i++) if (b2[i]=='$') b2[i]=MAKESTREX_CHR;
  vsprintf(buf,b2,params);
  for (i=0;s[i];i++) if (s[i]=='$' && s[i+1]=='(') {
    s[i]=MAKESTREX_CHR;
    for (j=0;s[j];) if (s[i+j++]==')') break;
    for (ps=buf;ps!=NULL;ps=strchr(ps+1,MAKESTREX_CHR))
      if (!strncmp(s+i,ps,j)) break;
    if (ps==NULL) return NULL;
    ps1=strchr(ps+j,MAKESTREX_CHR);
    k= ps1==NULL ? strlen(ps+j) : (int)(ps1-ps-j);
    memmove(s+i+k,s+i+j,strlen(s+i+j)+1);
    memmove(s+i,ps+j,k);
    i+=k-j;
  }
  return s;
}

static char* GetStaticStr(void) {
  static int index=0;
  static char buf[STATICSTR_CNT][STATICSTR_LEN+1];

  index=(index+1)%STATICSTR_CNT;
  return buf[index];
}

char* GetStrVaEx(View w,int id,char* fmt,va_list args) {
  char* s;

  s=GetStaticStr();
  strcpy(s,GetStr(w,id));
  s=MakeStrVaEx(s,fmt==NULL ? "" : fmt,args);
  return s==NULL ? "(BadString)" : s;
}

char* GetStrEx(View w,int id,char* fmt,...) {
  char* s;
  va_list vl;

  va_start(vl,fmt);
  s=GetStrVaEx(w,id,fmt,vl);
  va_end(vl);

  return s;
}

void DrawHighlightRect(View w,int mode) {
  if (w->app!=NULL && w->app->highlightLocks>0) return;

  switch (mode) {
    case DRAW_ON:
      SetViewMode(w,VMX_HIGHLIGHTRECT);
      if (w->bHrShown) return;
      w->bHrShown=1;
      break;
    case DRAW_OFF:
      SetViewMode(w,VMX_HIGHLIGHTRECT);
      if (!w->bHrShown) return;
      w->bHrShown=0;
      break;
    case DRAW_ERASE:
      SetViewMode(w,VM0_HIGHLIGHTRECT);
      w->bHrShown=0;
      break;
    default: assert(0);
  }

  if (w->hrMinX!=w->hrMaxX || w->hrMinY!=w->hrMaxY) {
    DrawViewRect(w,w->hrMinX,w->hrMinY,w->hrMaxX,w->hrMaxY);
  }
}

void SetHighlightRect(View w,double x1,double y1,double x2,double y2) {
  DrawHighlightRect(w,DRAW_OFF);

  w->hrMinX=min(x1,x2);
  w->hrMinY=min(y1,y2);
  w->hrMaxX=max(x1,x2);
  w->hrMaxY=max(y1,y2);

  DrawHighlightRect(w,DRAW_ON);
  AddAppUpdate(w->app);
}

ViewShape AddViewShapeLine(View w,double x1,double y1,double x2,double y2) {
  ViewShape vs=Malloc(sizeof(*vs));
  vs->type=T_VIEWSHAPE;
  vs->shapeType=SHAPE_LINE;
  vs->bDrawn=0;

  vs->x1=x1;
  vs->y1=y1;
  vs->x2=x2;
  vs->y2=y2;

  GroupAdd(w->shapes,vs);

  DrawViewShape(w,vs,DRAW_ON);

  return vs;
}

void* DelViewShape(View w,ViewShape vs) {
  DrawViewShape(w,vs,DRAW_ERASE);

  GroupDel(w->shapes,vs);
  vs=Free(vs);

  if (w->app!=NULL) AddAppUpdate(w->app);

  return NULL;
}

void ClearViewShapes(View w) {
  while (!IsEmptyGroup(w->shapes))
    DelViewShape(w,Group1st(w->shapes,NULL));
}

void DrawViewShape(View w,ViewShape vs,int mode) {
  if (w->app!=NULL && w->app->highlightLocks>0) return;

  switch (mode) {
    case DRAW_ON:
      if (vs->bDrawn) return;
      vs->bDrawn=1;
      SetViewMode(w,VMX_HIGHLIGHTRECT);
      break;
    case DRAW_OFF:
      if (!vs->bDrawn) return;
      vs->bDrawn=0;
      SetViewMode(w,VMX_HIGHLIGHTRECT);
      break;
    case DRAW_ERASE:
      if (!vs->bDrawn) return;
      vs->bDrawn=0;
      SetViewMode(w,VM0_HIGHLIGHTRECT);
      break;
    default: assert(0);
  }

  switch (vs->shapeType) {
    case SHAPE_LINE:
      DrawViewLine(w,vs->x1,vs->y1,vs->x2,vs->y2);
      break;
    default:
      assert(0);
  }
}

void DrawAllViewShapes(View w,int mode) {
  ViewShape vs;
  Index ix;

  for (vs=Group1st(w->shapes,&ix);vs!=NULL;vs=Next(&ix))
    DrawViewShape(w,vs,mode);
}

void AddViewArrow(View w,double x1,double y1,double x2,double y2) {
  double h;

  if (x1==x2 && y1==y2) return;

  AddViewShapeLine(w,x1,y1,x2,y2);

  x1=x2-x1;
  y1=y2-y1;

  h=hypot(x1,y1);
  x1*=-w->arrowLength/h/w->zoomX;
  y1*=-w->arrowLength/h/w->zoomY;
  h=(double)w->arrowWidth/w->arrowLength;

  AddViewShapeLine(w,x2+x1+y1*h,y2+y1+x1*h,x2,y2);
  AddViewShapeLine(w,x2+x1-y1*h,y2+y1+x1*h,x2,y2);
}

void SetViewEditTopology(View w,int bEnable) {
  if (bEnable==w->bEditTopology) return;

  w->bEditTopology=bEnable;
  ClearView(w);
  RepaintView(w);
}

void DrawPolyLine(View w,Group g,double startPos,double endPos) {
  XY xy,xy1;
  Index ix;
  double l=0,h,x1,y1,x2,y2;

  if (endPos<0) endPos=MAXDOUBLE;

  /* Get the first segment */

  xy1=Group1st(g,&ix);
  if (xy1==NULL) return;

  /* Follow the line */
  for (;xy=Next(&ix),xy!=NULL;l+=h,xy1=xy) {
    h=hypot(xy->x-xy1->x,xy->y-xy1->y);

    /* Skip segments not displayed at all */
    if (l>=endPos || l+h<=startPos) continue;

    /* Draw a complete or partial segment */
    if (l<startPos) {
      x1=xy1->x+(xy->x-xy1->x)*(startPos-l)/h;
      y1=xy1->y+(xy->y-xy1->y)*(startPos-l)/h;
    } else {
      x1=xy1->x;
      y1=xy1->y;
    }

    if (l+h>endPos) {
      x2=xy1->x+(xy->x-xy1->x)*(endPos-l)/h;
      y2=xy1->y+(xy->y-xy1->y)*(endPos-l)/h;
    } else {
      x2=xy->x;
      y2=xy->y;
    }
    DrawViewLine(w,x1,y1,x2,y2);
  }
}

/* increment angle by sign*w->xyAngle about the center of the screen */
void ScreenRotate(View w,int sign,double* px,double* py) {
  if (w->xyAngle==0) return;
  Rotate(sign*w->xyAngle,w->centerX,w->centerY,px,py);
}

