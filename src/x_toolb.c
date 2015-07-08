#include "x_dg.h"
#include <stdint.h>

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
    w->x->tools[(uintptr_t)xtp]=tp;
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

/* copy of x_view.c function */
static void AddDw2ShowFlags(Widget wg,XtPointer w,XtPointer value) {
  AddDependentWidget((View)w,wg,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyShowFlags,value);
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
     "b#!@A>:addElem",1,4,EhToolSelInput,TlAddElem,TlAddElem,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:addSrc",2,4,EhToolSelInput,TlAddSource,TlAddSource,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A:addChord",3,4,EhToolSelInput,TlAddChord,TlAddChord,
        CbToolBarDlgSetTool,w,
/*     "b#!@A:setXpt",EhToolSelInput,TlSetXPoint,TlSetXPoint,
        CbToolBarDlgSetTool,w, */
     "b#!@A:extChord",1,5,EhToolSelInput,TlExtChord,TlExtChord,
        CbToolBarDlgSetTool,w,
     "b#!@A>:addSurf",2,5,EhToolSelInput,TlAddSurface,TlAddSurface,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:addGP",3,5,EhToolSelInput,TlAddGridPoint,TlAddGridPoint,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:splitElem",1,6,EhToolSelInput,TlSplitElem,TlSplitElem,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:joinElem",2,6,EhToolSelInput,TlJoinElems,TlJoinElems,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:ctPts",3,6,EhToolSelInput,TlConnectPoints,TlConnectPoints,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A>:chElem",1,7,EhToolSelInput,TlRepositionElem,TlRepositionElem,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A:chNormals",2,7,EhToolSelInput,TlMirrorNormals,TlMirrorNormals,
        CbToolBarDlgSetTool,w,
     "b#!@A>:moveMeshPt",3,7,EhToolSelInput,TlMoveMeshPoint,TlMoveMeshPoint,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,
     "b#!@A:rotate",1,8,EhToolSelInput,TlRotate,TlRotate,
        CbToolBarDlgSetTool,w,
     "b#!@A>:stretch",2,8,EhToolSelInput,TlStretch,TlStretch,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)0,
     "b#!@A>:adjChord",3,8,EhToolSelInput,TlAdjustChord,TlAdjustChord,
        CbToolBarDlgSetTool,w,AddDw2ShowFlags,w,(XtPointer)1,

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

