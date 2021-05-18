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
  SurfaceSplineInfo sspline;
  int cacheSx,cacheSy;
  double* cache;
};

struct _SurfCell {
  double x[4],y[4];
  int n,f;
  int d[4];
};

#define CID_UNCHANGEDFLAG '+'

#define EqCell(eq,x,y) ((eq)->val[(eq)->sx*(y)+(x)])

#define EqAverageCellWidth(eq) \
  (fabs((eq)->x[(eq)->sx-1]-(eq)->x[0])/(eq)->sx)

#define EqAverageCellHeight(eq) \
  (fabs((eq)->y[(eq)->sy-1]-(eq)->y[0])/(eq)->sy)

#define SurfaceCreatorIdUnchanged(id) (*(id)==CID_UNCHANGEDFLAG)

#define EquilSplinesOk(eq) ((eq)->hSplines!=NULL && \
                           (eq)->vSplines!=NULL && (eq)->sspline!=NULL)

/* Will be used in future for spline-processed equilibria */

#define EqSX(eq) ((eq)->sx)
#define EqSY(eq) ((eq)->sy)

#define EqX(eq,ix) ((eq)->x[(ix)])
#define EqY(eq,iy) ((eq)->y[(iy)])

double EqCorrCell(Equil eq,int cx,int cy,double level);
double EqCellEx(Equil eq,int cx,int cy,int sx,int sy);
double EqCorrCellEx(Equil eq,int cx,int cy,double level,int sx,int sy);
int AddEquil(App a,char* fName);
int DelEquil(App a);

int CalcEquilSplines(Equil eq);
int FreeEquilSplines(Equil eq);

int GetEquilCell(Equil eq,double x,double y,int* px,int* py);
int GetEquilCellEx(Equil eq,double x,double y,int* px,int* py,int sx,int sy);
int GetEquilLevel(Equil eq,double x,double y,double* pLevel,
    double* pGradX,double* pGradY);

void AllocEquilCache(Equil eq,int sx,int sy);
void FreeEquilCache(Equil eq);

char* ConstructSurfaceCreatorId(int area,int count,double alpha1,
    double alpha2,int law,double l1,double l2,int carreFlag);
int ParseSurfaceCreatorId(char* id,int* pArea,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,double* pL1,double* pL2,int* pCarreFlag);

int FindCarreMinMaxSurfaceLevel(App a,int nArea,double* pL1,double* pL2,
    int* bOuterSurfaceNeeded);
int DistributeSurfaces(App a,int area,int count,double a1,double a2,
    int law,double level1,double level2,int carreMode,char** pCreatorId);

int RebuildCarreSurfaces(App a,int area);

int WriteDgEquil(Equil eq,char* fName,int sx,int sy);
int WriteSplineEquil(Equil eq,char* fName,int sx,int sy);

/* Private functions - only for Act_xxx
*/
int LoadDgEquilFile(Equil eq);
int Load1EquilFile(Equil eq);
void CalcEquilValues(Equil eq);
void CalcSurfData(Equil eq,int cx,int cy,double level,struct _SurfCell* sc,
    int sx,int sy);
int CalcSurfaceLine(Equil eq,int cx,int cy,double level,Group* gXY,int sx,int sy);
int CheckSurfaceTargetIntersection(Group gXY,Group target);

#endif
