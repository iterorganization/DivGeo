#include "x_dg.h"

#define DLG_TOOLBAR "dlgToolBox"

static void EhToolSelInput(Widget wg,ToolProc tp,XEvent* xev,Boolean* pbDispatch) {
  if (xev->type==ButtonPress || xev->type==ButtonRelease) {
    SetValues(XtParent(wg),XmNuserData,xev->xbutton.button-1,NULL);
    xev->xbutton.button=1;
  }
  *pbDispatch=True;
  return;
}

static void CbToolBarDlgSetTool(Widget wg,XtPointer pView,XtPointer pcbs) {
  View w=(View)pView;
  XtPointer xtp;
  ToolProc tp;

  GetValues(wg,XmNuserData,&xtp,NULL);tp=(ToolProc)xtp;
  GetValues(XtParent(wg),XmNuserData,&xtp,NULL);
  if (tp!=NULL) {
    w->x->tools[(int)xtp]=tp;
    NotifyNewTool(w);
  }
}

static void DwUpdateToolBox(Widget wToolBar,View w,int evt,void*obj,void*udt) {
  WidgetList wl;
  Cardinal cn;
  XtPointer xtp;
  int i,j;

  GetValues(wToolBar,
      XmNchildren,&wl,
      XmNnumChildren,&cn,
      NULL);
  for (i=0;i<cn;i++) if (XmIsLabel(wl[i])) {
    GetValues(wl[i],XmNuserData,&xtp,NULL);
    if (xtp==NULL) continue;
    for (j=0;j<3;j++) if ((ToolProc)xtp==w->x->tools[j]) break;
    SetValues(wl[i],XmNbackground,w->x->pxTool[j],NULL);
  }
}

static Widget CreateToolBarDlg(View w,Widget wParent) {
  Widget wDlg,wUndo,wRedo,wForm;
  WidgetList wl;
  Cardinal i,j;

  wDlg=Cw(XmCreateFormDialog,wParent,DLG_TOOLBAR,
    XmNautoUnmanage,False,
    XmNdeleteResponse,XmDO_NOTHING,
    XmNfractionBase,10000,
    NULL);

  XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
    CbUnmap,NULL);
  CreateWidgetSystem(wDlg,
    "#5?:subForm",&wForm,
     "b#A:close",1,1,CbUnmap,NULL,
     "b#A:menu",2,1,CbOptionsMenuToggle,w,
     "b#A?:undo",3,1,CbUndo,w,&wUndo,
     "b#!@A:move",1,2,EhToolSelInput,TlMoveObject,TlMoveObject,
        CbToolBarDlgSetTool,w,
     "b#!@A:remove",2,2,EhToolSelInput,TlRemoveObject,TlRemoveObject,
        CbToolBarDlgSetTool,w,
     "b#A?:redo",3,2,CbRedo,w,&wRedo,
     "b#!@A:examine",1,3,EhToolSelInput,TlExamine,TlExamine,
        CbToolBarDlgSetTool,w,
     "b#!@A:zoom",2,3,EhToolSelInput,TlZoom,TlZoom,
        CbToolBarDlgSetTool,w,
     "b#!@A:mark",3,3,EhToolSelInput,TlMark,TlMark,
        CbToolBarDlgSetTool,w,
     "b#!@A:addElem",1,4,EhToolSelInput,TlAddElem,TlAddElem,
        CbToolBarDlgSetTool,w,
     "b#!@A:addSrc",2,4,EhToolSelInput,TlAddSource,TlAddSource,
        CbToolBarDlgSetTool,w,
     "b#!@A:addChord",3,4,EhToolSelInput,TlAddChord,TlAddChord,
        CbToolBarDlgSetTool,w,
/*     "b#!@A:setXpt",EhToolSelInput,TlSetXPoint,TlSetXPoint,
        CbToolBarDlgSetTool,w, */
     "b#!@A:addSurf",2,5,EhToolSelInput,TlAddSurface,TlAddSurface,
        CbToolBarDlgSetTool,w,
     "b#!@A:addGP",3,5,EhToolSelInput,TlAddGridPoint,TlAddGridPoint,
        CbToolBarDlgSetTool,w,
     "b#!@A:splitElem",1,6,EhToolSelInput,TlSplitElem,TlSplitElem,
        CbToolBarDlgSetTool,w,
     "b#!@A:joinElem",2,6,EhToolSelInput,TlJoinElems,TlJoinElems,
        CbToolBarDlgSetTool,w,
     "b#!@A:ctPts",3,6,EhToolSelInput,TlConnectPoints,TlConnectPoints,
        CbToolBarDlgSetTool,w,
     "b#!@A:chElem",1,7,EhToolSelInput,TlRepositionElem,TlRepositionElem,
        CbToolBarDlgSetTool,w,
     "b#!@A:chNormals",2,7,EhToolSelInput,TlMirrorNormals,TlMirrorNormals,
        CbToolBarDlgSetTool,w,
     "b#!@A:moveMeshPt",3,7,EhToolSelInput,TlMoveMeshPoint,TlMoveMeshPoint,
        CbToolBarDlgSetTool,w,
    "-#:",
/*    "l:empty", */
    NULL);

  AddDependentWidget(w,wUndo,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyUndoButton,(XtPointer)0);
  AddDependentWidget(w,wRedo,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyUndoButton,(XtPointer)1);

/*  GetValues(wDlg,
    XmNchildren,&wl,
    XmNnumChildren,&j,
    NULL);
  if (j%3) FatalError("CreateToolBarDlg()-%dchildren: fatal error 1",j);
  for (i=0;i<j;i++) SetValues(wl[i],
    XmNtopAttachment,XmATTACH_POSITION,
    XmNtopPosition,(long)(i/3)*10000/(j/3),
    XmNbottomAttachment,XmATTACH_POSITION,
    XmNbottomPosition,(long)(i/3+1)*10000/(j/3),
    XmNleftAttachment,XmATTACH_POSITION,
    XmNleftPosition,(long)(i%3)*10000/3,
    XmNrightAttachment,XmATTACH_POSITION,
    XmNrightPosition,(long)(i%3+1)*10000/3,
    NULL); */

  AddDependentWidget(w,wForm,N_NOW | N_NEWTOOL,NULL,
      DwUpdateToolBox,NULL);

  return wDlg;
}

Widget OpenToolBarDlg(View w) {
  Widget wDlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_TOOLBAR);
  if (wDlg==NULL) wDlg=CreateToolBarDlg(w,w->x->wMain);
  else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

