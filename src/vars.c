#include "dg.h"

static int CheckGroupForType(Group g,int varType,void** pErrObj);

void SetObjDouble(App a,void* obj,int fieldOffset,double value,int bRedraw) {
  struct _ChangeObjDoubleRec ar;

  ar.objChange=obj;
  ar.fieldOffset=fieldOffset;
  ar.length=sizeof(double);
  ar.value=value;
  ar.bRedraw=bRedraw;

  ActChangeObjDouble(a,&ar);
}

void SetObjInt(App a,void* obj,int fieldOffset,int length,long value,
    int bRedraw) {
  struct _ChangeObjIntRec ar;

  ar.objChange=obj;
  ar.fieldOffset=fieldOffset;
  ar.length=length;
  ar.value=value;
  ar.bRedraw=bRedraw;

  ActChangeObjInt(a,&ar);
}

void SetObjPointer(App a,void* obj,int fieldOffset,int length,void* value,
    int bRedraw) {
  struct _ChangeObjPointerRec ar;

  ar.objChange=obj;
  ar.fieldOffset=fieldOffset;
  ar.length=length;
  ar.value=value;
  ar.bRedraw=bRedraw;

  ActChangeObjPointer(a,&ar);
}

void SetObjString(App a,void* obj,int fieldOffset,char* str,int bRedraw) {
  struct _ChangeObjStringRec ar;

  ar.objChange=obj;
  ar.obj=(str==NULL) ? NULL : MallocString(str);
  ar.fieldOffset=fieldOffset;
  ar.bRedraw=bRedraw;

  ActChangeObjString(a,&ar);
}

void ChangeObjGroup(App a,void* obj,int fieldOffset,void* member,int
    status,int bRedraw) {
  struct _ChangeObjGroupRec ar;

  if (!!InGroup(*(Group*)((char*)obj+fieldOffset),member)==!!status) return;

  ar.objChange=obj;
  ar.fieldOffset=fieldOffset;
  ar.member=member;
  ar.status=status;
  ar.bRedraw=bRedraw;

  ActChangeObjGroup(a,&ar);

/* Add vars into varsContaining fields */

  if (GetObjType(obj)==T_VAR) switch(GetObjType(member)) {
    case T_ELEM:
      ChangeObjGroup(a,member,GetOffset(Elem,varsContaining),obj,status,1);
      break;
    case T_CHORD:
      ChangeObjGroup(a,member,GetOffset(Chord,varsContaining),obj,status,1);
      break;
    case T_MESHCELL:
      ChangeObjGroup(a,member,GetOffset(MeshCell,varsContaining),obj,status,1);
      break;
    case T_MESHELEMENT:
      ChangeObjGroup(a,member,GetOffset(MeshElement,varsContaining),obj,
          status,1);
      break;
    default: assert(0);
  }
}

void SetObjGroup(App a,void* obj,int fieldOff,Group group,int bRedraw) {
  void* p;
  Index ix;
  for (p=Group1st(*(Group*)((char*)obj+fieldOff),&ix);p!=NULL;p=Next(&ix)) {
    if (group==NULL || !InGroup(group,p))
      ChangeObjGroup(a,obj,fieldOff,p,0,bRedraw);
  }

  if (group!=NULL) for (p=Group1st(group,&ix);p!=NULL;p=Next(&ix)) {
    if (!InGroup(*(Group*)((char*)obj+fieldOff),p))
      ChangeObjGroup(a,obj,fieldOff,p,1,bRedraw);
  }
}

static Var AddVar(App a,void* object,VarDef vd,VarSet vs) {
  struct _ActRec ar;
  Var v;

  v=Malloc(sizeof(*v));
  v->type=T_VAR;
  v->locks=0;
  v->origin=object;
  v->def=vd;
  v->varSet=vs;
  v->val=NULL;

  ar.obj=v;
  if (ActAddVar(a,&ar)) FatalError("AddVar()-?: fatal error");

  return v;
}

static void ChangeVar(App a,Var v,void* newVal) {
  if (v->val==newVal) return;

  v->def->varType & VTM_HASGROUP ?
    SetObjGroup(a,v,GetOffset(Var,val),newVal,1) :
    SetObjString(a,v,GetOffset(Var,val),newVal,1);

  return;
}

void ChangeVarEx(App a,Var v,void* member,int status) {
  assert(v->def->varType & VTM_HASGROUP);

  ChangeObjGroup(a,v,GetOffset(Var,val),member,status,1);

  return;
}

void DelVar(App a,Var v) {
  struct _DelRec ar;

  if (v->val!=NULL) ChangeVar(a,v,NULL);
  ar.delete=v;
  if (ActDelVar(a,&ar)) FatalError("DelVar()-?: fatal error 1");
}

static Var GetVarPtr(void* object,VarDef vd,VarSet vs) {
  Var v;
  Index ix;

  ValidatePtr(object,"GetVarPtr");
  ValidatePtr(vd,"GetVarPtr_");
  switch(GetObjType(object)) {
    case T_CHORD:
      assert(vs!=NULL);
      for (v=Group1st(((Chord)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd && v->varSet==vs) return v;
      return NULL;
    case T_SOURCE:
      ValidatePtr(vs,"GetVarPtr.SOURCE");
      for (v=Group1st(((Source)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd && v->varSet==vs) return v;
      return NULL;
    case T_SEPARATOR:
      ValidatePtr(vs,"GetVarPtr.SEPARATOR");
      for (v=Group1st(((Separator)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd && v->varSet==vs) return v;
      return NULL;
    case T_ELEM:
      ValidatePtr(vs,"GetVarPtr.ELEM");
      for (v=Group1st(((Elem)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd && v->varSet==vs) return v;
      return NULL;
    case T_VARSET:
      for (v=Group1st(((VarSet)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd) return v;
      return NULL;
    case T_VARSETDEF:
      if (vs!=NULL && (VarSetDef)vs!=object)
        FatalError("GetVar()-varSet: fatal error 1");
      for (v=Group1st(((VarSetDef)object)->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->def==vd) return v;
      return NULL;
    default:
      FatalError("GetVar()-type: fatal error 1",GetObjType(object));
      return NULL;
  }
}

void* GetVar(void* object,VarDef vd,VarSet vs) {
  Var v;
  void* val;

  v=GetVarPtr(object,vd,vs);
  val= v!=NULL? v->val : NULL;

  if (vd->varType & VTM_HASGROUP) {
    if (val==NULL) {
      val=GetEmptyStaticGroup();
    }
  } else {
    if (val==NULL/* || !*(char*)val*/) val=GetObjType(object)!=T_VARSETDEF?
      GetVar(vd->varSetDef,vd,NULL) : "";
  }

  assert(val!=NULL);
  return val;
}

static int GetVarLocks(void* object,VarDef vd,VarSet vs) {
  Var v;

  v=GetVarPtr(object,vd,vs);
  return v==NULL ? 0:IsLocked(v);
}

int SetVar(App a,void* object,VarDef vd,VarSet vs,void* newVal) {
  Var v;
  int i=0;

  ValidatePtr(a,"SetVar");
  ValidatePtr(object,"SetVar_");
  ValidatePtr(vd,"SetVar__");

  if (a->bStrict && !IsEmptyValue(newVal,vd)) {
    i=CheckValue(a,newVal,vd->varType,NULL);
    if (i) return i;
  } else
  if (vd->varType & VTM_HASGROUP) {
    i=CheckGroupForType(newVal,vd->varType,NULL);
    if (i) return i;
  }

  v=GetVarPtr(object,vd,vs);
  if (v!=NULL && v->val==newVal) return 0;
  if (v==NULL || v->origin!=object)
    if (newVal==NULL) return 0; else v=AddVar(a,object,vd,vs);

  newVal==NULL ? DelVar(a,v) : ChangeVar(a,v,newVal);

  return 0;
}

VarSetDef AddVarSetDef(App a,char* name,char* descr,
    int minVarSets,int maxVarSets) {
  struct _ActRec ar;
  VarSetDef vsd;

  ValidatePtr(a,"AddVarSetDef");

  vsd=Malloc(sizeof(*vsd));
  vsd->type=T_VARSETDEF;
  vsd->locks=0;
  vsd->minVarSets=minVarSets;
  vsd->maxVarSets=maxVarSets;
  vsd->name=vsd->descr=vsd->help=NULL;

  ar.obj=vsd;
  if (ActAddVarSetDef(a,&ar))
    FatalError("AddVarSetDef()-?: fatal error");

  SetObjString(a,vsd,GetOffset(VarSetDef,name),name,1);
  SetObjString(a,vsd,GetOffset(VarSetDef,descr),descr,1);

  return vsd;
}

void ChangeVarSetDef(App a,VarSetDef vsd,char* name,char* descr,
    int minVarSets,int maxVarSets,int* pErr) {
  struct _ChangeVarSetDefRec ar;
  int errBuf;
/*  int i;
  VarSet vs;
  Index ix; */

  ValidatePtr(a,"ChangeVarSetDef");
  ValidatePtr(vsd,"ChangeVarSetDef_");

  if (pErr==NULL) pErr=&errBuf;
  *pErr=0;

  ar.vsd=vsd;
  ar.minVarSets=minVarSets;
  ar.maxVarSets=maxVarSets;
  if (ActChangeVarSetDef(a,&ar))
    FatalError("ChangeVarSetDef()-?: fatal error");
  SetObjString(a,vsd,GetOffset(VarSetDef,name),name,1);
  SetObjString(a,vsd,GetOffset(VarSetDef,descr),descr,1);

/*  if (i=GroupCount(vsd->varSets),i<minVarSets || i>maxVarSets)
    FatalError("ChangeVarSetDef()-%s minmax: fatal error 1",vsd->name); */

  return;
}

void* DelVarSetDef(App a,VarSetDef vsd) {
  struct _DelRec ar;
  VarDef vd;
  VarSet vs;
  Index ix;

  ValidatePtr(a,"DelVarSetDef");
  ValidatePtr(vsd,"DelVarSetDef");

  ChangeHelpString(a,vsd,NULL);
  ChangeVarSetDef(a,vsd,NULL,NULL,0,vsd->maxVarSets,NULL);
  for (vs=Group1st(vsd->varSets,&ix);vs!=NULL;vs=Next(&ix)) DelVarSet(a,vs);
  for (vd=Group1st(vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix)) DelVarDef(a,vd);

  ar.delete=vsd;
  ActDelVarSetDef(a,&ar);

  return NULL;
}

VarDef AddVarDef(App a,VarSetDef vsd,char* name,char* descr,int type,
    int flags,int col,int row) {
  struct _ActRec ar;
  VarDef vd;

  ValidatePtr(a,"AddVarDef");
  ValidatePtr(vsd,"AddVarDef_");

  vd=Malloc(sizeof(*vd));
  vd->type=T_VARDEF;
  vd->locks=0;
  vd->name=vd->descr=vd->help=vd->enumValues=NULL;
  vd->varSetDef=vsd;
  vd->varType=type;
  vd->flags=flags;
  vd->col=col;
  vd->row=row;

  ar.obj=vd;
  ActAddVarDef(a,&ar);
  SetObjString(a,vd,GetOffset(VarDef,name),name,1);
  SetObjString(a,vd,GetOffset(VarDef,descr),descr,1);

  return vd;
}

int ChangeVarDef(App a,VarDef vd,VarSetDef vsd,char* name,char* descr,
    int type,int flags,int col,int row) {
  struct _ChangeVarDefRec ar;
  Var v;
  Index ix;
  int err=0;

  ValidatePtr(a,"ChangeVarDef");
  ValidatePtr(vd,"ChangeVarDef_");
  ValidatePtr(vsd,"ChangeVarDef__");

  if ((flags^vd->flags)&VFM_MULTIPLE || (type^vd->varType)&VTM_HASGROUP ||
      (type|vd->varType)&VTF_HAS_MESH_OBJECTS && type!=vd->varType ||
      vsd!=vd->varSetDef) {
    for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix)) DelVar(a,v);
    err=STR_VARSLOST;
  }
  ar.vd=vd;
  ar.vsd=vsd;
  ar.varType=type;
  ar.flags=flags;
  ar.col=col;
  ar.row=row;
  if (ActChangeVarDef(a,&ar)) FatalError("ChangeVarDef()-?: fatal error 1");

  if (name!=vd->name) SetObjString(a,vd,GetOffset(VarDef,name),name,1);
  if (descr!=vd->descr) SetObjString(a,vd,GetOffset(VarDef,descr),descr,1);

  for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix))
    if (!IsEmptyValue(v->val,vd) && CheckValue(a,v->val,vd->varType,NULL)) {
      DelVar(a,v);
      err=STR_VARSLOST;
    }

  return err;
}

void* DelVarDef(App a,VarDef vd) {
  struct _DelRec ar;
  Var v;
  Index ix;

  ValidatePtr(a,"DelVarDef");
  ValidatePtr(vd,"DelVarDef_");

  for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix))
    DelVar(a,v);
  ChangeHelpString(a,vd,NULL);
  ChangeEnumString(a,vd,NULL);
  ChangeVarDef(a,vd,vd->varSetDef,NULL,NULL,vd->varType,vd->flags,
    vd->col,vd->row);

  ar.delete=vd;
  ActDelVarDef(a,&ar);

  return NULL;
}

void ChangeHelpString(App a,void* obj,char* helpString) {
  VarSetDef vsd;
  VarDef vd;

  switch(GetObjType(obj)) {
    case T_VARSETDEF:
      vsd=obj;
      if (vsd->help!=helpString)
        SetObjString(a,vsd,GetOffset(VarSetDef,help),helpString,1);
      break;
    case T_VARDEF:
      vd=obj;
      if (vd->help!=helpString)
        SetObjString(a,vd,GetOffset(VarDef,help),helpString,1);
      break;
    default:
      FatalError("ChangeHelpString()-type %d: fatal error 1",
        GetObjType(obj));
  }
}

void ChangeEnumString(App a,void* obj,char* enumString) {
  VarDef vd;

  switch(GetObjType(obj)) {
    case T_VARDEF:
      vd=obj;
      if (vd->enumValues!=enumString)
        SetObjString(a,vd,GetOffset(VarDef,enumValues),enumString,1);
      break;
    default:
      FatalError("ChangeEnumString()-type %d: fatal error 1",
        GetObjType(obj));
  }
}

VarSet AddVarSet(App a,VarSetDef vsd) {
  struct _ActRec ar;
  VarSet vs;

  if (vsd->maxVarSets<=GroupCount(vsd->varSets))
    FatalError("AddVarSet()-maxAllowed: fatal error 1");

  vs=Malloc(sizeof(*vs));
  vs->type=T_VARSET;
  vs->locks=0;
  vs->def=vsd;

  ar.obj=vs;
  if (ActAddVarSet(a,&ar)) FatalError("AddVarSet()-?: fatal error");

  return vs;
}

void* DelVarSet(App a,VarSet vs) {
  struct _DelRec ar;
  Var v;
  Index ix;

  if (vs->def->minVarSets>=GroupCount(vs->def->varSets))
    FatalError("DelVarSet()-minAllowed: fatal error 1");

  for (v=Group1st(vs->vars,&ix);v!=NULL;v=Next(&ix))
    DelVar(a,v);

  ar.delete=vs;
  if (ActDelVarSet(a,&ar)) FatalError("DelVarSet()-?: fatal error");

  return NULL;
}

VarSetDef FindVarSetDef(App a,char* name) {
  Index ix;
  VarSetDef vsd;

  for (vsd=AppVarSetDef1st(a,&ix);vsd!=NULL;vsd=Next(&ix))
    if (!strcmp(vsd->name,name)) return vsd;

  return NULL;
}

VarDef FindVarDef(VarSetDef vsd,char* name) {
  VarDef vd;
  Index ix;

  for (vd=Group1st(vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix))
    if (!strcmp(vd->name,name)) return vd;

  return NULL;
}

Group GetVarOriginGroup(App a,void* object,VarDef vd,int bMarkedOnly) {
  void* p;
  Index ix;
  Group g;

  g=CreateGroup();
  switch(GetObjType(object)) {
    case T_VARSETDEF:
      GroupAdd(g,object);
      break;
    case T_VARSET:
      if (!(vd->flags & VFM_MULTIPLE)) {
        GroupAdd(g,object);
        break;
      }
      if (vd->varType & VTM_HASGROUP)
        FatalError("GetVarEx()-group: fatal error 1");
      if (bMarkedOnly) {
        for (p=AppMark1st(a,&ix);p!=NULL;p=Next(&ix))
          switch(GetObjType(p)) {
            case T_ELEM:
              if (vd->flags & VF_FORELEMS) GroupAdd(g,p);
              break;
            case T_SEPARATOR:
              if (vd->flags & VF_FORSEPARATORS) GroupAdd(g,p);
              break;
            case T_SOURCE:
              if (vd->flags & VF_FORSOURCES) GroupAdd(g,p);
              break;
            case T_CHORD:
              if (vd->flags & VF_FORCHORDS) GroupAdd(g,p);
              break;
          }
      } else {
        if (vd->flags & VF_FORELEMS) MergeGroup(g,a->elems);
        if (vd->flags & VF_FORSEPARATORS) MergeGroup(g,a->separators);
        if (vd->flags & VF_FORSOURCES) MergeGroup(g,a->sources);
        if (vd->flags & VF_FORCHORDS) MergeGroup(g,a->chords);
      }
      break;
    default:
      assert(0);
  }
  return g;
}

void* GetVarEx(App a,void* object,VarDef vd) {
  void* val=NULL,*p;
  void* org;
  Group g;
  Index ix;

  g=GetVarOriginGroup(a,object,vd,1);
  if (IsEmptyGroup(g)) {
    g=FreeGroup(g);
    return VV_NOSEL;
  }
  org=Group1st(g,&ix);
  val=GetVar(org,vd,object);
  while ((org=Next(&ix))!=NULL) {
    p=GetVar(org,vd,object);
    if (CompValues(val,p,vd)) {
      g=FreeGroup(g);
      return VV_DIFF;
    }
  }
  g=FreeGroup(g);

  return val;
}

#ifdef __TURBOC__
#pragma argsused
#endif
int GetVarExLocks(App a,void* object,VarDef vd) {
  Var v;
  Index ix;

  switch(GetObjType(object)) {
    case T_VARSETDEF:
      return GetVarLocks(object,vd,NULL);
    case T_VARSET:
      if (!(vd->flags & VFM_MULTIPLE)) return GetVarLocks(object,vd,NULL);
      if (vd->varType & VTM_HASGROUP)
        FatalError("GetVarExLocks()-group: fatal error 1");
      for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->varSet==object && IsLocked(v)) return IsLocked(v);
      return 0;
    default:
      FatalError("GetVarExLocks()-type%d: fatal error 2",GetObjType(object));
      return 0;
  }
}

char* WhyLockedEx(View w,void* object,VarDef vd) {
  Var v;
  Index ix;

  switch(GetObjType(object)) {
    case T_VARSETDEF:
      return WhyLocked(w,GetVarPtr(object,vd,NULL));
    case T_VARSET:
      if (!(vd->flags & VFM_MULTIPLE))
        return WhyLocked(w,GetVarPtr(object,vd,NULL));
      assert(!(vd->varType & VTM_HASGROUP));
      for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix))
        if (v->varSet==object && IsLocked(v)) return WhyLocked(w,v);
      FatalError("WhyLockedEx()-!locks: fatal error 1");
    default:
      FatalError("WhyLockedEx()-type%d: fatal error 2",GetObjType(object));
      return 0;
  }
}

int SetVarEx(App a,void* object,VarDef vd,void* val) {
  void* p;
  Group g;
  Index ix;
  int i=0;

  ValidatePtr(val,"SetValEx___");

  g=GetVarOriginGroup(a,object,vd,1);
  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix)) {
    i=SetVar(a,p,vd,object,val);
    if (i) break;
  }
  FreeGroup(g);

  return i;
}

Var GetVarPtrByType(App a,int type) {
  Index ix,ix1;
  VarDef vd,vdx;
  VarSet vs,vsx;


  for (vdx=NULL,vs=AppVarSet1st(a,&ix);vs!=NULL;vs=Next(&ix))
    for (vd=Group1st(vs->def->varDefs,&ix1);vd!=NULL;vd=Next(&ix1))
      if (vd->varType==type) if (vdx==NULL) vdx=vd,vsx=vs; else
        FatalError("GetVarByType()-multiple: fatal error 1");
  if (vdx==NULL) return NULL;

  return GetVarPtr(vsx,vdx,NULL);
}

int CheckValue(App a,void* value,int varType,void** errObj) {
  int i,r;
  double f;
  void* p;
  Index ix,ixv,ixvd;
  Var v;
  VarDef vdPart;
  static char s1[DG_FNAME_LEN],s2[DG_FNAME_LEN];
  Elem e;
  MeshElement me;
  Group g1,g2,g3,g;
  Index ix1,ix2;

  if (errObj!=NULL) *errObj=NULL;

/*  if (varType & VTF_HASELEMS && value!=NULL)
    for (p=Group1st(value,&ix);p!=NULL;p=Next(&ix))
        if (GetObjType(p)!=T_ELEM) {
      *errObj=p;
      return ERR_WRONGTYPE;
    }
  if (varType & VTF_HASCHORDS && value!=NULL)
    for (p=Group1st(value,&ix);p!=NULL;p=Next(&ix))
        if (GetObjType(p)!=T_CHORD) {
      *errObj=p;
      return ERR_WRONGTYPE;
    } */

  if (varType & VTM_HASGROUP) {
    r=CheckGroupForType(value,varType,errObj);
    if (r) return r;
  }

  switch(varType) {
    case VT_TEXT:
      return 0;
    case VT_INT:
      if (sscanf(value,"%d",&i)!=1) return ERR_INVNUMBERS;
      if (sscanf(value,"%s%s",s1,s2)!=1) return ERR_INVNUMBERS;
      strcpy(value,s1);
      return 0;
    case VT_FLOAT:
      if (sscanf(value,SCANFLT,&f)!=1) return ERR_INVNUMBERS;
      if (sscanf(value,"%s%s",s1,s2)!=1) return ERR_INVNUMBERS;
      strcpy(value,s1);
      return 0;
    case VT_ELEM:
      if (value==NULL || IsEmptyGroup(value)) return ERR_NOELEMS;
      if (GroupCount(value)>1) return ERR_MANYSELELEMS;
      return 0;
    case VT_ELEMS:
      return 0;
    case VT_TARGET1:
    case VT_TARGET2:
      if (value==NULL || IsEmptyGroup(value)) return ERR_NOELEMS;
/*      for (s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix))
        if (!s->closed&&(i=CheckSurfaceTargetIntersection(s->line,value))!=0)
          return i; */
      return OrderTargetElems(value,errObj);
    case VT_STRUCTURE:
      if (value==NULL || IsEmptyGroup(value)) return ERR_NOELEMS;
      for (vdPart=AppVarDef1st(a,&ixvd);vdPart!=NULL;vdPart=Next(&ixvd)) {
        if (~vdPart->varType & VT_STRUCTPART) continue;
        for (v=Group1st(vdPart->vars,&ixv);v!=NULL;v=Next(&ixv)) {
          if (v->val==NULL) continue;
          if ((i=CheckStructurePart(value,v->val,errObj))!=0) return i;
        }
      }
      r=OrderStructureElems(a,value,&g1,&g2,&g3,errObj);
      if (!r) {
        ClearGroup(value);
        MergeGroupOfGroups(value,g1);
        MergeGroupOfGroups(value,g2);
        MergeGroupOfGroups(value,g3);
        FreeStructureInfo(&g1,&g2,&g3);
      }
      return r;
    case VT_STRUCTPART:
      if (value==NULL || IsEmptyGroup(value)) return 0;
      v=GetVarPtrByType(a,VT_STRUCTURE);
      if (CheckValue(a,v->val,VT_STRUCTURE,NULL))
        return ERR_BADSTRUCTURE;
      if ((i=CheckStructurePart(v->val,value,errObj))!=0) return i;
      return 0;
    case VT_MESH_H_ELEMENTS:
    case VT_MESH_V_ELEMENTS:
      if (value==NULL) return 0;
      for (me=Group1st(value,&ix);me!=NULL;me=Next(&ix))
        if (!me->bVertical != !(varType==VT_MESH_V_ELEMENTS)) {
          *errObj=me;
          return ERR_MESH_ELEMENT_DIR;
        }
      return 0;
    case VT_CHORDS:
    case VT_MESH_CELLS:
    case VT_MESH_ELEMENTS:
      return 0;
    default:
      FatalError("FormatValue()-type%d: fatal error 1",varType);
      return 0;
  }
}

int IsEmptyValue(void* value,VarDef vd) {
  static char s[DG_FNAME_LEN];

  if (value==NULL) return 1;

  if (vd->varType & VTM_HASGROUP)
    return IsEmptyGroup(value);
  else
    return sscanf(value,"%s",s)!=1;
}

int CompValues(void* v1,void* v2,VarDef vd) {
  if (vd->varType & VTM_HASGROUP) {
    if (v1==NULL && v2==NULL) return 0;
    if (v1==NULL && IsEmptyGroup(v2)) return 0;
    if (v2==NULL && IsEmptyGroup(v1)) return 0;
    assert(0); /* $$$ Not supported yet */
    return 0;
  } else {
    if (IsEmptyValue(v1,vd) && IsEmptyValue(v2,vd)) return 0;
    if (v1==NULL) return -1;
    if (v2==NULL) return 1;
    return strcmp(v1,v2);
  }
}

char* GetIndexVarValue(VarSet vs) {
  VarDef vd;
  char* s;
  Index ix;

  for (vd=Group1st(vs->def->varDefs,&ix);vd!=NULL;vd=Next(&ix))
    if (vd->flags & VF_LAYERINDEX) break;

  if (vd!=NULL) {
    s=GetVar(vs,vd,vs);
    if (s==NULL || !*s) vd=NULL;
  }

  return vd==NULL? NULL : s;
}

static int CheckGroupForType(Group g,int varType,void** pErrObj) {
  int t;
  void* p;
  Index ix;

  if (pErrObj!=NULL) *pErrObj=NULL;

  if (!(varType & VTM_HASGROUP)) return 0;
  else if (varType & VTF_HASELEMS) t=T_ELEM;
  else if (varType & VTF_HASCHORDS) t=T_CHORD;
  else if (varType & VTF_HAS_MESH_OBJECTS) {
    switch(varType) {
      case VT_MESH_CELLS:t=T_MESHCELL;break;
      case VT_MESH_ELEMENTS:
      case VT_MESH_H_ELEMENTS:
      case VT_MESH_V_ELEMENTS:t=T_MESHELEMENT;break;
      default: assert(0);
    }
  } else assert(0);

  if (g==NULL) return 0;

  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix)) {
    if (GetObjType(p)!=t) {
      if (pErrObj!=NULL) *pErrObj=p;
      return ERR_WRONGTYPE;
    }
  }

  return 0;
}
