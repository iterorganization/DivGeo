#include "x_dg.h"

/**********************************************************************\
*                                                                      *
*  DlgEditVars                                                         *
*                                                                      *
\**********************************************************************/

#define DLG_VARSEDIT "dlgVarsEdit"

#define RF_NEWVAL 0x0001 /* Comment line only */
#define RF_OLDVAL 0x0002 /* Comment line only */
#define RF_SEL    0x0004
#define RF_SENS   0x0008
#define RF_VALUE  0x0010 /* New value field */
#define RF_CLEARVAL 0x0020
#define RF_HOLD   0x0040

#define XT_VEF    0x65438953
#define XT_NEWVAL 0x23490834
#define XT_COMP   0x53190783

#define CV_MARK_EQUAL 1
#define CV_MARK_NOT_EQUAL 2
#define CV_MARK_GT 3
#define CV_MARK_GE 4
#define CV_MARK_LT 5
#define CV_MARK_LE 6

typedef struct _VarsEditDlg {
  int type;
  App a;
  View w;
  void* obj;
  VarSetDef vsd;
  int rows,cols;
  int updateFlag,bKeepHoldFlag,
    nSensitive,            /* # of sensitive fields */
    bViewAlreadyDestroyed; /* If set, do not update View */
  
  Group fields;

  Widget wDlg,wSwHold,wPopupMenu,wVarsFrame,wVarsForm,
    wBnAcceptAll,wBnResetAll,wMessage,wMainForm,wBnCollapse,
    wMbCompare,wMbDisplay;
  struct _VarsEditField* popupMenuField;
}* VarsEditDlg;

typedef struct _VarsEditField {
  int type;
  VarsEditDlg dlg;
  VarDef vd;
  Widget wOldValue,wNewValue,wBnSet;
  int bSensitive;
}* VarsEditField;

static void ActVarsDlgPopupMenu(Widget wg,XEvent* xev,String*,Cardinal*);
static void CbVarsDlgAccept(Widget wg,VarsEditDlg dlg,void* pcbs);
static void CbVarsDlgReset(Widget wg,VarsEditDlg dlg,void* pcbs);
static void CbVarsEditDlgVDHelp(Widget wg,VarsEditField vef,void* pcbs);
static void CbVarsDlgInput(Widget wg,XtPointer xtpVef,XtPointer pcbs);
static void CbVarsDlgAcceptVar(Widget wg,VarsEditField vef,void* pcbs);
static void CbVarsDlgResetVar(Widget wg,VarsEditField vef,void* pcbs);
static void CbVarsDlgResetVar_n(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbDestroyVarsEditDlg(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarsEditFocus(Widget wg,XtPointer pField,XtPointer pcbs);
static void CbVarsEditCompare_p(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarsEditDisplay_p(Widget wg,XtPointer xtpDlg,XtPointer pcbs);
static void CbVarsDlgCollapse(Widget wg,XtPointer xtpDlg,XtPointer pcbs);

static void ResetVarsDlgField(VarsEditField vef,unsigned flags);
static void CollapseVarsDlg(VarsEditDlg dlg,int bCollapse);

#define VarsDlgCollapsed(dlg) (!XtIsManaged((dlg)->wMainForm))

static XtActionsRec varsDlgActions[]={
  {"PopupVarsDlgMenu",ActVarsDlgPopupMenu}
};

static void CbVarsDlgClose(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;
  
  XtPopdown(XtParent(dlg->wDlg));
}

static void DwNotifyVarsDlg(Widget wg,View w,int evt,void* obj,
    VarsEditDlg dlg) {
  int i;
  VarsEditField vef;
  Index ixv;

  switch(evt) {
    case N_DESTROYVIEW:
      dlg->bViewAlreadyDestroyed=1;
      break;
    case N_DEL:
      if (obj!=dlg->obj) return;
    case N_NEWAPP:
      XtDestroyWidget(XtParent(dlg->wDlg));
      return;
    case N_ALT:
      if (w->x->changes & CHF_VARDEFS) {
        XtDestroyWidget(XtParent(dlg->wDlg));
        break;
      }
      
      /* Change the rest only if mapped */
      
      if (!IsMapped(XtParent(dlg->wDlg))) break;
      
    case N_USER:
      if (w->x->changes & CHF_VARS || evt==N_USER) {
        SetValues(XtParent(dlg->wDlg),XmNtitle,
          GetObjDescription(dlg->w,dlg->obj),
          NULL);
      }
      
      if (w->x->changes & (CHF_VARS|CHF_MARK) || evt==N_USER) {
        i=dlg->wSwHold==NULL ? 0 : XmToggleButtonGetState(dlg->wSwHold);
        for (vef=Group1st(dlg->fields,&ixv);vef!=NULL;vef=Next(&ixv))
          ResetVarsDlgField(vef,RF_SENS | (i? 0:RF_VALUE));
      }
      break;
    default:
      assert(0);
  }
}

#define COL_W 8
#define ADDW(wG,dx) (pi[pic].wg=(wG),pi[pic].x=vd->col*COL_W+(dx)+1,\
        pi[pic++].y=vd->row+1)

static VarsEditDlg CreateVarsEditDlg(View w,void* obj) {
  VarsEditDlg dlg;
  Widget wg,wg1,wg2,wForm1;
  char s[DG_FNAME_LEN*2];
  VarDef vd;
  Index ix;
  int i,j,k;
  VarsEditField vef;
  XmString xms;
  struct _PosInfo* pi;
  int pic;
  XtWidgetGeometry xtwg;
  Dimension spacing;

  /* Add actions, if this has not yet happened */

  XtAppAddActions(XtWidgetToApplicationContext(w->x->wMain),
      varsDlgActions,XtNumber(varsDlgActions));

  /* Allocate data structures */

  dlg=Malloc(sizeof(*dlg));
  dlg->type=T_VARSDLG;
  dlg->w=w;
  dlg->a=w->app;
  dlg->updateFlag=0;
  dlg->bKeepHoldFlag=0;
  dlg->obj=obj;
  dlg->popupMenuField=NULL;
  dlg->nSensitive=0;
  dlg->bViewAlreadyDestroyed=0;
  
  /* Create the dialog */

  dlg->wDlg=Cw(XmCreateFormDialog,w->x->wMain,DLG_VARSEDIT,
    XmNautoUnmanage,False,
    XmNdeleteResponse,XmDO_NOTHING,
    NULL);
  XtAddCallback(dlg->wDlg,XmNdestroyCallback,CbDestroyVarsEditDlg,
      (XtPointer)dlg);
  XmAddWMProtocolCallback(XtParent(dlg->wDlg),w->xapp->x->wm_delete_window,
    (XtCallbackProc)CbVarsDlgClose,dlg);
    
  dlg->fields=CreateGroup();

  switch(GetObjType(obj)) {
    case T_VARSET:
      SetValues(XtParent(dlg->wDlg),XmNtitle,GetObjDescription(w,obj),NULL);
      dlg->vsd=((VarSet)obj)->def;
      break;
    default:
      FatalError("CreateVarsEditDlg()-type%d: fatal error 1",
        GetObjType(obj));
  }
  wg=Cmw(XmCreateRowColumn,dlg->wDlg,"rowcol",
    XmNorientation,XmHORIZONTAL,
    XmNadjustLast,True,

    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    XmNtopAttachment,XmATTACH_FORM,
    NULL);

  /* Create top buttons, if needed */

  dlg->wBnCollapse=Cmw(XmCreateArrowButton,wg,"collapse",
    XmNarrowDirection,XmARROW_UP,
    NULL);
  XtAddCallback(dlg->wBnCollapse,XmNactivateCallback,CbVarsDlgCollapse,
    (XtPointer)dlg);

  dlg->rows=dlg->cols=0;
  dlg->wBnAcceptAll=dlg->wBnResetAll=NULL;
  
  for (k=j=i=0,vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) {
    vd->varType & VTM_HASGROUP ? i++ : j++;
    if (vd->flags & VFM_MULTIPLE) k++;
    dlg->rows=max(dlg->rows,vd->row+1);
    dlg->cols=max(dlg->cols,vd->col+1);
  }

  /* i: # of group vars / j: # of non-group vars / k: # of vfm_multiple's */

  wg1=NULL;

  if (j && !i) {
    dlg->wBnAcceptAll=Cmw(XmCreatePushButton,wg,"setAll",
      NULL);
    XtAddCallback(dlg->wBnAcceptAll,XmNactivateCallback,
      (XtCallbackProc)CbVarsDlgAccept,dlg);
  }

  if (j && !i) {
    dlg->wBnResetAll=Cmw(XmCreatePushButton,wg,"resetAll",
      NULL);
    XtAddCallback(dlg->wBnResetAll,XmNactivateCallback,
      (XtCallbackProc)CbVarsDlgReset,dlg);
  }

  if (k) dlg->wSwHold=Cmw(XmCreateToggleButton,wg,"hold",
    XmNset,False,
    NULL); else dlg->wSwHold=NULL;

  wg1=Cmw(XmCreatePushButton,wg,"close",
    NULL);
  XtAddCallback(wg1,XmNactivateCallback,
    (XtCallbackProc)CbVarsDlgClose,dlg);
  SetValues(dlg->wDlg,XmNcancelButton,wg1,NULL);

  wg1=Cmw(XmCreatePushButton,wg,"help",
    NULL);
  XtAddCallback(wg1,XmNactivateCallback,
    (XtCallbackProc)CbHelp,w);

  dlg->wMainForm=wForm1=Cmw(XmCreateForm,dlg->wDlg,"form1",
    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    XmNbottomAttachment,XmATTACH_FORM,
    XmNtopAttachment,XmATTACH_WIDGET,
    XmNtopWidget,wg,
    NULL);
    
  CreateWidgetSystem(wForm1,
    "s8:separ",
    "l2?:message",&dlg->wMessage,
    "s2:separ",
    "$5?:varsFrame",/*(XtPointer)XmCreateFrame,NULL,*/
      (XtPointer)XmCreateScrolledWindow,
      XmNscrollingPolicy,XmAUTOMATIC,
      NULL,
      &dlg->wVarsFrame,
    NULL);

  /* Create the popup menu */

  wg=XtCreateWidget("foo",coreWidgetClass,XtParent(dlg->wDlg),NULL,0);

  dlg->wPopupMenu=wg=Cw(XmCreatePopupMenu,wg,"varsPopupMenu",NULL);
  CreateMenuSystem(wg,
    "bA:reset",CbVarsDlgResetVar_n,(XtPointer)dlg,
    "s:separator",
    "c?:compare",&dlg->wMbCompare,
    "+:compareMenu",
     "bA@:equal",CbVarsEditCompare_p,(XtPointer)dlg,(XtPointer)CV_MARK_EQUAL,
     "bA@:not_equal",CbVarsEditCompare_p,(XtPointer)dlg,
                    (XtPointer)CV_MARK_NOT_EQUAL,
     "bA@:gt",CbVarsEditCompare_p,(XtPointer)dlg,(XtPointer)CV_MARK_GT,
     "bA@:ge",CbVarsEditCompare_p,(XtPointer)dlg,(XtPointer)CV_MARK_GE,
     "bA@:lt",CbVarsEditCompare_p,(XtPointer)dlg,(XtPointer)CV_MARK_LT,
     "bA@:le",CbVarsEditCompare_p,(XtPointer)dlg,(XtPointer)CV_MARK_LE,
    "-:",
    "bA?:display",CbVarsEditDisplay_p,dlg,&dlg->wMbDisplay,
    NULL);
  AddCallbackToTree(XtParent(dlg->wPopupMenu),xmPushButtonWidgetClass,
    XmNarmCallback,CbDisplayHelpLine,(XtPointer)dlg->wMessage);
  AddCallbackToTree(XtParent(dlg->wPopupMenu),xmPushButtonWidgetClass,
    XmNdisarmCallback,
    (XtCallbackProc)CbClearHelpLine,(XtPointer)dlg->wMessage);

  /* Create fields for individual variables */

  wg=dlg->wVarsForm=Cw(XmCreateForm,dlg->wVarsFrame,"vars",
    XmNautoUnmanage,False,
    NULL);
  SetValues(dlg->wVarsFrame,XmNworkWindow,wg,NULL);


  pi=Malloc(GroupCount(dlg->vsd->varDefs)*COL_W*sizeof(*pi)+1);
  pic=0;

  for (vd=Group1st(dlg->vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) {

    vef=Malloc(sizeof(*vef));
    GroupAdd(dlg->fields,vef);
    vef->type=XT_VEF;
    vef->dlg=dlg;
    vef->vd=vd;
    vef->bSensitive=False;

    xms=MakeXmString(vd->descr);
    wg1=Cmw(XmCreateLabel,wg,"varName",
      XmNalignment,XmALIGNMENT_BEGINNING,
      XmNlabelString,xms,
      XmNuserData,(XtPointer)vef,
      NULL);
    XmStringFree(xms);

    ADDW(wg1,1);

    if (vef->vd->help!=NULL) {
      wg1=Cmw(XmCreatePushButton,wg,"varHelp",
        XmNalignment,XmALIGNMENT_BEGINNING,
        XmNuserData,(XtPointer)vef,
        NULL);
      XtAddCallback(wg1,XmNactivateCallback,
        (XtCallbackProc)CbVarsEditDlgVDHelp,vef);
      XtAddCallback(wg1,XmNarmCallback,CbVarsEditFocus,(XtPointer)vef);
      XtAddCallback(wg1,XmNdisarmCallback,CbVarsEditFocus,(XtPointer)vef);
      ADDW(wg1,2);
    }

    if (!(vd->varType & VTM_HASGROUP)) {
      vef->wNewValue=wg1=Cmw(XmCreateText,wg,"newValueText",
        XmNuserData,(XtPointer)vef,
        NULL);
      XtAddCallback(vef->wNewValue,XmNvalueChangedCallback,
        (XtCallbackProc)CbVarsDlgInput,(XtPointer)vef);
      XtAddCallback(vef->wNewValue,XmNfocusCallback,
        CbVarsEditFocus,(XtPointer)vef);
      XtAddCallback(vef->wNewValue,XmNlosingFocusCallback,
        CbVarsEditFocus,(XtPointer)vef);
      ADDW(wg1,3);
    } else {
      vef->wNewValue=NULL;
      wg1=Cmw(XmCreatePushButton,wg,"resetVar",
        XmNuserData,(XtPointer)vef,
        NULL);
      XtAddCallback(wg1,XmNactivateCallback,
        (XtCallbackProc)CbVarsDlgResetVar,(XtPointer)vef);
      XtAddCallback(wg1,XmNarmCallback,CbVarsEditFocus,(XtPointer)vef);
      XtAddCallback(wg1,XmNdisarmCallback,CbVarsEditFocus,(XtPointer)vef);
      ADDW(wg1,3);
    }
 
    wg1=vef->wBnSet=Cmw(XmCreatePushButton,wg,"set",
      XmNarrowDirection,XmARROW_RIGHT,
        XmNuserData,(XtPointer)vef,
      NULL);
    XtAddCallback(wg1,XmNactivateCallback,
      (XtCallbackProc)CbVarsDlgAcceptVar,vef);
    XtAddCallback(wg1,XmNarmCallback,CbVarsEditFocus,(XtPointer)vef);
    XtAddCallback(wg1,XmNdisarmCallback,CbVarsEditFocus,(XtPointer)vef);
    ADDW(wg1,5);

    vef->wOldValue=NULL;
  }
  
  dlg->popupMenuField=NULL;

  CreateFormTable(wg,pi,pic);
  pi=Free(pi);
  
  XtManageChild(dlg->wVarsForm);
  
  /* Adjust ScrolledWindow size */

  XtQueryGeometry(dlg->wVarsForm,NULL,&xtwg);
  GetValues(dlg->wVarsFrame,XmNspacing,&spacing,NULL);
  
  XtMakeResizeRequest(dlg->wVarsFrame,
      xtwg.width+spacing,xtwg.height+spacing,NULL,NULL);

  /* Finish widget creation */

  AddDependentWidget(w,dlg->wDlg,N_NEWAPP | N_ALT | N_DEL | N_DESTROYVIEW,NULL,
    (DwNotifyProc)DwNotifyVarsDlg,dlg);
  XtManageChild(dlg->wDlg);
  
  /* Add to the list of active dialogs */

  GroupAdd(w->x->varsDialogs,dlg);

  return dlg;
}
#undef AddW

static void ActVarsDlgPopupMenu(Widget wg,XEvent* xev,String* args,
    Cardinal* pargn) {
  VarsEditField vef=(VarsEditField)GetUserData(wg);
  if (vef==NULL) return;
  if (vef->type!=XT_VEF) return;

  vef->dlg->popupMenuField=vef;
  
  SetSensitiveEx(vef->dlg->wMbCompare,vef->vd->flags & VFM_MULTIPLE);
  SetSensitiveEx(vef->dlg->wMbDisplay,vef->vd->flags & VFM_MULTIPLE);
  
  XmMenuPosition(vef->dlg->wPopupMenu,&xev->xbutton);
  XtManageChild(vef->dlg->wPopupMenu);
}

static void CbVarsDlgInput(Widget wg,XtPointer xtpVef,XtPointer pcbs) {
  VarsEditField vef=(VarsEditField)xtpVef;
  assert(vef->type==XT_VEF);
  
  if (vef->dlg->wSwHold!=NULL && !vef->dlg->bKeepHoldFlag) {
    if (!XmToggleButtonGetState(vef->dlg->wSwHold))
      XmToggleButtonSetState(vef->dlg->wSwHold,True,True);
  }
    
  ResetVarsDlgField(vef,RF_SENS);
}

static char* GetVarValueDescr(View w,void* value,VarDef vd) {
  static char s[DG_FNAME_LEN];
  Group g;
  char* ps;

  if (vd->varType & VTM_HASGROUP) {
    g=value;
    if (g==NULL || !GroupCount(g))
      return GetStr(w,STR_NOELEMS);
/*    if (vd->varType==VT_ELEM) {
      sprintf(s,"#%d",((Elem)Group1st(g,NULL))->id);
      return s;
    } */
    if (vd->varType & VTF_HASELEMS)
      sprintf(s,"%d %s",GroupCount(g),GetStr(w,STR_ELEM));
    else if (vd->varType & VTF_HASCHORDS)
      sprintf(s,"%d %s",GroupCount(g),GetStr(w,STR_CHORD));
    else if (vd->varType & VTF_HAS_MESH_OBJECTS)
      sprintf(s,"%d %s",GroupCount(g),
      GetStr(w,vd->varType==VT_MESH_CELLS? STR_MESH_CELL : STR_MESH_ELEMENT));
    else assert(0);
    if (GroupCount(g)>1) strcat(s,"s");
    return s;
  } else {
    ps=value;
    if (ps==VV_NOSEL) ps=GetStr(w,STR_NOSEL);
    else if (ps==VV_DIFF) ps=GetStr(w,STR_DIFF);

    return ps;
  }
  FatalError("GetVarsDlgFieldValue()-return: fatal error 1");
}
 

static void ResetVarsDlgField(VarsEditField vef,unsigned flags) {
  char* s,* s1;
  XmString xms;
  Group g;
  void* p;
  int b;

  p=GetVarEx(vef->dlg->w->app,vef->dlg->obj,vef->vd);

  if (flags & (RF_NEWVAL|RF_OLDVAL)) {
    s=GetVarValueDescr(vef->dlg->w,p,vef->vd);

    s=GetResourceStringEx(vef->dlg->wDlg,
      flags & RF_OLDVAL? "strOldVal" : "strNewVal","StrVal",
      "$(VAR)%s$(VALUE)%s",vef->vd->descr,s);
    if (GetUserData(vef->dlg->wMessage)==(XtPointer)XT_COMP)
      SetUserData(vef->dlg->wMessage,NULL);
    else {
      SetLabelString(vef->dlg->wMessage,s);
      SetUserData(vef->dlg->wMessage,
          (XtPointer)(flags & RF_NEWVAL? XT_NEWVAL : XT_VEF));
    }
  }
  
  if (flags & RF_CLEARVAL) {
    if (GetUserData(vef->dlg->wMessage)==(XtPointer)XT_VEF) {
      SetLabelString(vef->dlg->wMessage,"");
      SetUserData(vef->dlg->wMessage,NULL);
    }
  }
  
  if (flags & RF_VALUE) {
    vef->dlg->bKeepHoldFlag++;
    if (vef->wNewValue!=NULL && XmIsText(vef->wNewValue)) {
      if (p==VV_NOSEL || p==VV_DIFF) s="";
      else s=p;
      XmTextSetString(vef->wNewValue,s);
    }
    vef->dlg->bKeepHoldFlag--;
  }
    
  if (flags & RF_SEL) {
    if (vef->vd->varType & VTM_HASGROUP) {
      MarkGroup(vef->dlg->a,p,10);
    }
  }
  
  if (flags & RF_SENS) {
    b=-1;
    if (!(vef->vd->varType & VTM_HASGROUP) && vef->wNewValue!=NULL &&
        XmIsText(vef->wNewValue) && vef->wBnSet!=NULL) {
      s=XmTextGetString(vef->wNewValue);
      b=0;
      if (p==VV_NOSEL) b=0;
      else if (p==VV_DIFF) b=!!*s;
      else b=!!strcmp(p,s);
    } else if (vef->vd->varType & VTM_HASGROUP && vef->wBnSet!=NULL) {
      b=1;
    }
    if (b>=0) {
      if (!b != !vef->bSensitive) {
        b? vef->dlg->nSensitive++ : vef->dlg->nSensitive--;
        vef->bSensitive=b;
      }

      if (flags & RF_HOLD && vef->dlg->wSwHold!=NULL) {
        if (!vef->dlg->nSensitive)
          XmToggleButtonSetState(vef->dlg->wSwHold,False,False);
      }

      SetSensitiveEx(vef->wBnSet,vef->bSensitive);
      if (vef->dlg->wBnAcceptAll!=NULL) 
        SetSensitiveEx(vef->dlg->wBnAcceptAll,!!vef->dlg->nSensitive);

      if (vef->dlg->wBnResetAll!=NULL) {
        SetSensitiveEx(vef->dlg->wBnResetAll,!!vef->dlg->nSensitive ||
            (vef->dlg->wSwHold!=NULL && 
            XmToggleButtonGetState(vef->dlg->wSwHold)));
      }
    }
  }
  
}
        
static int AcceptVarsDlgField(VarsEditField vef) {
  void* val,* errObj=NULL;
  int r;

  if (GetVarExLocks(vef->dlg->a,vef->dlg->obj,vef->vd)) return ERR_USEDBY;

  if (vef->vd->varType & VTM_HASGROUP) {
    val=CopyGroup(vef->dlg->a->mark,NULL);
/*    if (vef->vd->varType & VTF_HASELEMS) {
      val=CopyGroup(vef->dlg->a->mark,NULL);
      RestrictGroupToType(val,T_ELEM);
    } else if (vef->vd->varType & VTF_HASCHORDS) {
      val=CopyGroup(vef->dlg->a->mark,NULL);
      RestrictGroupToType(val,T_CHORD);
    } else assert(0);*/
  } else
    val=XmTextGetString(vef->wNewValue);

  r=SetVarEx(vef->dlg->a,vef->dlg->obj,vef->vd,val);
  if (r) {
    CheckValue(vef->dlg->a,val,vef->vd->varType,&errObj);
    if (errObj!=NULL && vef->dlg->w!=NULL) {
      LabelObject(vef->dlg->w,errObj,
        GetStr(vef->dlg->w,STR_ERRLABEL),True);
      UndoMark(vef->dlg->a);
    }
  }

  if (vef->vd->varType & VTM_HASGROUP)
    val=FreeGroup(val);
  else
    XtFree(val);

  return r;
}

static int IsEmptyField(VarsEditField vef) {
  char *s,s2[DG_FNAME_LEN];
  int b;

  if (vef->vd->type & VTM_HASGROUP) return 0;
  if (!XmIsText(vef->wNewValue)) return 0;
  s=XmTextGetString(vef->wNewValue);
  b=sscanf(s,"%s",s2);
  XtFree(s);
  return b!=1;
}

static void CbVarsDlgAcceptVar(Widget wg,VarsEditField vef,void* pcbs) {
  int i;

  i=AcceptVarsDlgField(vef);
  if (i) ErrorBox(vef->dlg->wDlg,i!=ERR_USEDBY?
    GetStr(vef->dlg->w,i) :
    WhyLockedEx(vef->dlg->w,vef->dlg->obj,vef->vd));
  else {
    UndoMark(vef->dlg->a);
    ResetVarsDlgField(vef,RF_NEWVAL|RF_SENS|RF_HOLD);
  }
}

static void CbVarsDlgResetVar(Widget wg,VarsEditField vef,void* pcbs) {
  ResetVarsDlgField(vef,RF_VALUE|RF_SEL|RF_SENS|RF_HOLD);
  UndoMark(vef->dlg->a);
}

static void CbVarsDlgResetVar_n(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;
  
  if (dlg->popupMenuField==NULL) return;
  
  ResetVarsDlgField(dlg->popupMenuField,RF_VALUE|RF_SEL|RF_SENS|RF_HOLD);
  UndoMark(dlg->a);
}

static void CbVarsDlgAccept(Widget wg,VarsEditDlg dlg,void* pcbs) {
  VarsEditField vef;
  Index ix;
  int i;
  char s[DG_FNAME_LEN*2];

  for (vef=Group1st(dlg->fields,&ix);vef!=NULL;vef=Next(&ix)) {
    if (vef->vd->flags & VFM_MULTIPLE && IsEmptyField(vef)) continue;
    i=AcceptVarsDlgField(vef);
    if (i) {
      Cancel(dlg->a);
      sprintf(s,"%s : %s",vef->vd->descr,i != ERR_USEDBY ?
        GetStr(vef->dlg->w,i) :
        WhyLockedEx(dlg->w,vef->dlg->obj,vef->vd));
      ErrorBox(dlg->wDlg,s);
      break;
    }
  }
  if (vef!=NULL) Cancel(dlg->a); else {
    for (vef=Group1st(dlg->fields,&ix);vef!=NULL;vef=Next(&ix))
      ResetVarsDlgField(vef,RF_SENS|RF_HOLD);
    if (dlg->wSwHold!=NULL) XmToggleButtonSetState(dlg->wSwHold,False,False);
    UndoMark(dlg->a);
  }
}

static void CbVarsDlgReset(Widget wg,VarsEditDlg dlg,void* pcbs) {
  VarsEditField vef;
  Index ix;

  for (vef=Group1st(dlg->fields,&ix);vef!=NULL;vef=Next(&ix))
    ResetVarsDlgField(vef,RF_SEL|RF_VALUE|RF_SENS|RF_HOLD);

  if (vef!=NULL) Cancel(dlg->a); else {
    UndoMark(dlg->a);
    if (dlg->wSwHold!=NULL) XmToggleButtonSetState(dlg->wSwHold,False,False);
  }

  UndoMark(dlg->a);
}

Widget OpenVarsEditDlg(View w,void* obj) {
  VarsEditDlg dlg;
  VarsEditField vef;
  Index ix;

  switch(GetObjType(obj)) {
    case T_VARSET:
      break;
    default:
      FatalError("OpenVarsEditDlg()-type%d:fatal error 1",GetObjType(obj));
  }

  for (dlg=Group1st(w->x->varsDialogs,&ix);dlg!=NULL;dlg=Next(&ix))
      if (dlg->obj==obj) {
    DwNotifyVarsDlg(dlg->wDlg,w,N_USER,NULL,dlg);
    XtPopup(XtParent(dlg->wDlg),XtGrabNone);
    CollapseVarsDlg(dlg,False);
    return dlg->wDlg;
  }

  dlg=CreateVarsEditDlg(w,obj);
  DwNotifyVarsDlg(dlg->wDlg,w,N_USER,NULL,dlg);

  return dlg->wDlg;
}

static void CbDestroyVarsEditDlg(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;

  if (!dlg->bViewAlreadyDestroyed) GroupDel(dlg->w->x->varsDialogs,dlg);
  
  if (dlg->fields!=NULL) dlg->fields=FreeMallocedGroup(dlg->fields);
  dlg=Free(dlg);
}

static void CbVarsEditDlgVDHelp(Widget wg,VarsEditField vef,void* pcbs) {
  Widget wDlg;

  DisplayHelpTitle(vef->dlg->w,vef->vd->descr);
  DisplayHelpText(vef->dlg->w,vef->vd->help,0);
}

static void CbVarsEditFocus(Widget wg,XtPointer pField,XtPointer pcbs) {
  VarsEditField vef=(VarsEditField)pField;
  VarsEditDlg dlg=vef->dlg;
  XmAnyCallbackStruct* cbs=(XmAnyCallbackStruct*)pcbs;
  String s;

  switch(cbs->reason) {
    case XmCR_FOCUS:
    case XmCR_ARM:
      ResetVarsDlgField(vef,RF_OLDVAL);
      break;
    case XmCR_LOSING_FOCUS:
    case XmCR_DISARM:
      ResetVarsDlgField(vef,RF_CLEARVAL);
      break;
    default:
      assert(0);
  }
}

/* To be called ONLY from the action-triggered popup menu */
static void CbVarsEditCompare_p(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;
  VarsEditField vef=dlg->popupMenuField;
  int mode=(int)GetUserData(wg);
  int i1,i2,comp;
  double f1,f2;
  char* s,* s1;
  Group g;
  Index ix;
  void* p;
  
  if (vef==NULL || vef->type!=XT_VEF) return;
  if (vef->wNewValue==NULL || !XmIsText(vef->wNewValue)) return;
  
  if (!(vef->vd->flags & VFM_MULTIPLE)) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadScope",NULL,NULL));
    return;
  }

  switch (vef->vd->varType) {
    case VT_INT:
    case VT_FLOAT:
    case VT_TEXT:
      s=XmTextGetString(vef->wNewValue);
      break;
    default:
      return;
  }
  
  switch (vef->vd->varType) {
    case VT_INT:
      if (sscanf(s,"%d",&i1)!=1) {
        XtFree(s);
        ErrorBox(dlg->wDlg,
            GetResourceString(dlg->wDlg,"errBadNumber",NULL,NULL));
        return;
      }
      break;
    case VT_FLOAT:
      if (sscanf(s,SCANFLT,&f1)!=1) {
        XtFree(s);
        ErrorBox(dlg->wDlg,
            GetResourceString(dlg->wDlg,"errBadNumber",NULL,NULL));
        return;
      }
      break;
  }
      
  g=GetVarOriginGroup(dlg->w->app,dlg->obj,vef->vd,0);
  
  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix)) {
    s1=GetVar(p,vef->vd,dlg->obj);

    switch(vef->vd->varType) {
      case VT_INT:
        if (sscanf(s1,"%d",&i2)!=1) comp=-2;
        else if (i1==i2) comp=0;
        else comp=(i1<i2)? 1 : -1;
        break;
      case VT_FLOAT:
        if (sscanf(s1,SCANFLT,&f2)!=1) comp=-2;
        else if (f1==f2) comp=0;
        else comp=(f1<f2)? 1 : -1;
        break;
      case VT_TEXT:
        comp=strcmp(s1,s);
        break;
      default:
        assert(0);
    }
    
    switch (mode) {
      case CV_MARK_EQUAL:
        if (comp!=0) GroupDel(g,p);
        break;
      case CV_MARK_NOT_EQUAL:
        if (comp==0) GroupDel(g,p);
        break;
      case CV_MARK_GT:
        if (comp!=1) GroupDel(g,p);
        break;
      case CV_MARK_GE:
        if (comp!=1 && comp!=0) GroupDel(g,p);
        break;
      case CV_MARK_LT:
        if (comp!=-1) GroupDel(g,p);
        break;
      case CV_MARK_LE:
        if (comp!=-1 && comp!=0) GroupDel(g,p);
        break;
      default:
        assert(0);
    }
  }
  XtFree(s);
  
  MarkGroup(dlg->w->app,g,10);
  UndoMark(dlg->w->app);

  SetLabelString(dlg->wMessage,
      GetResourceStringEx(dlg->wDlg,"msgObjectsMatching",NULL,"$(COUNT)%d",
      GroupCount(g)));
  SetUserData(dlg->wMessage,(XtPointer)XT_COMP);

  g=FreeGroup(g);
}  

/* To be called ONLY from the action-triggered popup menu */
static void CbVarsEditDisplay_p(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;
  VarsEditField vef=dlg->popupMenuField;
  char* s,* s1;
  Group g;
  Index ix;
  void* p;
  
  if (vef==NULL || vef->type!=XT_VEF) return;
  
  if (!(vef->vd->flags & VFM_MULTIPLE)) {
    ErrorBox(dlg->wDlg,GetResourceString(dlg->wDlg,"errBadScope",NULL,NULL));
    return;
  }

  g=GetVarOriginGroup(dlg->w->app,dlg->obj,vef->vd,0);

  RemoveAllViewLabels(dlg->w);
  
  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix)) {
    s=GetVar(p,vef->vd,dlg->obj);
    LabelObject(dlg->w,p,GetVarValueDescr(dlg->w,s,vef->vd),False);
  }

  g=FreeGroup(g);
  
  UndoMark(dlg->w->app);
}  

static void CollapseVarsDlg(VarsEditDlg dlg,int bCollapse) {
  if (bCollapse && !VarsDlgCollapsed(dlg)) {
    XtUnmanageChild(dlg->wMainForm);
    SetValues(dlg->wBnCollapse,XmNarrowDirection,XmARROW_DOWN,NULL);
  } else if (!bCollapse && VarsDlgCollapsed(dlg)) {
    XtManageChild(dlg->wMainForm);
    SetValues(dlg->wBnCollapse,XmNarrowDirection,XmARROW_UP,NULL);
  }
}

static void CbVarsDlgCollapse(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  VarsEditDlg dlg=(VarsEditDlg)xtpDlg;
  
  CollapseVarsDlg(dlg,!VarsDlgCollapsed(dlg));
}

/**********************************************************************\
*                                                                      *
*   InvalidVarsDlg functions and callbacks                             *
*                                                                      *
\**********************************************************************/

#define DLG_INVALIDVARS "dlgInvalidVars"

typedef struct _InvalidVarsDlg {
  View w;
  Widget wDlg,wList;
}* InvalidVarsDlg;

static void CbInvalidVarsWhere(Widget wg,XtPointer xDlg,XtPointer pcbs);
static void DwInvalidVars(Widget wg,View w,int ev,void* obj,void* pDlg);
static void ResetInvalidVarsDlg(Widget wDlg);

/* Public functions ***************************************************/

Widget OpenInvalidVarsDlg(View w) {
  InvalidVarsDlg dlg;
  Widget wDlg,wg;
  XtPointer xtp;

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_INVALIDVARS);
  if (wDlg!=NULL) {
    ResetInvalidVarsDlg(wDlg);
    XtPopup(XtParent(wDlg),XtGrabNone);
  } else {
    dlg=Malloc(sizeof(*dlg));
    dlg->w=w;

    wDlg=dlg->wDlg=CreateOkCancelDialog(w->x->wMain,DLG_INVALIDVARS);
    SetValues(dlg->wDlg,XmNuserData,(XtPointer)dlg,NULL);
    XtAddCallback(wDlg,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    XtAddCallback(wDlg,XmNokCallback,CbInvalidVarsWhere,(XtPointer)dlg);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);

    dlg->wList=Cmw(XmCreateScrolledList,wDlg,"list",
      XmNselectionPolicy,XmBROWSE_SELECT,
      NULL);
    XtAddCallback(dlg->wList,XmNdefaultActionCallback,CbInvalidVarsWhere,
      (XtPointer)dlg);

    AddDependentWidget(w,wDlg,N_NOW|N_ALT|N_NEWAPP,NULL,
	DwInvalidVars,dlg);

    ResetInvalidVarsDlg(wDlg);
    XtManageChild(wDlg);
  }

}

/* Callbacks **********************************************************/

static void CbInvalidVarsWhere(Widget wg,XtPointer xDlg,XtPointer pcbs) {
  InvalidVarsDlg dlg=(InvalidVarsDlg)xDlg;
  Group g;
  VarDef vd=NULL;
  VarSet vs=NULL;
  int err=0;
  int* pListPos;
  int listCount;

  if (!XmListGetSelectedPos(dlg->wList,&pListPos,&listCount)) return;
  CheckAllVars(dlg->w->app,&g);
  if (listCount==1 && *pListPos>=1 &&
      *pListPos<=GroupCount(g)/2) {
    vd=GroupAt(g,(*pListPos-1)*2);
    vs=GroupAt(g,(*pListPos-1)*2+1);
  }
  FreeGroup(g);
  XtFree((XtPointer)pListPos);
  if (vd==NULL) return;

  if (!CheckVar(dlg->w->app,&g,vd,vs,&err)) { /* Should never happen */
    FreeGroup(g);
    ResetInvalidVarsDlg(dlg->wDlg);
    return;
  }

  if (vd->varType & VTM_HASGROUP) {
    MarkGroup(dlg->w->app,GetVar(vs,vd,NULL),10);
    if (!IsEmptyGroup(g)) LabelObject(dlg->w,Group1st(g,NULL),
	GetStr(dlg->w,STR_ERRLABEL),True);
  } else MarkGroup(dlg->w->app,g,10);

  FreeGroup(g);

  if (err!=0) ViewMsgEx(dlg->w,err,NULL);

  OpenVarsEditDlg(dlg->w,vs);

  UndoMark(dlg->w->app);
}

static void DwInvalidVars(Widget wg,View w,int ev,void* obj,void* pDlg) {
  InvalidVarsDlg dlg=(InvalidVarsDlg)pDlg;
  int i=0;
  XmString xms;
  VarSetDef vsd;
  Index ix;

  if (!IsMapped(wg)) return;
/* -- outputMode change was not processed correctly
  if (ev==N_ALT && !(w->x->changes & (CHF_VARS|CHF_VARDEFS|CHF_GEOMETRY)))
    return;
*/
  ResetInvalidVarsDlg(dlg->wDlg);
}

static void ResetInvalidVarsDlg(Widget wDlg) {
  InvalidVarsDlg dlg;
  XtPointer xtp;
  VarDef vd;
  VarSet vs;
  int i;
  XmString xms;
  Group g;
  Index ix;

  GetValues(wDlg,XmNuserData,&xtp,NULL);
  dlg=(InvalidVarsDlg)xtp;

  XmListDeleteAllItems(dlg->wList);

  XtSetSensitive(XmMessageBoxGetChild(dlg->wDlg,XmDIALOG_OK_BUTTON),
    CheckAllVars(dlg->w->app,&g));

  for (i=0,vd=Group1st(g,&ix);vd!=NULL;i++,vd=Next(&ix)) {
    vs=Next(&ix);
    assert(vs!=NULL);

    xms=MakeXmString(vd->descr);
    XmListAddItem(dlg->wList,xms,++i);
    XmStringFree(xms);
  }

  FreeGroup(g);

  XmListSelectPos(dlg->wList,1,True);
}
