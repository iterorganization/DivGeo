/* Output file creation functions
*/

#ifndef _dgoutput_h
#define _dgoutput_h

/* Application flags - show validity of the (possible) output data
*/

#define AF_VALIDVARS       0x0001
#define AF_VALIDTARGETS    0x0002
#define AF_VALIDSTRUCTURE  0x0004
#define AF_VALIDSURFACES   0x0008
#define AF_VALIDGRIDPOINTS 0x0010
#define AF_VALIDCELLS      0x0020

int GetAppFlags(App a);
int WriteOutputFile(App a,char* fName);
int WriteTargetsFile(App a,char* fName);
int WriteStructureFile(App a,char* fName);

int CheckVar(App a,Group* pgObjects,VarDef vd,VarSet vs,int* pErr);
int CheckAllVars(App a,Group* pgErrorPairs);

int SurfaceZoneSortProc(void* p1,void* p2,void* arg);
int GridPointSegSortProc(void* p1,void* p2,void* arg);

#endif
