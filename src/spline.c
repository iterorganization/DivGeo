/* Spline routines */

#include "dg.h"

#define ID_PARAMSPLINEINFO   0x25486324
#define ID_SIMPLESPLINEINFO  0x56488145
#define ID_SURFACESPLINEINFO 0x34278923

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

/* Surface spline /////////////////////////////////////////////////// */

#define IDX(si,ptx,pty) ((pty)*(si)->pointCount[0]+(ptx))

SurfaceSplineInfo CreateSurfaceSplineInfo(
    int pointsX,int pointsY,double* ptx,double *pty,double* ptz) {
  SurfaceSplineInfo si;
  int i,j,k,n;
  double c;
  Group g;
  SimpleSplineInfo* pssi;
  SimpleSplineInfo ssi;
  double* ptzOld;

  assert(pointsX>1 && pointsY>=1);
  /* Allocate & init structures */

  si=Malloc(sizeof(*si));
  si->id=ID_SURFACESPLINEINFO;

  si->pointCount[0]=pointsX;
  si->pointCount[1]=pointsY;

  for (n=0;n<2;n++)
    si->t[n]=Malloc(sizeof(*si->t[n])*si->pointCount[n]);

  for (i=0;i<si->pointCount[0];i++) si->t[0][i]=ptx[i];
  for (i=0;i<si->pointCount[1];i++) si->t[1][i]=pty[i];

  /* Allocate  a read/write duplicate of ptz */

  ptzOld=ptz;
  ptz=Malloc(sizeof(*ptz)*pointsX*pointsY);
  memcpy(ptz,ptzOld,sizeof(*ptz)*pointsX*pointsY);

  /* Rearrange x/y in ascending order, if needed */

  k=si->pointCount[0];
  if (si->t[0][0]>si->t[0][1]) {
    for (j=0;j<k/2;j++) {
      swap(si->t[0][j],si->t[0][k-1-j]);
      for (i=0;i<si->pointCount[1];i++)
        swap(ptz[IDX(si,j,i)],ptz[IDX(si,k-1-j,i)]);
    }
  }

  k=si->pointCount[1];
  if (si->t[1][0]>si->t[1][1]) {
    for (i=0;i<k/2;i++) {
      swap(si->t[1][i],si->t[1][k-1-i]);
      for (j=0;j<si->pointCount[0];i++)
        swap(ptz[IDX(si,j,i)],ptz[IDX(si,j,k-1-i)]);
    }
  }

  for (i=0;i<2;i++) for (j=1;j<si->pointCount[i];j++) {
    if (si->t[i][j]<si->t[i][j-1]) {
      ptz=Free(ptz);
      for (k=0;k<2;k++) si->t[k]=Free(si->t[k]);
      si=Free(si);
      return NULL;
    }
  }

  /* Find out minimum, maximum "t" values */

  for (n=0;n<2;n++) {
    si->tMin[n]=MAXDOUBLE;
    si->tMax[n]=-MAXDOUBLE;
    for (i=0;i<si->pointCount[n];i++) {
      si->tMin[n]=min(si->tMin[n],si->t[n][i]);
      si->tMax[n]=max(si->tMax[n],si->t[n][i]);
    }
  }

  si->coeff=Malloc(sizeof(*si->coeff)*si->pointCount[0]*si->pointCount[1]);

  /* Calculate coefficients */

  pssi=Malloc(sizeof(*pssi)*si->pointCount[1]);

   /* Calculate 1-D spline coefficients for each "row" */

  for (i=0;i<si->pointCount[1];i++) {
    g=CreateGroup();
    for (j=0;j<si->pointCount[0];j++)
      AddXY(g,si->t[0][j],ptz[IDX(si,j,i)]);

    pssi[i]=CreateSimpleSplineInfo(g);
    g=FreeMallocedGroup(g);
  }

  /* Calculate sets of 2D coefficients for each cell */

  for (j=0;j<si->pointCount[0];j++) {
    for (k=0;k<4;k++) {

      /* For each "column" and for each "horizontal" coefficient,
         build a "vertical" spline */

      g=CreateGroup();
      for (i=0;i<si->pointCount[1];i++) {
        switch (k) {
          case 0:
            c=pssi[i]->y[j];break;
          case 1:
            c=pssi[i]->b[j];break;
          case 2:
            c=pssi[i]->c[j];break;
          case 3:
            c=pssi[i]->d[j];break;
          default:
            assert(0);
        }
        AddXY(g,si->t[1][i],c);
      }
      ssi=CreateSimpleSplineInfo(g);
      FreeMallocedGroup(g);

      /* Store coefficients */

      for (i=0;i<si->pointCount[1];i++) {
        si->coeff[IDX(si,j,i)][0][k]=ssi->y[i];
        si->coeff[IDX(si,j,i)][1][k]=ssi->b[i];
        si->coeff[IDX(si,j,i)][2][k]=ssi->c[i];
        si->coeff[IDX(si,j,i)][3][k]=ssi->d[i];
      }

      ssi=FreeSimpleSplineInfo(ssi);
    }
  }

  /* Free "horizontal" splines and ptz double */

  for (i=0;i<si->pointCount[1];i++) pssi[i]=FreeSimpleSplineInfo(pssi[i]);
  pssi=Free(pssi);
  ptz=Free(ptz);

  return si;
}


int CalcSurfaceSplineValue(SurfaceSplineInfo si,double tx,double ty,
    double* pZ,double* pdX,double* pdY) {
  int i,j,k,nx,ny;
  double px,py;

  /* Check for valid args */

  assert(si->id==ID_SURFACESPLINEINFO);
  assert(tx>=si->tMin[0] && tx<=si->tMax[0] &&
      ty>=si->tMin[1] && ty<=si->tMax[1]);

  /* Find the cell */

  nx=ny=-1;
/*  for (i=0;i<si->pointCount[0]-1;i++)
    if (inrange(tx,si->t[0][i],si->t[0][i+1])) {nx=i;break;}

  for (i=0;i<si->pointCount[1]-1;i++)
    if (inrange(ty,si->t[1][i],si->t[1][i+1])) {ny=i;break;} */

  i=0;
  j=si->pointCount[0]-1;assert(j>0);
  while (i+1<j) {
    k=(i+j)/2;
    if (si->t[0][k]<tx) i=k; else j=k;
/* printf("x i j %d %d\n",i,j); */
  }
  if (inrange(tx,si->t[0][i],si->t[0][i+1])) {nx=i;}

  i=0;
  j=si->pointCount[1]-1;assert(j>0);
  while (i+1<j) {
    k=(i+j)/2;
    if (si->t[1][k]<ty) i=k; else j=k;
/* printf("y i j %d %d\n",i,j); */
  }
  if (inrange(ty,si->t[1][i],si->t[1][i+1])) {ny=i;}

  assert(nx>=0 && ny>=0);
/* printf("%d %d\n",nx,ny); */
  /* Calculate the value */

  if (pZ!=NULL) {
    *pZ=0;
    py=1;
    for (i=0;i<4;i++) {
      px=1;
      for (j=0;j<4;j++) {
        *pZ+=si->coeff[IDX(si,nx,ny)][i][j]*px*py;
        px*=(tx-si->t[0][nx]);
      }
      py*=(ty-si->t[1][ny]);
    }
  }
/* printf("%e %e\n",tx-si->t[0][nx],ty-si->t[1][ny]); */

  /* Calculate derivatives, if needed */

  if (pdX!=NULL) {
    *pdX=0;
    py=1;
    for (i=0;i<4;i++) {
      px=1;
      for (j=1;j<4;j++) {
        *pdX+=j*si->coeff[IDX(si,nx,ny)][i][j]*px*py;
        px*=(tx-si->t[0][nx]);
      }
      py*=(ty-si->t[1][ny]);
    }
  }
/* printf("%g\n",*pdX);
 */
  if (pdY!=NULL) {
    *pdY=0;
    py=1;
    for (i=1;i<4;i++) {
      px=1;
      for (j=0;j<4;j++) {
        *pdY+=i*si->coeff[IDX(si,nx,ny)][i][j]*px*py;
        px*=(tx-si->t[0][nx]);
      }
      py*=(ty-si->t[1][ny]);
    }
  }

  return 0;
}

void* FreeSurfaceSplineInfo(SurfaceSplineInfo si) {
  int i;

  assert(si->id==ID_SURFACESPLINEINFO);

  si->coeff=Free(si->coeff);
  for (i=0;i<2;i++) si->t[i]=Free(si->t[i]);

  si=Free(si);

  return si;
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
c  b = diagonal, d = off-diagonal, c = right hand side.
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
