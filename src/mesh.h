#ifndef _mesh_h
#define _mesh_h

/* New Mesh object */

struct _Mesh {
  int type,locks;
  App app;
  int alt,sizeX,sizeY,bComplete;
  int cellCount,pointCount,elementCount;
  int fileFormat;
  long fingerprint;
  char* fileName;
  char* headerString;
  MeshCell cells;
  MeshPoint points;
  MeshElement elements;
};

struct _MeshCell {
  int type,locks;
  Mesh mesh;
  int eN,nx,ny,breakFlags;
  MeshPoint points[4];
  MeshCell neighbors[4];
  MeshElement elements[4];
  int cutFlags[4];
  double centerX,centerY,ratio,
         backupCenterX,backupCenterY,backupRatio;
  Group varsContaining;
};

struct _MeshElement {
  int type,locks;
  Mesh mesh;
  MeshPoint points[2];
  MeshCell cells[2];
  int cutFlag,bVertical;
  Group varsContaining;
};

struct _MeshPoint {
  int type,locks;
  Mesh mesh;
  /* Group cells,elements; */
  MeshCell cells[4];
  MeshElement elements[4];
  double x,y,backupX,backupY;
  MeshPoint neighbors[4];
  int nx,ny,cutFlag;
};

/* Tool */

void TlMoveMeshPoint(View w,int event,double x,double y);

/* Mid-level actions */

void* DelMesh(Mesh m);
void SetMeshFileName(Mesh m,char* fName);
void SetMeshAlt(Mesh m,int alt);
void SetMeshFingerprint(Mesh m,long fingerprint);
void SetMeshHeaderString(Mesh m,char* hs);
int CheckMeshHeaderString(Mesh m,char* hs);

int SetMeshCellCenter(MeshCell mc,double cX,double cY);
int SetMeshCellFieldRatio(MeshCell mc,double ratio);
int ChangeMeshPoint(MeshPoint mpt,double x,double y);
int SetMeshPointBackupCoords(MeshPoint mpt,double x,double y);

/* High-level actions */

int RecalculateMeshCenters(Mesh m);
int RecalculateMeshCellCenter(MeshCell mc);

/* Utilities */

int MeshPointsConnected(MeshPoint mpt1,MeshPoint mpt2);
int PointInsideMeshCell(double x,double y,MeshCell mc,int bUseBackups);
int IsIrregularMeshCell(MeshCell mc);

long CalcMeshFingerprint(Mesh m);
void GetMeshElementId(MeshElement me,int* pid1,int* pid2);

MeshCell FindMeshCellByNumber(Mesh m,int eN);
MeshElement FindMeshElementById(Mesh m,int id1,int id2);

int MeshPointChanged(MeshPoint mpt);
int MeshCellChanged(MeshCell mc);

/* File I/O */

int LoadMesh(App a,char* fileName);
int SaveMesh(Mesh m,char* fileName);

/* These values sre stored numerically in files and should NOT be changed */

#define MCN_TOP    0
#define MCN_BOTTOM 1
#define MCN_LEFT   2
#define MCN_RIGHT  3

#define MCN_START  0
#define MCN_END    4
#define MCNX_OPPOSITE 1
#define MCN_NONE   16

/* Sliding modes for App::MeshSlidingMode */

#define MSMF_USE_SPLINE  0x0001
#define MSMF_USE_SURFACE 0x0002
#define MSMF_USE_POINTS  0x0004

#define MSM_SPLINE  (MSMF_USE_SPLINE|MSMF_USE_POINTS)
#define MSM_SURFACE (MSMF_USE_SURFACE)

/* Private and semi-private stuff */

/* For ChangeMeshPoint optimization in AddUndoRec() */

typedef struct _ChangeMeshPointRec {
  ActProc actProc;
  void* obj;
  MeshPoint mpt;
  double x,y;
}* ChangeMeshPointRec;

int ActChangeMeshPoint(App a,ChangeMeshPointRec ar);

/* For DrawObject() */

void DrawMeshCell(View w,MeshCell mc,int mode);
void DrawMeshElement(View w,MeshElement me,int mode);
void DrawWholeMesh(View w,Mesh m,int mode);

#endif
