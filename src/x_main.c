/* Motif version
*/

#include "x_dg.h"

#define ENV_NOCHECK          "DG_NO_OUTPUT_CHECK"

XApp xap;
App a;
View w=NULL;

main(int argc,char** argv) {
  int i,err;
  char* s;
  Widget wNewPrefsDlg;

  xap=CreateXApp(&argc,argv);

  a=AddApp(xap);
  if (a==NULL) {
    fprintf(stderr,"Error opening configuration file");
    exit(1);
  }

  if (argc<=1) {
    w=CreateXmView(xap,a);
    FreeUndoInfo(a);
  } else {
    a=FreeApp(a);
    for (i=1;i<argc;i++) {
      a=LoadNormalApp(xap,argv[i],&s,&err);
      if (err!=0) {
	fprintf(stderr,"%s:\n",argv[i]);
	fprintf(stderr,"%s\n",XAppStr(xap,err));
	exit(1);
      }
      if (s!=NULL) {
	puts(s);                        /* relcheck_ignore_line */
        s=Free(s);
      }
      w=CreateXmView(xap,a);
      FreeUndoInfo(a);
      if (a!=NULL) AddXAppRecentFile(xap,a->fName);
    }
  }

  if (w!=NULL) { /* Always true */
    wNewPrefsDlg=OpenNewPrefsFileDlg(w,True);
    OpenNewVersionDlg(w,True);
    if (wNewPrefsDlg!=NULL) XtPopup(XtParent(wNewPrefsDlg),XtGrabNone);

    if (getenv(ENV_NOCHECK)!=NULL) ErrorBox(w->x->wMain,
      GetResourceString(w->x->wMain,"errEnvNoCheckObsolete",NULL,NULL));
  }

  XtAppMainLoop(xap->x->appContext);
}
