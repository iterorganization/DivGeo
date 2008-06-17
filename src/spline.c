/* Spline routines */

#include "dg.h"

#define ID_PARAMSPLINEINFO  0x25486324
#define ID_SIMPLESPLINEINFO 0x56488145

static void spline(int n,double* x,double* y,double* b,double* c,double* d);

/* Simple spline //////////////////////////////////////////////////// */

SimpleSplineInfo CreateSimpleSplineInfo(Group gXY) {
  SimpleSplineInfo si;
  XY xy;
  int i,n;
  Index ix;

  si=Malloc(sizeof(*si));
  si->id=ID_SIMPLESPLINEINFO;

  si->pointCount=n=GroupCount(gXY);

  assert(n>0);

  si->y=Malloc(sizeof(*si->y)*n);

  si->t=Malloc(sizeof(*si->t)*n);

  si->b=Malloc(sizeof(*si->b)*n);
  si->c=Malloc(sizeof(*si->c)*n);
  si->d=Malloc(sizeof(*si->d)*n);

  si->tMin=MAXDOUBLE;
  si->tMax=-MAXDOUBLE;

  for (i=0,xy=Group1st(gXY,&ix);xy!=NULL;xy=Next(&ix)) {
    if (i>0 && xy->x==si->t[i-1]) continue;
    assert(i<n);
    si->t[i]=xy->x;
    si->y[i]=xy->y;

    si->tMin=min(si->tMin,si->t[i]);
    si->tMax=max(si->tMax,si->t[i]);
    i++;
  }

  n=si->pointCount=i;

  if (n>1) {
    spline(n,si->t,si->y,si->b,si->c,si->d);
  }

  return si;
}

int CalcSimpleSplineValue(SimpleSplineInfo si,double t,double* pY) {
  int i;
  double v;

  assert(si->id==ID_SIMPLESPLINEINFO);
  assert(t>=si->tMin && t<=si->tMax);

  if (si->pointCount==1) {
    *pY=si->y[0];
    return 0;
  }

  for (i=0;i<si->pointCount;i++) if (i>0 && si->t[i]>=t) break;

  i=min(i,si->pointCount-1);

  i--;

  v=t-si->t[i];

  *pY=si->y[i]+si->b[i]*v+si->c[i]*v*v+si->d[i]*v*v*v;

  return 0;
}

int CalcSimpleSplineDY(SimpleSplineInfo si,double t,double* pDY) {
  int i;
  double v;

  assert(si->id==ID_SIMPLESPLINEINFO);
  assert(t>=si->tMin && t<=si->tMax);

  if (si->pointCount==1) {
    *pDY=0;
    return 0;
  }

  for (i=0;i<si->pointCount;i++) if (i>0 && si->t[i]>=t) break;

  i=min(i,si->pointCount-1);

  i--;

  v=t-si->t[i];

  v=si->b[i]+2*si->c[i]*v+3*si->d[i]*v*v;
  *pDY=v;

  return 0;
}


void* FreeSimpleSplineInfo(SimpleSplineInfo si) {
  assert(si->id==ID_SIMPLESPLINEINFO);

  si->y=Free(si->y);
  si->t=Free(si->t);
  si->b=Free(si->b);
  si->c=Free(si->c);
  si->d=Free(si->d);

  return Free(si);
}

/* Parametric spline //////////////////////////////////////////////// */

ParamSplineInfo CreateParamSplineInfo(Group gXY) {
  ParamSplineInfo si;
  XY xy;
  int i,n;
  double t2,h;
  Index ix;

  si=Malloc(sizeof(*si));
  si->id=ID_PARAMSPLINEINFO;

  si->pointCount=n=GroupCount(gXY);

  assert(n>0);

  si->x=Malloc(sizeof(*si->x)*n);
  si->y=Malloc(sizeof(*si->y)*n);

  si->t=Malloc(sizeof(*si->t)*n);

  si->bx=Malloc(sizeof(*si->bx)*n);
  si->cx=Malloc(sizeof(*si->cx)*n);
  si->dx=Malloc(sizeof(*si->dx)*n);

  si->by=Malloc(sizeof(*si->by)*n);
  si->cy=Malloc(sizeof(*si->cy)*n);
  si->dy=Malloc(sizeof(*si->dy)*n);


  for (t2=0,i=0,xy=Group1st(gXY,&ix);xy!=NULL;xy=Next(&ix)) {
    if (i>0 && xy->x==si->x[i-1] && xy->y==si->y[i-1]) continue;
    assert(i<n);
    si->x[i]=xy->x;
    si->y[i]=xy->y;
    if (i>0) {
      h=hypot(si->x[i]-si->x[i-1],si->y[i]-si->y[i-1]);
      if (h<=0) continue;
      t2+=h;
    }
    si->t[i]=t2;
    i++;
  }

  n=si->pointCount=i;

  if (n>1) {
    spline(n,si->t,si->x,si->bx,si->cx,si->dx);
    spline(n,si->t,si->y,si->by,si->cy,si->dy);
  }

  si->tMin=0;
  si->tMax=t2;

  return si;
}

int CalcParamSplineValue(ParamSplineInfo si,double t,double* pX,double* pY) {
  int i;
  double v;

  assert(si->id==ID_PARAMSPLINEINFO);
  assert(t>=si->tMin && t<=si->tMax);

  if (si->pointCount==1) {
    *pX=si->x[0];
    *pY=si->y[0];
    return 0;
  }

  for (i=0;i<si->pointCount;i++) if (i>0 && si->t[i]>=t) break;

  i=min(i,si->pointCount-1);

  i--;

  v=t-si->t[i];

  *pX=si->x[i]+si->bx[i]*v+si->cx[i]*v*v+si->dx[i]*v*v*v;
  *pY=si->y[i]+si->by[i]*v+si->cy[i]*v*v+si->dy[i]*v*v*v;

  return 0;
}

void* FreeParamSplineInfo(ParamSplineInfo si) {
  assert(si->id==ID_PARAMSPLINEINFO);

  si->x=Free(si->x);
  si->y=Free(si->y);
  si->t=Free(si->t);
  si->bx=Free(si->bx);
  si->cx=Free(si->cx);
  si->dx=Free(si->dx);
  si->by=Free(si->by);
  si->cy=Free(si->cy);
  si->dy=Free(si->dy);

  return Free(si);
}


/* Spline routine /////////////////////////////////////////////////// */

/* --- Converted from: Netlib spline.f */

static void spline(int n,double* x,double* y,double* b,double* c,double* d) {

/*
c  the coefficients b(i), c(i), and d(i), i=1,2,...,n are computed
c  for a cubic interpolating spline
c
c    s(x) = y(i) + b(i)*(x-x(i)) + c(i)*(x-x(i))**2 + d(i)*(x-x(i))**3
c
c    for  x(i) .le. x .le. x(i+1)
c
c  input..
c
c    n = the number of data points or knots (n.ge.2)
c    x = the abscissas of the knots in strictly increasing order
c    y = the ordinates of the knots
c
c  output..
c
c    b, c, d  = arrays of spline coefficients as defined above.
c
c  using  p  to denote differentiation,
c
c    y(i) = s(x(i))
c    b(i) = sp(x(i))
c    c(i) = spp(x(i))/2
c    d(i) = sppp(x(i))/6  (derivative from the right)
c
c  the accompanying function subprogram  seval  can be used
c  to evaluate the spline.
c
c
*/

  int nm1,ib,i;
  double t;

  /* Accomodate Fortran arrays */

  x--;
  y--;
  b--;
  c--;
  d--;

  nm1 = n-1;
  if (n<2) return;
  if (n<3) goto _50;

/*
c
c  set up tridiagonal system
c
c  b = diagonal, d = offdiagonal, c = right hand side.
c
*/
   d[1] = x[2] - x[1];
   c[2] = (y[2] - y[1])/d[1];
   for (i=2;i<=nm1;i++) {
      d[i] = x[i+1] - x[i];
      b[i] = 2.*(d[i-1] + d[i]);
      c[i+1] = (y[i+1] - y[i])/d[i];
      c[i] = c[i+1] - c[i];
   }
/*
c
c  end conditions.  third derivatives at  x(1)  and  x(n)
c  obtained from divided differences
c
*/
  b[1] = -d[1];
  b[n] = -d[n-1];
  c[1] = 0.;
  c[n] = 0.;
  if (n==3) goto _15;
  c[1] = c[3]/(x[4]-x[2]) - c[2]/(x[3]-x[1]);
  c[n] = c[n-1]/(x[n]-x[n-2]) - c[n-2]/(x[n-1]-x[n-3]);
  c[1] = c[1]*d[1]*d[1]/(x[4]-x[1]);
  c[n] = -c[n]*d[n-1]*d[n-1]/(x[n]-x[n-3]);
/*
c
c  forward elimination
c
*/
  _15:

  for (i=2;i<=n;i++) {
    t = d[i-1]/b[i-1];
    b[i] = b[i] - t*d[i-1];
    c[i] = c[i] - t*c[i-1];
  }

/*
c
c  back substitution
c
*/

  c[n] = c[n]/b[n];
  for (ib=1;ib<=nm1;ib++) {
    i = n-ib;
    c[i] = (c[i] - d[i]*c[i+1])/b[i];
  }

/*
c
c  c(i) is now the sigma(i) of the text
c
c  compute polynomial coefficients
c
*/

  b[n] = (y[n] - y[nm1])/d[nm1] + d[nm1]*(c[nm1] + 2.*c[n]);
  for (i=1;i<=nm1;i++) {
    b[i] = (y[i+1] - y[i])/d[i] - d[i]*(c[i+1] + 2.*c[i]);
    d[i] = (c[i+1] - c[i])/d[i];
    c[i] = 3.*c[i];
  }
  c[n] = 3.*c[n];
  d[n] = d[n-1];
  return;

  _50:

  b[1] = (y[2]-y[1])/(x[2]-x[1]);
  c[1] = 0.;
  d[1] = 0.;
  b[2] = b[1];
  c[2] = 0.;
  d[2] = 0.;
  return;
}

/* --- */
