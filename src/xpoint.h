/* Separatrix & grid points data types&functions
*/

#ifndef _xpoint_h
#define _xpoint_h

/* Separatrix areas
*/
#define SPA_LOOP    0
#define SPA_TARGET1 1
#define SPA_TARGET2 2
#define SPA_OUTER   3

struct _XPointMinMax {
  int x,y,t;
  double lvl;
};

struct _XPoint {
  int type,locks;
  double x1,y1,x2,y2;
  int lpx,lpy;
  Group line[3];
  double lineLen[3];
};

struct _XPointTest {
  int type,locks;
  App app;
  int cx1,cy1,cx2,cy2;
  double lvlMin,lvlMax,level,centerX,centerY;
  struct _XPointMinMax minMax[4];
  Group segs;
  Group gradients[4];
};

struct _XPointSeg {
  int type,locks;
  XPointTest xpt;
  int number,zone,dir;
  char* zoneName;
  double startPos,endPos;
  double lineLen;
  Group line,gridPoints;
};

struct _GridPoint {
  int type,locks;
  int area;
  double value,x,y,dx,dy;
  char* creatorId;
};

int AddXPoint(App a,double x1,double y1,double x2,double y2);
void DelXPoint(App a);

int FindSurfaceOriginPoint(App a,int area,double level,
    double* px,double* py);

#define FindEquilPeakPoint(a,px,py) \
    FindSurfaceOriginPoint((a),1,MAXDOUBLE,(px),(py))

#define GridPointCreatorIdUnchanged(id) (*(id)==CID_UNCHANGEDFLAG)

int FindLoopPoint(App a);

GridPoint AddGridPoint(App a,int area,double value);
void* DelGridPoint(App a,GridPoint gp);
void ChangeGridPoint(App a,GridPoint gp,int area,double value);

int ChangeGridPointCreatorId(App a,GridPoint gp,char* id);
char* GetGridPointCreatorId(GridPoint gp);

int DistributeGridPoints(App a,int area,int count,double a1,double a2,
    int law,int carreMode);
int RebuildCarreGridPoints(App a,int area);

char* ConstructGridPointCreatorId(int zone,int count,double alpha1,double
    alpha2,int law,int carreFlag);
int ParseGridPointCreatorId(char* id,int* pZone,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,int* pCarreFlag);

XPointTest AddXPointTest(App a,int cx1,int cy1,int cx2,int cy2,double level);
void* DelXPointTest(App a,XPointTest xpt);

XPointSeg AddXPointSeg(App a,XPointTest xpt,int number,double sp,double ep);
int ChangeXPointSeg(App a,XPointSeg xps,int dir,int zone,char* zoneName);
void* DelXPointSeg(App a,XPointSeg xps);

/* Private functions
*/
int CalcXPointLine(Equil eq,XPoint xpt,int cx1,int cy1,int cx2,int cy2,
    Group t1,Group t2);
void CalcGridPointCoords(XPoint xp,GridPoint gp);

int DetectXPoints(App a);
int DetectXPointSegs(App a);


#endif
