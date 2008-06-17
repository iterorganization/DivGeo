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

SimpleSplineInfo CreateSimpleSplineInfo(Group gXY);
int CalcSimpleSplineValue(SimpleSplineInfo si,double t,double* pY);
int CalcSimpleSplineDY(SimpleSplineInfo si,double t,double* pDY);
void* FreeSimpleSplineInfo(SimpleSplineInfo si);

ParamSplineInfo CreateParamSplineInfo(Group gXY);
int CalcParamSplineValue(ParamSplineInfo si,double t,double* pX,double* pY);
void* FreeParamSplineInfo(ParamSplineInfo si);

