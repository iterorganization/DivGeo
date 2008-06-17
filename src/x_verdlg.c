#include "x_dg.h"

#define DLG_NEWPREFSFILE "dlgNewPrefsFile"
#define DLG_NEWVERSION "dlgNewVersion"

#define PSTR_PREFSPRESENT "DG.Signature"
#define PSTR_DGVERSION "DG.LastUsedVersion"
#define PVAL_PREFSPRESENT "XDivGeo"


Widget OpenNewPrefsFileDlg(View w,int bUsePrefsFile) {
  Widget wDlg;
  char* s;

  if (bUsePrefsFile) {
    s=GetUserPrefsString(w->xapp,PSTR_PREFSPRESENT);
    if (!strcmp(s,PVAL_PREFSPRESENT)) return NULL;

    LockUserPrefsFile(w->xapp);
    SetUserPrefsString(w->xapp,PSTR_PREFSPRESENT,PVAL_PREFSPRESENT);
    UnlockUserPrefsFile(w->xapp);

    s=GetUserPrefsString(w->xapp,PSTR_PREFSPRESENT);
    if (strcmp(s,PVAL_PREFSPRESENT)) {
      fprintf(stderr,"Failure to initialize the preferences file '%s'\n",
	  GetUserPrefsFileName(w->xapp));
      exit(1);
    }
  }

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_NEWPREFSFILE);
  if (wDlg==NULL) {
    wDlg=CreateDialogEx(w->x->wMain,DLG_NEWPREFSFILE,NULL,
      CDLG_MESSAGE|CDLG_NOCANCEL);
    XtAddCallback(wDlg,XmNokCallback,CbUnmap,NULL);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
    SetMessageString(wDlg,GetResourceStringEx(wDlg,"messageStringEx",
      NULL,"$(PATH)%s",GetUserPrefsFileName(w->xapp)));
    XtManageChild(wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

static void CbNewVersionOk(Widget wg,XtPointer xtpV,XtPointer pcbs);

Widget OpenNewVersionDlg(View w,int bOnlyIfNeeded) {
  Widget wDlg;
  char buf[100];
  char* s;
  int i,lastVersion=0,newVersion=0;


  if (bOnlyIfNeeded) {
    newVersion=DG_VERSION;
    if (newVersion%10)
      fprintf(stderr,"OpenNewVersionDlg:Interim version\n");

    sprintf(buf,"%d",newVersion);

    s=GetUserPrefsString(w->xapp,PSTR_DGVERSION);
    i=sscanf(s,"%d",&lastVersion);
    if (i!=1) lastVersion=0;
    LockUserPrefsFile(w->xapp);
    SetUserPrefsString(w->xapp,PSTR_DGVERSION,buf);
    UnlockUserPrefsFile(w->xapp);

    if (!strcmp(s,buf)) return;
  }

  wDlg=XtNameToWidget(w->x->wMain,"*"DLG_NEWVERSION);
  if (wDlg==NULL) {
    wDlg=CreateDialogEx(w->x->wMain,DLG_NEWVERSION,NULL,
      CDLG_MESSAGE);
    XtAddCallback(wDlg,XmNokCallback,CbNewVersionOk,(XtPointer)w);
    XtAddCallback(wDlg,XmNhelpCallback,CbHelp,(XtPointer)w);
    SetMessageString(wDlg,GetResourceStringEx(wDlg,"messageStringEx",
	NULL,"$(OLDVERSION)%s$(VERSION)%s",
	GetVersionStr(lastVersion),GetVersionStr(newVersion)));
    XtManageChild(wDlg);
  } else XtPopup(XtParent(wDlg),XtGrabNone);

  return wDlg;
}

static void CbNewVersionOk(Widget wg,XtPointer xtpV,XtPointer pcbs) {
  CbHelpMenu(wg,xtpV,pcbs);
  XtPopdown(XtParent(wg));
}
