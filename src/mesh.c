/* New mesh object */

#include "dg.h"

/* Declarations ////////////////////////////////////////////////////// */

#define SONNET_FACTOR 1e3

#define FINGERPRINT_MOD 255

#define MESH_BORDER_THRESHOLD .1

typedef struct _MeshCellData {
  int nx,ny,eN;
  double x1,y1,x2,y2,x3,y3,x4,y4,cX,cY,ratio;
  MeshCell mc;
  struct _MeshCellData* mcn[4];
  int mccf[4];
}* MeshCellData;

typedef struct _MoveMeshPointData {
  int bEnh,bMoved,hm;
  MeshPoint mpt;
  struct {
    MeshPoint mpt;
    double oldPos,oldX,oldY;
    int bMoved;
  }* pts,* ptsA;
  int ptsMin,ptsMax,ptsCount,ptsPtNo/*,bDoubleLeft,bDoubleRight*/;
  double posMin,posMax,leftMinDist,rightMinDist;
  Group gXY;
  ParamSplineInfo si;
}* MoveMeshPointData;

static void HighlightDragMeshPoint(View w,MeshPoint mpt,int flag);
static Group GetMeshPointChain(MeshPoint mpt,int bStopOnCuts);

static int ActAddMesh(App a,ActRec ar);
static int ActDelMesh(App a,DelRec ar);
int ActChangeMeshPoint(App a,ChangeMeshPointRec ar);
static double MeshPointDist(MeshPoint mpt1,MeshPoint mpt2);

static int LoadSonnetMeshFile(char* fName,Group* pG,
    Group cmtG,long* pFingerprint);
static int CreateMeshTopology(Mesh* pM,Group mcdG);
static void HighlightDragMeshPoint_1(View w,MeshPoint mpt,int status);
static void HighlightDragMeshPoint_2(View w,MeshPoint mpt,int status);

/*static*/ int CalcFieldRatio(App a,double x,double y,double* pRatio);

/* MoveMeshPoint tool //////////////////////////////////////////////// */

static void SetupSlideMeshPoint(View w,MoveMeshPointData d) {
  Group g;
  Index ix;
  MeshPoint mpt1;
  SurfaceEx sx;
  XY xy;
  double pos,pos1,dpL,dpR;
  int i;

  assert(!d->bEnh);

  /* If we are on a double border, jump to the outermost mesh point */

  if (w->app->bDoubleMeshBorder) {
    if (d->mpt->nx==1) d->mpt=d->mpt->neighbors[MCN_LEFT];
    else if (d->mpt->nx==d->mpt->mesh->sizeX-1)
      d->mpt=d->mpt->neighbors[MCN_RIGHT];
    if (d->mpt==NULL) return;

    if (d->mpt->ny==1) d->mpt=d->mpt->neighbors[MCN_TOP];
    else if (d->mpt->ny==d->mpt->mesh->sizeY-1)
      d->mpt=d->mpt->neighbors[MCN_BOTTOM];
    if (d->mpt==NULL) return;
  }

  /* Calculate the line along which to slide */

  if (w->app->meshSlidingMode & MSMF_USE_SURFACE) {
    if (w->app->equil==NULL) {
      SetHighlightMode(w->app,d->hm=0);
      HighlightDragMeshPoint(w,d->mpt,1);
      SetViewMsg(w,GetStr(w,ERR_NOEQUIL));
      return;
    }

    sx=AddSurfaceExXY(w->app,d->mpt->backupX,d->mpt->backupY,&i);
    if (sx==NULL) {
      SetHighlightMode(w->app,d->hm=0);
      HighlightDragMeshPoint(w,d->mpt,1);
      SetViewMsg(w,GetStr(w,i));
      return;
    }
    if (!SurfaceExOk(sx) || sx->line==NULL || IsEmptyGroup(sx->line)) {
      sx=DelSurfaceEx(sx);
      SetHighlightMode(w->app,d->hm=0);
      HighlightDragMeshPoint(w,d->mpt,1);
      return;
    }

    d->gXY=CreateGroup();
    for (xy=Group1st(sx->line,&ix);xy!=NULL;xy=Next(&ix))
      AddXY(d->gXY,xy->x,xy->y);

    if (SurfaceExClosed(sx)) for (i=0;i<GroupCount(d->gXY)/2;i++) {
      xy=Group1st(d->gXY,NULL);
      GroupDel(d->gXY,xy);
      GroupAdd(d->gXY,xy);
    }

    DelSurfaceEx(sx);

  } else if (w->app->meshSlidingMode & MSMF_USE_POINTS) {

    g=GetMeshPointChain(d->mpt,0);
    assert(g!=NULL && !IsEmptyGroup(g));

    d->gXY=CreateGroup();
    for (mpt1=Group1st(g,&ix);mpt1!=NULL;mpt1=Next(&ix))
      AddXY(d->gXY,mpt1->backupX,mpt1->backupY);
    FreeGroup(g);

  } else assert(0);

  if (w->app->meshSlidingMode & MSMF_USE_SPLINE)
    d->si=CreateParamSplineInfo(d->gXY);

  /* Determine the current position on the line */

  pos=ProjectPointToPolyLine(d->gXY,d->mpt->backupX,d->mpt->backupY);

  /* Get a chain of neighbors */

  g=GetMeshPointChain(d->mpt,1);
  assert(g!=NULL && !IsEmptyGroup(g));

  if (GroupCount(g)<3) {
    g=FreeGroup(g);
    d->mpt=NULL;
    SetHighlightMode(w->app,d->hm=0);
    HighlightDragMeshPoint(w,d->mpt,1);
    return;
  }

  /* Re-align the chain so that the position value always grows */

  i=GroupIndex(g,d->mpt);
  if (i>0) {
    mpt1=GroupAt(g,i-1);
    pos1=ProjectPointToPolyLine(d->gXY,mpt1->backupX,mpt1->backupY);
    if (pos1>pos) RevertGroup(g);
  } else if (i<GroupCount(g)-1) {
    mpt1=GroupAt(g,i+1);
    pos1=ProjectPointToPolyLine(d->gXY,mpt1->backupX,mpt1->backupY);
    if (pos1<pos) RevertGroup(g);
  }

  /* Allocate the pts array */

  d->ptsCount=GroupCount(g);
  d->ptsPtNo=GroupIndex(g,d->mpt);

  d->ptsA=d->pts=Malloc(sizeof(*d->pts)*d->ptsCount);

  for (i=0,mpt1=Group1st(g,&ix);mpt1!=NULL;mpt1=Next(&ix),i++) {
    d->pts[i].mpt=mpt1;
    d->pts[i].oldX=mpt1->x;
    d->pts[i].oldY=mpt1->y;
    d->pts[i].oldPos=
        ProjectPointToPolyLine(d->gXY,mpt1->x,mpt1->y);
    d->pts[i].bMoved=0;
  }

  FreeGroup(g);

  /* Calculate array and position limits */

  d->ptsMin=-1;
  for (i=d->ptsPtNo-1;i>=0;i--) {
    if (d->pts[i].mpt->cutFlag) {d->ptsMin=i;break;}
    if (d->pts[i].oldPos>d->pts[i+1].oldPos) {d->ptsMin=i+1;break;}
  }

  d->ptsMax=d->ptsCount;
  for (i=d->ptsPtNo+1;i<d->ptsCount;i++) {
    if (d->pts[i].mpt->cutFlag) {d->ptsMax=i;break;}
    if (d->pts[i].oldPos<d->pts[i-1].oldPos) {d->ptsMax=i-1;break;}
  }

  /* Detect double lines at ends */

/*  d->bDoubleLeft=d->bDoubleRight=0;
  if (d->ptsCount>=3) {
    if (d->ptsMin<0 && MeshPointDist(d->pts[0].mpt,d->pts[1].mpt)<
        w->app->meshBorderThreshold) d->bDoubleLeft=1;
    if (d->ptsMax>=d->ptsCount &&
        MeshPointDist(d->pts[d->ptsCount-1].mpt,d->pts[d->ptsCount-2].mpt)<
        w->app->meshBorderThreshold) d->bDoubleRight=1;
  } */


  assert(d->ptsCount>=3);

/*  if (d->bDoubleLeft && d->ptsPtNo==1)
    d->mpt=d->pts[--d->ptsPtNo].mpt;

  if (d->bDoubleRight && d->ptsPtNo==d->ptsCount-2)
    d->mpt=d->pts[++d->ptsPtNo].mpt;*/

  /* Calculate position limits */

  dpL=max(0,d->ptsPtNo-!!w->app->bDoubleMeshBorder)*
      w->app->meshSlidingThreshold;
  dpR=max(0,d->ptsCount-d->ptsPtNo-!!w->app->bDoubleMeshBorder-1)*
      w->app->meshSlidingThreshold;

  d->posMin=(d->ptsMin<0)? dpL :
      d->pts[d->ptsMin].oldPos+dpL;

  d->posMax=(d->ptsMax>=d->ptsCount)? CalcLineLength(d->gXY)-dpR :
      d->pts[d->ptsMax].oldPos-dpR;

  HighlightDragMeshPoint_1(w,d->mpt,1);

}


/* Also highlight upper/lower neighbors, if on double line */

static void HighlightDragMeshPoint_2(View w,MeshPoint mpt,int status) {
  MeshPoint mpt1;

  mpt1=NULL;
  if (mpt->mesh->app->bDoubleMeshBorder) {
    if (mpt->ny==0) mpt1=mpt->neighbors[MCN_BOTTOM];
    else if (mpt->ny==mpt->mesh->sizeY) mpt1=mpt->neighbors[MCN_TOP];
  }

  HighlightDragMeshPoint(w,mpt,status);
  if (mpt1!=NULL) HighlightDragMeshPoint(w,mpt1,status);
}

/* Also highlight left/right neighbors, if on double line */

static void HighlightDragMeshPoint_1(View w,MeshPoint mpt,int status) {
  MeshPoint mpt1;

  mpt1=NULL;
  if (mpt->mesh->app->bDoubleMeshBorder) {
    if (mpt->nx==0) mpt1=mpt->neighbors[MCN_RIGHT];
    else if (mpt->nx==mpt->mesh->sizeX) mpt1=mpt->neighbors[MCN_LEFT];
  }

  HighlightDragMeshPoint_2(w,mpt,status);
  if (mpt1!=NULL) HighlightDragMeshPoint_2(w,mpt1,status);
}

/* Also move upper/lower neighbors, if on double line */

static void MoveMeshPoint_2(MeshPoint mpt,double x,double y) {
  MeshPoint mpt1,mpt2;
  double h = 0,l;

  mpt1=mpt2=NULL;
  if (mpt->mesh->app->bDoubleMeshBorder) {
    if (mpt->ny==0) {
      mpt1=mpt->neighbors[MCN_BOTTOM];
      if (mpt1!=NULL) mpt2=mpt1->neighbors[MCN_BOTTOM];
    } else if (mpt->ny==mpt->mesh->sizeY) {
      mpt1=mpt->neighbors[MCN_TOP];
      if (mpt1!=NULL) mpt2=mpt1->neighbors[MCN_TOP];
    }
  }

  if (mpt1!=NULL) h=MeshPointDist(mpt,mpt1);

  ChangeMeshPoint(mpt,x,y);

  if (mpt1!=NULL && mpt2!=NULL) {
    l=MeshPointDist(mpt,mpt2);
    ChangeMeshPoint(mpt1,
        mpt->x+(mpt2->x-mpt->x)/l*h,mpt->y+(mpt2->y-mpt->y)/l*h);
  }
}

/* Also move left/right neighbors, if on double line */

static void MoveMeshPoint_1(MeshPoint mpt,double x,double y) {
  MeshPoint mpt1,mpt2;
  double h = 0,l;

  mpt1=mpt2=NULL;
  if (mpt->mesh->app->bDoubleMeshBorder) {
    if (mpt->nx==0) {
      mpt1=mpt->neighbors[MCN_RIGHT];
      if (mpt1!=NULL) mpt2=mpt1->neighbors[MCN_RIGHT];
    } else if (mpt->nx==mpt->mesh->sizeX) {
      mpt1=mpt->neighbors[MCN_LEFT];
      if (mpt1!=NULL) mpt2=mpt1->neighbors[MCN_LEFT];
    }
  }

  if (mpt1!=NULL) h=MeshPointDist(mpt,mpt1);

  MoveMeshPoint_2(mpt,x,y);

  if (mpt1!=NULL && mpt2!=NULL) {
    l=MeshPointDist(mpt,mpt2);
    MoveMeshPoint_2(mpt1,
        mpt->x+(mpt2->x-mpt->x)/l*h,mpt->y+(mpt2->y-mpt->y)/l*h);
  }
}

#define IHDM(mpt) ((mpt)->mesh->app->bDoubleMeshBorder && \
    ((mpt)->nx==1 || (mpt)->nx==(mpt)->mesh->sizeX-1))

static void DoSlideMeshPoint(View w,MoveMeshPointData d,double x,double y) {
  MeshPoint mpt1,mpt2,mpt3;
  double pos,pos1,nx,ny,l,l1,h;
  int i,k;

  if (d->mpt==NULL || !d->hm) return;

  if (d->gXY==NULL) return;
  pos=ProjectPointToPolyLine(d->gXY,x,y);
  l=CalcLineLength(d->gXY);

  /* Treat different cases on closed surfaces? */

  if (d->posMin<d->posMax) {
    pos=max(d->posMin,pos);
    pos=min(d->posMax,pos);
  } else {
    if (pos>d->posMax && pos<d->posMin)
      pos=fabs(pos-d->posMax)<fabs(pos-d->posMin)?
          d->posMax : d->posMin;
  }

  /* Calculate the new position for the dragged point */

  if (d->si!=NULL) CalcParamSplineValue(d->si,pos,&nx,&ny);
  else GetPolyLinePoint(d->gXY,pos,&nx,&ny);

  if (nx==d->mpt->x && ny==d->mpt->y) return;

  MoveMeshPoint_1(d->mpt,nx,ny);
  d->bMoved=1;

  /* Push neighboring mesh points, if needed */

  /* To the left */

  for (i=d->ptsPtNo-1,pos1=pos;i>=0 && i>d->ptsMin;i--) {
    if (IHDM(d->pts[i].mpt)) continue;

    pos1-=w->app->meshSlidingThreshold;
    if (pos1<0) pos1+=l;
    if (d->pts[i].oldPos<=pos1) break;

    HighlightDragMeshPoint_1(w,d->pts[i].mpt,1);

    if (d->si!=NULL) CalcParamSplineValue(d->si,pos1,&nx,&ny);
    else GetPolyLinePoint(d->gXY,pos1,&nx,&ny);

    MoveMeshPoint_1(d->pts[i].mpt,nx,ny);

    d->pts[i].bMoved=1;
  }

  /* Unhighlight & restore the remainders */

  if (i==0 && w->app->bDoubleMeshBorder) i++;

  for (k=i+1;i>=0;i--) {
    if (d->pts[i].bMoved) {
      MoveMeshPoint_1(d->pts[i].mpt,d->pts[i].oldX,d->pts[i].oldY);
      HighlightDragMeshPoint_1(w,d->pts[i].mpt,0);
      d->pts[i].bMoved=0;
    }
  }

  HighlightDragMeshPoint_1(w,d->pts[k].mpt,1);

  /* To the right */

  for (pos1=pos,i=d->ptsPtNo+1;i<d->ptsCount && i<d->ptsMax;i++) {
    if (IHDM(d->pts[i].mpt)) continue;

    pos1+=w->app->meshSlidingThreshold;
    if (pos1>l) pos1-=l;
    if (d->pts[i].oldPos>=pos1) break;

    HighlightDragMeshPoint_1(w,d->pts[i].mpt,1);

    if (d->si!=NULL) CalcParamSplineValue(d->si,pos1,&nx,&ny);
    else GetPolyLinePoint(d->gXY,pos1,&nx,&ny);

    MoveMeshPoint_1(d->pts[i].mpt,nx,ny);
    d->pts[i].bMoved=1;
  }

  /* Unhighlight & restore the remainders */

  if (i==d->mpt->mesh->sizeX && w->app->bDoubleMeshBorder) i--;

  for (k=i-1;i<d->ptsCount;i++) {
    if (d->pts[i].bMoved) {
      MoveMeshPoint_1(d->pts[i].mpt,d->pts[i].oldX,d->pts[i].oldY);
      HighlightDragMeshPoint_1(w,d->pts[i].mpt,0);
      d->pts[i].bMoved=0;
    }
  }

  HighlightDragMeshPoint_1(w,d->pts[k].mpt,1);

  /* Move the companion if a double point is dragged */
}


void TlMoveMeshPoint(View w,int event,double x,double y) {
  MoveMeshPointData d;
  int i;
  MeshPoint mpt1;

  d=w->toolData;

  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=event==TL_DBLCLK;
      d->bMoved=0;
      d->ptsA=d->pts=NULL;
      d->gXY=NULL;
      d->si=NULL;

      SetViewFlags(w,w->showFlags|SHW_MESH);

      d->mpt=HitViewObject(w,x,y,SHWX_MESHPOINTS);

      if (d->mpt==NULL) {
        d=Free(d);
        break;
      }

      d->hm=!IsLocked(d->mpt);
      SetHighlightMode(w->app,d->hm);

      if (d->mpt->cutFlag) {
        SetHighlightMode(w->app,d->hm=0);
        HighlightDragMeshPoint(w,d->mpt,1);
        ViewMsgEx(w,ERR_MOVE_CUT_MESH_PT,NULL);
        break;
      }

      if (d->bEnh) {
        HighlightDragMeshPoint(w,d->mpt,1);
      } else {
        SetupSlideMeshPoint(w,d);
      }

      if (d->mpt!=NULL) SetExamineMsg(w,d->mpt);

      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->mpt->x==x && d->mpt->y==y) break;
      if (d->bEnh) {
        ChangeMeshPoint(d->mpt,x,y);
        d->bMoved=1;
      } else DoSlideMeshPoint(w,d,x,y);
      SetExamineMsg(w,d->mpt);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);

      if (d->hm && d->bMoved) UndoMark(w->app); else Cancel(w->app);
      if (d->hm) ViewMsgEx(w,0,NULL);

      if (d->ptsA!=NULL) d->pts=d->ptsA=Free(d->ptsA);
      if (d->gXY!=NULL) d->gXY=FreeMallocedGroup(d->gXY);
      if (d->si!=NULL) d->si=FreeParamSplineInfo(d->si);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,d->hm && event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      if (d->ptsA!=NULL) d->pts=d->ptsA=Free(d->ptsA);
      if (d->gXY!=NULL) d->gXY=FreeMallocedGroup(d->gXY);
      if (d->si!=NULL) d->si=FreeParamSplineInfo(d->si);
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

/* Basic operations ////////////////////////////////////////////////// */

void* DelMesh(Mesh m) {
  struct _DelRec ar;
  MeshElement me;
  MeshCell mc;
  Var v;
  Index ix;
  int i;

  assert(m!=NULL);

  for (i=0;i<m->elementCount;i++) {
    me=m->elements+i;
    MarkObject(m->app,me,0);
    for (v=Group1st(me->varsContaining,&ix);v!=NULL;v=Next(&ix))
      ChangeVarEx(m->app,v,me,0);
  }

  for (i=0;i<m->cellCount;i++) {
    mc=m->cells+i;
    MarkObject(m->app,mc,0);
    for (v=Group1st(mc->varsContaining,&ix);v!=NULL;v=Next(&ix))
      ChangeVarEx(m->app,v,mc,0);
  }

  SetMeshFileName(m,NULL);
  SetMeshHeaderString(m,NULL);

  ar.delete=m;
  ActDelMesh(m->app,&ar);

  return NULL;
}

void SetMeshFileName(Mesh m,char* fName) {
  SetObjString(m->app,m,GetOffset(Mesh,fileName),fName,0);
}

void SetMeshHeaderString(Mesh m,char* hs) {
  SetObjString(m->app,m,GetOffset(Mesh,headerString),hs,0);
  SetMeshAlt(m,m->alt+1);
}


/* Works exactly like strtok but also returns empty strings between
   two adjacent delimiters */


static char* xstrtok(char* s1,char* s2) {
  static char* stored_s=NULL;
  int i;

  if (s1!=NULL) stored_s=s1;
  if (stored_s==NULL) return NULL;

  s1=stored_s;

  for (;*stored_s;stored_s++) {
    for (i=0;s2[i];i++) if (s2[i]==*stored_s) {
      *stored_s++=0;
      return s1;
    }
  }

  stored_s=NULL;
  return s1;
}

int CheckMeshHeaderString(Mesh m,char* hs) {
  char* s,*s1;
  int i,bIDString=0;

  if (*hs && hs[strlen(hs)-1]!='\n') return ERR_MESH_HDR_NO_LF;

  s1=MallocString(hs);

  s=xstrtok(s1,"\n");

  for (i=0;s!=NULL;i++) {
    if (!strcmp(s,"   Element output:") && i<10) bIDString=1;
    s=xstrtok(NULL,"\n");
  }

  Free(s1);

  if (!bIDString) return ERR_MESH_HDR_NO_ID;

  return 0;
}

void SetMeshAlt(Mesh m,int alt) {
  SetObjInt(m->app,m,GetOffset(Mesh,alt),sizeof(m->alt),alt,0);
}

void SetMeshFingerprint(Mesh m,long fingerprint) {
  SetObjInt(m->app,m,GetOffset(Mesh,fingerprint),sizeof(m->fingerprint),
      fingerprint,0);
}

int SetMeshCellCenter(MeshCell mc,double cX,double cY) {
  if (mc->centerX!=cX)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,centerX),cX,1);
  if (mc->centerY!=cY)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,centerY),cY,1);

  return 0;
}

int SetMeshCellFieldRatio(MeshCell mc,double ratio) {
  if (mc->ratio!=ratio)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,ratio),ratio,0);

  return 0;
}

static int CalcGradient(Equil eq,double x,double y,double* pGrad) {
  double t,gx,gy,grad;
  double r_dummy;
  Group gXY;
  SimpleSplineInfo si;
  int i;

  if (pGrad==NULL) pGrad=&r_dummy;
  *pGrad=0;

  if (eq==NULL) return ERR_NOEQUIL;

  if (x<=min(eq->x[0],eq->x[eq->sx-1]) || x>=max(eq->x[0],eq->x[eq->sx-1]) ||
      y<=min(eq->y[0],eq->y[eq->sy-1]) || y>=max(eq->y[0],eq->y[eq->sy-1]))
    return ERR_MESH_OUT_OF_EQUIL;

  if (eq->hSplines==NULL || eq->hSplines==NULL)
    CalcEquilSplines(eq);

  gXY=CreateGroup();
  for (i=0;i<eq->sx;i++) {
    CalcSimpleSplineValue(eq->vSplines[i],y,&t);
    AddXY(gXY,eq->x[i],t);
  }
  si=CreateSimpleSplineInfo(gXY);
  CalcSimpleSplineDY(si,x,&gx);
  si=FreeSimpleSplineInfo(si);
  gXY=FreeMallocedGroup(gXY);

  gXY=CreateGroup();
  for (i=0;i<eq->sy;i++) {
    CalcSimpleSplineValue(eq->hSplines[i],x,&t);
    AddXY(gXY,eq->y[i],t);
  }
  si=CreateSimpleSplineInfo(gXY);
  CalcSimpleSplineDY(si,y,&gy);
  si=FreeSimpleSplineInfo(si);
  gXY=FreeMallocedGroup(gXY);

  *pGrad=hypot(gx,gy);

  return 0;
}

int RecalculateMeshCellCenter(MeshCell mc) {
  double x,y,or,ra,r;
  int k;

  if (mc->mesh->app->equil==NULL) return ERR_MESH_NO_EQUIL;

  /* In CARRE mode or when the diagonals are parallel, use the mass center */

  if (mc->mesh->app->outputMode==OUTPUTMODE_CARRE ||
      VIntersect(mc->points[0]->x,mc->points[0]->y,
      mc->points[3]->x,mc->points[3]->y,mc->points[1]->x,mc->points[1]->y,
      mc->points[2]->x,mc->points[2]->y,&ra,NULL)==-2) {
    for (x=y=0,k=0;k<4;k++) {
      x+=mc->points[k]->x;
      y+=mc->points[k]->y;
    }
    x/=4;
    y/=4;
  } else {

  /* Otherwise (SONNET), use the intersection of diagonals */

    x=mc->points[0]->x+(mc->points[3]->x-mc->points[0]->x)*ra;
    y=mc->points[0]->y+(mc->points[3]->y-mc->points[0]->y)*ra;
  }

  SetMeshCellCenter(mc,x,y);

  /* Calculate the gradient at the "old" cell center */

  k=CalcGradient(mc->mesh->app->equil,mc->backupCenterX,mc->backupCenterY,&ra);
  if (k) return k;

  /* Calculate the gradient at the "new" cell center */

  k=CalcGradient(mc->mesh->app->equil,mc->centerX,mc->centerY,&r);
  if (k) return k;

  /* Multiply the old field ratio by the ratio of gradients */

  if (ra!=0) SetMeshCellFieldRatio(mc,r/ra*mc->backupRatio);

  return 0;
}


int RecalculateMeshCenters(Mesh m) {
  MeshCell mc;
  int i,k,nxMin,nyMin,nxMax,nyMax;

  for (i=0;i<m->cellCount;i++) {
    mc=m->cells+i;

    if (!MeshCellChanged(mc)) continue;

    k=RecalculateMeshCellCenter(mc);
    if (k) return k;
  }

  return 0;
}

int ChangeMeshPoint(MeshPoint mpt,double x,double y) {
  struct _ChangeMeshPointRec ar;
  int i,r;

  assert(mpt!=NULL);

  if (mpt->x==x && mpt->y==y) return 0;

  ar.mpt=mpt;
  ar.x=x;
  ar.y=y;

  r=ActChangeMeshPoint(mpt->mesh->app,&ar);

  if (!r) for (i=0;i<4;i++)
    if (mpt->cells[i]!=NULL) RecalculateMeshCellCenter(mpt->cells[i]);

  if (!r) SetMeshAlt(mpt->mesh,mpt->mesh->alt+1);

  return r;
}

int SetMeshPointBackupCoords(MeshPoint mpt,double x,double y) {
  assert(!mpt->locks);

  if (x!=mpt->backupX)
    SetObjDouble(mpt->mesh->app,mpt,GetOffset(MeshPoint,backupX),x,0);
  if (y!=mpt->backupY)
    SetObjDouble(mpt->mesh->app,mpt,GetOffset(MeshPoint,backupY),y,0);

  return 0;
}

static int SetMeshCellBackups(MeshCell mc,double cX,double cY,double ratio) {
  assert(!mc->locks);
  if (cX!=mc->backupCenterX)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,backupCenterX),
        cX,0);
  if (cY!=mc->backupCenterY)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,backupCenterY),
        cY,0);
  if (ratio!=mc->backupRatio)
    SetObjDouble(mc->mesh->app,mc,GetOffset(MeshCell,backupRatio),
        ratio,0);

  return 0;
}

/* Utilities ///////////////////////////////////////////////////////// */

static void HighlightDragMeshPoint(View w,MeshPoint mpt,int flag) {
  int i,j;

  Highlight(w,mpt,flag);
  for (i=0;i<4;i++) if (mpt->cells[i]!=NULL)
    Highlight(w,mpt->cells[i],flag);

   for (i=MCN_START;i<MCN_END;i++) if (mpt->elements[i]!=NULL)
    Highlight(w,mpt->elements[i],flag);
}

int MeshPointsConnected(MeshPoint mpt1,MeshPoint mpt2) {
  int i;
  MeshElement me;

  for (i=MCN_START;i<MCN_END;i++) if (mpt1->elements[i]!=NULL) {
    me=mpt1->elements[i];
    if (me->points[0]==mpt2 || me->points[1]==mpt2) return 1;
  }

  return 0;
}

static double MeshPointDist(MeshPoint mpt1,MeshPoint mpt2) {
  return hypot(mpt1->x-mpt2->x,mpt1->y-mpt2->y);
}

int MeshPointChanged(MeshPoint mpt) {
  return mpt->x!=mpt->backupX || mpt->y!=mpt->backupY;
}

int MeshCellChanged(MeshCell mc) {
  MeshPoint mpt;
  int i;

  for (i=0;i<4;i++) if (MeshPointChanged(mc->points[i])) return 1;

  return 0;
}

static Group GetMeshPointChain(MeshPoint mpt,int bStopOnCuts) {
  MeshPoint mpt1;
  Group g;

  g=CreateGroup();

  for (mpt1=mpt;(mpt1=mpt1->neighbors[MCN_LEFT])!=NULL;) {
    GroupAdd(g,mpt1);
    if (mpt1==mpt) return g;
    if (mpt1->cutFlag && bStopOnCuts) break;
  }

  RevertGroup(g);
  GroupAdd(g,mpt);

  for (mpt1=mpt;(mpt1=mpt1->neighbors[MCN_RIGHT])!=NULL;) {
    if (mpt1==mpt) break;
    GroupAdd(g,mpt1);
    if (mpt1->cutFlag && bStopOnCuts) break;
  }

  return g;
}

int PointInsideMeshCell(double xx,double yy,MeshCell mc,int bUseBackups) {
  double x,y;
  int i;

  if (bUseBackups) {
    for (x=y=i=0;i<4;i++) {
      x+=fabs(mc->points[i]->backupX)+1;
      y+=fabs(mc->points[i]->backupY)+1;
    }

    i=0;
    if (!VIntersect(xx,yy,x,y,
        mc->points[0]->backupX,mc->points[0]->backupY,
        mc->points[1]->backupX,mc->points[1]->backupY,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[1]->backupX,mc->points[1]->backupY,
        mc->points[3]->backupX,mc->points[3]->backupY,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[3]->backupX,mc->points[3]->backupY,
        mc->points[2]->backupX,mc->points[2]->backupY,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[2]->backupX,mc->points[2]->backupY,
        mc->points[0]->backupX,mc->points[0]->backupY,NULL,NULL)) i++;
  } else {
    for (x=y=i=0;i<4;i++) {
      x+=fabs(mc->points[i]->x)+1;
      y+=fabs(mc->points[i]->y)+1;
    }

    i=0;
    if (!VIntersect(xx,yy,x,y,
        mc->points[0]->x,mc->points[0]->y,
        mc->points[1]->x,mc->points[1]->y,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[1]->x,mc->points[1]->y,
        mc->points[3]->x,mc->points[3]->y,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[3]->x,mc->points[3]->y,
        mc->points[2]->x,mc->points[2]->y,NULL,NULL)) i++;
    if (!VIntersect(xx,yy,x,y,
        mc->points[2]->x,mc->points[2]->y,
        mc->points[0]->x,mc->points[0]->y,NULL,NULL)) i++;
  }

  return i%2==1;
}

int IsIrregularMeshCell(MeshCell mc) {
  double x,y;
  int i,j,k;

  if (!VIntersect(
      mc->points[0]->x,mc->points[0]->y,
      mc->points[1]->x,mc->points[1]->y,
      mc->points[2]->x,mc->points[2]->y,
      mc->points[3]->x,mc->points[3]->y,NULL,NULL))
    return STR_MESH_CELL_TWISTED;

  if (!VIntersect(
      mc->points[0]->x,mc->points[0]->y,
      mc->points[2]->x,mc->points[2]->y,
      mc->points[1]->x,mc->points[1]->y,
      mc->points[3]->x,mc->points[3]->y,NULL,NULL))
    return STR_MESH_CELL_TWISTED;

/*  for (x=y=i=0;i<4;i++) {
    x+=fabs(mc->points[i]->x)+1;
    y+=fabs(mc->points[i]->y)+1;
  }

  i=0;
  if (!VIntersect(mc->centerX,mc->centerY,x,y,
      mc->points[0]->x,mc->points[0]->y,
      mc->points[1]->x,mc->points[1]->y,NULL,NULL)) i++;
  if (!VIntersect(mc->centerX,mc->centerY,x,y,
      mc->points[1]->x,mc->points[1]->y,
      mc->points[3]->x,mc->points[3]->y,NULL,NULL)) i++;
  if (!VIntersect(mc->centerX,mc->centerY,x,y,
      mc->points[3]->x,mc->points[3]->y,
      mc->points[2]->x,mc->points[2]->y,NULL,NULL)) i++;
  if (!VIntersect(mc->centerX,mc->centerY,x,y,
      mc->points[2]->x,mc->points[2]->y,
      mc->points[0]->x,mc->points[0]->y,NULL,NULL)) i++; */

  if (!PointInsideMeshCell(mc->centerX,mc->centerY,mc,0))
    return STR_MESH_CELL_BAD_CENTER;

  for (k=0;k<4;k++) {
    /* find adjacent mesh points; must be cycled through in order (0,1,3,2) */
    i=k^(k^k>>1)%2+1;
    j=i^(i^i>>1)%2+1;
    if ((mc->points[i]->x-mc->points[k]->x)*(mc->points[j]->y-mc->points[i]->y)
      <(mc->points[j]->x-mc->points[i]->x)*(mc->points[i]->y-mc->points[k]->y))
      return STR_MESH_CELL_CONCAVE;
  }

  return 0;
}

MeshCell FindMeshCellByNumber(Mesh m,int eN) {
  int i;

  if (eN<m->cellCount && m->cells[eN].eN==eN) return m->cells+eN;
  for (i=0;i<m->cellCount;i++) if (m->cells[i].eN==eN) return m->cells+i;

  return NULL;
}

MeshElement FindMeshElementById(Mesh m,int id1,int id2) {
  MeshCell mc;

  mc=FindMeshCellByNumber(m,id1);
  if (mc==NULL) return NULL;

  return mc->elements[id2];
}

void GetMeshElementId(MeshElement me,int* pid1,int* pid2) {
  int i;

  assert(me->cells[0]!=NULL);
  *pid1=me->cells[0]->eN;
  for (i=MCN_START;i<MCN_END;i++) if (me->cells[0]->elements[i]==me) break;
  assert(i<MCN_END);

  *pid2=i;
}

/* FILE I/O ////////////////////////////////////////////////////////// */

int LoadMesh(App a,char* fName) {
  Mesh m=NULL;
  Group mcdG=NULL,cmtG=NULL;
  long fp;
  char* s,* hs;
  int i,l,r;
  struct _ActRec ar;
  Index ix;

  r=DetectFileType(fName);

  switch (r) {
    case FT_SONNET:
      cmtG=CreateGroup();
      r=LoadSonnetMeshFile(fName,&mcdG,cmtG,&fp);
      break;
    case FT_NOTFOUND:
      return ERR_FILENOTFOUND;
    default:
      return ERR_BADFILETYPE;
  }

  if (r==0) r=CreateMeshTopology(&m,mcdG);


  if (r==0) {
    if (a->mesh!=NULL) DelMesh(a->mesh);
    assert(a->mesh==NULL);

    m->app=a;
    m->fileFormat=r;

    ar.obj=m;
    r=ActAddMesh(a,&ar);
  }

  if (r==0 && cmtG!=NULL) {
    for (l=0,s=Group1st(cmtG,&ix);s!=NULL;s=Next(&ix))
      l+=strlen(s);

    hs=Malloc(l+1);
    for (l=0,s=Group1st(cmtG,&ix);s!=NULL;s=Next(&ix)) {
      strcpy(hs+l,s);
      l+=strlen(s);
    }

    SetMeshHeaderString(m,hs);
    SetMeshAlt(m,0);
    Free(hs);
  }



  if (mcdG!=NULL) mcdG=FreeMallocedGroup(mcdG);
  if (cmtG!=NULL) cmtG=FreeMallocedGroup(cmtG);
  if (r && m!=NULL) m=Free(m);

  if (r==0) {
    SetMeshFileName(m,fName);
    SetMeshFingerprint(m,fp);
  }

  return r;
}

static int LoadSonnetMeshFile(char* fName,Group* pG,Group cmtG,
    long* pFingerprint) {
  FILE* f;
  double x1,y1,x2,y2,x3,y3,x4,y4,ratio,cX,cY;
  char s[DG_FNAME_LEN*2];
  int eN,i,j,k,bBeg;
  Group mcdG=NULL;
  MeshCellData mcd;
  unsigned long fp=0,fpl=0,c_no=0,l_no=0;

  *pG=NULL;

  f=fopen(fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  *pG=mcdG=CreateGroup();

  fp=0;
  bBeg=1;

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"   Element    %d = (  %d,  %d): ( "SCANFLT","SCANFLT
        ")      ( "SCANFLT","SCANFLT")", &eN,&i,&j,&x1,&y1,&x2,&y2)!=7) {
      if (bBeg) GroupAdd(cmtG,MallocString(s));
      continue;
    }

    bBeg=0;

    if (eN<0 || i<0 || j<0) {
      fclose(f);
      *pG=mcdG=FreeMallocedGroup(mcdG);
      return ERR_BADFILE;
    }

    fpl=0;for (k=0;s[k];k++) if (isgraph(s[k]))
      fpl=fpl^(((unsigned)s[k] & 0xff)*(1+c_no++%255));

    assert(fgets(s,sizeof(s)-1,f)!=NULL);
    if (sscanf(s,"   Field ratio  =  "SCANFLT"             ( "SCANFLT","
        SCANFLT")",&ratio,&cX,&cY)!=3) {
      fclose(f);
      *pG=mcdG=FreeMallocedGroup(mcdG);
      return ERR_BADFILE;
    }

    for (k=0;s[k];k++) if (isgraph(s[k]))
      fpl=fpl^(((unsigned)s[k] & 0xff)*(1+c_no++%255));

    assert(fgets(s,sizeof(s)-1,f)!=NULL);
    if (sscanf(s,"                             ( "SCANFLT","SCANFLT
        ")      ( "SCANFLT","SCANFLT")",&x3,&y3,&x4,&y4)!=4) {
      fclose(f);
      *pG=mcdG=FreeMallocedGroup(mcdG);
      return ERR_BADFILE;
    }

    for (k=0;s[k];k++) if (isgraph(s[k]))
      fpl=fpl^(((unsigned)s[k] & 0xff)*(1+c_no++%255));
    fp=fp^fpl*(1+l_no++%32766);

    if (j<0 || i<0) {
      fclose(f);
      *pG=mcdG=FreeMallocedGroup(mcdG);
      return ERR_BADFILE;
    }

    mcd=Malloc(sizeof(*mcd));
    mcd->eN=eN;
    mcd->nx=i;
    mcd->ny=j;
    mcd->x1=x3*SONNET_FACTOR;
    mcd->y1=y3*SONNET_FACTOR;
    mcd->x2=x4*SONNET_FACTOR;
    mcd->y2=y4*SONNET_FACTOR;
    mcd->x3=x1*SONNET_FACTOR;
    mcd->y3=y1*SONNET_FACTOR;
    mcd->x4=x2*SONNET_FACTOR;
    mcd->y4=y2*SONNET_FACTOR;
    mcd->cX=cX*SONNET_FACTOR;
    mcd->cY=cY*SONNET_FACTOR;
    mcd->ratio=ratio;
    mcd->mc=NULL;
    GroupAdd(mcdG,mcd);
  }

  fclose(f);

  *pFingerprint=fp;

  return 0;
}

int SaveMesh(Mesh m,char* fileName) {
  FILE* f;
  MeshCell mc;
  MeshPoint mpt;
  int i,k;
  Index ix;
  static char s[2048];
  unsigned long fp=0,fpl,c_no=0,l_no=0;

  i=RecalculateMeshCenters(m);
  if (i) return i;

  assert(fileName!=NULL);

  f=fopen(fileName,"w");
  if (f==NULL) return ERR_FWRITE;

  SetMeshFileName(m,fileName);

  if (m->headerString==NULL) {
    fprintf(f,
        "\n"
        "   Element output:\n"
        "\n"
        "   ========================================================================================\n"
    );
  } else fputs(m->headerString,f);

  for (i=0;i<m->cellCount;i++) {
    mc=m->cells+i;
    sprintf(s,
      "   Element%5d = (%3d,%3d): (% 17.10E,% 17.10E)      (% 17.10E,% 17.10E)\n"
      "   Field ratio  = % 17.10E             (% 17.10E,% 17.10E)\n"
      "                             (% 17.10E,% 17.10E)      (% 17.10E,% 17.10E)\n"
      ,
      mc->eN,mc->nx,mc->ny,
          mc->points[2]->x/SONNET_FACTOR,mc->points[2]->y/SONNET_FACTOR,
          mc->points[3]->x/SONNET_FACTOR,mc->points[3]->y/SONNET_FACTOR,
      mc->ratio,mc->centerX/SONNET_FACTOR,mc->centerY/SONNET_FACTOR,
      mc->points[0]->x/SONNET_FACTOR,mc->points[0]->y/SONNET_FACTOR,
          mc->points[1]->x/SONNET_FACTOR,mc->points[1]->y/SONNET_FACTOR
      );
    fputs(s,f);
    fputs("   ----------------------------------------------------------------------------------------\n",
        f);

    fpl=0;for (k=0;s[k];k++) if (isgraph(s[k]))
      fpl=fpl^(((unsigned)s[k] & 0xff)*(1+c_no++%255));
    fp=fp^fpl*(1+l_no++%32766);
  }

  fclose(f);

  SetMeshFingerprint(m,fp);

  for (i=0;i<m->pointCount;i++) {
    mpt=m->points+i;
    SetMeshPointBackupCoords(mpt,mpt->x,mpt->y);
  }

  for (i=0;i<m->cellCount;i++) {
    mc=m->cells+i;
    SetMeshCellBackups(mc,mc->centerX,mc->centerY,mc->ratio);
  }

  SetMeshAlt(m,0);

  return 0;
}

/* Topology processing and recognition /////////////////////////////// */

static MeshCell AddMeshCell(Mesh m,int eN,int nx,int ny,
    MeshPoint pt1,MeshPoint pt2,MeshPoint pt3,MeshPoint pt4,
    double centerX,double centerY,double ratio) {
  MeshCell mc;
  int i;

  assert(!m->bComplete);

  assert(m->cellCount<m->sizeX*m->sizeY);
  mc=m->cells+m->cellCount++;

  mc->type=T_MESHCELL;
  mc->locks=0;
  mc->mesh=m;
  mc->breakFlags=0;
  mc->eN=eN;
  mc->nx=nx;
  mc->ny=ny;
  mc->points[0]=pt1;
  mc->points[1]=pt2;
  mc->points[2]=pt3;
  mc->points[3]=pt4;

  mc->backupCenterX=mc->centerX=centerX;
  mc->backupCenterY=mc->centerY=centerY;
  mc->backupRatio=mc->ratio=ratio;

  mc->varsContaining=NULL;

  for (i=MCN_START;i<MCN_END;i++) mc->elements[i]=NULL;
  for (i=MCN_START;i<MCN_END;i++) mc->neighbors[i]=NULL;
  for (i=MCN_START;i<MCN_END;i++) mc->cutFlags[i]=0;

  return mc;
}

static MeshElement AddMeshElement(Mesh m,MeshPoint pt1,MeshPoint pt2,int bV) {
  MeshElement me;

  assert(!m->bComplete);

  assert(m->elementCount<m->sizeX*m->sizeY*4);
  me=m->elements+m->elementCount++;

  me->type=T_MESHELEMENT;
  me->locks=0;
  me->mesh=m;
  me->points[0]=pt1;
  me->points[1]=pt2;
  me->cells[0]=me->cells[1]=0;
  me->cutFlag=0;
  me->bVertical=bV;
  me->varsContaining=NULL;

  return me;
}

static MeshPoint AddMeshPoint(Mesh m,double x,double y) {
  MeshPoint mpt;
  int i;

  assert(!m->bComplete);

  assert(m->pointCount<m->sizeX*m->sizeY*4);

  mpt=m->points+m->pointCount++;

  mpt->type=T_MESHPOINT;
  mpt->locks=0;
  mpt->mesh=m;
  mpt->x=mpt->backupX=x;
  mpt->y=mpt->backupY=y;

  for (i=0;i<4;i++) mpt->cells[i]=NULL;
  for (i=MCN_START;i<MCN_END;i++) {
    mpt->neighbors[i]=NULL;
    mpt->elements[i]=NULL;
  }

  mpt->cutFlag=0;
  mpt->nx=mpt->ny=-1;

  return mpt;
}

static int CreateMeshTopology(Mesh* pM,Group mcdG) {
  Mesh m;
  MeshCell mc;
  MeshPoint mpt,pt1,pt2,pt3,pt4;
  MeshElement me;
  MeshCellData* mcda;
  MeshCellData mcd,mcd1,mcd2;
  int i,j,k,eXm,eYm,eNm,mcRowSize;
  int* iarray;
  Index ix,ix1;

  m=*pM=NULL;

  /* Initial checks */

  eNm=0;eXm=eYm=0;

  /* Find out mesh dimensions */

  for (mcd=Group1st(mcdG,&ix);mcd!=NULL;mcd=Next(&ix)) {
    eXm=max(eXm,mcd->nx+1);
    eYm=max(eYm,mcd->ny+1);
    eNm=max(eNm,mcd->eN+1);
  }

  /* Verify mesh dimensions */

  if (!eXm || !eYm) {
    return ERR_BADFILE;
  }

  /* Make sure every mesh cell number is unique */

  iarray=Malloc(sizeof(*iarray)*eNm);
  for (i=0;i<eNm;i++) iarray[i]=0;
  for (mcd=Group1st(mcdG,&ix);mcd!=NULL;mcd=Next(&ix)) {
    iarray[mcd->eN]++;
    if (iarray[mcd->eN]>1) {iarray=Free(iarray);return ERR_DUP_MESH_ELNO;}
  }
  iarray=Free(iarray);


  /* Create a 2-D array of mesh cell infos */

  mcda=Malloc(sizeof(*mcda)*eXm*eYm);
  for (i=0;i<eXm*eYm;i++) mcda[i]=NULL;
  for (mcd=Group1st(mcdG,&ix);mcd!=NULL;mcd=Next(&ix))
    mcda[mcd->ny*eXm+mcd->nx]=mcd;

  /* Make sure every mesh cell in a rectangle exists */

  for (i=0;i<eYm;i++) for (j=0;j<eXm;j++) {
    mcd=mcda[i*eXm+j];
    if (mcd==NULL) {
      mcda=Free(mcda);
      mcdG=FreeMallocedGroup(mcdG);
      return ERR_BADFILE;
    }
  }

  /* Detect neighboring cells */

  for (i=0;i<eYm;i++) for (j=0;j<eXm;j++) {
    mcd=mcda[i*eXm+j];
    for (k=MCN_START;k<MCN_END;k++) {
      mcd->mcn[k]=NULL;
      mcd->mccf[k]=0;
    }

    /* Above */

    if (i>0) {
      mcd1=mcda[(i-1)*eXm+j];
      if (mcd->x1==mcd1->x3 && mcd->y1==mcd1->y3 &&
          mcd->x2==mcd1->x4 && mcd->y2==mcd1->y4) mcd->mcn[MCN_TOP]=mcd1;
      else
        for (k=0;k<eXm*eYm;k++) {
          mcd1=mcda[k];
          if (mcd1==mcd) continue;
          if (mcd->x1==mcd1->x3 && mcd->y1==mcd1->y3 &&
              mcd->x2==mcd1->x4 && mcd->y2==mcd1->y4) {
            mcd->mcn[MCN_TOP]=mcd1;
            mcd->mccf[MCN_TOP]=1;
          }
        }
    }

    /* Below */

    if (i<eYm-1) {
      mcd1=mcda[(i+1)*eXm+j];
      if (mcd->x3==mcd1->x1 && mcd->y3==mcd1->y1 &&
          mcd->x4==mcd1->x2 && mcd->y4==mcd1->y2) mcd->mcn[MCN_BOTTOM]=mcd1;
      else
        for (k=0;k<eXm*eYm;k++) {
          mcd1=mcda[k];
          if (mcd1==mcd) continue;
          if (mcd->x3==mcd1->x1 && mcd->y3==mcd1->y1 &&
              mcd->x4==mcd1->x2 && mcd->y4==mcd1->y2) {
            mcd->mcn[MCN_BOTTOM]=mcd1;
            mcd->mccf[MCN_BOTTOM]=1;
          }
        }
    }

    /* Left */

    if (j>0) {
      mcd1=mcda[i*eXm+j-1];
      if (mcd->x1==mcd1->x2 && mcd->y1==mcd1->y2 &&
          mcd->x3==mcd1->x4 && mcd->y3==mcd1->y4) mcd->mcn[MCN_LEFT]=mcd1;
      else
        for (k=0;k<eXm*eYm;k++) {
          mcd1=mcda[k];
          if (mcd1==mcd) continue;
          if (mcd->x1==mcd1->x2 && mcd->y1==mcd1->y2 &&
              mcd->x3==mcd1->x4 && mcd->y3==mcd1->y4) {
            mcd->mcn[MCN_LEFT]=mcd1;
            mcd->mccf[MCN_LEFT]=1;
          }
        }
    }

    /* Right */

    if (j<eXm-1) {
      mcd1=mcda[i*eXm+j+1];
      if (mcd->x2==mcd1->x1 && mcd->y2==mcd1->y1 &&
          mcd->x4==mcd1->x3 && mcd->y4==mcd1->y3) mcd->mcn[MCN_RIGHT]=mcd1;
      else
        for (k=0;k<eXm*eYm;k++) {
          mcd1=mcda[k];
          if (mcd1==mcd) continue;
          if (mcd->x2==mcd1->x1 && mcd->y2==mcd1->y1 &&
              mcd->x4==mcd1->x3 && mcd->y4==mcd1->y3) {
            mcd->mcn[MCN_RIGHT]=mcd1;
            mcd->mccf[MCN_RIGHT]=1;
          }
        }
    }
  }

  /* Allocate the mesh structure */

  k=eXm*eYm;

  *pM=m=Malloc(sizeof(*m)+sizeof(*m->cells)*k+sizeof(*m->elements)*k*4+
      sizeof(*m->points)*k*4+GetSafeAlignment()*3);
  m->type=T_MESH;
  m->locks=0;

  m->app=NULL;
  m->alt=0;
  m->fingerprint=0;
  m->bComplete=0;
  m->sizeX=eXm;
  m->sizeY=eYm;
  m->fileName=NULL;
  m->headerString=NULL;

  m->cells=(MeshCell)(m+1);
  m->cells=SafeAlignedPointer(m->cells,m);
  m->elements=(MeshElement)(m->cells+k);
  m->elements=SafeAlignedPointer(m->elements,m);
  m->points=(MeshPoint)(m->elements+k*4);
  m->points=SafeAlignedPointer(m->points,m);
  m->cellCount=m->pointCount=m->elementCount=0;

  /* Create mesh cells and points */

  for (i=0;i<eYm;i++) for (j=0;j<eXm;j++) {

    mcd=mcda[i*eXm+j];

    pt1=pt2=pt3=pt4=NULL;

    /* Use points from already created neighboring cells if possible */

    if (mcd->mcn[MCN_TOP]!=NULL && mcd->mcn[MCN_TOP]->mc!=NULL) {
      pt1=mcd->mcn[MCN_TOP]->mc->points[2];
      pt2=mcd->mcn[MCN_TOP]->mc->points[3];
    }

    if (mcd->mcn[MCN_BOTTOM]!=NULL && mcd->mcn[MCN_BOTTOM]->mc!=NULL) {
      pt3=mcd->mcn[MCN_BOTTOM]->mc->points[0];
      pt4=mcd->mcn[MCN_BOTTOM]->mc->points[1];
    }

    if (mcd->mcn[MCN_LEFT]!=NULL && mcd->mcn[MCN_LEFT]->mc!=NULL) {
      pt1=mcd->mcn[MCN_LEFT]->mc->points[1];
      pt3=mcd->mcn[MCN_LEFT]->mc->points[3];
    }

    if (mcd->mcn[MCN_RIGHT]!=NULL && mcd->mcn[MCN_RIGHT]->mc!=NULL) {
      pt2=mcd->mcn[MCN_RIGHT]->mc->points[0];
      pt4=mcd->mcn[MCN_RIGHT]->mc->points[2];
    }

    /* Create the remaining points */

    if (pt1==NULL) pt1=AddMeshPoint(m,mcd->x1,mcd->y1);
    if (pt2==NULL) pt2=AddMeshPoint(m,mcd->x2,mcd->y2);
    if (pt3==NULL) pt3=AddMeshPoint(m,mcd->x3,mcd->y3);
    if (pt4==NULL) pt4=AddMeshPoint(m,mcd->x4,mcd->y4);

    /* Create the mesh cell */

    mcd->mc=mc=AddMeshCell(m,mcd->eN,mcd->nx,mcd->ny,pt1,pt2,pt3,pt4,
        mcd->cX,mcd->cY,mcd->ratio);
    assert(mc!=NULL);
    pt1->cells[3]=pt2->cells[2]=pt3->cells[1]=pt4->cells[0]=mc;

  }
  pt1=pt2=pt3=pt4=NULL;

  /* Set cut flags & neighbors, create mesh elements */

  for (i=0;i<eYm;i++) for (j=0;j<eXm;j++) {

    mcd=mcda[i*eXm+j];
    mc=mcd->mc;

    /* Set cut flags and neighbors */

    for (k=MCN_START;k<MCN_END;k++) {
      mc->cutFlags[k]=mcd->mccf[k];
      if (mcd->mcn[k]!=NULL) mc->neighbors[k]=mcd->mcn[k]->mc;
    }

    /* Create mesh elements */

    if (mc->neighbors[MCN_TOP]==NULL) {
      me=AddMeshElement(m,mc->points[0],mc->points[1],0);
      me->cutFlag=mc->cutFlags[MCN_TOP];
      me->points[0]->elements[MCN_RIGHT]=me->points[1]->elements[MCN_LEFT]=me;
      me->cells[0]=mc;
      mc->elements[MCN_TOP]=me;
    } else mc->elements[MCN_TOP]=mc->neighbors[MCN_TOP]->elements[MCN_BOTTOM];

    if (mc->neighbors[MCN_LEFT]==NULL) {
      me=AddMeshElement(m,mc->points[0],mc->points[2],1);
      me->cutFlag=mc->cutFlags[MCN_LEFT];
      me->points[0]->elements[MCN_BOTTOM]=me->points[1]->elements[MCN_TOP]=me;
      me->cells[0]=mc;
      mc->elements[MCN_LEFT]=me;
    } else mc->elements[MCN_LEFT]=mc->neighbors[MCN_LEFT]->elements[MCN_RIGHT];

    me=AddMeshElement(m,mc->points[1],mc->points[3],1);
    me->cutFlag=mc->cutFlags[MCN_RIGHT];
    me->points[0]->elements[MCN_BOTTOM]=me->points[1]->elements[MCN_TOP]=me;
    me->cells[0]=mc;
    if (mc->neighbors[MCN_RIGHT]!=NULL) {
      me->cells[1]=mc->neighbors[MCN_RIGHT];
      mc->neighbors[MCN_RIGHT]->elements[MCN_LEFT]=me;
    }
    mc->elements[MCN_RIGHT]=me;

    me=AddMeshElement(m,mc->points[2],mc->points[3],0);
    me->cutFlag=mc->cutFlags[MCN_BOTTOM];
    me->points[0]->elements[MCN_RIGHT]=me->points[1]->elements[MCN_LEFT]=me;
    me->cells[0]=mc;
    if (mc->neighbors[MCN_BOTTOM]!=NULL) {
      me->cells[1]=mc->neighbors[MCN_BOTTOM];
      mc->neighbors[MCN_BOTTOM]->elements[MCN_TOP]=me;
    }
    mc->elements[MCN_BOTTOM]=me;
  }

  /* Set cut flags for mesh points */

  for (i=0;i<m->pointCount;i++) {
    mpt=m->points+i;
    for (j=MCN_START;j<MCN_END;j++) {
      me=mpt->elements[j];
      if (me==NULL) continue;
      if (me->cutFlag) {
        mpt->cutFlag=1;
        break;
      }
    }
  }

  /* Set neighbors and numbers for mesh points */

  for (i=0;i<m->cellCount;i++) {
    mc=m->cells+i;
    mc->points[0]->nx=mc->nx;mc->points[0]->ny=mc->ny;
    mc->points[1]->nx=mc->nx+1;mc->points[1]->ny=mc->ny;
    mc->points[2]->nx=mc->nx;mc->points[2]->ny=mc->ny+1;
    mc->points[3]->nx=mc->nx+1;mc->points[3]->ny=mc->ny+1;

    if (!mc->points[0]->cutFlag) {
      mc->points[0]->neighbors[MCN_BOTTOM]=mc->points[2];
      mc->points[0]->neighbors[MCN_RIGHT] =mc->points[1];
    }
    if (!mc->points[1]->cutFlag) {
      mc->points[1]->neighbors[MCN_BOTTOM]=mc->points[3];
      mc->points[1]->neighbors[MCN_LEFT]  =mc->points[0];
    }
    if (!mc->points[2]->cutFlag) {
      mc->points[2]->neighbors[MCN_TOP]   =mc->points[0];
      mc->points[2]->neighbors[MCN_RIGHT] =mc->points[3];
    }
    if (!mc->points[3]->cutFlag) {
      mc->points[3]->neighbors[MCN_TOP]   =mc->points[1];
      mc->points[3]->neighbors[MCN_LEFT]  =mc->points[2];
    }
  }

  mcda=Free(mcda);
  m->bComplete=1;

  return 0;
}

/* Actions /////////////////////////////////////////////////////////// */

static int ActAddMesh(App a,ActRec ar) {
  DelRec ur;
  Mesh m;
  int i;

  if (AppLocked(a)) return 0;

  assert(a->mesh==NULL);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelMesh);
  ur->delete=ar->obj;

  m=ar->obj;
  ar->obj=NULL;

  assert(m->bComplete);

  for (i=0;i<m->cellCount;i++) m->cells[i].varsContaining=CreateGroup();
  for (i=0;i<m->elementCount;i++) m->elements[i].varsContaining=CreateGroup();

  NotifyAdd(a,m);
  DrawAppHighlight(a,DRAW_OFF);

  a->mesh=m;

  DrawAppObject(a,m,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,m);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

static int ActDelMesh(App a,DelRec ar) {
  ActRec ur;
  Mesh m;
  int i;

  if (AppLocked(a)) return 0;

  m=ar->delete;
  assert(m==a->mesh);
  assert (!m->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddMesh);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,m,DRAW_ERASE);

  a->mesh=NULL;

  for (i=0;i<m->cellCount;i++) {
    assert(IsEmptyGroup(m->cells[i].varsContaining));
    m->cells[i].varsContaining=FreeGroup(m->cells[i].varsContaining);
  }
  for (i=0;i<m->elementCount;i++) {
    assert(IsEmptyGroup(m->elements[i].varsContaining));
    m->elements[i].varsContaining=FreeGroup(m->elements[i].varsContaining);
  }

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

int ActChangeMeshPoint(App a,ChangeMeshPointRec ar) {
  ChangeMeshPointRec ur;
  Index ix;
  MeshElement me;
  MeshCell mc;
  int i,j;

  if (AppLocked(a)) return 0;

  assert(!ar->mpt->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeMeshPoint);
  ur->mpt=ar->mpt;
  ur->x=ar->mpt->x;
  ur->y=ar->mpt->y;

  NotifyChange(a,ar->mpt);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->mpt,DRAW_OFF);

/*  for (i=MCN_START;i<MCN_END;i++)
    if (ar->mpt->elements[i]!=NULL)
      DrawAppObject(a,ar->mpt->elements[i],DRAW_OFF);*/

  for (i=0;i<4;i++) if (ar->mpt->cells[i]!=NULL)
    for (j=MCN_START;j<MCN_END;j++) {
      me=ar->mpt->cells[i]->elements[j];
      if (me!=NULL) DrawAppObject(a,me,DRAW_OFF);
    }
  for (i=0;i<4;i++)
    if (ar->mpt->cells[i]!=NULL)
      DrawAppObject(a,ar->mpt->cells[i],DRAW_OFF);

  ar->mpt->x=ar->x;
  ar->mpt->y=ar->y;

  for (i=0;i<4;i++)
    if (ar->mpt->cells[i]!=NULL)
      DrawAppObject(a,ar->mpt->cells[i],DRAW_ON);

  for (i=0;i<4;i++) if (ar->mpt->cells[i]!=NULL)
    for (j=MCN_START;j<MCN_END;j++) {
      me=ar->mpt->cells[i]->elements[j];
      if (me!=NULL) DrawAppObject(a,me,DRAW_ON);
    }

  DrawAppObject(a,ar->mpt,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->mpt);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

/* Drawing routines ////////////////////////////////////////////////// */

void DrawMeshElement(View w,MeshElement me,int mode) {
  double x,y,l;
  int i,r,m;

  if (~w->showFlags & SHW_MESH) return;

  if (IsMarked(w->app,me) && !IsHighlighted(w->app,me)) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMMARK);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMMARK);
        break;
    }
    DrawViewLine(w,me->points[0]->x,me->points[0]->y,
      me->points[1]->x,me->points[1]->y);
  }


  /* Check for neighboring irregular cells */

  r=1;
  for (i=0;i<2;i++)
    if (me->cells[i]!=NULL && IsIrregularMeshCell(me->cells[i]))
      {r=0;break;}
  if ((~w->showFlags & SHW_IRREGCELLS)) r=1;

  /* If irregular, draw the "red line" */

  if (mode!=DRAW_ERASE && IsHighlighted(w->app,me)) {
    if (!r) {
      SetViewMode(w,VMX2_MESHELEMENT);
      DrawViewLine(w,me->points[0]->x,me->points[0]->y,
        me->points[1]->x,me->points[1]->y);
    }
    SetViewMode(w,r? VMX_MESHELEMENT : VMX12_MESHELEMENT);
  } else
  switch(mode) {
    case DRAW_ON:
      if (!r) {
        SetViewMode(w,VM2_MESHELEMENT);
        DrawViewLine(w,me->points[0]->x,me->points[0]->y,
          me->points[1]->x,me->points[1]->y);
      }
      SetViewMode(w,VM1_MESHELEMENT);
      break;
    case DRAW_OFF:
    case DRAW_ERASE:
      SetViewMode(w,VM0_MESHELEMENT);
      break;
  }
  DrawViewLine(w,me->points[0]->x,me->points[0]->y,
    me->points[1]->x,me->points[1]->y);
}

void DrawMeshCell(View w,MeshCell mc,int mode) {
  double x,y,l;

  if (~w->showFlags & SHW_MESH) return;

  if (IsMarked(w->app,mc) && !IsHighlighted(w->app,mc)) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMMARK);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMMARK);
        break;
    }
    DrawViewCircle(w,mc->centerX,mc->centerY,
      w->meshCellCenterLen*M_SQRT2/w->zoomX);
  }

  if (~w->showFlags & SHW_MESHDETAILS) return;

  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,mc) ? VMX_MESHCELLCENTER :
          VM1_MESHCELLCENTER);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,mc)) {SetViewMode(w,VMX_MESHCELLCENTER);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_MESHCELLCENTER);
      break;
  }

  DrawViewLine(w,mc->centerX-w->meshCellCenterLen/w->zoomX,
      mc->centerY-w->meshCellCenterLen/w->zoomY,
      mc->centerX+w->meshCellCenterLen/w->zoomX,
      mc->centerY+w->meshCellCenterLen/w->zoomY);
  DrawViewLine(w,mc->centerX+w->meshCellCenterLen/w->zoomX,
      mc->centerY-w->meshCellCenterLen/w->zoomY,
      mc->centerX-w->meshCellCenterLen/w->zoomX,
      mc->centerY+w->meshCellCenterLen/w->zoomY);

}

void DrawWholeMesh(View w,Mesh m,int mode) {
  int i;

  for (i=0;i<m->cellCount;i++) DrawObject(w,m->cells+i,mode);

  for (i=0;i<m->elementCount;i++)
    DrawObject(w,m->elements+i,mode);
}
