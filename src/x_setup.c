#include "x_dg.h"

#define DLG_SETUP "dlgSetup"
#define DLG_VARSETDEFLIST "dlgVarSetDefList"
#define DLG_VARSETDEFCREATE "dlgVarSetDefCreate"
#define DLG_VARSETDEFEDIT "dlgVarSetDefEdit"
#define DLG_VARDEFCREATE "dlgVarDefCreate"
#define DLG_VARDEFEDIT "dlgVarDefEdit"
#define DLG_SAVECONFIG "dlgSaveConfig"
#define DLG_SETUPWARNING "dlgSetupWarning"
#define DLG_HELPEDIT "dlgHelpEdit"
#define DLG_OUTPUTMODE "dlgOutputMode"
#define DLG_MESHOPTIONS "dlgMeshOptions"

typedef struct _VarSetDefListDlg {
  View w;
  Widget wDlg,wList;
  VarSetDef vsdSelect;
}* VarSetDefListDlg;

typedef struct _VarSetDefCreateDlg {
  View w;
  Widget wDlg,wName;
}* VarSetDefCreateDlg;

typedef struct _VarSetDefEditDlg {
  View w;
  VarSetDef vsd;
  Widget wDlg,wDescr,wMin,wMax,wVarDefs,wVarFrame;
}* VarSetDefEditDlg;

typedef struct _VarSetDefEditField {
  VarSetDefEditDlg dlg;
  Widget wBn;
  int col,row;
  VarDef vd; /* NULL = empty */
}* VarSetDefEditField;

typedef struct _VarDefCreateDlg {
  View w;
  Widget wDlg,wName;
  VarSetDef vsd;
  int row,col;
}* VarDefCreateDlg;

typedef struct _VarDefEditDlg {
  View w;
  VarDef vd;
  Widget wDlg,wName,wDescr,wType,wSwNoExport,wOrg,wDefault,wDefaultLabel,
    wOrgVarSet,wOrgElems,wOrgSources,wOrgChords,wSwLayerIndex;
}* VarDefEditDlg;

typedef struct _HelpEditDlg {
  View w;
  VarDef vd;
  Widget wDlg,wText;
}* HelpEditDlg;

static Widget OpenSetupWarningDlg(View w,XtCallbackProc cbp);
static void CbSetupWarningOk(Widget,XtPointer xtpV,XtPointer pcbs);

static Widget OpenVarSetDefListDlg(View w);
static void DwVarSetDefList(Widget wg,View w,int ev,void* obj,void* pDlg);
static void CbVarSetDefListAdd(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarSetDefListModify(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarSetDefListRemove(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

static Widget OpenVarSetDefCreateDlg(View w);
static void CbVarSetDefCreateOk(Widget,XtPointer xtpDlg,XtPointer pcbs);

static Widget OpenVarSetDefEditDlg(View w,VarSetDef vsd);
static void DwVarSetDefEdit(Widget wg,View w,int ev,void* obj,void* pDlg);
static void ResetVarSetDefEditDlg(Widget wDlg);
static void CbVarSetDefEditOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbEditVarDef(Widget wg,XtPointer xtpVdf,XtPointer pcbs);
static void CbVarSetDefEditDefs(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarDefEditSensitive(Widget wg,XtPointer xtpDlg,XtPointer p);

static Widget OpenVarDefCreateDlg(View w,VarSetDef vsd,int row,int col);
static void DwVarDefCreate(Widget wg,View w,int ev,void* obj,void* pDlg);
static void CbVarDefCreateOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

static Widget OpenVarDefEditDlg(View w,VarDef vd);
static void DwVarDefEdit(Widget wg,View w,int ev,void* obj,void* pDlg);
static void ResetVarDefEditDlg(Widget wDlg);
static void CbVarDefEditOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarDefEditRemove(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarDefEditHelpStr(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarDefEditToggle(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

static Widget OpenHelpEditDlg(View w,VarDef vd);
static void ResetHelpEditDlg(Widget wDlg);
static void DwHelpEdit(Widget wg,View w,int ev,void* obj,void* pDlg);
static void CbHelpEditDlgAccept(Widget wg,XtPointer arg,XtPointer pcbs);

static Widget OpenSaveConfigDlg(View w);
static void CbSaveConfigDlgOk(Widget wDlg,XtPointer xtpV,XtPointer pcbs);

/*
static VarDefEditDlg CreateVarDefEditDlg(VarSetDefEditField vdf,
    Widget wParent);
static void UpdateVarSetDefEditDlg(VarSetDefEditDlg dlg);
*/

/***********************************************************************
**                                                                    //
**  Menu callbacks                                                   ///
**                                                                  ////
***********************************************************************/

void CbEditVarSetDefs(Widget wg,XtPointer pView,void* pcbs) {
  View w=(View)pView;
  Widget wDlg;

  if (OpenSetupWarningDlg(w,CbEditVarSetDefs)!=NULL) return;

  wDlg=OpenVarSetDefListDlg(w);
}

void CbSaveConfig(Widget wg,XtPointer pView,XtPointer pcbs) {
  View w=(View)pView;

  if (OpenSetupWarningDlg(w,CbSaveConfig)!=NULL) return;

  OpenSaveConfigDlg(w);
}

/***********************************************************************
**                                                                    //
**  "Layer Types"" dialog box & callbacks                            ///
**                                                                  ////
***********************************************************************/

static Widget OpenVarSetDefListDlg(View w) {
  VarSetDefListDlg dlg;
  Widget wDlg,wg,wAdd,wModify,wClose,wRemove;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_VARSETDEFLIST);
  if (wDlg!=NULL) {
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;

    wDlg=dlg->wDlg=CreateMessageDialog(w->x->wMain,DLG_VARSETDEFLIST,
      "b?:add",&wAdd,
      "b?\n:modify",&wModify,
      "b?:remove",&wRemove,
      "b?\b:cancel",&wClose,
/*      "bA:help",CbHelp,(XtPointer)w, */
      NULL);

    SetValues(wDlg,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNautoUnmanage,False,
      XmNuserData,(XtPointer)dlg,
    NULL);

    SetValues(XtParent(wDlg),
      XmNdeleteResponse,XmDO_NOTHING,
    NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,dlg);
    XtAddCallback(wAdd,XmNactivateCallback,
      (XtCallbackProc)CbVarSetDefListAdd,dlg);
    XtAddCallback(wModify,XmNactivateCallback,
      (XtCallbackProc)CbVarSetDefListModify,dlg);
    XtAddCallback(wRemove,XmNactivateCallback,
      (XtCallbackProc)CbVarSetDefListRemove,dlg);
    XtAddCallback(wClose,XmNactivateCallback,CbUnmap,NULL);
    XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wDlg,XmNcancelCallback,CbUnmap,NULL);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    dlg->wList=Cmw(XmCreateScrolledList,dlg->wDlg,"list",
      XmNselectionPolicy,XmBROWSE_SELECT,
      NULL);
    XtAddCallback(dlg->wList,XmNdefaultActionCallback,
      (XtCallbackProc)CbVarSetDefListModify,dlg);
    dlg->vsdSelect=NULL;

    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_NEWAPP,NULL,
        DwVarSetDefList,dlg);

    XtManageChild(dlg->wDlg);
  }

  return wDlg;
}

static void CbVarSetDefListAdd(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  VarSetDefListDlg dlg=(VarSetDefListDlg)xDlg;

  OpenVarSetDefCreateDlg(dlg->w);
}

static void CbVarSetDefListModify(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  VarSetDefListDlg dlg=(VarSetDefListDlg)xDlg;
  VarSetDef vsd=NULL;
  int* pListPos;
  int listCount;

  if (!XmListGetSelectedPos(dlg->wList,&pListPos,&listCount)) return;
  if (listCount==1 && *pListPos>=1 &&
      *pListPos<=GroupCount(dlg->w->app->varSetDefs))
    vsd=GroupAt(dlg->w->app->varSetDefs,*pListPos-1);
  XtFree((XtPointer)pListPos);
  if (vsd==NULL) return;

  if (IsLocked(vsd)) {
    ErrorBox(dlg->wDlg,WhyLocked(dlg->w,vsd));
    return;
  }

  OpenVarSetDefEditDlg(dlg->w,vsd);
}

static void CbVarSetDefListRemove(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  VarSetDefListDlg dlg=(VarSetDefListDlg)xDlg;
  VarSetDef vsd=NULL;
  int* pListPos;
  int listCount;

  if (!XmListGetSelectedPos(dlg->wList,&pListPos,&listCount)) return;
  if (listCount==1) vsd=GroupAt(dlg->w->app->varSetDefs,*pListPos-1);
  XtFree((XtPointer)pListPos);
  if (vsd==NULL) return;

  if (IsLocked(vsd)) {
    ErrorBox(dlg->wDlg,WhyLocked(dlg->w,vsd));
    return;
  }
  DelVarSetDef(dlg->w->app,vsd);
  UndoMark(dlg->w->app);
}

static void DwVarSetDefList(Widget wg,View w,int ev,void* obj,void* pDlg) {
  VarSetDefListDlg dlg=pDlg;
  int i=0;
  XmString xms;
  VarSetDef vsd;
  Index ix;

  if (ev==N_ALT && ~w->x->changes & CHF_VARDEFS) return;

  XmListDeleteAllItems(dlg->wList);
  for (vsd=AppVarSetDef1st(dlg->w->app,&ix);vsd!=NULL;vsd=Next(&ix)) {
    xms=MakeXmString(vsd->name);
    XmListAddItem(dlg->wList,xms,++i);
    XmStringFree(xms);
  }
  XmListSelectPos(dlg->wList,1,True);
}

/***********************************************************************
**                                                                    //
**  "Add layer type..." dialog box & callbacks                       ///
**                                                                  ////
***********************************************************************/

static Widget OpenVarSetDefCreateDlg(View w) {
  Widget wDlg,wg;
  VarSetDefCreateDlg dlg;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_VARSETDEFCREATE);
  if (wDlg!=NULL) {
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_VARSETDEFCREATE);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNokCallback,CbVarSetDefCreateOk,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    wg=Cmw(XmCreateForm,wDlg,"form",
      NULL);
    CreateMenuSystem(wg,
      "l@:nameLabel",0x0101,
      "x?@:name",&dlg->wName,0x0201,
    NULL);
    Form2Table(wg);
    XtManageChild(wDlg);
  }
  return wDlg;
}

static void CbVarSetDefCreateOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarSetDefCreateDlg dlg=(VarSetDefCreateDlg)xtpDlg;
  char name[DG_FNAME_LEN],* s;
  VarSetDef vsd;
  Index ix;

  s=XmTextGetString(dlg->wName);
  if (sscanf(s,"%s%s",name,name)!=1) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_BADNAME));
    XtFree(s);
    return;
  }
  XtFree(s);

  for (vsd=AppVarSetDef1st(dlg->w->app,&ix);vsd!=NULL;vsd=Next(&ix)) {
    if (!strcmp(vsd->name,name)) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_DUPNAME));
      return;
    }
  }

  AddVarSetDef(dlg->w->app,name,"CLASSIFIED",0,1);
  UndoMark(dlg->w->app);
  XtPopdown(XtParent(dlg->wDlg));
}

/***********************************************************************
**                                                                    //
**  "Layer type..." dialog box & callbacks                           ///
**                                                                  ////
***********************************************************************/

static Widget OpenVarSetDefEditDlg(View w,VarSetDef vsd) {
  VarSetDefEditDlg dlg;
  Widget wDlg,wg,wg1;
  char s[DG_FNAME_LEN];

  sprintf(s,"*%s_%s",DLG_VARSETDEFEDIT,vsd->name);
  wDlg=XtNameToWidget(w->x->wMain,s);
  if (wDlg!=NULL) {
    wDlg=XtParent(wDlg);
    ResetVarSetDefEditDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->vsd=vsd;

    sprintf(s,"%s_%s",DLG_VARSETDEFEDIT,vsd->name);

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_VARSETDEFEDIT);
    SetValues(dlg->wDlg,XmNuserData,dlg,NULL);

    XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,dlg);
    XtAddCallback(dlg->wDlg,XmNokCallback,
      (XtCallbackProc)CbVarSetDefEditOk,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
/*
    XtAddCallback(dlg->wDlg,XmNhelpCallback,
      (XtCallbackProc)CbVarSetDefEditDefs,dlg);
*/

    wg=Cmw(XmCreateForm,dlg->wDlg,s,
      XmNautoUnmanage,False,
      NULL);

    wg1=Cmw(XmCreateForm,wg,"form2",
      XmNleftAttachment,XmATTACH_FORM,
      XmNrightAttachment,XmATTACH_FORM,
      XmNtopAttachment,XmATTACH_FORM,
      NULL);

    CreateMenuSystem(wg1,
      "l@:descrLabel",0x0101,
      "x?@:descr",&dlg->wDescr,0x0201,
      "l@:minLabel",0x0102,
      "x?@:min",&dlg->wMin,0x0202,
      "l@:maxLabel",0x0103,
      "x?@:max",&dlg->wMax,0x0203,
    NULL);
    Form2Table(wg1);

    wg1=Cmw(XmCreateForm,wg,"outerForm",
      XmNautoUnmanage,False,
      XmNresizePolicy,XmRESIZE_GROW,

      XmNleftAttachment,XmATTACH_FORM,
      XmNrightAttachment,XmATTACH_FORM,
      XmNtopAttachment,XmATTACH_WIDGET,
      XmNtopWidget,wg1,
      XmNbottomAttachment,XmATTACH_FORM,
      NULL);

    dlg->wVarFrame=Cmw(XmCreateFrame,wg1,"frame",
      XmNleftAttachment,XmATTACH_FORM,
      XmNrightAttachment,XmATTACH_FORM,
      XmNtopAttachment,XmATTACH_FORM,
      XmNbottomAttachment,XmATTACH_FORM,
      NULL);

    dlg->wVarDefs=NULL;

    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
        DwVarSetDefEdit,dlg);
    ResetVarSetDefEditDlg(wDlg);
    XtManageChild(dlg->wDlg);
  }

  return wDlg;
}

static void ResetVarSetDefEditDlg(Widget wDlg) {
  char s[DG_FNAME_LEN];
  VarSetDefEditDlg dlg;
  XtPointer xtp;

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  dlg=(VarSetDefEditDlg)xtp;

  SetValues(XtParent(wDlg),XmNtitle,
    GetResourceStringEx(wDlg,"dialogTitleEx","DialogTitle",
      "$(NAME)%s$(DESCR)%s",dlg->vsd->name,dlg->vsd->descr),
  NULL);

  XmTextSetString(dlg->wDescr,dlg->vsd->descr);
  sprintf(s,"%d",dlg->vsd->minVarSets);
  XmTextSetString(dlg->wMin,s);
  sprintf(s,"%d",dlg->vsd->maxVarSets);
  XmTextSetString(dlg->wMax,s);
}

static void DwVarSetDefEdit(Widget wg,View w,int ev,void* obj,void* pDlg) {
  VarSetDefEditDlg dlg=(VarSetDefEditDlg)pDlg;
  int i,j,rows,cols;
  VarDef vd;
  Index ix;
  VarSetDefEditField vdf;
  XmString xms;

  if (dlg->vsd==NULL) return;

  switch(ev) {
    case N_DEL:
      if (obj!=dlg->vsd) return;
    case N_NEWAPP:
      dlg->vsd=NULL;
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
    case N_ALT:
      if (~dlg->w->x->changes & CHF_VARDEFS) return;
  }

  for (rows=cols=0,vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;
      vd=Next(&ix)) {
    cols=max(cols,vd->col+1);
    rows=max(rows,vd->row+1);
  }
  cols++;rows++;
  cols=max(cols,3);
  rows=max(rows,3);
  if (dlg->wVarDefs!=NULL) {
    /*    XtDestroyWidget(dlg->wVarDefs); */
    dlg->wVarDefs=NULL;
  }
  dlg->wVarDefs=Cmw(XmCreateForm,dlg->wVarFrame,"varDefs",
    XmNfractionBase,10000,
    NULL);
  for (i=0;i<rows;i++) for (j=0;j<cols;j++) {
    for (vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix))
      if (vd->row==i && vd->col==j) break;
    vdf=Malloc(sizeof(*vdf));
    vdf->dlg=dlg;
    vdf->vd=vd;
    vdf->col=j;
    vdf->row=i;
    vdf->wBn=Cmw(XmCreatePushButton,dlg->wVarDefs,"varDef",
      XmNleftAttachment,XmATTACH_POSITION,
      XmNleftPosition,j*10000/cols,
      XmNrightAttachment,XmATTACH_POSITION,
      XmNrightPosition,(j+1)*10000/cols,
      XmNtopAttachment,XmATTACH_POSITION,
      XmNtopPosition,i*10000/rows,
      XmNbottomAttachment,XmATTACH_POSITION,
      XmNbottomPosition,(i+1)*10000/rows,
      NULL);
    if (vd!=NULL) {
      xms=MakeXmString(vd->name);
      SetValues(vdf->wBn,XmNlabelString,xms,NULL);
      XmStringFree(xms);
    }
    XtAddCallback(vdf->wBn,XmNdestroyCallback,CbFree,vdf);
    XtAddCallback(vdf->wBn,XmNactivateCallback,
      (XtCallbackProc)CbEditVarDef,vdf);
  }
}

static void CbVarSetDefEditOk(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  VarSetDefEditDlg dlg=(VarSetDefEditDlg)xDlg;
  int vsMin,vsMax;
  char* s;
  VarSetDef vsd;
  VarDef vd;
  Index ix;
  int i;

  s=XmTextGetString(dlg->wMin);
  if (sscanf(s,"%d",&vsMin)!=1) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    XtFree(s);
    return;
  }
  XtFree(s);
  s=XmTextGetString(dlg->wMax);
  if (sscanf(s,"%d",&vsMax)!=1) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
      XtFree(s);
      return;
  }
  XtFree(s);

  if (vsMin>vsMax) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_INVNUMBERS));
    return;
  }
  i=GroupCount(dlg->vsd->varSets);
  if (i<vsMin || i>vsMax) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_MINMAXVARSETS));
    return;
  }

  if (vsMax>1) {
    for (vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix))
      if (vd->varType & VTF_SINGLE) {
        ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_SINGLE));
        return;
      }
  }

  s=XmTextGetString(dlg->wDescr);
  ChangeVarSetDef(dlg->w->app,dlg->vsd,dlg->vsd->name,*s ? s : "CLASSIFIED",
    vsMin,vsMax,NULL);
  XtFree(s);
  UndoMark(dlg->w->app);
  XtPopdown(XtParent(dlg->wDlg));
}

/*
static void CbVarSetDefEditDefs(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  VarSetDefEditDlg dlg=(VarSetDefEditDlg)xDlg;
  Widget wDlg;

  wDlg=OpenVarsEditDlg(dlg->w,dlg->vsd,dlg->wDlg,True);
  ProcessModalDialog(wDlg);
  UndoMark(dlg->w->app);
}
*/

static void CbEditVarDef(Widget wg,XtPointer xVdf,XtPointer pcbs) {
  VarSetDefEditField vdf=(VarSetDefEditField)xVdf;

  if (vdf->vd==NULL)
    OpenVarDefCreateDlg(vdf->dlg->w,vdf->dlg->vsd,vdf->row,vdf->col);
  else OpenVarDefEditDlg(vdf->dlg->w,vdf->vd);
}

/***********************************************************************
**                                                                    //
**  "Variable options" dialog box & callbacks                        ///
**                                                                  ////
***********************************************************************/

static Widget OpenVarDefEditDlg(View w,VarDef vd) {
  char s[DG_FNAME_LEN];
  Widget wDlg;
  VarDefEditDlg dlg;
  Widget wg;
  XtPointer xtp;

  sprintf(s,"*"DLG_VARDEFEDIT"_%p",vd);
  wDlg=XtNameToWidget(w->x->wMain,s);
  if (wDlg!=NULL) {
    wDlg=XtParent(wDlg);
    ResetVarDefEditDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->vd=vd;

    wDlg=dlg->wDlg=CreateMessageDialog(w->x->wMain,DLG_VARDEFEDIT,
      "bA\n:ok",CbVarDefEditOk,dlg,
      "bA\b:cancel",CbUnmap,NULL,
      "bA:editHelp",CbVarDefEditHelpStr,dlg,
      "bA:remove",CbVarDefEditRemove,dlg,
      NULL);
    SetValues(wDlg,XmNuserData,(XtPointer)dlg,NULL);

    XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbFree,dlg);

    sprintf(s,DLG_VARDEFEDIT"_%p",vd);
    wg=Cmw(XmCreateForm,dlg->wDlg,s,
      NULL);

    CreateMenuSystem(wg,
      "l@:descrLabel",0x0101,
      "x?@:descr",&dlg->wDescr,0x0201,
      "l@:typeLabel",0x0102,
      "*:typeMenu",
      "b@A:int",       (XtPointer)VT_INT,  CbVarDefEditSensitive,dlg,
      "b@A:float",     (XtPointer)VT_FLOAT,CbVarDefEditSensitive,dlg,
      "b@A:text",      (XtPointer)VT_TEXT, CbVarDefEditSensitive,dlg,
      "b@A:filename",  (XtPointer)VT_FILENAME,CbVarDefEditSensitive,dlg,
      "s:separator",
      "b@A:elem",      (XtPointer)VT_ELEM, CbVarDefEditSensitive,dlg,
      "b@A:elems",     (XtPointer)VT_ELEMS,CbVarDefEditSensitive,dlg,
      "b@A:target1",   (XtPointer)VT_TARGET1,CbVarDefEditSensitive,dlg,
      "b@A:target2",   (XtPointer)VT_TARGET2,   CbVarDefEditSensitive,dlg,
      "b@A:structure", (XtPointer)VT_STRUCTURE, CbVarDefEditSensitive,dlg,
      "b@A:structPart",(XtPointer)VT_STRUCTPART,CbVarDefEditSensitive,dlg,
      "b@A:chords",    (XtPointer)VT_CHORDS,CbVarDefEditSensitive,dlg,
      "b@A:topView",   (XtPointer)VT_TOPVIEW,CbVarDefEditSensitive,dlg,
      "s:separator",
      "b@A:meshCells", (XtPointer)VT_MESH_CELLS,CbVarDefEditSensitive,dlg,
      "b@A:meshElems", (XtPointer)VT_MESH_ELEMENTS,CbVarDefEditSensitive,dlg,
      "b@A:meshHElems",(XtPointer)VT_MESH_H_ELEMENTS,CbVarDefEditSensitive,dlg,
      "b@A:meshVElems",(XtPointer)VT_MESH_V_ELEMENTS,CbVarDefEditSensitive,dlg,
      "-:",
      "o?@:type",&dlg->wType,0x0202,
      "l@:orgLabel",0x0103,
      "*:orgMenu",
      "b?:withVarSet",&dlg->wOrgVarSet,
      "b?:withElems",&dlg->wOrgElems,
      "b?:withSources",&dlg->wOrgSources,
      "b?:withChords",&dlg->wOrgChords,
      "-:",
      "o?@:origin",&dlg->wOrg,0x0203,
      "l?@:defLabel",&dlg->wDefaultLabel,0x0104,
      "x?@:def",&dlg->wDefault,0x0204,
      "t?@:noExport",&dlg->wSwNoExport,0x0205,
      "t?@:layerIndex",&dlg->wSwLayerIndex,0x0206,
    NULL);

    ResetVarDefEditDlg(wDlg);
    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
        DwVarDefEdit,dlg);
    Form2Table(wg);
    XtManageChild(dlg->wDlg);
  }
  return wDlg;
}

static void ResetVarDefEditDlg(Widget wDlg) {
  char s[DG_FNAME_LEN];
  VarDefEditDlg dlg;
  Widget wg;
  XtPointer xtp;

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  dlg=(VarDefEditDlg)xtp;

  SetValues(XtParent(wDlg),XmNtitle,
    GetResourceStringEx(wDlg,"dialogTitleEx","DialogTitle",
      "$(NAME)%s$(DESCR)%s",dlg->vd->name,dlg->vd->descr),
  NULL);

  XmTextSetString(dlg->wDescr,dlg->vd->descr);
  SetOptionMenuValue(dlg->wType,(XtPointer)dlg->vd->varType);

  switch(dlg->vd->flags & VFM_MULTIPLE) {
    case VF_FORELEMS: wg=dlg->wOrgElems;break;
    case VF_FORSOURCES: wg=dlg->wOrgSources;break;
    case VF_FORCHORDS: wg=dlg->wOrgChords;break;
    default: wg=dlg->wOrgVarSet;break;
  }
  SetValues(dlg->wOrg,XmNmenuHistory,wg,NULL);

  XmToggleButtonSetState(dlg->wSwNoExport,
    !!(dlg->vd->flags & VF_NOEXPORT),False);

  XmToggleButtonSetState(dlg->wSwLayerIndex,
    !!(dlg->vd->flags & VF_LAYERINDEX),False);

  XmTextSetString(dlg->wDefault,dlg->vd->varType & VTM_HASGROUP? "" :
    GetVar(dlg->vd->varSetDef,dlg->vd,NULL));

  CbVarDefEditSensitive(NULL,(XtPointer)dlg,NULL);
}

static void DwVarDefEdit(Widget wg,View w,int ev,void* obj,void* pDlg) {
  VarDefEditDlg dlg=(VarDefEditDlg)pDlg;
  int i,j,rows,cols;
  VarDef vd;
  Index ix;
  VarSetDefEditField vdf;
  XmString xms;

  if (dlg->vd==NULL) return;

  switch(ev) {
    case N_DEL:
      if (obj!=dlg->vd) return;
    case N_NEWAPP:
      dlg->vd=NULL;
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
    case N_ALT:
      if (~dlg->w->x->changes & CHF_VARDEFS) return;
  }
}

static void CbVarDefEditOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarDefEditDlg dlg=(VarDefEditDlg)xtpDlg;
  char* descr,*s;
  char s1[DG_FNAME_LEN],s2[DG_FNAME_LEN];
  VarDef vd;
  Index ix;
  int i,type,opt,err;

  type=(int)GetOptionMenuValue(dlg->wType);
  opt=0;
  GetValues(dlg->wOrg,XmNmenuHistory,&wg,NULL);
  if (wg==dlg->wOrgElems) opt|=VF_FORELEMS;
  if (wg==dlg->wOrgSources) opt|=VF_FORSOURCES;
  if (wg==dlg->wOrgChords) opt|=VF_FORCHORDS;

  if (XmToggleButtonGetState(dlg->wSwNoExport)) opt|=VF_NOEXPORT;
  if (XmToggleButtonGetState(dlg->wSwLayerIndex)) opt|=VF_LAYERINDEX;
  if (opt & VFM_MULTIPLE && type & VTM_HASGROUP) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_MULTGROUP));
    return;
  }

  if (type & VTF_SINGLE) {
    if (opt & VFM_MULTIPLE) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_SINGLE));
      return;
    }
    for (vd=AppVarDef1st(dlg->w->app,&ix);vd!=NULL;vd=Next(&ix)) {
      if (vd!=dlg->vd && vd->varType==type) {
        ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_SINGLE));
        return;
      }
    }
  }

  /* Additional checks if VF_LAYERINDEX set */

  if (opt & VF_LAYERINDEX) {
    if (opt & VFM_MULTIPLE) {
      ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errIndexMultiple",
          NULL,NULL));
      return;
    }
    if (type & VTM_HASGROUP) {
      ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errIndexGroup",
          NULL,NULL));
      return;
    }
    for (vd=Group1st(dlg->vd->varSetDef->varDefs,&ix);vd!=NULL;
        vd=Next(&ix)) {
      if (vd==dlg->vd) continue;
      if (vd->flags & VF_LAYERINDEX) {
        ErrorBox(dlg->wDlg,GetResourceStringEx(dlg->wDlg,"errIndex2nd",
            NULL,"$(NAME)%s$(DESCR)%s",vd->name,vd->descr));
        return;
      }
    }
  }

  descr=XmTextGetString(dlg->wDescr);

  err=ChangeVarDef(dlg->w->app,dlg->vd,dlg->vd->varSetDef,dlg->vd->name,
    *descr ? descr : "CLASSIFIED",type,opt,
    dlg->vd->col,dlg->vd->row);
  XtFree(descr);

  if (err && err!=STR_VARSLOST) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,GetStr(dlg->w,err));
    XtFree(descr);
    return;
  }

  if (!(type & VTM_HASGROUP)) {
    s=XmTextGetString(dlg->wDefault);
    err=SetVar(dlg->w->app,dlg->vd->varSetDef,dlg->vd,NULL,s);
    XtFree(s);

    if (err) {
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,GetStr(dlg->w,err));
      return;
    }
  }

  UndoMark(dlg->w->app);
  XtPopdown(XtParent(dlg->wDlg));
}

static void CbVarDefEditRemove(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarDefEditDlg dlg=(VarDefEditDlg)xtpDlg;

  DelVarDef(dlg->w->app,dlg->vd);
  UndoMark(dlg->w->app);
}

static void CbVarDefEditHelpStr(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarDefEditDlg dlg=(VarDefEditDlg)xtpDlg;
  Widget wHDlg;

  if (dlg->vd==NULL) return;
  wHDlg=OpenHelpEditDlg(dlg->w,dlg->vd);
}

static void CbVarDefEditSensitive(Widget wg,XtPointer xtpDlg,XtPointer p) {
  VarDefEditDlg dlg=(VarDefEditDlg)xtpDlg;
  int type;

  type=(int)GetOptionMenuValue(dlg->wType);
  XtSetSensitive(dlg->wDefault,!(type & VTM_HASGROUP));
  XtSetSensitive(dlg->wDefaultLabel,!(type & VTM_HASGROUP));
}

/***********************************************************************
**                                                                    //
**  "Setup warning" dialog box & callbacks                           ///
**                                                                  ////
***********************************************************************/

static Widget OpenSetupWarningDlg(View w,XtCallbackProc cbp) {
  Widget wDlg;
  XtPointer xtp;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_SETUPWARNING);
  if (wDlg!=NULL) {
    GetValues(wDlg,XmNuserData,&xtp,NULL);
    if (xtp==NULL) return NULL;
    SetValues(wDlg,XmNuserData,(XtPointer)cbp,NULL);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    wDlg=Cw(XmCreateWarningDialog,w->x->wMain,DLG_SETUPWARNING,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNautoUnmanage,False,
      XmNuserData,(XtPointer)cbp,
      NULL);
    XtAddCallback(wDlg,XmNokCallback,CbSetupWarningOk,(XtPointer)w);
    XtAddCallback(wDlg,XmNcancelCallback,CbUnmap,NULL);
    XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    XtManageChild(wDlg);
  }
  return wDlg;
}

static void CbSetupWarningOk(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  XtCallbackProc cbp;
  XtPointer xtp;

  GetValues(wg,XmNuserData,&xtp,NULL);
  XtPopdown(XtParent(wg));
  if (xtp!=NULL) {
    SetValues(wg,XmNuserData,NULL,NULL);
    cbp=(XtCallbackProc)xtp;
    cbp(wg,xtpV,pcbs);
  }
}

/***********************************************************************
**                                                                    //
**  "Save Configuration" dialog and callbacks                        ///
**                                                                  ////
***********************************************************************/

static Widget OpenSaveConfigDlg(View w) {
  Widget wDlg;
  char s[DG_FNAME_LEN], *s1;
  XmString xms;
  XtPointer xtp;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_SAVECONFIG);
  if (wDlg!=NULL) {
    GetValues(wDlg,XmNuserData,&xtp,NULL);
    XtRemoveCallback(wDlg,XmNdestroyCallback,CbFree,xtp);
    Free((void*)xtp);
  } else {
    wDlg=Cw(XmCreateQuestionDialog,w->x->wMain,DLG_SAVECONFIG,
      XmNdeleteResponse,XmDO_NOTHING,
      XmNautoUnmanage,False,
      NULL);
    XtAddCallback(wDlg,XmNokCallback,CbSaveConfigDlgOk,(XtPointer)w);
    XtAddCallback(wDlg,XmNcancelCallback,CbUnmap,NULL);
    XmAddWMProtocolCallback(XtParent(wDlg),w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
  }

  if (w->xapp->config==NULL) {
    strcpy(s,w->xapp->argv[0]);
    strcpy(GetFileExt(s),GetStr(w,FSTR_CFGEXT));
  } else strcpy(s,w->xapp->config->fName);
  s1=MallocString(s);

  SetValues(wDlg,XmNuserData,(XtPointer)s1,NULL);
  XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)s1);

  xms=XmStringCreateLocalized(GetResourceStringEx(wDlg,"messageStringEx",
    "MessageString","$(FILENAME)%s",s1));
  SetValues(wDlg,XmNmessageString,xms,NULL);
  XmStringFree(xms);

  XtManageChild(wDlg);
  XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

static void CbSaveConfigDlgOk(Widget wDlg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  App a=w->app;
  XApp xap;
  char* s,* msg;
  int err;
  Index ix;
  Group g;
  XtPointer xtp;

  /* Check for no visual objects */

  if (a==NULL) return;
  xap=w->xapp;
  if (a->equil!=NULL || a->template!=NULL ||
      !IsEmptyGroup(a->nodes) || !IsEmptyGroup(a->elems) ||
      !IsEmptyGroup(a->surfacesEx) || !IsEmptyGroup(a->gridPointsEx) ||
      !IsEmptyGroup(a->separators))
    if (WarningBox(wDlg,GetStr(w,QUE_WITHGEOMETRY))) return;

  /* Get configuration filename */

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  s=(char*)xtp;
  assert(s!=NULL);

  /* Save as configuration */

  err=SaveApp(a,s,DGFM_CONFIG);
  if (err) {
    ErrorBox(w->x->wMain,GetStr(w,err));
    return;
  }

  /* Reload configuration */

  if (xap->config!=NULL) xap->config=FreeApp(xap->config);

  xap->config=LoadApp(xap,s,&msg,&err);
  if (msg!=NULL) Free(msg);
  if (err) {
    ErrorBox(w->x->wMain,GetStr(w,err));
    return;
  }

  /* Are there other documents? */

  g=CreateGroup();
  for (w=Group1st(xap->views,&ix);w!=NULL;w=Next(&ix))
    if (w->app!=NULL && w->app!=a && !InGroup(g,w->app))
      GroupAdd(g,w->app);
  w=(View)xtpV; /* Restore w to its value after being used in the loop */

  /* If yes, then ask the user whether to update them */

  if (!IsEmptyGroup(g))
    if (!QuestionBox(wDlg,GetStr(w,QUE_UPDATEAPPS)))
      for (a=Group1st(g,&ix);a!=NULL;a=Next(&ix)) {
        if (IsEmptyGroup(a->views)) continue;
        UpdateLoadedApp(a,xap->config);
        UndoMark(a);
      }
  g=FreeGroup(g);

  XtPopdown(XtParent(wDlg));
}

/***********************************************************************
**                                                                    //
**  "Help Edit" dialog & callbacks                                   ///
**                                                                  ////
***********************************************************************/

static Widget OpenHelpEditDlg(View w,VarDef vd) {
  HelpEditDlg dlg=NULL;
  Widget wDlg;
  char s[DG_FNAME_LEN];

  sprintf(s,"*"DLG_HELPEDIT"_%p",vd);
  wDlg=XtNameToWidget(w->x->wMain,s);
  if (wDlg!=NULL) {
    wDlg=XtParent(XtParent(wDlg));
    ResetHelpEditDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->vd=vd;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_HELPEDIT);
    SetValues(dlg->wDlg,XmNuserData,(XtPointer)dlg,NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNokCallback,CbHelpEditDlgAccept,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    sprintf(s,DLG_HELPEDIT"_%p",vd);
    dlg->wText=Cmw(XmCreateScrolledText,dlg->wDlg,s,
      XmNeditMode,XmMULTI_LINE_EDIT,
    NULL);

    ResetHelpEditDlg(wDlg);
    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
        DwHelpEdit,dlg);

    XtManageChild(wDlg);
  }
  return wDlg;
}

static void ResetHelpEditDlg(Widget wDlg) {
  HelpEditDlg dlg;
  XtPointer xtp;

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  dlg=(HelpEditDlg)xtp;

  XmTextSetString(dlg->wText,dlg->vd->help!=NULL ? dlg->vd->help : NULL);
}

static void DwHelpEdit(Widget wg,View w,int ev,void* obj,void* pDlg) {
  HelpEditDlg dlg=(HelpEditDlg)pDlg;
  int i,j,rows,cols;
  VarDef vd;
  Index ix;
  VarSetDefEditField vdf;
  XmString xms;

  if (dlg->vd==NULL) return;

  switch(ev) {
    case N_DEL:
      if (obj!=dlg->vd) return;
    case N_NEWAPP:
      dlg->vd=NULL;
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
    case N_ALT:
      if (~dlg->w->x->changes & CHF_VARDEFS) return;
  }
  SetValues(XtParent(dlg->wDlg),XmNtitle,
    GetResourceStringEx(dlg->wDlg,"dialogTitleEx","DialogTitle",
      "$(NAME)%s$(DESCR)%s",dlg->vd->name,dlg->vd->descr),
  NULL);
}

static void CbHelpEditDlgAccept(Widget wg,XtPointer arg,XtPointer pcbs) {
  HelpEditDlg dlg=(HelpEditDlg)arg;
  char* s;

  s=XmTextGetString(dlg->wText);
  ChangeHelpString(dlg->w->app,dlg->vd,s);
  UndoMark(dlg->w->app);

  XtPopdown(XtParent(dlg->wDlg));
}

/***********************************************************************
**                                                                    //
**  "Add variable..." dialog box & callbacks                         ///
**                                                                  ////
***********************************************************************/

static Widget OpenVarDefCreateDlg(View w,VarSetDef vsd,int row,int col) {
  Widget wDlg,wg;
  VarDefCreateDlg dlg=NULL;
  char s[DG_FNAME_LEN];
  XtPointer xtp;

  sprintf(s,"*"DLG_VARDEFCREATE"_%p",vsd);
  wDlg=XtNameToWidget(w->x->wMain,s);
  if (wDlg!=NULL) {
    wDlg=XtParent(wDlg);
    GetValues(wDlg,XmNuserData,&xtp,NULL);
    dlg=(VarDefCreateDlg)xtp;
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->vsd=vsd;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_VARDEFCREATE);
    SetValues(dlg->wDlg,XmNuserData,(XtPointer)dlg,NULL);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNokCallback,CbVarDefCreateOk,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    sprintf(s,DLG_VARDEFCREATE"_%p",vsd);
    wg=Cmw(XmCreateForm,wDlg,s,
      NULL);
    CreateMenuSystem(wg,
      "l@:nameLabel",0x0101,
      "x?@:name",&dlg->wName,0x0201,
    NULL);
    Form2Table(wg);
    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
        DwVarDefCreate,dlg);

    XtManageChild(wDlg);
  }

  dlg->row=row;
  dlg->col=col;
  return wDlg;
}

static void DwVarDefCreate(Widget wg,View w,int ev,void* obj,void* pDlg) {
  VarDefCreateDlg dlg=(VarDefCreateDlg)pDlg;
  int i,j,rows,cols;
  VarDef vd;
  Index ix;
  VarSetDefEditField vdf;
  XmString xms;

  if (dlg->vsd==NULL) return;

  switch(ev) {
    case N_DEL:
      if (obj!=dlg->vsd) return;
    case N_NEWAPP:
      dlg->vsd=NULL;
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
    case N_ALT:
      if (~dlg->w->x->changes & CHF_VARDEFS) return;
  }
  SetValues(XtParent(dlg->wDlg),XmNtitle,
    GetResourceStringEx(dlg->wDlg,"dialogTitleEx","DialogTitle",
      "$(NAME)%s$(DESCR)%s",dlg->vsd->name,dlg->vsd->descr),
  NULL);
}

static void CbVarDefCreateOk(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarDefCreateDlg dlg=(VarDefCreateDlg)xtpDlg;
  char name[DG_FNAME_LEN],* s;
  VarDef vd;
  Index ix;

  s=XmTextGetString(dlg->wName);
  if (sscanf(s,"%s%s",name,name)!=1) {
    ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_BADNAME));
    XtFree(s);
    return;
  }
  XtFree(s);

  for (vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) {
    if (!strcmp(vd->name,name)) {
      ErrorBox(dlg->wDlg,GetStr(dlg->w,ERR_DUPNAME));
      return;
    }
  }

  vd=AddVarDef(dlg->w->app,dlg->vsd,name,"CLASSIFIED",VT_FLOAT,0,
    dlg->col,dlg->row);
  UndoMark(dlg->w->app);
  OpenVarDefEditDlg(dlg->w,vd);
  XtPopdown(XtParent(dlg->wDlg));
}

/***********************************************************************
**                                                                    //
**  OutputModeDlg                                                    ///
**                                                                  ////
***********************************************************************/

typedef struct _OutputModeDlg {
  View w;
  Widget wDlg,wSwSonnet,wSwCarre;
  Widget wSwVars,wSwStructure,wSwTargets,wSwSurfaces,
      wSwGPoints/*,wSwCells*/;
}* OutputModeDlg;

static void ResetOutputModeDlg(OutputModeDlg dlg);
static void AcceptOutputModeDlg(OutputModeDlg dlg);
static void CbSetOutputMode(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

Widget OpenOutputModeDlg(View w) {
  OutputModeDlg dlg=NULL;
  Widget wDlg;
  char s[DG_FNAME_LEN];
  XtPointer xtp;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_OUTPUTMODE);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->wDlg=wDlg=CreateOkCancelDialog(w->x->wMain,DLG_OUTPUTMODE);
    SetValues(wDlg,
      XmNuserData,(XtPointer)dlg,
      NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbSetOutputMode,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    CreateMenuSystem(wDlg,
      "#:form",
       "$+8#:mode",XmCreateRadioBox,XmNorientation,XmHORIZONTAL,NULL,0,1,
        "t?:sonnet",&dlg->wSwSonnet,
        "t?:carre",&dlg->wSwCarre,
       "-:",
       "s-#:sep",0,2,
       "t#?:vars",1,3,&dlg->wSwVars,
       "t#?:structure",1,4,&dlg->wSwStructure,
       "t#?:targets",1,5,&dlg->wSwTargets,
       "t#?:surfaces",1,6,&dlg->wSwSurfaces,
       "t#?:gpoints",1,7,&dlg->wSwGPoints,
/*       "t#?:cells",1,8,&dlg->wSwCells, */
      "-#:",
      NULL);

    ResetOutputModeDlg(dlg);

    XtManageChild(wDlg);
  }
  else {
    GetValues(wDlg,XmNuserData,&xtp,NULL);
    ResetOutputModeDlg((OutputModeDlg)xtp);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }
    /*AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_DEL|N_NEWAPP,NULL,
        DwVarDefCreate,dlg); */
  return wDlg;
}

#define XTBSS XmToggleButtonSetState
#define XTBGS XmToggleButtonGetState

static void ResetOutputModeDlg(OutputModeDlg dlg) {
  XTBSS(dlg->wSwVars,!(dlg->w->app->outputFlags & OF_NC_VARS),False);
  XTBSS(dlg->wSwStructure,!(dlg->w->app->outputFlags & OF_NC_STRUCTURE),False);
  XTBSS(dlg->wSwTargets,!(dlg->w->app->outputFlags & OF_NC_TARGETS),False);
  XTBSS(dlg->wSwSurfaces,!(dlg->w->app->outputFlags & OF_NC_SURFACES),False);
  XTBSS(dlg->wSwGPoints,!(dlg->w->app->outputFlags & OF_NC_GPOINTS),False);
/*  XTBSS(dlg->wSwCells,!(dlg->w->app->outputFlags & OF_NC_CELLS),False); */

  XTBSS(dlg->wSwSonnet,dlg->w->app->outputMode==OUTPUTMODE_SONNET,False);
  XTBSS(dlg->wSwCarre,dlg->w->app->outputMode==OUTPUTMODE_CARRE,False);
}

static void AcceptOutputModeDlg(OutputModeDlg dlg) {
  int l;

  l=0;

  if (!XTBGS(dlg->wSwVars))      l|=OF_NC_VARS;
  if (!XTBGS(dlg->wSwStructure)) l|=OF_NC_STRUCTURE;
  if (!XTBGS(dlg->wSwTargets))   l|=OF_NC_TARGETS;
  if (!XTBGS(dlg->wSwSurfaces))  l|=OF_NC_SURFACES;
  if (!XTBGS(dlg->wSwGPoints))   l|=OF_NC_GPOINTS;
/*  if (!XTBGS(dlg->wSwCells))     l|=OF_NC_CELLS; */

  SetOutputFlags(dlg->w->app,l);
  SetAppOutputMode(dlg->w->app,
      XTBGS(dlg->wSwCarre)? OUTPUTMODE_CARRE : OUTPUTMODE_SONNET);
  UndoMark(dlg->w->app);

  XtPopdown(XtParent(dlg->wDlg));
}

static void CbSetOutputMode(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  AcceptOutputModeDlg((OutputModeDlg)xtpDlg);
}

/***********************************************************************
**                                                                    **
**  MeshEditOptionsDlg                                                **
**                                                                    **
***********************************************************************/

typedef struct _MeshOptionsDlg {
  View w;
  Widget wDlg,wMode,wSlidingThreshold,wSwDoubleBorder;
}* MeshOptionsDlg;

static void ResetMeshOptionsDlg(Widget wDlg);
static void AcceptMeshOptionsDlg(Widget wDlg);
static void CbSetMeshOptions(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

Widget OpenMeshOptionsDlg(View w) {
  MeshOptionsDlg dlg=NULL;
  Widget wDlg;
  XtPointer xtp;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_MESHOPTIONS);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;
    dlg->wDlg=wDlg=CreateOkCancelDialog(w->x->wMain,DLG_MESHOPTIONS);
    SetValues(wDlg,
      XmNuserData,(XtPointer)dlg,
      NULL);

    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,(XtCallbackProc)CbSetMeshOptions,dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    CreateMenuSystem(wDlg,
      "#:form",
       "l#:modeLabel",1,1,
       "*:modeMenu",
        "b@:surfaces",(XtPointer)MSM_SURFACE,
        "b@:splines",(XtPointer)MSM_SPLINE,
       "-:",
       "o?#:mode",&dlg->wMode,2,1,
       "s#:separator",0,2,
       "l#:slideLabel",1,3,
       "x?#:slide",&dlg->wSlidingThreshold,2,3,
       "t?#:border",&dlg->wSwDoubleBorder,2,4,
      "-#:",
      NULL);

    ResetMeshOptionsDlg(wDlg);

    XtManageChild(wDlg);
  }
  else {
    ResetMeshOptionsDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  }
  return wDlg;
}

static void ResetMeshOptionsDlg(Widget wDlg) {
  MeshOptionsDlg dlg=(MeshOptionsDlg)GetUserData(wDlg);
  char s[1024];

  assert(dlg!=NULL);

  SetOptionMenuValue(dlg->wMode,(XtPointer)dlg->w->app->meshSlidingMode);

  sprintf(s,"%g",dlg->w->app->meshSlidingThreshold);
  XmTextSetString(dlg->wSlidingThreshold,s);

  XmToggleButtonSetState(dlg->wSwDoubleBorder,dlg->w->app->bDoubleMeshBorder,
    True);
}

static void AcceptMeshOptionsDlg(Widget wDlg) {
  MeshOptionsDlg dlg=(MeshOptionsDlg)GetUserData(wDlg);
  double d;

  assert(dlg!=NULL);

  d=GetXmTextDouble(dlg->wSlidingThreshold);
  if (d==MAXDOUBLE || d<=0) {
    Cancel(dlg->w->app);
    ErrorBox(dlg->wDlg,
        GetResourceString(dlg->wDlg,"errBadSlidingThreshold",NULL,NULL));
    return;
  }
  SetMeshSlidingThreshold(dlg->w->app,d);

  SetDoubleMeshBorderFlag(dlg->w->app,
      XmToggleButtonGetState(dlg->wSwDoubleBorder));

  SetMeshSlidingMode(dlg->w->app,(int)GetOptionMenuValue(dlg->wMode));

  UndoMark(dlg->w->app);
  XtPopdown(XtParent(dlg->wDlg));
}

static void CbSetMeshOptions(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  AcceptMeshOptionsDlg(wg);
}

