#include "dg.h"
#include "git_version_DG.h"

#define FILE_VERSION 115
#define DG_VERSION_NEEDED 15000

static struct _FlagsRec showFlags[]={
  {SHW_AXES,'X'},
  {SHW_GRID,'G'},
  {SHW_ELEMS,'e'},
  {SHW_NORMALS,'N'},
  {SHW_NUMBERS,'0'},
  {SHW_NODES,'n'},
  {SHW_IRRNODES,'r'},
  {SHW_TEMPLATE,'T'},
  {SHW_EQUIL,'E'},
  {SHW_SURFACES,'s'},
  {SHW_GRIDPOINTS,'p'},
  {SHW_MESH,'g'},
  {SHW_SEPARATORS,'S'},
  {SHW_SOURCES,'c'},
  {SHW_CHORDS,'h'},
  {SHW_MENU,'-'},
  {SHW_TOOLBAR,'='},
  {SHW_XPOINTTESTS,'x'},
  {SHW_MESHDETAILS,'m'},
  {SHW_STRETCH,'t'},
  {SHW_TOPVIEW,'v'},
  {SHW_3DCHORDS,'3'},
  {0,0}
};

static struct _FlagsRec varDefFlags[]={
  {VF_NOEXPORT,'n'},
  {VF_FORELEMS,'m'},
  {VF_FORSEPARATORS,'S'},
  {VF_FORSOURCES,'s'},
  {VF_FORCHORDS,'h'},
  {VF_LAYERINDEX,'i'},
  {0,0}
};

static struct _NameRec varDefTypes[]={
  {VT_INT,"Int"},
  {VT_FLOAT,"Float"},
  {VT_TEXT,"Text"},
  {VT_FILENAME,"Filename"},
  {VT_ELEM,"Elem"},
  {VT_ELEMS,"Elems"},
  {VT_STRUCTPART,"StructPart"},
  {VT_TARGET1,"Target1"},
  {VT_TARGET2,"Target2"},
  {VT_STRUCTURE,"Structure"},
  {VT_ELEMS,"Chords"},         /* To fix problems with 'dirty' files */
  {VT_CHORDS,"SetOfChords"},
  {VT_MESH_CELLS,"SetOfMeshCells"},
  {VT_MESH_ELEMENTS,"SetOfMeshElements"},
  {VT_MESH_H_ELEMENTS,"SetOfMeshElementsH"},
  {VT_MESH_V_ELEMENTS,"SetOfMeshElementsV"},
  {VT_TOPVIEW,"TopViewObjects"},
  {0,NULL}
};

static struct _NameRec outputModes[]={
  {OUTPUTMODE_SONNET,"Sonnet"},
  {OUTPUTMODE_CARRE,"Carre"},
  {0,NULL}
};

static struct _FlagsRec outputFlags[]={
  {OF_NC_VARS,'V'},
  {OF_NC_STRUCTURE,'S'},
  {OF_NC_TARGETS,'T'},
  {OF_NC_SURFACES,'s'},
  {OF_NC_GPOINTS,'g'},
  {OF_NC_CELLS,'c'},
  {0,0}
};

static struct _NameRec meshSlidingModes[]={
  {MSM_SURFACE,"Surfaces"},
  {MSM_SPLINE, "Spline"},
  {0,NULL}
};

static struct _FlagsRec surfaceZoneFlags[]={
  {SZF_LIMITBYSURFACE,'i'},
  {0,0}
};

static struct _FlagsRec gridPointSegFlags[]={
  {GPSF_USED,'u'},
  {GPSF_TARGET_CW,'t'},
  {0,0}
};

App LoadNormalApp(XApp xap,char* fName,char** pMsg,int* err) {
  App a;
  char* s,*s1;
  int r;

  ValidatePtr(xap,"LoadNormalApp");
  ValidatePtr(fName,"LoadNormalApp_");
  ValidatePtr(pMsg,"LoadNormalApp__");
  ValidatePtr(err,"LoadNormalApp___");

  s=s1=NULL;
  a=LoadApp(xap,fName,&s1,err);
  if (*err) return a;
  if (s1!=NULL) {
    s=ReallocString(s,s1);
    s1=Free(s1);
  }
  if (xap->config!=NULL) {
    r=UpdateLoadedApp(a,xap->config);
    if (r!=0)
      s=ReallocString(s,XAppStr(xap,r));
  }
  *pMsg=s;

  FreeUndoInfo(a);
  RecalcTopologyCache(a);

  return a;
}

char* GetLoadErrFlagsDescription(XApp xap,int errFlags) {
  char* s=NULL;

  if (errFlags & DGFE_OLDTOPO) {
    s=ReallocString(s,XAppStr(xap,WRN_OLDTOPO));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_OLD_SURFACES_LOST) {
    s=ReallocString(s,XAppStr(xap,WRN_OLD_SURFACES_LOST));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_NOEQUIL) {
    s=ReallocString(s,XAppStr(xap,WRN_NOEQUIL));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_NOTEMPL) {
    s=ReallocString(s,XAppStr(xap,WRN_NOTEMPL));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_BADTYPE) {
    s=ReallocString(s,XAppStr(xap,WRN_BADTYPE));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_SYNTAX) {
    s=ReallocString(s,XAppStr(xap,WRN_SYNTAX));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_NOSONNET) {
    s=ReallocString(s,XAppStr(xap,WRN_NOSONNET));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_BADTARG1) {
    s=ReallocString(s,XAppStr(xap,WRN_BADTARG1));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_BADTARG2) {
    s=ReallocString(s,XAppStr(xap,WRN_BADTARG2));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_BADSTRUC) {
    s=ReallocString(s,XAppStr(xap,WRN_BADSTRUC));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_BADXPT) {
    s=ReallocString(s,XAppStr(xap,WRN_BADXPT));s=ReallocString(s,"\n");
  }
  if (errFlags & DGFE_MESH_FP) {
    s=ReallocString(s,XAppStr(xap,WRN_MESH_FP));s=ReallocString(s,"\n");
  }

  return s;
}

static char* ReadText(FILE* f) {
  char s[DG_FNAME_LEN*2];
  char* text;
  int i,j;

  text=MallocString("");

  if (fgets(s,sizeof(s)-1,f)==NULL) return text;
  if (sscanf(s,"text101 %d",&j)!=1) return text;

  for (i=0;i<j;i++) {
    if (fgets(s,sizeof(s)-1,f)==NULL) break;
    text=Realloc(text,strlen(text)+strlen(s)+1);
    strcat(text,s);
  }
  return text;
}

void WriteText(char* s,FILE* f) {
  int j;
  char* s1;

  if (s==NULL) s="";
  for (j=0,s1=s;*s1;s1++) if (*s1=='\n') j++;
  if (*s && s[strlen(s)-1]!='\n') j++;

  fprintf(f,"text101 %d\n",j);
  fputs(s,f);
  if (*s && s[strlen(s)-1]!='\n') fputc('\n',f);
}

static char* RemoveLF(char* str) {
  size_t l;

  if (l=strlen(str),l && str[l-1]=='\n')
    str[l-1]=0;

  return str;
}

static void WriteVar(App a,FILE* f,void* obj,VarDef vd,VarSet vs){
  Group g;
  char* s;
  Elem e;
  Chord ch;
  MeshElement me;
  MeshCell mc;
  Index ix;
  int i1,i2;

  if (vd->varType & VTM_HASGROUP) {
    if (vd->varType & VTF_HASELEMS) {
      g=GetVar(obj,vd,vs);
      fprintf(f,"Group100 %u\n",(unsigned)GroupCount(g));
      for (e=Group1st(g,&ix);e!=NULL;e=Next(&ix))
        fprintf(f,"%u\n",(unsigned)GroupIndex(a->elems,e));
    } else if (vd->varType & VTF_HASCHORDS) {
      g=GetVar(obj,vd,vs);
      fprintf(f,"Group100 %u\n",(unsigned)GroupCount(g));
      for (ch=Group1st(g,&ix);ch!=NULL;ch=Next(&ix))
        fprintf(f,"%u\n",(unsigned)GroupIndex(a->chords,ch));
    } else if (vd->varType & VTF_HAS_MESH_OBJECTS) {
      g=GetVar(obj,vd,vs);
      fprintf(f,"Group100 %u\n",(unsigned)GroupCount(g));
      if (vd->varType==VT_MESH_CELLS) {
        for (mc=Group1st(g,&ix);mc!=NULL;mc=Next(&ix)) fprintf(f,"%d\n",mc->eN);
      } else {
        for (me=Group1st(g,&ix);me!=NULL;me=Next(&ix)) {
          GetMeshElementId(me,&i1,&i2);
          fprintf(f,"%d %d\n",i1,i2);
        }
      }
    } else assert(0);
  } else {
    s=GetVar(obj,vd,vs);
/*    if (s==NULL) fprintf(f,"\n"); else */
      fprintf(f,"%s\n",s);
  }
}

static void WriteVars(App a,FILE* f,void* p) {
  VarSetDef vsd = NULL;
  VarDef vd;
  Index ix,ixe;
  void* org;

  switch (GetObjType(p)) {
    case T_VARSETDEF:
      vsd=p;
      break;
    case T_VARSET:
      vsd=((VarSet)p)->def;
      break;
    default:
      FatalError("WriteVars()-type%d: fatal error 1",GetObjType(p));
  }

  fprintf(f,"Vars110 %u\n",(unsigned)GroupCount(vsd->varDefs));
  for (vd=Group1st(vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) {
    if (GetObjType(p)==T_VARSET && (vd->flags & VFM_MULTIPLE)) {
      if (vd->flags & VF_FORELEMS)
        for (org=AppElem1st(a,&ixe);org!=NULL;org=Next(&ixe))
          WriteVar(a,f,org,vd,p);
      if (vd->flags & VF_FORSEPARATORS)
        for (org=AppSeparator1st(a,&ixe);org!=NULL;org=Next(&ixe))
          WriteVar(a,f,org,vd,p);
      if (vd->flags & VF_FORSOURCES)
        for (org=AppSource1st(a,&ixe);org!=NULL;org=Next(&ixe))
          WriteVar(a,f,org,vd,p);
      if (vd->flags & VF_FORCHORDS)
        for (org=AppChord1st(a,&ixe);org!=NULL;org=Next(&ixe))
          WriteVar(a,f,org,vd,p);
    } else
      WriteVar(a,f,p,vd,NULL);
  }
}

static void WriteApp_File(App a,FILE* f) {
  int i,j,i1,i2,i3;
  double f1,f2 = 0;
  Elem e;
  Node n;
  Separator sep;
  Source src;
  Chord ch;
  VarSetDef vsd;
  VarDef vd;
  VarSet vs;
  MeshPoint mpt;
  MeshCell mc = NULL;
  MeshElement me;
  XPointTest xpt;
  GridPointSeg gps;
  SurfaceEx sx;
  GridPointEx gpx;
  SurfaceZone sz;
  void* p;
  Group g;
  Index ix,ix1;

  fprintf(f,"DgFile %d {\n; For compatibility\n}\n",FILE_VERSION);

  if (a->equil!=NULL) {
    fprintf(f,"Equil100 %s\n",a->equil->fName);
    fprintf(f,"EquilHints114 %e %e %e %e\n",
      EqX(a->equil,0),EqY(a->equil,0),
      EqX(a->equil,EqSX(a->equil)-1),EqY(a->equil,EqSY(a->equil)-1));
  }

  if (a->template!=NULL) fprintf(f,"Template110 %e %e %e %e %s\n",
    a->template->xIncr,a->template->yIncr,a->template->angle,
    a->template->scale,a->template->fName);

  if (a->mesh!=NULL) {
    fprintf(f,"Mesh112 %s\n",a->mesh->fileName);
    fprintf(f,"MeshFingerprint112 %lx\n",a->mesh->fingerprint);

    for (i=0;i<a->mesh->pointCount;i++) {
      mpt=a->mesh->points+i;
      if (MeshPointChanged(mpt)) {
        for (j=0;j<4;j++) if ((mc=mpt->cells[j])!=NULL) break;
        if (mc==NULL) continue;
        for (j=0;j<4;j++) if (mc->points[j]==mpt) break;
        assert(j<4);
        fprintf(f,"MeshPoint112 %d %d %e %e\n",mc->eN,j,mpt->x,mpt->y);
      }
    }

    if (a->mesh->alt && a->mesh->headerString!=NULL) {
      fprintf(f,"MeshFileHeader113\n");
      WriteText(a->mesh->headerString,f);
    }
  }

  fprintf(f,"Nodes100 %u\n",(unsigned)GroupCount(a->nodes));
  for (n=AppNode1st(a,&ix);n!=NULL;n=Next(&ix))
    fprintf(f,"%e %e\n",n->x,n->y);

  fprintf(f,"Elems100 %d\n",(unsigned)GroupCount(a->elems));
  for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix))
    fprintf(f,"%u %u %d\n",(unsigned)GroupIndex(a->nodes,e->n[1]),
        (unsigned)GroupIndex(a->nodes,e->n[2]),e->id);

  fprintf(f,"XPointsEx114 %d\n",(unsigned)GroupCount(a->xpointTests));
  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix)) {
    fprintf(f,"%d %d %d %d %d %e %e %e\n",
        xpt->cx1,xpt->cy1,xpt->cx2,xpt->cy2,xpt->id,xpt->level,
        xpt->centerX,xpt->centerY);
  }

  fprintf(f,"GridPointSegs114 %d\n",(unsigned)GroupCount(a->gridPointSegs));
  for (gps=AppGridPointSeg1st(a,&ix);gps!=NULL;gps=Next(&ix)) {
    g=CreateGroup();
    GroupAdd(g,gps->xps->xpt);
    for (xpt=AppXPointTest1st(a,&ix1);xpt!=NULL;xpt=Next(&ix1))
      if (InGroup(xpt->segs,gps->xps))
        if (!InGroup(g,xpt)) GroupAdd(g,xpt);
    i3=0;
    i2=-1;
    if (GroupCount(g)<1) {i1=-1;f1=XPS_ANGLE_BAD;}
    else {
      xpt=gps->xps->xpt;
      i1=xpt->id;
      f1=CalcXPointSegAngle(a,gps->xps,gps->xps->xpt);
      f2=CalcLineLength(gps->xps->line);
      if (GroupCount(g)>1) {
        for (xpt=Group1st(g,&ix);xpt!=NULL;xpt=Next(&ix))
          if (xpt!=gps->xps->xpt) i2=xpt->id;
      }
    }

    fprintf(f,"%d %d %e %e %d %d %d %d %d %e %e %s\n",
        gps->xps->xpt->id,gps->xps->number,
        gps->xps->startPos,gps->xps->endPos,
        gps->zone,gps->dir,gps->targetDir,i1,i2,f1,f2,
        Flags2Str(gps->flags,gridPointSegFlags));
    fprintf(f,"%s\n",GetGridPointSegShortName(gps));
    fprintf(f,"%s\n",GetGridPointSegLongName(gps));

    FreeGroup(g);
  }

  fprintf(f,"SurfaceZones114 %d\n",(unsigned)GroupCount(a->surfaceZones));
  for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix)) {
    fprintf(f,"%d %d %d %d %s %d\n",
        sz->zone,sz->gpZone1,sz->gpZone2,sz->orient,
        Flags2Str(sz->flags,surfaceZoneFlags),
        sz->innermost!=NULL ? sz->innermost->id : -1);
    fprintf(f,"%s\n",GetSurfaceZoneShortName(sz));
    fprintf(f,"%s\n",GetSurfaceZoneLongName(sz));
  }

  fprintf(f,"SurfacesEx114 %d\n",(unsigned)GroupCount(a->surfacesEx));
  for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix)) {
    fprintf(f,"%d %e %e %e %s\n",
        sx->zone,sx->level,sx->originX,sx->originY,
        GetSurfaceExCreatorId(sx));
  }

  fprintf(f,"GridPointsEx114 %d\n",(unsigned)GroupCount(a->gridPointsEx));
  for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix)) {
    fprintf(f,"%d %e %s\n",
        gpx->zone,gpx->value,GetGridPointExCreatorId(gpx));
  }

  fprintf(f,"Separators101 %u\n",(unsigned)GroupCount(a->separators));
  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    fprintf(f,"%e %e %u %d\n",sep->x,sep->y,
      (unsigned)GroupIndex(a->nodes,sep->n),sep->id);

  fprintf(f,"Sources104 %u\n",(unsigned)GroupCount(a->sources));
  for (src=AppSource1st(a,&ix);src!=NULL;src=Next(&ix))
    fprintf(f,"%e %e\n",src->x,src->y);

  fprintf(f,"Chords115 %u\n",(unsigned)GroupCount(a->chords));
  for (ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix))
    fprintf(f,"%e %e %e %e %e %e\n",ch->x1,ch->y1,ch->z1,ch->x2,ch->y2,ch->z2);

  for (i=0,p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->elems,p)) i++;
  fprintf(f,"MarkedElems100 %d\n",i);
  for (p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->elems,p))
      fprintf(f,"%u\n",(unsigned)GroupIndex(a->elems,p));

  for (i=0,p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->separators,p)) i++;
  fprintf(f,"MarkedSeparators104 %d\n",i);
  for (p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->separators,p))
      fprintf(f,"%u\n",(unsigned)GroupIndex(a->separators,p));

  for (i=0,p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->sources,p)) i++;
  fprintf(f,"MarkedSources104 %d\n",i);
  for (p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->sources,p))
      fprintf(f,"%u\n",(unsigned)GroupIndex(a->sources,p));

  for (i=0,p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->chords,p)) i++;
  fprintf(f,"MarkedChords110 %d\n",i);
  for (p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
    if (InGroup(a->chords,p))
      fprintf(f,"%u\n",(unsigned)GroupIndex(a->chords,p));

  if (a->mesh!=NULL) {

    g=CopyGroup(a->mark,NULL);
    RestrictGroupToType(g,T_MESHELEMENT);
    if (!IsEmptyGroup(g)) {
      fprintf(f,"MarkedMeshElements112 %u\n",(unsigned)GroupCount(g));
      for (me=Group1st(g,&ix);me!=NULL;me=Next(&ix)) {
        GetMeshElementId(me,&i,&j);
        fprintf(f,"%d %d\n",i,j);
      }
    }
    g=FreeGroup(g);

    g=CopyGroup(a->mark,NULL);
    RestrictGroupToType(g,T_MESHCELL);
    if (!IsEmptyGroup(g)) {
      fprintf(f,"MarkedMeshCells112 %u\n",(unsigned)GroupCount(g));
      for (mc=Group1st(g,&ix);mc!=NULL;mc=Next(&ix))
        fprintf(f,"%d\n",mc->eN);
    }
    g=FreeGroup(g);
  }
  fprintf(f,"VarSetDefs101 %u\n",(unsigned)GroupCount(a->varSetDefs));
  for (vsd=AppVarSetDef1st(a,&ix);vsd!=NULL;vsd=Next(&ix)) {
    fprintf(f,"%s\n%s\n",vsd->name,vsd->descr);
    WriteText(vsd->help,f);
    fprintf(f,"%d %d\n",vsd->minVarSets,vsd->maxVarSets);
    fprintf(f,"VarDefs102 %u\n",(unsigned)GroupCount(vsd->varDefs));

    for (vd=Group1st(vsd->varDefs,&ix1);vd!=NULL;vd=Next(&ix1)) {
      fprintf(f,"%s\n%s\n",vd->name,vd->descr);
      WriteText(vd->help,f);
      fprintf(f,"%s %s %d %d\n",
        Int2Name(vd->varType,varDefTypes),Flags2Str(vd->flags,varDefFlags),
        vd->col,vd->row);
      fprintf(f,"EnumText102\n");
      WriteText(vd->enumValues,f);
    }
    WriteVars(a,f,vsd);
  }

  fprintf(f,"VarSets100 %u\n",(unsigned)GroupCount(a->varSets));
  for (vs=AppVarSet1st(a,&ix);vs!=NULL;vs=Next(&ix)) {
    fprintf(f,"%u\n",(unsigned)GroupIndex(a->varSetDefs,vs->def));
    WriteVars(a,f,vs);
  }

  fprintf(f,"MaxElemId101 %d\n",a->maxElemId);
  fprintf(f,"CreaTime101\n%s\n",a->creationTime==NULL ? "---" :
    a->creationTime);
  fprintf(f,"TopoName114 %s\n",GetTopologyName(a));

  fprintf(f,"OutputMode109 %s\n",Int2Name(a->outputMode,outputModes));
  fprintf(f,"OutputFlags111 %s\n",Flags2Str(a->outputFlags,outputFlags));

  fprintf(f,"MeshSlidingOptions112 %s %e %d\n",
      Int2Name(a->meshSlidingMode,meshSlidingModes),
      a->meshSlidingThreshold,a->bDoubleMeshBorder);

  fprintf(f,"ViewAttr115 %e %e %e %e %e %s\n",a->minX,a->minY,a->maxX,a->maxY,
    a->xyAngle,Flags2Str(a->showFlags,showFlags));
}

static int ReadVar(App a,FILE* f,void* obj,VarDef vd,VarSet vs){
  char s[500];
  Group g;
  unsigned i,n;
  int i1,i2;
  MeshCell mc;
  MeshElement me;

  if (vd->varType & VTM_HASGROUP) {
    if (vd->varType & VTF_HASELEMS) {
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"Group100 %u",&n)==1) {
        g=CreateGroup();
        while (n--) {
          fgets(s,sizeof(s)-1,f);
          if (sscanf(s,"%u",&i)==1) GroupAdd(g,GroupAt(a->elems,i));
        }
        SetVar(a,obj,vd,vs,g);
        g=FreeGroup(g);
      } else return -1;
    } else if (vd->varType & VTF_HASCHORDS) {
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"Group100 %u",&n)==1) {
        g=CreateGroup();
        while (n--) {
          fgets(s,sizeof(s)-1,f);
          if (sscanf(s,"%u",&i)==1) GroupAdd(g,GroupAt(a->chords,i));
        }
        SetVar(a,obj,vd,vs,g);
        g=FreeGroup(g);
      } else return -1;
    } else if (vd->varType & VTF_HAS_MESH_OBJECTS) {
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"Group100 %u",&n)==1) {
        g=CreateGroup();
        while (n--) {
          fgets(s,sizeof(s)-1,f);
          if (vd->varType==VT_MESH_CELLS) {
            if (a->mesh!=NULL && sscanf(s,"%d",&i1)==1)
              if ((mc=FindMeshCellByNumber(a->mesh,i1))!=NULL)
                GroupAdd(g,mc);
          } else {
            if (a->mesh!=NULL && sscanf(s,"%d %d",&i1,&i2)==2)
              if ((me=FindMeshElementById(a->mesh,i1,i2))!=NULL)
                GroupAdd(g,me);
          }
        }
        SetVar(a,obj,vd,vs,g);
        g=FreeGroup(g);
      } else return -1;
    } else assert(0);
  } else {
    fgets(s,sizeof(s)-1,f);
    SetVar(a,obj,vd,vs,RemoveLF(s));
  }
  return 0;
}

/* IMPORTANT: call AFTER a Vars <nn> { header
*/
static int ReadVars(App a,FILE* f,void* p) {
  VarSetDef vsd = NULL;
  VarDef vd;
  Index ix,ixe;
  void* obj;
  int fErr=0;
  switch (GetObjType(p)) {
    case T_VARSETDEF:
      vsd=p;
      break;
    case T_VARSET:
      vsd=((VarSet)p)->def;
      break;
    default:
      FatalError("ReadVars()-type%d: fatal error 1",GetObjType(p));
  }

  for (vd=Group1st(vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) {
    if (GetObjType(p)==T_VARSET && (vd->flags & VFM_MULTIPLE)) {
      if (vd->flags & VF_FORELEMS)
        for (obj=AppElem1st(a,&ixe);obj!=NULL;obj=Next(&ixe))
          fErr|=ReadVar(a,f,obj,vd,p);
      if (vd->flags & VF_FORSEPARATORS)
        for (obj=AppSeparator1st(a,&ixe);obj!=NULL;obj=Next(&ixe))
          fErr|=ReadVar(a,f,obj,vd,p);
      if (vd->flags & VF_FORSOURCES)
        for (obj=AppSource1st(a,&ixe);obj!=NULL;obj=Next(&ixe))
          fErr|=ReadVar(a,f,obj,vd,p);
      if (vd->flags & VF_FORCHORDS)
        for (obj=AppChord1st(a,&ixe);obj!=NULL;obj=Next(&ixe))
          fErr|=ReadVar(a,f,obj,vd,p);
    }
    else
      fErr|=ReadVar(a,f,p,vd,NULL);
  }

  return fErr;
}

static int ReadApp_File(App a,FILE* f,int* ef) {
  int version=0,i,i1,i2,i3,i4,i5,r;
  unsigned k,n,n1,n2;
  unsigned long fl;
  long long1;
  char s[DG_FNAME_LEN*2],s1[DG_FNAME_LEN],s2[DG_FNAME_LEN],
    s3[DG_FNAME_LEN],s4[DG_FNAME_LEN],* ps;
  double f1,f2,f3,f4,f5,f6;
  VarSet vs;
  VarSetDef vsd;
  VarDef vd;
  MeshCell mc;
  MeshElement me;
  XPointTest xpt;
  XPointSeg xps;
  GridPointSeg gps;
  SurfaceZone sz;
  SurfaceEx sx;
  GridPointEx gpx;
  int bMeshFingerprintOk=0;
  Index ix;

  *ef=0;
  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"DgFile%d",&version)==1) break;
  }

  if (!version) return ERR_BADFILE;
  switch(version) {
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
      break;
    default:
      return ERR_BADFILEVERSION;
  }

  fgets(s,sizeof(s)-1,f);
  fgets(s,sizeof(s)-1,f);

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    RemoveLF(s);

    if (sscanf(s,"Equil100 %s",s1)==1) {
      if (AddEquil(a,s1))
      if (AddEquil(a,GetShortFName(s1))) *ef|=DGFE_NOEQUIL;
    } else

    if (sscanf(s,"EquilHints114 "SCANFLT""SCANFLT""SCANFLT""SCANFLT,
        &f1,&f2,&f3,&f4)==4) {
      /* Ignore */
    } else
    if (sscanf(s,"Template100"SCANFLT""SCANFLT"%d%s",&f1,&f2,&i1,s1)==4) {
      if (AddTemplate(a,s1,f1,f2,M_PI*i1/2,1))
      if (AddTemplate(a,GetShortFName(s1),f1,f2,M_PI*i1/2,1))
      *ef|=DGFE_NOTEMPL;
    } else

    if (sscanf(s,"Template110"SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
        &f1,&f2,&f3,&f4,s1)==5) {
      if (AddTemplate(a,s1,f1,f2,f3,f4))
      if (AddTemplate(a,GetShortFName(s1),f1,f2,f3,f4))
      *ef|=DGFE_NOTEMPL;
    } else

    if (sscanf(s,"SonnetData100 %s",s1)==1 ||
        sscanf(s,"Mesh112 %s",s1)==1 ) {
      if (LoadMesh(a,s1))
      if (a->fName==NULL || (strcpy(s2,GetFilePath(a->fName)),
          strcat(s2,GetShortFName(s1)),LoadMesh(a,s2)))
      if (LoadMesh(a,GetShortFName(s1)))
      *ef|=DGFE_NOSONNET;
    } else

    if (sscanf(s,"MeshFingerprint112 %lx",&long1)==1) {
      if (a->mesh!=NULL && long1==a->mesh->fingerprint)
        bMeshFingerprintOk=1;
    } else

    if (sscanf(s,"MeshPoint112 %d %d "SCANFLT""SCANFLT,&i1,&i2,&f1,&f2)==4) {
      if (bMeshFingerprintOk && a->mesh!=NULL) {
        mc=FindMeshCellByNumber(a->mesh,i1);
        if (mc!=NULL) ChangeMeshPoint(mc->points[i2],f1,f2);
      } else *ef|=DGFE_MESH_FP;
    } else

    if (sscanf(s,"Nodes100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT,&f1,&f2)==2) AddNode(a,f1,f2);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Elems100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u %u %d",&n1,&n2,&i1)==3) {
            ChangeElemId(a,
               AddElem(a,GroupAt(a->nodes,n1),GroupAt(a->nodes,n2)),i1);
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Surfaces100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT,&f1,&f2)==2 && a->equil!=NULL) {
         AddSurfaceExXY(a,f1,f2,NULL);
         *ef|=DGFE_OLDTOPO;
       }
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Surfaces108 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT"%s",&f1,&f2,s1)==3 && a->equil!=NULL) {
         i1=0;
         sx=AddSurfaceExXY(a,f1,f2,&i1);
         if (sx!=NULL) {
           ChangeSurfaceExCreatorId(sx,s1);
          *ef|=DGFE_OLDTOPO;
         }
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"GridPoints100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d"SCANFLT,&i1,&f1)==2)
         AddGridPointEx(a,i1,f1);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"GridPoints108 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d"SCANFLT"%s",&i1,&f1,s1)==3) {
         gpx=AddGridPointEx(a,i1,f1);
         if (gpx!=NULL) ChangeGridPointExCreatorId(gpx,s1);
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"XPointsEx114 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d%d%d%d%d"SCANFLT,&i1,&i2,&i3,&i4,&i5,&f1)==6) {
         xpt=AddXPointTest(a,i1,i2,i3,i4,f1,i5);
         if (xpt==NULL) *ef|=DGFE_NEQUIL;
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"GridPointSegs114 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       r=sscanf(s,"%d%d"SCANFLT""SCANFLT"%d%d%d",
           &i1,&i2,&f1,&f2,&i3,&i4,&i5);
       if (sscanf(s,"%s%s%s%s%s%s%s%s%s%s%s%s",
           s1,s1,s1,s1,s1,s1,s1,s1,s1,s1,s1,s3)!=12) strcpy(s3,"");
       if (r==7) {
         fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
         fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);

         xpt=FindXPointTest(a,i1);
         if (xpt==NULL) {*ef|=DGFE_NEQUIL;goto bad;}
         xps=AddXPointSeg(a,xpt,i2,f1,f2);
         if (xps==NULL) {*ef|=DGFE_NEQUIL;goto bad;}
         gps=FindGridPointSegBySegment(a,xps);
         assert(gps!=NULL);

         SetGridPointSegZone(gps,i3);
         SetGridPointSegDir(gps,i4);
         SetGridPointSegTargetDir(gps,i5);
         SetGridPointSegShortName(gps,s1);
         SetGridPointSegLongName(gps,s2);
         fl=0;Str2Flags(s3,&fl,gridPointSegFlags);
         SetGridPointSegFlags(gps,fl);

         bad:;
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"SurfaceZones114 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f); /* szNo2==-1 -> unlimited */
       r=sscanf(s,"%d%d%d%d%s%d",&i1,&i2,&i3,&i4,s3,&i5);
       if (r==4) {strcpy(s3,"");r++;}
       if (r==5) {if (sscanf(s3,"%d",&i5)) strcpy(s3,"");r++;}
       if (r==6) {
         fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
         fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
         sz=AddSurfaceZone(a,i1,i2,i3,i4,i5);
         if (sz==NULL) {*ef|=DGFE_NEQUIL;goto badSZ;}
         SetSurfaceZoneShortName(sz,s1);
         SetSurfaceZoneLongName(sz,s2);
         fl=0;Str2Flags(s3,&fl,surfaceZoneFlags);
         SetSurfaceZoneFlags(sz,fl);
         badSZ:;
       }
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"SurfacesEx114 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d"SCANFLT""SCANFLT""SCANFLT"%s",
           &i1,&f1,&f2,&f3,s1)==5 && a->equil!=NULL) {
         if (i1!=SZN_XY) sx=AddSurfaceEx(a,i1,f1,NULL);
         else sx=AddSurfaceExXY(a,f2,f3,NULL);
         if (sx==NULL) {*ef|=DGFE_NEQUIL;goto badSX;}
         ChangeSurfaceExCreatorId(sx,s1);

         badSX:;
       }
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"GridPointsEx114 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d"SCANFLT"%s",&i1,&f1,s1)==3) {
         gpx=AddGridPointEx(a,i1,f1);
         if (gpx==NULL) {*ef|=DGFE_NEQUIL;goto badGPX;}
         ChangeGridPointExCreatorId(gpx,s1);

         badGPX:;
       }
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Separators101 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT"%u%d",&f1,&f2,&i1,&i2)==4)
         ChangeSeparatorId(a,AddSeparator(a,f1,f2,GroupAt(a->nodes,i1)),i2);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Sources104 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT,&f1,&f2)==2)
         AddSource(a,f1,f2);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Chords115 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT,
           &f1,&f2,&f3,&f4,&f5,&f6)==6)
         AddChord3D(a,f1,f2,f3,f4,f5,f6);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"Chords106 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,SCANFLT""SCANFLT""SCANFLT""SCANFLT,&f1,&f2,&f3,&f4)==4)
         AddChord(a,f1,f2,f3,f4);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedElems100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u",&n1)==1) MarkObject(a,GroupAt(a->elems,n1),1);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedSeparators104 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u",&n1)==1) MarkObject(a,GroupAt(a->separators,n1),1);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedSources104 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u",&n1)==1) MarkObject(a,GroupAt(a->sources,n1),1);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedChords110 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u",&n1)==1) MarkObject(a,GroupAt(a->chords,n1),1);
       else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedMeshElements112 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d %d",&i1,&i2)==2) {
         if (bMeshFingerprintOk && a->mesh!=NULL) {
           me=FindMeshElementById(a->mesh,i1,i2);
           if (me!=NULL) MarkObject(a,me,1);
         } else *ef|=DGFE_MESH_FP;
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"MarkedMeshCells112 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%d",&i1)==1) {
         if (bMeshFingerprintOk && a->mesh!=NULL) {
           mc=FindMeshCellByNumber(a->mesh,i1);
           if (mc!=NULL) MarkObject(a,mc,1);
         } else *ef|=DGFE_MESH_FP;
       } else *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"VarSetDefs100 %u",&n)==1) for (k=0;k<n;k++) {
      fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
      fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"%d %d\n",&i1,&i2)!=2)
        {*ef|=DGFE_SYNTAX;continue;}
      vsd=AddVarSetDef(a,s1,s2,i1,i2);
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"VarDefs100 %u",&n1)!=1) {*ef|=DGFE_SYNTAX;continue;}
      while (n1--) {
        fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
        fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
        fgets(s,sizeof(s)-1,f);
        if (sscanf(s,"%s %s %d %d\n",s3,s4,&i1,&i2)!=4)
          {*ef|=DGFE_SYNTAX;continue;}
        fl=0;Str2Flags(s4,&fl,varDefFlags);
        vd=AddVarDef(a,vsd,s1,s2,Name2Int(s3,varDefTypes),(unsigned)fl,i1,i2);
      }
      fgets(s,sizeof(s)-1,f);
      if (ReadVars(a,f,vsd)) *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"VarSetDefs101 %u",&n)==1) for (k=0;k<n;k++) {
      fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
      fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
      ps=ReadText(f);
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"%d %d\n",&i1,&i2)!=2)
        {*ef|=DGFE_SYNTAX;ps=Free(ps);continue;}
      vsd=AddVarSetDef(a,s1,s2,i1,i2);
      ChangeHelpString(a,vsd,ps);
      ps=Free(ps);
      fgets(s,sizeof(s)-1,f);
      if (sscanf(s,"VarDefs101 %u",&n1)==1) {
        while (n1--) {
          fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
          fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
          ps=ReadText(f);
          fgets(s,sizeof(s)-1,f);
          if (sscanf(s,"%s %s %d %d\n",s3,s4,&i1,&i2)!=4)
            {*ef|=DGFE_SYNTAX;ps=Free(ps);continue;}
          fl=0;Str2Flags(s4,&fl,varDefFlags);
          vd=AddVarDef(a,vsd,s1,s2,Name2Int(s3,varDefTypes),(int)fl,
            i1,i2);
          ChangeHelpString(a,vd,ps);
          ps=Free(ps);
        }
      } else if (sscanf(s,"VarDefs102 %u",&n1)==1) {
        while (n1--) {
          fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
          fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
          ps=ReadText(f);
          fgets(s,sizeof(s)-1,f);
          if (sscanf(s,"%s %s %d %d\n",s3,s4,&i1,&i2)!=4)
            {*ef|=DGFE_SYNTAX;ps=Free(ps);continue;}
          fl=0;Str2Flags(s4,&fl,varDefFlags);
          vd=AddVarDef(a,vsd,s1,s2,Name2Int(s3,varDefTypes),(unsigned)fl,
            i1,i2);
          ChangeHelpString(a,vd,ps);
          ps=Free(ps);
          fgets(s,sizeof(s)-1,f);RemoveLF(s);
          if (!strcmp(s,"EnumText102")) {
            ps=ReadText(f);
            ChangeEnumString(a,vd,ps);
            ps=Free(ps);
          }
        }
      }  else
          {*ef|=DGFE_SYNTAX;continue;}
      fgets(s,sizeof(s)-1,f);
      if (ReadVars(a,f,vsd)) *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"VarSets100 %u",&n)==1) for (k=0;k<n;k++) {
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"%u",&n1)!=1) {*ef|=DGFE_SYNTAX;continue;}
       vs=AddVarSet(a,GroupAt(a->varSetDefs,n1));
       fgets(s,sizeof(s)-1,f);
       if (sscanf(s,"Vars100 %u",&n1)!=1 && sscanf(s,"Vars104 %u",&n1)!=1
           && sscanf(s,"Vars110 %u",&n1)!=1)
         {*ef|=DGFE_SYNTAX;continue;}
       if (ReadVars(a,f,vs)) *ef|=DGFE_SYNTAX;
    } else

    if (sscanf(s,"XPoint100"SCANFLT""SCANFLT""SCANFLT""SCANFLT,
        &f1,&f2,&f3,&f4)==4) {
/*      if (a->equil!=NULL) i1=AddXPoint(a,f1,f2,f3,f4);  -- obsolete */
      *ef |= DGFE_OLDTOPO;
    } else

    if (sscanf(s,"MaxElemId101%d",&i1)==1) {
      SetMaxElemId(a,i1);
    } else

    if (!strcmp(s,"CreaTime101")) {
      fgets(s,sizeof(s)-1,f);RemoveLF(s);
      SetAppCreationTime(a,s);
    } else

    if (sscanf(s,"OutputMode109 %s",s1)==1) {
      SetAppOutputMode(a,Name2Int(s1,outputModes));
    } else

    if (sscanf(s,"TopoName114 %s",s1)==1) {
      SetTopologyName(a,s1);
    } else

/* ViewAttr103 may contain invalid flags due to errors in some versions
   of DG
*/
    if (sscanf(s,"ViewAttr103"SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
        &f1,&f2,&f3,&f4,s1)==5); else

    if (sscanf(s,"ViewAttr105"SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
        &f1,&f2,&f3,&f4,s1)==5) {
      Str2Flags(s1,&a->showFlags,showFlags);
      a->minX=f1;
      a->minY=f2;
      a->maxX=f3;
      a->maxY=f4;
    } else

    if (sscanf(s,"ViewAttr115"SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT"%s",
        &f1,&f2,&f3,&f4,&f5,s1)==6) {
      Str2Flags(s1,&a->showFlags,showFlags);
      a->minX=f1;
      a->minY=f2;
      a->maxX=f3;
      a->maxY=f4;
      a->xyAngle=f5;
    } else

    if (sscanf(s,"OutputFlags111%s",s1)==1) {
      Str2Flags(s1,&a->outputFlags,outputFlags);
    } else

    if (sscanf(s,"MeshSlidingOptions112%s"SCANFLT"%d",s1,&f1,&i1)==3) {
      SetMeshSlidingMode(a,Name2Int(s1,meshSlidingModes));
      SetMeshSlidingThreshold(a,f1);
      SetDoubleMeshBorderFlag(a,i1);
    } else

    if (!strcmp(s,"MeshFileHeader113")) {
      ps=ReadText(f);
      if (a->mesh!=NULL) SetMeshHeaderString(a->mesh,ps);
      ps=Free(ps);
    } else

    *ef |= DGFE_SYNTAX;
  }

  if (*ef & DGFE_OLDTOPO) ConvertOldDgSurfaces(a,ef);

  return 0;
}

int SaveApp(App a,char* fName,int mode) {
  FILE* f;
  time_t timer;

  f=fopen(fName,"w");
  if (f==NULL) return ERR_FWRITE;

  switch(mode) {
    case DGFM_APP:
      fprintf(f,"DivGeo data file>>\n");
      break;
    case DGFM_CONFIG:
      fprintf(f,"DivGeo config file>>\n");
      break;
    case DGFM_TMP:
      fprintf(f,"DivGeo temporary file>>\n");
      break;
    default: FatalError("SaveApp()-mode%d: fatal error 3",mode);
  }

  fprintf(f,"\nThis file should not be modified manually.\n\n"
    "DG version:             %s\n"
    "File format version:    %d\n"
    "Creation time:          %s\n"
    "Last Modified time:     %s\n"
    "Original filename:      %s\n"
    "GIT version:            %s\n",
    GetVersionStr(DG_VERSION),
    FILE_VERSION,
    a->creationTime==NULL ? "---" : a->creationTime,
    asctime(localtime((time(&timer),&timer))),fName,
    GIT_VERSION_DG);

  if (a->equil!=NULL) fprintf(f,
    "Equilibrium file:       %s\n",a->equil->fName);
  if (a->template!=NULL) fprintf(f,
    "Template file:          %s\n",a->template->fName);
  if (a->mesh!=NULL && a->mesh->fileName!=NULL) fprintf(f,
    "Mesh:                   %s\n\n",a->mesh->fileName);

  if (!IsEmptyGroup(a->elems))
    fprintf(f,"Elements:               %u\n",(unsigned)GroupCount(a->elems));

  if (!IsEmptyGroup(a->surfaceZones)) {
    fprintf(f,"Radial cells:           %s\n",GetSurfacesStatsStr(a));
  }
  if (!IsEmptyGroup(a->gridPointSegs))
    fprintf(f,"Poloidal Cells:         %s\n",GetGridPointStatsStr(a));
  if (!IsEmptyGroup(a->separators))
    fprintf(f,"Separators:             %u\n",(unsigned)GroupCount(a->separators));
  if (!IsEmptyGroup(a->sources))
    fprintf(f,"Sources:                %u\n",(unsigned)GroupCount(a->sources));

  fprintf(f,
    "\n------------------------------------------------------------------------\n\n");


  WriteApp_File(a,f);

  fclose(f);
  a->alt=0;

  return 0;
}

App LoadApp(XApp xap,char* fName,char** pMsg,int* err) {
  FILE* f;
  App a=NULL;
  int errFlags;

  ValidatePtr(xap,"LoadApp");
  ValidatePtr(fName,"LoadApp_");
  ValidatePtr(pMsg,"LoadApp__");
  ValidatePtr(err,"LoadApp___");

  errFlags=0;
  *err=0;
  switch(DetectFileType(fName)) {
    case FT_NOTFOUND:
      *err=ERR_FILENOTFOUND;
      break;
    case FT_DG_APP:
    case FT_DG_CONFIG:
    case FT_DG_TMP:
      f=fopen(fName,"r");
      if (f==NULL) {*err=ERR_FILENOTFOUND;return NULL;}
      a=CreateApp();
      SetAppFName(a,fName);
      EnableUndo(a,0);
      *err=ReadApp_File(a,f,&errFlags);
      EnableUndo(a,1);
      fclose(f);
      if (*err) a=FreeApp(a);
      break;
    default:
      a=CreateApp();
      *err=ReadOldDgFile(a,fName,&errFlags);
      if (*err) a=FreeApp(a);
      else SetAppFName(a,fName);
      break;
  }
  if (a!=NULL) {
    FreeUndoInfo(a);
    a->alt=0;
  }
  if (pMsg!=NULL) *pMsg=GetLoadErrFlagsDescription(xap,errFlags);
  return a;
}

/* Topology management ////////////////////////////////////////////// */

int LoadTopology(App a,char* fName,int bDetectXPoints) {
  int version=0,i,i1,i2,i3,i4,i5,i6,i7,i8,r;
  unsigned k,n,n1,n2,zoneMin,numberMin;
  unsigned long fl;
  long long1;
  char s[DG_FNAME_LEN*2],s1[DG_FNAME_LEN],s2[DG_FNAME_LEN],
    s3[DG_FNAME_LEN],s4[DG_FNAME_LEN],* ps;
  double f1,f2,f3,f4,d,dMin,angle;
  XPointTest xpt,xptMin;
  XPointSeg xps,xpsMin;
  GridPointSeg gps;
  SurfaceZone sz;
  SurfaceEx sx;
  GridPointEx gpx;
  int bMeshFingerprintOk=0;
  Index ix;
  int efPos;
  int* ef=&efPos;
  int bEqHints=0,bXpts=0,bGpss=0,bSZs=0;
  double eqx1=0,eqy1=0,eqx2=0,eqy2=0;
  double eqX1,eqY1,eqX2,eqY2;
  Group g;

  FILE* f;

  if (a->equil==NULL) return ERR_NOEQUIL;
  eqX1=EqX(a->equil,0);
  eqY1=EqY(a->equil,0);
  eqX2=EqX(a->equil,EqSX(a->equil)-1);
  eqY2=EqY(a->equil,EqSY(a->equil)-1);

  f=fopen(fName,"r");
  if (f==NULL) return ERR_FILENOTFOUND;

  *ef=0;
  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    if (sscanf(s,"DgFile%d",&version)==1) break;
  }

  if (!version) return ERR_BADFILE;
  switch(version) {
    case 114:
    case 115:
      break;
    default:
      return ERR_OLD_FILE_NO_TOPOLOGY;
  }

  fgets(s,sizeof(s)-1,f);
  fgets(s,sizeof(s)-1,f);

  while (fgets(s,sizeof(s)-1,f)!=NULL) {
    RemoveLF(s);

    if (sscanf(s,"EquilHints114 "SCANFLT""SCANFLT""SCANFLT""SCANFLT,
        &f1,&f2,&f3,&f4)==4) {
      eqx1=f1;
      eqy1=f2;
      eqx2=f3;
      eqy2=f4;
      bEqHints=0;
    } else

    if (sscanf(s,"XPointsEx114 %u",&n)==1) {
      if (bDetectXPoints) DetectXPoints(a);
      g=CreateGroup();
      for (k=0;k<n;k++) {
        fgets(s,sizeof(s)-1,f);
        if (sscanf(s,"%d%d%d%d%d"SCANFLT""SCANFLT""SCANFLT,
            &i1,&i2,&i3,&i4,&i5,&f1,&f2,&f3)==8) {
          f2=eqX1+(f2-eqx1)/(eqx2-eqx1)*(eqX2-eqX1);
          f3=eqY1+(f3-eqy1)/(eqy2-eqy1)*(eqY2-eqY1);

          dMin=MAXDOUBLE;xptMin=NULL;
          for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix)) {
            d=hypot(xpt->centerX-f2,xpt->centerY-f3);
            if (d<dMin) {dMin=d;xptMin=xpt;}
          }

          if (xptMin!=NULL) {
            SetXPointTestId(a,xptMin,i5);
            GroupAdd(g,xptMin);
          }
        }
      }

      for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix)) {
        if (!InGroup(g,xpt)) DelXPointTest(a,xpt);
      }
      g=FreeGroup(g);
    } else
    if (sscanf(s,"GridPointSegs114 %u",&n)==1) {
      DetectXPointSegs(a);
      g=CreateGroup();

      for (k=0;k<n;k++) {
        fgets(s,sizeof(s)-1,f);
        if (sscanf(s,"%s%s%s%s%s%s%s%s%s%s%s%s",
           s1,s1,s1,s1,s1,s1,s1,s1,s1,s1,s1,s3)!=12) strcpy(s3,"");
        if (sscanf(s,"%d%d"SCANFLT""SCANFLT"%d%d%d%d%d"SCANFLT""SCANFLT,
            &i1,&i2,&f1,&f2,&i3,&i4,&i5,&i6,&i7,&f3,&f4)==11) {
          if (f4<=0) f4=1; /* Bugfix for DG files made with beta */
          fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
          fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);

          xpt=FindXPointTest(a,i6);
          if (xpt==NULL) {*ef|=DGFE_NEQUIL;goto bad;}

          dMin=MAXDOUBLE;xpsMin=NULL;
          for (xps=Group1st(xpt->segs,&ix);xps!=NULL;xps=Next(&ix)) {
            angle=CalcXPointSegAngle(a,xps,xpt);
            d=fabs(angle-f3);
            if (d>180) d=fabs(d-360); /* Correct for "-179 vs +179" */
            if (d<dMin) {dMin=d;xpsMin=xps;}
          }
          if (xpsMin==NULL) goto bad;

          xptMin=xpsMin->xpt;
          numberMin=xpsMin->number;

          dMin=MAXDOUBLE;xpsMin=NULL;
          for (xps=Group1st(xpt->segs,&ix);xps!=NULL;xps=Next(&ix)) {
            if (xps->number!=numberMin || xps->xpt!=xptMin) continue;
            d=fabs(f1/f4-xps->startPos/xps->lineLen);
            if (d<dMin) {dMin=d;xpsMin=xps;}
          }

          gps=FindGridPointSegBySegment(a,xpsMin);
          assert(gps!=NULL);

          SetGridPointSegZone(gps,i3);
          SetGridPointSegDir(gps,i4);
          SetGridPointSegTargetDir(gps,i5);
          SetGridPointSegShortName(gps,s1);
          SetGridPointSegLongName(gps,s2);

          if (xpt!=gps->xps->xpt) {
            SetGridPointSegDir(gps,-gps->dir);
            SetGridPointSegTargetDir(gps,-gps->targetDir);
          }

          fl=0;Str2Flags(s3,&fl,gridPointSegFlags);
          SetGridPointSegFlags(gps,fl);

          GroupAdd(g,gps->xps);

          /*xps=AddXPointSeg(a,xpt,i2,f1,f2);
          if (xps==NULL) {*ef|=DGFE_NEQUIL;goto bad;} */

          bad:;
        } else *ef|=DGFE_SYNTAX;
      }
      for (xps=AppXPointSeg1st(a,&ix);xps!=NULL;xps=Next(&ix))
        if (!InGroup(g,xps)) DelXPointSeg(a,xps);

      FreeGroup(g);

    } else
    if (sscanf(s,"SurfaceZones114 %u",&n)==1) {
      for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix))
        DelSurfaceZone(sz);
      for (k=0;k<n;k++) {
        fgets(s,sizeof(s)-1,f); /* szNo2==-1 -> unlimited */

        r=sscanf(s,"%d%d%d%d%s",&i1,&i2,&i3,&i4,s3);
        if (r==4) {strcpy(s3,"");r=5;}
        if (r==5) {
          fgets(s1,sizeof(s1)-1,f);RemoveLF(s1);
          fgets(s2,sizeof(s2)-1,f);RemoveLF(s2);
          sz=AddSurfaceZone(a,i1,i2,i3,i4,-1);
          if (sz==NULL) {*ef|=DGFE_NEQUIL;goto badSZ;}
          SetSurfaceZoneShortName(sz,s1);
          SetSurfaceZoneLongName(sz,s2);

          fl=0;Str2Flags(s3,&fl,surfaceZoneFlags);
          SetSurfaceZoneFlags(sz,fl);

          badSZ:;
        }
        else *ef|=DGFE_SYNTAX;
      }
    } else

    if (sscanf(s,"TopoName114 %s",s1)==1) {
      SetTopologyName(a,s1);
    } else

    *ef |= DGFE_SYNTAX;
  }

  fclose(f);
  
  RecalcTopologyCache(a);
  return 0;
}

/* Private; used only from Read[Old]DgFile */
/* Converts XY-Surfaces into regular ones using "old" topology */
void ConvertOldDgSurfaces(App a,int* ef) {
  SurfaceEx sx;
  Index ix;
  int signInside=0;
  Group g;
  double level;
  int zone;

  for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix))
    if (sx->zone==SZN_XY && SurfaceExClosed(sx)) break;

  if (sx!=NULL) signInside=sx->level>0? 1 : -1;

  g=CopyGroup(a->surfacesEx,NULL);
  for (sx=Group1st(g,&ix);sx!=NULL;sx=Next(&ix)) {
    if (sx->zone!=SZN_XY) continue;
    level=sx->level;
    if (SurfaceExClosed(sx)) zone=1;
    else if (sx->level*signInside<0) zone=2;
    else zone=3;
    DelSurfaceEx(sx);
    if (signInside) AddSurfaceEx(a,zone,level,NULL);
    else *ef |= DGFE_OLD_SURFACES_LOST;
  }
  FreeGroup(g);
}

/* Returns # of radial cells */
char* GetSurfacesStatsStr(App a) {
  SurfaceZone sz;
  SurfaceEx sx;
  Index ixsz,ixsx;
  int n;
  static char s[2048];
  char buf[2048];

  *s=0;
  for (sz=AppSurfaceZone1st(a,&ixsz);sz!=NULL;sz=Next(&ixsz)) {
    for (n=0,sx=AppSurfaceEx1st(a,&ixsx);sx!=NULL;sx=Next(&ixsx))
      if (SurfaceExOk(sx) && sx->zone==sz->zone) n++;

      /* Add one extra cell if limited by segments from both sides */
      if (sz->gpZone2>=0) n++;
    sprintf(buf,"%d%s ",n,GetSurfaceZoneShortName(sz));
    strcat(s,buf);
  }

  return s;
}

/* Return the # of poloidal cells */
char* GetGridPointStatsStr(App a) {
  GridPointSeg gps;
  GridPointEx gpx;
  Index ixsz,ixsx;
  int n;
  static char s[2048];
  char buf[2048];

  *s=0;
  for (gps=AppGridPointSeg1st(a,&ixsz);gps!=NULL;gps=Next(&ixsz)) {
    if (!GridPointSegIsUsed(gps)) continue;
    for (n=0,gpx=AppGridPointEx1st(a,&ixsx);gpx!=NULL;gpx=Next(&ixsx))
      if (GridPointExOk(gpx) && gpx->zone==gps->zone) n++;

    n++; /* # of cells = # of grid points + 1 */
    sprintf(buf,"%d%s ",n,GetGridPointSegShortName(gps));
    strcat(s,buf);
  }

  return s;
}


