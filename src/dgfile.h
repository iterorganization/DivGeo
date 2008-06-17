/* DgFile I/O functions
*/

#ifndef _dgfile_h
#define _dgfile_h

/* Error flags for LoadApp
*/

#define DGFE_NOEQUIL  0x0001
#define DGFE_NOTEMPL  0x0002
#define DGFE_BADTYPE  0x0004
#define DGFE_SYNTAX   0x0008
#define DGFE_NOSONNET 0x0010
#define DGFE_BADTARG1 0x0020
#define DGFE_BADTARG2 0x0040
#define DGFE_BADSTRUC 0x0080
#define DGFE_BADXPT   0x0080
#define DGFE_MESH_FP  0x0100

#define DGFM_APP      1
#define DGFM_CONFIG   2
#define DGFM_TMP      3

App LoadNormalApp(XApp xap,char* fName,char** pMsg,int* err);
App LoadApp(XApp xap,char* fName,char** pMsg,int* err);
int SaveApp(App a,char* fName,int mode);

char* GetLoadErrFlagsDescription(XApp xap,int errFlags);

/* Private functions
*/
int ReadOldDgFile(App a,char* fName,int* errFlags);

#endif
