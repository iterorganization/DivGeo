#include "dg.h"

/* Old stuff /////////////////////////////////////////////// */

#define XPLEVEL 0

#define EqIn(eq,x0,y0,cx1,cy1,cx2,cy2) \
  (x0>=eq->x[cx1] && y0>=eq->y[cy1] && x0<=eq->x[cx2] && y0<=eq->y[cy2])

#define EqInS(eq,x0,y0,cx1,cy1,cx2,cy2) \
  (x0>eq->x[cx1] && y0>eq->y[cy1] && x0<eq->x[cx2] && y0<eq->y[cy2])

int AddXPoint(App a,double x1,double y1,double x2,double y2) {
  XPoint xpt;
  struct _ActRec ar;
  int i;

  ValidatePtr(a,"AddXPoint");
  ValidatePtr(a->equil,"AddXPoint.equil");

  xpt=Malloc(sizeof(*xpt));
  xpt->type=T_XPOINT;
  xpt->locks=0;
  xpt->x1=x1;
  xpt->y1=y1;
  xpt->x2=x2;
  xpt->y2=y2;
  xpt->lpx=xpt->lpy=-1;
  if (xpt->x1>xpt->x2) swap(xpt->x1,xpt->x2);
  if (xpt->y1>xpt->y2) swap(xpt->y1,xpt->y2);

  if (a->xpoint!=NULL) DelXPoint(a);

  ar.obj=xpt;
  i=ActAddXPoint(a,&ar);

  if (i) Free(xpt);

  return i;
}

void DelXPoint(App a) {
  struct _DelRec ar;

  ValidatePtr(a,"DelXPoint");
  ValidatePtr(a->xpoint,"DelXPoint.xpoint");

  ar.delete=a->xpoint;
  ActDelXPoint(a,&ar);
}

GridPoint AddGridPoint(App a,int area,double value) {
  GridPoint gp;
  struct _ActRec ar;

  if (area<0 || area>2)
    FatalError("AddGridPoint()-area%d: fatal error 1",area);
  if (value<0 || value>1)
    /* relcheck_ignore_line */ FatalError("AddGridPoint()-value%f: fatal error 1",value);


  gp=Malloc(sizeof(*gp));
  gp->type=T_GRIDPOINT;
  gp->locks=0;
  gp->area=area;
  gp->value=value;
  gp->creatorId=NULL;

  ar.obj=gp;
  ActAddGridPoint(a,&ar);

  return gp;
}

void ChangeGridPoint(App a,GridPoint gp,int area,double value) {
  struct _ChangeGridPointRec ar;

  if (area<0 || area>2)
    FatalError("ChangeGridPoint()-area%d: fatal error 1",area);
  if (value<0 || value>1)
    /* relcheck_ignore_line */ FatalError("ChangeGridPoint()-value%f: fatal error 1",value);

  ar.gp=gp;
  ar.area=area;
  ar.value=value;
  ActChangeGridPoint(a,&ar);

  if (*GetGridPointCreatorId(gp)==CID_UNCHANGEDFLAG)
    ChangeGridPointCreatorId(a,gp,GetGridPointCreatorId(gp)+1);
}

void* DelGridPoint(App a,GridPoint gp) {
  struct _DelRec ar;

  ChangeGridPointCreatorId(a,gp,NULL);

  ar.delete=gp;
  ActDelGridPoint(a,&ar);

  return NULL;
}

int ChangeGridPointCreatorId(App a,GridPoint gp,char* id) {
  if (id!=NULL && !strcmp(id,GetGridPointCreatorId(gp))) return 0;

  SetObjString(a,gp,GetOffset(GridPoint,creatorId),id,0);

  return 0;
}

char* GetGridPointCreatorId(GridPoint gp) {
  return gp->creatorId==NULL? "*" : gp->creatorId;
}

char* ConstructGridPointCreatorId(int zone,int count,double alpha1,double
    alpha2,int law,int carreFlag) {
  static char s[1024];

  sprintf(s,"+z%dn%dh%gH%gw%dC%d",
    zone,count,alpha1,alpha2,law,carreFlag);
  return s;
}

int ParseGridPointCreatorId(char* id,int* pArea,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,int* pCarreFlag) {
  int i;

  if (*id==CID_UNCHANGEDFLAG) id++;
  *pCarreFlag=0;

  i=sscanf(id,"z%dn%dh"SCANFLT"H"SCANFLT"w%dC%d",
      pArea,pCount,pAlpha1,pAlpha2,pLaw,pCarreFlag);

  return i<5? -1 : 0;
}

int DistributeGridPoints(App a,int area,int count,double a1,double a2,
    int law,int carreMode) {
  GridPoint gp;
  char* creatorId;
  int i;
  double v;
  Index ix;

  for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix))
    if (gp->area==area) DelGridPoint(a,gp);

  creatorId=ConstructGridPointCreatorId(area,count,a1,a2,law,carreMode);

  for (i=0;i<count;i++) {
    v=DistributeLaw((i+1)/(double)(count+1),law,a1,a2,count);
    v=max(v,0);
    v=min(v,1);

    /* if (v<0 || v>1) { --- Works bad with Carre mode
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,
	  GetResourceString(dlg->dg.wLaw,"errBadLaw",NULL,NULL));
      return;
    } --- */

    gp=AddGridPoint(a,area,v);
    ChangeGridPointCreatorId(a,gp,creatorId);
  }

  return 0;
}

/* Area==-1 means recursively process all areas */

int RebuildCarreGridPoints(App a,int area) {
  GridPoint gp;
  int count,law,carreMode,r,foo;
  double a1,a2;
  Index ix;
  char* id;

  if (a->outputMode!=OUTPUTMODE_CARRE) return ERR_CARRE_MODE_NEEDED;

  if (area<0) {
    r=RebuildCarreGridPoints(a,SPA_LOOP);if (r) return r;
    r=RebuildCarreGridPoints(a,SPA_TARGET1);if (r) return r;
    r=RebuildCarreGridPoints(a,SPA_TARGET2);if (r) return r;
    return 0;
  }

  if (!CountGridPoints(a,area)) return 0;  /* $$ - maybe some errorcode? */

  for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix)) {
    if (gp->area!=area) continue;
    id=GetGridPointCreatorId(gp);
    if (ParseGridPointCreatorId(id,&foo,&count,
	&a1,&a2,&law,&carreMode))
      continue;
    if (carreMode) break;
  }

  /* $$ - maybe some errorcode if no gridpoints? */
  if (gp==NULL) {count=0;a1=a2=1;law=DGLAW_NORMAL;carreMode=1;}

  r=DistributeGridPoints(a,area,count,a1,a2,law,carreMode);

  return r;
}

/* Private functions - only for ActXX
*/
static int CutLineByTargets(Group line,Group t1,Group t2) {
  XY xy,xy1;
  Elem e;
  Index ix,ixt;
  double r;
  int b=0;

  for (xy=Group1st(line,&ix);(xy1=Next(&ix))!=NULL;xy=xy1) {
    for (e=Group1st(t1,&ixt);e!=NULL;e=Next(&ixt))
      if (!VIntersect(xy->x,xy->y,xy1->x,xy1->y,
          e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
          &r,NULL)) {
        xy->x=xy->x+(xy1->x-xy->x)*r;
        xy->y=xy->y+(xy1->y-xy->y)*r;
        b=1;
      }
      if (b) break;

    for (e=Group1st(t2,&ixt);e!=NULL;e=Next(&ixt))
      if (!VIntersect(xy->x,xy->y,xy1->x,xy1->y,
          e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
          &r,NULL)) {
        xy->x=xy->x+(xy1->x-xy->x)*r;
        xy->y=xy->y+(xy1->y-xy->y)*r;
        b=2;
      }
      if (b) break;
  }
  if (!b) return ERR_NOTARGETS;

  for (xy1=Group1st(line,&ixt);xy1!=xy;xy1=Next(&ixt)) {
    GroupDel(line,xy1);
    Free(xy1);
  }

  xy=Next(&ix);
  if (xy==NULL) return ERR_BADTARGETS;
  for (;(xy1=Next(&ix))!=NULL;xy=xy1) {
    for (e=Group1st(b==2? t1 : t2,&ixt);e!=NULL;e=Next(&ixt))
      if (!VIntersect(xy->x,xy->y,xy1->x,xy1->y,
          e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
          &r,NULL)) {
        xy1->x=xy->x+(xy1->x-xy->x)*r;
        xy1->y=xy->y+(xy1->y-xy->y)*r;
        b+=2;
      }
      if (b>2) break;
  }
  if (xy1==NULL) return ERR_BADTARGETS;
  while ((xy1=Next(&ix))!=NULL) {
    GroupDel(line,xy1);
    Free(xy1);
  }
  if (b==4) RevertGroup(line);

  return 0;
}

int CalcXPointLine(Equil eq,XPoint xpt,int cx1,int cy1,int cx2,int cy2,
    Group t1,Group t2) {
  int i,/*j,k,*/n;
  Group line[4];
  int closed[4];
  XY xy,xy1/*,xyt,xyt1*/;
  Index ix/*,ix1*/;
  XY xx[4];
  double r;
  int err;

  if (cx1>cx2) swap(cx1,cx2);
  if (cy1>cy2) swap(cy1,cy2);
  cx2=min(cx2,eq->sx-2);
  cy2=min(cy2,eq->sy-2);
  if (cx2<=cx1 || cy2<=cy1) return ERR_OUTOFEQUIL;

  n=0;
  for (i=0;i<4;i++) line[i]=NULL;

  for (i=cx1;i<cx2;i++)  if (inrange_s(XPLEVEL,EqCorrCell(eq,i,cy1,XPLEVEL),
      EqCorrCell(eq,i+1,cy1,XPLEVEL))) {
    if (n==4) goto err_toomany;
    closed[n]=CalcSurfaceLine(eq,i,cy1,XPLEVEL,&line[n]);
    if (closed[n]>=0) n++;
  }
  for (i=cx1;i<cx2;i++)  if (inrange_s(XPLEVEL,EqCorrCell(eq,i,cy2,XPLEVEL),
      EqCorrCell(eq,i+1,cy2,XPLEVEL))) {
    if (n==4) goto err_toomany;
    closed[n]=CalcSurfaceLine(eq,i,cy2,XPLEVEL,&line[n]);
    if (closed[n]>=0) n++;
  }
  for (i=cy1;i<cy2;i++)  if (inrange_s(XPLEVEL,EqCorrCell(eq,cx1,i,XPLEVEL),
      EqCorrCell(eq,cx1,i+1,XPLEVEL))) {
    if (n==4) goto err_toomany;
    closed[n]=CalcSurfaceLine(eq,cx1,i,XPLEVEL,&line[n]);
    if (closed[n]>=0) n++;
  }
  for (i=cy1;i<cy2;i++)  if (inrange_s(XPLEVEL,EqCorrCell(eq,cx2,i,XPLEVEL),
      EqCorrCell(eq,cx2,i,XPLEVEL))) {
    if (n==4) goto err_toomany;
    closed[n]=CalcSurfaceLine(eq,cx2,i,XPLEVEL,&line[n]);
    if (closed[n]>=0) n++;
  }
  if (n<2) {err=ERR_BADXPOINT;goto genErr;}

  if (n<4) {line[n]=line[0];closed[n++]=closed[0];line[0]=NULL;}
  for (i=1;i<n;i++) if (!closed[i]) {line[0]=line[i];line[i]=NULL;break;}
  if (n>1 && !closed[1] && line[1]!=NULL) line[1]=FreeMallocedGroup(line[1]);
  for (i=2;i<n;i++) if (closed[i]) {line[1]=line[i];line[i]=NULL;break;}
  for (i=2;i<n;i++) if (line[i]!=NULL) line[i]=FreeMallocedGroup(line[i]);
  if (line[0]==NULL) {err=ERR_BADXPOINT;goto genErr;}

  if ((err=CutLineByTargets(line[0],t1,t2))!=0) goto genErr;

  if (line[1]!=NULL) {
    line[2]=CreateGroup();
    for (xy=Group1st(line[1],&ix);xy!=NULL;xy=Next(&ix))
      if (inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) &&
          inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;
    if (xy==NULL) return ERR_BADXPOINT;
    xy1=xy;
    for (;xy!=NULL;xy=Next(&ix))
      if (!inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) ||
          !inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;
    if (xy!=NULL) for (;xy!=NULL;xy=Next(&ix)) AddXY(line[2],xy->x,xy->y);
    for (xy=Group1st(line[1],&ix);xy!=xy1;xy=Next(&ix))
      AddXY(line[2],xy->x,xy->y);
    line[1]=FreeMallocedGroup(line[1]);
  }
  line[1]=CreateGroup();
  for (xy=Group1st(line[0],&ix);xy!=NULL;xy=Next(&ix)) {
    if (inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) &&
        inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;
    AddXY(line[1],xy->x,xy->y);
  }
  for (;xy!=NULL;xy=Next(&ix))
    if (!inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) ||
        !inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;

  if (line[2]==NULL) {
    line[2]=CreateGroup();
    for (;xy!=NULL;xy=Next(&ix)) {
      if (inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) &&
          inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;
      AddXY(line[2],xy->x,xy->y);
    }
    for (;xy!=NULL;xy=Next(&ix))
      if (!inrange_s(xy->x,eq->x[cx1],eq->x[cx2]) ||
          !inrange_s(xy->y,eq->y[cy1],eq->y[cy2])) break;
  }

  line[3]=CreateGroup();
  for (;xy!=NULL;xy=Next(&ix)) AddXY(line[3],xy->x,xy->y);
  line[0]=FreeMallocedGroup(line[0]);

  for (i=1;i<4;i++) if (IsEmptyGroup(line[i])) {
    err=ERR_BADXPOINT;goto genErr;
  }

  RevertGroup(line[1]);
  xx[0]=Group1st(line[1],NULL);
  RevertGroup(line[1]);
  xx[2]=Group1st(line[2],NULL);
  RevertGroup(line[2]);
  xx[1]=Group1st(line[2],NULL);
  xx[3]=Group1st(line[3],NULL);
  if (VIntersect(xx[0]->x,xx[0]->y,xx[2]->x,xx[2]->y,xx[1]->x,xx[1]->y,
      xx[3]->x,xx[3]->y,&r,NULL)) {
    RevertGroup(line[2]);
    if (VIntersect(xx[0]->x,xx[0]->y,xx[1]->x,xx[1]->y,xx[2]->x,xx[2]->y,
        xx[3]->x,xx[3]->y,&r,NULL))
          return ERR_BADXPOINT;
    xx[0]->x+=(xx[1]->x-xx[0]->x)*r;
    xx[0]->y+=(xx[1]->y-xx[0]->y)*r;
  } else {
    xx[0]->x+=(xx[2]->x-xx[0]->x)*r;
    xx[0]->y+=(xx[2]->y-xx[0]->y)*r;
  }
  for (i=1;i<4;i++) {
    xx[i]->x=xx[0]->x;
    xx[i]->y=xx[0]->y;
  }

  xpt->line[1]=line[1];
  xpt->line[0]=line[2];
  xpt->line[2]=line[3];
  for (i=0;i<3;i++) xpt->lineLen[i]=CalcLineLength(xpt->line[i]);

  return 0;

  err_toomany:
  err=ERR_BADXPOINT;
  genErr:
  for (i=0;i<4;i++) if (line[i]!=NULL) line[i]=FreeMallocedGroup(line[i]);
  return err;
}

void CalcGridPointCoords(XPoint xp,GridPoint gp) {
  double s,l/*,x,y*/;
  XY xy,xy1;
  Index ix;

  xy=Group1st(xp->line[gp->area],&ix);
  if (xy==NULL) FatalError("CalcGridPointCoords()-empty: fatal error 1");
  for (s=0;(xy1=Next(&ix))!=NULL;xy=xy1) {
    l=hypot(xy->x-xy1->x,xy->y-xy1->y)/xp->lineLen[gp->area];
    if (l+s>=gp->value) {
      gp->x=xy->x+(xy1->x-xy->x)*(gp->value-s)/l;
      gp->y=xy->y+(xy1->y-xy->y)*(gp->value-s)/l;
      gp->dx=(xy1->x-xy->x)/l/xp->lineLen[gp->area];
      gp->dy=(xy1->y-xy->y)/l/xp->lineLen[gp->area];
      return;
    }
    s+=l;
  }
  gp->x=xy->x;gp->y=xy->y;gp->dx=1;gp->dy=0;
}

#define EQX a->equil->x
#define EQY a->equil->y
#define EQL(x,y) EqCell(a->equil,(x),(y))

/* Level==MAXDOUBLE to find the peak */

int FindSurfaceOriginPoint(App a,int area,double level,
    double* px,double* py) {
  Chord ch;
  int ocx,ocy,cx,cy,cxe,cye,cx1,cy1,cx2,cy2,b,len;
  int i,s,s0,bPeakSearch;
  double ol,l;

  if (a->equil==NULL) return ERR_NOEQUIL;
  if (a->xpoint==NULL) return ERR_NOXPOINT;

  if (GetEquilCell(a->equil,a->xpoint->x1,a->xpoint->y1,&cx1,&cy1))
    return ERR_OUTOFEQUIL;
  if (GetEquilCell(a->equil,a->xpoint->x2,a->xpoint->y2,&cx2,&cy2))
    return ERR_OUTOFEQUIL;

  i=FindLoopPoint(a);
  if (i) return i;
  s0=s=a->equil->signInside;
  if (!s) return ERR_NOCLOSEDSURFS;

  /* Special case: peak search */

  bPeakSearch=0;
  if (level==MAXDOUBLE) {
    bPeakSearch=1;
    if (a->equil->signInside<0) level=-level;
  }

  /* Check if the given level can exist in the given area */

  switch(area) {
    case 1:
    case 3:
      if ((level-XPLEVEL)*s<=0) return ERR_LEVELNOTINAREA;
      break;
    case 2:
      if (bPeakSearch) level=-level;
      if ((level-XPLEVEL)*s>=0) return ERR_LEVELNOTINAREA;
      break;
    }

  cx=a->xpoint->lpx;
  cy=a->xpoint->lpy;

  /* Find a segment on the X Point rectangle that faces
     the desired surface area */

  len=0;
  switch(area) {
    case 3:
      while ((EqCell(a->equil,cx,cy)-XPLEVEL)*s>0)
	FollowRectCW(&cx,&cy,cx1,cy1,cx2,cy2);
      s=-s;
    case 2:
      while ((EqCell(a->equil,cx,cy)-XPLEVEL)*s>0)
	FollowRectCW(&cx,&cy,cx1,cy1,cx2,cy2);
      s=-s;
    case 1:
      while ((EqCell(a->equil,cx,cy)-XPLEVEL)*s>0)
	FollowRectCW(&cx,&cy,cx1,cy1,cx2,cy2);
      cxe=cx;cye=cy;
      do {
	len++;
	FollowRectCCW(&cx,&cy,cx1,cy1,cx2,cy2);
      } while ((EqCell(a->equil,cx,cy)-XPLEVEL)*s>0);
      break;
    default: assert(0);
  }

  /* See if the desired surface intersects the X Point rectangle */

  do {
    ocx=cx;
    ocy=cy;
    ol=EqCell(a->equil,cx,cy);

    FollowRectCW(&cx,&cy,cx1,cy1,cx2,cy2);
    l=EqCell(a->equil,cx,cy);

    if (min(l,ol)<=level && max(l,ol)>=level) goto Found;
  } while (cx!=cxe || cy!=cye);

  /* Level not found on the rectangle. Follow the gradient until the level
     is found, or the peak or an edge of the equilibrium is reached */

  for (i=0;i<len/2;i++) FollowRectCCW(&cx,&cy,cx1,cy1,cx2,cy2);

  while (1) {
    ocx=cx;
    ocy=cy;

    if (ocx>1 && (EQL(ocx-1,ocy)-EQL(cx,cy))*s>0) {
      cx=ocx-1;cy=ocy;
    }
    if (ocy>1 && (EQL(ocx,ocy-1)-EQL(cx,cy))*s>0) {
      cx=ocx;cy=ocy-1;
    }
    if (ocx<a->equil->sx-2 && (EQL(ocx+1,ocy)-EQL(cx,cy))*s>0) {
      cx=ocx+1;cy=ocy;
    }
    if (ocy<a->equil->sy-2 && (EQL(ocx,ocy+1)-EQL(cx,cy))*s>0) {
      cx=ocx;cy=ocy+1;
    }
    if (ocx==cx && ocy==cy)
      if (bPeakSearch) break;
      else return ERR_LEVELNOTFOUND;

    ol=EQL(ocx,ocy);
    l=EQL(cx,cy);

    if (min(l,ol)<=level && max(l,ol)>=level) break;
  }

  Found:

  if (bPeakSearch || l==ol) l=0; else l=(level-ol)/(l-ol);
  *px=EQX[ocx]+(EQX[cx]-EQX[ocx])*l;
  *py=EQY[ocy]+(EQY[cy]-EQY[ocy])*l;

  return 0;
}

#undef EQX
#undef EQY

int FindLoopPoint(App a) {
  int cx,cy,cx1,cy1,cx2,cy2,b;
  Surface s;

  if (a->xpoint==NULL) return ERR_NOXPOINT;

  if (a->xpoint->lpx>=0) return 0;

  if (GetEquilCell(a->equil,a->xpoint->x1,a->xpoint->y1,&cx1,&cy1))
    return ERR_OUTOFEQUIL;
  if (GetEquilCell(a->equil,a->xpoint->x2,a->xpoint->y2,&cx2,&cy2))
    return ERR_OUTOFEQUIL;

  cx=cx1;cy=cy1;

  do {
    DisableAppUpdate(a,1);
    s=AddSurface(a,a->equil->x[cx],a->equil->y[cy],NULL);
    b=s!=NULL && s->closed;
    if (s!=NULL) {DelSurface(a,s);s=NULL;}
    DisableAppUpdate(a,-1);
    if (b) {
      a->xpoint->lpx=cx;
      a->xpoint->lpy=cy;
      return 0;
    }

    if (cy==cy1) cx==cx2 ? cy++ : cx++; else
    if (cx==cx2) cy==cy2 ? cx-- : cy++; else
    if (cy==cy2) cx==cx1 ? cy-- : cx--; else
    if (cx==cx1) cy==cy1 ? cx++ : cy--; else
    assert(0);
  } while (cx!=cx1 || cy!=cy1);

  return ERR_LOOPNOTFOUND;
}

/***********************************************************************
**                                                                    //
**  XPointTest - new stuff for multiple X Points                     ///
**                                                                  ////
**  Not actually used yet                                           ////
**                                                                  ////
***********************************************************************/

extern View w;
extern App a;

static int CheckXPointLevels(Equil eq,int cx1,int cy1,int cx2,int cy2,
    int x0,int y0,int bMinMax);
static int CheckXPointRect(Equil eq,XPointTest xpt);
static Group FindXPointRects(Equil eq);
static int CalcXPointLevels(Group xpg);
static int FindXPointCenter(Equil eq,XPointTest xpt);
static Group CalcSeparatrixLine(Equil eq,XPointTest xpt,Group xpg,int idx);
static void DelDuplicateXPointSegs(App a);
static Group CalcGradientLine(Equil eq,int x0,int y0,int steps,int bMinMax);

static int ActAddXPointTest(App a,ActRec ar);
static int ActDelXPointTest(App a,DelRec ar);
static int ActAddXPointSeg(App a,ActRec ar);
static int ActDelXPointSeg(App a,DelRec ar);

static int CalcXPointSegLine(App a,XPointSeg xps);
static void* FreeXPointSegLine(App a,XPointSeg xps);

static int CheckXPointLevels(Equil eq,int cx1,int cy1,int cx2,int cy2,
    int x0,int y0,int bMinMax) {

  int x,y,nx,ny,d,nd;

  if ((x0==cx1 || x0==cx2) && (y0==cy1 || y0==cy2)) {
/*    AddSource(a,eq->x[x0],eq->y[y0]); */
    return 0;
  }

  x=x0;y=y0;

  if (x==cx1) {d=1;x++;} else
  if (x==cx2) {d=3;x--;} else
  if (y==cy1) {d=2;y++;} else
  if (y==cy2) {d=0;y--;} else
  assert(0);

  if ((EqCell(eq,x0,y0)-EqCell(eq,x,y))*bMinMax>0) return 0;

  do {
    for (nd=d-1;nd<=d+2;nd++) {
      nx=x;
      ny=y;
      switch(nd & 3) {
	case 0:ny=y-1;break;
	case 1:nx=x+1;break;
	case 2:ny=y+1;break;
	case 3:nx=x-1;break;
	default:assert(0);
      }

      if ((EqCell(eq,x0,y0)-EqCell(eq,nx,ny))*bMinMax<=0) {
	d=nd;
	break;
      }
    }

    x=nx;
    y=ny;
/*printf("%d %d    ",x,y);*/
  } while (nx!=cx1 && nx!=cx2 && ny!=cy1 && ny!=cy2);

/*  if (nx==x0 && ny==y0) puts("Yau!"),AddSource(a,eq->x[x0],eq->y[y0]);*/
/*  else puts("AAA"); */
  return nx!=x0 || ny!=y0;
}

static int CheckXPointRect(Equil eq,XPointTest xpt) {
  struct _XPointMinMax p[6];
  int x,y,ox,oy,n,d,i;
  double lvl,lvl1;

Group g;
XY xy,xy1;
Index ix;

  assert(xpt->cx1<xpt->cx2);
  assert(xpt->cy1<xpt->cy2);
  assert(xpt->cx1>=0);
  assert(xpt->cy1>=0);
  assert(xpt->cx2<eq->sx);
  assert(xpt->cy2<eq->sy);

/* Detect extrema */

  x=xpt->cx1;
  y=xpt->cy1;
  d=1;
  n=0;

  while(n<6) {
    lvl=EqCell(eq,x,y);

    ox=x;oy=y;

    if (y==xpt->cy1) x==xpt->cx2 ? y++ : x++; else
    if (x==xpt->cx2) y==xpt->cy2 ? x-- : y++; else
    if (y==xpt->cy2) x==xpt->cx1 ? y-- : x--; else
    if (x==xpt->cx1) y==xpt->cy1 ? x++ : y--; else
    assert(0);

    lvl1=EqCell(eq,x,y);

    if (d==1 && lvl1<lvl) {
      p[n].x=ox;
      p[n].y=oy;
      p[n].t=d;
      p[n].lvl=lvl;
      n++;
      d=-1;
    } else if (d==-1 && lvl1>lvl) {
      p[n].x=ox;
      p[n].y=oy;
      p[n].t=d;
      p[n].lvl=lvl;
      n++;
      d=1;
    }

    if (x==xpt->cx1 && y==xpt->cy1 && n==0) break;
  }

/* No extrema - error */

  if (!n) return -1;
  assert(n==6);

/* Check for exactly 4 extrema */

  for (i=2;i<n-1;i++)
    if (p[i].x==p[1].x && p[i].y==p[1].y) return -1; /* Too few */

  if (p[n-1].x!=p[1].x || p[n-1].y!=p[1].y) return -1;        /* Too many */

/* Make sure a minimum is 1st */

  if (p[1].t!=-1) for (i=1;i<4;i++) {
    p[0]=p[i];
    p[i]=p[i+1];
    p[i+1]=p[0];
  }

/* Shift values to the left */

  for (i=0;i<4;i++) p[i]=p[i+1];
  n=4;

/* Check for each minimum < each maximum */

  if (p[0].lvl>=p[1].lvl || p[0].lvl>=p[3].lvl) return -1;
  if (p[2].lvl>=p[1].lvl || p[2].lvl>=p[3].lvl) return -1;

/* Check for an 'X'-intersection */

  for (i=0;i<4;i++) {
    if (CheckXPointLevels(eq,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2,
	  p[i].x,p[i].y,p[i].t)) return -1;
  }

  xpt->lvlMin=max(p[0].lvl,p[2].lvl);
  xpt->lvlMax=min(p[1].lvl,p[3].lvl);

/*
  for (i=0;i<4;i++) {
    MarkObject(a,AddSource(a,eq->x[p[i].x],eq->y[p[i].y]),i&1);
  }
*/

  for (i=0;i<4;i++) xpt->minMax[i]=p[i];

  return 0;
}

/* Returns a list of rectangles surrounding X points */

static Group FindXPointRects(Equil eq) {
  int i,j,s;
  Group g;
  XPointTest xp,xpC;
  Index ix;

  g=CreateGroup();

  xpC=Malloc(sizeof(*xpC));
  for (s=1;s<6;s++) {
    for (i=1;i<eq->sx-s-1;i++) {
      for (j=1;j<eq->sy-s-1;j++) {
	xpC->cx1=i;
	xpC->cy1=j;
	xpC->cx2=i+s;
	xpC->cy2=j+s;

	for (xp=Group1st(g,&ix);xp!=NULL;xp=Next(&ix))
	  if (xp->cx2>=xpC->cx1 && xp->cx1<=xpC->cx2 &&
	      xp->cy2>=xpC->cy1 && xp->cy1<=xpC->cy2) goto skip;

	if (CheckXPointRect(eq,xpC)) continue;

	GroupAdd(g,xpC);
	xpC=Malloc(sizeof(*xpC));

	skip:;
      }
    }
  }
  Free(xpC);

  return g;
}

static int CalcXPointLevels(Group xpg) {
  XPointTest xpt;
  Index ix;

  for (xpt=Group1st(xpg,&ix);xpt!=NULL;xpt=Next(&ix))
    xpt->level=(xpt->lvlMin+xpt->lvlMax)/2;

  return 0;
}

static int FindXPointCenter(Equil eq,XPointTest xpt) {
  double xs[4],ys[4],lvl,lvl1,r;
  int n=0,x,y,ox,oy;

  x=xpt->cx1;y=xpt->cy1;

  while(n<4) {
    lvl=EqCorrCell(eq,x,y,xpt->level);

    ox=x;oy=y;

    if (y==xpt->cy1) x==xpt->cx2 ? y++ : x++; else
    if (x==xpt->cx2) y==xpt->cy2 ? x-- : y++; else
    if (y==xpt->cy2) x==xpt->cx1 ? y-- : x--; else
    if (x==xpt->cx1) y==xpt->cy1 ? x++ : y--; else
    assert(0);

    lvl1=EqCorrCell(eq,x,y,xpt->level);

    if ((lvl-xpt->level)*(lvl1-xpt->level)<0) {
      xs[n]=eq->x[ox]+(eq->x[x]-eq->x[ox])*(xpt->level-lvl)/(lvl1-lvl);
      ys[n]=eq->y[oy]+(eq->y[y]-eq->y[oy])*(xpt->level-lvl)/(lvl1-lvl);
/*      AddSource(a,xs[n],ys[n]); */
      n++;
    }

    if (x==xpt->cx1 && y==xpt->cy1 && n==0) break;
  }

  if (n<4) return -1;

  if (VIntersect(xs[0],ys[0],xs[2],ys[2],xs[1],ys[1],xs[3],ys[3],&r,NULL))
    return -1;

  xpt->centerX=xs[0]+(xs[2]-xs[0])*r;
  xpt->centerY=ys[0]+(ys[2]-ys[0])*r;

/*  AddSource(a,xpt->centerX,xpt->centerY); */

  return 0;
}

static Group CalcSeparatrixLine(Equil eq,XPointTest xpt,Group xpg,int idx) {
  int i,n,x,y,ox,oy;
  XPointTest xpi;
  struct _SurfCell sc;
  Group g;
  XY xy,xy1,xy0;
  Index ix,ixpg;

  /* Find a cell that contains the proper "surface" */

  assert(xpt->cx1>0);
  assert(xpt->cy1>0);
  assert(xpt->cx2<eq->sx-1);
  assert(xpt->cy2<eq->sy-1);

  n=0;

  x=xpt->cx1;y=xpt->cy1;
  while(1) {
    ox=x;oy=y;
    if (y==xpt->cy1) x==xpt->cx2 ? y++ : x++; else
    if (x==xpt->cx2) y==xpt->cy2 ? x-- : y++; else
    if (y==xpt->cy2) x==xpt->cx1 ? y-- : x--; else
    if (x==xpt->cx1) y==xpt->cy1 ? x++ : y--; else
    assert(0);

    if (inrange_s(xpt->level,EqCorrCell(eq,ox,oy,xpt->level),
	EqCorrCell(eq,x,y,xpt->level)))
      if (n++==idx) break;

/*    CalcSurfData(eq,x,y,xpt->level,&sc);
    if (sc.n>1) if (n++==idx) break;

    if (y==xpt->cy1-1) x==xpt->cx2-1 ? x++,y++ : x++; else
    if (x==xpt->cx2)   y==xpt->cy2-1 ? x--,y++ : y++; else
    if (y==xpt->cy2)   x==xpt->cx1   ? x--,y-- : x--; else
    if (x==xpt->cx1-1) y==xpt->cy1   ? x++,y-- : y--; else
    assert(0);
*/
    if (x==xpt->cx1 && y==xpt->cy1) {puts("1");return NULL;}
  }

  if (x>ox) swap(x,ox);
  if (y>oy) swap(y,oy);

/*  if (x==xpt->cx1) x--;
  if (y==xpt->cy1) y--; */

  if (x==ox) {if (x==xpt->cx1) x--;} else
  if (y==oy) {if (y==xpt->cy1) y--;} else
  assert(0);

/*  AddChord(a,eq->x[x],eq->y[y],eq->x[x+1],eq->y[y+1]);
  AddChord(a,eq->x[x+1],eq->y[y],eq->x[x],eq->y[y+1]);
*/
  /* Calculate a line */

  i=CalcSurfaceLine(eq,x,y,xpt->level,&g);
  if (i<0) { puts("F");return NULL;}    /* XPointTest */

  if (GroupCount(g)<3) {FreeGroup(g); puts("2");return NULL;}  /* XPointTest */

  /* Detect the direction of the line and cut it if needed */

  if (i) { /* Closed surface */

    for (xy0=xy1=Group1st(g,&ix);;) {
      xy=Next(&ix);

      if (EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
	  !EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
	  EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
	RevertGroup(g); /* Cell -> XPointRect - reverse */
	for (xy=Group1st(g,&ix);xy!=xy1;xy=Next(&ix)); /* Find it again */
/*printf("a:%d\n",GroupIndex(g,xy)); */
/*AddSource(a,xy->x,xy->y); */
	break;
      }

      GroupDel(g,xy1);GroupAdd(g,xy1); /* Rotate the group */

      if (EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
	  !EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
	  EqIn(eq,xy1->x,xy1->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
/*printf("b:%d\n",GroupIndex(g,xy)); */
/* XPointRec -> Cell - found */
/*AddSource(a,xy->x,xy->y);*/
	break;
      }


      if ((xy1=xy)==xy0) break;
    }

  } else { /* Non-closed surface */

    for (xy1=Group1st(g,&ix);(xy=Next(&ix))!=NULL;xy1=xy) {
      if (EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
	  !EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
	  EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
/*puts("c");*/
	break; /* Cell -> XPointRect - found */
      }

      if (EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
	  !EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
	  EqIn(eq,xy1->x,xy1->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
	RevertGroup(g); /* XPointRec -> Cell - reverse */
	for (xy=Group1st(g,&ix);xy!=xy1;xy=Next(&ix)); /* Find it again */
/*puts("d");*/
	break; /* found */
      }
    }
    for (;xy!=NULL;xy=Next(&ix)) {
      GroupDel(g,xy);
      Free(xy);
    }
    RevertGroup(g);
  }


/* Obsolete
printf("GC=%d\n",GroupCount(g));   -- XPointTest --
  xy=GroupAt(g,1);
  if (EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2))
    puts("!"),RevertGroup(g);      -- XPointTest --
*/


  xy=GroupAt(g,1);
  if (EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
    /* assert(0); */
/*
AddSource(a,xy->x,xy->y);
xy=GroupAt(g,0);
AddSource(a,xy->x,xy->y);
xy=GroupAt(g,2);
AddSource(a,xy->x,xy->y);
*/
    FreeGroup(g);
    puts("3"); /* XPointTest */
    return NULL;
  }

  /* Cut the line at the 1st x-point met */

  xy=Group1st(g,&ix);
  while ((xy=Next(&ix))!=NULL) {
    for (xpi=Group1st(xpg,&ixpg);xpi!=NULL;xpi=Next(&ixpg))
      if (EqIn(eq,xy->x,xy->y,xpi->cx1,xpi->cy1,xpi->cx2,xpi->cy2))
	goto BigBreak;
  }
  BigBreak:;

/*  if (xy!=NULL) printf("GI=%d\n",GroupIndex(g,xy)); */

  if (xy!=NULL) while ((xy=Next(&ix))!=NULL) {
    GroupDel(g,xy);
    Free(xy);
  }

/* Add X point center */

  if (xpi!=NULL) AddXY(g,xpi->centerX,xpi->centerY); /* Closed separatrix */

  RevertGroup(g);
  AddXY(g,xpt->centerX,xpt->centerY);
  RevertGroup(g);

  /* Kill duplicate points - bugfix for closed surfaces */

  for (xy1=Group1st(g,&ix);(xy=Next(&ix))!=NULL;xy1=xy)
    if (xy1->x==xy->x && xy1->y==xy->y) {
/*      AddSource(a,xy->x,xy->y); */
      GroupDel(g,xy);
      Free(xy);
      xy=xy1;
    }


  return g;
}

XPointTest AddXPointTest(App a,int cx1,int cy1,int cx2,int cy2,double level) {
  struct _ActRec ar;
  XPointTest xpt;
  int i;

  if (a->equil==NULL) return NULL;

  xpt=Malloc(sizeof(*xpt));

  xpt->type=T_XPOINTTEST;
  xpt->locks=0;
  xpt->app=a;

  xpt->cx1=cx1;
  xpt->cx2=cx2;
  xpt->cy1=cy1;
  xpt->cy2=cy2;
  xpt->level=level;

  if (CheckXPointRect(a->equil,xpt)) {
    Free(xpt);
    return NULL;
  }

  if (FindXPointCenter(a->equil,xpt)) {
    Free(xpt);
    return NULL;
  }

  ar.obj=xpt;
  i=ActAddXPointTest(a,&ar);
  assert(!i);

  return xpt;
}

void* DelXPointTest(App a,XPointTest xpt) {
  struct _DelRec ar;
  XPointSeg xps;
  int i;
  Index ix;

  for (xps=Group1st(xpt->segs,&ix);xps!=NULL;xps=Next(&ix))
    DelXPointSeg(a,xps);

  ar.delete=xpt;

  i=ActDelXPointTest(a,&ar);
  assert(!i);

  return xpt;
}

static int ActAddXPointTest(App a,ActRec ar) {
  DelRec ur;
  XPointTest xpt;
  int i;

  if (AppLocked(a)) return;

  assert(a->equil!=NULL);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelXPointTest);
  ur->delete=ar->obj;

  xpt=ar->obj;
  ar->obj=NULL;

  NotifyAdd(a,xpt);
  DrawAppHighlight(a,DRAW_OFF);

  xpt->segs=CreateGroup();
  for (i=0;i<4;i++) xpt->gradients[i]=
    CalcGradientLine(a->equil,xpt->minMax[i].x,xpt->minMax[i].y,10,i%2*2-1);

  GroupAdd(a->xpointTests,xpt);

  DrawAppObject(a,xpt,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,xpt);

/*  LockObject(a->equil,1); */

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);

  return 0;
}

static int ActDelXPointTest(App a,DelRec ar) {
  ActRec ur;
  XPointTest xpt;
  int i;

  if (AppLocked(a)) return;

  xpt=ar->delete;
  assert (!xpt->locks);

/*  LockObject(a->equil,-1); */

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddXPointTest);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,xpt,DRAW_ERASE);

  GroupDel(a->xpointTests,xpt);

  for (i=0;i<4;i++) if (xpt->gradients[i]!=NULL)
    xpt->gradients[i]=FreeMallocedGroup(xpt->gradients[i]);
  xpt->segs=FreeGroup(xpt->segs);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

static int ActAddXPointSeg(App a,ActRec ar) {
  DelRec ur;
  XPointSeg xps;
  int i;

  if (AppLocked(a)) return;

  i=CalcXPointSegLine(a,ar->obj);
  if (i) return i;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelXPointSeg);
  ur->delete=ar->obj;

  xps=ar->obj;
  ar->obj=NULL;

  NotifyAdd(a,xps);
  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,xps->xpt,DRAW_OFF);

  xps->gridPoints=CreateGroup();
  GroupAdd(xps->xpt->segs,xps);
  GroupAdd(a->xPointSegs,xps);

  DrawAppObject(a,xps->xpt,DRAW_ON);
  DrawAppObject(a,xps,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,xps);

/*  LockObject(xps->xpt,1); */
/*  LockObject(a->equil,1); */

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);

  return 0;
}

static int ActDelXPointSeg(App a,DelRec ar) {
  ActRec ur;
  XPointSeg xps;

  if (AppLocked(a)) return;

  xps=ar->delete;
  assert (!xps->locks);
  assert(a->equil!=NULL);

/*  LockObject(a->equil,-1); */
/*  LockObject(xps->xpt,-1); */

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddXPointSeg);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,xps->xpt,DRAW_OFF);

  DrawAppObject(a,xps,DRAW_ERASE);

  GroupDel(a->xPointSegs,xps);
  GroupDel(xps->xpt->segs,xps);
  assert(IsEmptyGroup(xps->gridPoints));
  xps->gridPoints=FreeGroup(xps->gridPoints);

  FreeXPointSegLine(a,xps);

  DrawAppObject(a,xps->xpt,DRAW_ON);
  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

XPointSeg AddXPointSeg(App a,XPointTest xpt,int number,double sp,double ep) {
  XPointSeg xps;
  struct _ActRec ar;
  int i;

  xps=Malloc(sizeof(*xps));

  xps->type=T_XPOINTSEG;
  xps->locks=0;
  xps->xpt=xpt;
  xps->number=number;
  xps->startPos=sp;
  xps->endPos=ep;
  xps->zone=-1;
  xps->dir=0;
  xps->zoneName="";
  xps->line=NULL;
  xps->lineLen=0;
  xps->gridPoints=NULL;

  ar.obj=xps;

  i=ActAddXPointSeg(a,&ar);

  if (i) {
    Free(xps);
    return NULL;
  }

  return xps;
}

void* DelXPointSeg(App a,XPointSeg xps) {
  struct _DelRec ar;
  int i;

  ar.delete=xps;
  i=ActDelXPointSeg(a,&ar);

  assert(!i);

  return NULL;
}

static int CalcXPointSegLine(App a,XPointSeg xps) {
  Group g;

  /* Calculate the full line */

  if (a->equil==NULL) return ERR_NOEQUIL;

  g=CalcSeparatrixLine(a->equil,xps->xpt,a->xpointTests,xps->number);
  if (g==NULL) return ERR_BADXPOINTSEG;

  /* Cut the line if needed */
  if (xps->endPos>0) CutPolyLine(g,xps->endPos,0);
  if (xps->startPos>0) CutPolyLine(g,xps->startPos,1);

  /* Update the XPointSeg structure and display the result */

  if (xps->line!=NULL) xps->line=FreeMallocedGroup(xps->line);
  xps->line=g;
  xps->lineLen=CalcLineLength(xps->line);

  return 0;
}

static void* FreeXPointSegLine(App a,XPointSeg xps) {
  if (xps->line==NULL) return NULL;

  xps->line=FreeMallocedGroup(xps->line);
  xps->lineLen=0;

  return NULL;
}

static void DelDuplicateXPointSegs(App a) {
  XPointSeg xps1,xps2;
  Index ix1,ix2;
  int k=0;

  for (xps1=AppXPointSeg1st(a,&ix1);xps1!=NULL;xps1=Next(&ix1)) {
    if (xps1->line==NULL) continue;

    for (xps2=AppXPointSeg1st(a,&ix2);xps2!=xps1;xps2=Next(&ix2)) {
      if (xps2->line==NULL) continue;
k++;
      if (!CompPolyLines(xps1->line,xps2->line)) {
/*puts("Deleted!"); */
	DelXPointSeg(a,xps2);
/*        break; */
      }
    }
  }
/*  printf("Comparisions:%d\n",k); */
}

static Group CalcGradientLine(Equil eq,int x0,int y0,int steps,int bMinMax) {
  Group g;
  int x,y,ox,oy,nx,ny;
  int cx1,cy1,cx2,cy2,cx,cy,ocx,ocy,d;
  double fx,fy,xyLvl,lvl,angle,angleMax,dist;

/*puts("[CalcGradientLine");*/

  g=CreateGroup();

  x=x0*steps;
  y=y0*steps;

  ox=oy=-1;

  while(x>0 && y>0 && x<(eq->sx-1)*steps && y<(eq->sy-1)*steps) {
    fx=eq->x[x/steps]+(eq->x[x/steps+1]-eq->x[x/steps])*(x%steps)/steps;
    fy=eq->y[y/steps]+(eq->y[y/steps+1]-eq->y[y/steps])*(y%steps)/steps;

    AddXY(g,fx,fy);

    if (GetEquilLevel(eq,fx,fy,&xyLvl,NULL,NULL)) break;

    angleMax=-1;

    cx1=(x-1)/steps;
    cy1=(y-1)/steps;
    cx2=x/steps+1;
    cy2=y/steps+1;

    cx=cx1;
    cy=cy1;

    do {
      ocx=cx;
      ocy=cy;

      if (cy==cy1) cx==cx2 ? cy++ : cx++; else
      if (cx==cx2) cy==cy2 ? cx-- : cy++; else
      if (cy==cy2) cx==cx1 ? cy-- : cx--; else
      if (cx==cx1) cy==cy1 ? cx++ : cy--; else
      assert(0);

      for (d=0;d<steps;d++) {
	lvl=EqCell(eq,ocx,ocy)+(EqCell(eq,cx,cy)-EqCell(eq,ocx,ocy))*d/steps;
	dist=hypot(eq->x[ocx]+(eq->x[cx]-eq->x[ocx])*d/steps-fx,
	    eq->y[ocy]+(eq->y[cy]-eq->y[ocy])*d/steps-fy);
	if (dist==0) continue;
	angle=(lvl-xyLvl)/dist*bMinMax;

	if (angleMax<0 || angle>angleMax) {
	  angleMax=angle;
	  nx=ocx*steps+(cx-ocx)*d;
	  ny=ocy*steps+(cy-ocy)*d;
	}
      }
    } while (cx!=cx1 || cy!=cy1);

    if (angleMax<0 || (nx==ox && ny==oy)) break;

/* printf("New xy: %d %d %d %d / %d %d %g\n",cx1,cy1,cx2,cy2,nx,ny,angleMax); */
    ox=x;
    oy=y;
    x=nx;
    y=ny;
  }

/*printf("]CalcGradientLine: %d\n",GroupCount(g));*/
  return g;
}

/* Detects all X points. Removes all pre-existing xpoints with segments */

int DetectXPoints(App a) {
  Group g;
  Index ix;
  XPointTest xp,xpt;
  int i;

  if (a->equil==NULL) return ERR_NOEQUIL;

  /* Delete all pre-existing xpoints */

  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    DelXPointTest(a,xpt);

  /* Find all xpoints and calculate levels */

  g=FindXPointRects(a->equil);

  CalcXPointLevels(g);

  /* Add all xpoints */

  for (xp=Group1st(g,&ix);xp!=NULL;xp=Next(&ix)) {
    xpt=AddXPointTest(a,xp->cx1,xp->cy1,xp->cx2,xp->cy2,xp->level);
  }

  FreeMallocedGroup(g);

  /* Add X-Point segments */

  i=DetectXPointSegs(a);

  if (i) return i;

  return 0;
}


#define MAX_INS_N 100

int DetectXPointSegs(App a) {
  Group g;
  Index ix,ix1;
  XPointTest xpt;
  XPointSeg xps;
  int i,j,n;
  double pos[MAX_INS_N];

  if (a->equil==NULL) return ERR_NOEQUIL;

  /* Delete all existing XPointSegs */

  for (xps=AppXPointSeg1st(a,&ix);xps!=NULL;xps=Next(&ix))
    DelXPointSeg(a,xps);

  /* Add xpoint segments and delete duplicates */

  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    for (i=0;i<4;i++) AddXPointSeg(a,xpt,i,-1,-1);

  DelDuplicateXPointSegs(a);

  /* Cut xpoint segments using gradients */

  g=CopyGroup(a->xPointSegs,NULL); /* Do not interfere with added segs */

  for (xps=Group1st(g,&ix1);xps!=NULL;xps=Next(&ix1)) {
    n=0;
    if (xps->line==NULL) continue;

    for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix)) {
      for (i=0;i<4;i++) {
	if (PolyLinesIntersect(xps->line,xpt->gradients[i],&pos[n],NULL))
	  continue;
	if (++n>=MAX_INS_N) goto BigBreak1; /* should never happen */
      }
    }

    BigBreak1:

    if (!n) continue;

    /* Sort intersection points */

    for (i=0;i<n;i++) for (j=i+1;j<n;j++)
      if (pos[j]<pos[i]) swap(pos[j],pos[i]);

    /* Add segments between intersections */

    if (pos[0]>0) AddXPointSeg(a,xps->xpt,xps->number,-1,pos[0]),puts("a");

    for (i=0;i<n-1;i++) if (pos[i+1]>pos[i])
      AddXPointSeg(a,xps->xpt,xps->number,pos[i],pos[i+1]),puts("b");

    if (pos[n-1]<xps->lineLen)
      AddXPointSeg(a,xps->xpt,xps->number,pos[n-1],-1),puts("c");

    /* Delete the original segment */

    DelXPointSeg(a,xps);
  }

  FreeGroup(g);

  return 0;
}


