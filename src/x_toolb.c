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
  Widget wDlg,wUndo,wRedo;
  WidgetList wl;
  Cardinal i,j;

  wDlg=Cw(XmCreateFormDialog,wParent,DLG_TOOLBAR,
    XmNautoUnmanage,False,
    XmNdeleteResponse,XmDO_NOTHING,
    XmNfractionBase,10000,
    NULL);

  XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
    CbUnmap,NULL);
  CreateMenuSystem(wDlg,
    "bA:close",CbUnmap,NULL,
    "bA:menu",CbOptionsMenuToggle,w,
    "bA?:undo",CbUndo,w,&wUndo,
    "b!@A:move",EhToolSelInput,TlMoveObject,TlMoveObject,
       CbToolBarDlgSetTool,w,
    "b!@A:remove",EhToolSelInput,TlRemoveObject,TlRemoveObject,
       CbToolBarDlgSetTool,w,
    "bA?:redo",CbRedo,w,&wRedo,
    "b!@A:examine",EhToolSelInput,TlExamine,TlExamine,
       CbToolBarDlgSetTool,w,
    "b!@A:zoom",EhToolSelInput,TlZoom,TlZoom,
       CbToolBarDlgSetTool,w,
    "b!@A:mark",EhToolSelInput,TlMark,TlMark,
       CbToolBarDlgSetTool,w,
    "b!@A:addElem",EhToolSelInput,TlAddElem,TlAddElem,
       CbToolBarDlgSetTool,w,
    "b!@A:addSrc",EhToolSelInput,TlAddSource,TlAddSource,
       CbToolBarDlgSetTool,w,
    "b!@A:addChord",EhToolSelInput,TlAddChord,TlAddChord,
       CbToolBarDlgSetTool,w,
    "b!@A:setXpt",EhToolSelInput,TlSetXPoint,TlSetXPoint,
       CbToolBarDlgSetTool,w,
    "b!@A:addSurf",EhToolSelInput,TlAddSurface,TlAddSurface,
       CbToolBarDlgSetTool,w,
    "b!@A:addGP",EhToolSelInput,TlAddGridPoint,TlAddGridPoint,
       CbToolBarDlgSetTool,w,
    "b!@A:splitElem",EhToolSelInput,TlSplitElem,TlSplitElem,
       CbToolBarDlgSetTool,w,
    "b!@A:joinElem",EhToolSelInput,TlJoinElems,TlJoinElems,
       CbToolBarDlgSetTool,w,
    "b!@A:ctPts",EhToolSelInput,TlConnectPoints,TlConnectPoints,
       CbToolBarDlgSetTool,w,
    "b!@A:chElem",EhToolSelInput,TlRepositionElem,TlRepositionElem,
       CbToolBarDlgSetTool,w,
    "b!@A:chNormals",EhToolSelInput,TlMirrorNormals,TlMirrorNormals,
       CbToolBarDlgSetTool,w,
    "b!@A:moveMeshPt",EhToolSelInput,TlMoveMeshPoint,TlMoveMeshPoint,
       CbToolBarDlgSetTool,w,
/*    "l:empty", */
    NULL);

  AddDependentWidget(w,wUndo,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyUndoButton,(XtPointer)0);
  AddDependentWidget(w,wRedo,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyUndoButton,(XtPointer)1);

  GetValues(wDlg,
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
    NULL);

  AddDependentWidget(w,wDlg,N_NOW | N_NEWTOOL,NULL,
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

