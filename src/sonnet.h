/* Structures & functions for processing SONNET output and for
   managing separators
*/

#ifndef _sonnet_h
#define _sonnet_h

struct _SonnetData {
  int type,locks;
  App app;
  char fName[DG_FNAME_LEN];
  int sx,sy;
  XY points;
};

struct _Separator {
  int type,locks;
  double x,y;
  Node n;
  Group vars;
  int id;
};

typedef Group* CellsInfo;

#define SeparatorVar1st(sep,ix) Group1st((sep)->vars,(ix))

int AddSonnetData(App a,char* fName);
int DelSonnetData(App a);

Separator AddSeparator(App a,double x,double y,Node n);
void* DelSeparator(App a,Separator sep);
void ChangeSeparator(App a,Separator sep,Node n);
void ChangeSeparatorId(App a,Separator sep,int id);

int InstallSeparators(App a);
void RemoveAllSeparators(App a);

CellsInfo CreateCellsInfo(App a,int* pErr,void** pObj);
void* FreeCellsInfo(CellsInfo ci);

/* Private functions - only for ActXX()
*/
int LoadSonnetFile(SonnetData sd);
int CountSonnetDataPoints(SonnetData sd);

#endif
