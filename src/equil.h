/* Equilibrium data structures & functions
*/

#ifndef _equil_h
#define _equil_h

struct _Equil {
  int type,locks;
  App app;
  char fName[DG_FNAME_LEN];
  int sx,sy,signInside;
  double* x,*y,*val;
  double minVal,maxVal;
  SimpleSplineInfo* hSplines,* vSplines;
};

struct _Surface {
  int type,locks;
  double originX,originY,level;
  char* creatorId;

  int closed;
  Group line;
};

struct _SurfCell {
  double x[4],y[4];
  int n,f;
  int d[4];
};

#define CID_UNCHANGEDFLAG '+'

#define EqCell(eq,x,y) ((eq)->val[(eq)->sx*(y)+(x)])
#define SurfaceLine1st(s,ix) Group1st((s)->line,(ix))

#define EqAverageCellWidth(eq) \
  (fabs((eq)->x[(eq)->sx-1]-(eq)->x[0])/(eq)->sx)

#define EqAverageCellHeight(eq) \
  (fabs((eq)->y[(eq)->sy-1]-(eq)->y[0])/(eq)->sy)

#define SurfaceCreatorIdUnchanged(id) (*(id)==CID_UNCHANGEDFLAG)

double EqCorrCell(Equil eq,int cx,int cy,double level);
int AddEquil(App a,char* fName);
int DelEquil(App a);
Surface AddSurface(App a,double x,double y,int* pErr);
Surface AddSurfaceByLevel(App a,int area,double level,int* pErr);
void* DelSurface(App a,Surface s);
int ChangeSurface(App a,Surface s,double x,double y);
int ChangeSurfaceCreatorId(App a,Surface s,char* creatorId);

int CalcEquilSplines(Equil eq);
int FreeEquilSplines(Equil eq);

int GetEquilCell(Equil eq,double x,double y,int* px,int* py);
int GetEquilLevel(Equil eq,double x,double y,double* pLevel,
    double* pGradX,double* pGradY);
int GetSurfaceArea(App a,Surface s);
char* GetSurfaceCreatorId(Surface s);


char* ConstructSurfaceCreatorId(int area,int count,double alpha1,
    double alpha2,int law,double l1,double l2,int carreFlag);
int ParseSurfaceCreatorId(char* id,int* pArea,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,double* pL1,double* pL2,int* pCarreFlag);

int FindCarreMinMaxSurfaceLevel(App a,int nArea,double* pL1,double* pL2);
int DistributeSurfaces(App a,int area,int count,double a1,double a2,
    int law,double level1,double level2,int carreMode,char** pCreatorId);

int RebuildCarreSurfaces(App a,int area);

int WriteDgEquil(Equil eq,char* fName,int sx,int sy);

/* Private functions - only for Act_xxx
*/
int LoadDgEquilFile(Equil eq);
int Load1EquilFile(Equil eq);
void CalcEquilValues(Equil eq);
void CalcSurfData(Equil eq,int cx,int cy,double level,struct _SurfCell* sc);
int CalcSurfaceLine(Equil eq,int cx,int cy,double level,Group* gXY);
int CheckSurfaceTargetIntersection(Group gXY,Group target);

#endif
