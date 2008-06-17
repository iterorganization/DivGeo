#include "x_dg.h"

#define DLG_ROTMOVE "dlgRotMove"
#define DLG_GLUEPOINTS "dlgGluePoints"
#define DLG_GLUEELEMS "dlgGlueElems"
#define DLG_STRETCH "dlgStretch"
#define DLG_SETANGLE "dlgSetAngle"

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  RotMoveDlg                                                  // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

#define XTBGS XmToggleButtonGetState
#define XTBSS XmToggleButtonSetState

typedef struct _RotMoveDlg {
  View w;
  Widget wDlg,wMoveX,wMoveY,wScale,wSwTemplate,wSwElems;
}* RotMoveDlg;

static void CbRotMoveDlgRotate(Widget wg,RotMoveDlg dlg,void* pcbs) {
  Node n;
  Index ix;
  double x,y,angle,th;
  int bN,bT;

  SetActiveView(dlg->w);

  bN=XTBGS(dlg->wSwElems);
  bT=XTBGS(dlg->wSwTemplate);
  if (!bT && !bN) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_NOTHINGTODO));
    return;
  }
  LockAppUpdate(dlg->w->app,1);
  if (bT) {
    angle=dlg->w->app->template->angle+M_PI/2;
    if (angle>=M_PI*2) angle-=M_PI*2;
    ChangeTemplate(dlg->w->app,dlg->w->app->template->xIncr,
      dlg->w->app->template->yIncr,angle,dlg->w->app->template->scale);
    SetViewFlags(dlg->w,dlg->w->showFlags | SHW_TEMPLATE);
  }
  if (bN) {
    for (n=AppNode1st(dlg->w->app,&ix);n!=NULL;n=Next(&ix)) {
      if (IsLocked(n)) {
        Cancel(dlg->w->app);
        ErrorBox(dlg->wDlg,WhyLocked(dlg->w,n));
        return;
      }
      ChangeNode(dlg->w->app,n,-n->y,n->x); /* need to change this line */
    }
    SetViewFlags(dlg->w,dlg->w->showFlags | SHW_ELEMS);
  }
  LockAppUpdate(dlg->w->app,-1);
  UndoMark(dlg->w->app);
}

static void CbRotMoveDlgMove(Widget wg,RotMoveDlg dlg,void* pcbs) {
  Node n;
  Index ix;
  double x,y,scale;
  int bN,bT;

  SetActiveView(dlg->w);

  x=GetXmTextDouble(dlg->wMoveX);
  y=GetXmTextDouble(dlg->wMoveY);
  scale=GetXmTextDouble(dlg->wScale);
  if (x==MAXDOUBLE || y==MAXDOUBLE || scale==MAXDOUBLE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

  if (scale<=0) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadScale",NULL,NULL));
    return;
  }

  bN=XTBGS(dlg->wSwElems);
  bT=XTBGS(dlg->wSwTemplate);
  if (!bT && !bN) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_NOTHINGTODO));
    return;
  }
  LockAppUpdate(dlg->w->app,1);
  if (bT) {
    ChangeTemplate(dlg->w->app,dlg->w->app->template->xIncr*scale+x,
      dlg->w->app->template->yIncr*scale+y,dlg->w->app->template->angle,
      dlg->w->app->template->scale*scale);
    SetViewFlags(dlg->w,dlg->w->showFlags | SHW_TEMPLATE);
  }
  if (bN) for (n=AppNode1st(dlg->w->app,&ix);n!=NULL;n=Next(&ix)) {
    if (IsLocked(n)) {
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,WhyLocked(dlg->w,n));
      return;
    }
    ChangeNode(dlg->w->app,n,n->x*scale+x,n->y*scale+y);
    SetViewFlags(dlg->w,dlg->w->showFlags | SHW_ELEMS);
  }
  LockAppUpdate(dlg->w->app,-1);
  UndoMark(dlg->w->app);
}

static RotMoveDlg CreateRotMoveDlg(View w,Widget wParent) {
  RotMoveDlg dlg;
  Widget wg,wg1;

  dlg=Malloc(sizeof(*dlg));
  dlg->w=w;
  dlg->wDlg=CreateOkCancelDialog(wParent,DLG_ROTMOVE);
  XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
  XtAddCallback(dlg->wDlg,XmNokCallback,
    (XtCallbackProc)CbRotMoveDlgMove,dlg);
  XtAddCallback(dlg->wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

  wg=Cmw(XmCreatePushButton,dlg->wDlg,"rotate",
    NULL);
  XtAddCallback(wg,XmNactivateCallback,
    (XtCallbackProc)CbRotMoveDlgRotate,dlg);

  wg1=Cmw(XmCreateForm,dlg->wDlg,"form",
    NULL);

  CreateWidgetSystem(wg1,
    "#8?:form1",&wg,
     "l#:xLabel",1,1,
     "x#?:x",2,1,&dlg->wMoveX,
     "l#:yLabel",1,2,
     "x#?:y",2,2,&dlg->wMoveY,
     "l#:scaleLabel",1,3,
     "x#?:scale",2,3,&dlg->wScale,
    "-#:",
    NULL);

  Cmw(XmCreateSeparator,wg1,"separ",
    XmNorientation,XmHORIZONTAL,

    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    XmNtopAttachment,XmATTACH_FORM,
    NULL);

  wg=Cmw(XmCreateRowColumn,wg1,"rowCol",
    XmNorientation,XmHORIZONTAL,

    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    XmNtopAttachment,XmATTACH_WIDGET,
    XmNtopWidget,wg,
    XmNbottomAttachment,XmATTACH_FORM,
    NULL);

  dlg->wSwElems=Cmw(XmCreateToggleButton,wg,"elems",
    NULL);
  AddDependentWidget(w,dlg->wSwElems,N_NOW | N_NEWAPP | N_ALT,NULL,
    DwNotifyIfExists,(void*)T_ELEM);

  dlg->wSwTemplate=Cmw(XmCreateToggleButton,wg,"template",
    NULL);
  AddDependentWidget(w,dlg->wSwTemplate,N_NOW | N_NEWAPP | N_ALT,NULL,
    DwNotifyIfExists,(void*)T_TEMPLATE);

  XtManageChild(dlg->wDlg);

  return dlg;
}

Widget OpenRotMoveDlg(View w) {
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_ROTMOVE);
  if (wDlg==NULL) wDlg=CreateRotMoveDlg(w,w->x->wMain)->wDlg;
  else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  GluePoints menu command & dialog box                        // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _GluePointsDlg {
  View w;
  Widget wDlg,wMaxDist,wSwMarkedOnly;
}* GluePointsDlg;

Widget OpenGluePointsDlg(View w);
void CbGluePointsDlgOk(Widget,XtPointer xtpD,XtPointer);

/* Public function ////////////////////////////////////////////////// */

void CbGluePoints(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View) xtpV;

  OpenGluePointsDlg(w);
}

/* Dialog box /////////////////////////////////////////////////////// */

Widget OpenGluePointsDlg(View w) {
  GluePointsDlg dlg;
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_GLUEPOINTS);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_GLUEPOINTS);
    XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(dlg->wDlg,XmNokCallback,CbGluePointsDlgOk,(XtPointer)dlg);
    XtAddCallback(dlg->wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    CreateWidgetSystem(dlg->wDlg,
      "#:form",
        "l#:maxDistLabel",1,1,
        "x#?:maxDist",2,1,&dlg->wMaxDist,
        "l#:millimeters",3,1,
        "t#?:markedOnly",2,2,&dlg->wSwMarkedOnly,
      "-#:",
      NULL);
    XtManageChild(dlg->wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);
}

void CbGluePointsDlgOk(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  GluePointsDlg dlg=(GluePointsDlg)xtpD;
  double maxDist;
  int i,r=0,bMarkedOnly;

  maxDist=GetXmTextDouble(dlg->wMaxDist);
  if (maxDist==MAXDOUBLE || maxDist<=0) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadMaxDist",NULL,
      NULL));
    return;
  }

  bMarkedOnly=XmToggleButtonGetState(dlg->wSwMarkedOnly);

  i=GlueNodes(dlg->w->app,bMarkedOnly? dlg->w->app->mark : NULL,
      maxDist,&r);

  if (i) {
    Cancel(dlg->w->app);
    MessageBox(dlg->wDlg,GetStr(dlg->w,i));
    return;
  } else {
    UndoMark(dlg->w->app);
    XtPopdown(XtParent(dlg->wDlg));
    SetViewMsg(dlg->w,GetResourceStringEx(dlg->wDlg,"msgElemsAdded",NULL,
      "$(COUNT)%d",r));
  }
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  GlueElems menu command & dialog box                         // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _GlueElemsDlg {
  View w;
  Widget wDlg,wThreshold,wSwMarkedOnly,wMaxLen,wSwCutLonger;
}* GlueElemsDlg;

Widget OpenGlueElemsDlg(View w);
void CbGlueElemsDlgOk(Widget,XtPointer xtpD,XtPointer);

/* Public function ////////////////////////////////////////////////// */

void CbReduceElements(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View) xtpV;

  OpenGlueElemsDlg(w);
}

/* Dialog box /////////////////////////////////////////////////////// */

Widget OpenGlueElemsDlg(View w) {
  GlueElemsDlg dlg;
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_GLUEELEMS);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_GLUEELEMS);
    XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(dlg->wDlg,XmNokCallback,CbGlueElemsDlgOk,(XtPointer)dlg);
    XtAddCallback(dlg->wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    CreateWidgetSystem(dlg->wDlg,
      "#:form",
        "l#:thresholdLabel",1,1,
        "x#?:threshold",2,1,&dlg->wThreshold,
        "l#:millimeters",3,1,
        "l#:maxLenLabel",1,2,
        "x#?:maxLen",2,2,&dlg->wMaxLen,
        "l#:millimeters",3,2,
        "t#?:cutLonger",2,3,&dlg->wSwCutLonger,
        "t#?:markedOnly",2,4,&dlg->wSwMarkedOnly,
      "-#:",
      NULL);
    XtManageChild(dlg->wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);
}

void CbGlueElemsDlgOk(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  GlueElemsDlg dlg=(GlueElemsDlg)xtpD;
  double maxDist,maxLen;
  int i,r=0,bMarkedOnly,bCutLonger;

  maxDist=GetXmTextDouble(dlg->wThreshold);
  if (maxDist==MAXDOUBLE || maxDist<=0) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadMaxDist",NULL,
      NULL));
    return;
  }

  maxLen=GetXmTextDouble(dlg->wMaxLen);
  if (maxLen==MAXDOUBLE || maxLen<=0) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadMaxLen",NULL,
      NULL));
    return;
  }

  bMarkedOnly=XmToggleButtonGetState(dlg->wSwMarkedOnly);
  bCutLonger=XmToggleButtonGetState(dlg->wSwCutLonger);

  i=GlueElems(dlg->w->app,bMarkedOnly? dlg->w->app->mark : NULL,
      maxDist,maxLen,bCutLonger,&r);

  if (i) {
    Cancel(dlg->w->app);
    MessageBox(dlg->wDlg,GetStr(dlg->w,i));
    return;
  } else {
    UndoMark(dlg->w->app);
    XtPopdown(XtParent(dlg->wDlg));
    SetViewMsg(dlg->w,GetResourceStringEx(dlg->wDlg,"msgElemsRemoved",NULL,
      "$(COUNT)%d",r));
  }
}

/* ////////////////////////////////////////////////////////////////// */

void CbGroupNormals(Widget wg,XtPointer xtpV,XtPointer pbcs) {
  View w=(View)xtpV;
  int i,r;
  Group g;

  i=GlueNormals(w->app,(int)GetUserData(wg)? w->app->mark : NULL,&r);
  if (i) {
    Cancel(w->app);
    MessageBox(w->x->wMain,GetStr(w,i));
    return;
  } else {
    UndoMark(w->app);
    SetViewMsg(w,GetResourceStringEx(w->x->wMain,"msgNormalsGrouped",NULL,
        "$(COUNT)%d",r));
  }
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  Stretch Dialog                                              // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */


typedef struct _StretchDlg {
  View w;
  Widget wDlg,wFactorX,wFactorY;
}* StretchDlg;

static void StretchDlgResetFields(StretchDlg dlg) {
  char s[256];

  sprintf(s,"%f",1.);
  XmTextSetString(dlg->wFactorX,s);
  XmTextSetString(dlg->wFactorY,s);
}

static void CbStretchDlgSetFactors(Widget wg,StretchDlg dlg,void* pcbs) {
  double xScale,yScale;

  ValidatePtr(dlg->w,"CbStretchDlgSetFactors");
  SetActiveView(dlg->w);

  xScale=GetXmTextDouble(dlg->wFactorX);
  yScale=GetXmTextDouble(dlg->wFactorY);
  if (xScale==0 || xScale==MAXDOUBLE || yScale==0 || yScale==MAXDOUBLE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

  SetViewRect(dlg->w,
    1/xScale*dlg->w->minX+(1-1/xScale)*dlg->w->centerX,
    1/yScale*dlg->w->minY+(1-1/yScale)*dlg->w->centerY,
    1/xScale*dlg->w->maxX+(1-1/xScale)*dlg->w->centerX,
    1/yScale*dlg->w->maxY+(1-1/yScale)*dlg->w->centerY);
  UndoMark(dlg->w->app);
}

static void CbStretchDlgReset(Widget wg,StretchDlg dlg,void* pcbs) { 
  StretchDlgResetFields(dlg);
}

static StretchDlg CreateStretchDlg(View w,Widget wParent) {
  StretchDlg dlg;
  Widget wg,wg1;

  dlg=Malloc(sizeof(*dlg));
  dlg->w=w;
  dlg->wDlg=CreateOkCancelDialog(wParent,DLG_STRETCH);
  XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
  XtAddCallback(dlg->wDlg,XmNokCallback,
    (XtCallbackProc)CbStretchDlgSetFactors,dlg);
  XtAddCallback(dlg->wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

  wg=Cmw(XmCreatePushButton,dlg->wDlg,"reset",
    NULL);
  XtAddCallback(wg,XmNactivateCallback,
    (XtCallbackProc)CbStretchDlgReset,dlg);

  wg1=Cmw(XmCreateForm,dlg->wDlg,"form",
    NULL);

  CreateWidgetSystem(wg1,
    "#8?:form1",&wg,
     "l#:xLabel",1,1,
     "x#?:x",2,1,&dlg->wFactorX,
     "l#:yLabel",1,2,
     "x#?:y",2,2,&dlg->wFactorY,
    "-#:",
    NULL);

  StretchDlgResetFields(dlg);

  Cmw(XmCreateSeparator,wg1,"separ",
    XmNorientation,XmHORIZONTAL,

    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    XmNtopAttachment,XmATTACH_FORM,
    NULL);

  XtManageChild(dlg->wDlg);

  return dlg;
}

Widget OpenStretchDlg(View w) {
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_STRETCH);
  if (wDlg==NULL) wDlg=CreateStretchDlg(w,w->x->wMain)->wDlg;
  else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* //  Set Angle Dialog                                            // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

typedef struct _SetAngleDlg {
  View w;
  Widget wDlg,wAngle;
}* SetAngleDlg;

static void CbSetAngle(Widget wg,SetAngleDlg dlg,void* pcbs) {
  double deg;

  ValidatePtr(dlg->w,"CbSetAngle");
  SetActiveView(dlg->w);

  deg=GetXmTextDouble(dlg->wAngle);
  if (deg==MAXDOUBLE) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }
  SetViewAngle(dlg->w,M_PI/180*deg);

  XtPopdown(XtParent(dlg->wDlg));
  UndoMark(dlg->w->app);
}

Widget OpenSetAngleDlg(View w) {
  XtPointer xtp;
  SetAngleDlg dlg;
  Widget wDlg,wg;
  char s[256];

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_SETANGLE);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->wDlg=wDlg=CreateOkCancelDialog(w->x->wMain,DLG_SETANGLE);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbSetAngle,dlg);
    XtUnmanageChild(XtNameToWidget(wDlg,"Help"));

    wg=Cmw(XmCreateForm,wDlg,"form",
      NULL);
    CreateMenuSystem(wg,
    "l@:angleLabel",0x0101,
    "x?@:angle",&dlg->wAngle,0x0102,
       NULL);
    Form2Table(wg);
    sprintf(s,"%f",180/M_PI*w->xyAngle);
    XmTextSetString(dlg->wAngle,s);
    XtManageChild(wDlg);
  }
  else {
    sprintf(s,"%f",180/M_PI*w->xyAngle);
    wg=XtNameToWidget(w->x->wMain,"*angle");
    if (wg==NULL)  fprintf(stderr,"couldn't set string\n");
    XmTextSetString(wg,s);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }

  return wDlg;
}

