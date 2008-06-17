/* XApp - Motif toolkit version
*/

#ifndef _x_xapp_h
#define _x_xapp_h

struct _XAppExt {
/*Implementation-specific part
*/
  XtAppContext appContext;
  Widget wShell;
  Atom wm_delete_window;
  int resourceFileVersion;
  int bDisplayHelp,bCheckResources,bCheckHelpFile;
  int xViewShellsCount; /* Prevent problems in latent callbacks */
  String helpFile;
  int bPrefsLocked;
  struct _StringSource strs;
};

char* GetUserPrefsString(XApp xa,char* name);
int GetUserPrefsInt(XApp xa,char* name);
int SetUserPrefsString(XApp xa,char* str,char* value);
int SetUserPrefsInt(XApp xa,char* name,int value);

char* GetUserPrefsFileName(XApp xap);
int LockUserPrefsFile(XApp xa);
void UnlockUserPrefsFile(XApp xa);

Stack GetRecentFilesStack(XApp xap);
void AddXAppRecentFile(XApp xap,char* fileName);

View FindViewByFilename(XApp xap,char* fileName);

#endif
