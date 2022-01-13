#include "x_dg.h"

#define DLG_ABOUT "dlgAbout"
#define DLG_INFO "dlgInfo"

typedef struct _InfoDlg {
  View w;
  XApp xapp;
  Widget wParent;
  int alt;
  Widget wDlg,wMouseX,wMouseY,
    wChords,
    wNodes,wElems,wUnusedNumbers,wSurfaces,wGridPoints,wSeparators,wSources;
}* InfoDlg;

Widget OpenAboutDlg(View w) {
  Widget wDlg;
  XmString xms;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_ABOUT);
  if (wDlg==NULL) {
    wDlg=Cw(XmCreateMessageDialog,w->x->wMain,DLG_ABOUT,
      NULL);

    xms=MakeXmString(GetResourceStringEx(wDlg,"messageStringEx",
      "MessageStringEx","$(VERSION)%s",GetVersionStr(DG_VERSION)));
    SetValues(wDlg,XmNmessageString,xms,NULL);
    XmStringFree(xms);

    XtUnmanageChild(XtNameToWidget(wDlg,"Cancel"));
    XtUnmanageChild(XtNameToWidget(wDlg,"Help"));
    XtManageChild(wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

static void UpdateInfoDlg(InfoDlg dlg) {
  char s[DG_FNAME_LEN*2],buf[1024];
  XmString xms;
  SurfaceEx sx;
  SurfaceZone sz;
  GridPointSeg gps;
  GridPointEx gpx;
  Index ixsz,ixsx;
  int n;

  if (dlg->w->app==NULL) return;

  dlg->alt=dlg->w->app->alt;

  sprintf(s,"%u",(unsigned)GroupCount(dlg->w->app->nodes));
  SetLabelString(dlg->wNodes,s);

  sprintf(s,"%u",(unsigned)GroupCount(dlg->w->app->elems));
  SetLabelString(dlg->wElems,s);

  sprintf(s,"%u",(unsigned)GroupCount(dlg->w->app->sources));
  SetLabelString(dlg->wSources,s);

  sprintf(s,"%u",(unsigned)GetUnusedNumberCount(dlg->w->app));
  SetLabelString(dlg->wUnusedNumbers,s);

  SetLabelString(dlg->wSurfaces,GetSurfacesStatsStr(dlg->w->app));

  SetLabelString(dlg->wGridPoints,GetGridPointStatsStr(dlg->w->app));

  sprintf(s,"%u",(unsigned)GroupCount(dlg->w->app->separators));
  SetLabelString(dlg->wSeparators,s);

  sprintf(s,"%u",(unsigned)GroupCount(dlg->w->app->chords));
  SetLabelString(dlg->wChords,s);
}

static void DwNotifyInfoDlg(Widget wg,View w,int evt,void* obj,
    InfoDlg idlg) {
  UpdateInfoDlg(idlg);
}

static InfoDlg CreateInfoDlg(View w,Widget wParent) {
  InfoDlg dlg;
  Widget wF,wF1,wF2,wg;

  dlg=Malloc(sizeof(*dlg));
  dlg->xapp=w->xapp;
  dlg->w=w;
  dlg->wParent=wParent;

  dlg->wDlg=CreateMessageDialog(wParent,DLG_INFO,
    "bA:close",CbUnmap,NULL,
    NULL);
  SetValues(XtParent(dlg->wDlg),XmNdeleteResponse,XmDO_NOTHING,NULL);
  XmAddWMProtocolCallback(XtParent(dlg->wDlg),w->xapp->x->wm_delete_window,
    CbUnmap,NULL);
  XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

  CreateWidgetSystem(dlg->wDlg,
    "#:form",
     "#4:table",
      "l#:nodesLabel",1,1,
      "l#:elemsLabel",1,2,
      "l#:unusedLabel",1,3,
      "l#:sourcesLabel",1,4,
      "l#:surfacesLabel",1,5,
      "l#:gridPointsLabel",1,6,
      "l#:separatorsLabel",1,7,
      "l#:chordsLabel",1,8,
     "-#:",
     "#5:values",
      "l#?:nodes",2,1,&dlg->wNodes,
      "l#?:elems",2,2,&dlg->wElems,
      "l#?:unused",2,3,&dlg->wUnusedNumbers,
      "l#?:sources",2,4,&dlg->wSources,
      "l#?:surfaces",2,5,&dlg->wSurfaces,
      "l#?:gridPoints",2,6,&dlg->wGridPoints,
      "l#?:separators",2,7,&dlg->wSeparators,
      "l#?:chords",2,8,&dlg->wChords,
     "-#:",
    "-:",
    NULL);

  AddDependentWidget(w,dlg->wDlg,N_NOW | N_NEWAPP | N_ALT,NULL,
    (DwNotifyProc)DwNotifyInfoDlg,dlg);

  return dlg;
}

Widget OpenInfoDlg(View w) {
  InfoDlg dlg;
  Index ix;
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_INFO);
  if (wDlg==NULL) {
    dlg=CreateInfoDlg(w,w->x->wMain);
    wDlg=dlg->wDlg;
    XtManageChild(dlg->wDlg);
  } else {
    XtPopup(XtParent(wDlg),XtGrabNone);
  }
  return wDlg;
}

