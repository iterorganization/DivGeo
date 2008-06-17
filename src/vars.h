/* Parameter variables - typedefs and functions
*/

#ifndef _vars_h
#define _vars_h

/* VarDef flags
*/
#define VF_NOEXPORT          0x0001
#define VF_FORELEMS          0x0002
#define VF_FORSEPARATORS     0x0004
#define VF_FORSOURCES        0x0008
#define VF_FORCHORDS         0x0010
#define VF_LAYERINDEX        0x0020

#define VFM_MULTIPLE  (VF_FORELEMS|VF_FORSEPARATORS|VF_FORSOURCES| \
    VF_FORCHORDS)


/* VarDef bits in the type field
*/
#define VTF_HASELEMS         0x0100
#define VTF_LOCKGROUP        0x0200
#define VTF_SINGLE           0x0400
#define VTF_STRUCTPART       0x0800
#define VTF_HASCHORDS        0x1000
#define VTF_HAS_MESH_OBJECTS 0x2000

#define VTM_HASGROUP         (VTF_HASELEMS|VTF_HASCHORDS|VTF_HAS_MESH_OBJECTS)

/* Variable types
*/
#define VT_INT               0x00
#define VT_FLOAT             0x01
#define VT_TEXT              0x02
#define VT_ELEM              (0x04|VTF_HASELEMS|VTF_LOCKGROUP)
#define VT_ELEMS             (0x05|VTF_HASELEMS|VTF_LOCKGROUP)
#define VT_TARGET1           (0x06|VTF_HASELEMS|VTF_LOCKGROUP|VTF_SINGLE)
#define VT_TARGET2           (0x07|VTF_HASELEMS|VTF_LOCKGROUP|VTF_SINGLE)
#define VT_STRUCTURE         (0x08|VTF_HASELEMS|VTF_LOCKGROUP|VTF_SINGLE)
#define VT_STRUCTPART        (0x09|VTF_HASELEMS|VTF_LOCKGROUP|VTF_STRUCTPART)
#define VT_CHORDS            (0x0A|VTF_HASCHORDS)
#define VT_MESH_CELLS        (0x0B|VTF_HAS_MESH_OBJECTS)
#define VT_MESH_ELEMENTS     (0x0C|VTF_HAS_MESH_OBJECTS)
#define VT_MESH_H_ELEMENTS   (0x0D|VTF_HAS_MESH_OBJECTS)
#define VT_MESH_V_ELEMENTS   (0x0E|VTF_HAS_MESH_OBJECTS)

/* Variable values
*/
#define VV_UNDEF             NULL
#define VV_DIFF              ((void*)-1L)
#define VV_NOSEL             ((void*)-2L)

struct _VarSetDef {
  int type,locks;
  int minVarSets,maxVarSets;
  char* name;
  char* descr;
  char* help;
  Group varDefs,vars,varSets;
};

struct _VarDef {
  int type,locks;
  VarSetDef varSetDef;
  int varType,flags,row,col;
  char* name;
  char* descr;
  char* help;
  char* enumValues;
  Group vars;
};

struct _VarSet {
  int type,locks;
  VarSetDef def;
  Group vars;
};

struct _Var {
  int type,locks;
  void* origin;
  VarDef def;
  VarSet varSet;
  void* val;
};

/* Functions
*/

void SetObjString(App a,void* obj,int fieldOffset,char* str,int bRedraw);
void SetObjGroup(App a,void* obj,int fieldOff,Group group,int bRedraw);
void SetObjInt(App a,void* obj,int fieldOffset,int length,long value,
    int bRedraw);
void SetObjPointer(App a,void* obj,int fieldOffset,int length,void* value,
    int bRedraw);
void SetObjDouble(App a,void* obj,int fieldOffset,double value,int bRedraw);
void ChangeObjGroup(App a,void* obj,int fldOffs,void* member,int status,
    int bRedraw);

VarSetDef AddVarSetDef(App a,char* name,char* descr,
  int minVarSets,int maxVarSets);
void ChangeVarSetDef(App a,VarSetDef vsd,char* name,char* descr,
  int minVarSets,int maxVarSets,int* pErr);
void* DelVarSetDef(App a,VarSetDef vsd);

VarDef AddVarDef(App a,VarSetDef vsd,char* name,char* descr,int type,
  int flags,int col,int row);
int ChangeVarDef(App a,VarDef vd,VarSetDef vsd,char* name,char* descr,
  int type,int flags,int col,int row);
void* DelVarDef(App a,VarDef vd);
void ChangeHelpString(App a,void* obj,char* helpString);
void ChangeEnumString(App a,void* obj,char* enumString);

VarSet AddVarSet(App a,VarSetDef vsd);
void* DelVarSet(App a,VarSet vs);

/*int FormatValue(App a,void* value,VarDef vd,void** errObj); */
int CheckValue(App a,void* value,int varType,void** errObj);
int IsEmptyValue(void* value,VarDef vd);
int CompValues(void* v1,void* v2,VarDef vd);

void* GetVar(void* object,VarDef vd,VarSet vs);
int SetVar(App a,void* object,VarDef vd,VarSet vs,void* newVal);
void ChangeVarEx(App a,Var v,void* member,int status);
void DelVar(App a,Var v);

Group GetVarOriginGroup(App a,void* object,VarDef vd,int bMarkedOnly);
void* GetVarEx(App a,void* object,VarDef vd);
int GetVarExLocks(App a,void* object,VarDef vd);
int SetVarEx(App a,void* object,VarDef vd,void* newVal);
char* WhyLockedEx(View w,void* object,VarDef vd);
Var GetVarPtrByType(App a,int type);

VarSetDef FindVarSetDef(App a,char* name);
VarDef FindVarDef(VarSetDef vsd,char* name);
char* GetIndexVarValue(VarSet vs);

#endif
