#include "x_dg.h"

#define MDLG_SCROLLMIN 10

void ProcessModalDialog(Widget w) {
  XEvent xev;
  Widget wShell;
  static int level=0;

  XtManageChild(w);
  for (wShell=w;!XtIsShell(wShell);wShell=XtParent(wShell));
  XtAddGrab(wShell,True,False);

  while (XtIsManaged(w)) {
    XtAppNextEvent(XtWidgetToApplicationContext(wShell),&xev);
    XtDispatchEvent(&xev);
  }
  XtDestroyWidget(wShell);
}

Widget CreateMessageDlg(Widget wParent,String name,String string) {
  Widget wDlg,wScroll,wLabel;
  int i,j;
  char* s;
  XmString xms;

  wDlg=Cw(XmCreateMessageDialog,wParent,name,
      NULL);
  XtUnmanageChild(XtNameToWidget(wDlg,"Help"));
  XtUnmanageChild(XtNameToWidget(wDlg,"Cancel"));
  xms=MakeXmString(string);

  for (j=0,s=string;*s;s++) if (*s=='\n') j++;
  if (j<MDLG_SCROLLMIN) {
    SetValues(wDlg,XmNmessageString,xms,NULL);
  } else {
    wScroll=Cmw(XmCreateScrolledWindow,wDlg,"scroll",
      NULL);
    wLabel=Cmw(XmCreateLabel,wScroll,"label",
      XmNlabelString,xms,
      NULL);
  }
  XmStringFree(xms);

  return wDlg;
}

void ErrorBox(Widget w,char* name) {
  Widget wg;
  XmString xms;

  wg=CreateMessageDlg(w,"errBox",name);
  ProcessModalDialog(wg);
}

void MessageBox(Widget w,char* name) {
  Widget wg;
  XmString xms;

  wg=CreateMessageDlg(w,"messageBox",name);
  ProcessModalDialog(wg);
}

static void CbStoreTrue(Widget wg,XtPointer arg,XtPointer pcbs) {
  *(Boolean*)arg=True;
}
/*
void SetLabelString(Widget w,char* string) {
  XmString xms;

  xms=MakeXmString(string);
  SetValues(w,XmNlabelString,xms,NULL);
  XmStringFree(xms);
} */

int QuestionBox(Widget w,char* name) {
  Widget wg;
  XmString xms;
  Boolean b=False;

  wg=CreateMessageDlg(w,"questionBox",name);
  XtAddCallback(wg,XmNokCallback,CbStoreTrue,&b);
  XtManageChild(XtNameToWidget(wg,"Cancel"));
  ProcessModalDialog(wg);
  return !b;
}

int WarningBox(Widget w,char* name) {
  Widget wg;
  XmString xms;
  Boolean b=False;

  wg=CreateMessageDlg(w,"warningBox",name);
  XtAddCallback(wg,XmNokCallback,CbStoreTrue,&b);
  XtManageChild(XtNameToWidget(wg,"Cancel"));
  ProcessModalDialog(wg);
  return !b;
}

void CbFree(Widget wg,XtPointer p,XtPointer pcbs) {
  Free((void*)p);
}
