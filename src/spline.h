/* Spline routines */

typedef struct _SimpleSplineInfo {
  int id;

  int pointCount;
  double tMin,tMax;

  double *y,* t,* b,* c,* d;
}* SimpleSplineInfo;

typedef struct _ParamSplineInfo {
  int id;

  int pointCount;
  double tMin,tMax;

  double *x,*y,* t,* bx,* cx,* dx,* by,* cy,* dy;
}* ParamSplineInfo;

typedef struct _SurfaceSplineInfo {
  int id;

  int pointCount[2];
  double tMin[2],tMax[2];

  double* t[2];
  double (*coeff)[4][4];
}* SurfaceSplineInfo;

SimpleSplineInfo CreateSimpleSplineInfo(Group gXY);
int CalcSimpleSplineValue(SimpleSplineInfo si,double t,double* pY);
int CalcSimpleSplineDY(SimpleSplineInfo si,double t,double* pDY);
void* FreeSimpleSplineInfo(SimpleSplineInfo si);

ParamSplineInfo CreateParamSplineInfo(Group gXY);
int CalcParamSplineValue(ParamSplineInfo si,double t,double* pX,double* pY);
void* FreeParamSplineInfo(ParamSplineInfo si);

#define SurfaceSplineValidXY(si,x,y) \
    ((x)>=si->tMin[0] && (y)>=si->tMin[1] && \
     (x)<=si->tMax[0] && (y)<=si->tMax[1])

SurfaceSplineInfo CreateSurfaceSplineInfo(
    int pointsX,int pointsY,double* ptx,double *pty,double* ptz);
int CalcSurfaceSplineValue(SurfaceSplineInfo si,double tx,double ty,
    double* pZ,double* pdX,double* pdY);
void* FreeSurfaceSplineInfo(SurfaceSplineInfo si);
