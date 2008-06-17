#include "dg.h"

#define CSF_YP    0x1
#define CSF_YM    0x2
#define CSF_XM    0x4
#define CSF_XP    0x8

#define CS_YP    0
#define CS_XP    1
#define CS_YM    2
#define CS_XM    3

int AddEquil(App a,char* fName) {
  Equil eq;
  struct _ActRec ar;
  int i;

  ValidatePtr(a,"AddEquil");
  ValidatePtr(fName,"AddEquil_");

  if (a->equil!=NULL) {
    i=DelEquil(a);
    if (i) return i;
  }

  eq=Malloc(sizeof(*eq));
  eq->type=T_EQUIL;
  eq->locks=0;
  eq->app=a;
  eq->sx=eq->sy=0;
  eq->hSplines=eq->vSplines=NULL;

  ar.obj=eq;

  strncpy(eq->fName,fName,sizeof(eq->fName)-1);
  i=ActAddEquil(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

  if (a->fName!=NULL) {
    strncpy(eq->fName,a->fName,sizeof(eq->fName)-1);
    strcat(GetFilePath(eq->fName),GetShortFName(fName));
    i=ActAddEquil(a,&ar);
    if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
  }

  strncpy(eq->fName,GetShortFName(fName),sizeof(eq->fName)-1);
  i=ActAddEquil(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

/*  strncpy(eq->fName,a->xapp->argv[0],sizeof(eq->fName)-1);
  strcat(GetFilePath(eq->fName),GetShortFName(fName));
  i=ActAddEquil(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
*/
  if (ar.obj!=NULL) ar.obj=Free(ar.obj);
  
  if (a->equil!=NULL) CalcEquilSplines(a->equil);
  
  return i;
}

int DelEquil(App a) {
  struct _DelEquilRec ar;
  Surface s;
  XPointTest xpt;
  Index ix;

  ValidatePtr(a,"DelEquil");
  if (a->equil==NULL) FatalError("DelEquil()-null: fatal error 1");
  if (a->bStrict && IsLocked(a->equil)) return ERR_LOCKED;

  if (a->xpoint!=NULL) DelXPoint(a);
  for (s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix))
    DelSurface(a,s);

  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    DelXPointTest(a,xpt);

  ar.eq=a->equil;
  ActDelEquil(a,&ar);
  return 0;
}

Surface AddSurface(App a,double x,double y,int* pErr) {
  struct _ActRec ar;
  Surface s;
  int i;

  ValidatePtr(a,"AddSurface");

  s=Malloc(sizeof(*s));
  s->type=T_SURFACE;
  s->locks=0;
  s->originX=x;
  s->originY=y;
  s->line=NULL;
  s->creatorId=NULL;

  ar.obj=s;
  i=ActAddSurface(a,&ar);

  if (i) s=Free(s);
  if (pErr!=NULL) *pErr=i;

  return s;
}

void* DelSurface(App a,Surface s) {
  struct _DelSurfaceRec ar;

  ChangeSurfaceCreatorId(a,s,NULL);

  ar.s=s;
  ActDelSurface(a,&ar);

  return NULL;
}

int ChangeSurface(App a,Surface s,double x,double y) {
  struct _ChangeSurfaceRec ar;
  int i;

  ar.s=s;
  ar.x=x;
  ar.y=y;
  i=ActChangeSurface(a,&ar);

  if (i) return i;

  if (*GetSurfaceCreatorId(s)==CID_UNCHANGEDFLAG)
    ChangeSurfaceCreatorId(a,s,GetSurfaceCreatorId(s)+1);

  return i;
}

int ChangeSurfaceCreatorId(App a,Surface s,char* id) {
  if (id!=NULL && !strcmp(id,GetSurfaceCreatorId(s))) return 0;

  SetObjString(a,s,GetOffset(Surface,creatorId),id,0);

  return 0;
}

int CalcEquilSplines(Equil eq) {
  Group gXY;
  int i,j;

  if (eq->hSplines!=NULL && eq->vSplines!=NULL) return 0;

  FreeEquilSplines(eq);

  eq->hSplines=Malloc(sizeof(*eq->hSplines)*eq->sy);
  for (j=0;j<eq->sy;j++) {
    gXY=CreateGroup();
    for (i=0;i<eq->sx;i++)
      AddXY(gXY,eq->x[i],EqCell(eq,i,j));
    eq->hSplines[j]=CreateSimpleSplineInfo(gXY);
    assert(eq->hSplines[j]!=NULL);
    gXY=FreeMallocedGroup(gXY);
  }

  eq->vSplines=Malloc(sizeof(*eq->vSplines)*eq->sx);
  for (j=0;j<eq->sx;j++) {
    gXY=CreateGroup();
    for (i=0;i<eq->sy;i++)
      AddXY(gXY,eq->y[i],EqCell(eq,j,i));
    eq->vSplines[j]=CreateSimpleSplineInfo(gXY);
    assert(eq->vSplines[j]!=NULL);
    gXY=FreeMallocedGroup(gXY);
  }
  return 0;
}

int FreeEquilSplines(Equil eq) {
  int i;

  if (eq->hSplines!=NULL) {
    for (i=0;i<eq->sy;i++) FreeSimpleSplineInfo(eq->hSplines[i]);
    eq->hSplines=Free(eq->hSplines);
  }

  if (eq->vSplines!=NULL) {
    for (i=0;i<eq->sx;i++) FreeSimpleSplineInfo(eq->vSplines[i]);
    eq->vSplines=Free(eq->vSplines);
  }

  return 0;
}

int GetEquilCell(Equil eq,double x,double y,int* px,int* py) {
  int i;

  for (i=0;i<eq->sx-1;i++)
    if (inrange(x,eq->x[i],eq->x[i+1])) {*px=i;break;}
  if (i>=eq->sx-1) return -1;
  for (i=0;i<eq->sy-1;i++)
    if (inrange(y,eq->y[i],eq->y[i+1])) {*py=i;break;}
  if (i>=eq->sy-1) return -1;

  return 0;
}

int GetEquilLevel(Equil eq,double x,double y,double* pLevel,
    double* pGradX,double* pGradY) {
  int cx,cy;
  double a1,a2,a3,a4,a13,a24,a13d,a24d;

  if (GetEquilCell(eq,x,y,&cx,&cy)) return -1;

  a1=EqCell(eq,cx,cy);
  a2=EqCell(eq,cx+1,cy);
  a3=EqCell(eq,cx,cy+1);
  a4=EqCell(eq,cx+1,cy+1);
  a13=a1+(a1-a3)*(eq->y[cy]-y)/(eq->y[cy+1]-eq->y[cy]);
  a24=a2+(a2-a4)*(eq->y[cy]-y)/(eq->y[cy+1]-eq->y[cy]);
  if (pLevel!=NULL)
    *pLevel=a13+(a13-a24)*(eq->x[cx]-x)/(eq->x[cx+1]-eq->x[cx]);

  if (pGradX!=NULL) {
    *pGradX=-(a13-a24)/(eq->x[cx+1]-eq->x[cx]);
    a13d=-(a1-a3)/(eq->y[cy+1]-eq->y[cy]);
    a24d=-(a2-a4)/(eq->y[cy+1]-eq->y[cy]);
    *pGradY=a13d+(a13d-a24d)*(eq->x[cx]-x)/(eq->x[cx+1]-eq->x[cx]);
  }

  return 0;
}

double EqCorrCell(Equil eq,int cx,int cy,double level) {
  double a;

  return (a=EqCell(eq,cx,cy))==level ? a+(eq->maxVal-eq->minVal)*1e-7 : a;
}

/* Return:
   0 - area unknown (unknown sign inside the separatrix)
   1 - closed surface
   2 - sign opposite to that of closed surfaces
   2 - sign the same as that of closed surfaces
*/

int GetSurfaceArea(App a,Surface s) {
  if (a->equil->signInside==0) return 0;
  if (s->closed) return 1;

  return s->level*a->equil->signInside>0 ? 3 : 2;
}

char* GetSurfaceCreatorId(Surface s) {
  return s->creatorId==NULL? "*" : s->creatorId;
}

char* ConstructSurfaceCreatorId(int area,int count,double alpha1,
    double alpha2,int law,double l1,double l2,int carreFlag) {
  static char s[1024];

  sprintf(s,"+a%dl%gL%gn%dh%gH%gw%dC%d",
    area,l1,l2,count,alpha1,alpha2,law,carreFlag);
  return s;
}

int ParseSurfaceCreatorId(char* id,int* pArea,int* pCount,double* pAlpha1,
    double* pAlpha2,int* pLaw,double* pL1,double* pL2,int* pCarreFlag) {
  int i;

  if (*id==CID_UNCHANGEDFLAG) id++;

  *pCarreFlag=0;
  i=sscanf(id,"a%dl"SCANFLT"L"SCANFLT"n%dh"SCANFLT"H"SCANFLT"w%dC%d",
      pArea,pL1,pL2,pCount,pAlpha1,pAlpha2,pLaw,pCarreFlag);

  return i<7? -1 : 0;
}

int DistributeSurfaces(App a,int area,int count,double a1,double a2,
    int law,double level1,double level2,int carreMode,char** oldId) {
  char* creatorId;
  int i,cnt,r;
  double v,t;
  Surface s;
  Index ix;

  /* In Carre mode, determine the levels automatically */

  if (carreMode) {
    level1=0;

    switch (area) {
      case 1:
      case 2:
      case 3:
	r=FindCarreMinMaxSurfaceLevel(a,area,&level1,&level2);
	if (r) {
	  if (r==ERR_OUTOFEQUIL) r=ERR_CARRE_OUTOFEQUIL;
	  return r;
	}
	break;
      default:
	assert(0);
    }
  }

  /* Delete existing surfaces in Carre mode. Cannot be done earlier
     due to level calculations in main plasma */

  if (carreMode) for (s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix))
    if (GetSurfaceArea(a,s)==area) DelSurface(a,s);

  /* Construct a creatorId from creation parameters */

  creatorId=ConstructSurfaceCreatorId(area,count,a1,a2,law,
      level1,level2,!!carreMode);

  /* Create surfaces */

  cnt=count;

  /* Carre mode: one extra bounding surface */
  if (carreMode) cnt++;

  for (i=0;i<cnt;i++) {
    v=DistributeLaw((i+1)/(double)(count+1),law,a1,a2,count);
    v=max(v,0);
    v=min(v,1);

    /* -- Created problems in the (count+1)th point in Carre mode --
    if (v<0 || v>1) {
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,
	GetResourceString(dlg->dg.wLaw,"errBadLaw",NULL,NULL));
      return;
    } --- */

    t=level1+(level2-level1)*v;

    s=AddSurfaceByLevel(a,area,t,&r);
    if (s==NULL) {
      return r;
    }
    ChangeSurfaceCreatorId(a,s,creatorId);
  }

  if (oldId!=NULL) *oldId=creatorId;

  return 0;
}

/* Area==-1 means recursively process all areas */

int RebuildCarreSurfaces(App a,int area) {
  Surface s;
  int count,law,carreMode,r,foo;
  double a1,a2,l1,l2;
  Index ix;
  char* id;

  if (a->outputMode!=OUTPUTMODE_CARRE) return ERR_CARRE_MODE_NEEDED;

  if (area<0) {
    r=RebuildCarreSurfaces(a,1);if (r) return r;
    r=RebuildCarreSurfaces(a,2);if (r) return r;
    r=RebuildCarreSurfaces(a,3);if (r) return r;
    return 0;
  }

  if (!CountSurfaces(a,area)) return 0;  /* $$ - maybe some errorcode? */

  for (s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix)) {
    if (GetSurfaceArea(a,s)!=area) continue;
    id=GetSurfaceCreatorId(s);
    if (ParseSurfaceCreatorId(id,&foo,&count,
	&a1,&a2,&law,&l1,&l2,&carreMode))
      continue;
    if (carreMode) break;
  }

  if (s==NULL) return 0;  /* $$ - maybe some errorcode? */

  r=DistributeSurfaces(a,area,count,a1,a2,law,l1,l2,carreMode,NULL);

  return r;
}

Surface AddSurfaceByLevel(App a,int area,double level,int* pErr) {
  double x,y;
  int r;

  r=FindSurfaceOriginPoint(a,area,level,&x,&y);
  if (r) {
    if (pErr!=NULL) *pErr=r;
    return NULL;
  }

  return AddSurface(a,x,y,pErr);
}

int WriteDgEquil(Equil eq,char* fName,int sx,int sy) {
  FILE* f;
  unsigned i,j;

  f=fopen(fName,"w");
  if (f==NULL) return ERR_FWRITE;

  fprintf(f,"DivGeo equilibrium file>>\nWidth=%d\nHeight=%d\n",sx,sy);
  for (i=0;i<sx;i++) fprintf(f,"%e\n",eq->x[i*eq->sx/sx]);
  fprintf(f,"\n");
  for (i=0;i<sy;i++) fprintf(f,"%e\n",eq->y[i*eq->sy/sy]);
  fprintf(f,"\n");

  for (i=0;i<sy;i++) for (j=0;j<sx;j++)
    fprintf(f,"%e\n",EqCell(eq,j*eq->sx/sx,i*eq->sy/sy));

  fclose(f);

  return 0;
}

#define MIN_STEPS_IN_EQ_CELL 20

static int FindEquilMinMaxSegment(Equil eq,int signMinMax,
    double x1,double y1,double x2,double y2,double* pLevel) {
  int i,stepCount;
  double x,y,l,xMax,yMax,lMax;

  stepCount=hypot(x1-x2,y1-y2)*MIN_STEPS_IN_EQ_CELL/
      min(EqAverageCellWidth(eq),EqAverageCellHeight(eq));
  stepCount=max(MIN_STEPS_IN_EQ_CELL,stepCount);

  lMax=-signMinMax*MAXDOUBLE;

  for (i=0;i<=stepCount;i++) {
    x=x1+(x2-x1)*i/stepCount;
    y=y1+(y2-y1)*i/stepCount;
    if (GetEquilLevel(eq,x,y,&l,NULL,NULL)) continue;
    if ((l-lMax)*signMinMax>0) {
      lMax=l;
      xMax=x;
      yMax=y;
    }
  }

  if (fabs(lMax)==MAXDOUBLE) return ERR_OUTOFEQUIL;
  if (pLevel!=NULL) *pLevel=lMax;

  return 0;
}

int FindCarreMinMaxSurfaceLevel(App a,int nArea,double* pL1,double* pL2) {
  Var v;
  Elem e;
  double l,lMin,lMax,l1,l2;
  int signMinMax,r;
  Surface s;
  Group g,gST,gSC,gSO,gt;
  Index ix,ixg;

  assert(a->outputMode==OUTPUTMODE_CARRE);

  /* Make sure requirements are met */

  if (a->equil==NULL) return ERR_NOEQUIL;
  if (!a->equil->signInside) return ERR_NOCLOSEDSURFS;

  /* Find signMinMax and do special processing for the main plasma */

  switch(nArea) {
    case 1:
      /* Special case Area 1: use the innermost closed surface */

      lMax=lMin=0;
      for (s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix)) {
	if (GetSurfaceArea(a,s)==nArea &&
	    fabs(s->level-lMin)>fabs(lMax-lMin)) lMax=s->level;
      }
      if (lMax==lMin) return ERR_CARRE_AREA1SURFACES;
      goto AllFound;
      break;
    case 2:
      signMinMax=-a->equil->signInside;
      break;
    case 3:
      signMinMax=a->equil->signInside;
      break;
    default:
      assert(0);
  }

  /* Fetch & split structure, fetch targets */

  lMin=0;
  lMax=signMinMax*MAXDOUBLE;

  v=GetVarPtrByType(a,VT_STRUCTURE);
  if (v==NULL || v->val==NULL || IsEmptyGroup(v->val))
    return ERR_NOSTRUCTURE;

  r=OrderStructureElems(a,v->val,&gST,&gSO,&gSC,NULL);
  if (r) return r;

  /* Make sure all surfaces pass through structures containing targets */

  for (g=Group1st(gST,&ixg);g!=NULL;g=Next(&ixg)) {
    l1=-signMinMax*MAXDOUBLE;l2=signMinMax*MAXDOUBLE;
    for (e=Group1st(g,&ix);e!=NULL;e=Next(&ix)) {
      if (!FindEquilMinMaxSegment(a->equil,signMinMax,
	  e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,&l))
	if ((l-l1)*signMinMax>0) l1=l;
      if (!FindEquilMinMaxSegment(a->equil,-signMinMax,
	  e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,&l))
	if ((l-l2)*signMinMax<0) l2=l;
    }
    if ((l1-lMax)*signMinMax<0) lMax=l1;
    if ((l2-lMin)*signMinMax>0) lMin=l2;
  }


  /* Make sure they are restricted by other structures */

  gt=CopyGroup(gSC,NULL);
  MergeGroup(gt,gSO);

  for (g=Group1st(gt,&ixg);g!=NULL;g=Next(&ixg)) {
    for (e=Group1st(g,&ix);e!=NULL;e=Next(&ix)) {
      if (FindEquilMinMaxSegment(a->equil,-signMinMax,
	  e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,&l)) continue;
      if (l*signMinMax<=0) continue; /* Skip elements in other areas */
      if ((l-lMax)*signMinMax<0) lMax=l;
    }
  }

  /* Clean up */

  FreeGroup(gt);
  FreeStructureInfo(&gST,&gSC,&gSO);

  if (fabs(lMax)==MAXDOUBLE) return ERR_OUTOFEQUIL;  /* $ - risky */
  assert(fabs(lMin)!=MAXDOUBLE);

  AllFound:

  if (pL1!=NULL) *pL1=lMin;
  if (pL2!=NULL) *pL2=lMax;

  return 0;
}

/* Private functions - to be called only from Act_xxx
*/

int LoadDgEquilFile(Equil eq) {
  FILE* f;
  char s[256];
  int i;

  ValidatePtr(eq,"LoadDgEquil");

  f=fopen(eq->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  fgets(s,sizeof(s),f);
  if (fscanf(f,"Width=%d\nHeight=%d",&eq->sx,&eq->sy)!=2) goto err;
  if (!eq->sx || !eq->sy) goto err;
  eq->x=Malloc(eq->sx*sizeof(*eq->x));
  eq->y=Malloc(eq->sy*sizeof(*eq->y));
  eq->val=Malloc(eq->sx*eq->sy*sizeof(*eq->val));

  for (i=0;i<eq->sx;i++) if (fscanf(f,SCANFLT,&eq->x[i])!=1) goto err;
  for (i=0;i<eq->sy;i++) if (fscanf(f,SCANFLT,&eq->y[i])!=1) goto err;
  for (i=0;i<eq->sx*eq->sy;i++)
    if (fscanf(f,SCANFLT,&eq->val[i])!=1) goto err;
  fclose(f);

  return 0;
  err:
  fclose(f);
  return ERR_BADFILE;
}

int Load1EquilFile(Equil eq) {
  FILE* f;
  char s[256];
  int i;
  double psib;
  char c;

  ValidatePtr(eq,"Load1EquilFile");

  f=fopen(eq->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;
  eq->sx=eq->sy=0;

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    sscanf(s,"     jm    =  %d",&eq->sx);
    sscanf(s,"     km    =  %d",&eq->sy);
    sscanf(s,"     psib  = "SCANFLT,&psib);
    if (sscanf(s,"     r(1:jm)%c",&c)==1) goto scanRows;
  }
  goto err;

  scanRows:

  if (eq->sx==0 || eq->sy==0) goto err;
  eq->x=Malloc(eq->sx*sizeof(*eq->x));
  eq->y=Malloc(eq->sy*sizeof(*eq->y));
  eq->val=Malloc(eq->sx*eq->sy*sizeof(*eq->val));

  for (i=0;i<eq->sx;i++) {
    if (fscanf(f,SCANFLT,&eq->x[i])!=1) goto err;
    eq->x[i]*=1e3;
  }

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"     z(1:km)%c",&c)==1) goto scanCols;
  }
  goto err;

  scanCols:
  for (i=0;i<eq->sy;i++) {
    if (fscanf(f,SCANFLT,&eq->y[i])!=1) goto err;
    eq->y[i]*=1e3;
  }

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"     ((psi(j,k)-psib,j=1,jm),k=1,km%c",&c)==1)
      goto scanMain;
  }
  goto err;

  scanMain:
  for (i=0;i<eq->sx*eq->sy;i++)
    if (fscanf(f,SCANFLT,&eq->val[i])!=1) goto err;

  fclose(f);
  return 0;
  err:
  fclose(f);
  return ERR_BADFILE;
}

void CalcEquilValues(Equil eq) {
  int i;

  eq->minVal=eq->maxVal=eq->val[0];
  for (i=0;i<eq->sx*eq->sy;i++) {
    eq->minVal=min(eq->minVal,eq->val[i]);
    eq->maxVal=max(eq->maxVal,eq->val[i]);
  }
  eq->signInside=0;
}

void CalcSurfData(Equil eq,int cx,int cy,double level,
    struct _SurfCell* sc) {
  double a1,a2;
  int d;

  sc->n=sc->f=0;
  if (cx<0 || cy<0 || cx>=eq->sx-1 || cy>=eq->sy-1) return;
  a1=EqCorrCell(eq,cx,cy,level);
  a2=EqCorrCell(eq,cx+1,cy,level);
  if (inrange(level,a1,a2)) {
    d=CS_YM;
    sc->x[d]=eq->x[cx]+(eq->x[cx+1]-eq->x[cx])*(level-a1)/(a2-a1);
    sc->y[d]=eq->y[cy];
    sc->f |= CSF_YM;
    sc->d[sc->n++]=d;
  }
  a2=EqCorrCell(eq,cx,cy+1,level);
  if (inrange_s(level,a1,a2)) {
    d=CS_XM;
    sc->x[d]=eq->x[cx];
    sc->y[d]=eq->y[cy]+(eq->y[cy+1]-eq->y[cy])*(level-a1)/(a2-a1);
    sc->f |= CSF_XM;
    sc->d[sc->n++]=d;
  }
  a1=EqCorrCell(eq,cx,cy+1,level);
  a2=EqCorrCell(eq,cx+1,cy+1,level);
  if (inrange(level,a1,a2)) {
    d=CS_YP;
    sc->x[d]=eq->x[cx]+(eq->x[cx+1]-eq->x[cx])*(level-a1)/(a2-a1);
    sc->y[d]=eq->y[cy+1];
    sc->f |= CSF_YP;
    sc->d[sc->n++]=d;
  }
  a1=EqCorrCell(eq,cx+1,cy,level);
  if (inrange_s(level,a1,a2)) {
    d=CS_XP;
    sc->x[d]=eq->x[cx+1];
    sc->y[d]=eq->y[cy]+(eq->y[cy+1]-eq->y[cy])*(level-a1)/(a2-a1);
    sc->f |= CSF_XP;
    sc->d[sc->n++]=d;
  }

  return;
}

/* Calculate group of points representing a surface
   Return: -1 = invalid origin
            0 = ok, surface not closed
            1 = ok, surface closed
*/

int CalcSurfaceLine(Equil eq,int cx,int cy,double level,Group* gXY) {
  struct _SurfCell sc;
  int oCx,oCy,d,closed=0;
  Group g;

  oCx=cx;
  oCy=cy;
  CalcSurfData(eq,cx,cy,level,&sc);
  if (sc.n!=2 && sc.n!=4) return -1;
  d=sc.d[0];
  g=CreateGroup();

  do {
    AddXY(g,sc.x[d],sc.y[d]);
    if (d==CS_YM) cy--;
    if (d==CS_YP) cy++;
    if (d==CS_XM) cx--;
    if (d==CS_XP) cx++;
    CalcSurfData(eq,cx,cy,level,&sc);
    if (sc.n==2) {
      d= (d^2)==sc.d[0] ? sc.d[1] : sc.d[0];
    }
    if (cx==oCx && cy==oCy) {
      AddXY(g,sc.x[d],sc.y[d]);
      closed=1;
      goto surfaceOk;
    }
  } while (sc.n==2 || sc.n==4);

  RevertGroup(g);
  CalcSurfData(eq,cx=oCx,cy=oCy,level,&sc);
  d= sc.n==2 ? sc.d[1] : sc.d[0]^2;
  do {
    AddXY(g,sc.x[d],sc.y[d]);
    if (d==CS_YM) cy--;
    if (d==CS_YP) cy++;
    if (d==CS_XM) cx--;
    if (d==CS_XP) cx++;
    CalcSurfData(eq,cx,cy,level,&sc);
    if (sc.n==2) {
      d= (d^2)==sc.d[0] ? sc.d[1] : sc.d[0];
    }
  } while (sc.n==2 || sc.n==4);

  surfaceOk:
  *gXY=g;
  return closed;
}

int CheckSurfaceTargetIntersection(Group gXY,Group target) {
  XY xy,xy1;
  Elem e;
  Index ixy,ixe;

  if (target==NULL || IsEmptyGroup(target)) return 0;

  xy=Group1st(gXY,&ixy);
  if (xy==NULL) return ERR_SURFTARGETINS;

  for (;xy1=Next(&ixy),xy1!=NULL;xy=xy1) {
    for (e=Group1st(target,&ixe);e!=NULL;e=Next(&ixe))
      if (!VIntersect(xy->x,xy->y,xy1->x,xy1->y,
        e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,NULL,NULL)) return 0;
  }
  return ERR_SURFTARGETINS;
}
