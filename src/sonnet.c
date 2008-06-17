#include "dg.h"

#define SONNET_FACTOR 1e3

int AddSonnetData(App a,char* fName) {
  struct _ActRec ar;
  SonnetData sd;
  int i;

  ValidatePtr(a,"AddSonnetData");
  ValidatePtr(fName,"AddSonnetData_");

  if (a->sonnetData!=NULL) DelSonnetData(a);

  sd=Malloc(sizeof(*sd));
  sd->type=T_SONNET;
  sd->locks=0;
  sd->app=a;
  sd->sx=sd->sy=0;
  sd->points=NULL;

  ar.obj=sd;

  strncpy(sd->fName,fName,sizeof(sd->fName)-1);
  i=ActAddSonnetData(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

  if (a->fName!=NULL) {
    strncpy(sd->fName,a->fName,sizeof(sd->fName)-1);
    strcat(GetFilePath(sd->fName),GetShortFName(fName));
    i=ActAddSonnetData(a,&ar);
    if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
  }

  strncpy(sd->fName,GetShortFName(fName),sizeof(sd->fName)-1);
  i=ActAddSonnetData(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}

/*  strncpy(sd->fName,a->xapp->argv[0],sizeof(sd->fName)-1);
  strcat(GetFilePath(sd->fName),GetShortFName(fName));
  i=ActAddSonnetData(a,&ar);
  if (i!=ERR_FILENOTFOUND) {if (ar.obj!=NULL) ar.obj=Free(ar.obj);return i;}
*/
  if (ar.obj!=NULL) ar.obj=Free(ar.obj);

  return i;
}

int DelSonnetData(App a) {
  struct _DelRec ar;
  Separator sep;
  Index ix;

  ValidatePtr(a,"DelSonnetData");
  ValidatePtr(a->sonnetData,"DelSonnetData.data");
  if (IsLocked(a->sonnetData)) return ERR_LOCKED;

  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    DelSeparator(a,sep);

  ar.delete=a->sonnetData;
  ActDelSonnetData(a,&ar);

  return 0;
}

Separator AddSeparator(App a,double x,double y,Node n) {
  Separator sep;
  struct _ActRec ar;

  ValidatePtr(a,"AddSeparator");
  ValidatePtr(n,"AddSeparator___");

  sep=Malloc(sizeof(*sep));
  sep->type=T_SEPARATOR;
  sep->locks=0;
  sep->x=x;
  sep->y=y;
  sep->n=n;
  sep->id=GetNextElemId(a);

  ar.obj=sep;
  ActAddSeparator(a,&ar);

  return sep;
}

void* DelSeparator(App a,Separator sep) {
  struct _DelRec ar;
  Node n;
  Var v;
  Index ix;
  int i,id;

  ValidatePtr(a,"DelSeparator");
  ValidatePtr(sep,"DelSeparator_");
  if (IsHighlighted(a,sep))
    FatalError("DelSeparator()-highlight: fatal error 1");
  MarkObject(a,sep,0);

  for (v=SeparatorVar1st(sep,&ix);v!=NULL;v=Next(&ix))
    SetVar(a,sep,v->def,v->varSet,NULL);

  n=sep->n;
  id=sep->id;
  ar.delete=sep;
  i=ActDelSeparator(a,&ar);
  if (IsEmptyNode(n)) DelNode(a,n);

  return NULL;
}

void ChangeSeparator(App a,Separator sep,Node n) {
  struct _ChangeSeparatorRec ar;
  Node n1;

  ValidatePtr(a,"ChangeSeparator");
  ValidatePtr(sep,"ChangeSeparator_");
  ValidatePtr(n,"ChangeSeparator__");

  n1=sep->n;

  ar.sep=sep;
  ar.x=sep->x;
  ar.y=sep->y;
  ar.id=sep->id;
  ar.n=n;
  ActChangeSeparator(a,&ar);

  if (IsEmptyNode(n1)) DelNode(a,n1);
}

void ChangeSeparatorId(App a,Separator sep,int id) {
  struct _ChangeSeparatorRec ar;

  ValidatePtr(a,"ChangeSeparatorId");
  ValidatePtr(sep,"ChangeSeparatorId_");

  if (id>a->maxElemId) SetMaxElemId(a,id);

  ar.sep=sep;
  ar.x=sep->x;
  ar.y=sep->y;
  ar.n=sep->n;
  ar.id=id;
  ActChangeSeparator(a,&ar);
}

/* Mode==1: connect to points LEFT of the vector
   Mode==-1:connect to points RIGHT of the vector
*/

static int InstallSeparator(App a,Group g,double x1,double y1,
    double x2,double y2,int mode) {
  Node n,nHit=NULL;
  double d,dHit=0;
  Index ix;

  for (n=Group1st(g,&ix);n!=NULL;n=Next(&ix)) {
    if (mode*((y1-y2)*(n->x-x1)+(x2-x1)*(n->y-y1))>0) {
      d=hypot(x1-n->x,y1-n->y);
      if (nHit==NULL || d<dHit) {
        nHit=n;
        dHit=d;
      }
    }
  }
  if (nHit!=NULL) AddSeparator(a,x1,y1,nHit);
  return nHit==NULL ? ERR_NONODES : 0;
}

int InstallSeparators(App a) {
  SonnetData sd;
/*  Node n; */
  int i,j,pos;
  double x0,y0;
  Var v;
  Group g;
  Elem e;
  Index ix;

  ValidatePtr(a,"InstallSeparators");
  if ((sd=a->sonnetData)==NULL) return ERR_NOSONNET;
  v=GetVarPtrByType(a,VT_STRUCTURE);
  if (CheckValue(a,v->val,VT_STRUCTURE,NULL)) return ERR_BADSTRUCTURE;

  g=CreateGroup();
  for (e=Group1st(v->val,&ix);e!=NULL;e=Next(&ix)) {
    if (!InGroup(g,e->n[1])) GroupAdd(g,e->n[1]);
    if (!InGroup(g,e->n[2])) GroupAdd(g,e->n[2]);
  }

  RemoveAllSeparators(a);
  for (i=2;i+1<sd->sx;i++) {
    pos=i*4+(sd->sy-1)*sd->sx*4+2;
    j=InstallSeparator(a,g,sd->points[pos].x,sd->points[pos].y,
      sd->points[pos+1].x,sd->points[pos+1].y,1);
    if (j) {g=FreeGroup(g);return j;}
  }
  x0=sd->points[5].x;
  y0=sd->points[5].y;
  for (i=2;i+1<sd->sx;i++) {
    pos=i*4;
    if (sd->points[pos].x!=x0 || sd->points[pos].y!=y0) continue;
    x0=sd->points[pos+1].x;
    y0=sd->points[pos+1].y;
    j=InstallSeparator(a,g,sd->points[pos].x,sd->points[pos].y,
      sd->points[pos+1].x,sd->points[pos+1].y,-1);
    if (j) {g=FreeGroup(g);return j;}
  }
  g=FreeGroup(g);
  return 0;
}

void RemoveAllSeparators(App a) {
  Separator sep;
  Index ix;

  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    DelSeparator(a,sep);
}

CellsInfo CreateCellsInfo(App a,int* pErr,void** pObj) {
  int errBuf;
  void* objBuf;
  CellsInfo ci;
  Elem e,e0[4];
  Separator sep;
  Var strVar;
  Group str;
  SonnetData sd;
  XY pt;
  int i,j,nodeN,stopN;
  Index ix;

  if (pErr==NULL) pErr=&errBuf;*pErr=0;
  if (pObj==NULL) pObj=&objBuf;*pObj=0;

  if ((sd=a->sonnetData)==NULL) {*pErr=ERR_NOSONNET;return NULL;}
  if (AppSeparator1st(a,NULL)==NULL) {*pErr=ERR_NOSEPARATORS;return NULL;}

  strVar=GetVarPtrByType(a,VT_STRUCTURE);
  if (CheckValue(a,strVar->val,VT_STRUCTURE,NULL)) {
    *pErr=ERR_BADSTRUCTURE;
    return NULL;
  }
  str=strVar->val;

  for (i=0;i<4;i++) e0[i]=NULL;

  for (i=GroupCount(str),e=Group1st(str,&ix);i--;e=GetNextElem(e,str,2)) {
    pt=sd->points+4;
    if (!VIntersect(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
        2*pt[0].x-pt[1].x,2*pt[0].y-pt[1].y,
        2*pt[1].x-pt[0].x,2*pt[1].y-pt[0].y,NULL,NULL)) e0[3]=e;
    pt=sd->points+sd->sx*4-8;
    if (!VIntersect(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
        2*pt[0].x-pt[1].x,2*pt[0].y-pt[1].y,
        2*pt[1].x-pt[0].x,2*pt[1].y-pt[0].y,NULL,NULL)) e0[2]=e;
    pt=sd->points+sd->sx*(sd->sy-1)*4+6;
    if (!VIntersect(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
        2*pt[0].x-pt[1].x,2*pt[0].y-pt[1].y,
        2*pt[1].x-pt[0].x,2*pt[1].y-pt[0].y,NULL,NULL)) e0[0]=e;
    pt=sd->points+sd->sx*sd->sy*4-6;
    if (!VIntersect(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
        2*pt[0].x-pt[1].x,2*pt[0].y-pt[1].y,
        2*pt[1].x-pt[0].x,2*pt[1].y-pt[0].y,NULL,NULL)) e0[1]=e;
  }

  for (i=0;i<4;i++) if (e0[i]==NULL) {
    *pErr=ERR_SONNETSTRUCTINS;
    return NULL;
  }
  for (j=i=0,e=e0[0];i<=4;) {
    if (e==e0[i%4]) {i++;continue;}
    e=GetNextElem(e,str,1);
    j++;
  }
  if (j!=GroupCount(str)) {*pErr=ERR_SONNETSTRUCTINS;return NULL;}

  ci=Malloc(sizeof(*ci)*(GroupCount(a->separators)+3));
  for (i=0;i<GroupCount(a->separators)+2;i++) ci[i]=CreateGroup();
  ci[i]=NULL;

  e=e0[0];
  nodeN=1;
  stopN=1;
  sep=AppSeparator1st(a,&ix);
  i=0;
  j=0;

  while(ci[i]!=NULL) {
    if (!j++) GroupAdd(ci[i],e);
    if (e!=e0[stopN]) {
      if (sep!=NULL && InGroup(e->n[nodeN]->separators,sep)) {
        GroupAdd(ci[i],sep);
        GroupAdd(ci[i+1],sep);
        sep=Next(&ix);
        i++;
        continue;
      }
      if (GroupCount(e->n[nodeN]->separators)!=j-1) {
        ci=FreeCellsInfo(ci);
        *pErr=ERR_SEPARATORORDER;
        *pObj=e->n[nodeN];
        return NULL;
      }
      e=GetNextElem(e,str,nodeN);
      j=0;
    }
    else if (stopN==1) {
      e=e0[3];
      nodeN=2;
      stopN=2;
      i++;
      j=0;
    }
    else if (stopN==2) {i++;break;}
    else assert(0);
  }
  if (ci[i]!=NULL) {
    ci=FreeCellsInfo(ci);
    *pErr=ERR_INTERNALCELLS;
    return NULL;
  }
  return ci;
}

void* FreeCellsInfo(CellsInfo ci) {
  size_t i;

  for (i=0;ci[i]!=NULL;i++) ci[i]=FreeGroup(ci[i]);

  return Free(ci);
}

/* Private functions
*/

int LoadSonnetFile(SonnetData sd) {
  size_t allocSize,pos;
  FILE* f;
  double x1,y1,x2,y2/*,x3,y3,x4,y4*/;
  char s[DG_FNAME_LEN*2];
  int eN,i,j,eXm,eYm;

  f=fopen(sd->fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;
  allocSize=pos=0;sd->points=Malloc(1);
  eXm=eYm=0;

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"   Element    %d = (  %d,  %d): ( "SCANFLT","SCANFLT
        ")      ( "SCANFLT","SCANFLT")", &eN,&i,&j,&x1,&y1,&x2,&y2)!=7)
      continue;
    eXm=max(eXm,i);
    eYm=max(eYm,j);
    fgets(s,sizeof(s)-1,f);
    fgets(s,sizeof(s)-1,f);
    if (pos+4>=allocSize) {
      allocSize=pos+100;
      sd->points=Realloc(sd->points,allocSize*sizeof(*sd->points));
    }
    sd->points[pos].x=x1;sd->points[pos++].y=y1;
    sd->points[pos].x=x2;sd->points[pos++].y=y2;
    if (sscanf(s,"                             ( "SCANFLT
        ","SCANFLT")      ( "SCANFLT","SCANFLT,&x1,&y1,&x2,&y2)!=4) {
      sd->points=Free(sd->points);return ERR_BADFILE;
    }
    sd->points[pos].x=x1;sd->points[pos++].y=y1;
    sd->points[pos].x=x2;sd->points[pos++].y=y2;
  }
  if (!eXm || !eYm) {
    sd->points=Free(sd->points);return ERR_BADFILE;
  }
  for (i=0;i<pos;i++) {
    sd->points[i].x*=SONNET_FACTOR;sd->points[i].y*=SONNET_FACTOR;
  }
  sd->sx=eXm+1;
  sd->sy=eYm+1;
  sd->points=Realloc(sd->points,sd->sx*sd->sy*sizeof(*sd->points)*4);

  return 0;
}

