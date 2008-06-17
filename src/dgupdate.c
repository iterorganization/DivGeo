#include "dg.h"

/* Update routines - name translation & description update
*/

/* Update a VarDef structure:
   vd==NULL vdC!=NULL  - create a VarDef with attributes from vdC
   vd!=NULL vdC==NULL  - delete vd
   vd!=NULL vdC!=NULL  - override parameters of vd with those of vdC
   Return: 0 = success; STR_VARSLOST = vars lost
*/
int UpdateVarDef(App a,VarDef vd,VarDef vdC) {
  VarSetDef vsd;
  char* s;
  int r=0;

  if (vdC==NULL) {
    ValidatePtr(vd,"UpdateVar.vd&vdc");
    while (IsLocked(vd)) DelObject(a,GetLockingObject(a,vd));
    DelVarDef(a,vd);
    return 0;
  }
  if (vd==NULL) {
    vsd=FindVarSetDef(a,vdC->varSetDef->name);
    ValidatePtr(vsd,"UpdateVar+.vsd");
    vd=AddVarDef(a,vsd,vdC->name,vdC->descr,vdC->varType,vdC->flags,
      vdC->col,vdC->row);
  } else {
    if (CompStrings(vd->name,vdC->name) ||
        CompStrings(vd->descr,vdC->descr) ||
        vd->varType!=vdC->varType ||
        vd->flags!=vdC->flags ||
        vd->col!=vdC->col ||
        vd->row!=vdC->row) {
      while (IsLocked(vd)) DelObject(a,GetLockingObject(a,vd));
      r=ChangeVarDef(a,vd,vd->varSetDef,vd->name,vdC->descr,
          vdC->varType,vdC->flags,vdC->col,vdC->row);
    }
  }

  if (vd!=NULL && !r) {
    ChangeHelpString(a,vd,vdC->help);
    ChangeEnumString(a,vd,vdC->enumValues);
    if (!(vd->varType & VTM_HASGROUP)) {
      SetVar(a,vd->varSetDef,vd,NULL,GetVar(vdC->varSetDef,vdC,NULL));
    }
  }

  return r;
}

/* Update a VarSetDef structure:
   vsd==NULL vsdC!=NULL  - create a VarSetDef with attributes from vdC
   vsd!=NULL vsdC==NULL  - delete vsd
   vsd!=NULL vsdC!=NULL  - override parameters of vsd with those of vsdC
   Return: 0 = success; STR_VARSLOST = vars lost
*/
int UpdateVarSetDef(App a,VarSetDef vsd,VarSetDef vsdC) {
  int i;

  if (vsdC==NULL) {
    ValidatePtr(vsd,"UpdateVarSetDef.vsd&vsdC");
    while (IsLocked(vsd)) DelObject(a,GetLockingObject(a,vsd));
    vsd=DelVarSetDef(a,vsd);
  } else
  if (vsd==NULL) {
    vsd=AddVarSetDef(a,vsdC->name,vsdC->descr,vsdC->minVarSets,
      vsdC->maxVarSets);
  }
  else {
    if (CompStrings(vsd->descr,vsdC->descr) ||
        vsd->minVarSets!=vsdC->minVarSets ||
        vsd->maxVarSets!=vsdC->maxVarSets) {
      while (IsLocked(vsd)) DelObject(a,GetLockingObject(a,vsd));
      ChangeVarSetDef(a,vsd,vsd->name,vsdC->descr,vsdC->minVarSets,
        vsdC->maxVarSets,NULL);
    }
  }
  if (vsd!=NULL) {
    if ((i=GroupCount(vsd->varSets))<vsd->minVarSets)
      for (;i<vsd->minVarSets;i++) AddVarSet(a,vsd);
    else if (i>vsd->maxVarSets)
      for (;i>vsd->maxVarSets;i--) DelVarSet(a,Group1st(vsd->varSets,NULL));
  }
  return 0;
}

int UpdateLoadedApp(App a,App config) {
  VarSetDef vsd,vsdC;
  VarDef vd,vdC;
  Index ix;
  int err=0,r;

  ValidatePtr(a,"UpdateLoadedApp");
  ValidatePtr(config,"UpdateLoadedApp_");

  for (vsdC=AppVarSetDef1st(config,&ix);vsdC!=NULL;vsdC=Next(&ix)) {
    vsd=FindVarSetDef(a,vsdC->name);
    r=UpdateVarSetDef(a,vsd,vsdC);
    if (r) err=r;
  }
  for (vdC=AppVarDef1st(config,&ix);vdC!=NULL;vdC=Next(&ix)) {
    vsd=FindVarSetDef(a,vdC->varSetDef->name);
    ValidatePtr(vsd,"UpdateVars.222");
    vd=FindVarDef(vsd,vdC->name);
    r=UpdateVarDef(a,vd,vdC);
    if (r) err=r;
  }

  for (vd=AppVarDef1st(a,&ix);vd!=NULL;vd=Next(&ix)) {
    vsdC=FindVarSetDef(config,vd->varSetDef->name);
    if (vsdC==NULL) vdC=NULL; else vdC=FindVarDef(vsdC,vd->name);
    if (vdC==NULL) UpdateVarDef(a,vd,vdC);
  }
  for (vsd=AppVarSetDef1st(a,&ix);vsd!=NULL;vsd=Next(&ix)) {
    vsdC=FindVarSetDef(config,vsd->name);
    if (vsdC==NULL) UpdateVarSetDef(a,vsd,vsdC);
  }
  return err;
}
