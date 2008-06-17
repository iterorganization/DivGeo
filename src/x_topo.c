#include "x_dg.h"

#define DLG_TOPOLOGY "dlgTopology"
#define DLG_IMPORTTOPO "dlgImportTopology"
#define ENV_IMPORT_TOPO_MASK "DG_IMPORT_TOPOLOGY_MASK"

static Widget OpenTopologyDlg(View w);

#define SZ_NEWPTR ((void*)-4)

void CmEditTopology(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  SetActiveView(w);

  OpenTopologyDlg(w);
}

typedef struct _TopologyDlg {
  View w;

  int oldOutputMode;

  Widget wDlg,wSwXPoints,wSwSections,wSwZones,wSwName,
    wXptLevel,wXptLevelBtn,
    wXSegId,wXSegName,wXSegDescr,wXSegBtn,wXSegRevertBtn,wXSegUsed,
    wXSegTargetCW,
    wSZList,wSZId,wSZname,wSZdescr,wSZgps1,wSZgps2,wSZorient,wSZClosed,
    wSZSetBtn,wSZDeleteBtn,
    wTNname,wTNsetBtn;
  void* curObject;
}* TopologyDlg;

/*static void CbCreateGridPointOk(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbCGPD_PickSettings(Widget wg,XtPointer xtpD,XtPointer pcbs);*/

static void DwTopoDlg(Widget wg,View w,int evt,void* obj,void* udt);

static void CbTopoUnmap(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoDetectXPoints(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoDetectXSegs(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoXptLevelBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoXSegBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoXSegRevert(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoSZSetBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoSZNewBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoSZDeleteBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoSZListSelect(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void TopoDlgUpdateList(TopologyDlg dlg);
static void InitSZFromZone(TopologyDlg dlg,SurfaceZone sz);
static void CbTopoNameBtn(Widget wg,XtPointer xtpD,XtPointer pcbs);
static void CbTopoNameChange(Widget wg,XtPointer xtpD,XtPointer pcbs);

static void CbToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs);

static Widget OpenTopologyDlg(View w) {
  Atom wm_delete_window;
  XtPointer xtp;
  TopologyDlg dlg;
  Widget wDlg,
    wFrameXPoints,wFrameSections,wFrameZones,wFrameName;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_TOPOLOGY);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->oldOutputMode=-1;
    dlg->curObject=NULL;

    dlg->wDlg=wDlg=Cw(XmCreateMessageDialog,w->x->wMain,DLG_TOPOLOGY,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNuserData,(XtPointer)dlg,
      NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);


    XtAddCallback(wDlg,XmNcancelCallback,CbTopoUnmap,(XtPointer)dlg);
    wm_delete_window=XmInternAtom(XtDisplay(w->x->wMain),
      "WM_DELETE_WINDOW",False);
    XmAddWMProtocolCallback(XtParent(wDlg),wm_delete_window,
      CbTopoUnmap,(XtPointer)dlg);

      XtUnmanageChild(XmMessageBoxGetChild(wDlg,XmDIALOG_MESSAGE_LABEL));
      XtUnmanageChild(XmMessageBoxGetChild(wDlg,XmDIALOG_OK_BUTTON));
      XtUnmanageChild(XmMessageBoxGetChild(wDlg,XmDIALOG_CANCEL_BUTTON));
      XtUnmanageChild(XmMessageBoxGetChild(wDlg,XmDIALOG_HELP_BUTTON));

/*    XtAddCallback(wDlg,XmNokCallback,
        (XtCallbackProc)CbCreateGridPointOk,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w); */

    CreateWidgetSystem(wDlg,
      "#:mainForm",
/*        "$+8:dialogKind",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL, */
       "$+8:dialogKind",XmCreateRowColumn,XmNorientation,XmHORIZONTAL,NULL,
        "t?:xpoints",&dlg->wSwXPoints,
        "t?:sections",&dlg->wSwSections,
        "t?:zones",&dlg->wSwZones,
        "t?:name",&dlg->wSwName,
       "-:",

       /* *** Create XPoints *** */

       "f5?:tabFrame",&wFrameXPoints,
        "#:xpointsForm",
          "b#A:detectAllXPoints",0,1,CbTopoDetectXPoints,(XtPointer)dlg,
          "s-#:separator",0,5,
          "l#:xptLevelLabel",1,6,
          "x#?:levelText",2,6,&dlg->wXptLevel,
          "b#?A:xptLevelBtn",3,6,&dlg->wXptLevelBtn,CbTopoXptLevelBtn,(XtPointer)dlg,
        "-#:",
       "-:",

       /* *** Create Sections *** */

       "f5?_:tabFrame",&wFrameSections,
        "#:sectionsForm",
         "b#A:detectAll",0,1,CbTopoDetectXSegs,(XtPointer)dlg,
         "s-#:separator",0,5,
         "l#:sectTitle",0,6,
         "l#:idLabel",1,10,
         "x#?:id",2,10,&dlg->wXSegId,
         "l#:shortLabel",1,11,
         "x#?:short",2,11,&dlg->wXSegName,
         "l#:longLabel",1,12,
         "x#?:long",2,12,&dlg->wXSegDescr,
         "t#?:gpsTargetCW",1,13,&dlg->wXSegTargetCW,
         "t#?:sectUsed",2,13,&dlg->wXSegUsed,
         "b#?A:sectBtn",0,20,&dlg->wXSegBtn,CbTopoXSegBtn,(XtPointer)dlg,
         "b#?A:sectRevertBtn",0,21,&dlg->wXSegRevertBtn,CbTopoXSegRevert,(XtPointer)dlg,
        "-#:",
       "-:",

       /* *** Create Zones *** */

       "f5?_:tabFrame",&wFrameZones,
        "#:zonesForm",
         "##:listForm",0,1,
          "$?5:list",XmCreateScrolledList,
              XmNselectionPolicy,XmBROWSE_SELECT,NULL,&dlg->wSZList,
         "-:",
         "s-#:separator",0,5,
         "l#:zonesTitle",0,6,
         "l#:zidLabel",1,10,
         "x#?:id",2,10,&dlg->wSZId,
         "l#:zshortLabel",1,11,
         "x#?:short",2,11,&dlg->wSZname,
         "l#:zlongLabel",1,12,
         "x#?:long",2,12,&dlg->wSZdescr,
         "l#:gps1Label",1,13,
         "x#?:gps1",2,13,&dlg->wSZgps1,
         "l#:orientLabel",1,15,
         "*:orientMenu",
           "b@:left",(XtPointer)1,
           "b@:right",(XtPointer)-1,
         "-:",
         "o#?:orient",2,15,&dlg->wSZorient,
         "l#:gps2Label",1,14,
         "x#?:gps2",2,14,&dlg->wSZgps2,
         "t#?:closed",2,16,&dlg->wSZClosed,

         "b#?A:szSetBtn",0,20,&dlg->wSZSetBtn,CbTopoSZSetBtn,(XtPointer)dlg,
/*         "b#A:szAddBtn",0,21,CbTopoSZNewBtn,(XtPointer)dlg, */
         "b#?A:szDeleteBtn",0,22,&dlg->wSZDeleteBtn,
             CbTopoSZDeleteBtn,(XtPointer)dlg,
        "-#:",
       "-:",

       /* *** Set Name *** */

       "f5?_:tabFrame",&wFrameName,
        "#:nameForm",
          "l#:topoNameLabel",1,6,
          "x#?T:topoName",2,6,&dlg->wTNname,CbTopoNameChange,(XtPointer)dlg,
          "b#?A:topoNameBtn",3,6,&dlg->wTNsetBtn,CbTopoNameBtn,(XtPointer)dlg,
        "-#:",
       "-:",

      "-:",
      NULL);

    SetValues(XtParent(dlg->wSZList),XmNtopAttachment,XmATTACH_FORM,NULL);
    SetValues(XtParent(dlg->wSZList),XmNbottomAttachment,XmATTACH_FORM,NULL);
    SetValues(XtParent(dlg->wSZList),XmNleftAttachment,XmATTACH_FORM,NULL);
    SetValues(XtParent(dlg->wSZList),XmNrightAttachment,XmATTACH_FORM,NULL);
    XtAddCallback(dlg->wSZList,XmNbrowseSelectionCallback,CbTopoSZListSelect,
      (XtPointer)dlg);

    SetValues(dlg->wDlg,XmNdefaultButton,NULL,NULL);

    XmToggleButtonSetState(dlg->wSwXPoints,True,True);
    XtAddCallback(dlg->wSwXPoints,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameXPoints);
    XtAddCallback(dlg->wSwSections,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameSections);
    XtAddCallback(dlg->wSwZones,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameZones);
    XtAddCallback(dlg->wSwName,XmNvalueChangedCallback,
        CbToggleManaged,(XtPointer)wFrameName);

/*    wg=XmMessageBoxGetChild(wDlg,XmDIALOG_OK_BUTTON);
    AddDependentWidget(w,wg,N_NOW | N_NEWAPP | N_ALT,NULL,
      DwNotifyIfExists,(void*)T_XPOINT_NOLONGEROK);
    XtManageChild(wDlg);

    DG_Init(&dlg->dg,w,dlg->wDlg,
        dlg->wMDraw,dlg->wMCount,dlg->wMAlpha,wMAlphaLabel,
        dlg->wMAlpha2,wMAlpha2Label,dlg->wMLaw,wLawLabel); */

    AddDependentWidget(w,dlg->wDlg,N_NOW | N_NEWAPP | N_ALT,NULL,
      DwTopoDlg,(void*)dlg);

    XtManageChild(wDlg);
  }
  else {
    dlg=GetUserData(wDlg);
    DwTopoDlg(NULL,w,N_NOW,NULL,dlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }

  SetViewEditTopology(w,1);

/*  SetViewFlags(w,w->showFlags | SHW_GRIDPOINTS);
  UndoMark(w->app); */

  TopoDlg_NotifyExamine(w,NULL);

/*  TopoDlgUpdateList(dlg);*/

  return wDlg;
}

static void CbToggleManaged(Widget wg,XtPointer xtpWmanage,XtPointer pcbs) {
  Widget wManage=(Widget)xtpWmanage;

  if (XmToggleButtonGetState(wg))
    XtManageChild(wManage);
  else XtUnmanageChild(wManage);
}


static void CbTopoUnmap(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  SetViewEditTopology(dlg->w,0);
  CbUnmap(wg,NULL,NULL);
}

static void CbTopoDetectXPoints(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  SetActiveView(dlg->w);

  LockAppUpdate(dlg->w->app,1);
  DetectXPoints(dlg->w->app);
  LockAppUpdate(dlg->w->app,-1);

  UndoMark(dlg->w->app);
}

static void CbTopoDetectXSegs(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  SetActiveView(dlg->w);

  LockAppUpdate(dlg->w->app,1);
  DetectXPointSegs(dlg->w->app);
  LockAppUpdate(dlg->w->app,-1);

  UndoMark(dlg->w->app);
}

void TopoDlg_NotifyExamine(View w,void* object) {
  Widget wDlg;
  TopologyDlg dlg;
  char buf[4096];
  XPointTest xpt;
  XPointSeg xps;
  GridPointSeg gps;
  SurfaceZone sz;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_TOPOLOGY);
  if (wDlg==NULL) return;

  dlg=(TopologyDlg)GetUserData(wDlg);
  assert(dlg!=NULL);

  SetSensitiveEx(dlg->wXptLevelBtn,False);
  SetSensitiveEx(dlg->wXSegRevertBtn,False);

  if (object==NULL) return;

  switch (GetObjType(object)) {
    case T_XPOINTTEST:
      XmToggleButtonSetState(dlg->wSwXPoints,True,True);
      xpt=object;
      sprintf(buf,"%g",xpt->level);
      XmTextSetString(dlg->wXptLevel,buf);
      SetSensitiveEx(dlg->wXptLevelBtn,True);
      dlg->curObject=xpt;
      break;
    case T_XPOINTSEG:
      if (XmToggleButtonGetState(dlg->wSwXPoints))
        XmToggleButtonSetState(dlg->wSwSections,True,True);
      xps=object;
      gps=FindGridPointSegBySegment(dlg->w->app,xps);
      assert(gps!=NULL);
      sprintf(buf,"%d",gps->zone);
      XmTextSetString(dlg->wXSegId,buf);
      XmTextSetString(dlg->wXSegName,GetGridPointSegShortName(gps));
      XmTextSetString(dlg->wXSegDescr,GetGridPointSegLongName(gps));
      SetSensitiveEx(dlg->wXSegBtn,True);
      XmToggleButtonSetState(dlg->wXSegUsed,!!(gps->flags & GPSF_USED),False);
      XmToggleButtonSetState(dlg->wXSegTargetCW,
          !!(gps->flags & GPSF_TARGET_CW),False);
      SetSensitiveEx(dlg->wXSegRevertBtn,True);
      dlg->curObject=xps;
      break;
  }
}

static void CbTopoXptLevelBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  double level;
  int cx1,cy1,cx2,cy2,id;
  XPointTest xpt;

  SetActiveView(dlg->w);

  if (dlg->curObject==NULL ||
      !InGroup(dlg->w->app->xpointTests,dlg->curObject) ||
      GetObjType(dlg->curObject)!=T_XPOINTTEST) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errCurObjectGone",NULL,NULL));
    return;
  }
  xpt=dlg->curObject;

  level=GetXmTextDouble(dlg->wXptLevel);
  if (level==MAXDOUBLE) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadXptLevel",NULL,NULL));
    return;
  }

  cx1=xpt->cx1;
  cy1=xpt->cy1;
  cx2=xpt->cx2;
  cy2=xpt->cy2;
  id=xpt->id;

  DelXPointTest(dlg->w->app,xpt);
  xpt=AddXPointTest(dlg->w->app,cx1,cy1,cx2,cy2,level,id);

  if (xpt==NULL) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadNewLevel",NULL,NULL));
    return;
  }

  UndoMark(dlg->w->app);
  SetSensitiveEx(dlg->wXptLevelBtn,0);
  SetViewMsg(dlg->w,GetResourceString(dlg->wDlg,"msgXptLevelAdjusted",NULL,NULL));
}

static void CbTopoXSegBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  int zone;
  XPointSeg xps;
  GridPointSeg gps,gps1;
  GridPointEx gpx;
  String s;
  Index ix;
  long flags;

  SetActiveView(dlg->w);

  if (dlg->curObject==NULL ||
      !InGroup(dlg->w->app->xPointSegs,dlg->curObject) ||
      GetObjType(dlg->curObject)!=T_XPOINTSEG) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errCurObjectGone",NULL,NULL));
    return;
  }
  xps=dlg->curObject;
  gps=FindGridPointSegBySegment(dlg->w->app,xps);
  assert(gps!=NULL);

  zone=GetXmTextInt(dlg->wXSegId);
  if (zone==MAXINT) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadXpsZone",NULL,NULL));
    return;
  }

  for (gps1=AppGridPointSeg1st(dlg->w->app,&ix);gps1!=NULL;gps1=Next(&ix))
    if (gps1!=gps && gps1->zone==zone) {
      ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errDupXpsZone",NULL,NULL));
      return;
    }

  SetGridPointSegZone(gps,zone);

  s=XmTextGetString(dlg->wXSegName);
  SetGridPointSegShortName(gps,s);
  XtFree(s);

  s=XmTextGetString(dlg->wXSegDescr);
  SetGridPointSegLongName(gps,s);
  XtFree(s);

  flags=gps->flags & ~GPSF_USED & ~GPSF_TARGET_CW;
  if (XmToggleButtonGetState(dlg->wXSegUsed)) flags |= GPSF_USED;
  if (XmToggleButtonGetState(dlg->wXSegTargetCW)) flags |= GPSF_TARGET_CW;
  SetGridPointSegFlags(gps,flags);

  if (!(gps->flags & GPSF_USED)) {
    for (gpx=AppGridPointEx1st(gps->app,&ix);gpx!=NULL;gpx=Next(&ix))
      if (gpx->zone==gps->zone) DelGridPointEx(gpx);
  }

  UndoMark(dlg->w->app);
  SetSensitiveEx(dlg->wXSegBtn,0);
  SetViewMsg(dlg->w,GetResourceString(dlg->wDlg,"msgXSegChanged",NULL,NULL));
}

static void CbTopoXSegRevert(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  int zone;
  XPointSeg xps;
  GridPointSeg gps,gps1;
  String s;
  Index ix;

  SetActiveView(dlg->w);

  if (dlg->curObject==NULL ||
      !InGroup(dlg->w->app->xPointSegs,dlg->curObject) ||
      GetObjType(dlg->curObject)!=T_XPOINTSEG) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errCurObjectGone",NULL,NULL));
    return;
  }
  xps=dlg->curObject;
  gps=FindGridPointSegBySegment(dlg->w->app,xps);
  assert(gps!=NULL);

  SetGridPointSegDir(gps,-gps->dir);
  UndoMark(dlg->w->app);

  UndoMark(dlg->w->app);
  SetViewMsg(dlg->w,GetResourceString(dlg->wDlg,"msgXSegReverted",NULL,NULL));
}

static void CbTopoSZSetBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  int zone;
  SurfaceZone sz,sz1;
  int gps2,gps1,orient;
  long flags;
  String s;
  Index ix;

  SetActiveView(dlg->w);

  if (dlg->curObject!=SZ_NEWPTR) {
    if (dlg->curObject==NULL ||
        !InGroup(dlg->w->app->surfaceZones,dlg->curObject) ||
        GetObjType(dlg->curObject)!=T_SURFACEZONE) {
      ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errCurObjectGone",NULL,NULL));
      return;
    }
    sz=dlg->curObject;
  } else sz=NULL;

  zone=GetXmTextInt(dlg->wSZId);
  if (zone==MAXINT || zone<0) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadSZone",NULL,NULL));
    return;
  }

  for (sz1=AppSurfaceZone1st(dlg->w->app,&ix);sz1!=NULL;sz1=Next(&ix))
    if (sz1!=sz && sz1->zone==zone) {
      ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errDupSZone",NULL,NULL));
      return;
    }

  gps1=GetXmTextInt(dlg->wSZgps1);
  if (IsXmTextEmpty(dlg->wSZgps2)) gps2=-1;
  else gps2=GetXmTextInt(dlg->wSZgps2);

  orient=(int)GetOptionMenuValue(dlg->wSZorient);
  if (orient!=1) orient=-1; /* If the convertion pointer->negative int fails */

  if (gps1==MAXINT || gps2==MAXINT || gps2<-1) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errSZBadGPZone",NULL,NULL));
    return;
  }

  if (FindGridPointSeg(dlg->w->app,gps1)==NULL ||
      gps2>=0 && FindGridPointSeg(dlg->w->app,gps2)==NULL) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errSZNotFoundGPZone",NULL,NULL));
    return;
  }

/*  if (sign==MAXINT || sign!=1 && sign!=-1) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errSZBadSign",NULL,NULL));
    return;
  } */

  flags=0;
  if (XmToggleButtonGetState(dlg->wSZClosed)) flags |= SZF_LIMITBYSURFACE;

  if (sz!=NULL) DelSurfaceZone(sz);

  sz=AddSurfaceZone(dlg->w->app,zone,gps1,gps2,orient);
  SetSurfaceZoneFlags(sz,flags);

  s=XmTextGetString(dlg->wSZname);
  SetSurfaceZoneShortName(sz,s);
  XtFree(s);

  s=XmTextGetString(dlg->wSZdescr);
  SetSurfaceZoneLongName(sz,s);
  XtFree(s);

  UndoMark(dlg->w->app);

/*  TopoDlgUpdateList(dlg); */

/*  SetSensitiveEx(dlg->wXSegBtn,0); */
  SetViewMsg(dlg->w,GetResourceString(dlg->wDlg,"msgSZChanged",NULL,NULL));
}


static void TopoDlgUpdateList(TopologyDlg dlg) {
  SurfaceZone sz;
  Index ix;
  XmString *xmsa;
  char buf[1024];
  int i,szCount;

  szCount=GroupCount(dlg->w->app->surfaceZones);
  xmsa=Malloc(sizeof(*xmsa)*(szCount+1));
  xmsa[0]=MakeXmString("(New)");

/*  XmListDeleteAllItems(dlg->wSZList);
  xms=MakeXmString("(New)");
  XmListAddItem(dlg->wSZList,xms,0);
  XmStringFree(xms); */

  for (i=1,sz=AppSurfaceZone1st(dlg->w->app,&ix);sz!=NULL;i++,sz=Next(&ix)) {
    sprintf(buf,"%d (%s)",sz->zone,GetSurfaceZoneLongName(sz));
    xmsa[i]=MakeXmString(buf);
  }

  SetXmListItems(dlg->wSZList,xmsa,szCount+1);
  for (i=0;i<szCount+1;i++) XmStringFree(xmsa[i]);
  xmsa=Free(xmsa);

  dlg->curObject=NULL;
  CbTopoSZListSelect(NULL,(XtPointer)dlg,NULL);
}

static void CbTopoSZListSelect(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  int pos;
  SurfaceZone sz;

  pos=GetXmListSelPos(dlg->wSZList);

  if (pos<0) {
    dlg->curObject=NULL;
    InitSZFromZone(dlg,NULL);
  } else if (pos==0) {
    dlg->curObject=SZ_NEWPTR;
    InitSZFromZone(dlg,NULL);
  } else {
    sz=GroupAt(dlg->w->app->surfaceZones,pos-1);
    dlg->curObject=sz;
    InitSZFromZone(dlg,sz);
  }
  SetSensitiveEx(dlg->wSZSetBtn,pos>=0);
  SetSensitiveEx(dlg->wSZDeleteBtn,pos>0);
}

static void CbTopoSZNewBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  dlg->curObject=SZ_NEWPTR;
  InitSZFromZone(dlg,NULL);
}

static void CbTopoSZDeleteBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  if (dlg->curObject==NULL ||
      !InGroup(dlg->w->app->surfaceZones,dlg->curObject) ||
      GetObjType(dlg->curObject)!=T_SURFACEZONE) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errCurObjectGone",NULL,NULL));
    return;
  }
  DelSurfaceZone(dlg->curObject);
  UndoMark(dlg->w->app);
/*  TopoDlgUpdateList(dlg); */
}

static void InitSZFromZone(TopologyDlg dlg,SurfaceZone sz) {
  char buf[1024];

  *buf=0;

  if (sz!=NULL) sprintf(buf,"%d",sz->zone);
  XmTextSetString(dlg->wSZId,buf);

  if (sz!=NULL) sprintf(buf,"%s",GetSurfaceZoneShortName(sz));
  XmTextSetString(dlg->wSZname,buf);

  if (sz!=NULL) sprintf(buf,"%s",GetSurfaceZoneLongName(sz));
  XmTextSetString(dlg->wSZdescr,buf);

  if (sz!=NULL) sprintf(buf,"%d",sz->gpZone1);
  XmTextSetString(dlg->wSZgps1,buf);

  if (sz!=NULL) if (sz->gpZone2<0) *buf=0; else sprintf(buf,"%d",sz->gpZone2);
  XmTextSetString(dlg->wSZgps2,buf);

  if (sz!=NULL) SetOptionMenuValue(dlg->wSZorient,(XtPointer)sz->orient);

  if (sz!=NULL)
    XmToggleButtonSetState(dlg->wSZClosed,sz->flags & SZF_LIMITBYSURFACE!=0,0);
}

static void DwTopoDlg(Widget wg,View w,int ev,void* obj,void* pDlg) {
  TopologyDlg dlg=(TopologyDlg)pDlg;

  if (ev!=N_NOW && !IsMapped(XtParent(dlg->wDlg))) return;

  XmTextSetString(dlg->wTNname,GetTopologyName(dlg->w->app));
  SetSensitiveEx(dlg->wTNsetBtn,False);

  if (ev==N_ALT && !(w->x->changes & CHF_TOPOLOGY)) return;

  TopoDlgUpdateList(dlg);
}

static void CbTopoNameBtn(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;
  int r;
  char* name,nameb[4096];

  SetActiveView(dlg->w);

  name=XmTextGetString(dlg->wTNname);
  r=sscanf(name,"%s%s",nameb,nameb);
  XtFree(name);

  if (r!=1) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadTopoName",NULL,NULL));
    return;
  }

  SetTopologyName(dlg->w->app,nameb);

  UndoMark(dlg->w->app);
  SetViewMsg(dlg->w,GetResourceString(dlg->wDlg,"msgTopoNameChanged",NULL,NULL));
}

static void CbTopoNameChange(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  TopologyDlg dlg=(TopologyDlg)xtpD;

  SetSensitiveEx(dlg->wTNsetBtn,True);
}

/* Import Topology ///////////////////////////////////////////////////// */

static Widget OpenImportTopologyDlg(View w);

void CmImportTopology(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  SetActiveView(w);

  OpenImportTopologyDlg(w);
}

typedef struct _ImportTopo_Dlg {
  View w;
  Widget wDlg,wSwDetectXPoints;
}* ImportTopoDlg;

/*static int IT_OkCheck(EEAT_Dlg dlg);*/

static void CbImportTopo(Widget,XtPointer,XtPointer);
/*static Group EEAT_GetGroup(EEAT_Dlg dlg); */

static Widget OpenImportTopologyDlg(View w) {
  Widget wg,wOptMenu;
  String s;
  XmString xms,xms2;
  ImportTopoDlg dlg;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_IMPORTTOPO);
  if (wg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_IMPORTTOPO,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNuserData,(XtPointer)dlg,
      NULL);
    dlg->wDlg=wg;
    XtAddCallback(wg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    dlg->wSwDetectXPoints=Cmw(XmCreateToggleButton,dlg->wDlg,"detectXPoints",
      NULL);

    s=getenv(ENV_IMPORT_TOPO_MASK);
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbImportTopo,
        (XtPointer)dlg);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);

/*    CreateWidgetSystem(dlg->wDlg,
      "*:optMenu",
      "b@:all",(XtPointer)EEAT_ALL,
      "b@:marked",(XtPointer)EEAT_MARKED,
      "-:",
      "o?:whichElems",&dlg->wOptMenu,
      NULL);

    AddDependentWidget(w,XmFileSelectionBoxGetChild(wg,XmDIALOG_DEFAULT_BUTTON),
      N_NOW|N_ALT|N_NEWAPP,NULL,DwNotifyIfExists,(XtPointer)T_ELEM); */

  } else {

    /* Redo the search, but keep the old selection intact*/
    
    s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
    XmFileSelectionDoSearch(wg,NULL);
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),s);
    xms=MakeXmString(s);
    XmListDeselectAllItems(XmFileSelectionBoxGetChild(wg,XmDIALOG_LIST));
    XmListSelectItem(XmFileSelectionBoxGetChild(wg,XmDIALOG_LIST),
      xms,False);
    XmStringFree(xms);
    XtFree(s);
    
    XtPopup(XtParent(wg),XtGrabNone);
  }

  return wg;
}

static void CbImportTopo(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  ImportTopoDlg dlg=(ImportTopoDlg)xtpD;
  String s;
  int i,bxpt;

  SetActiveView(dlg->w);

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  bxpt=XmToggleButtonGetState(dlg->wSwDetectXPoints);

  i=LoadTopology(dlg->w->app,s,bxpt);

  if (i) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,GetStr(dlg->w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewMsg(dlg->w,GetResourceString(wg,"msgTopoImported",NULL,NULL));
    UndoMark(dlg->w->app);
  }
  XtFree(s);
}
void CmUpdateTopology(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  SetActiveView(w);

  UpdateAfterTopologyChange(w->app);
}

