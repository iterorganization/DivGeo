#include "x_dg.h"

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  Create Object dialog boxes                                  // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

/* Dialog box names ///////////////////////////////////////////////// */

#define DLG_CREATE_NODE         "dlgCreateNode"
#define DLG_CREATE_SOURCE       "dlgCreateSource"
#define DLG_CREATE_SURFACE      "dlgCreateSurface"
#define DLG_CREATE_GPOINT       "dlgCreateGridPoint"
#define DLG_CREATE_CHORD        "dlgCreateChord"
#define DLG_CREATE_SURFACE_ADV  "dlgCreateSurfaceAdv"
#define DLG_CREATE_GPOINT_ADV   "dlgCreateGridPointAdv"
#define WG_DISTRGRAPH           "distributeGraph"

/* Routines public to the whole source file ///////////////////////// */

static Widget OpenCreateNodeDlg(View w);
static Widget OpenCreateSourceDlg(View w);
static Widget OpenCreateChordDlg(View w);
static Widget OpenCreateSurfaceDlg(View w);
static Widget OpenCreateGridPointDlg(View w);
static Widget OpenSurfaceAdvDlg(View w);
static Widget OpenGridPointAdvDlg(View w);

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* // Public callback function handling all menu commands          // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

void CbCmCreateAt(Widget wg,XtPointer arg,XtPointer pcbs) {
  View w=(View)arg;
  XtPointer userData;

  if (w->app==NULL) return;
  SetActiveView(w);

  GetValues(wg,XmNuserData,&userData,NULL);

  switch((int)userData) {
    case T_NODE:
      OpenCreateNodeDlg(w);
      break;
    case T_SOURCE:
      OpenCreateSourceDlg(w);
      break;
    case T_CHORD:
      OpenCreateChordDlg(w);
      break;
    case T_SURFACEEX:
      OpenCreateSurfaceDlg(w);
      break;
    case T_GRIDPOINTEX:
      OpenCreateGridPointDlg(w);
      break;
    default:
      assert(0);
  }
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  CreateNodeDlg                                               // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _CreateNodeDlg {
  View w;
  Widget wX,wY;
}* CreateNodeDlg;

static void CbCreateNode(Widget wg,CreateNodeDlg dlg,void* pcbs);

static Widget OpenCreateNodeDlg(View w) {
  XtPointer xtp;
  CreateNodeDlg dlg;
  Widget wDlg,wg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_CREATE_NODE);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wDlg=CreateOkCancelDialog(w->x->wMain,DLG_CREATE_NODE);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbCreateNode,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    wg=Cmw(XmCreateForm,wDlg,"form",
      NULL);
    CreateMenuSystem(wg,
      "l@:xLabel",0x0101,
      "x?@:x",&dlg->wX,0x0201,
      "l@:yLabel",0x0102,
      "x?@:y",&dlg->wY,0x0202,
      NULL);
    Form2Table(wg);
    XtManageChild(wDlg);
  }
  else XtPopup(XtParent(wDlg),XtGrabNone);

  SetViewFlags(w,w->showFlags | SHW_IRRNODES);
  UndoMark(w->app);

  return wDlg;
}

static void CbCreateNode(Widget wg,CreateNodeDlg dlg,void* pcbs) {
  double x,y;
  char* s,* s1;
  Node n;
  Index ix;

  SetActiveView(dlg->w);
  if (dlg->w->app==NULL) return;

  s=XmTextGetString(dlg->wX);
  s1=XmTextGetString(dlg->wY);
  if (sscanf(s,SCANFLT,&x)!=1 || sscanf(s1,SCANFLT,&y)!=1) {
    XtFree(s);
    XtFree(s1);
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }
  XtFree(s);
  XtFree(s1);

  for (n=AppNode1st(dlg->w->app,&ix);n!=NULL;n=Next(&ix))
      if (n->x==x && n->y==y) {
    LabelObject(dlg->w,n,GetStr(dlg->w,STR_ERRLABEL),True);
    UndoMark(dlg->w->app);
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_ALREADYEXISTS));
    return;
  }

  n=AddNode(dlg->w->app,x,y);
  LabelObject(dlg->w,n,GetStr(dlg->w,STR_NEWLABEL),True);
  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_NODES | SHW_IRRNODES);
  UndoMark(dlg->w->app);
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  CreateSourceDlg                                             // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef CreateNodeDlg CreateSourceDlg;

static void CbCreateSource(Widget wg,CreateSourceDlg dlg,void* pcbs);

static Widget OpenCreateSourceDlg(View w) {
  XtPointer xtp;
  CreateSourceDlg dlg;
  Widget wDlg,wg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_CREATE_SOURCE);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wDlg=CreateOkCancelDialog(w->x->wMain,DLG_CREATE_SOURCE);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbCreateSource,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
    wg=Cmw(XmCreateForm,wDlg,"form",
      NULL);
    CreateMenuSystem(wg,
      "l@:xLabel",0x0101,
      "x?@:x",&dlg->wX,0x0201,
      "l@:yLabel",0x0102,
      "x?@:y",&dlg->wY,0x0202,
      NULL);
    Form2Table(wg);
    XtManageChild(wDlg);
  }
  else XtPopup(XtParent(wDlg),XtGrabNone);

  SetViewFlags(w,w->showFlags | SHW_SOURCES);
  UndoMark(w->app);

  return wDlg;
}

static void CbCreateSource(Widget wg,CreateSourceDlg dlg,void* pcbs) {
  double x,y;
  char* s,* s1;
  Source src;
  Index ix;

  SetActiveView(dlg->w);
  if (dlg->w->app==NULL) return;

  s=XmTextGetString(dlg->wX);
  s1=XmTextGetString(dlg->wY);
  if (sscanf(s,SCANFLT,&x)!=1 || sscanf(s1,SCANFLT,&y)!=1) {
    XtFree(s);
    XtFree(s1);
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }
  XtFree(s);
  XtFree(s1);

  for (src=AppSource1st(dlg->w->app,&ix);src!=NULL;src=Next(&ix))
      if (src->x==x && src->y==y) {
    LabelObject(dlg->w,src,GetStr(dlg->w,STR_ERRLABEL),True);
    UndoMark(dlg->w->app);
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_ALREADYEXISTS));
    return;
  }

  src=AddSource(dlg->w->app,x,y);
  LabelObject(dlg->w,src,GetStr(dlg->w,STR_NEWLABEL),True);
  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_SOURCES);
  UndoMark(dlg->w->app);
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  CreateChordDlg                                              // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _CreateChordDlg {
  View w;
  Widget wX1,wYp1,wYt1,wX2,wYp2,wYt2,wR1,wR2,wZ1,wZ2,wPh1,wPh2;
  Widget wSwCartes;
}* CreateChordDlg;

static void CbCreateChord(Widget wg,CreateChordDlg dlg,void* pcbs);
static void CbCToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs);

static Widget OpenCreateChordDlg(View w) {
  XtPointer xtp;
  CreateChordDlg dlg;
  Widget wDlg,wg;
  Widget wSwCylind,wFrameCartes,wFrameCylind;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_CREATE_CHORD);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wDlg=CreateOkCancelDialog(w->x->wMain,DLG_CREATE_CHORD);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbCreateChord,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
    wg=Cmw(XmCreateForm,wDlg,"form",
      NULL);
    CreateMenuSystem(wg,
      "#:mainForm",
       "$+8:dialogKind",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL,
        "t?:cartesian",&dlg->wSwCartes,
        "t?:cylindrical",&wSwCylind,
       "-:",

       "f5?_:tabFrame",&wFrameCartes,
        "#:distributeForm",
         "#2:table",
          "l#:x1Label",1,1,
          "x#?:x1",2,1,&dlg->wX1,
          "l#:yp1Label",1,2,
          "x#?:yp1",2,2,&dlg->wYp1,
          "l#:yt1Label",1,3,
          "x#?:yt1",2,3,&dlg->wYt1,
          "l#:x2Label",1,4,
          "x#?:x2",2,4,&dlg->wX2,
          "l#:yp2Label",1,5,
          "x#?:yp2",2,5,&dlg->wYp2,
          "l#:yt2Label",1,6,
          "x#?:yt2",2,6,&dlg->wYt2,
         "-#:",
        "-:",
       "-:",

       "f5?_:tabFrame",&wFrameCylind,
        "#:distributeForm",
         "#2:table",
          "l#:r1Label",1,1,
          "x#?:r1",2,1,&dlg->wR1,
          "l#:z1Label",1,2,
          "x#?:z1",2,2,&dlg->wZ1,
          "l#:ph1Label",1,3,
          "x#?:ph1",2,3,&dlg->wPh1,
          "l#:r2Label",1,4,
          "x#?:r2",2,4,&dlg->wR2,
          "l#:z2Label",1,5,
          "x#?:z2",2,5,&dlg->wZ2,
          "l#:ph2Label",1,6,
          "x#?:ph2",2,6,&dlg->wPh2,
         "-#:",
        "-:",
       "-:",
      NULL);

    XtAddCallback(dlg->wSwCartes,XmNvalueChangedCallback,
      CbCToggleManaged,(XtPointer)wFrameCartes);
    XtAddCallback(wSwCylind,XmNvalueChangedCallback,
      CbCToggleManaged,(XtPointer)wFrameCylind);

    XmToggleButtonSetState(dlg->wSwCartes,True,True);
    XtManageChild(wDlg);
  }
  else XtPopup(XtParent(wDlg),XtGrabNone);


  SetViewFlags(w,w->showFlags | SHW_CHORDS);
  UndoMark(w->app);

  return wDlg;
}

static void CbCreateChord(Widget wg,CreateChordDlg dlg,void* pcbs) {
  double x1,y1,x2,y2,z1,z2,ph1,ph2;
  char* s,* s1;
  Chord ch;
  Index ix;

  SetActiveView(dlg->w);
  if (dlg->w->app==NULL) return;

  if (XmToggleButtonGetState(dlg->wSwCartes)) {
    s=XmTextGetString(dlg->wX1);
    s1=XmTextGetString(dlg->wYp1);
    if (sscanf(s,SCANFLT,&x1)!=1 || sscanf(s1,SCANFLT,&y1)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);

    s=XmTextGetString(dlg->wX2);
    s1=XmTextGetString(dlg->wYp2);
    if (sscanf(s,SCANFLT,&x2)!=1 || sscanf(s1,SCANFLT,&y2)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);

    s=XmTextGetString(dlg->wYt1);
    s1=XmTextGetString(dlg->wYt2);
    if (sscanf(s,SCANFLT,&z1)!=1 || sscanf(s1,SCANFLT,&z2)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);
  }
  else {
    s=XmTextGetString(dlg->wR1);
    s1=XmTextGetString(dlg->wZ1);
    if (sscanf(s,SCANFLT,&x1)!=1 || sscanf(s1,SCANFLT,&y1)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);

    s=XmTextGetString(dlg->wR2);
    s1=XmTextGetString(dlg->wZ2);
    if (sscanf(s,SCANFLT,&x2)!=1 || sscanf(s1,SCANFLT,&y2)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);

    s=XmTextGetString(dlg->wPh1);
    s1=XmTextGetString(dlg->wPh2);
    if (sscanf(s,SCANFLT,&ph1)!=1 || sscanf(s1,SCANFLT,&ph2)!=1) goto scan_err;
    XtFree(s);
    XtFree(s1);

    z1=x1*sin(M_PI/180*ph1);x1=x1*cos(M_PI/180*ph1);
    z2=x2*sin(M_PI/180*ph2);x2=x2*cos(M_PI/180*ph2);
  }

  for (ch=AppChord1st(dlg->w->app,&ix);ch!=NULL;ch=Next(&ix))
      if (ch->x1==x1 && ch->y1==y1 && ch->x2==x2 && ch->y2==y2
	  && ch->z1==z1 && ch->z2==z2) {
    LabelObject(dlg->w,ch,GetStr(dlg->w,STR_ERRLABEL),True);
    UndoMark(dlg->w->app);
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_ALREADYEXISTS));
    return;
  }
  ch=AddChord3D(dlg->w->app,x1,y1,z1,x2,y2,z2);
  if (ch==NULL) return;
  LabelObject(dlg->w,ch,GetStr(dlg->w,STR_NEWLABEL),True);
  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_CHORDS);
  UndoMark(dlg->w->app);
  return;

 scan_err:
  XtFree(s);
  XtFree(s1);
  ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
  return;
}

static void CbCToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs) {
  Widget wManage=(Widget)xtpWmanage;

  if (XmToggleButtonGetState(wg))
    XtManageChild(wManage);
  else XtUnmanageChild(wManage);
}

/* ////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  Graph subsystem for creating multiple surfaces/grid points  // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _DistrGraph {
  int count,law;
  double alpha[2],oldDragAlpha,gx[2],gy[2];

  View w;

  int bGraphDrawn,mouseInside,bDragging,nDragAlpha,flags,prevFlags;
  int width,height,margin,gridLen,extraLeftMargin;
  Pixel pxBg,pxFg,pxGrid,pxRect;
  GC gc;
  XtIntervalId toUpdate;
  Widget wDlg,wDraw,wCount,wAlpha[2],wAlphaLabel[2],wLaw,wLawLabel;
}* DistrGraph;

#define DGD_GRAPH 0x0001
#define DGD_FRAME 0x0002
#define DGD_LINES 0x0004
#define DGD_DISTR 0x0008

#define GI2X(w,x) ((double)(x)/(w))
#define GI2Y(h,y) (1-(double)(y)/(h))
#define GX2I(w,x) ((int)((w)*(x)))
#define GY2I(h,y) ((int)((h)*(1-(y))))

static void CbDG_NumbersChanged(Widget wg,XtPointer xtpG,XtPointer pcbs);
static void CbDG_Expose(Widget wg,XtPointer xtpG,XtPointer pcbs);
static void CbDG_Resize(Widget wg,XtPointer xtpG,XtPointer pcbs);
static double DG_CalcCoeff(DistrGraph dg,double x,double y,double* pAlpha);

static XtResource dgRes[]={
  XmNtranslations,XmCTranslations,XmRTranslationTable,sizeof(XtTranslations),
    0,XmRString,"",
};

static void DG_Init(DistrGraph dg,View w,Widget wDlg,
    Widget wDraw,Widget wCount,Widget wAlpha,Widget wAlphaLabel,
    Widget wAlpha2,Widget wAlpha2Label,Widget wLaw,Widget wLawLabel){
  XtTranslations translations;
  Widget wg;

  dg->w=w;
  dg->wDlg=wDlg;
  dg->wDraw=wDraw;
  dg->wCount=wCount;
  dg->wAlpha[0]=wAlpha;
  dg->wAlphaLabel[0]=wAlphaLabel;
  dg->wAlpha[1]=wAlpha2;
  dg->wAlphaLabel[1]=wAlpha2Label;
  dg->wLaw=wLaw;
  dg->wLawLabel=wLawLabel;

  dg->toUpdate=(int) NULL;
  dg->bGraphDrawn=True;

  dg->bDragging=False;
  dg->flags=DGD_GRAPH | DGD_FRAME | DGD_LINES | DGD_DISTR;
  dg->prevFlags=dg->flags;

  dg->alpha[0]=dg->alpha[1]=-1;
  dg->count=-1;
  dg->law=-1;
  dg->gx[0]=dg->gy[0]=dg->gx[1]=dg->gy[1]=-1;

  dg->mouseInside=0;

  SetValues(dg->wDraw,XmNuserData,(XtPointer)dg,NULL);

  XtAddCallback(dg->wDraw,XmNdestroyCallback,CbRemovePTimeOut,
    (XtPointer)&dg->toUpdate);

  if (dg->wCount!=NULL)
    XtAddCallback(dg->wCount,XmNvalueChangedCallback,CbDG_NumbersChanged,
        (XtPointer)dg);
  if (dg->wAlpha[0]!=NULL)
    XtAddCallback(dg->wAlpha[0],XmNvalueChangedCallback,CbDG_NumbersChanged,
        (XtPointer)dg);
  if (dg->wAlpha[1]!=NULL)
    XtAddCallback(dg->wAlpha[1],XmNvalueChangedCallback,CbDG_NumbersChanged,
        (XtPointer)dg);
  if (dg->wLaw!=NULL) {
    GetValues(dg->wLaw,XmNsubMenuId,&wg,NULL);
    AddCallbackToTree(wg,xmPushButtonWidgetClass,XmNactivateCallback,
        CbDG_NumbersChanged,(XtPointer)dg);
  }

  XtAddCallback(dg->wDraw,XmNexposeCallback,CbDG_Expose,
      (XtPointer)dg);
  XtAddCallback(dg->wDraw,XmNresizeCallback,CbDG_Resize,
      (XtPointer)dg);

  assert(XtIsRealized(dg->wDraw));

  dg->gc=XCreateGC(XtDisplay(dg->wDraw),XtWindow(dg->wDraw),0L,NULL);
  XtAddCallback(dg->wDraw,XmNdestroyCallback,CbFreeGC,(XtPointer)dg->gc);

  GetValues(dg->wDraw,
    XmNbackground,&dg->pxBg,
    XmNforeground,&dg->pxFg,
    NULL);

  dg->pxRect=GetResourcePixel(dg->wDraw,"rectColor",XmCForeground,"Black");
  dg->pxGrid=GetResourcePixel(dg->wDraw,"gridColor",XmCForeground,"Black");
  dg->margin=GetResourceInt(dg->wDraw,XmNmargin,XmCMargin,5);
  dg->extraLeftMargin=GetResourceInt(dg->wDraw,"extraMargin",XmCMargin,10);
  dg->gridLen=GetResourceInt(dg->wDraw,"gridLength","GridLength",5);

  /* Manually install translations in order to overcome a Motif bug */

  XtGetApplicationResources(dg->wDraw,&translations,
    dgRes,XtNumber(dgRes),NULL,0);
  XtOverrideTranslations(dg->wDraw,translations);

  CbDG_Resize(dg->wDraw,(XtPointer)dg,NULL);
  CbDG_NumbersChanged(NULL,(XtPointer)dg,NULL);
}

#define SetForeground(c) XSetForeground(XtDisplay(dg->wDraw),dg->gc,c)
#define DrawLine(x1,y1,x2,y2) \
    XDrawLine(XtDisplay(dg->wDraw),XtWindow(dg->wDraw),dg->gc,x1,y1,x2,y2)
#define DrawRect(x1,y1,x2,y2) XDrawRectangle(\
    XtDisplay(dg->wDraw),XtWindow(dg->wDraw),dg->gc,x1,y1,x2,y2)

static void DG_DrawRect(DistrGraph dg) {
  if (~dg->flags & DGD_FRAME) return;

  SetForeground(dg->pxRect);
  DrawRect(dg->margin,dg->margin,
      dg->width-2*dg->margin,dg->height-2*dg->margin);
  DrawLine(dg->margin+dg->extraLeftMargin,dg->margin,
      dg->margin+dg->extraLeftMargin,dg->height-dg->margin);
}

static double DG_Law(DistrGraph dg,double x) {
  return DistributeLaw(x,dg->law,dg->alpha[0],dg->alpha[1],dg->count);
}

static void DG_DrawCurve(DistrGraph dg,int bDrawOrErase,double* pAlpha,
    double newAlpha) {

  int i,x,y,ox,oy=0,oy1=0,gw,gh,xStep=1;
  double oldAlpha;

  xStep=GetResourceInt(dg->wDraw,"graphStep",NULL,1);

  gw=dg->width-2*dg->margin-dg->extraLeftMargin;
  gh=dg->height-2*dg->margin;

  if (dg->flags & DGD_GRAPH && dg->alpha[0]>0) {
    ox=-1;
    for (i=0;i<gw+xStep;i+=xStep) {
      i=min(i,gw);

      x=dg->margin+dg->extraLeftMargin+i;
      y=dg->margin+GY2I(gh,DG_Law(dg,GI2X(gw,i)));

      SetForeground(bDrawOrErase? dg->pxFg : dg->pxBg);
      if (ox>=0) DrawLine(ox,oy,x,y);
      oy=y;

      if (pAlpha!=NULL) {
        oldAlpha=*pAlpha;
        *pAlpha=newAlpha;
        y=dg->margin+GY2I(gh,DG_Law(dg,GI2X(gw,i)));
        *pAlpha=oldAlpha;

        SetForeground(!bDrawOrErase? dg->pxFg : dg->pxBg);
        if (ox>=0) DrawLine(ox,oy1,x,y);
        oy1=y;
      }

      ox=x;
    }
  }
}

static void DG_DrawGraph(DistrGraph dg,int bDrawOrErase) {
  int i,x,y=0,gw,gh;
  double t;
  Pixel fg;

  if (!dg->bGraphDrawn && !bDrawOrErase && dg->prevFlags==dg->flags) return;
  dg->bGraphDrawn=bDrawOrErase;
  dg->prevFlags=dg->flags;

  if (bDrawOrErase) DG_DrawRect(dg);

  gw=dg->width-2*dg->margin-dg->extraLeftMargin;
  gh=dg->height-2*dg->margin;

  /* Draw vertical and horizontal lines */

  SetForeground(bDrawOrErase? dg->pxGrid : dg->pxBg);
  for (i=0;i<dg->count;i++) {
    t=1.0/(dg->count+1)*(i+1);
    x=dg->margin+dg->extraLeftMargin+GX2I(gw,t);
    if (dg->alpha[0]>0) {
      if (dg->flags & DGD_LINES) {
        y=dg->margin+GY2I(gh,DG_Law(dg,t));
        DrawLine(x,dg->height-dg->margin-1,x,y);
        if (dg->law==DGLAW_FLIPPED) DrawLine(dg->width-dg->margin,y,x,y);
        else DrawLine(dg->margin+dg->extraLeftMargin,y,x,y);
      }
      if (dg->flags & DGD_DISTR)
        DrawLine(dg->margin,y,dg->extraLeftMargin+dg->margin,y);
    } else if (dg->flags & DGD_LINES)
      DrawLine(x,dg->margin+1,x,dg->height-dg->margin);
  }

  DG_DrawCurve(dg,bDrawOrErase,NULL,0);

  if (!bDrawOrErase) DG_DrawRect(dg);
}

#undef SetForeground
#undef DrawLine

static int DG_GetData(DistrGraph dg,int bShowErrors) {
  int i;

  dg->count=dg->alpha[0]=dg->alpha[0]=-1;

  dg->law=(int)GetOptionMenuValue(dg->wLaw);

  dg->count=GetXmTextInt(dg->wCount);
  if (dg->count==MAXINT) dg->count=-1;

  dg->alpha[0]=GetXmTextDouble(dg->wAlpha[0]);
  if (dg->alpha[0]==MAXDOUBLE) dg->alpha[0]=-1;

  dg->alpha[1]=GetXmTextDouble(dg->wAlpha[1]);
  if (dg->alpha[1]==MAXDOUBLE) dg->alpha[1]=-1;

  i=GetResourceInt(dg->wCount,"max",NULL,0);
  if (dg->count<1 || dg->count>i) {
    dg->count=-1;
    if (bShowErrors) ErrorBox(dg->wDlg,GetResourceStringEx(dg->wCount,
        "errBadRange",NULL,"$(MIN)%d$(MAX)%d",1,i));
    return -1;
  } else dg->count--;

  if (dg->alpha[0]<=0) {
    dg->alpha[0]=-1;
    if (bShowErrors) ErrorBox(dg->wDlg,GetResourceString(dg->wAlpha[0],
      "errBadRange",NULL,NULL));
    return -1;
  }

  if ((dg->law==DGLAW_2ALPHA || dg->law==DGLAW_DELTA) && dg->alpha[0]<=0) {
    dg->alpha[0]=-1;
    if (bShowErrors) ErrorBox(dg->wDlg,GetResourceString(dg->wAlpha[1],
      "errBadRange",NULL,NULL));
    return -1;
  }

  if (DG_Law(dg,-1)<0) {
    if (bShowErrors) ErrorBox(dg->wDlg,GetResourceString(dg->wLaw,
      "errBadLaw",NULL,NULL));
    return -1;
  }

  return 0;
}


static void ToDG_UpdateGraph(XtPointer xtpG,XtIntervalId* timer) {
  DistrGraph dg=(DistrGraph)xtpG;

  dg->toUpdate=(int) NULL;
  DG_DrawGraph(dg,True);
}

/* Updates the DistrGraph structure when changes are made in the dialog */

#define DG_NC_CURVE_FIT_ITERATIONS 10
#define DG_NC_RESET_DELTA 1e-5

static void CbDG_NumbersChanged(Widget wg,XtPointer xtpG,XtPointer pcbs) {
  DistrGraph dg=(DistrGraph)xtpG;
  XtAppContext apc=XtWidgetToApplicationContext(dg->wDraw);
  int oldLaw,oldCount,b,newCount,i;
  double t;
  char s[256];

  /* No action if values change as the graph is being dragged. Also
     used to prevent unwanted updates in other cases */

  if (dg->bDragging) return;

  /* Preserve some old values */

  oldLaw=dg->law;
  oldCount=dg->count;

  /* Erase the graph and update the DistrGraph structure */

  DG_DrawGraph(dg,False);
  DG_GetData(dg,False);

  /* Remove the update timeout, if any */

  if (dg->toUpdate!=(int) NULL) {
    XtRemoveTimeOut(dg->toUpdate);
    dg->toUpdate=(int) NULL;
  }

  /* Add a new update timeout */

  dg->toUpdate=XtAppAddTimeOut(apc,
      GetResourceInt(dg->wDraw,"updateDelay",NULL,1000),
      ToDG_UpdateGraph,(XtPointer)dg);

  /* Make the 2nd alpha/delta widget sensitive according to the law */

  b=dg->law==DGLAW_2ALPHA || dg->law==DGLAW_DELTA;
  SetSensitiveEx(dg->wAlpha[1],b);
  SetSensitiveEx(dg->wAlphaLabel[1],b);

  /* Update labels on alpha/delta widgets if law changes */

  if (oldLaw!=dg->law) {
    SetLabelString(dg->wAlphaLabel[0],GetResourceString(dg->wAlphaLabel[0],
        dg->law==DGLAW_DELTA? "labelDelta" : "labelAlpha",NULL,NULL));
    SetLabelString(dg->wAlphaLabel[1],GetResourceString(dg->wAlphaLabel[1],
        dg->law==DGLAW_DELTA? "labelDelta" : "labelAlpha",NULL,NULL));
  }

  /* Recalculate deltas if law is Delta and # of points changed */

  if (dg->law==DGLAW_DELTA && oldCount!=dg->count && wg==dg->wCount) {

    /* Get 2 points of the "old" graph */

    if (dg->gx[0]<0) {
      newCount=dg->count;
      dg->count=oldCount;
      dg->gy[0]=DG_Law(dg,dg->gx[0]=1.0/3);
      dg->gy[1]=DG_Law(dg,dg->gx[1]=2.0/3);
      dg->count=newCount;
    }

    /* Fit the "new" graph as closely as possible to match the old points */

    dg->alpha[0]=dg->alpha[1]=DG_NC_RESET_DELTA;
    for (b=3,i=0;i<DG_NC_CURVE_FIT_ITERATIONS;i++) {
      t=DG_CalcCoeff(dg,dg->gx[0],dg->gy[0],&dg->alpha[0]);
      if (t!=MAXDOUBLE) {dg->alpha[0]=t;b&=~1;} else b|=1;
      t=DG_CalcCoeff(dg,dg->gx[1],dg->gy[1],&dg->alpha[1]);
      if (t!=MAXDOUBLE) {dg->alpha[1]=t;b&=~2;} else b|=2;
    }
    if (!b && dg->count>1) dg->gx[0]=dg->gx[1]=-1;

    /* Update text boxes for delta's */

    dg->bDragging++;  /* Prevent this callback from recurrent entries */
    sprintf(s,"%g",dg->alpha[0]);
    XmTextSetString(dg->wAlpha[0],s); /* as we update text boxes */
    sprintf(s,"%g",dg->alpha[1]);
    XmTextSetString(dg->wAlpha[1],s);
    dg->bDragging--;
  } else  dg->gx[0]=dg->gx[1]=-1;
}

static void CbDG_Expose(Widget wg,XtPointer xtpG,XtPointer pcbs) {
  DistrGraph dg=(DistrGraph)xtpG;

  if (!XtIsRealized(dg->wDraw)) return;

  DG_DrawGraph(dg,True);
}

static void CbDG_Resize(Widget wg,XtPointer xtpG,XtPointer pcbs) {
  DistrGraph dg=(DistrGraph)xtpG;
  Dimension dWidth,dHeight;

  GetValues(dg->wDraw,
    XmNwidth,&dWidth,
    XmNheight,&dHeight,
    NULL);

  dg->width=dWidth;
  dg->height=dHeight;

  if (XtIsRealized(dg->wDraw))
    XClearArea(XtDisplay(dg->wDraw),XtWindow(dg->wDraw),0,0,0,0,True);
}

static void CbDG_Reset(Widget wg,XtPointer xtpG,XtPointer pcbs) {
  DistrGraph dg=(DistrGraph)xtpG;
  char s[256];

  switch (dg->law) {
    case DGLAW_NORMAL:
    case DGLAW_FLIPPED:
    case DGLAW_2ALPHA:
    case DGLAW_SYMMETRIC:
      XmTextSetString(dg->wAlpha[0],"1");
      XmTextSetString(dg->wAlpha[1],"1");
      break;
    case DGLAW_DELTA:
      sprintf(s,"%g",1.0/max(1,dg->count+1));
      XmTextSetString(dg->wAlpha[0],s);
      XmTextSetString(dg->wAlpha[1],s);
      break;
    default:
      assert(0);
  }
}

#define DGI_PRESS   1
#define DGI_DRAG    2
#define DGI_RELEASE 4
#define DGI_CANCEL  8

#define DG_CC_LOWER 1e-10
#define DG_CC_UPPER 1e10
#define DG_CC_UPPERDELTA 0.5
#define DG_MAX_ITERATIONS 1000       /* Emergency brake */

/* Returns MAXDOUBLE if impossible */
static double DG_CalcCoeff(DistrGraph dg,double x,double y,
    double* pAlpha) {
  double oldAlpha,lower,upper,v1,v2,v3;
  int i;

  oldAlpha=*pAlpha;

  lower=DG_CC_LOWER;
  upper=(dg->law==DGLAW_DELTA)? DG_CC_UPPERDELTA : DG_CC_UPPER;

  *pAlpha=lower;
  v1=DG_Law(dg,x);
  *pAlpha=upper;
  v2=DG_Law(dg,x);

  *pAlpha=oldAlpha;
  if ((v1-y)*(v2-y)>=0) return MAXDOUBLE;

  for (i=0;i<DG_MAX_ITERATIONS;i++) {
    *pAlpha=(upper+lower)/2;
    if (*pAlpha==upper || *pAlpha==lower) break;
    v3=DG_Law(dg,x);

    if ((v3-y)*(v1-y)==0) break;
    if ((v3-y)*(v2-y)==0) break;
    else if ((v3-y)*(v2-y)<0) {lower=*pAlpha;v1=v3;}
    else {upper=*pAlpha;v2=v3;}
  }
  upper=lower=*pAlpha;
  *pAlpha=oldAlpha;

  return lower;
}

static void DG_VisualInput(DistrGraph dg,int evt,int ix,int iy) {
  double x,y,nv;
  int gw,gh;
  char s[256];

  switch (evt) {
    case DGI_PRESS:
      if (dg->bDragging) return;
      break;
    case DGI_DRAG:
    case DGI_RELEASE:
    case DGI_CANCEL:
      if (!dg->bDragging) return;
      break;
    default: assert(0);
  }

  gw=dg->width-2*dg->margin-dg->extraLeftMargin;
  gh=dg->height-2*dg->margin;

  x=GI2X(gw,ix-dg->margin-dg->extraLeftMargin);
  y=GI2Y(gh,iy-dg->margin);

  if (evt==DGI_PRESS) {
    dg->bDragging=True;
    dg->nDragAlpha=
        ((dg->law==DGLAW_2ALPHA || dg->law==DGLAW_DELTA) && x>=0.5) ? 1:0;
    dg->oldDragAlpha=dg->alpha[dg->nDragAlpha];

    DG_DrawGraph(dg,False);
    dg->flags &= ~(DGD_LINES | DGD_DISTR);
    DG_DrawGraph(dg,True);
  }

  switch (evt) {
    case DGI_PRESS:
    case DGI_DRAG:
    case DGI_RELEASE:
      nv=MAXDOUBLE;
      if (x>=0 && x<=1 && y>=0 && y<=1)
        nv=DG_CalcCoeff(dg,x,y,&dg->alpha[dg->nDragAlpha]);
      if (nv==MAXDOUBLE) nv=dg->alpha[dg->nDragAlpha];
      break;
    case DGI_CANCEL:
      nv=dg->oldDragAlpha;
      break;
    default:
      assert(0);
  }

  if (nv!=dg->alpha[dg->nDragAlpha]) {
    DG_DrawCurve(dg,False,&dg->alpha[dg->nDragAlpha],nv);
    dg->alpha[dg->nDragAlpha]=nv;
    XFlush(XtDisplay(dg->wDraw));

    sprintf(s,"%g",nv);
    XmTextSetString(dg->wAlpha[dg->nDragAlpha],s);
  }

  switch (evt) {
    case DGI_RELEASE:
    case DGI_CANCEL:
      dg->bDragging=False;
      dg->flags |= (DGD_LINES | DGD_DISTR);
      CbDG_NumbersChanged(NULL,(XtPointer)dg,NULL);
      DG_DrawGraph(dg,True);
      break;
    default:
      break;
  }
}

void XtActDistrGraph(Widget wg,XEvent* xev,String* args,Cardinal* argn) {
  DistrGraph dg;
  int evt=0,ix=0,iy=0;

  if (*argn!=1) {
    XtWarning("DistrGraph() action called with wrong # of arguments");
    return;
  }

  if (strcmp(XtName(wg),WG_DISTRGRAPH)) {
    XtWarning("DistrGraph() action called for a wrong widget");
    return;
  }

  dg=(DistrGraph)GetUserData(wg);
  assert(dg!=NULL);

  switch(xev->type) {
    case ButtonPress:
        dg->mouseInside=True;
    case ButtonRelease:
        ix=xev->xbutton.x;
        iy=xev->xbutton.y;
      break;
    case MotionNotify:
        if (!dg->mouseInside) return;
        ix=xev->xmotion.x;
        iy=xev->xmotion.y;
      break;
    case EnterNotify:
      dg->mouseInside=1;
      break;
    case LeaveNotify:
      dg->mouseInside=0;
      break;
  }

  if (!strcmp(*args,"Press"))        evt=DGI_PRESS; else
  if (!strcmp(*args,"Drag"))         evt=DGI_DRAG; else
  if (!strcmp(*args,"Release"))      evt=DGI_RELEASE; else
  if (!strcmp(*args,"Enter"))        return; else
  if (!strcmp(*args,"Leave"))        return; else
  if (!strcmp(*args,"Cancel"))       evt=DGI_CANCEL; else
  if (!strcmp(*args,"RelCancel")) {
    evt=dg->mouseInside ? DGI_RELEASE:DGI_CANCEL;
  } else {
    XtWarning("DistrGraph() called with wrong arguments");
    return;
  }

  if (evt) DG_VisualInput(dg,evt,ix,iy);
}


/* ////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  CreateSurfaceDlg - single surface                           // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

#define CSD_BYPOINT 1
#define CSD_BYLEVEL 2
#define TX_CREATESURFACEDLG 16924138

typedef struct _CreateSurfaceDlg {
  int type;
  View w;

  char oldCreatorId[512];
  int oldOutputMode;

  Widget wDlg,wSwByPoint,wSwByLevel,wSwSingle, /* Single surface set */
    wX,wY,wArea,wLevel;

  struct _DistrGraph dg;                       /* Multi surface set */
  Widget wMArea,wMLevel1,wMLevel2,wMLevel1Label,wMLevel2Label,
    wMLevel1Pick,wMLevel2Pick,wMBoundingElem,
    wMCount,wMAlpha,wMAlpha2,wMDraw,wMLaw,wSwRemoveOld;
}* CreateSurfaceDlg;

static void CbCreateSurfaceOk(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCSD_CopyLevel(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCSD_PickSettings(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCSD_SelectArea(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCSD_MarkBoundingElem(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs);
static void DwCSD(Widget wg,View w,int evt,void*obj,void*udt);

static Widget OpenCreateSurfaceDlg(View w) {
  XtPointer xtp;
  CreateSurfaceDlg dlg;
  Widget wg,wDlg,wFrameLevel,wFrameXY,wFrameSingle,wFrameMulti,
    wSwMulti,wMAlphaLabel,wMAlpha2Label,wLawLabel;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_CREATE_SURFACE);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->type=TX_CREATESURFACEDLG;
    dlg->w=w;
    strcpy(dlg->oldCreatorId,"");
    dlg->oldOutputMode=-1;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_CREATE_SURFACE);
    SetUserData(wDlg,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbCreateSurfaceOk,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
    CreateMenuSystem(wDlg,
      "#:mainForm",

       "$+8:dialogKind",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL,
        "t?:single",&dlg->wSwSingle,
        "t?:multiple",&wSwMulti,
       "-:",

       /* *** Create Single *** */

       "f5?_:tabFrame",&wFrameSingle,

        "#:singleForm",
         "$+8:byWhat",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL,
          "t?:byLevel",&dlg->wSwByLevel,
          "t?:byPoint",&dlg->wSwByPoint,
         "-:",
         "f5_?:tabFrame",&wFrameXY,
          "#:formXY",
           "l#:xLabel",1,1,
           "x?#:x",&dlg->wX,2,1,
           "l#:yLabel",1,2,
           "x?#:y",&dlg->wY,2,2,
          "-#:",
         "-:",
         "f5?:tabFrame",&wFrameLevel,
          "#:formLevel",
           "l#:areaLabel",1,1,
           "*:areaMenu",
            "b@:area1",(XtPointer)1,
            "b@:area2",(XtPointer)2,
            "b@:area3",(XtPointer)3,
           "-:",
           "o?#:area",&dlg->wArea,2,1,
           "l#:levelLabel",1,2,
           "x?#:level",&dlg->wLevel,2,2,
          "-#:",
         "-:",
        "-:",
       "-:",

       /* *** Create Multiple *** */

       "f5?:tabFrame",&wFrameMulti,
        "#:distributeForm",
         "#2:table",
          "l#:areaLabel",1,1,
          "*:areaMenu",
           "b@A:area1",(XtPointer)1,CbCSD_SelectArea,(XtPointer)dlg,
           "b@A:area2",(XtPointer)2,CbCSD_SelectArea,(XtPointer)dlg,
           "b@A:area3",(XtPointer)3,CbCSD_SelectArea,(XtPointer)dlg,
          "-:",
          "o#?:area",2,1,&dlg->wMArea,
          "l#?:level1Label",1,2,&dlg->wMLevel1Label,
          "x#?:level1",2,2,&dlg->wMLevel1,
          "b#A@?:copy1",3,2,CbCSD_CopyLevel,(XtPointer)dlg,1,
              &dlg->wMLevel1Pick,
          "l#?:level2Label",1,3,&dlg->wMLevel2Label,
          "x#?:level2",2,3,&dlg->wMLevel2,
          "b#A@?:copy2",3,3,CbCSD_CopyLevel,(XtPointer)dlg,2,
              &dlg->wMLevel2Pick,
          "l#:countLabel",1,4,
          "x#?:count",2,4,&dlg->wMCount,
          "l#?:alphaLabel",1,5,&wMAlphaLabel,
          "x#?:alpha",2,5,&dlg->wMAlpha,
          "l#?:alpha2Label",1,6,&wMAlpha2Label,
          "x#?:alpha2",2,6,&dlg->wMAlpha2,
          "l#?:lawLabel",1,7,&wLawLabel,
          "*:lawMenu",
           "b@:lawN",(XtPointer)DGLAW_NORMAL,
           "b@:lawR",(XtPointer)DGLAW_FLIPPED,
           "b@:lawS",(XtPointer)DGLAW_SYMMETRIC,
           "b@:law2",(XtPointer)DGLAW_2ALPHA,
           "b@:lawD",(XtPointer)DGLAW_DELTA,
          "-:",
          "o#?:law",2,7,&dlg->wMLaw,
          "b#?:boundingElem",2,8,&dlg->wMBoundingElem,
          "b#A:reset",1,9,CbDG_Reset,(XtPointer)&dlg->dg,
          "t#?:removeOld",2,9,&dlg->wSwRemoveOld,
          "b#A:copy3",4,0,CbCSD_PickSettings,(XtPointer)dlg,
         "-#:",
         "f5:frame",
          "$?:"WG_DISTRGRAPH,XmCreateDrawingArea,NULL,
            &dlg->wMDraw,
          "-:",
         "-:",
        "-:",
       "-:",
      /*"-:",*/
      NULL);

    XmToggleButtonSetState(dlg->wSwByLevel,True,True);
    XmToggleButtonSetState(wSwMulti,True,True);
    SetSensitiveEx(dlg->wSwRemoveOld,False);

    XtAddCallback(dlg->wSwSingle,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameSingle);
    XtAddCallback(wSwMulti,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameMulti);

    XtAddCallback(dlg->wSwByLevel,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameLevel);
    XtAddCallback(dlg->wSwByPoint,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameXY);

    XtAddCallback(dlg->wMBoundingElem,XmNactivateCallback,
	CbCSD_MarkBoundingElem,(XtPointer)dlg);

    /*XtAddCallback(dlg->wMLevel1,XmNvalueChangedCallback,
        CbCSD_LevelChanged,(XtPointer)dlg);
    XtAddCallback(dlg->wMLevel2,XmNvalueChangedCallback,
        CbCSD_LevelChanged,(XtPointer)dlg);*/

    wg=XtNameToWidget(wDlg,"OK");
    AddDependentWidget(w,wg,N_NOW | N_NEWAPP | N_ALT,NULL,
      DwNotifyIfExists,(void*)T_EQUIL);

    XtManageChild(wDlg);

    DG_Init(&dlg->dg,w,dlg->wDlg,
        dlg->wMDraw,dlg->wMCount,dlg->wMAlpha,wMAlphaLabel,
        dlg->wMAlpha2,wMAlpha2Label,dlg->wMLaw,wLawLabel);

    AddDependentWidget(w,dlg->wDlg,N_NOW | N_NEWAPP | N_ALT,NULL,
      DwCSD,(void*)dlg);
  }
  else {
    dlg=(CreateSurfaceDlg)GetUserData(wDlg);
    DwCSD(NULL,w,N_NOW,NULL,dlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }

  SetViewFlags(w,w->showFlags | SHW_SURFACES);
  UndoMark(w->app);

  return wDlg;
}

static void CSD_CreateSingle(CreateSurfaceDlg dlg) {
  int i,area;
  double x,y,level;
  char* s,* s1;
  SurfaceEx surfex;
  Index ix;

  SetActiveView(dlg->w);
  if (dlg->w->app==NULL) return;
  if (dlg->w->app->equil==NULL) return;

  /* Prevent manual editing in Carre mode */

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_CARRE_INHIBITS));
    return;
  }

  /* Differentiate between "by leven" and "by x-y" */

  if (XmToggleButtonGetState(dlg->wSwByLevel)) {

    /* By level - parse area, level */

    area=(int)GetOptionMenuValue(dlg->wArea);
    level=GetXmTextDouble(dlg->wLevel);

    if (level==MAXDOUBLE) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
      return;
    }

    /* Check for duplicate surfaces */

    for (surfex=AppSurfaceEx1st(dlg->w->app,&ix);surfex!=NULL;surfex=Next(&ix))
        if (surfex->zone==area && surfex->level==level) {
      LabelObject(dlg->w,surfex,GetStr(dlg->w,STR_ERRLABEL),True);
      UndoMark(dlg->w->app);
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_ALREADYEXISTS));
      return;
    }

    /* Try adding the surface */

    surfex=AddSurfaceEx(dlg->w->app,area,level,&i);
    if (surfex==NULL) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,i));
      return;
    }
  } else {

    /* Add surface by x-y */

    x=GetXmTextDouble(dlg->wX);
    y=GetXmTextDouble(dlg->wY);

    if (x==MAXDOUBLE || y==MAXDOUBLE) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
      return;
    }

    /* Check for duplicate surfaces */

    for (surfex=AppSurfaceEx1st(dlg->w->app,&ix);surfex!=NULL;surfex=Next(&ix))
        if (surfex->bDrawn && surfex->originX==x && surfex->originY==y) {
      LabelObject(dlg->w,surfex,GetStr(dlg->w,STR_ERRLABEL),True);
      UndoMark(dlg->w->app);
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_ALREADYEXISTS));
      return;
    }

    /* Try adding the surface */

    surfex=AddSurfaceExByXY(dlg->w->app,x,y,&i);
    if (surfex==NULL) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,i));
      return;
    }
  }

  LabelObject(dlg->w,surfex,GetStr(dlg->w,STR_NEWLABEL),True);

  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_SURFACES);
  UndoMark(dlg->w->app);
}

static void CSD_CreateMultiple(CreateSurfaceDlg dlg) {
  int area=(int)GetOptionMenuValue(dlg->wMArea);
  double level1,level2,t,x,y,v;
  int i,r,cnt,carreMode;
  SurfaceEx sex;
  Index ix;
  String creatorId;

  /* Get distribution data, make sure an X point exists */

  if (DG_GetData(&dlg->dg,True)) return;

/*  if (dlg->w->app->xpoint==NULL) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_NOXPOINT));
    return;
  } */

  carreMode=dlg->w->app->outputMode==OUTPUTMODE_CARRE;

  /* Determine the level range */

  if (dlg->w->app->outputMode!=OUTPUTMODE_CARRE) {

    /* Sonnet mode: read levels from text boxes */

    level1=GetXmTextDouble(dlg->wMLevel1);
    level2=GetXmTextDouble(dlg->wMLevel2);
    if (level1==MAXDOUBLE || level2==MAXDOUBLE) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
      return;
    }
  } else {

    /* Carre mode: use the opportunity to check law */

    if (dlg->dg.law!=DGLAW_DELTA) {
      ErrorBox(dlg->wDlg,
          GetResourceString(dlg->wMLaw,"errWrongCarreLaw",NULL,NULL));
      return;
    }

    /* And determine the levels depending from the ares */

    level1=0;
    level2=1;
  }

  if (level1==level2) {
    ErrorBox(dlg->wDlg,
        GetResourceString(dlg->wDlg,"errEqualLevels",NULL,NULL));
    return;
  }

  /* if (level2<level1) swap(level1,level2); -- Why did I do this?? */

  /* Remove old surfaces */

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE)
    XmToggleButtonSetState(dlg->wSwRemoveOld,True,False);

  if (XmToggleButtonGetState(dlg->wSwRemoveOld)) {
    if (dlg->w->app->outputMode!=OUTPUTMODE_CARRE) {

      /* Sonnet mode: delete surfaces with matching creatorId */

      for (i=0,sex=AppSurfaceEx1st(dlg->w->app,&ix);sex!=NULL;sex=Next(&ix)) {
        if (!strcmp(StripPrefix(GetSurfaceExCreatorId(sex),"+"),
            StripPrefix(dlg->oldCreatorId,"+")))
          {DelSurfaceEx(sex);i++;}
      }
      if (!i) {
        Cancel(dlg->w->app);
        ErrorBox(dlg->wDlg,
            GetResourceString(dlg->wDlg,"errOldSurfacesNotFound",NULL,NULL));
        return;
      }
    } else {

      /* Carre mode: delete all surfaces in the area */
      /* -- Now done in DistributeSurfaces, because of level calculation
      for (s=AppSurface1st(dlg->w->app,&ix);s!=NULL;s=Next(&ix))
        if (GetSurfaceArea(dlg->w->app,s)==area) DelSurface(dlg->w->app,s);
*/

    }
  } else if (dlg->w->app->outputMode==OUTPUTMODE_CARRE) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,
        GetResourceString(dlg->wDlg,"errCarreOldSurfaces",NULL,NULL));
    return;
  }

  /* Create surfaces */

  r=DistributeSurfaces(dlg->w->app,area,dlg->dg.count,
    dlg->dg.alpha[0],dlg->dg.alpha[1],dlg->dg.law,
    level1,level2,carreMode,&creatorId);

  if (r) {
    Cancel(dlg->w->app);
    MessageBox(dlg->wDlg,GetStr(dlg->w,r));
    return;
  }

  strcpy(dlg->oldCreatorId,creatorId);
  if (!carreMode) {
    SetSensitiveEx(dlg->wSwRemoveOld,!!dlg->dg.count);
    XmToggleButtonSetState(dlg->wSwRemoveOld,!!dlg->dg.count,True);
  }

  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_SURFACES);
  UndoMark(dlg->w->app);
}

static void CbCreateSurfaceOk(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)xtpD;

  if (XmToggleButtonGetState(dlg->wSwSingle)) CSD_CreateSingle(dlg);
  else CSD_CreateMultiple(dlg);
}

static void CbCSD_CopyLevel(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)xtpD;
  char s[256];
  int area;
  SurfaceEx surfex;

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE) return;

  surfex=dlg->w->lastExaminedSurfaceEx;
  if (surfex==NULL || !InGroup(dlg->w->app->surfacesEx,surfex)) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,
        "errNoRememberedSurface",NULL,NULL));
    return;
  }

  area=surfex->zone;
  if (area<0) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_NOSURFZONE));
    return;
  }

  SetOptionMenuValue(dlg->wMArea,
      (XtPointer)area);
  sprintf(s,"%g",surfex->level);
  switch ((int)GetUserData(wg)) {
    case 1:
      XmTextSetString(dlg->wMLevel1,s);
      break;
    case 2:
      XmTextSetString(dlg->wMLevel2,s);
      break;
    default:
      assert(0);
  }
  XmToggleButtonSetState(dlg->wSwRemoveOld,False,True);
}

static void CbCSD_PickSettings(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)xtpD;
  int area,count,law,carreFlag;
  double l1,l2,alpha,alpha2;
  char s[256];
  SurfaceEx surfex;

  surfex=dlg->w->lastExaminedSurfaceEx;
  if (surfex==NULL || !InGroup(dlg->w->app->surfacesEx,surfex)) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,
        "errNoRememberedSurface",NULL,NULL));
    return;
  }

  if (ParseSurfaceCreatorId(GetSurfaceExCreatorId(surfex),
      &area,&count,&alpha,&alpha2,&law,&l1,&l2,&carreFlag)) {
    ErrorBox(dlg->wDlg,
      GetResourceString(dlg->wDlg,"errBadCreatorId",NULL,NULL));
    return;
  }

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE && !carreFlag) {
    ErrorBox(dlg->wDlg,
      GetResourceString(dlg->wDlg,"errNonCarreMode",NULL,NULL));
    return;
  }

  SetOptionMenuValue(dlg->wMArea,(XtPointer)area);
  if (!carreFlag) {
    sprintf(s,"%g",l1);XmTextSetString(dlg->wMLevel1,s);
    sprintf(s,"%g",l2);XmTextSetString(dlg->wMLevel2,s);
  }
  sprintf(s,"%d",count+1);XmTextSetString(dlg->dg.wCount,s);
  sprintf(s,"%g",alpha);XmTextSetString(dlg->dg.wAlpha[0],s);
  sprintf(s,"%g",alpha2);XmTextSetString(dlg->dg.wAlpha[1],s);
  SetOptionMenuValue(dlg->dg.wLaw,(XtPointer)law);
  strcpy(dlg->oldCreatorId,GetSurfaceExCreatorId(surfex));

  CbDG_NumbersChanged(NULL,(XtPointer)&dlg->dg,NULL);
  if (dlg->w->app->outputMode!=OUTPUTMODE_CARRE) {
    SetSensitiveEx(dlg->wSwRemoveOld,True);
    XmToggleButtonSetState(dlg->wSwRemoveOld,True,True);
  }
}

static void CbCSD_SelectArea(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)xtpD;

  if (dlg->w->app->outputMode!=OUTPUTMODE_CARRE) {
    SetSensitiveEx(dlg->wSwRemoveOld,False);
    XmToggleButtonSetState(dlg->wSwRemoveOld,False,True);
  }
}

static void CbCSD_MarkBoundingElem(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)xtpD;
  int area=(int)GetOptionMenuValue(dlg->wMArea);
  SurfaceZone sz;

  if (dlg->w->app==NULL) return;
  sz = FindSurfaceZone(dlg->w->app,area);

  MarkGroup(dlg->w->app,dlg->w->app->mark,0);
  if (sz->innermost!=NULL) MarkObject(dlg->w->app,sz->innermost,1);
  else SetViewMsg(dlg->w,GetStr(dlg->w,MSG_NOBOUNDINGELEMS));
  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_ELEMS);
  UndoMark(dlg->w->app);
}

static void DwCSD(Widget wg,View w,int evt,void*obj,void*udt) {
  CreateSurfaceDlg dlg=(CreateSurfaceDlg)udt;
  int b;
  XmString* xmsa;
  int i,xmsca,xmsc;
  XtPointer* values;
  SurfaceZone sz;
  Index ix;

  assert(dlg->type==TX_CREATESURFACEDLG);

  if (evt==N_ALT && !IsMapped(XtParent(dlg->wDlg))) return;

  /* Update the list of surfaces on topo change */
  if (evt==N_NOW || evt==N_NEWAPP ||
      (evt==N_ALT && w->x->changes & CHF_TOPOLOGY)) {
    xmsca=GroupCount(w->app->surfaceZones);
    xmsa=Malloc(sizeof(*xmsa)*xmsca+1);
    values=Malloc(sizeof(*values)*xmsca+1);
    for (xmsc=0,sz=AppSurfaceZone1st(w->app,&ix);sz!=NULL;sz=Next(&ix)) {
      if (!SurfaceZoneIsUsed(sz)) continue;
      xmsa[xmsc]=MakeXmString(GetSurfaceZoneDescription(sz));
      values[xmsc]=(XtPointer)sz->zone;
      xmsc++;
    }
    SetOptionMenuItems(dlg->wArea,xmsc,xmsa,values);
    SetOptionMenuItems(dlg->wMArea,xmsc,xmsa,values);
    for (i=0;i<xmsc;i++) XmStringFree(xmsa[i]);
    Free(xmsa);
    Free(values);
  }

  /* Check for an output mode change */

  if (dlg->oldOutputMode!=dlg->w->app->outputMode) {
    dlg->oldOutputMode=dlg->w->app->outputMode;

    b=dlg->w->app->outputMode==OUTPUTMODE_CARRE;

    SetSensitiveEx(dlg->wMLevel1,!b);
    SetSensitiveEx(dlg->wMLevel1Label,!b);
    SetSensitiveEx(dlg->wMLevel1Pick,!b);
    SetSensitiveEx(dlg->wMLevel2,!b);
    SetSensitiveEx(dlg->wMLevel2Label,!b);
    SetSensitiveEx(dlg->wMLevel2Pick,!b);
    SetSensitiveEx(dlg->wSwRemoveOld,!b);
    SetSensitiveEx(dlg->wMBoundingElem,b);
    XmToggleButtonSetState(dlg->wSwRemoveOld,b,True);

    if (b) {
      XmTextSetString(dlg->wMLevel1,"");
      XmTextSetString(dlg->wMLevel2,"");
      SetOptionMenuValue(dlg->wMLaw,(XtPointer)DGLAW_DELTA);
      SetSensitiveEx(dlg->wSwRemoveOld,False);
      CbDG_NumbersChanged(NULL,(XtPointer)&dlg->dg,NULL);
    }
  }
}

static void CbToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs) {
  Widget wManage=(Widget)xtpWmanage;

  if (XmToggleButtonGetState(wg))
    XtManageChild(wManage);
  else XtUnmanageChild(wManage);
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  CreateGridPointDlg - single grid point                      // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

#define TX_CGRIDPOINTDLG 98790873

typedef struct _CreateGridPointDlg {
  int type;
  View w;

  int oldOutputMode;

  Widget wDlg,wArea,wValue,wSwSingle;

  struct _DistrGraph dg;                       /* Multi surface set */
  Widget wMZone,wMCount,wMAlpha,wMAlpha2,wMDraw,wMLaw;
}* CreateGridPointDlg;

static void CbCreateGridPointOk(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCGPD_PickSettings(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void DwCGPD(Widget wg,View w,int evt,void*obj,void*udt);

static Widget OpenCreateGridPointDlg(View w) {
  XtPointer xtp;
  CreateGridPointDlg dlg;
  Widget wDlg,wg,wg1,wSwMultiple,wFrameSingle,wFrameMultiple,wMAlpha2Label,
    wMAlphaLabel,wLawLabel;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_CREATE_GPOINT);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->type=TX_CGRIDPOINTDLG;
    dlg->w=w;
    dlg->oldOutputMode=-1;

    dlg->wDlg=wDlg=CreateOkCancelDialog(w->x->wMain,DLG_CREATE_GPOINT);
    SetUserData(dlg->wDlg,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,
        (XtCallbackProc)CbCreateGridPointOk,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    CreateWidgetSystem(wDlg,
      "#:mainForm",
       "$+8:dialogKind",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL,
        "t?:single",&dlg->wSwSingle,
        "t?:multiple",&wSwMultiple,
       "-:",

       /* *** Create Single *** */

       "f5?_:tabFrame",&wFrameSingle,
        "#:singleForm",
         "l#:areaLabel",1,1,
         "*:areaMenu",
          "b@:area0",0,
          "b@?:area1",1,&wg1,
          "b@:area2",2,
         "-:",
         "o#?:area",2,1,&dlg->wArea,
         "l#:valueLabel",1,2,
         "x?#:value",&dlg->wValue,2,2,
        "-#:",
       "-:",

       /* *** Create Multiple *** */

       "f5?:tabFrame",&wFrameMultiple,
        "#:distributeForm",
         "#2:table",
         "l#:zoneLabel",1,1,
         "*:zoneMenu",
          "b@:area0",(XtPointer)0,
          "b@:area1",(XtPointer)1,
          "b@:area2",(XtPointer)2,
         "-:",
         "o#?:zone",2,1,&dlg->wMZone,
         "l#:countLabel",1,2,
         "x#?:count",2,2,&dlg->wMCount,
         "l#?:alphaLabel",1,3,&wMAlphaLabel,
         "x#?:alpha",2,3,&dlg->wMAlpha,
         "l#?:alpha2Label",1,4,&wMAlpha2Label,
         "x#?:alpha2",2,4,&dlg->wMAlpha2,
         "l#?:lawLabel",1,5,&wLawLabel,
         "*:lawMenu",
          "b@:lawN",(XtPointer)DGLAW_NORMAL,
          "b@:lawR",(XtPointer)DGLAW_FLIPPED,
          "b@:lawS",(XtPointer)DGLAW_SYMMETRIC,
          "b@:law2",(XtPointer)DGLAW_2ALPHA,
          "b@:lawD",(XtPointer)DGLAW_DELTA,
         "-:",
         "o#?:law",2,5,&dlg->wMLaw,
         "b#A:reset",1,6,CbDG_Reset,(XtPointer)&dlg->dg,
         "b#A:copy3",4,0,CbCGPD_PickSettings,(XtPointer)dlg,
        "-#:",
        "f5:frame",
         "$?:"WG_DISTRGRAPH,XmCreateDrawingArea,NULL,&dlg->wMDraw,
        "-:",
       "-:",
      "-:",
      NULL);

    XmToggleButtonSetState(wSwMultiple,True,True);
    XtAddCallback(dlg->wSwSingle,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameSingle);
    XtAddCallback(wSwMultiple,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameMultiple);

    wg=XtNameToWidget(wDlg,"OK");
    XtManageChild(wDlg);

    DG_Init(&dlg->dg,w,dlg->wDlg,
        dlg->wMDraw,dlg->wMCount,dlg->wMAlpha,wMAlphaLabel,
        dlg->wMAlpha2,wMAlpha2Label,dlg->wMLaw,wLawLabel);

    AddDependentWidget(w,dlg->wDlg,N_NOW | N_NEWAPP | N_ALT,NULL,
      DwCGPD,(void*)dlg);
  }
  else {
    dlg=(CreateGridPointDlg)GetUserData(wDlg);
    DwCGPD(NULL,w,N_NOW,NULL,dlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }

  SetViewFlags(w,w->showFlags | SHW_GRIDPOINTS);
  UndoMark(w->app);

  return wDlg;
}

/*
static String CGPD_MakeCreatorId(CreateGridPointDlg dlg,int zone) {
  static char s[1024];

  sprintf(s,"z%dn%dh%gH%gw%d",
    zone,dlg->dg.count,dlg->dg.alpha[0],dlg->dg.alpha[1],dlg->dg.law);
  return s;
}
*/

static void CGPD_CreateSingle(CreateGridPointDlg dlg) {
  GridPointEx gpx;
  int area;
  double value;
  Index ix;

  SetActiveView(dlg->w);
  if (dlg->w->app==NULL) return;
/*  if (dlg->w->app->xpoint==NULL) return; */

  /* Prevent manual editing in Carre mode */

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_CARRE_INHIBITS));
    return;
  }

  /* Parse area and value */

  area=(int)GetOptionMenuValue(dlg->wArea);

  value=GetXmTextDouble(dlg->wValue);
  if (value==MAXDOUBLE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

  /* Check for bad values */

  if (value<0 || value>1) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVAREA));
    return;
  }

  for (gpx=AppGridPointEx1st(dlg->w->app,&ix);gpx!=NULL;gpx=Next(&ix))
      if (gpx->zone==area && gpx->value==value) {
    LabelObject(dlg->w,gpx,GetStr(dlg->w,STR_ERRLABEL),True);
    UndoMark(dlg->w->app);
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_ALREADYEXISTS));
    return;
  }
  gpx=AddGridPointEx(dlg->w->app,area,value);
  if (gpx==NULL) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INTERNALGP));
    return;
  }
  LabelObject(dlg->w,gpx,GetStr(dlg->w,STR_NEWLABEL),True);

  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_GRIDPOINTS);
  UndoMark(dlg->w->app);
}

static void CGPD_CreateMultiple(CreateGridPointDlg dlg) {
  int zone=(int)GetOptionMenuValue(dlg->wMZone);
  int i,carreMode,r;
  double v;
  GridPointEx gpx;
  Index ix;
  String creatorId;

  if (DG_GetData(&dlg->dg,True)) return;

  carreMode=dlg->w->app->outputMode==OUTPUTMODE_CARRE;

  if (carreMode) {
    if (dlg->dg.law!=DGLAW_DELTA) {
      ErrorBox(dlg->wDlg,
          GetResourceString(dlg->wMLaw,"errWrongCarreLaw",NULL,NULL));
      return;
    }
  }

  for (gpx=AppGridPointEx1st(dlg->w->app,&ix);gpx!=NULL;gpx=Next(&ix))
    if (gpx->zone==zone) DelGridPointEx(gpx);

  r=DistributeGridPoints(dlg->w->app,zone,dlg->dg.count,dlg->dg.alpha[0],
      dlg->dg.alpha[1],dlg->dg.law,carreMode);

  if (r) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,GetStr(dlg->w,r));
    return;
  }

  SetViewFlags(dlg->w,dlg->w->showFlags | SHW_GRIDPOINTS);
  UndoMark(dlg->w->app);
}

static void CbCreateGridPointOk(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateGridPointDlg dlg=(CreateGridPointDlg)xtpD;

  if (XmToggleButtonGetState(dlg->wSwSingle)) CGPD_CreateSingle(dlg);
  else CGPD_CreateMultiple(dlg);
}

static void CbCGPD_PickSettings(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  CreateGridPointDlg dlg=(CreateGridPointDlg)xtpD;
  int area,count,law,carreFlag;
  double alpha,alpha2;
  char s[256];
  GridPointEx gpx;

  gpx=dlg->w->lastExaminedGridPointEx;
  if (gpx==NULL || !InGroup(dlg->w->app->gridPointsEx,gpx)) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,
        "errNoRememberedGridPoint",NULL,NULL));
    return;
  }

  if (ParseGridPointCreatorId(GetGridPointExCreatorId(gpx),
      &area,&count,&alpha,&alpha2,&law,&carreFlag)) {
    ErrorBox(dlg->wDlg,
      GetResourceString(dlg->wDlg,"errBadCreatorId",NULL,NULL));
    return;
  }

  if (dlg->w->app->outputMode==OUTPUTMODE_CARRE && !carreFlag) {
    ErrorBox(dlg->wDlg,
      GetResourceString(dlg->wDlg,"errNonCarreMode",NULL,NULL));
    return;
  }

  SetOptionMenuValue(dlg->wMZone,(XtPointer)area);
  sprintf(s,"%d",count+1);XmTextSetString(dlg->dg.wCount,s);
  sprintf(s,"%g",alpha);XmTextSetString(dlg->dg.wAlpha[0],s);
  sprintf(s,"%g",alpha2);XmTextSetString(dlg->dg.wAlpha[1],s);
  SetOptionMenuValue(dlg->dg.wLaw,(XtPointer)law);

  CbDG_NumbersChanged(NULL,(XtPointer)&dlg->dg,NULL);
}

static void DwCGPD(Widget wg,View w,int evt,void*obj,void*udt) {
  CreateGridPointDlg dlg=(CreateGridPointDlg)udt;
  GridPointSeg gps;
  XmString* xmsa;
  XtPointer* values;
  int xmsc,xmsca,i;
  Index ix;
  int b;

  assert(dlg->type==TX_CGRIDPOINTDLG);

  /* Do nothing if unmapped */
  if (evt==N_ALT && !IsMapped(XtParent(dlg->wDlg))) return;

  /* Update the list of separatrix segments on topo change */
  if (evt==N_NOW || evt==N_NEWAPP ||
      (evt==N_ALT && w->x->changes & CHF_TOPOLOGY)) {
    xmsca=GroupCount(w->app->gridPointSegs);
    xmsa=Malloc(sizeof(*xmsa)*xmsca+1);
    values=Malloc(sizeof(*values)*xmsca+1);
    for (xmsc=0,gps=AppGridPointSeg1st(w->app,&ix);gps!=NULL;gps=Next(&ix)) {
      if (!GridPointSegIsUsed(gps)) continue;
      xmsa[xmsc]=MakeXmString(GetGridPointSegDescription(gps));
      values[xmsc]=(XtPointer)gps->zone;
      xmsc++;
    }
    SetOptionMenuItems(dlg->wArea,xmsc,xmsa,values);
    SetOptionMenuItems(dlg->wMZone,xmsc,xmsa,values);
    for (i=0;i<xmsc;i++) XmStringFree(xmsa[i]);
    Free(xmsa);
    Free(values);
  }

  /* Make adjustments if the output mode has changed */
  if (dlg->oldOutputMode!=dlg->w->app->outputMode) {
    dlg->oldOutputMode=dlg->w->app->outputMode;

    b=dlg->w->app->outputMode==OUTPUTMODE_CARRE;

    if (b) {
      SetOptionMenuValue(dlg->wMLaw,(XtPointer)DGLAW_DELTA);
      CbDG_NumbersChanged(NULL,(XtPointer)&dlg->dg,NULL);
    }
  }
}


