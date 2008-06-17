/* X dialog management routines
*/

#ifndef _x_dlg_h
#define _x_dlg_h

/* Redirections to avoid default processing in vacreate */

#define ErrorBox X_Dlg_ErrorBox
#define CbFree X_Dlg_CbFree

void ProcessPending(Widget wg);
void ProcessModalDialog(Widget wShell);
void MessageBox(Widget w,char* name);
void ErrorBox(Widget w,char* name);
int WarningBox(Widget w,char* name);
int QuestionBox(Widget w,char* name);

void CbFree(Widget wg,XtPointer p,XtPointer pcbs);

Widget CreateMessageDlg(Widget wParent,String name,String string);
void SetLabelString(Widget w,char* string);

/*
void CbFree(Widget wg,XtPointer arg,XtPointer pcbs);
void CbCancel(Widget wg,XtPointer arg,XtPointer pcbs);
void CbDestroy(Widget wg,XtPointer arg,XtPointer pcbs);
void CbUnmap(Widget wg,XtPointer arg,XtPointer pcbs);
*/

#endif
