/* Separatrix & grid points data types&functions
*/

#ifndef _xpoint_h
#define _xpoint_h

#define GPZ_NONE    -65537 /* DO NOT CHANGE! Used in DG files! */
#define SZN_XY      -65536 /* DO NOT CHANGE! Used in DG files! */

#define XPS_ANGLE_BAD 65536.0 /* DO NOT CHANGE! */

#define ViewShowTopology(w) \
    (((w)->showFlags & SHWM_XPOINT) || ((w)->bEditTopology))

struct _XPointMinMax {
  int x,y,t;
  double lvl;
};

struct _XPointTest {
  int type,locks;
  App app;
  int cx1,cy1,cx2,cy2;
  double lvlMin,lvlMax,level,centerX,centerY;
  struct _XPointMinMax minMax[4];
  Group segs;
  Group gradients[4];

  int id;
};

struct _XPointSeg {
  int type,locks;
  XPointTest xpt;
  int number;
  double startPos,endPos;
  double lineLen;
  Group line;
};

struct _GridPointSeg { /* $ TEMP - non-undoable */
  int type,locks;
  App app;
  int zone,dir,targetDir/*Kill*/;
  char* shortName,* longName;
  XPointSeg xps;
  Group line;
  long flags;
  double lineLength,level;
};

#define GPSF_USED      0x0001
#define GPSF_TARGET_CW 0x0002

struct _SurfaceZone {
  int type,locks;
  App app;
  int zone;
  char* shortName,* longName;
  int gpZone1,gpZone2,sign,orient;
  long flags;
  double level1,level2;
  Group bounds;
  Elem innermost;
};

/* Orientation: +1 to the left  -1 to the right */

#define SZF_LIMITBYSURFACE 0x0001

struct _GridPointEx {
  int type,locks;
  App app;
  int zone,bDrawn,bCoordsOk;
  double value;
  double x,y,dx,dy;
  char* creatorId;
};


struct _SurfaceEx {
  int type,locks;
  App app;
  int zone,bDrawn,bCoordsOk;
  double originX,originY,level,nLevel;
  char* creatorId;
  int lastError;  /* NOT managed via Undo */

  int closed;
  Group line;
};

#define GridPointCreatorIdUnchanged(id) (*(id)==CID_UNCHANGEDFLAG)

int DistributeGridPoints(App a,int area,int count,double a1,double a2,
    int law,int carreMode);
int RebuildCarreGridPoints(App a,int area);

char* ConstructGridPointCreatorId(int zone,int count,double alpha1,double
    alpha2,int law,int carreFlag);
int ParseGridPointCreatorId(char* id,int* pZone,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,int* pCarreFlag);

XPointTest AddXPointTest(App a,int cx1,int cy1,int cx2,int cy2,double level,int id);
void* DelXPointTest(App a,XPointTest xpt);
XPointTest FindXPointTest(App a,int id);

#define SetXPointTestId(a,xpt,__id) SetObjInt(a,(xpt),\
  GetOffset(XPointTest,id), sizeof(int),(__id),1)

XPointSeg AddXPointSeg(App a,XPointTest xpt,int number,double sp,double ep);
/*int ChangeXPointSeg(App a,XPointSeg xps,int dir,int zone,char* zoneName); */
void* DelXPointSeg(App a,XPointSeg xps);
double CalcXPointSegAngle(App a,XPointSeg xps,XPointTest xpt);
int CalcXPointSegNumber(App a,XPointSeg xps,XPointTest xpt);

/* GridPointSeg ////////////////////////////////////////////////////// */

GridPointSeg AddGridPointSeg(App a,XPointSeg xps,int zone); /* $ */
void* DelGridPointSeg(GridPointSeg gps);
GridPointSeg FindGridPointSeg(App a,int zone);
GridPointSeg FindGridPointSegBySegment(App a,XPointSeg xps);
void SetGridPointSegZone(GridPointSeg gps,int zoneNew);
void DrawGridPointSeg(View w,GridPointSeg gps,int mode);
void RecalcGridPointSegLine(GridPointSeg gps,VarDef* pvd,VarSet* pvs); /* only cuts by targets */
void SetGridPointSegFlags(GridPointSeg gps,long flags);
int GridPointSegOpen(GridPointSeg gps);

#define SetGridPointSegDir(gps,__dir) SetObjInt((gps)->app,(gps),\
  GetOffset(GridPointSeg,dir), sizeof(int),(__dir),1)

#define SetGridPointSegTargetDir(gps,dir) SetObjInt((gps)->app,(gps),\
  GetOffset(GridPointSeg,targetDir), sizeof(int),(dir),1)

#define SetGridPointSegShortName(gps,n)\
  SetObjString((gps)->app,(gps),GetOffset(GridPointSeg,shortName),(n),1)
#define GetGridPointSegShortName(gps)\
  ((gps)->shortName==NULL? "" : (gps)->shortName)

#define SetGridPointSegLongName(gps,n)\
  SetObjString((gps)->app,(gps),GetOffset(GridPointSeg,longName),(n),1)
#define GetGridPointSegLongName(gps)\
  ((gps)->longName==NULL? "" : (gps)->longName)

#define GridPointSegIsUsed(gps) ((gps)->flags & GPSF_USED)

char* GetGridPointSegDescription(GridPointSeg gps);

/* SurfaceZone ////////////////////////////////////////////////////// */

#define SurfaceZoneIsUsed(sz) ((sz)->zone>=0)

SurfaceZone AddSurfaceZone(App a,int szNo,int gpsNo1,int gpsNo2,int orient,
    int innerId);
void* DelSurfaceZone(SurfaceZone sz);
void SetSurfaceZoneFlags(SurfaceZone sz,long flags);

int CheckSurfaceZoneData(App a,int szNo,int gpsNo1,int gpsNo2,int sign);
void DrawSurfaceZone(View w,SurfaceZone sz,int mode);
SurfaceZone FindSurfaceZone(App a,int zone);

int FindSurfaceOriginPointEx(App a,int zone,double level,
    double* px,double* py);

/* -1 == not found */
int GetSurfaceZoneByXY(App a,double x,double y,double* pLevel,int* pErr);

int SurfaceZoneLevelOk(SurfaceZone sz,double level);

int RecalcSurfaceZoneSign(SurfaceZone sz);

#define SetSurfaceZoneShortName(sz,n)\
  SetObjString((sz)->app,(sz),GetOffset(SurfaceZone,shortName),(n),1)
#define GetSurfaceZoneShortName(gps)\
  ((gps)->shortName==NULL? "" : (gps)->shortName)

#define SetSurfaceZoneLongName(gps,n)\
  SetObjString((gps)->app,(gps),GetOffset(SurfaceZone,longName),(n),1)
#define GetSurfaceZoneLongName(gps)\
  ((gps)->longName==NULL? "" : (gps)->longName)

char* GetSurfaceZoneDescription(SurfaceZone sz);

int SurfaceZoneBoundsCached(App a);

/* GridPointEx ////////////////////////////////////////////////////// */

typedef struct _ChangeGridPointExRec {
  ActProc actProc;
  void* obj;
  GridPointEx gpx;
  int zone;
  double value;
}* ChangeGridPointExRec;

int ActAddGridPointEx(App a,ActRec ar);
int ActDelGridPointEx(App a,DelRec ar);
int ActChangeGridPointEx(App a,ChangeGridPointExRec ar);

#define GridPointExOk(gpx) ((gpx)->bCoordsOk)

GridPointEx AddGridPointEx(App a,int zone,double value);
void* DelGridPointEx(GridPointEx gpx);
int ChangeGridPointEx(GridPointEx gpx,int zone,double value);

int ChangeGridPointExCreatorId(GridPointEx gpx,char* id);
char* GetGridPointExCreatorId(GridPointEx gpx);
int GetGridPointExNumber(App a,GridPointEx gpx);

int RecalcGridPointExPos(GridPointEx gpx);
void DrawGridPointEx(View w,GridPointEx gpx,int mode);

/* SurfaceEx //////////////////////////////////////////////////////// */

#define SurfaceExOk(sx) ((sx)->bCoordsOk)
#define SurfaceExVirtual(sx) ((sx)->zone==SZN_XY)

SurfaceEx AddSurfaceEx(App a,int zone,double level,int* pErr);
SurfaceEx AddSurfaceExXY(App a,double x,double y,int* pErr);
int ChangeSurfaceEx(SurfaceEx sx,int zone,double level);
int ChangeSurfaceExXY(SurfaceEx sx,double x,double y);
void* DelSurfaceEx(SurfaceEx sx);

SurfaceEx AddSurfaceExByXY(App a,double x,double y,int* pErr);
int ChangeSurfaceExByXY(SurfaceEx sx,double x,double y,int* pErr);

int ChangeSurfaceExCreatorId(SurfaceEx sx,char* id);
char* GetSurfaceExCreatorId(SurfaceEx sx);

int RecalcSurfaceExPos(SurfaceEx sx);
int InvalidateSurfaceExPos(SurfaceEx sx);
void DrawSurfaceEx(View w,SurfaceEx sx,int mode);

void UpdateSurfaceExPos(SurfaceEx sx);
void UpdateGridPointExPos(GridPointEx gpx);
void UpdateAfterTopologyChange(App a);
void InvalidateTopologyCache(App a);
void RecalcTopologyCache(App a);

int GetCarreSurfaceExCount(App a,int zone,int nSpec);
int SurfaceExClosed(SurfaceEx sx);

typedef struct _ChangeSurfaceExRec {
  ActProc actProc;
  void* obj;
  SurfaceEx sx;
  double x,y,level;
  int zone;
}* ChangeSurfaceExRec;

/* SurfacesEx. Each surfaceEx increases the lock count of the equilibrium by 1.
   Without equilibrium, these actions cause fatal errors.
*/

int ActAddSurfaceEx(App a,ActRec ar);
int ActDelSurfaceEx(App a,DelRec ar);
int ActChangeSurfaceEx(App a,ChangeSurfaceExRec ar);


/* Private functions
*/
int DetectXPoints(App a);
int DetectXPointSegs(App a);


#endif
