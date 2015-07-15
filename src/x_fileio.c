#include "x_dg.h"

#define DLG_APPUNSAVED "dlgAppUnsaved"
#define DLG_APP_UNSAVED "dlgAppUnsaved"
#define DLG_MESH_UNSAVED "dlgMeshUnsaved"
#define DLG_FILEOPEN "dlgFileOpen"
#define DLG_FILESAVE "dlgFileSave"
#define DLG_EQUIL "dlgEquil"
#define DLG_TEMPLATE "dlgTemplate"
#define DLG_PRINT "dlgPrint"
#define DLG_SONNET "dlgSonnet"
#define DLG_EXPORT_MESH "dlgExportMesh"
#define DLG_EXPORT_ELEMS_TEMPLATE "dlgExportElemsAsTemplate"

#define ENV_EXPORT_TEMPLATE_MASK "DG_EXPORT_TEMPLATE_MASK"
#define ENV_EXPORT_MESH_MASK "DG_EXPORT_MESH_MASK"

static void CbEquilLoad(Widget wg,View w,void* xtp);
static void CbTemplateLoad(Widget wg,View w,void* xtp);
static void CbSonnetLoad(Widget wg,View w,void* xtp);
static void CbFile_Open(Widget wg,View w,void* xtp);
static void CbFile_Save(Widget wg,View w,void* xtp);
static void CbFilePrintOk(Widget,XtPointer,XtPointer);
static void CbExportMesh(Widget,XtPointer,XtPointer);

Widget OpenEquilDlg(View w) {
  Widget wg;
  XmString xms,xms2;
  String s;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_EQUIL);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_EQUIL,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(GetStr(w,ENV_EQUILMASK2));
    if (s==NULL) s=getenv(GetStr(w,ENV_EQUILMASK));
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }
    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbEquilLoad,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);
  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }
  if (w->app->equil!=NULL)
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),
      w->app->equil->fName);

  SetViewFlags(w,w->showFlags | SHW_EQUIL);
  UndoMark(w->app);

  return wg;
}

Widget OpenTemplateDlg(View w) {
  Widget wg;
  XmString xms,xms2;
  String s;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_TEMPLATE);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_TEMPLATE,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(GetStr(w,ENV_TEMPLATEMASK2));
    if (s==NULL) s=getenv(GetStr(w,ENV_TEMPLATEMASK));
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbTemplateLoad,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);
  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  if (w->app->template!=NULL)
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),
      w->app->template->fName);

  SetViewFlags(w,w->showFlags | SHW_TEMPLATE);
  UndoMark(w->app);

  return wg;
}

Widget OpenSonnetDlg(View w) {
  Widget wg;
  String s;
  XmString xms,xms2;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_SONNET);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_SONNET,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(GetStr(w,ENV_SONNETMASK2));
    if (s==NULL) s=getenv(GetStr(w,ENV_SONNETMASK));
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbSonnetLoad,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);
  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  if (w->app->mesh!=NULL)
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),
      w->app->mesh->fileName);

  SetViewFlags(w,w->showFlags | SHW_MESH);
  UndoMark(w->app);

  return wg;
}

Widget OpenExportMeshDlg(View w,int bAtExit) {
  Widget wg;
  String s;
  XmString xms,xms2;

  if (w->app->mesh==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOSONNET));
    return NULL;
  }

  wg=XtNameToWidget(w->x->wMain,"*"DLG_EXPORT_MESH);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_EXPORT_MESH,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(ENV_EXPORT_MESH_MASK);
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbExportMesh,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);

    AddDependentWidget(w,XmFileSelectionBoxGetChild(wg,XmDIALOG_DEFAULT_BUTTON),
      N_NOW|N_ALT|N_NEWAPP,NULL,DwNotifyIfExists,(XtPointer)T_MESH);

  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  if (w->app->mesh!=NULL && w->app->mesh->fileName!=NULL) {
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),
      w->app->mesh->fileName);
    xms=MakeXmString(w->app->mesh->fileName);
    XmListSelectItem(XmFileSelectionBoxGetChild(wg,XmDIALOG_LIST),
      xms,False);
    XmStringFree(xms);
  }

  SetValues(wg,XmNuserData,(XtPointer)bAtExit,NULL);

  return wg;
}




Widget OpenFileOpenDlg(View w) {
  Widget wg;
  XmString xms,xms2;
  String s;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_FILEOPEN);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_FILEOPEN,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(GetStr(w,ENV_LOADMASK2));
    if (s==NULL) s=getenv(GetStr(w,ENV_LOADMASK));
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbFile_Open,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);
  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  return wg;
}

Widget OpenFileSaveDlg(View w,int bAtExit) {
  Widget wg;
  XmString xms,xms2;
  String s;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_FILESAVE);
  if (wg==NULL) {
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_FILESAVE,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(GetStr(w,ENV_SAVEMASK2));
    if (s==NULL) s=getenv(GetStr(w,ENV_SAVEMASK));
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbFile_Save,w);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);
  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  SetValues(wg,XmNuserData,(XtPointer)bAtExit,NULL);

  XmListDeselectAllItems(XmFileSelectionBoxGetChild(wg,XmDIALOG_LIST));
  if (w->app->fName!=NULL) {
    XmTextSetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT),
      w->app->fName);
    xms=MakeXmString(w->app->fName);
    XmListSelectItem(XmFileSelectionBoxGetChild(wg,XmDIALOG_LIST),
      xms,False);
    XmStringFree(xms);
  }

  return wg;
}

static void CbEquilLoad(Widget wg,View w,void* xtp) {
  String s;
  int i;

  SetActiveView(w);

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  i=AddEquil(w->app,s);
  if (i) {
    Cancel(w->app);
    ErrorBox(wg,GetStr(w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewFlags(w,w->showFlags | SHW_EQUIL);
    SetViewMsg(w,GetStr(w,MSG_EQUILLOADED));
    UndoMark(w->app);
  }
  XtFree(s);
}

static void CbTemplateLoad(Widget wg,View w,void* xtp) {
  String s;
  int i;

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  i=AddTemplate(w->app,s,0,0,0,1);
  if (i) {
    Cancel(w->app);
    ErrorBox(wg,GetStr(w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewFlags(w,w->showFlags | SHW_TEMPLATE);
    SetViewMsg(w,GetStr(w,MSG_TEMPLATELOADED));
    UndoMark(w->app);
  }
  XtFree(s);
}

static void CbSonnetLoad(Widget wg,View w,void* xtp) {
  String s;
  int i;

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  i=LoadMesh(w->app,s);
  if (i) {
    Cancel(w->app);
    ErrorBox(wg,GetStr(w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewFlags(w,w->showFlags | SHW_MESH);
    SetViewMsg(w,GetStr(w,MSG_SONNETLOADED));
    UndoMark(w->app);
  }
  XtFree(s);
}

static void CbExportMesh(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  String s;
  int i;

  SetActiveView(w);

  if (w->app->mesh==NULL) {
    ErrorBox(wg,GetStr(w,ERR_NOSONNET));
    return;
  }

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  i=SaveMesh(w->app->mesh,s);
  if (i) {
    Cancel(w->app);
    ErrorBox(wg,GetStr(w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewMsg(w,GetResourceString(wg,"msgMeshSaved",NULL,NULL));
    UndoMark(w->app);
    if ((int)GetUserData(wg)) CloseXmView(w,-1);
  }
  XtFree(s);

}

static void CbFile_Open(Widget wg,View w,void* xtp) {
  App a,a2;
  View w2;
  String s,s0;
  char* msg=NULL;
  int err;

  if (XmIsFileSelectionBox(wg)) {
    s0=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
    s=MallocString(s0);
    XtFree(s0);
  } else if (XmIsPushButton(wg))
    s=MallocString(GetUserData(wg));
  else assert(0);

  w2=FindViewByFilename(w->xapp,ExpandFilename(s));
  if (w2!=NULL) {
    Free(s);
    XtPopup(w2->x->wShell,XtGrabNone);
    XMapWindow(XtDisplay(w2->x->wShell),XtWindow(w2->x->wShell));
    SetViewMsg(w2,GetResourceString(w2->x->wMain,"msgFileAlreadyLoaded",
        NULL,NULL));
    return;
  }

  a=LoadNormalApp(w->xapp,s,&msg,&err);
  if (err) {
    ErrorBox(wg,GetStr(w,err));
  } else {
    if (XmIsFileSelectionBox(wg)) XtPopdown(XtParent(wg));

    if (w->app!=NULL && w->app->fName==NULL &&
        !IsAppUnsaved(w->app)) {
      a2=w->app;
      SetViewApp(w,a);
      CreateAutosaveInfo(w);
      if (IsEmptyGroup(a2->views)) FreeApp(a2);
      w2=w;
    }  else {
      w2=CreateXmView(w->xapp,a);
    }
    if (msg!=NULL) {
      MessageBox(w2->x->wMain,msg);
      msg=Free(msg);
    }
    SetViewMsg(w2,GetStr(w,MSG_FILEOPENED));
/*    ShowPicture(w2); */
/*    NotifyChange(a,w2); ??? - cannot figure out the purpose of this */
    FreeUndoInfo(a);
    AddXAppRecentFile(w2->xapp,a->fName);
  }

  Free(s);
}

static void CbFile_Save(Widget wg,View w,void* xtp) {
  String s;
  int err;
  XtPointer userData;

  GetValues(wg,XmNuserData,&userData,NULL);

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));

  if (DetectFileType(s)==FT_DG_CONFIG) {
    ErrorBox(wg,GetStr(w,ERR_NOSAVECONFIG));
    XtFree(s);
    return;
  }

  err=SaveApp(w->app,s,DGFM_APP);
  if (err) {
    ErrorBox(wg,GetStr(w,err));
  } else {
    XtPopdown(XtParent(wg));
    SetViewMsg(w,GetStr(w,MSG_FILESAVED));
    SetAppFName(w->app,s);
    UndoMark(w->app);
    w->app->alt=0;
    UndoMark(w->app);
    AddXAppRecentFile(w->xapp,w->app->fName);

    if ((int)userData) CloseXmView(w,-1);
  }
  XtFree(s);
}



static void CbAppUnsavedSave(Widget wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View)xtpView;

  XtPopdown(XtParent(wg));
  OpenFileSaveDlg(w,True);
}

static void CbAppUnsavedDiscard(Widget wg,XtPointer xtpView,XtPointer pcbs){
  View w=(View)xtpView;

  w->x->discardFlags |= DISCARD_DOC;

  XtPopdown(XtParent(XtParent(wg)));
  CloseXmView(w,-1);
}

static void OpenAppUnsavedDlg(View w) {
  Widget wDlg,wg;

  /* Pop up and deiconify the View window */

  XMapWindow(XtDisplay(w->x->wShell),XtWindow(w->x->wShell));
  XtPopup(w->x->wShell,XtGrabNone);

  /* Pop up the dialog box */

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_APP_UNSAVED);
  if (wDlg==NULL) {
    wDlg=Cw(XmCreateQuestionDialog,w->x->wMain,"dlgAppUnsaved",
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wDlg,XmNokCallback,CbAppUnsavedSave,w);
    XtAddCallback(wDlg,XmNcancelCallback,CbUnmap,NULL);
/*    XtUnmanageChild(XtNameToWidget(wDlg,"Help"));*/
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    wg=Cmw(XmCreatePushButton,wDlg,"discard",
      NULL);
    XtAddCallback(wg,XmNactivateCallback,CbAppUnsavedDiscard,w);
    XtManageChild(wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);
}


static void CbMeshUnsavedSave(Widget wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View)xtpView;

  XtPopdown(XtParent(wg));
  OpenExportMeshDlg(w,True);
}

static void CbMeshUnsavedDiscard(Widget wg,XtPointer xtpView,XtPointer pcbs){
  View w=(View)xtpView;

  XtPopdown(XtParent(XtParent(wg)));
  w->x->discardFlags |= DISCARD_MESH;

  CloseXmView(w,-1);
}

static void OpenMeshUnsavedDlg(View w) {
  Widget wDlg,wg;

  /* Pop up and deiconify the View window */

  XMapWindow(XtDisplay(w->x->wShell),XtWindow(w->x->wShell));
  XtPopup(w->x->wShell,XtGrabNone);

  /* Pop up the dialog box */

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_MESH_UNSAVED);
  if (wDlg==NULL) {
    wDlg=Cw(XmCreateQuestionDialog,w->x->wMain,DLG_MESH_UNSAVED,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      NULL);
    XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wDlg,XmNokCallback,CbMeshUnsavedSave,w);
    XtAddCallback(wDlg,XmNcancelCallback,CbUnmap,NULL);
/*    XtUnmanageChild(XtNameToWidget(wDlg,"Help"));*/
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    wg=Cmw(XmCreatePushButton,wDlg,"discard",
      NULL);
    XtAddCallback(wg,XmNactivateCallback,CbMeshUnsavedDiscard,w);
    XtManageChild(wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);
}


void CloseXmView(View w,int bAsk) {
  App a=w->app;
  XApp xap=w->xapp;

  if (bAsk!=-1) w->x->discardFlags=0;

  if (bAsk && GroupCount(a->views)==1) {
    if (a->mesh!=NULL && a->mesh->alt && ~w->x->discardFlags & DISCARD_MESH) {
      OpenMeshUnsavedDlg(w);
      return;
    }
    if (IsAppUnsaved(a) && ~w->x->discardFlags & DISCARD_DOC) {
      OpenAppUnsavedDlg(w);
      return;
    }
  }

  SetViewApp(w,NULL);
  FreeView(w);
  if (!GroupCount(a->views)) FreeApp(a);
}

/***********************************************************************
**                                                                    //
**  PrintDlg functions and callbacks                                 ///
**                                                                  ////
***********************************************************************/

#define CMT_CMD  1
#define CMT_FILE 2

typedef struct _FilePrintDlg {
  View w;
  Widget wDlg,wCmdType,wCommand,wWidth,wHeight,wMargin,wSwFit;
}* FilePrintDlg;

Widget OpenFilePrintDlg(View w) {
  FilePrintDlg dlg;
  Widget wDlg,wg;
  char s[DG_FNAME_LEN];

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_PRINT);
  if (wDlg!=NULL) {
/*    ResetFilePrintDlg(wDlg); */
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_PRINT);
    SetValues(wDlg,XmNuserData,(XtPointer)dlg,NULL);

    XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,dlg);
    XtAddCallback(dlg->wDlg,XmNokCallback,
      (XtCallbackProc)CbFilePrintOk,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    wg=Cmw(XmCreateForm,dlg->wDlg,"form",
      XmNautoUnmanage,False,
      NULL);

    CreateMenuSystem(wg,
      "*:cmdTypeMenu",
      "b@:file",CMT_FILE,
      "b@:cmd",CMT_CMD,
      "-:",
      "o@?:cmdType",0x0101,&dlg->wCmdType,
      "x@?:command",0x0201,&dlg->wCommand,
      "s@:separator",0x0002,
      "l@:widthLabel",0x0103,
      "x@?:width",0x0203,&dlg->wWidth,
      "l@:heightLabel",0x0104,
      "x@?:height",0x0204,&dlg->wHeight,
      "l@:marginLabel",0x0105,
      "x@?:margin",0x0205,&dlg->wMargin,
      "t@?:fit",0x0006,&dlg->wSwFit,
    NULL);

    Form2Table(wg);
    XtManageChild(dlg->wDlg);
    /*ResetFilePrintDlg(wDlg);*/
  }

  return wDlg;
}

static void CbFilePrintOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  FilePrintDlg dlg=(FilePrintDlg)xtpDlg;
  View w=dlg->w,psW;
  char buf[1024];
  char* s,*title=NULL;
  double width,height,margin;
  int r,fd[2];

  SetActiveView(dlg->w);

/* Parse parameters */

  s=XmTextGetString(dlg->wWidth);
  r=sscanf(s,SCANFLT"%s",&width,buf);
  XtFree(s);
  if (r!=1) {
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

  s=XmTextGetString(dlg->wHeight);
  r=sscanf(s,SCANFLT"%s",&height,buf);
  XtFree(s);
  if (r!=1) {
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

  s=XmTextGetString(dlg->wMargin);
  r=sscanf(s,SCANFLT"%s",&margin,buf);
  XtFree(s);
  if (r!=1) {
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }

/* Create a PostScript view */

  psW=CreatePsView(dlg->w->app,width*72,height*72,margin*72);
  if (psW==NULL) {
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_PSVIEWSIZE));
    return;
  }
  psW->bEditTopology=w->bEditTopology;

  if (XmToggleButtonGetState(dlg->wSwFit)) ShowPicture(psW);

/* Get filename/command */

  s=XmTextGetString(dlg->wCommand);

  switch((int)GetOptionMenuValue(dlg->wCmdType)) {
    case CMT_FILE:
      r=PsViewOutput(psW,s,-1,title);
      break;
    case CMT_CMD:
      if (pipe(fd)) {
        r=ERR_PIPECREATE;
        break;
      }

      switch(fork()) { /* Create a child process to receive PS data */
        case -1:       /* fork failed - error */
          close(fd[0]);
          close(fd[1]);
          r=ERR_FORK;
          goto break_outer;

        case 0:        /* We are child process now - run command */
          close(fd[1]);
          dup2(fd[0],0);
          system(s);
          while (getchar()!=EOF); /* Avoid 'broken pipe' errors */
          exit(0);     /* Exit when the command finishes */
      }

      close(fd[0]);    /* We are still the parent process */

      r=PsViewOutput(psW,NULL,fd[1],title);
      if (r) close(fd[1]);
  }

  if (r) {
    ErrorBox(w->x->wMain,GetStr(dlg->w,r));
  } else {
    XtPopdown(XtParent(dlg->wDlg));
    ViewMsgEx(w,MSG_FILEPRINTED,NULL);
  }

  break_outer:
  XtFree(s);
  SetViewApp(psW,NULL);
  FreeView(psW);
}

void DwNotifyRecentFiles(Widget wg,View w,int evt,void* obj,void* d){
  Widget wgC;
  String s,ls;
  Stack g;
  Index ix;

  g=GetRecentFilesStack(w->xapp);

  if (!IsEmptyGroup(g)) {
    CreateWidgetSystem(wg,
      "s-?:separ",&wgC,
      NULL);
    AddDependentWidget(w,wgC,N_RECENTFILES,NULL,DwDestroy,(void*)wgC);
  }

  for (s=Group1st(g,&ix);s!=NULL;s=Next(&ix)) {
    s=MallocString(s);

    wgC=Cmw(XmCreatePushButton,wg,"openRecentFile",
      XmNuserData,(XtPointer)s,
      NULL);
    XtAddCallback(wgC,XmNdestroyCallback,CbFree,(XtPointer)s);

    ls=GetShortFName(s);
    if (FindViewByFilename(w->xapp,s)!=NULL)
      ls=GetResourceStringEx(wgC,"labelStringRaise",NULL,
        "$(SHORTNAME)%s",ls);
    else ls=GetResourceStringEx(wgC,"labelStringOpen",NULL,
        "$(SHORTNAME)%s",ls);

    SetLabelString(wgC,ls);

    XtAddCallback(wgC,
      XmNarmCallback,CbDisplayDataLine,(XtPointer)w->x->wMsg);
    XtAddCallback(wgC,XmNdisarmCallback,
      (XtCallbackProc)CbClearHelpLine,(XtPointer)w->x->wMsg);

    XtAddCallback(wgC,XmNactivateCallback,
        (XtCallbackProc)CbFile_Open,(XtPointer)w);

    AddDependentWidget(w,wgC,N_RECENTFILES,NULL,DwDestroy,(void*)wgC);
  }
  FreeMallocedGroup(g);
}

/***********************************************************************
**                                                                    //
**  ExportElemsAsTemplateDlg functions and callbacks                 ///
**                                                                  ////
***********************************************************************/

typedef struct _EEAT_Dlg {
  View w;
  Widget wDlg,wOptMenu;
}* EEAT_Dlg;

#define EEAT_ALL    1
#define EEAT_MARKED 2

static int EEAT_OkCheck(EEAT_Dlg dlg);
static void CbExportElemsAsTemplate(Widget,XtPointer,XtPointer);
static Group EEAT_GetGroup(EEAT_Dlg dlg);

Widget OpenExportElemsAsTemplateDlg(View w) {
  Widget wg,wOptMenu;
  String s;
  XmString xms,xms2;
  EEAT_Dlg dlg;

  wg=XtNameToWidget(w->x->wMain,"*"DLG_EXPORT_ELEMS_TEMPLATE);
  if (wg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    wg=Cmw(XmCreateFileSelectionDialog,w->x->wMain,DLG_EXPORT_ELEMS_TEMPLATE,
      XmNautoUnmanage,False,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNuserData,(XtPointer)dlg,
      NULL);
    dlg->wDlg=wg;
    XtAddCallback(wg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XmAddWMProtocolCallback(XtParent(wg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);

    s=getenv(ENV_EXPORT_TEMPLATE_MASK);
    if (s!=NULL) {
      xms=XmStringCreateLocalized(s);
      SetValues(wg,XmNdirMask,xms,NULL);
      GetValues(wg,XmNdirMask,&xms2,NULL);
      if (!XmStringCompare(xms,xms2))
        ErrorBox(w->x->wMain,GetStr(w,ERR_BADMASK));
      XmStringFree(xms);
      XmStringFree(xms2);
    }

    XtAddCallback(wg,XmNokCallback,(XtCallbackProc)CbExportElemsAsTemplate,
        (XtPointer)dlg);
    XtAddCallback(wg,XmNcancelCallback,CbUnmap,NULL);

    CreateWidgetSystem(dlg->wDlg,
      "*:optMenu",
      "b@:all",(XtPointer)EEAT_ALL,
      "b@:marked",(XtPointer)EEAT_MARKED,
      "-:",
      "o?:whichElems",&dlg->wOptMenu,
      NULL);

    AddDependentWidget(w,XmFileSelectionBoxGetChild(wg,XmDIALOG_DEFAULT_BUTTON),
      N_NOW|N_ALT|N_NEWAPP,NULL,DwNotifyIfExists,(XtPointer)T_ELEM);

  } else {
    XmFileSelectionDoSearch(wg,NULL);
    XtPopup(XtParent(wg),XtGrabNone);
  }

  return wg;
}

static void CbExportElemsAsTemplate(Widget wg,XtPointer xtpD,XtPointer pcbs) {
  EEAT_Dlg dlg=(EEAT_Dlg)xtpD;
  String s;
  int i;

  SetActiveView(dlg->w);

  if (EEAT_OkCheck(dlg)) return;

  s=XmTextGetString(XmFileSelectionBoxGetChild(wg,XmDIALOG_TEXT));
  i=WriteElemsAsDgTemplate(EEAT_GetGroup(dlg),s);
  if (i) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,GetStr(dlg->w,i));
  } else {
    XtPopdown(XtParent(wg));
    SetViewMsg(dlg->w,GetResourceString(wg,"msgTemplateSaved",NULL,NULL));
    UndoMark(dlg->w->app);
  }
  XtFree(s);
}

static int EEAT_OkCheck(EEAT_Dlg dlg) {
  void* p;
  Index ix;
  Group g;

  if (IsEmptyGroup(EEAT_GetGroup(dlg))) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_NO_MARKED_ELEMS));
    return -1;
  }

  for (p=Group1st(EEAT_GetGroup(dlg),&ix);p!=NULL;p=Next(&ix))
    if (GetObjType(p)!=T_ELEM) {
      LabelObject(dlg->w,p,GetStr(dlg->w,STR_ERRLABEL),1);
      UndoMark(dlg->w->app);
      ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,ERR_BAD_OBJECT_TYPE));
      return -1;
    }

  return 0;
}


static Group EEAT_GetGroup(EEAT_Dlg dlg) {
  switch((int)GetOptionMenuValue(dlg->wOptMenu)) {
    case EEAT_ALL:
      return dlg->w->app->elems;
    case EEAT_MARKED:
      return dlg->w->app->mark;
    default:
      assert(0);
    return NULL; /* Make the compiler happy */
  }
}
