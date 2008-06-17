/* Menu commands */

#include "x_dg.h"

/* Menu callbacks
*/

void CbFileNew(Widget wg,View w,void* xtp) {
  App a,a1;
  View w1;

  SetActiveView(w);
  if (w->app==NULL) return;

  a=AddApp(w->xapp);
  if (a==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOCONFIG));
    return;
  }
  if (w->app->fName==NULL && !IsAppUnsaved(w->app)) {
    a1=w->app;
    SetViewApp(w,a);
    if (IsEmptyGroup(a1->views)) a1=FreeApp(a1);
    w1=w;
  } else {
    w1=CreateXmView(w->xapp,a);
  }
  /* ShowPicture(w1); */
  FreeUndoInfo(a);
  a->alt=0;
}

void CbFileOpen(Widget wg,View w,void* xtp) {
  SetActiveView(w);
  if (w->app==NULL) return;

  OpenFileOpenDlg(w);
}

void CbFileSave(Widget wg,View w,void* xtp) {

  SetActiveView(w);
  if (w->app==NULL) return;

  OpenFileSaveDlg(w,False);
}

void CbFileOutput(Widget wg,XtPointer xtpw,XtPointer pcbs) {
  View w=(View) xtpw;
  char oFName[DG_FNAME_LEN],sFName[DG_FNAME_LEN],tFName[DG_FNAME_LEN],
    s[DG_FNAME_LEN*6];
  int r;

  if (w->app==NULL) return;
  SetActiveView(w);

  r=WriteOutputFile(w->app,NULL);
  if (!r) r=WriteStructureFile(w->app,NULL);
  if (!r) r=WriteTargetsFile(w->app,NULL);

  if (r) {ErrorBox(w->x->wMain,GetStr(w,r));return;}

  if (w->app->fName==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOFNAME));
    return;
  }

  strcpy(oFName,w->app->fName);
  strcpy(GetFileExt(oFName),GetStr(w,FSTR_OUTPUTEXT));
  strcpy(sFName,w->app->fName);
  strcpy(GetFileExt(sFName),GetStr(w,FSTR_STRUCTUREEXT));
  strcpy(tFName,w->app->fName);
  strcpy(GetFileExt(tFName),GetStr(w,FSTR_TARGETSEXT));

  sprintf(s,"%s\n%s: %s\n%s: %s\n%s: %s",
    w->app->outputMode==OUTPUTMODE_CARRE?
      GetResourceString(w->x->wMain,"msgOutputModeCarre",NULL,NULL) :
      GetResourceString(w->x->wMain,"msgOutputModeSonnet",NULL,NULL),
    GetStr(w,STR_OUTPUTFNAME),oFName,
    GetStr(w,STR_STRUCTUREFNAME),sFName,
    GetStr(w,STR_TARGETSFNAME),tFName);

  if (QuestionBox(w->x->wMain,s)) return;

  r=WriteOutputFile(w->app,oFName);
  if (r) {ErrorBox(w->x->wMain,GetStr(w,r));return;}
  r=WriteStructureFile(w->app,sFName);
  if (r) {ErrorBox(w->x->wMain,GetStr(w,r));return;}
  r=WriteTargetsFile(w->app,tFName);
  if (r) {ErrorBox(w->x->wMain,GetStr(w,r));return;}

  SetViewMsg(w,GetStr(w,MSG_OUTPUT));
}

void CbEquil(Widget wg,View w,void* xtp) {
  Widget wDlg;
  XmString xms;

  SetActiveView(w);
  if (w->app==NULL) return;

  if (w->app->equil!=NULL && IsLocked(w->app->equil)) {
    ErrorBox(w->x->wMain,WhyLocked(w,w->app->equil));
    return;
  }

  OpenEquilDlg(w);
}

void CbTemplate(Widget wg,View w,void* xtp) {
  Widget wDlg;
  XmString xms;

  SetActiveView(w);
  if (w->app==NULL) return;

  if (w->app->template!=NULL && IsLocked(w->app->template)) {
    ErrorBox(w->x->wMain,WhyLocked(w,w->app->template));
    return;
  }

  OpenTemplateDlg(w);
}

void CbSonnet(Widget wg,View w,void* xtp) {
  Widget wDlg;
  XmString xms;

  SetActiveView(w);
  if (w->app==NULL) return;

  if (w->app->sonnetData!=NULL && IsLocked(w->app->sonnetData)) {
    ErrorBox(w->x->wMain,WhyLocked(w,w->app->sonnetData));
    return;
  }

  OpenSonnetDlg(w);
}

void CbFilePrint(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV,psW;
  int i;

  SetActiveView(w);

  OpenFilePrintDlg(w);
}

void CmExportMesh(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  
  SetActiveView(w);
  
  OpenExportMeshDlg(w,False);
}

void CmExportElemsAsTemplate(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  
  SetActiveView(w);
  
  OpenExportElemsAsTemplateDlg(w);
}

void CbQuit(Widget wg,View w,void* xtp) {
  Index ix;

  for (w=Group1st(w->xapp->views,&ix);w!=NULL;w=Next(&ix))
    CloseXmView(w,True);
}

/* Edit menu commands
*/

void CbUndo(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  Undo(w->app);
}

void CbRedo(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  Redo(w->app);
}

void CbRedoAll(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  while (!IsEmptyGroup(w->app->redoStack)) Redo(w->app);
}

void CbMarkAll(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  MarkGroup(w->app,w->app->elems,1);
  SetViewFlags(w,w->showFlags | SHW_ELEMS);
  UndoMark(w->app);
}

void CbUnmarkAll(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  MarkGroup(w->app,w->app->mark,0);
  SetViewFlags(w,w->showFlags | SHW_ELEMS);
  UndoMark(w->app);
}

void CbMarkSeparators(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  MarkGroup(w->app,w->app->separators,1);
  SetViewFlags(w,w->showFlags | SHW_SEPARATORS);
  UndoMark(w->app);
}

void CbCmDelObjects(Widget wg,View w,void* pcbs) {
  XtPointer objType;
  void* p;
  Index ix;

  if (w->app==NULL) return;
  SetActiveView(w);

  GetValues(wg,XmNuserData,&objType,NULL);
  switch((int)objType) {
    case T_MARKING:
      for (p=AppMark1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        switch (GetObjType(p)) {
          case T_ELEM:
          case T_SOURCE:
          case T_CHORD:
	    if (IsLocked(p)) {
	      Cancel(w->app);
	      LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
	      SetViewMsg(w,WhyLocked(w,p));
	      return;
	    }
	    DelObject(w->app,p);
            break;
          default:
	    Cancel(w->app);
	    LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
	    SetViewMsg(w,GetStr(w,ERR_DEL_MARKED_OBJECT_TYPE));
	    return;
          
        }
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_MARKED_REMOVED,NULL);
      break;
    case T_NODE:
      for (p=AppNode1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        if (!IsEmptyGroup(((Node)p)->elems)) continue;
        if (IsLocked(p)) {
          Cancel(w->app);
          LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
          SetViewMsg(w,WhyLocked(w,p));
          return;
        }
        DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_EMPTYNODESREMOVED,NULL);
      break;
    case T_ELEM:
      for (p=AppElem1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        if (IsLocked(p)) {
          Cancel(w->app);
          LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
          SetViewMsg(w,WhyLocked(w,p));
          return;
        }
        DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_ELEMSREMOVED,NULL);
      break;
    case T_SOURCE:
      for (p=AppSource1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
	if (IsLocked(p)) {
	  Cancel(w->app);
	  LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
	  SetViewMsg(w,WhyLocked(w,p));
	  return;
	}
	DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_SOURCESREMOVED,NULL);
      break;
    case T_CHORD:
      for (p=AppChord1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        if (IsLocked(p)) {
          Cancel(w->app);
          LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
          SetViewMsg(w,WhyLocked(w,p));
          return;
        }
        DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_CHORDSREMOVED,NULL);
      break;
    case T_SURFACE:
      for (p=AppSurface1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        if (IsLocked(p)) {
          Cancel(w->app);
          LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
          SetViewMsg(w,WhyLocked(w,p));
          return;
        }
        DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_SURFACESREMOVED,NULL);
      break;
    case T_GRIDPOINT:
      for (p=AppGridPoint1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
        if (IsLocked(p)) {
          Cancel(w->app);
          LabelObject(w,p,GetStr(w,STR_ERRLABEL),True);
          SetViewMsg(w,WhyLocked(w,p));
          return;
        }
        DelObject(w->app,p);
      }
      UndoMark(w->app);
      ViewMsgEx(w,MSG_GPOINTSREMOVED,NULL);
      break;
    case T_SEPARATOR:
      RemoveAllSeparators(w->app);
      UndoMark(w->app);
      ViewMsgEx(w,MSG_SEPARATORSREMOVED,NULL);
      break;
    case T_EQUIL:
      if (IsLocked(w->app->equil))
	SetViewMsg(w,WhyLocked(w,w->app->equil));
      else {
	DelObject(w->app,w->app->equil);
	UndoMark(w->app);
	ViewMsgEx(w,MSG_EQUILREMOVED,NULL);
      }
      break;
    case T_TEMPLATE:
      if (IsLocked(w->app->template))
        SetViewMsg(w,WhyLocked(w,w->app->template));
      else {
	DelObject(w->app,w->app->template);
	UndoMark(w->app);
	ViewMsgEx(w,MSG_TEMPLATEREMOVED,NULL);
      }
      break;
    case T_SONNET:
      if (IsLocked(w->app->sonnetData))
        SetViewMsg(w,WhyLocked(w,w->app->sonnetData));
      else {
	DelObject(w->app,w->app->sonnetData);
	UndoMark(w->app);
	ViewMsgEx(w,MSG_SONNETREMOVED,NULL);
      }
      break;
    case T_MESH:
      if (w->app->mesh==NULL) break;
      if (IsLocked(w->app->mesh))
        SetViewMsg(w,WhyLocked(w,w->app->mesh));
      else {
	DelObject(w->app,w->app->mesh);
	UndoMark(w->app);
	ViewMsgEx(w,MSG_SONNETREMOVED,NULL);
      }
      break;
    case T_XPOINT:
      if (IsLocked(w->app->xpoint))
        SetViewMsg(w,WhyLocked(w,w->app->xpoint));
      else {
	DelObject(w->app,w->app->xpoint);
	UndoMark(w->app);
	ViewMsgEx(w,MSG_XPOINTREMOVED,NULL);
      }
      break;
    default:
      assert(0);
  }
}

/* View menu commands
*/

void CbViewRefresh(Widget wg,XtPointer xtpView,void* pcbs) {
  View w=(View)xtpView;

  SetActiveView(w);
  ClearView(w);
  RepaintView(w);
}

void CbZoomIn(Widget wg,View w,void* xtp) {
  SetActiveView(w);
  if (w->app==NULL) return;
  SetViewFactor(w,w->centerX,w->centerY,w->zoomX*2,w->zoomY*2);
  UndoMark(w->app);
}

void CbZoomOut(Widget wg,View w,void* xtp) {
  SetActiveView(w);
  if (w->app==NULL) return;
  SetViewFactor(w,w->centerX,w->centerY,w->zoomX/2,w->zoomY/2);
  UndoMark(w->app);
}

void CbShowPicture(Widget wg,View w,void* xtp) {
  SetActiveView(w);
  if (w->app==NULL) return;
  ShowPicture(w);
  UndoMark(w->app);
}

void CbShowSelection(Widget wg,View w,void* xtp) {
  double minX,minY,maxX,maxY;

  if (w->app==NULL) return;
  SetActiveView(w);

  if (IsEmptyGroup(w->app->mark)) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOSELECTION));
    return;
  }
  minX=0;maxX=-1;
  CalcGroupExtents(w->app->mark,&minX,&minY,&maxX,&maxY);
  if (minX>=maxX || minY>=maxY)
    SetViewFactor(w,minX,minY,w->zoomX,w->zoomY);
  else SetViewRect(w,
    minX-(maxX-minX)*w->shpIncr/100,
    minY-(maxY-minY)*w->shpIncr/100,
    maxX+(maxX-minX)*w->shpIncr/100,
    maxY+(maxY-minY)*w->shpIncr/100);

  UndoMark(w->app);
}

void CmPrevZoom(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  double minX,minY,maxX,maxY;
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);

  i=GetPrevViewInfo(w->app,&minX,&minY,&maxX,&maxY,False);
  if (i) {
/*    SetViewMsg(w,GetStr(w,i)); -- Does not work, because -1 is returned */
    Cancel(w->app);
  } else {
    SetViewRect(w,minX,minY,maxX,maxY);
    MarkPrevViewDone(w->app);
    UndoMark(w->app);
  }
}

void CmRemoveLabels(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);

  RemoveAllViewLabels(w);
  UndoMark(w->app);
}


void CmViewMeshHeader(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  if (w->app==NULL) return;
  SetActiveView(w);
  
  if (w->app->mesh==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOSONNET));
    return;
  }
  
  OpenEditMeshHeaderDlg(w,w->app->mesh);
}

/* Commands for the Commands menu
*/
/*
void CbCmAt(Widget wg,View w,void* pcbs) {
  Widget  wDlg;

  if (w->app==NULL) return;
  SetActiveView(w);

  wDlg=OpenAtDlg(w);
  ProcessModalDialog(wDlg);
} */

void CbCmRotMove(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  OpenRotMoveDlg(w);
}

void CbCmAppendTemplate(Widget wg,View w,void* pcbs) {
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);
  if (w->app->template==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOTEMPLATE));
    return;
  }

  LockAppUpdate(w->app,1);
  i=AppendTemplate(w->app);
  SetViewFlags(w,w->showFlags | SHW_ELEMS | SHW_IRRNODES | SHW_NORMALS);
  LockAppUpdate(w->app,-1);
  SetViewMsg(w,GetStr(w,0));
  UndoMark(w->app);
}

void CmConvertTemplateToChords(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);

  i=ConvertTemplateToChords(w->app);

  if (i) {
    Cancel(w->app);
    ErrorBox(w->x->wMain,GetStr(w,i));
  } else UndoMark(w->app);
}

void CmConvertElemsToChords(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  int i;
  void* pErr;
  Group g;

  if (w->app==NULL) return;
  SetActiveView(w);

  g=CopyGroup(w->app->mark,NULL);
  RestrictGroupToType(g,T_ELEM);

  if (IsEmptyGroup(g)) {
    FreeGroup(g);
    ErrorBox(w->x->wMain,GetStr(w,ERR_NO_MARKED_ELEMS));
    return;
  }

  i=ConvertElemsToChords(w->app,g,&pErr);
  FreeGroup(g);

  if (i) {
    Cancel(w->app);
    if (pErr!=NULL) LabelObject(w,pErr,GetStr(w,STR_ERRLABEL),True);
    UndoMark(w->app);
    ErrorBox(w->x->wMain,i==ERR_LOCKED? WhyLocked(w,pErr) : GetStr(w,i));
  } else UndoMark(w->app);
}

void CmConvertChordsToElems(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  int i;
  void* pErr;
  Group g;

  if (w->app==NULL) return;
  SetActiveView(w);

  g=CopyGroup(w->app->mark,NULL);
  RestrictGroupToType(g,T_CHORD);

  if (IsEmptyGroup(g)) {
    FreeGroup(g);
    ErrorBox(w->x->wMain,GetStr(w,ERR_NO_MARKED_CHORDS));
    return;
  }

  i=ConvertChordsToElems(w->app,g,&pErr);
  FreeGroup(g);

  if (i) {
    Cancel(w->app);
    if (pErr!=NULL) LabelObject(w,pErr,GetStr(w,STR_ERRLABEL),True);
    UndoMark(w->app);
    ErrorBox(w->x->wMain,i==ERR_LOCKED? WhyLocked(w,pErr) : GetStr(w,i));
  } else UndoMark(w->app);
}


void CbCmRemoveXPoint(Widget wg,View w,void* pcbs) {
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);

  if (w->app->xpoint==NULL) {
    ErrorBox(w->x->wMain,GetStr(w,ERR_NOXPOINT));
    return;
  }
  DelXPoint(w->app);
  SetViewMsg(w,GetStr(w,MSG_XPOINTREMOVED));
  UndoMark(w->app);
}

void CbCmRenumber(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  LockAppUpdate(w->app,1);
  RenumberElems(w->app);
  LockAppUpdate(w->app,-1);
  UndoMark(w->app);
}

void CbCmRemoveEmptyNodes(Widget wg,View w,void* pcbs) {
  Node n;
  Index ix;

  if (w->app==NULL) return;
  SetActiveView(w);

  for (n=AppNode1st(w->app,&ix);n!=NULL;n=Next(&ix))
    if (IsEmptyNode(n)) DelNode(w->app,n);

  UndoMark(w->app);
}

void CbCmInstallSeparators(Widget wg,View w,void* pcbs) {
  int i;

  if (w->app==NULL) return;
  SetActiveView(w);

  SetViewFlags(w,w->showFlags | SHW_SEPARATORS);
  i=InstallSeparators(w->app);
  if (i) {
    Cancel(w->app);
    ErrorBox(w->x->wMain,GetStr(w,i));
  } else
    UndoMark(w->app);
}

void CbCmRemoveSeparators(Widget wg,View w,void* pcbs) {

  if (w->app==NULL) return;
  SetActiveView(w);

  RemoveAllSeparators(w->app);
  UndoMark(w->app);
}

void CbCmShowInvalidVars(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  if (!CheckAllVars(w->app,NULL)) ViewMsgEx(w,MSG_ALLVARSVALID,NULL);
  else {
    ViewMsgEx(w,MSG_SOMEVARSINVALID,NULL);
    OpenInvalidVarsDlg(w);
  }
}

void CbCmFindXPoints(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  SetActiveView(w);

  LockAppUpdate(w->app,1);
  DetectXPoints(w->app);
  LockAppUpdate(w->app,-1);

  UndoMark(w->app);
}

void CbCmFindXPointSegs(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;

  SetActiveView(w);

  LockAppUpdate(w->app,1);
  DetectXPointSegs(w->app);
  LockAppUpdate(w->app,-1);

  UndoMark(w->app);
}

void CmRebuildCarreInfo(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View)xtpV;
  int r;

  SetActiveView(w);

  r=RebuildCarreSurfaces(w->app,-1);
  if (r) {
    Cancel(w->app);
    ErrorBox(w->x->wMain,GetStr(w,r));
    return;
  }

  r=RebuildCarreGridPoints(w->app,-1);
  if (r) {
    Cancel(w->app);
    ErrorBox(w->x->wMain,GetStr(w,r));
    return;
  }

  UndoMark(w->app);
}


void CbAddVarSet(Widget wg,VarSetDef vsd,void* pcbs) {
  View w=NULL;
  VarSet vs;
  Widget wDlg;

  GetValues(XtParent(wg),XmNuserData,&w,NULL);
  ValidatePtr(w,"CbAddVarSet.view");
  if (w->app==NULL) return;
  SetActiveView(w);

  vs=AddVarSet(w->app,vsd);

  UndoMark(w->app);

  wDlg=OpenVarsEditDlg(w,vs);
}

void CbDelVarSet(Widget wg,VarSet vs,void* pcbs) {
  View w=NULL;

  GetValues(XtParent(wg),XmNuserData,&w,NULL);
  ValidatePtr(w,"CbDelVarSet.view");
  if (w->app==NULL) return;
  SetActiveView(w);

  DelVarSet(w->app,vs);
  UndoMark(w->app);
}

void CbEditVarSet(Widget wg,VarSet vs,void* pcbs) {
  View w=NULL;
  Widget wDlg;

  GetValues(XtParent(wg),XmNuserData,&w,NULL);
  ValidatePtr(w,"CbDelVarSet.view");
  if (w->app==NULL) return;
  SetActiveView(w);

  wDlg=OpenVarsEditDlg(w,vs);
}

/* Option menu commands
*/

void CmSetOutputModeX(Widget wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View)xtpView;

  SetActiveView(w);

  OpenOutputModeDlg(w);
}

void CmSetMeshOptions(Widget wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View)xtpView;

  SetActiveView(w);

  OpenMeshOptionsDlg(w);
}

void CbOptionsMenuToggle(Widget wg,XtPointer xtpView,void* pcbs) {
  View w=(View)xtpView;

  XmToggleButtonSetState(w->x->wSwMenu,
    !XmToggleButtonGetState(w->x->wSwMenu),True);
}

void CbSwMenu(Widget wg,XtPointer xtpView,void* pcbs) {
  View w=(View)xtpView;
  Widget wS1;
  int state;

  SetActiveView(w);
  state=XmToggleButtonGetState(w->x->wSwMenu);

  if (state) {
    if (XtIsManaged(w->x->wMenu)) return;

    XtUnmanageChild(w->x->wBnShowMenu);

    GetValues(w->x->wMain,XmNmenuBar,&wS1,NULL);

    XtManageChild(w->x->wMenu);
    SetValues(w->x->wMain,XmNmenuBar,w->x->wMenu,NULL);

    XtDestroyWidget(wS1);
  } else {
    if (!XtIsManaged(w->x->wMenu)) return;

    wS1=XtCreateManagedWidget("dummy",xmManagerWidgetClass,w->x->wMain,
      NULL,0);
    SetValues(w->x->wMain,XmNmenuBar,wS1,NULL);

    XtUnmanageChild(w->x->wMenu);

    if (w->x->wBnShowMenu==NULL) {
      w->x->wBnShowMenu=Cmw(XmCreatePushButton,w->x->wDraw,"menuOn",
	NULL);
      XtAddCallback(w->x->wBnShowMenu,XmNactivateCallback,
	(XtCallbackProc)CbOptionsMenuToggle,w);
    }

    XtManageChild(w->x->wBnShowMenu);
  }
}

void CbSwToolbar(Widget wg,XtPointer xtpView,void* pcbs) {
  View w=(View)xtpView;
  Widget wS1;
  int state;

  SetActiveView(w);
  state=XmToggleButtonGetState(w->x->wSwToolBar);

  if (state) {
    if (XtIsManaged(w->x->wCommand)) return;

    GetValues(w->x->wMain,XmNcommandWindow,&wS1,NULL);

    XtManageChild(w->x->wCommand);
    SetValues(w->x->wMain,XmNcommandWindow,w->x->wCommand,NULL);

    XtDestroyWidget(wS1);
  } else {
    if (!XtIsManaged(w->x->wCommand)) return;

    wS1=XtCreateManagedWidget("dummy",xmManagerWidgetClass,w->x->wMain,
      NULL,0);
    SetValues(w->x->wMain,XmNcommandWindow,wS1,NULL);

    XtUnmanageChild(w->x->wCommand);
  }
}

void CbSwManualRefresh(Widget wg,XtPointer xtpView,void* pcbs) {
  View w=(View)xtpView;
  int state;

  state=XmToggleButtonGetState(w->x->wSwManualRefresh);
  if (state) {
    XtManageChild(w->x->wCmRefresh);
    XtManageChild(w->x->wSepRefresh);
  } else {
    XtUnmanageChild(w->x->wCmRefresh);
    XtUnmanageChild(w->x->wSepRefresh);
    CbViewRefresh(wg,xtpView,pcbs);
  }
}


/* Window menu commands
*/

void CbWindowToolBar(Widget wg,XtPointer arg,XtPointer pcbs) {
  View w=(View)arg;

  if (w->x->wToolBar==NULL) {
    w->x->wToolBar=OpenToolBarDlg(w);
    XtManageChild(w->x->wToolBar);
    NotifyNewTool(w);
  } else {
    XtPopup(XtParent(w->x->wToolBar),XtGrabNone);
  }
  XmToggleButtonSetState(w->x->wSwToolBar,False,True);
}

void CbWindowOpen(Widget wg,View w,void* pcbs) {
  View w1;

  if (w->app==NULL) return;
  SetActiveView(w);

  w1=CreateXmView(w->xapp,w->app);
  UndoMark(w->app);
}

/* This is also the WM_DELETE_WINDOW protocol callback
*/
void CbWindowClose(Widget wg,View w,void* pcbs) {

  SetActiveView(w);
  CloseXmView(w,True);
}

void CbAbout(Widget wg,View w,void* pcbs) {
  Widget wDlg;

  if (w->app==NULL) return;
  SetActiveView(w);

  wDlg=OpenAboutDlg(w);
  ProcessModalDialog(wDlg);
}

void CbWindowInfo(Widget wg,View w,void* pcbs) {
  if (w->app==NULL) return;
  SetActiveView(w);

  OpenInfoDlg(w);
}

void CbHelpMenu(Widget wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View) xtpView;
  String s;

  s=GetResourceString(wg,"helpToDisplay","HelpToDisplay",NULL);
  if (s==NULL) return;

  DisplayHelpTopic(w,s);
}


/* Test menu item //////////////////////////////////////////////////// */

#define PTS 100

void CmTest(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  View w=(View) xtpV;
/*  double* x,* y,* t,* b,* c,* d,* b1,* c1,* d1; */
  double x1,y1,x2,y2,t2;
  int i,j,n;
  Source src,src1;
  Surface s;
/*  Index ix;
  XY xy; */
  ParamSplineInfo si;

  SetActiveView(w);
  
  s=AppSurface1st(w->app,NULL);assert(s!=NULL);
  
  si=CreateParamSplineInfo(s->line);
  assert(si!=NULL);


  SetViewMode(w,VMX_CHORD);
  
  SetViewMode(w,VM1_CHORD);
  n=GroupCount(s->line)*PTS;
  for (i=0;i<n;i++) {
    CalcParamSplineValue(si,si->tMin+(si->tMax-si->tMin)*i/n,&x2,&y2);
    if (i>0) DrawViewLine(w,x1,y1,x2,y2);
    x1=x2;
    y1=y2;
  }
  
  si=FreeParamSplineInfo(si);
}
