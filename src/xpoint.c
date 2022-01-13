#include "dg.h"

/* Old stuff /////////////////////////////////////////////// */

#define DEL_DUP_MAX_ERROR 1e-2
#define XPS_ANGLE_FRACTION 0.03
#define XPS_ANGLE_CELLS_MIN 2
/*
#define __EqIn(eq,x0,y0,cx1,cy1,cx2,cy2) \
  (x0>=eq->x[cx1] && y0>=eq->y[cy1] && x0<=eq->x[cx2] && y0<=eq->y[cy2])

#define __EqInS(eq,x0,y0,cx1,cy1,cx2,cy2) \
  (x0>eq->x[cx1] && y0>eq->y[cy1] && x0<eq->x[cx2] && y0<eq->y[cy2]) */

static int EqIn(Equil eq,
    double x0,double y0,int cx1,int cy1,int cx2,int cy2) {
  if (x0<min(eq->x[cx1],eq->x[cx2]) || x0>max(eq->x[cx1],eq->x[cx2]) ||
      y0<min(eq->y[cy1],eq->y[cy2]) || y0>max(eq->y[cy1],eq->y[cy2]))
    return 0;

  return 1;
}

static int EqInS(Equil eq,
    double x0,double y0,int cx1,int cy1,int cx2,int cy2) {
  if (x0<=min(eq->x[cx1],eq->x[cx2]) || x0>=max(eq->x[cx1],eq->x[cx2]) ||
      y0<=min(eq->y[cy1],eq->y[cy2]) || y0>=max(eq->y[cy1],eq->y[cy2]))
    return 0;

  return 1;
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
  GridPointEx gpx;
  char* creatorId;
  int i;
  double v;
  Index ix;

  for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix))
    if (gpx->zone==area) DelGridPointEx(gpx);

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

    gpx=AddGridPointEx(a,area,v);
    ChangeGridPointExCreatorId(gpx,creatorId);
  }

  return 0;
}

/* Area==-1 means recursively process all areas */

int RebuildCarreGridPoints(App a,int area) {
  GridPointEx gpx;
  GridPointSeg gps;
  int count,law,carreMode,r,foo;
  double a1,a2;
  Index ix;
  char* id;

  if (a->outputMode!=OUTPUTMODE_CARRE) return ERR_CARRE_MODE_NEEDED;

  if (area<0) {
    for (gps=AppGridPointSeg1st(a,&ix);gps!=NULL;gps=Next(&ix))
      if (gps->flags & GPSF_USED) RebuildCarreGridPoints(a,gps->zone);
    return 0;
  }


  if (!CountGridPoints(a,area)) return 0;  /* $$ - maybe some error code? */

  for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix)) {
    if (gpx->zone!=area) continue;
    id=GetGridPointExCreatorId(gpx);
    if (ParseGridPointCreatorId(id,&foo,&count,
        &a1,&a2,&law,&carreMode))
      continue;
    if (carreMode) break;
  }

  /* $$ - maybe some error code if no grid points? */
  if (gpx==NULL) {count=0;a1=a2=1;law=DGLAW_NORMAL;carreMode=1;}

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

/***********************************************************************
**                                                                    //
**  XPointTest - new stuff for multiple X Points                     ///
**                                                                  ////
**  Not actually used yet                                           ////
**                                                                  ////
***********************************************************************/

extern View w;
/*extern App a;*/

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
static int NextGridPointSegNumber(App a);

#define GPSEG_STARTNO 1001

static int CheckXPointLevels(Equil eq,int cx1,int cy1,int cx2,int cy2,
    int x0,int y0,int bMinMax) {

  int x,y,nx=0,ny=0,d,nd;

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
  XPointTest xpi = NULL;
  struct _SurfCell sc;
  Group g,g_t;
  XY xy,xy1,xy0;
  Index ix,ixpg;

  assert(eq->type==T_EQUIL);

  /* Find a cell that contains the proper "surface" */

  assert(xpt->cx1>0);
  assert(xpt->cy1>0);
  assert(xpt->cx2<eq->sx-1);
  assert(xpt->cy2<eq->sy-1);

  assert(xpt->cx1<xpt->cx2);
  assert(xpt->cy1<xpt->cy2);

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
    if (x==xpt->cx1 && y==xpt->cy1) {/* puts("1"); */return NULL;}
  }

  if (x>ox) swap(x,ox);
  if (y>oy) swap(y,oy);

/*  if (x==xpt->cx1) x--;
  if (y==xpt->cy1) y--; */

  if (x==ox) {if (x==xpt->cx1) x--;} else
  if (y==oy) {if (y==xpt->cy1) y--;} else
  assert(0);

 /*  AddChord(w->app,eq->x[x],eq->y[y],eq->x[x+1],eq->y[y+1]); */
 /*  AddChord(w->app,eq->x[x+1],eq->y[y],eq->x[x],eq->y[y+1]); */

  /* Calculate a line */

  i=CalcSurfaceLine(eq,x,y,xpt->level,&g,eq->sx,eq->sy);
  if (i<0) {
    /*puts("F");*/return NULL;}    /* XPointTest */

  if (GroupCount(g)<3) {
    FreeGroup(g); /*puts("2");*/return NULL;}  /* XPointTest$*/

  /* Order the line */

  xy=GroupAt(g,0);
  xy1=GroupAt(g,GroupCount(g)-1);

  if (xy->x<xy1->x) RevertGroup(g);
  else if (xy->x==xy1->x && xy->y<xy1->y) RevertGroup(g);

  /* Create a double number of points */

  g_t=CreateGroup();
  for (xy0=xy1=Group1st(g,&ix);(xy=Next(&ix))!=NULL;xy1=xy) {
    GroupAdd(g_t,xy1);
    AddXY(g_t,(xy1->x+xy->x)/2,(xy1->y+xy->y)/2);
  }
  GroupAdd(g_t,xy1);
  if (i) AddXY(g_t,(xy1->x+xy0->x)/2,(xy1->y+xy0->y)/2);
  FreeGroup(g);
  g=g_t;
  g_t=NULL;

  /* --- End create */

  /* Detect the direction of the line and cut it if needed */

 /* xy=GroupAt(g,0); */
 /* if (xy!=NULL) AddViewLabel(w,xy->x,xy->y,"00",0); */
 /* return g; */

  if (i) { /* Closed surface */

    for (xy0=xy1=Group1st(g,&ix);;) {
      xy=Next(&ix);

 /* AddSource(w->app,(EqX(eq,x)+EqX(eq,x+1))/2,(EqY(eq,y)+EqY(eq,y+1))/2); */
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

    /* Kill the part that passes through the X point rectangle and beyond */

    for (xy1=Group1st(g,&ix);(xy=Next(&ix))!=NULL;xy1=xy) {
      /* Trace the surface line */

/*
if (!EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
    EqIn(eq,xy->x,xy->y,x,y,x+1,y+1))
AddViewLabel(w,xy->x,xy->y,"Enter",0);

if (EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
    !EqIn(eq,xy->x,xy->y,x,y,x+1,y+1))
AddViewLabel(w,xy1->x,xy1->y,"Leave",0);
*/

      if (EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
          !EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
          EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
/*puts("c");*/
 /* if (xy1!=NULL) AddViewLabel(w,xy->x,xy->y,"Direct",0); */
        break; /* Cell -> XPointRect - found */
      }

      if (EqIn(eq,xy->x,xy->y,x,y,x+1,y+1) &&
          !EqIn(eq,xy1->x,xy1->y,x,y,x+1,y+1) &&
          EqIn(eq,xy1->x,xy1->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2)) {
 /* if (xy!=NULL) AddViewLabel(w,xy1->x,xy1->y,"Reverse",0); */
        RevertGroup(g); /* XPointRec -> Cell - reverse */
        for (xy=Group1st(g,&ix);xy!=xy1;xy=Next(&ix)); /* Find it again */
/*puts("d");*/
        break; /* found */
      }
    }
 /* if (xy==NULL) AddViewLabel(w,xy1->x,xy1->y,"NotFound",0); */

    for (;xy!=NULL;xy=Next(&ix)) {
      GroupDel(g,xy);
      Free(xy);
    }
    RevertGroup(g);
  }

 /* xy=GroupAt(g,0); */
 /* if (xy!=NULL) AddViewLabel(w,xy->x,xy->y,"0",0); */
 /* xy=GroupAt(g,1); */
 /* if (xy!=NULL) AddViewLabel(w,xy->x,xy->y,"1",0); */

/* Obsolete
printf("GC=%d\n",GroupCount(g));   -- XPointTest -- obsolete --
  xy=GroupAt(g,1);
  if (EqIn(eq,xy->x,xy->y,xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2))
    puts("!"),RevertGroup(g);      -- XPointTest -- obsolete --
*/

  /* Make sure we are not immediately entering the XptRect again */

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
    FreeMallocedGroup(g);
    /*puts("3");*/ /* XPointTest */
    return NULL;
  }

  /* Cut the line at the 1st x-point met */

  xy=Group1st(g,&ix);
  while ((xy=Next(&ix))!=NULL) {
    for (xpi=Group1st(xpg,&ixpg);xpi!=NULL;xpi=Next(&ixpg))
      if (EqInS(eq,xy->x,xy->y,xpi->cx1,xpi->cy1,xpi->cx2,xpi->cy2))
        goto BigBreak;
  }
  BigBreak:;

/*  if (xy!=NULL) printf("GI=%d\n",GroupIndex(g,xy)); */

  if (xy!=NULL) do {
    GroupDel(g,xy);
    Free(xy);
  } while ((xy=Next(&ix))!=NULL);

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

  /* Make sure loops are in CCW order */
  if (xpi!=NULL && xpi==xpt) {
    xy=GroupAt(g,1);
    xy1=GroupAt(g,GroupCount(g)-2);
    if ((xy->x-xpt->centerX)*(xy1->y-xpt->centerY)-
        (xy1->x-xpt->centerX)*(xy->y-xpt->centerY)<0) RevertGroup(g);
  }

  return g;
}

XPointTest AddXPointTest(App a,int cx1,int cy1,int cx2,int cy2,double level,int id) {
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
  xpt->id=id;

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

XPointTest FindXPointTest(App a,int id) {
  XPointTest xpt;
  Index ix;

  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    if (xpt->id==id) return xpt;

  return NULL;
}

static int ActAddXPointTest(App a,ActRec ar) {
  DelRec ur;
  XPointTest xpt;
  int i;

  if (AppLocked(a)) return 0;

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

  if (AppLocked(a)) return 0;

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

  if (AppLocked(a)) return 0;

  i=CalcXPointSegLine(a,ar->obj);
  if (i) return i;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelXPointSeg);
  ur->delete=ar->obj;

  xps=ar->obj;
  ar->obj=NULL;

  NotifyAdd(a,xps);
  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,xps->xpt,DRAW_OFF);

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

  if (AppLocked(a)) return 0;

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
  xps->line=NULL;
  xps->lineLen=0;

  ar.obj=xps;

  i=ActAddXPointSeg(a,&ar);

  if (i) {
    Free(xps);
    return NULL;
  }

  AddGridPointSeg(a,xps,NextGridPointSegNumber(a));

  return xps;
}

void* DelXPointSeg(App a,XPointSeg xps) {
  GridPointSeg gps;
  struct _DelRec ar;
  Index ix;
  int i;

  for (gps=AppGridPointSeg1st(a,&ix);gps!=NULL;gps=Next(&ix))
    if (gps->xps==xps) DelGridPointSeg(gps);

  ar.delete=xps;
  i=ActDelXPointSeg(a,&ar);

  assert(!i);

  return NULL;
}

static int CalcXPointSegLine(App a,XPointSeg xps) {
  Group g;
  XY xy,xy0;
  Index ix;

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

double CalcXPointSegAngle(App a,XPointSeg xps,XPointTest xpt) {
  Equil eq=a->equil;
  Group g;
  double minD,dx,dy,angle;
  int b2dHalf;
  XY xy0,xy,xy1;
  Index ix;

  assert(eq!=NULL);

  g=CalcSeparatrixLine(a->equil,xps->xpt,a->xpointTests,xps->number);
  if (g==NULL) return XPS_ANGLE_BAD;

  xy=GroupAt(g,0);
  xy1=GroupAt(g,GroupCount(g)-1);

  /* Make the line point away from xpt */

  if (hypot(xy1->x-xpt->centerX,xy1->y-xpt->centerY)<
      hypot(xy->x-xpt->centerX,xy->y-xpt->centerY)) RevertGroup(g);

  minD=fabs(EqY(eq,0)-EqY(eq,EqSY(eq)-1))*XPS_ANGLE_FRACTION;
  minD=max(minD,(EqAverageCellWidth(eq)+EqAverageCellHeight(eq))/2*
      XPS_ANGLE_CELLS_MIN);

  xy0=Group1st(g,&ix);
  while ((xy=Next(&ix))!=NULL) {
    if (hypot(dx=xy->x-xy0->x,dy=xy->y-xy0->y)>minD) break;
  }

  if (xy==NULL) {
    FreeMallocedGroup(g);
    return XPS_ANGLE_BAD;
  }

  if (dx==0) angle=dy<0? -M_PI_2 : M_PI_2;
  else angle=atan(dy/dx);

  if (dx<0) angle+=M_PI;

  FreeMallocedGroup(g);

  return angle/M_PI*180;
}

int CalcXPointSegNumber(App a,XPointSeg xps,XPointTest xpt) {
  Group g;
  XPointSeg xps1;
  double minD,dx,dy,angle;
  int b2dHalf;
  XY xy0,xy,xy1;
  Index ix;
  int nTotal,nBefore;

  nTotal=nBefore=0;

  for (xps1=AppXPointSeg1st(a,&ix);xps1!=NULL;xps1=Next(&ix)) {
    if (xps1->xpt==xps->xpt && xps1->number==xps->number) {
      nTotal++;
      if (xps1->startPos<xps->startPos) nBefore++;
    }
  }

  /* Invert the number if the opposite xpoint */
  if (xpt!=xps->xpt) nBefore=nTotal-nBefore-1;

  return nBefore;
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
      if (!CompPolyLinesEx(xps1->line,xps2->line,
          EqAverageCellWidth(a->equil)*DEL_DUP_MAX_ERROR)) {
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
  int x,y,ox,oy,nx=0,ny=0;
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

  for (i=0,xp=Group1st(g,&ix);xp!=NULL;xp=Next(&ix),i++) {
    xpt=AddXPointTest(a,xp->cx1,xp->cy1,xp->cx2,xp->cy2,xp->level,i);
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
  GridPointSeg gps;
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

    if (pos[0]>0) AddXPointSeg(a,xps->xpt,xps->number,-1,pos[0]);

    for (i=0;i<n-1;i++) if (pos[i+1]>pos[i])
      AddXPointSeg(a,xps->xpt,xps->number,pos[i],pos[i+1]);

    if (pos[n-1]<xps->lineLen)
      AddXPointSeg(a,xps->xpt,xps->number,pos[n-1],-1);

    /* Delete the original segment */

    DelXPointSeg(a,xps);
  }

  FreeGroup(g);

  return 0;
}

/* GridPointSeg ///////////////////////////////////////////////////// */

static int ActAddGridPointSeg(App a,ActRec ar);
static int ActDelGridPointSeg(App a,DelRec ar);

static int ActAddGridPointSeg(App a,ActRec ar) {
  GridPointSeg gps;
  DelRec ur;

  if (AppLocked(a)) return 0;

  gps=ar->obj;
  ar->obj=NULL;

  assert(gps->type==T_GRIDPOINTSEG);
  assert(gps->line==NULL);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelGridPointSeg);
  ur->delete=gps;

  gps->line=gps->xps->line;

  NotifyAdd(a,gps);
  DrawAppHighlight(a,DRAW_OFF);

  GroupAdd(a->gridPointSegs,gps);

  DrawAppObject(a,gps,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,gps);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);

  return 0;
}

static int ActDelGridPointSeg(App a,DelRec ar) {
  ActRec ur;
  GridPointSeg gps;

  if (AppLocked(a)) return 0;

  gps=ar->delete;

  assert(gps->type==T_GRIDPOINTSEG);
  assert(!gps->locks);
  assert(gps->line!=NULL);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddGridPointSeg);
  ur->obj=gps;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,gps,DRAW_ERASE);

  GroupDel(a->gridPointSegs,gps);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  gps->line=NULL;

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

GridPointSeg AddGridPointSeg(App a,XPointSeg xps,int zone) { /* $ */
  GridPointSeg gps;
  struct _ActRec ar;

  gps=Malloc(sizeof(*gps));

  gps->type=T_GRIDPOINTSEG;
  gps->locks=0;
  gps->app=a;

  gps->zone=zone;
  gps->xps=xps;
  gps->line=NULL;
  gps->lineLength=CalcLineLength(gps->xps->line);
  gps->level=gps->xps->xpt->level;
  gps->flags=0L;

  gps->shortName=NULL;
  gps->longName=NULL;
  gps->dir=1;
  gps->targetDir=1;

  ar.obj=gps;
  ActAddGridPointSeg(a,&ar);

  return gps;
}

void* DelGridPointSeg(GridPointSeg gps) {
  struct _DelRec ar;

  SetGridPointSegShortName(gps,NULL);
  SetGridPointSegLongName(gps,NULL);

  ar.delete=gps;
  ActDelGridPointSeg(gps->app,&ar);

  return NULL;
}



GridPointSeg FindGridPointSeg(App a,int zone) {
  GridPointSeg gps;
  Index ix;

  for (gps=Group1st(a->gridPointSegs,&ix);gps!=NULL;gps=Next(&ix))
    if (gps->zone==zone) return gps;

  return NULL;
}

GridPointSeg FindGridPointSegBySegment(App a,XPointSeg xps) {
  GridPointSeg gps;
  Index ix;

  for (gps=Group1st(a->gridPointSegs,&ix);gps!=NULL;gps=Next(&ix))
    if (gps->xps==xps) return gps;

  return NULL;
}

void SetGridPointSegZone(GridPointSeg gps,int zone) {
  SetObjInt(gps->app,gps,GetOffset(GridPointSeg,zone),sizeof(int),zone,
    1);
}

char* GetGridPointSegDescription(GridPointSeg gps) {
  static char buf[2048];

  if (*GetGridPointSegLongName(gps)) return GetGridPointSegLongName(gps);

  sprintf(buf,"Untitled (%d)",gps->zone);
  return buf;
}

void SetGridPointSegFlags(GridPointSeg gps,long flags) {
  SetObjInt(gps->app,gps,
      GetOffset(GridPointSeg,flags),sizeof(gps->flags),flags,0);
}

void DrawGridPointSeg(View w,GridPointSeg gps,int mode) {
  XY xy,xy1;
  double dx,dy,h;
  int i;
  char s[4096];

/*   if (~w->showFlags & SHW_GRIDPOINTS) return; */
  if (!w->bEditTopology) return;

  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,gps) ? VMX_ELEMNORMAL : VM1_AXES);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,gps)) {SetViewMode(w,VMX_ELEMNORMAL);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_ELEMMARK); /*$*/
      break;
  }

  /* Find the central point */
  i=GroupCount(gps->xps->line);
  if (i<4) i=0; else i/=2;

  xy=GroupAt(gps->xps->line,i);
  xy1=GroupAt(gps->xps->line,i+1);
  dx=-(xy1->x-xy->x)*gps->dir;
  dy=-(xy1->y-xy->y)*gps->dir;

#define DGPS_ANGLE (20.0/180.0*M_PI)

  h=hypot(dx,dy);
  if (h<=0) return;
  dx*=w->arrowLength/w->zoomX/h;
  dy*=w->arrowLength/w->zoomX/h;

  DrawViewLine(w,xy1->x,xy1->y,
    xy1->x+dx*cos(DGPS_ANGLE)-dy*sin(DGPS_ANGLE),
    xy1->y+dx*sin(DGPS_ANGLE)+dy*cos(DGPS_ANGLE));

  DrawViewLine(w,xy1->x,xy1->y,
    xy1->x+dx*cos(-DGPS_ANGLE)-dy*sin(-DGPS_ANGLE),
    xy1->y+dx*sin(-DGPS_ANGLE)+dy*cos(-DGPS_ANGLE));

  if (w->bEditTopology) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMNUMBER);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMNUMBER);
        break;
    }
    DrawViewLine(w,
        xy1->x+w->arrowLength/w->zoomX,xy1->y+w->arrowLength/w->zoomX,
        xy1->x,xy1->y);
    sprintf(s,"%d (%s)",gps->zone,GetGridPointSegShortName(gps));
    DrawViewText(w,xy1->x+w->arrowLength/w->zoomX,xy1->y+w->arrowLength/w->zoomX,s);
  }

}

/* GridPointEx ////////////////////////////////////////////////////// */

/* Procedures */

GridPointEx AddGridPointEx(App a,int zone,double value) {
  GridPointEx gpx;
  struct _ActRec ar;

  if (value<0 || value>1)
    FatalError("AddGridPointEx()-value%f: fatal error 1",value); /* relcheck_ignore_line */

  gpx=Malloc(sizeof(*gpx));
  gpx->type=T_GRIDPOINTEX;
  gpx->locks=0;
  gpx->app=a;

  gpx->zone=zone;
  gpx->value=value;
  gpx->creatorId=NULL;

  gpx->bDrawn=1;
  gpx->bCoordsOk=0;
  gpx->x=gpx->y=gpx->dx=gpx->dy=0;

  ar.obj=gpx;
  ActAddGridPointEx(a,&ar);

  return gpx;
}

int ChangeGridPointEx(GridPointEx gpx,int zone,double value) {
  struct _ChangeGridPointExRec ar;

  if (value<0 || value>1)
    FatalError("ChangeGridPointEx()-value%f: fatal error 1",value); /* relcheck_ignore_line */

  ar.gpx=gpx;
  ar.zone=zone;
  ar.value=value;
  ActChangeGridPointEx(gpx->app,&ar);

  if (*GetGridPointExCreatorId(gpx)==CID_UNCHANGEDFLAG)
    ChangeGridPointExCreatorId(gpx,GetGridPointExCreatorId(gpx)+1);

  return 0;
}

void* DelGridPointEx(GridPointEx gpx) {
  struct _DelRec ar;

  ChangeGridPointExCreatorId(gpx,NULL);

  ar.delete=gpx;
  ActDelGridPointEx(gpx->app,&ar);

  return NULL;
}

int ChangeGridPointExCreatorId(GridPointEx gpx,char* id) {
  if (id!=NULL && !strcmp(id,GetGridPointExCreatorId(gpx))) return 0;

  SetObjString(gpx->app,gpx,GetOffset(GridPointEx,creatorId),id,0);

  return 0;
}

char* GetGridPointExCreatorId(GridPointEx gpx) {
  return gpx->creatorId==NULL? "*" : gpx->creatorId;
}

/* Computations */

int RecalcGridPointExPos(GridPointEx gpx) {
  GridPointSeg gps;
  double s,l,value/*,x,y*/;
  XY xy,xy1;
  Index ix;

  gpx->bCoordsOk=0;
  gps=FindGridPointSeg(gpx->app,gpx->zone);
  if (gps==NULL) return -1;

  value=gpx->value;
  if (gps->dir<0) value=1-value;

  xy=Group1st(gps->line,&ix);
  if (xy==NULL)
    FatalError("RecalcGridPointExPos()-empty: fatal error 1");
  for (s=0;(xy1=Next(&ix))!=NULL;xy=xy1) {
    l=hypot(xy->x-xy1->x,xy->y-xy1->y)/gps->lineLength;
    if (l+s>=value) {
      gpx->x=xy->x+(xy1->x-xy->x)*(value-s)/l;
      gpx->y=xy->y+(xy1->y-xy->y)*(value-s)/l;
      gpx->dx=(xy1->x-xy->x)/l/gps->lineLength;
      gpx->dy=(xy1->y-xy->y)/l/gps->lineLength;
      gpx->bCoordsOk=1;
      return 0;
    }
    s+=l;
  }
  gpx->x=xy->x;gpx->y=xy->y;gpx->dx=1;gpx->dy=0;
  gpx->bCoordsOk=1;

  return 0;
}

void DrawGridPointEx(View w,GridPointEx gpx,int mode) {
  if (~w->showFlags & SHW_GRIDPOINTS) return;
  if (!gpx->bCoordsOk) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,gpx) ? VMX_GPOINT : VM1_GPOINT);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,gpx)) {SetViewMode(w,VMX_GPOINT);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_GPOINT);
      break;
  }
  DrawViewLine(w,gpx->x-gpx->dy/w->zoomX*w->gridPointLen/2,
    gpx->y+gpx->dx/w->zoomY*w->gridPointLen/2,
    gpx->x+gpx->dy/w->zoomX*w->gridPointLen/2,
    gpx->y-gpx->dx/w->zoomY*w->gridPointLen/2);
}



/* Actions */

int ActAddGridPointEx(App a,ActRec ar) {
  GridPointEx gpx;
  DelRec ur;
  Index ix;

  if (AppLocked(a)) return 0;

  gpx=ar->obj;
  ar->obj=NULL;

  assert(gpx->type==T_GRIDPOINTEX);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelGridPointEx);
  ur->delete=gpx;

  NotifyAdd(a,gpx);
  DrawAppHighlight(a,DRAW_OFF);

/*  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>gp->area ||(gp1->area==gp->area && gp1->value>gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix); -- old
  GroupAddAt(a->gridPoints,gp1,gp); */

  GroupAdd(a->gridPointsEx,gpx);
  if (gpx->bDrawn) RecalcGridPointExPos(gpx);

  DrawAppObject(a,gpx,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,gpx);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);

  return 0;
}

int ActChangeGridPointEx(App a,ChangeGridPointExRec ar) {
  GridPointEx gp1;
  ChangeGridPointExRec ur;
  Index ix;

  if (AppLocked(a)) return 0;

  if (ar->gpx->locks)
    FatalError("ActChangeGridPointEx()-locks: fatal error 1");
  assert(ar->gpx->type==T_GRIDPOINTEX);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeGridPointEx);
  ur->gpx=ar->gpx;
  ur->zone=ar->gpx->zone;
  ur->value=ar->gpx->value;

  NotifyChange(a,ar->gpx);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->gpx,DRAW_OFF);

/*   GroupDel(a->gridPointsEx,ar->gpx); -- obsolete */
  ar->gpx->zone=ar->zone;
  ar->gpx->value=ar->value;

/*  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>ar->gp->area ||
        (gp1->area==ar->gp->area && gp1->value>ar->gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix);
  GroupAddAt(a->gridPoints,gp1,ar->gp); */

  if (ar->gpx->bDrawn) RecalcGridPointExPos(ar->gpx);
  DrawAppObject(a,ar->gpx,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->gpx);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelGridPointEx(App a,DelRec ar) {
  ActRec ur;
  GridPointEx gpx;

  if (AppLocked(a)) return 0;

  gpx=ar->delete;
  assert(gpx->type==T_GRIDPOINTEX);

  if (gpx->locks) FatalError("ActDelGridPointEx()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddGridPointEx);
  ur->obj=gpx;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,gpx,DRAW_ERASE);

  GroupDel(a->gridPointsEx,gpx);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int GetGridPointExNumber(App a,GridPointEx gpx) {
  GridPointEx gpx1;
  Index ix;
  int n=1;

  for (gpx1=AppGridPointEx1st(a,&ix);gpx1!=NULL;gpx1=Next(&ix))
    if (gpx1->zone==gpx->zone && gpx1->value<gpx->value) n++;

  return n;
}

/* SurfaceZone ////////////////////////////////////////////////////// */

static int ActDelSurfaceZone(App a,DelRec ar);
static void FreeSurfaceZoneBounds(SurfaceZone sz);

static int ActAddSurfaceZone(App a,ActRec ar) {
  SurfaceZone sz;
  DelRec ur;
  Index ix;

  if (AppLocked(a)) return 0;

  sz=ar->obj;
  ar->obj=NULL;

  assert(sz->type==T_SURFACEZONE);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelSurfaceZone);
  ur->delete=sz;

  NotifyAdd(a,sz);
  DrawAppHighlight(a,DRAW_OFF);

  GroupAdd(a->surfaceZones,sz);

  DrawAppObject(a,sz,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,sz);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);

  return 0;
}

static int ActDelSurfaceZone(App a,DelRec ar) {
  ActRec ur;
  SurfaceZone sz;

  if (AppLocked(a)) return 0;

  sz=ar->delete;
  assert(sz->type==T_SURFACEZONE);
  if (sz->bounds!=NULL) FreeSurfaceZoneBounds(sz);

  if (sz->locks) FatalError("ActDelSurfaceZone()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddSurfaceZone);
  ur->obj=sz;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,sz,DRAW_ERASE);

  GroupDel(a->surfaceZones,sz);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

SurfaceZone AddSurfaceZone(App a,int szNo,int gpsNo1,int gpsNo2,int orient,
    int innerId) {
  SurfaceZone sz;
  struct _ActRec ar;
  Elem e;
  Index ix;

  assert(orient==1 || orient==-1);

  sz=Malloc(sizeof(*sz));
  sz->type=T_SURFACEZONE;
  sz->locks=0;
  sz->app=a;

  sz->zone=szNo;
  sz->gpZone1=gpsNo1;
  sz->gpZone2=gpsNo2;
  sz->sign=0;
  sz->orient=orient;
  sz->level1=sz->level2=0;
  sz->flags=0;
  sz->bounds=NULL;

  if (innerId >= 0) {
    for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix))
      if (e->id==innerId) sz->innermost=e;
  } else sz->innermost=NULL;

  sz->shortName=NULL;
  sz->longName=NULL;

  ar.obj=sz;
  ActAddSurfaceZone(a,&ar);

  RecalcSurfaceZoneSign(sz);

  return sz;
}

void* DelSurfaceZone(SurfaceZone sz) {
  struct _DelRec ar;

  SetSurfaceZoneShortName(sz,NULL);
  SetSurfaceZoneLongName(sz,NULL);

  ar.delete=sz;
  ActDelSurfaceZone(sz->app,&ar);

  return NULL;
}

void SetSurfaceZoneFlags(SurfaceZone sz,long flags) {
  SetObjInt(sz->app,sz,GetOffset(SurfaceZone,flags),sizeof(sz->flags),flags,0);
}

int CheckSurfaceZoneData(App a,int szNo,int gpsNo1,int gpsNo2,int sign) {
  GridPointSeg gps,gps2;

  gps=FindGridPointSeg(a,gpsNo1);
  if (gps==NULL) return ERR_BADGPZONENUMBER;

  if (gpsNo2>=0) {
    gps2=FindGridPointSeg(a,gpsNo2);
    if (gps2==NULL) return ERR_BADGPZONENUMBER;
    if (sign>0 && gps2->xps->xpt->level<gps->xps->xpt->level)
      return ERR_BADSURFACEZONE1;
    if (sign<0 && gps2->xps->xpt->level>gps->xps->xpt->level)
      return ERR_BADSURFACEZONE1;

  }

  return 0;
}


void DrawSurfaceZone(View w,SurfaceZone sz,int mode) {
}

SurfaceZone FindSurfaceZone(App a,int zone) {
  SurfaceZone sz;
  Index ix;

  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
    if (sz->zone==zone) return sz;

  return NULL;
}

char* GetSurfaceZoneDescription(SurfaceZone sz) {
  static char buf[2048];

  if (*GetSurfaceZoneLongName(sz)) return GetSurfaceZoneLongName(sz);

  sprintf(buf,"Untitled (%d)",sz->zone);
  return buf;
}

#define EQX a->equil->x
#define EQY a->equil->y
#define EQL(x,y) EqCell(a->equil,(x),(y))

int FindSurfaceOriginPointEx(App a,int zone,double level,
    double* px,double* py) {
  Chord ch;
  int ocx,ocy,cx,cy,cxe,cye,cx1,cy1,cx2,cy2,b,len;
  int i,s,s0,bPeakSearch;
  double ol,l;
  SurfaceZone sz;
  GridPointSeg gps,gps2;
  XY xy;

  if (a->equil==NULL) return ERR_NOEQUIL;

  bPeakSearch= level==MAXDOUBLE;

  sz=FindSurfaceZone(a,zone);
  if (sz==NULL) return ERR_BADSURFZONENUMBER;

  gps=FindGridPointSeg(a,sz->gpZone1);
  if (gps==NULL) return ERR_BADGPZONENUMBER;

  l=gps->xps->xpt->level;
  if (!bPeakSearch && (level-l)*sz->sign<0) return ERR_OUTOFSURFZONE;

  if (sz->gpZone2>=0) {
    gps2=FindGridPointSeg(a,sz->gpZone2);
    if (gps2==NULL) return ERR_BADGPZONENUMBER;
    l=gps2->xps->xpt->level;
    if (!bPeakSearch && (level-l)*sz->sign>0) return ERR_OUTOFSURFZONE;
  }

  i=GroupCount(gps->line);

  xy=GroupAt(gps->line,i/2);

  i=GetEquilCell(a->equil,xy->x,xy->y,&cx,&cy);

  if (i) return ERR_FSOPX_1; /* Out of equilibrium */

  /* Direct hit - return immediately */
  if (EQL(cx,cy)==level) {
    *px=EqX(a->equil,cx);
    *py=EqY(a->equil,cy);

    return 0;
  }

  /* Initialize the peak search flag */

  if (bPeakSearch) {
    level= (sz->sign<0)? -MAXDOUBLE:MAXDOUBLE;
  }


  /* Determine the needed gradient sign */

  s= EQL(cx,cy)>level? -1 : 1;

  /* s>0: go "upwards"  s<0: go "downwards" */

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
    if (ocx==cx && ocy==cy) {
      if (bPeakSearch) break;
      else return ERR_LEVELNOTFOUND;
    }

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

int SurfaceZoneLevelOk(SurfaceZone sz,double level) {
  GridPointSeg gps,gps2;
  double l;

  gps=FindGridPointSeg(sz->app,sz->gpZone1);
  if (gps==NULL) return 0; /*ERR_BADGPZONENUMBER; */

  l=gps->xps->xpt->level;
  if ((level-l)*sz->sign<0) return 0; /* ERR_OUTOFSURFZONE; */

  if (sz->gpZone2>=0) {
    gps2=FindGridPointSeg(sz->app,sz->gpZone2);
    if (gps2==NULL) return 0; /* ERR_BADGPZONENUMBER; */
    l=gps2->xps->xpt->level;
    if ((level-l)*sz->sign>0) return 0; /* ERR_OUTOFSURFZONE; */
  }

  return 1;
}

/*static int GridPointSegInSurfaceZone(GridPointSeg gps,SurfaceZone sz) {
  GridPointSeg gps1;
  XY xy,xy1;

  gps1=FindGridPointSeg(sz->app,sz->gpZone1);
  if (gps1==NULL) return 0;

  xy=
*/

int GetSurfaceZoneByXY(App a,double x,double y,double* pLevel,int* pErr) {
  SurfaceZone sz,szMin=NULL;
  GridPointSeg gps,gpsMin;
  double level,d,dMin=MAXDOUBLE;
  int cx,cy,ocx,ocy;
  Group g,gXY;
  Index ix,ixy;
  XY xy;
  int pErrBackup;

  if (pErr==NULL) pErr=&pErrBackup;

  if (a->equil==NULL) {
    *pErr=ERR_NOEQUIL;
    return -1;
  }

  if (GetEquilLevel(a->equil,x,y,&level,NULL,NULL)) {
    *pErr=ERR_OUTOFEQUIL;
    return -1;
  }

  g=CreateGroup();
  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
    if (SurfaceZoneLevelOk(sz,level)) GroupAdd(g,sz);

  if (GroupCount(g)<1) {
    FreeGroup(g);
    *pErr=ERR_NOSURFZONE;
    return -1;
  }

  for (sz=Group1st(g,&ix);sz!=NULL;sz=Next(&ix))
    if(sz->bounds==NULL) break;
  if (sz!=NULL) RecalcTopologyCache(a);

  for (sz=Group1st(g,&ix);sz!=NULL;sz=Next(&ix)) {
    if (sz->bounds==NULL) continue;
/*    gps=FindGridPointSeg(a,sz->gpZone1);
    if (gps==NULL) continue; -- old */

    d=PointToPolyLineDist(sz->bounds,x,y);
    if (d<dMin) {
      dMin=d;
      szMin=sz;
    }
  }

 /* $ */
/*  if (GroupCount(g)>1) {
    FreeGroup(g);
    *pErr=ERR_NOSONNET;
    return -1;
  }

  sz=Group1st(g,NULL); */

  FreeGroup(g);
  sz=szMin;

  if (sz==NULL) {
    *pErr=ERR_NOSURFZONE;
    return -1;
  }

  if (pLevel!=NULL) *pLevel=level;

  *pErr=0;
  return sz->zone;
}

#undef EQX
#undef EQY
#undef EQL

/* SurfaceEx //////////////////////////////////////////////////////// */

int ActAddSurfaceEx(App a,ActRec ar) {
  SurfaceEx s;
  DelRec ur;
  int i,cx,cy,sx,sy;
  double level;
  Group g;
  Var t1,t2;

  if (AppLocked(a)) return 0;

  assert(a->equil!=NULL);

  s=ar->obj;
  ar->obj=NULL;

  assert(s->type==T_SURFACEEX);
  assert(s->line==NULL);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelSurfaceEx);
  ur->delete=s;

  NotifyAdd(a,s);
  DrawAppHighlight(a,DRAW_OFF);

  if (s->bDrawn) RecalcSurfaceExPos(s);

      /*LockObject(a->equil,1);*/

  GroupAdd(a->surfacesEx,s);

      /*if (!a->equil->signInside && s->closed)
        a->equil->signInside=sign(s->level);*/

  DrawAppObject(a,s,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,s);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActDelSurfaceEx(App a,DelRec ar) {
  ActRec ur;
  SurfaceEx s;

  if (AppLocked(a)) return 0;

  s=ar->delete;
  assert(s->type==T_SURFACEEX);

  if (s->locks)
    FatalError("ActDelSurfaceEx()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc) ActAddSurfaceEx);
  ur->obj=s;

  NotifyDel(a,s);
  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,s,DRAW_ERASE);

  InvalidateSurfaceExPos(s);

  GroupDel(a->surfacesEx,s);
  /*LockObject(a->equil,-1);*/
  assert(s->line==NULL);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,s);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ActChangeSurfaceEx(App a,ChangeSurfaceExRec ar) {
  ChangeSurfaceExRec ur;
  int i,cx,cy,sx,sy;
  double level;
  Group g;
  Var t1,t2;
  SurfaceEx s=ar->sx;

  if (AppLocked(a)) return 0;

  assert(s->type==T_SURFACEEX);

  if (s->locks)
    FatalError("ActChangeSurfaceEx()-locks: fatal error 1");

  ur=CreateActRec(sizeof(*ur),(ActProc) ActChangeSurfaceEx);
  ur->sx=s;
  ur->x=s->originX;
  ur->y=s->originY;
  ur->level=s->level;
  ur->zone=s->zone;

  NotifyChange(a,s);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,s,DRAW_OFF);

  s->originX=ar->x;
  s->originY=ar->y;
  s->level=ar->level;
  s->zone=ar->zone;

  RecalcSurfaceExPos(s);

  DrawAppObject(a,s,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,s);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

/* **** */

int ChangeSurfaceExXY(SurfaceEx sx,double x,double y);
void* DelSurfaceEx(SurfaceEx sx);

int ChangeSurfaceExCreatorId(SurfaceEx sx,char* id);
char* GetSurfaceExCreatorId(SurfaceEx sx);

SurfaceEx AddSurfaceEx(App a,int zone,double level,int* pErr) {
  SurfaceEx sx;
  struct _ActRec ar;

  if (pErr!=NULL) *pErr=0; /* $ */

  assert(zone>=0);

  sx=Malloc(sizeof(*sx));
  sx->type=T_SURFACEEX;
  sx->locks=0;
  sx->app=a;

  sx->zone=zone;
  sx->level=level;
  sx->creatorId=NULL;

  sx->lastError=0;
  sx->bDrawn=1;
  sx->bCoordsOk=0;
  sx->originX=sx->originY=0;
  sx->line=NULL;
  sx->closed=-1;

  ar.obj=sx;
  ActAddSurfaceEx(a,&ar);

  return sx;
}

SurfaceEx AddSurfaceExXY(App a,double x,double y,int* pErr) {
  SurfaceEx sx;
  struct _ActRec ar;

  if (pErr!=NULL) *pErr=0; /* $ */

  sx=Malloc(sizeof(*sx));
  sx->type=T_SURFACEEX;
  sx->locks=0;
  sx->app=a;

  sx->lastError=0;
  sx->zone=SZN_XY;
  sx->level=0;
  sx->creatorId=NULL;
  sx->originX=x;
  sx->originY=y;

  sx->bDrawn=1;
  sx->bCoordsOk=0;
  sx->line=NULL;
  sx->closed=-1;

  ar.obj=sx;
  ActAddSurfaceEx(a,&ar);

  return sx;
}

int ChangeSurfaceEx(SurfaceEx sx,int zone,double level) {
  struct _ChangeSurfaceExRec ar;

  ar.sx=sx;
  ar.zone=zone;
  ar.level=level;
  ar.x=ar.y=0;

  ActChangeSurfaceEx(sx->app,&ar);

  if (*GetSurfaceExCreatorId(sx)==CID_UNCHANGEDFLAG)
    ChangeSurfaceExCreatorId(sx,GetSurfaceExCreatorId(sx)+1);

  return 0;
}

int ChangeSurfaceExXY(SurfaceEx sx,double x,double y) {
  struct _ChangeSurfaceExRec ar;

  ar.sx=sx;
  ar.zone=SZN_XY;
  ar.level=0;
  ar.x=x;
  ar.y=y;

  ActChangeSurfaceEx(sx->app,&ar);

  if (*GetSurfaceExCreatorId(sx)==CID_UNCHANGEDFLAG)
    ChangeSurfaceExCreatorId(sx,GetSurfaceExCreatorId(sx)+1);

  return 0;
}

void* DelSurfaceEx(SurfaceEx sx) {
  struct _DelRec ar;

  ChangeSurfaceExCreatorId(sx,NULL);

  ar.delete=sx;
  ActDelSurfaceEx(sx->app,&ar);

  return NULL;
}

int ChangeSurfaceExCreatorId(SurfaceEx sx,char* id) {
  assert(sx->type==T_SURFACEEX);

  if (id!=NULL && !strcmp(id,GetSurfaceExCreatorId(sx))) return 0;

  SetObjString(sx->app,sx,GetOffset(SurfaceEx,creatorId),id,0);

  return 0;
}

char* GetSurfaceExCreatorId(SurfaceEx sx) {
  assert(sx->type==T_SURFACEEX);

  return sx->creatorId==NULL? "*" : sx->creatorId;
}

int RecalcSurfaceExPos(SurfaceEx sx) {
  int r,cx,cy;
  double lvl;
  Group g;

  assert(sx->type==T_SURFACEEX);

  InvalidateSurfaceExPos(sx);

  if (sx->app->equil==NULL) {
    sx->lastError=ERR_NOEQUIL;
    return sx->lastError;
  }

  if (sx->zone!=SZN_XY) {
    sx->lastError=FindSurfaceOriginPointEx(sx->app,sx->zone,sx->level,
        &sx->originX,&sx->originY);
    if (sx->lastError) return sx->lastError;
  }

  if (GetEquilCellEx(sx->app->equil,sx->originX,sx->originY,&cx,&cy,
      EqSX(sx->app->equil),EqSY(sx->app->equil)))
    return sx->lastError=ERR_OUTOFEQUIL;

  if (GetEquilLevel(sx->app->equil,sx->originX,sx->originY,&lvl,NULL,NULL))
    FatalError("ActAddSurfaceEx()-level: fatal error 1");
  r=CalcSurfaceLine(sx->app->equil,cx,cy,lvl,&g,
      EqSX(sx->app->equil),EqSY(sx->app->equil)); /* $$$ */
  if (r<0) return ERR_FLAT_EQUIL;

  sx->line=g;
  sx->closed=r;
  if (sx->zone==SZN_XY) sx->level=lvl;

  sx->bCoordsOk=1;

  return 0;
}

int InvalidateSurfaceExPos(SurfaceEx sx) {
  assert(sx->type==T_SURFACEEX);

  if (sx->bCoordsOk) {
    sx->line=FreeMallocedGroup(sx->line);
    sx->bCoordsOk=0;
  }
  else assert(sx->line==0);

  return 0;
}

void DrawSurfaceEx(View w,SurfaceEx sx,int mode) {
  Index ix;
  XY xy1,xy2;

  assert(sx->type==T_SURFACEEX);

  if (~w->showFlags & SHW_SURFACES) return;
  if (!sx->bCoordsOk) return;
  assert(sx->line!=NULL);

  xy1=Group1st(sx->line,&ix);
  if (xy1==NULL) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,sx) ? VMX_SURFACE : VM1_SURFACE);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,sx)) {SetViewMode(w,VMX_SURFACE);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_SURFACE);
      break;
  }
  while ((xy2=Next(&ix))!=NULL) {
    DrawViewLine(w,xy1->x,xy1->y,xy2->x,xy2->y);
    xy1=xy2;
  }
}

SurfaceEx AddSurfaceExByXY(App a,double x,double y,int* pErr) {
  int pErrBackup,zone,r=0;
  double level;
  SurfaceEx sx;

  if (pErr==NULL) pErr=&pErrBackup;
  if (pErr!=NULL) *pErr=0;

  zone=GetSurfaceZoneByXY(a,x,y,&level,pErr);
  if (zone>=0) {
    sx=AddSurfaceEx(a,zone,level,&r);
  } else {
    sx=AddSurfaceExXY(a,x,y,&r);
  }

  if (pErr!=NULL && r) *pErr=r;

  return sx;
}

int ChangeSurfaceExByXY(SurfaceEx sx,double x,double y,int* pErr) {
  int pErrBackup,zone,r;
  double level;

  if (pErr==NULL) pErr=&pErrBackup;

  zone=GetSurfaceZoneByXY(sx->app,x,y,&level,pErr);
  if (zone>=0) {
    r=ChangeSurfaceEx(sx,zone,level);
  } else {
    r=ChangeSurfaceExXY(sx,x,y);
  }
  if (r) *pErr=r;

  return r;
}

void UpdateSurfaceExPos(SurfaceEx s) {
  App a=s->app;

  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,s,DRAW_OFF);

  RecalcSurfaceExPos(s);

  DrawAppObject(a,s,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);

  AddAppUpdate(a);
}

void UpdateGridPointExPos(GridPointEx gpx) {
  App a=gpx->app;

  DrawAppHighlight(a,DRAW_OFF);
  DrawAppObject(a,gpx,DRAW_OFF);

  RecalcGridPointExPos(gpx);

  DrawAppObject(a,gpx,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);

  AddAppUpdate(a);
}

void UpdateAfterTopologyChange(App a) {
  SurfaceEx sx;
  GridPointEx gpx;
  Index ix;

  for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix))
    UpdateSurfaceExPos(sx);

  for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix))
    UpdateGridPointExPos(gpx);
}

/* Returns angle "from" (dx1,dy1) "to" (dx2,dy2) */
double AngleBetween(double dx1,double dy1,double dx2,double dy2) {
  double a,a1,a2;

  a1=acos(dx1/hypot(dx1,dy1));
  if (dy1<0) a1=M_PI*2-a1;
  a2=acos(dx2/hypot(dx2,dy2));
  if (dy2<0) a2=M_PI*2-a2;

  a=a2-a1;
  if (a<0) a+=M_PI*2;

  return a;
}

#define ANGLE_0_THRESHOLD 1e-2

/* Y1 -> X2 -> Y2 -> X1 */
double EqXYToBorder(Equil eq,double x,double y) {
  double sum=0;

  if (y==eq->y[0])        return fabs(x-eq->x[0]);
  sum+=fabs(eq->x[eq->sx-1]-eq->x[0]);

  if (x==eq->x[eq->sx-1]) return sum+fabs(y-eq->y[0]);
  sum+=fabs(eq->y[eq->sy-1]-eq->y[0]);

  if (y==eq->y[eq->sy-1]) return sum+fabs(x-eq->x[eq->sx-1]);
  sum+=fabs(eq->x[eq->sx-1]-eq->x[0]);

  if (x==eq->x[0])        return sum+fabs(y-eq->y[eq->sy-1]);
  /* sum+=fabs(eq->y[eq->sy-1]-eq->y[0]); -- Not needed */

  return MAXDOUBLE; /* Not on equilibrium border */
}

struct _EqBorderSort {
  double b,x,y;
};

int EqBorderCompare(const void* b1,const void* b2) {
  struct _EqBorderSort
    * pb1=(struct _EqBorderSort*)b1,
    * pb2=(struct _EqBorderSort*)b2;

  if (pb1->b<pb2->b) return -1;
  if (pb1->b>pb2->b) return 1;

  return 0;
}

void RecalcTopologyCache(App a) {
  SurfaceZone sz;
  int* pErr;
  GridPointSeg gpsStart;
  struct _XPointSeg xpss;
  XPointTest xpt;
  int i,j1,j2,r,errBuf,n;
  XY xy,xy1,xyt;
  double gx,gy,l,sina,xStart,yStart,dx,dy,angle,angleHit,*eqBorderPts;
  double x1,y1,x2,y2,tx,ty,xStart2,yStart2;
  Group gResult,line,lines,ln,g,l1,lnHit;
  int bReverse;
  Index ix,ix1,ixsz;
  struct _EqBorderSort* bdr;
  Source src;

  /*if (pErr==NULL)*/ pErr=&errBuf;
  *pErr=0;

  /* Free all existing bounds */
  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
    if (sz->bounds!=NULL) FreeSurfaceZoneBounds(sz);

  if (a->equil==NULL) {*pErr=ERR_OUTOFEQUIL;return;} /* $ noequil! */

  /**** Generate all separatrix lines, store in "lines" */

  n=0;
  lines=CreateGroup();

  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix)) {
    for (i=0;i<4;i++) {
      xpss.xpt=xpt;
      xpss.number=i;
      xpss.startPos=xpss.endPos=-1;
      xpss.line=NULL;
      r=CalcXPointSegLine(a,&xpss);
      if (r) {
        if (xpss.line!=NULL) FreeMallocedGroup(xpss.line);
        continue;
      }
      GroupAdd(lines,xpss.line);
      n++;
    }
  }

  /* Detect and sort all equilibrium edge intersections */

  bdr=Malloc(sizeof(*bdr)*(2*n+1));
  n=0;
  g=CreateGroup();
  for (ln=Group1st(lines,&ix);ln!=NULL;ln=Next(&ix)) {

    /* Is the "other" end of the line at the equilibrium edge? */
    assert(!IsEmptyGroup(ln));
    xy=GroupAt(ln,GroupCount(ln)-1);
    bdr[n].b=EqXYToBorder(a->equil,xy->x,xy->y);

    /* No - if it is not a loop, skip it */
    if (bdr[n].b==MAXDOUBLE) {
       xyt=GroupAt(ln,0);
       if (xyt->x!=xy->x || xyt->y!=xy->y) continue;
    } else {
      /* Yes - add to the intersection array */
      bdr[n].x=xy->x;
      bdr[n].y=xy->y;
      n++;
    }

    /* Add the reversed line to the lines array */
    l1=CreateStack();
    for (xy=Group1st(ln,&ix1);xy!=NULL;xy=Next(&ix1))
      AddXY(l1,xy->x,xy->y);

    GroupAdd(g,l1);
  }
  MergeGroup(lines,g);
  FreeGroup(g);

  /* Sort eq.edge intersections in the CW order */
  qsort(bdr,n,sizeof(*bdr),EqBorderCompare);

  /* Connect equilibrium edge intersections with addl lines */

  x1=a->equil->x[0];
  y1=a->equil->y[0];
  x2=a->equil->x[a->equil->sx-1];
  y2=a->equil->y[a->equil->sy-1];
  if (x1>x2) {l=x1;x1=x2;x2=l;}
  if (y1>y2) {l=y1;y1=y2;y2=l;}

  for (i=0;i<n;i++) {
    j1=(i+1)%n;
    ln=CreateGroup(); /* For the logical CCW line: Y1->X2->Y2->X1 */

    tx=bdr[i].x;
    ty=bdr[i].y;
    AddXY(ln,tx,ty);

    do {
      if (ty==y1 && tx<x2) {
        tx= (ty==bdr[j1].y && tx<bdr[j1].x)? bdr[j1].x : x2;
      } else if (tx==x2 && ty<y2) {
        ty= (tx==bdr[j1].x && ty<bdr[j1].y)? bdr[j1].y : y2;
      } else if (ty==y2 && tx>x1) {
        tx= (ty==bdr[j1].y && tx>bdr[j1].x)? bdr[j1].x : x1;
      } else if (tx==x1 && ty>y1) {
        ty= (tx==bdr[j1].x && ty>bdr[j1].y)? bdr[j1].y : y1;
      }
      AddXY(ln,tx,ty);
    } while (tx!=bdr[j1].x || ty!=bdr[j1].y);

    /* Create a reverse line in CW order */
    l1=CreateStack();
    for (xy=Group1st(ln,&ix);xy!=NULL;xy=Next(&ix))
      AddXY(l1,xy->x,xy->y);

    GroupAdd(lines,ln);
    GroupAdd(lines,l1);
  }
  Free(bdr);

  /**** Now, generate bounds for each SurfaceZoe */

  for (sz=AppSurfaceZone1st(a,&ixsz);sz!=NULL;sz=Next(&ixsz)) {

    gpsStart=FindGridPointSeg(a,sz->gpZone1);
    if (gpsStart==NULL) {*pErr=ERR_BADGPZONENUMBER;continue;}

    /* Check the level to the "left" side */

    i=GroupCount(gpsStart->line);
    if (i<3) {*pErr=ERR_BADSURFACEZONE1;continue;} /* $ - change error msg */

    j1=i/2;
    j2=j1+1;
    if (j2<i-1) j2++;

    xy=GroupAt(gpsStart->line,j1);
    if (GetEquilLevel(a->equil,xy->x,xy->y,&l,&gx,&gy)) {
      *pErr=ERR_BADSURFACEZONE1;  /* $ - change error msg */
      continue;
    }

    /* Determine the initial direction */

    xy1=GroupAt(gpsStart->line,j2);
    sina=sz->sign*((xy1->x-xy->x)*gy-gx*(xy1->y-xy->y));
    bReverse=(sina<0);

    /* Generate the first line */

    xpss.xpt=gpsStart->xps->xpt;
    xpss.number=gpsStart->xps->number;
    xpss.startPos=xpss.endPos=-1;
    xpss.line=NULL;

    r=CalcXPointSegLine(a,&xpss);
    if (r) {
/*  puts("Unable to recreate xps"); */
      continue;
    }
    line=xpss.line;
    if (bReverse) RevertGroup(line);

    /* Store the "first" point coords - we will return here */

    xy=Group1st(line,&ix);
    xStart=xy->x;
    yStart=xy->y;
    xy=Next(&ix);
    xStart2=xy->x;
    yStart2=xy->y;

    /* Start walking from the original line */
    ln=line;
    gResult=CreateGroup();

    /* To prevent dead loops, limit the number of steps */
    for (j2=0;j2<GroupCount(lines);j2++) {

      /* Follow the line, add points except the last one to the result */

      for (xy1=xy=Group1st(ln,&ix);(xyt=Next(&ix))!=NULL;xy1=xy,xy=xyt)
        AddXY(gResult,xy->x,xy->y);

      /* Figure out where to go next */

      /* Detect the "leftmost" outgoung segment from this point */
      dx=xy1->x-xy->x;
      dy=xy1->y-xy->y;
      xyt=xy;

      lnHit=NULL;angleHit=MAXDOUBLE;
      for (ln=Group1st(lines,&ix);ln!=NULL;ln=Next(&ix)) {
        xy=Group1st(ln,&ix1);
        if (xy->x!=xyt->x || xy->y!=xyt->y) continue;
        xy1=Next(&ix1);
        angle=AngleBetween(xy1->x-xy->x,xy1->y-xy->y,dx,dy);
        if (angle<angleHit && angle>ANGLE_0_THRESHOLD) {
          lnHit=ln;
          angleHit=angle;
        }
      }

      /* Select the next line to follow */
      if (lnHit==NULL) break;
      ln=lnHit;

      /* Back at the beginning? */
      xy=Group1st(ln,&ix);
      xy1=Next(&ix);
/*src=AddSource(a,xy->x,xy->y);*/
      if (xy->x==xStart && xy->y==yStart && xy1->x==xStart2 && xy1->y==yStart2) {
/*MarkObject(a,src,1);*/
        break;
      }
      continue;
    }

    sz->bounds=gResult;
    line=FreeMallocedGroup(line);
  }

  /* Free the "lines" group */

  for (ln=Group1st(lines,&ix);ln!=NULL;ln=Next(&ix)) {
    GroupDel(lines,ln);
    FreeMallocedGroup(ln);
  }
  lines=FreeGroup(lines);

  return;
}

static void FreeSurfaceZoneBounds(SurfaceZone sz) {
  if (sz->bounds==NULL) return;

  sz->bounds=FreeMallocedGroup(sz->bounds);
}

void InvalidateTopologyCache(App a) {
  SurfaceZone sz;
  Index ix;

  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
    if (sz->bounds!=NULL) FreeSurfaceZoneBounds(sz);
}


/* $ - dirty - redraws the segment too */
void RecalcGridPointSegLine(GridPointSeg gps,VarDef* pvd,VarSet* pvs) {
  Elem e;
  VarDef vd,vdMin=NULL;
  VarSet vs,vsMin=NULL;
  GridPointEx gpx;
  double l,lMin;
  Index ixvd,ixvs,ixe,ixgp;
  Group g,g1;
  int r;

  vdMin=NULL;
  lMin=MAXDOUBLE;

  if (gps->line==NULL || IsEmptyGroup(gps->line)) return;
  /* if (!GridPointSegOpen(gps)) return; */ /* Skip non-open segments */

  /* Use all "target" vars in all VarSets */

  for (vs=AppVarSet1st(gps->app,&ixvs);vs!=NULL;vs=Next(&ixvs)) {
    for (vd=Group1st(vs->def->varDefs,&ixvd);vd!=NULL;vd=Next(&ixvd)) {
      if (!(vd->varType & VTF_TARGET)) continue;
      g=GetVar(vs,vd,vs);
      if (g==NULL) continue;
      if (g!=NULL) for(e=Group1st(g,&ixe);e!=NULL;e=Next(&ixe)) {
        g1=CreateGroup();
        AddXY(g1,e->n[1]->x,e->n[1]->y);
        AddXY(g1,e->n[2]->x,e->n[2]->y);
        r=PolyLinesIntersect(gps->line,g1,&l,NULL);
        FreeMallocedGroup(g1);
        if (r) continue;
        if (l<lMin) {
          lMin=l;
          vdMin=vd;
          vsMin=vs;
        }
      }
    }
  }

  /* Update the position and redraw, if necessary */
  if (lMin==MAXDOUBLE) lMin=CalcLineLength(gps->line);
  if (lMin!=gps->lineLength) {
    for (gpx=AppGridPointEx1st(gps->app,&ixgp);gpx!=NULL;gpx=Next(&ixgp))
      if (gpx->zone==gps->zone) DrawAppObject(gps->app,gpx,DRAW_OFF);
    DrawAppObject(gps->app,gps->xps,DRAW_OFF);

    gps->lineLength=lMin;

    DrawAppObject(gps->app,gps->xps,DRAW_ON);
    for (gpx=AppGridPointEx1st(gps->app,&ixgp);gpx!=NULL;gpx=Next(&ixgp))
      if (gpx->zone==gps->zone) {
        RecalcGridPointExPos(gpx);
        DrawAppObject(gps->app,gpx,DRAW_ON);
      }
  }
  if (pvs!=NULL) *pvs=vsMin;
  if (pvd!=NULL) *pvd=vdMin;
}

static int NextGridPointSegNumber(App a) {
  GridPointSeg gps;
  int nMax=GPSEG_STARTNO;
  Index ix;

  for (gps=AppGridPointSeg1st(a,&ix);gps!=NULL;gps=Next(&ix))
    if (gps->zone+1>nMax) nMax=gps->zone+1;

  return nMax;
}

int GetCarreSurfaceExCount(App a,int zone,int nSpec) {
  SurfaceZone sz=FindSurfaceZone(a,zone);
  if (sz==NULL) return 0;

  return (sz->gpZone2>=0)? nSpec : nSpec+1;
}

/* Return 1 if the segment crosses the equilibrium edge */
int GridPointSegOpen(GridPointSeg gps) {
  XY xy;
  Equil eq=gps->app->equil;

  assert(eq!=NULL);

  /* Check first point - should never return true */
  xy=Group1st(gps->line,NULL);
  if (xy->x==eq->x[0] || xy->y==eq->y[0] ||
      xy->x==eq->x[eq->sx-1] || xy->y==eq->y[eq->sy-1])
    return 1;

  /* Then the last point */
  xy=GroupAt(gps->line,GroupCount(gps->line)-1);
  if (xy->x==eq->x[0] || xy->y==eq->y[0] ||
      xy->x==eq->x[eq->sx-1] || xy->y==eq->y[eq->sy-1])
    return 1;

  return 0;
}

int SurfaceExClosed(SurfaceEx sx) {
  XY xy,xy1;

  assert(sx->type==T_SURFACEEX);

  if (!SurfaceExOk(sx)) return 0;
  if (sx->line==NULL || GroupCount(sx->line)<1) return 0;

  xy=GroupAt(sx->line,0);
  xy1=GroupAt(sx->line,GroupCount(sx->line)-1);

  return xy->x==xy1->x && xy->y==xy1->y;
}

int RecalcSurfaceZoneSign(SurfaceZone sz) {
  GridPointSeg gps;
  int i,j1,j2;
  XY xy,xy1;
  double sina,gx,gy,l;

  if (sz->app->equil==NULL) return ERR_NOEQUIL;

  gps=FindGridPointSeg(sz->app,sz->gpZone1);
  if (gps==NULL) return ERR_BADGPZONENUMBER;

  /* Check the level to the "left" side */

  i=GroupCount(gps->line);
  if (i<3) return ERR_BADSURFACEZONE1; /* $ - change error msg */

  j1=i/2;
  j2=j1+1;
  if (j2<i-1) j2++;

  xy=GroupAt(gps->line,j1);
  if (GetEquilLevel(sz->app->equil,xy->x,xy->y,&l,&gx,&gy))
     return ERR_BADSURFACEZONE1;  /* $ - change error msg */

  /* Angle between the line and the gradient */

  xy1=GroupAt(gps->line,j2);
  sina=gps->dir*((xy1->x-xy->x)*gy-gx*(xy1->y-xy->y));
  sz->sign=sz->orient*(sina<0? -1 : 1);

  return 0;
}

int SurfaceZoneBoundsCached(App a) {
  SurfaceZone sz;
  Index ix;

  assert(a->type==T_APP);

  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
    if (sz->bounds==NULL) return 0;

  return 1;
}
