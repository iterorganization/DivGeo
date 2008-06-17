#include "dg.h"

static void ATP(Group g,int id,void* p) {
  PtrTable pt;

  pt=Malloc(sizeof(*pt));
  pt->id=id;
  pt->p=p;
  GroupAdd(g,pt);
}

static void* TP(Group g,int id) {
  Index ix;
  PtrTable pt;

  for (pt=Group1st(g,&ix);pt!=NULL;pt=Next(&ix))
    if (pt->id==id) return pt->p;

  return NULL;
}

#define xstrwordBUFFERS 10
static char* xStrWord(char* s,int n) {
  static char buf[xstrwordBUFFERS][1024];
  static int bufn=0;
  int i;

  while (isspace(*s)) s++;
  while (--n) {
    while (*s && !isspace(*s)) switch(*s++) {
      case '\\':
        if (*s) s++;break;
      case '\"':
        while (*s && *s!='\"') if (*s++=='\\') if (*s) s++;
        if (*s) s++;
        break;
      default:
        break;
    }
    while (isspace(*s)) s++;
  }
  if (*s!='\"') for (i=0;s[i] && !isspace(s[i]);i++) buf[bufn][i]=s[i]; else
    for (s++,i=0;s[i] && s[i]!='\"';) {
      if (s[i]=='\\') s++;
      if (s[i]) {buf[bufn][i]=s[i];i++;}
    }
  buf[bufn][i]=0;
  s=buf[bufn];
  bufn=(bufn+1)%xstrwordBUFFERS;

  return s;
}

int ReadOldDgFile(App a,char* fName,int* errFlags) {
  FILE* f;
  int errBuf;
  int i,i1,i2,i3,version;
  int iT1=0,iT2=0,iS=0;
  Group gT1,gT2,gS,g0;
  double f1,f2,/*f3,*/f00,f01,f10,f11;
  double xpX1=0,xpY1=0,xpX2=0,xpY2=0;
  Stack g;
  Node n,n1;
  Elem e;
  VarSetDef vsd;
  VarSet vs;
  VarDef vd;
  char s[DG_FNAME_LEN*2],s1[DG_FNAME_LEN]/*,s2[DG_FNAME_LEN]*/;
  char* ps1;

  if (errFlags==NULL) errFlags=&errBuf;
  *errFlags=0;

  f=fopen(fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  while (fgets(s,sizeof(s)-1,f)!=NULL)
    if (sscanf(s,"DgFile %d",&version)==1) goto ok;
  return ERR_BADFILETYPE;

  ok:
  if (version!=50 && version!=51 && version!=60 && version!=70 &&
      version!=71) {
    fclose(f);
    return ERR_BADFILEVERSION;
  }

  g=CreateStack();
  gT1=CreateGroup();
  gT2=CreateGroup();
  gS=CreateGroup();

  while (fgets(s,sizeof(s),f)!=NULL) {
    FreeUndoInfo(a);
    if (sscanf(s,"  Node %d "SCANFLT""SCANFLT,&i,&f1,&f2)==3 ||
        sscanf(s,"  AddNode %d "SCANFLT""SCANFLT,&i,&f1,&f2)==3) {
      n=AddNode(a,f1,f2);
      ATP(g,i,n);
    } else

    if (sscanf(s,"  Elem %d %d %d %d",&i,&i1,&i2,&i3)==4 ||
        sscanf(s,"  AddGraphElem %d %d %d %d",&i,&i1,&i2,&i3)==4) {
      n=TP(g,i1);
      n1=TP(g,i2);
      if (n==NULL || n1==NULL) goto err;
      if (!NodesConnected(n,n1)) {
        e=AddElem(a,n,n1);
        ChangeElemId(a,e,i3);
        ATP(g,i,e);
      }
    } else

    if (sscanf(s,"  AddTemplateElem %d",&i)==1) {
/*      e=TP(g,i);
      if (e==NULL) goto err;
      DelElem(a,e);
*/    } else

    if (sscanf(s,"  ElemGroup %d %s",&i,s1)==2) {
      if (!strcmp(s1,"Template")) {
        e=TP(g,i);
        if (e!=NULL) DelElem(a,e);
      }
    } else

    if (sscanf(s,"  Parameters %d",&i)==1 ||
        sscanf(s,"  AddParameters %d",&i)==1) {
      ps1=xStrWord(s,3);
      if (!strcmp(ps1,"target") || !strcmp(ps1,"Target")) {
        i1=1;
        sscanf(xStrWord(s,4),"%d",&i1);
        if (i1==1) iT1=i; else iT2=i;
      } else
      if (!strcmp(ps1,"structure") || !strcmp(ps1,"Structure")) {
        iS=i;
      } else {
        vsd=FindVarSetDef(a,ps1);
        if (vsd==NULL)
          vsd=AddVarSetDef(a,ps1,ps1,0,8000);
        vs=AddVarSet(a,vsd);
        ATP(g,i,vs);
      }
    } else

    if (sscanf(s,"  ParamVar %d",&i)==1 ||
        sscanf(s,"  SetParameter %d",&i)==1) {
      vs=TP(g,i);
      vd=FindVarDef(vs->def,xStrWord(s,3));
      if (vs==NULL) goto err;
      if (vs->def->maxVarSets==8000)
        ChangeVarSetDef(a,vs->def,vs->def->name,vs->def->descr,0,9999,NULL);
      if (vd==NULL)
        vd=AddVarDef(a,vs->def,xStrWord(s,3),xStrWord(s,3),
          VT_TEXT,0,0,GroupCount(vs->def->varDefs));
      SetVar(a,vs,vd,NULL,xStrWord(s,4));
    } else

    if (sscanf(s,"  ElemVar %d %d",&i,&i1)==2 ||
        sscanf(s,"  SetElemParameter %d %d",&i,&i1)==2) {
      vs=TP(g,i);
      e=TP(g,i1);
      vd=FindVarDef(vs->def,xStrWord(s,4));
      if (vs==NULL || e==NULL) goto err;
      if (vs->def->maxVarSets==8000)
        ChangeVarSetDef(a,vs->def,vs->def->name,vs->def->descr,1,1,NULL);
      if (vd==NULL)
        vd=AddVarDef(a,vs->def,xStrWord(s,4),xStrWord(s,4),
          VT_TEXT,VF_FORELEMS,0,GroupCount(vs->def->varDefs));
      SetVar(a,e,vd,vs,xStrWord(s,5));
    } else

    if (sscanf(s,"  ParamElem %d %d",&i,&i1)) {
      e=TP(g,i1);
      if (e==NULL) goto err;
      if (i==iT1) GroupAdd(gT1,e); else
      if (i==iT2) GroupAdd(gT2,e); else
      if (i==iS) GroupAdd(gS,e); else {
        vs=TP(g,i);
        if (vs==NULL) goto err;
        if (vs->def->maxVarSets==8000)
          ChangeVarSetDef(a,vs->def,vs->def->name,vs->def->descr,0,9999,NULL);
        vd=FindVarDef(vs->def,"elemlist");
        if (vd==NULL) vd=AddVarDef(a,vs->def,"elemlist",
          "Elements",VT_ELEMS,0,0,GroupCount(vs->def->varDefs));
        g0=GetVar(vs,vd,NULL);
        if (g0==NULL) g0=CreateGroup(); else
          g0=CopyGroup(g0,NULL);
        GroupAdd(g0,e);
        SetVar(a,vs,vd,NULL,g0);
        FreeGroup(g0);
      }
    } else

    if (sscanf(s,"  EquilibriumFile %s",s1)==1) {
      if (AddEquil(a,s1)) *errFlags |= DGFE_NOEQUIL;
    } else

    if (sscanf(s,"  MagneticSurface "SCANFLT"%d%d",&f1,&i,&i1) ||
        sscanf(s,"  AddMagneticSurface "SCANFLT"%d%d",&f1,&i,&i1)) {
      if (a->equil!=NULL && f1!=0) {
        f00=EqCorrCell(a->equil,i,i1,f1);
        f01=EqCorrCell(a->equil,i+1,i1,f1);
        f10=EqCorrCell(a->equil,i,i1+1,f1);
        f11=EqCorrCell(a->equil,i+1,i1+1,f1);
        if (inrange_s(f1,f00,f01)) AddSurfaceExXY(a,
          a->equil->x[i]+(a->equil->x[i+1]-a->equil->x[i])*
            (f1-f00)/(f01-f00),a->equil->y[i1],NULL); else
        if (inrange_s(f1,f10,f11)) AddSurfaceExXY(a,
          a->equil->x[i]+(a->equil->x[i+1]-a->equil->x[i])*
            (f1-f10)/(f11-f10),a->equil->y[i1+1],NULL); else
        if (inrange_s(f1,f00,f10)) AddSurfaceExXY(a,a->equil->x[i],
          a->equil->y[i1]+(a->equil->y[i1+1]-a->equil->y[i1])*
            (f1-f00)/(f10-f00),NULL);
      }
    } else

    if (sscanf(s,"  AddXpoint %d%d%d%d",&i,&i1,&i2,&i3)==4 && a->equil!=NULL) {
      xpX1=a->equil->x[i];
      xpY1=a->equil->y[i1];
      xpX2=a->equil->x[i2+1];
      xpY2=a->equil->y[i3+1];
    } else

    if (sscanf(s,"  AddSeparPoint%d"SCANFLT,&i,&f1)==2) {
      AddGridPointEx(a,i,f1);
      *errFlags|=DGFE_OLDTOPO;
    } else

    if (sscanf(s,"  SetMaxNumber %d",&i)==1) {
      SetMaxElemId(a,i-1);
    } else

    /*puts(s),*errFlags |= DGFE_SYNTAX*/;
  }
  vsd=AddVarSetDef(a,"st","Structure",0,1);
  vs=AddVarSet(a,vsd);
  ChangeVarSetDef(a,vsd,vsd->name,vsd->descr,1,1,NULL);
  vd=AddVarDef(a,vsd,"structure","Structure",VT_STRUCTURE,VF_NOEXPORT,0,0);
  if (SetVar(a,vs,vd,NULL,gS)) *errFlags |= DGFE_BADSTRUC;
  vd=AddVarDef(a,vsd,"target1","Target 1",VT_TARGET1,VF_NOEXPORT,0,1);
  if (SetVar(a,vs,vd,NULL,gT1)) *errFlags |= DGFE_BADTARG1;
  vd=AddVarDef(a,vsd,"target2","Target 2",VT_TARGET2,VF_NOEXPORT,0,2);
  if (SetVar(a,vs,vd,NULL,gT2)) *errFlags |= DGFE_BADTARG2;

  fclose(f);
  FreeMallocedGroup(g);
  FreeGroup(gS);
  FreeGroup(gT1);
  FreeGroup(gT2);

  if (*errFlags & DGFE_OLDTOPO) ConvertOldDgSurfaces(a,errFlags);

  return 0;
  err:
  fclose(f);
  FreeMallocedGroup(g);
  FreeGroup(gS);
  FreeGroup(gT1);
  FreeGroup(gT2);

  return ERR_BADFILE;
}

