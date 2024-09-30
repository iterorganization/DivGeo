#include "dg.h"

#define FSTR_ORIGINS         "p1\n"
#define FSTR_DEFORIGIN       " 0,0,0\n"
#define FSTR_ENDS            "p2\n"
#define FSTR_DEFEND          " 1,1,1\n"
#define FSTR_UNUSEDNUMBERS   "misselem\n"
#define FSTR_USERBEGIN       "userdata\n"
#define FSTR_USEREND         "finish\n"
#define FSTR_SEPARATORS      "sprtrs\n"
#define FSTR_CELLS           "cells %d\n"
#define FSTR_SOURCES         "sources\n"
#define FSTR_PTCOORDS        "  %e, %e, %e\n"   /* relcheck_ignore_line */
#define FSTR_EQUILFILE       "# equil %s\n"
#define FSTR_MESHFILE        "# mesh %s\n"
#define FSTR_TOPONAME        "# topo %s\n"
#define FSTR_CHORDS          "chords\n"
#define FSTR_CHORDCOORDS     "  %e, %e, %e, %e\n" /* relcheck_ignore_line */
#define FSTR_XPOINTCENTER    "xptcntr %e, %e, %e\n" /* relcheck_ignore_line */
#define FSTR_NXPOINTCENTER   "xptcntr\n"
#define FSTR_XLOOPCENTER     "xlpcntr %e, %e, %e\n" /* relcheck_ignore_line */
#define FSTR_NXLOOPCENTER     "xlpcntr\n"
#define FSTR_SURFDELTA1      "dltr1\n"
#define FSTR_SURFDELTA2      "dltrn\n"
#define FSTR_SURFCOUNT       "npr\n"
#define FSTR_SURF_PENETRATE  "pntrat %e\n"      /* relcheck_ignore_line */
#define FSTR_NSURF_PENETRATE  "pntrat\n"
#define FSTR_GPOINTDELTA1    "dltp1\n"
#define FSTR_GPOINTDELTA2    "dltpn\n"
#define FSTR_GPOINTCOUNT     "nptseg\n"
#define FSTR_CLOSEDSTRUCTPARTS "clstruct %d\n"
#define FSTR_TARGET_LINE     "target %d\nline\n"

static int VarDefSortProc(void* pvd1,void* pvd2,void* userData) {
  VarDef vd1=(VarDef)pvd1,vd2=(VarDef)pvd2;

  if (vd2->col>vd1->col) return -1;
  if (vd2->col<vd1->col) return 1;

  if (vd2->row>vd1->row) return -1;
  if (vd2->row<vd1->row) return 1;

  return 0;
}

static void* FindObjectId(App a,int id) {
  Elem e;
  Separator sep;
  Index ix;

  for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix))
    if (e->id==id) return e;

  for (sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix))
    if (sep->id==id) return sep;

  return NULL;
}

static int GetObjectId(void* object) {
  MeshElement me;
  int i,vL,vR;

  vL=vR=MAXINT;

  switch(GetObjType(object)) {
    case T_ELEM:
      return ((Elem)object)->id;
    case T_SEPARATOR:
      return ((Separator)object)->id;
    case T_MESHELEMENT:
      me=object;
      for (i=0;i<2;i++)
        if (me->cells[i]!=NULL) {
          if (me->cells[i]->elements[me->bVertical? MCN_LEFT:MCN_TOP]==me)
            vR=me->cells[i]->eN; else
          if (me->cells[i]->elements[me->bVertical? MCN_RIGHT:MCN_BOTTOM]==me)
            vL=me->cells[i]->eN;
        }
      if (vR!=MAXINT) return vR;
      else if (vL!=MAXINT) return -vL-1;
      else assert(0);
    default:
      assert(0);
  }
  return 0;
}

static void OutputVar(App a,FILE* f,void* obj,VarDef vd,VarSet vs) {
  Index ix;
  Elem e;
  Chord ch;
  MeshCell mc;
  MeshElement me;
  Group g;
  char* s;
  int i;

  ValidatePtr(vd,"OutputVar__");

  if (obj==NULL) switch(vd->varType) {
    case VT_INT:
      zfprintf(f,"  0\n");
      break;
    case VT_FLOAT:
      zfprintf(f,"  0\n");
      break;
    case VT_TEXT:
      zfprintf(f,"  dummy\n");
      break;
    case VT_FILENAME:
      zfprintf(f,"  dummy\n");
      break;
    default:
      FatalError("OutputVar()-null/type%d: fatal error 1",vd->varType);
  } else
  if (vd->varType & VTF_HASELEMS) {
    g=GetVar(obj,vd,vs);
    if (g==NULL) return;
    if (vd->varType & VTF_STRUCTPART) {
      for (i=0,e=Group1st(GetVarPtrByType(a,VT_STRUCTURE)->val,&ix);
          e!=NULL;e=Next(&ix),i++)
        if (InGroup(g,e)) zfprintf(f,"  %d\n",i+1);
    } else {
      for (e=Group1st(g,&ix);e!=NULL;e=Next(&ix))
        zfprintf(f,"  %d\n", e->id);
    }
  } else if (vd->varType & VTF_HASCHORDS) {
    g=GetVar(obj,vd,vs);
    if (g==NULL) return;
    for (ch=Group1st(g,&ix);ch!=NULL;ch=Next(&ix))
      zfprintf(f,"  %d\n",GroupIndex(a->chords,ch)+1);
  } else if (vd->varType & VTF_HAS_MESH_OBJECTS) {
    g=GetVar(obj,vd,vs);
    switch(vd->varType) {
      case VT_MESH_CELLS:
        for (mc=Group1st(g,&ix);mc!=NULL;mc=Next(&ix))
          zfprintf(f,"  %d\n",mc->eN);
        break;
      case VT_MESH_ELEMENTS:
        for (i=0,me=Group1st(g,&ix);me!=NULL;me=Next(&ix))
          if (!me->bVertical) i++;
        zfprintf(f,"  %d\n",i); /* # of horizontal elements */
        for (i=0,me=Group1st(g,&ix);me!=NULL;me=Next(&ix))
          if (!me->bVertical) zfprintf(f,"  %d\n",GetObjectId(me));
        for (i=0,me=Group1st(g,&ix);me!=NULL;me=Next(&ix))
          if (me->bVertical) zfprintf(f,"  %d\n",GetObjectId(me));
        break;
      case VT_MESH_H_ELEMENTS:
      case VT_MESH_V_ELEMENTS:
        for (i=0,me=Group1st(g,&ix);me!=NULL;me=Next(&ix))
          if (!me->bVertical == !(vd->varType==VT_MESH_V_ELEMENTS))
            zfprintf(f,"  %d\n",GetObjectId(me));
        break;
      default:
        assert(0);
    }
  } else if (!(vd->varType & VTM_HASGROUP)) {
    s=GetVar(obj,vd,vs);
    zfprintf(f,"  %s\n",s==NULL ? "" : s);
  } else assert(0);
}

int WriteOutputFile(App a,char* fName) {
  FILE* f;
/*  char* val; */
  VarSetDef vsd;
  VarSet vs;
  VarDef vd;
  Var v;
  Elem e;
  Separator sep;
  Source src;
  Chord ch;
  CellsInfo ci;
  Group g;
  XPointTest xpt;
  void* obj;
  Index ivs,ivsd,ivd,ix;
  int i,b,maxId,r;
  double x,y,lvl;
  SurfaceZone sz;

  r=GetAppFlags(a);
  if (~r & AF_VALIDVARS)       return ERR_EMPTYVARS;
  if (~r & AF_VALIDSTRUCTURE)  return ERR_EMPTYSTRUCTURE;
  if (~r & AF_VALIDTARGETS)    return ERR_EMPTYTARGETS;
  if (~r & AF_VALIDSURFACES)   return ERR_BADSURFACES;
  if (~r & AF_VALIDGRIDPOINTS) return ERR_BADGRIDPOINTS;
  if (~r & AF_VALIDCELLS)      return ERR_BADCELLS;

/* Determine the maximum element number actually used */
  maxId=0;
  for (i=0;i<=a->maxElemId;i++) {
    obj=FindObjectId(a,i);
    if (obj==NULL) continue;
    maxId=i;
  }

  if (fName!=NULL) {
    f=fopen(fName,"w");
    if (f==NULL) return ERR_FWRITE;
  } else f=NULL;

  zfprintf(f,FSTR_USERBEGIN);

  zfprintf(f,FSTR_ORIGINS);
  for (i=1;i<=maxId;i++) {
    obj=FindObjectId(a,i);
    if (obj==NULL) zfprintf(f,FSTR_DEFORIGIN); else
      switch(GetObjType(obj)) {
        case T_ELEM:
          e=obj;
          zfprintf(f,FSTR_PTCOORDS,e->n[1]->x,e->n[1]->y,e->n[1]->z);
          break;
        case T_SEPARATOR:
          sep=obj;
          zfprintf(f,FSTR_PTCOORDS,sep->x,sep->y,0.0);
          break;
        default:
          FatalError("WriteOutputFile()-type%d: fatal error 12",
            GetObjType(obj));
      }
  }

  zfprintf(f,FSTR_ENDS);
  for (i=1;i<=maxId;i++) {
    obj=FindObjectId(a,i);
    if (obj==NULL) zfprintf(f,FSTR_DEFEND); else
      switch(GetObjType(obj)) {
        case T_ELEM:
          e=obj;
          zfprintf(f,FSTR_PTCOORDS,e->n[2]->x,e->n[2]->y,e->n[2]->z);
          break;
        case T_SEPARATOR:
          sep=obj;
          zfprintf(f,FSTR_PTCOORDS,sep->n->x,sep->n->y,sep->n->z);
          break;
        default:
          FatalError("WriteOutputFile()-type%d: fatal error 13",
            GetObjType(obj));
      }
  }

  zfprintf(f,FSTR_UNUSEDNUMBERS);
  for (i=1;i<=maxId;i++) {
    obj=FindObjectId(a,i);
    if (obj==NULL /*|| IsChordElem(obj)*/) zfprintf(f,"  %d\n",i);
  }

  zfprintf(f,FSTR_SEPARATORS);
  for (i=1;i<=maxId;i++) {
    obj=FindObjectId(a,i);
    if (obj!=NULL && GetObjType(obj)==T_SEPARATOR)
      zfprintf(f,"  %d\n",i);
  }

  zfprintf(f,FSTR_CHORDS);
  for (ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix))
    zfprintf(f,FSTR_CHORDCOORDS,ch->x1,ch->y1,ch->x2,ch->y2);

  zfprintf(f,FSTR_SOURCES);
  for (src=AppSource1st(a,&ix);src!=NULL;src=Next(&ix))
    zfprintf(f,"  %e, %e\n",src->x,src->y);

  ci=CreateCellsInfo(a,NULL,NULL);
  if (ci!=NULL) {
    for (i=0;ci[i]!=NULL;i++) {
      zfprintf(f,FSTR_CELLS,i+1);
      for (obj=Group1st(ci[i],&ix);obj!=NULL;obj=Next(&ix))
        zfprintf(f,"  %d\n",GetObjectId(obj));
    }
    ci=FreeCellsInfo(ci);
  }

  for (vsd=AppVarSetDef1st(a,&ivsd);vsd!=NULL;vsd=Next(&ivsd)) {
    for (i=1,vs=Group1st(vsd->varSets,&ivs);vs!=NULL;i++,vs=Next(&ivs)) {
      b=0;
      g=CopyGroup(vsd->varDefs,NULL);
      GroupQSort(g,VarDefSortProc,NULL);
      for (vd=Group1st(g,&ivd);vd!=NULL;vd=Next(&ivd)) {
        if (vd->flags & VF_NOEXPORT) continue;
        if (!b) zfprintf(f,"%s %d\n",vsd->name,i);
        b=1;
        zfprintf(f,"%s%c",vd->name,
        vd->flags&VFM_MULTIPLE || vd->varType & VTM_HASGROUP ? '\n' : ' ');
        if (!(vd->flags & VFM_MULTIPLE))
          OutputVar(a,f,vs,vd,NULL);
        else {
          if (vd->flags & (VF_FORELEMS | VF_FORSEPARATORS)) {
            for (i=1;i<=maxId;i++) {
              obj=FindObjectId(a,i);
              if (obj!=NULL && GetObjType(obj)==T_ELEM &&
                  ~vd->flags & VF_FORELEMS) obj=NULL;
              if (obj!=NULL && GetObjType(obj)==T_SEPARATOR &&
                  ~vd->flags & VF_FORSEPARATORS) obj=NULL;
              OutputVar(a,f,obj,vd,vs);
            }
          }
          if (vd->flags & VF_FORSOURCES)
            for (obj=AppSource1st(a,&ix);obj!=NULL;obj=Next(&ix))
              OutputVar(a,f,obj,vd,vs);
          if (vd->flags & VF_FORCHORDS)
            for (obj=AppChord1st(a,&ix);obj!=NULL;obj=Next(&ix))
              OutputVar(a,f,obj,vd,vs);
        }
      }
      g=FreeGroup(g);
    }
  }

/*  if (a->xpoint!=NULL) {
    XY xy;
    int px,py,nx,ny; */

  /* Output X Points */

  zfprintf(f,FSTR_NXPOINTCENTER);
  for (xpt=AppXPointTest1st(a,&ix);xpt!=NULL;xpt=Next(&ix))
    zfprintf(f,"  %e, %e, %e\n",xpt->centerX,xpt->centerY,(double)0);

  /* Output O Points */

  if (a->equil!=NULL) {
    zfprintf(f,FSTR_NXLOOPCENTER);
    for (sz=AppSurfaceZone1st(a,&ix);sz!=NULL;sz=Next(&ix)) {
      if (!(sz->flags & SZF_LIMITBYSURFACE)) continue;
      lvl=MAXDOUBLE; /* Workaround for the PC */
      if (FindSurfaceOriginPointEx(a,sz->zone,lvl,&x,&y)!=0)
        continue;
      if (GetEquilLevel(a->equil,x,y,&lvl,NULL,NULL)) continue;
      zfprintf(f,"  %e, %e, %e\n",x,y,lvl);
    }
  }

  zfprintf(f,FSTR_USEREND);

  if (f!=NULL) fclose(f);
  return 0;
}

/* Writes a structure file */
/* Arguments: */
/*   a      App structure */
/*   fName  Output filename */
/* Returns error code or 0 in case of success */

#define ERROR(s) {r=(s);goto Error;}

int WriteStructureFile(App a,char* fName) {
  FILE* f=NULL;
  int i,j,r=0,bBroken,bCheck;
  Elem e,e0,e00 = NULL,e1;
  Index ix,ix1;
  Var v;
  double xMin,yMin,xMax,yMax;
  static char* fmt="  %e , %e\n";       /* relcheck_ignore_line */

  bCheck=!(a->outputFlags & OF_NC_STRUCTURE);

  r=GetAppFlags(a);
  if (~r & AF_VALIDVARS)       ERROR(ERR_EMPTYVARS);
  if (~r & AF_VALIDSTRUCTURE)  ERROR(ERR_EMPTYSTRUCTURE);
  if (~r & AF_VALIDTARGETS)    ERROR(ERR_EMPTYTARGETS);
  if (~r & AF_VALIDSURFACES)   ERROR(ERR_BADSURFACES);
  if (~r & AF_VALIDGRIDPOINTS) ERROR(ERR_BADGRIDPOINTS);
  if (~r & AF_VALIDCELLS)      ERROR(ERR_BADCELLS);

/* Open output file */
  if (fName!=NULL) {
    f=fopen(fName,"w");
    if (f==NULL) ERROR(ERR_FWRITE);
  } else f=NULL;

/* Do not output empty values */
  v=GetVarPtrByType(a,VT_STRUCTURE);
  if (CheckValue(a,v->val,VT_STRUCTURE,NULL))
    ERROR(bCheck? ERR_BADSTRUCTURE : 0);

/* Calculate bounding rectangle dimensions */
  xMin=0;xMax=-1;
  CalcGroupExtents(v->val,&xMin,&yMin,&xMax,&yMax);

/* Calculate the number of rings */
  for (i=0,e0=NULL,e=Group1st(v->val,&ix);e!=NULL;e0=e,e=Next(&ix)) {
    e1=GetNextElem(e,v->val,1);
    if (e1!=e0 || e1==NULL) i++;
    /* if (GetNextElem(e,v->val,1)!=e0 ||) i++; */
  }

/* Write into the file */
  zfprintf(f,"%d\n",i);

/* ix will be used for ring separation and ix1 for element output */
  e=Group1st(v->val,&ix);
  e0=Group1st(v->val,&ix1);
  j=0; /* Number of the current ring, starting from 1 */

/* Process all rings */
  while (j++,e0!=NULL) {
    e=e0;

/* Follow the ring to figure out the number of elements in it */
/* NOTE: This assumes a formatted structure; otherwise it will crash */
    bBroken=0;
    while (GetNextElem(e,v->val,2)!=e0) {
      if (GetNextElem(e,v->val,2)==NULL) {bBroken=1;break;}
      e=Next(&ix);
    }
    i=GroupIndex(v->val,e)-GroupIndex(v->val,e0)+1;
    e=Next(&ix); /* Will be NULL for the last ring */

/* Write to file */
    zfprintf(f,"%d\n",(j==1 && a->outputMode!=OUTPUTMODE_CARRE) ?
        i+5:i+bBroken);

/* Write nodes */
    for (;e0!=e && e0!=NULL;e00=e0,e0=Next(&ix1))
      zfprintf(f,fmt,e0->n[1]->x,e0->n[1]->y);
    if (bBroken) zfprintf(f,fmt,e00->n[2]->x,e00->n[2]->y);

/* Write the bounding rectangle for the outermost ring */
    if (j==1 && a->outputMode!=OUTPUTMODE_CARRE) {
      e=Group1st(v->val,NULL);
      zfprintf(f,fmt,e->n[1]->x,e->n[1]->y);
      zfprintf(f,fmt,xMax,yMax);
      zfprintf(f,fmt,xMax,yMin);
      zfprintf(f,fmt,xMin,yMin);
      zfprintf(f,fmt,xMin,yMax);
    }
  }

  r=0;

/* Close file */
  Error:;
  if (f!=NULL) fclose(f);

  return r;
}

#undef ERROR

#define ERROR(s) {r=(s);goto Error;}

int SurfaceZoneSortProc(void* p1,void* p2,void* arg) {
  SurfaceZone sz1=(SurfaceZone)p1,sz2=(SurfaceZone)p2;

  assert(sz1->type==T_SURFACEZONE && sz2->type==T_SURFACEZONE);

  if (sz1->zone==sz2->zone) return 0;
  return sz1->zone<sz2->zone? -1 : 1;
}

int GridPointSegSortProc(void* p1,void* p2,void* arg) {
  GridPointSeg gps1=(GridPointSeg)p1,gps2=(GridPointSeg)p2;

  assert(gps1->type==T_GRIDPOINTSEG && gps2->type==T_GRIDPOINTSEG);

  if (gps1->zone==gps2->zone) return 0;
  return gps1->zone<gps2->zone? -1 : 1;
}

static int SurfaceSortProc(void* p1,void* p2,void* pBaseLevel) {
  SurfaceEx sx1=(SurfaceEx)p1,sx2=(SurfaceEx)p2;
  double* pbl=(double*)pBaseLevel;
  double l1,l2,l0=0;

  if (pbl==NULL) pbl=&l0;

  if (sx1->zone!=sx2->zone) return sx1->zone<sx2->zone? -1 : 1;

  assert(sx1->type==T_SURFACEEX && sx2->type==T_SURFACEEX);

  l1=fabs(sx1->level-*pbl);
  l2=fabs(sx2->level-*pbl);

  if (l1==l2) return 0;
  return l1<l2? -1 : 1;
}

static int GridPointSortProc(void* p1,void* p2,void* foo) {
  GridPointEx gpx1=(GridPointEx)p1,gpx2=(GridPointEx)p2;

  assert(gpx1->type==T_GRIDPOINTEX && gpx2->type==T_GRIDPOINTEX);

  if (gpx1->value==gpx2->value) return 0;
  return gpx1->value<gpx2->value? -1 : 1;
}

int WriteTargetsFile(App a,char* fName) {
  FILE* f;
  Elem e;
  Index ix,ixsz,ix1;
  Var v;
  Group t1,t2,pl,st;
  XY xy;
  int i,j,r;
  int bCheck;
/*  Surface s,s0; */
  SurfaceZone sz;
  SurfaceEx sx,sx2;
  GridPointEx gpx,gpx2;
/*  GridPoint gp; */
  GridPointSeg gps;
  static char* fmt="  %e , %e\n";       /* relcheck_ignore_line */
  char* cid = NULL;
  int area,law,carreFlag,count;
  double delta1,delta2,l1,l2,l,x,y;
  Group g1,g2,g3,gSZ=NULL,gGPS=NULL;
  VarSet vs;
  VarDef vd;

  /* Do initial checks */

  r=GetAppFlags(a);
  if (~r & AF_VALIDVARS) return ERR_EMPTYVARS;
  if (~r & AF_VALIDSTRUCTURE) return ERR_EMPTYSTRUCTURE;
  if (~r & AF_VALIDTARGETS) return ERR_EMPTYTARGETS;
  if (~r & AF_VALIDSURFACES) return ERR_BADSURFACES;
  if (~r & AF_VALIDGRIDPOINTS) return ERR_BADGRIDPOINTS;
  if (~r & AF_VALIDCELLS) return ERR_BADCELLS;

  /* Check for virtual surfaces, if needed */

  if (!(a->outputFlags & OF_NC_SURFACES)) {
    for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix))
      if (SurfaceExVirtual(sx)) return ERR_SURFACE_XY;
  }

  /* Open the output file, if asked to */

  if (fName!=NULL) {
    f=fopen(fName,"w");
    if (f==NULL) return ERR_FWRITE;
  } else f=NULL;


  /* Output the equilibrium filename */

  if (a->equil!=NULL) zfprintf(f,FSTR_EQUILFILE,a->equil->fName);

  /* Output the mesh filename */

  if (a->mesh!=NULL) zfprintf(f,FSTR_MESHFILE,a->mesh->fileName);

  /* Output the topology name */

  zfprintf(f,FSTR_TOPONAME,GetTopologyName(a));

  /* Output targets */

  bCheck=!(a->outputFlags & OF_NC_TARGETS);

  /* Create a group with all target vars */
  g1=CreateGroup();
  for (vs=AppVarSet1st(a,&ix1);vs!=NULL;vs=Next(&ix1)) {
    for (vd=Group1st(vs->def->varDefs,&ix);vd!=NULL;vd=Next(&ix))
      if (vd->varType & VTF_TARGET) {
        g2=GetVar(vs,vd,vs);
        r=CheckValue(a,g2,VT_TARGET1,NULL);
        if (r) r=r/*ERR_BADTARGETS*/;
        else if (!IsEmptyGroup(g2)) GroupAdd(g1,g2);
     }
  }

  gGPS=CopyGroup(a->gridPointSegs,NULL);
  GroupQSort(gGPS,GridPointSegSortProc,NULL);

  for (gps=Group1st(gGPS,&ix);gps!=NULL;gps=Next(&ix)) {
    if (!(gps->flags & GPSF_USED)) continue;
/*    if (!GridPointSegOpen(gps)) continue; */
    vd=NULL;
    vs=NULL;

    RecalcGridPointSegLine(gps,&vd,&vs);
    if (vd==NULL || vs==NULL) continue;

    g2=GetVar(vs,vd,vs);
    if (InGroup(g1,g2)) {
      zfprintf(f,FSTR_TARGET_LINE,gps->zone);
      if (gps->flags & GPSF_TARGET_CW) {
        RevertGroup(g2);
        j=1;
      } else j=0;

      e=Group1st(g2,&ix1);
      zfprintf(f,fmt,e->n[1+j]->x,e->n[1+j]->y);
      for (;e!=NULL;e=Next(&ix1))
        zfprintf(f,fmt,e->n[2-j]->x,e->n[2-j]->y);
      if (gps->flags & GPSF_TARGET_CW) RevertGroup(g2); /* RESTORE the var! */
      GroupDel(g1,g2);
    } else {
      r=ERR_TARGET_CROSSED_2X;
    }
  }

  if (!r && !IsEmptyGroup(g1)) r=ERR_TARGET_NOT_CROSSED;
  g1=FreeGroup(g1);

  if (bCheck && r) goto Error;

  bCheck=0;

  /* Surfaces    DO NOT COMMENT OUT because of FreeGroup() at the end */

  bCheck=!(a->outputFlags & OF_NC_SURFACES);

  gSZ=CopyGroup(a->surfaceZones,NULL);
  GroupQSort(gSZ,SurfaceZoneSortProc,NULL);

  for (sz=Group1st(gSZ,&ixsz);sz!=NULL;sz=Next(&ixsz)) {
    gps=FindGridPointSeg(a,sz->gpZone1);
    l= gps==NULL? 0 : gps->level;
    g2=CreateGroup();
    for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix)) {
      if (SurfaceExOk(sx) && sx->zone==sz->zone) GroupAdd(g2,sx);
    }
    GroupQSort(g2,SurfaceSortProc,(void*)&l);

    zfprintf(f,"region %d\nlevels\n",sz->zone);
    for (sx=Group1st(g2,&ix);sx!=NULL;sx=Next(&ix))
      zfprintf(f,"  %e\n",sx->level);

    g2=FreeGroup(g2);
  }

  /* Grid points     DO NOT COMMENT OUT because of FreeGroup() at the end*/

  bCheck=!(a->outputFlags & OF_NC_GPOINTS);

  for (gps=Group1st(gGPS,&ixsz);gps!=NULL;gps=Next(&ixsz)) {
    if (!(gps->flags & GPSF_USED)) continue;
    g2=CreateGroup();
    for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix)) {
      if (GridPointExOk(gpx) && gpx->zone==gps->zone) GroupAdd(g2,gpx);
    }
    GroupQSort(g2,GridPointSortProc,NULL);

    zfprintf(f,"zone %d\npoints\n",gps->zone);
    for (gpx=Group1st(g2,&ix);gpx!=NULL;gpx=Next(&ix))
      zfprintf(f,"  %e\n",gpx->value);

    g2=FreeGroup(g2);
  }

/*  if (a->xpoint!=NULL) for (i=0;i<3;i++) {
    zfprintf(f,"zone %d\npoints\n",i);
    for (gp=AppGridPoint1st(a,&ix);gp!=NULL;gp=Next(&ix))
      if (gp->area==i) zfprintf(f,"  %e\n",gp->value);
  } */

  /* Carre extras */

  if (a->outputMode==OUTPUTMODE_CARRE) {

    /* Make sure surfaces are intact */

    bCheck=!(a->outputFlags & OF_NC_SURFACES);
    r=0;

/*    for (i=0;i<=3;i++) cid[i]=NULL; */
    g1=CreateGroup();

    for (sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix)) {
      if (!SurfaceExOk(sx)) continue;

      for (sx2=Group1st(g1,&ix1);sx2!=NULL;sx2=Next(&ix1))
        if (sx2->zone==sx->zone) break;

      if (sx2==NULL) {
        GroupAdd(g1,sx);
        cid=GetSurfaceExCreatorId(sx);
        if (!SurfaceCreatorIdUnchanged(cid)) r=ERR_CARRE_SURFACES;
        if (!r && ParseSurfaceCreatorId(cid,&area,&count,&delta1,&delta2,
            &law,&l1,&l2,&carreFlag)) r=ERR_CARRE_SURFACES;
        if (!r && !carreFlag) r=ERR_CARRE_SURFACES;
        if (!r && CountSurfaces(a,sx->zone)!=
            GetCarreSurfaceExCount(a,sx->zone,count))
          r=ERR_CARRE_SURFACES;


        if (!r) j=FindCarreMinMaxSurfaceLevel(a,sx->zone,&l1,&l2,NULL);
        if (!r && j) r=ERR_CARRE_TANGENT;
        if (!r && strcmp(cid,ConstructSurfaceCreatorId(area,count,delta1,
            delta2,law,l1,l2,carreFlag)))
          r=ERR_CARRE_TANGENT;
      } else if (!r && strcmp(cid,GetSurfaceExCreatorId(sx)))
        r=ERR_CARRE_SURFACES;
    }

    /* Output Carre values for surfaces */

    if (!r) {
      zfprintf(f,FSTR_SURFDELTA1);
      for (sz=Group1st(gSZ,&ix);sz!=NULL;sz=Next(&ix)) {
        for (sx=Group1st(g1,&ix1);sx!=NULL;sx=Next(&ix1))
          if (sx->zone==sz->zone) break;
        cid= sx!=NULL? GetSurfaceExCreatorId(sx) : NULL;
        if (cid==NULL) {delta1=delta2=1;l2=1;l1=count=0;}
        else assert(!ParseSurfaceCreatorId(cid,&area,&count,&delta1,&delta2,
                &law,&l1,&l2,&carreFlag));
        zfprintf(f,"  %e\n",delta1*(l2-l1));
      }

      zfprintf(f,FSTR_SURFDELTA2);
      for (sz=Group1st(gSZ,&ix);sz!=NULL;sz=Next(&ix)) {
        for (sx=Group1st(g1,&ix1);sx!=NULL;sx=Next(&ix1))
          if (sx->zone==sz->zone) break;
        cid= sx!=NULL? GetSurfaceExCreatorId(sx) : NULL;
        if (cid==NULL) {delta1=delta2=1;l2=1;l1=count=0;}
        else assert(!ParseSurfaceCreatorId(cid,&area,&count,&delta1,&delta2,
                &law,&l1,&l2,&carreFlag));
        zfprintf(f,"  %e\n",delta2*(l2-l1));
      }

      zfprintf(f,FSTR_SURFCOUNT);
      for (sz=Group1st(gSZ,&ix);sz!=NULL;sz=Next(&ix)) {
        for (sx=Group1st(g1,&ix1);sx!=NULL;sx=Next(&ix1))
          if (sx->zone==sz->zone) break;
        cid= sx!=NULL? GetSurfaceExCreatorId(sx) : NULL;
        if (cid==NULL) {delta1=delta2=1;l2=1;l1=count=0;}
        else assert(!ParseSurfaceCreatorId(cid,&area,&count,&delta1,&delta2,
                &law,&l1,&l2,&carreFlag));
        zfprintf(f,"  %d\n",count+2);
      }
    }
    FreeGroup(g1);

    if (r && bCheck) goto Error;


    /* Output the position of the innermost surface */

    r=0;

    if (!r) zfprintf(f,FSTR_NSURF_PENETRATE);
    for (sz=Group1st(gSZ,&ix);sz!=NULL;sz=Next(&ix)) {

      if (!(sz->flags & SZF_LIMITBYSURFACE)) continue;
      gps=FindGridPointSeg(a,sz->gpZone1);
      if (gps==NULL) {
        r=ERR_BADGPZONENUMBER;
        break;
      }
      l=gps->level;

      for (sx2=NULL,sx=AppSurfaceEx1st(a,&ix1);sx!=NULL;sx=Next(&ix1))
        if (SurfaceExOk(sx) && sx->zone==sz->zone && (sx2==NULL ||
            fabs(sx->level-l)>fabs(sx2->level-l))) sx2=sx;

      if (sx2==NULL) r=ERR_BADSURFACES;

      if (!r) {
        l=MAXDOUBLE; /* Workaround for the PC */
        r=FindSurfaceOriginPointEx(a,sz->zone,l,&x,&y);

        if (!r) {
          pl=CreateGroup();
          AddXY(pl,gps->xps->xpt->centerX,gps->xps->xpt->centerY);
          AddXY(pl,x,y);

          r=PolyLinesIntersect(pl,sx2->line,&l,NULL);
          if (r) r=ERR_CARRE_PEAK;
          FreeMallocedGroup(pl);
        }
      }
      if (!r) zfprintf(f,"  %e\n",l);
    }

    if (r && bCheck) goto Error;

    /* Output Carre values for grid points */

    /* Make sure grid points are intact */

    bCheck=!(a->outputFlags & OF_NC_GPOINTS);
    r=0;

    g1=CreateGroup();
    for (gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix)) {
      if (!GridPointExOk(gpx)) continue;

      for (gpx2=Group1st(g1,&ix1);gpx2!=NULL;gpx2=Next(&ix1))
        if (gpx2->zone==gpx->zone) break;

      if (gpx2==NULL) {
        GroupAdd(g1,gpx);
        cid=GetGridPointExCreatorId(gpx);
        if (!GridPointCreatorIdUnchanged(cid))
          r=ERR_CARRE_GPOINTS;
        if (!r && ParseGridPointCreatorId(cid,&area,&count,&delta1,
            &delta2,&law,&carreFlag)) r=ERR_CARRE_GPOINTS;
        if (!r && !carreFlag) r=ERR_CARRE_GPOINTS;
        if (!r && CountGridPoints(a,gpx->zone)!=count)
          r=ERR_CARRE_GPOINTS;
      } else if (strcmp(cid,GetGridPointExCreatorId(gpx)))
        r=ERR_CARRE_GPOINTS;
    }

    if (!r) {
      zfprintf(f,FSTR_GPOINTDELTA1);
      for (gps=Group1st(gGPS,&ix);gps!=NULL;gps=Next(&ix)) {
        if (!(gps->flags & GPSF_USED)) continue;
        for (gpx=Group1st(g1,&ix1);gpx!=NULL;gpx=Next(&ix1))
          if (gpx->zone==gps->zone) break;
        cid= gpx!=NULL? GetGridPointExCreatorId(gpx) : NULL;
        if (cid==NULL) {delta1=delta2=1;count=0;} else {
          assert(!ParseGridPointCreatorId(cid,&area,&count,&delta1,&delta2,
              &law,&carreFlag));
          delta1*=gps->lineLength;
        }
        zfprintf(f,"  %e\n",delta1);
      }

      zfprintf(f,FSTR_GPOINTDELTA2);
      for (gps=Group1st(gGPS,&ix);gps!=NULL;gps=Next(&ix)) {
        if (!(gps->flags & GPSF_USED)) continue;
        for (gpx=Group1st(g1,&ix1);gpx!=NULL;gpx=Next(&ix1))
          if (gpx->zone==gps->zone) break;
        cid= gpx!=NULL? GetGridPointExCreatorId(gpx) : NULL;
        if (cid==NULL) {delta1=delta2=1;count=0;} else {
          assert(!ParseGridPointCreatorId(cid,&area,&count,&delta1,&delta2,
              &law,&carreFlag));
          delta2*=gps->lineLength;
        }
        zfprintf(f,"  %e\n",delta2);
      }

      zfprintf(f,FSTR_GPOINTCOUNT);
      for (gps=Group1st(gGPS,&ix);gps!=NULL;gps=Next(&ix)) {
        if (!(gps->flags & GPSF_USED)) continue;
        for (gpx=Group1st(g1,&ix1);gpx!=NULL;gpx=Next(&ix1))
          if (gpx->zone==gps->zone) break;
        cid= gpx!=NULL? GetGridPointExCreatorId(gpx) : NULL;
        if (cid==NULL) {delta1=delta2=1;count=0;} else {
          assert(!ParseGridPointCreatorId(cid,&area,&count,&delta1,&delta2,
              &law,&carreFlag));
          delta1*=gps->lineLength;
        }
        zfprintf(f,"  %d\n",count+2);
      }
    }

    FreeGroup(g1);

    if (r && bCheck) goto Error;

    /* Output the number of closed structure parts */

    bCheck=!(a->outputFlags & OF_NC_STRUCTURE);
    r=0;

    v=GetVarPtrByType(a,VT_STRUCTURE);
    if (v==NULL || v->val==NULL) st=NULL;
    else st=v->val;

    if (st!=NULL) {
      r=OrderStructureElems(a,st,&g1,&g2,&g3,NULL);
      if (!r) {
        zfprintf(f,FSTR_CLOSEDSTRUCTPARTS,GroupCount(g1)+GroupCount(g2));
        FreeStructureInfo(&g1,&g2,&g3);
      } else if (bCheck) goto Error;
    }
  } /* Carre */

  r=0;

  Error:
  if (f!=NULL) fclose(f);

  if (gSZ!=NULL) gSZ=FreeGroup(gSZ);
  if (gGPS!=NULL) gGPS=FreeGroup(gGPS);

  return r;
}

#undef ERROR


int GetAppFlags(App a) {
  int r=0,b;
  Var v;
  VarSet vs;
  VarDef vd;
  Elem e;
  Separator sep;
  Source src;
  CellsInfo ci;
  int i;
  Index ix,ix1,ix2;

  ValidatePtr(a,"GetAppFlags");

  r|=(AF_VALIDTARGETS|AF_VALIDSTRUCTURE);
  for (vs=AppVarSet1st(a,&ix);vs!=NULL;vs=Next(&ix)) {
    for (vd=Group1st(vs->def->varDefs,&ix1);vd!=NULL;vd=Next(&ix1)) {
      if (vd->varType & VTF_TARGET) {
        if (CheckValue(a,GetVar(vs,vd,NULL),vd->varType,NULL))
          r&=~AF_VALIDTARGETS;
      } else if (vd->varType == VT_STRUCTURE) {
        if (CheckValue(a,GetVar(vs,vd,NULL),vd->varType,NULL))
          r&=~AF_VALIDSTRUCTURE;
      }
    }
  }
  if (GetVarPtrByType(a,VT_STRUCTURE)==NULL) r&=~AF_VALIDSTRUCTURE;

/*  v=GetVarPtrByType(a,VT_TARGET1);
  if (CheckValue(a,v->val,VT_TARGET1,NULL))
    r&=~AF_VALIDTARGETS;
  v=GetVarPtrByType(a,VT_TARGET2);
  if (CheckValue(a,v->val,VT_TARGET2,NULL))
    r&=~AF_VALIDTARGETS;

  r|=AF_VALIDSTRUCTURE;
  v=GetVarPtrByType(a,VT_STRUCTURE);
  if (CheckValue(a,v->val,VT_STRUCTURE,NULL))
    r&=~AF_VALIDSTRUCTURE; -- old */

/*  if (a->equil!=NULL && a->equil->signInside &&
      CountSurfaces(a,2) && CountSurfaces(a,1)==CountSurfaces(a,3)) */
    r|=AF_VALIDSURFACES;

/*  if (a->xpoint!=NULL)   */
    r|=AF_VALIDGRIDPOINTS;

  if (!CheckAllVars(a,NULL)) r|= AF_VALIDVARS;

  r|=AF_VALIDCELLS;
  if (!IsEmptyGroup(a->separators)) {
    ci=CreateCellsInfo(a,&i,NULL);
    if (ci!=NULL) ci=FreeCellsInfo(ci);
    else {
      r &=~ AF_VALIDCELLS;
    }
  }


  if (a->outputFlags & OF_NC_VARS)      r|=AF_VALIDVARS;
  if (a->outputFlags & OF_NC_STRUCTURE) r|=AF_VALIDSTRUCTURE;
  if (a->outputFlags & OF_NC_TARGETS)   r|=AF_VALIDTARGETS;
  if (a->outputFlags & OF_NC_CELLS)     r|=AF_VALIDCELLS;
  if (a->outputFlags & OF_NC_SURFACES)  r|=AF_VALIDSURFACES;
  if (a->outputFlags & OF_NC_GPOINTS)   r|=AF_VALIDGRIDPOINTS;

  return r;
}

/* Check all values of a variable
   Returns     The number of encountered errors
   *pgObjects  Offending object if VTF_HASGROUP; else objects with
               offending values
   *pErr       Error value or ERR_MULTIPLEERRORS if it is the case */

int CheckVar(App a,Group* pgObjects,VarDef vd,VarSet vs,int* pErr) {
  void* obj;
  Index ix;
  int e,err=0,r=0;

  if (pgObjects!=NULL) *pgObjects=CreateGroup();
  if (pErr!=NULL) *pErr=0;

  if (!(vd->flags & VFM_MULTIPLE)) {
    if ((e=CheckValue(a,GetVar(vs,vd,NULL),vd->varType,&obj))) {
      if (!err) err=e;
      if (err!=e) err=ERR_MULTIPLEERRORS;
      if (pgObjects!=NULL) if (obj!=NULL) GroupAdd(*pgObjects,obj);
      r++;
    }
  }

  if (vd->flags & VF_FORELEMS) {
    for (obj=AppElem1st(a,&ix);obj!=NULL;obj=Next(&ix)) {
      if ((e=CheckValue(a,GetVar(obj,vd,vs),vd->varType,NULL))) {
        if (!err) err=e;
        if (err!=e) err=ERR_MULTIPLEERRORS;
        if (pgObjects!=NULL) GroupAdd(*pgObjects,obj);
        r++;
      }
    }
  }

  if (vd->flags & VF_FORSEPARATORS) {
    for (obj=AppSeparator1st(a,&ix);obj!=NULL;obj=Next(&ix)) {
      if ((e=CheckValue(a,GetVar(obj,vd,vs),vd->varType,NULL))) {
        if (!err) err=e;
        if (err!=e) err=ERR_MULTIPLEERRORS;
        if (pgObjects!=NULL) GroupAdd(*pgObjects,obj);
        r++;
      }
    }
  }

  if (vd->flags & VF_FORSOURCES) {
    for (obj=AppSource1st(a,&ix);obj!=NULL;obj=Next(&ix)) {
      if ((e=CheckValue(a,GetVar(obj,vd,vs),vd->varType,NULL))) {
        if (!err) err=e;
        if (err!=e) err=ERR_MULTIPLEERRORS;
        if (pgObjects!=NULL) GroupAdd(*pgObjects,obj);
        r++;
      }
    }
  }

  if (pErr!=NULL) *pErr=err;

  return r;
}

/* Checks all variables for valid values
   Returns the count of encountered errors */

int CheckAllVars(App a,Group* pgErrorPairs) {
  VarDef vd;
  VarSet vs;
  Index ivd,ivs;
  int r=0;

  if (pgErrorPairs!=NULL) *pgErrorPairs=CreateGroup();

  for (vd=AppVarDef1st(a,&ivd);vd!=NULL;vd=Next(&ivd)) {
    for (vs=Group1st(vd->varSetDef->varSets,&ivs);vs!=NULL;vs=Next(&ivs)) {
      if (CheckVar(a,NULL,vd,vs,NULL)) {
          r++;
          if (pgErrorPairs!=NULL) GroupAdd(*pgErrorPairs,vd);
          if (pgErrorPairs!=NULL) GroupAdd(*pgErrorPairs,vs);
      }
    }
  }

  return r;
}
