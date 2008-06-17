#include "x_dg.h"

#define DLG_HELPSYSTEM "dlgHelpSystem"
#define SHELL_HELPSYSTEM "helpSystemShell"

typedef struct _HelpSystemDlg {
  Widget wDlg,wText,wSwNoPopup;
  int bPoppedUp;
}* HelpSystemDlg;

static void CbHelpSystemPopdown(Widget,XtPointer,XtPointer);

void DisplayHelpText(View w,String text,size_t offset) {
  HelpSystemDlg dlg;
  Widget wShell,wDlg,wg;
  XtPointer xtp;

  wDlg=XtNameToWidget(w->xapp->x->wShell,"*"DLG_HELPSYSTEM);
  if (wDlg==NULL) {
    dlg=Malloc(sizeof(*dlg));

    wShell=XtCreatePopupShell(SHELL_HELPSYSTEM,
      topLevelShellWidgetClass,w->xapp->x->wShell,NULL,0);
    SetValues(wShell,XmNdeleteResponse,XmDO_NOTHING,NULL);
    XmAddWMProtocolCallback(wShell,w->xapp->x->wm_delete_window,
      CbUnmap,NULL);
    XtAddCallback(wShell,XmNpopdownCallback,CbHelpSystemPopdown,(XtPointer)dlg);
    XtAddCallback(wShell,XmNdestroyCallback,CbFree,(XtPointer)dlg);

    dlg->wDlg=wDlg=Cmw(XmCreateMainWindow,wShell,DLG_HELPSYSTEM,
      XmNuserData,(XtPointer)dlg,
      NULL,0);

    wg=Cmw(XmCreateMenuBar,wDlg,"helpMainMenu",
      NULL,0);
    SetValues(wDlg,XmNmenuBar,wg,NULL);

    CreateMenuSystem(wg,
      "c:window",
      "+:helpWindowMenu",
      "bA:close",CbUnmap,wShell,
      "s:separator",
      "t?:noPopup",&dlg->wSwNoPopup,
      "-:",
    NULL);

    dlg->wText=Cmw(XmCreateScrolledText,wDlg,"text",
      XmNeditMode,XmMULTI_LINE_EDIT,
      XmNeditable,False,
      XmNtraversalOn,False,
      XmNnavigationType,XmNONE,
    NULL);
  } else {
    GetValues(wDlg,XmNuserData,&xtp,NULL);
    dlg=(HelpSystemDlg)xtp;
  }

  if (text!=NULL) {
    XmTextSetString(dlg->wText,text);
    XmTextSetTopCharacter(dlg->wText,offset);
    XmTextSetInsertionPosition(dlg->wText,offset);
  }

  XtRealizeWidget(XtParent(wDlg));

  if (dlg->bPoppedUp && XmToggleButtonGetState(dlg->wSwNoPopup)) return;

  dlg->bPoppedUp=True;
  XtPopup(XtParent(wDlg),XtGrabNone);
  XMapWindow(XtDisplay(XtParent(wDlg)),XtWindow(XtParent(wDlg)));
}

static void CbHelpSystemPopdown(Widget wg,XtPointer xtpDlg,XtPointer pcbs) {
  HelpSystemDlg dlg=(HelpSystemDlg)xtpDlg;

  dlg->bPoppedUp=False;
}

void DisplayHelpTitle(View w,String title) {
  Widget wDlg;

  DisplayHelpText(w,NULL,0);

  wDlg=XtNameToWidget(w->xapp->x->wShell,"*"DLG_HELPSYSTEM);
  assert(wDlg!=NULL);

  SetValues(XtParent(wDlg),XmNtitle,title,NULL);
}

void DisplayHelpTopic(View w,String topic) {
  String s,s1;
  char buf[1024];

  assert(w!=NULL);
  assert(topic!=NULL);

  DisplayHelpTitle(w,topic);

  if (w->xapp->x->helpFile==NULL) {
    DisplayHelpText(w,GetStr(w,ERR_NOHELP),0);
    return;
  }

  strcpy(buf,"\n");
  strcat(buf,topic);
  strcat(buf,"\n");

  s=strstr(w->xapp->x->helpFile,buf);

  if (s==NULL)
    DisplayHelpText(w,GetStr(w,ERR_NOTOPIC),0);
  else {
    s1=strstr(s,"\n---\n");
    if (s1!=NULL) *s1=0;
    DisplayHelpText(w,s+strlen(buf),0);
    if (s1!=NULL) *s1='\n';
  }
}
