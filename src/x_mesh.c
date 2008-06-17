/* Mesh dialog boxes */

#include "x_dg.h"

#define DLG_EDIT_MESH_HEADER "dlgEditMeshHeader"

/***********************************************************************
**                                                                    //
**  "Mesh header Edit" dialog & callbacks                            ///
**                                                                  ////
***********************************************************************/

typedef struct _MeshHeaderDlg {
  View w;
  Mesh m;
  Widget wDlg,wOk,wText;
}* MeshHeaderDlg;

static void ResetMeshHeaderDlg(Widget wDlg);
static void DwMeshEdit(Widget wg,View w,int ev,void* obj,void* pDlg);
static void CbMeshHeaderDlgAccept(Widget wg,XtPointer arg,XtPointer pcbs);
static void CbMeshHeaderDlgInput(Widget wg,XtPointer arg,XtPointer pcbs);

Widget OpenEditMeshHeaderDlg(View w,Mesh m) {
  MeshHeaderDlg dlg=NULL;
  Widget wDlg;
  char s[DG_FNAME_LEN];

  assert(GetObjType(m)==T_MESH);

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_EDIT_MESH_HEADER);
  if (wDlg!=NULL) {
    ResetMeshHeaderDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->m=m;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_EDIT_MESH_HEADER);
    SetValues(dlg->wDlg,XmNuserData,(XtPointer)dlg,NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNokCallback,CbMeshHeaderDlgAccept,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    dlg->wOk=XmFileSelectionBoxGetChild(dlg->wDlg,XmDIALOG_DEFAULT_BUTTON);
    dlg->wText=Cmw(XmCreateScrolledText,dlg->wDlg,"text",
      XmNeditMode,XmMULTI_LINE_EDIT,
    NULL);
    XtAddCallback(dlg->wText,XmNvalueChangedCallback,CbMeshHeaderDlgInput,
        (XtPointer)dlg);

    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
	DwMeshEdit,dlg);

    ResetMeshHeaderDlg(wDlg);
    XtManageChild(wDlg);
  }
  return wDlg;
}

static void ResetMeshHeaderDlg(Widget wDlg) {
  MeshHeaderDlg dlg;
  XtPointer xtp;

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  dlg=(MeshHeaderDlg)xtp;

  assert(GetObjType(dlg->m)==T_MESH);

  XmTextSetString(dlg->wText,
      dlg->m->headerString!=NULL ? dlg->m->headerString : "");
  SetSensitiveEx(dlg->wOk,False);
}

static void DwMeshEdit(Widget wg,View w,int ev,void* obj,void* pDlg) {
  MeshHeaderDlg dlg=(MeshHeaderDlg)pDlg;

  switch(ev) {
    case N_DEL:
      if (obj!=dlg->m) return;
    case N_NEWAPP:
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
  }
}

static void CbMeshHeaderDlgAccept(Widget wg,XtPointer arg,XtPointer pcbs) {
  MeshHeaderDlg dlg=(MeshHeaderDlg)arg;
  char* s;
  int r;

  s=XmTextGetString(dlg->wText);
  r=CheckMeshHeaderString(dlg->m,s);
  if (r) {
    ErrorBox(dlg->w->x->wMain,GetStr(dlg->w,r));
  } else {
    SetMeshHeaderString(dlg->m,s);
    UndoMark(dlg->w->app);
    XtPopdown(XtParent(dlg->wDlg));
  }
  XtFree(s);
}

static void CbMeshHeaderDlgInput(Widget wg,XtPointer arg,XtPointer pcbs) {
  MeshHeaderDlg dlg=(MeshHeaderDlg)arg;

  SetSensitiveEx(dlg->wOk,True);
}
