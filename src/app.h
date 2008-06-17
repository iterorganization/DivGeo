#ifndef _app_h
#define _app_h

/* App data type & functions
*/

#define UM_NORM 1
#define UM_UNDO 2
#define UM_REDO 3
#define UM_CANCEL 4

#define OUTPUTMODE_SONNET 1
#define OUTPUTMODE_CARRE  2

#define OF_NC_VARS        0x0001
#define OF_NC_STRUCTURE   0x0002
#define OF_NC_TARGETS     0x0004
#define OF_NC_SURFACES    0x0008
#define OF_NC_GPOINTS     0x0010
#define OF_NC_CELLS       0x0020

/* Advanced notification flags */

#define CHF_APPVIEW     0x0001
#define CHF_GEOMETRY    0x0002
#define CHF_FILES       0x0004
#define CHF_VARS        0x0008
#define CHF_VARDEFS     0x0010
#define CHF_MARK        0x0020
#define CHF_APP         0x0040
#define CHF_VARSMENU    0x0080
#define CHF_MESH        0x0100
#define CHF_TOPOLOGY    0x0200
#define CHF_TARGETS     0x0400

struct _App {
  int type;
  Stack undoStack,redoStack;
  int undoMode,bNonDestructiveCancel;
  ActRec prevViewLastRec,prevViewLastUsedRec;
  Group views,highlight,mark;
  View activeAppView;

  int bStrict;
  int errorCode;
  void* errorObject,* errorCausedBy;

  int cancelToolFlag,alt,updateLocks;
  int highlightMode,highlightLocks;
  double minX,minY,maxX,maxY,xyAngle;
  unsigned long showFlags,outputFlags,chFlags;

  Group nodes,elems,separators,sources,chords;
  Group varSetDefs,varSets,varDefs,
      xpointTests,xPointSegs,gridPointSegs,gridPointsEx,surfaceZones,
      surfacesEx;

  Equil equil;
#ifndef __cplusplus
  Template template;
#else
  Template templ;
#endif
  SonnetData sonnetData;
  Mesh mesh;

  char* fName;
  char* creationTime;
  char* topologyName;
  int maxElemId,outputMode;
  int meshSlidingMode;
  double meshSlidingThreshold;
  int bDoubleMeshBorder;
};

#define AppLocked(app) (!!(app)->errorCode)

#define AppNode1st(app,ix) Group1st((app)->nodes,(ix))
#define AppElem1st(app,ix) Group1st((app)->elems,(ix))
#define AppView1st(app,ix) Group1st((app)->views,(ix))
#define AppHighlight1st(app,ix) Group1st((app)->highlight,(ix))
#define AppMark1st(app,ix) Group1st((app)->mark,(ix))
#define AppVarDef1st(app,ix) Group1st((app)->varDefs,(ix))
#define AppVarSet1st(app,ix) Group1st((app)->varSets,(ix))
#define AppVarSetDef1st(app,ix) Group1st((app)->varSetDefs,(ix))
#define AppGridPointEx1st(app,ix) ((GridPointEx)Group1st((app)->gridPointsEx,(ix)))
#define AppSeparator1st(app,ix) Group1st((app)->separators,(ix))
#define AppSource1st(app,ix) Group1st((app)->sources,(ix))
#define AppChord1st(app,ix) Group1st((app)->chords,(ix))
#define AppXPointTest1st(app,ix) Group1st((app)->xpointTests,(ix))
#define AppXPointSeg1st(app,ix) Group1st((app)->xPointSegs,(ix))
#define AppGridPointSeg1st(app,ix) ((GridPointSeg)Group1st((app)->gridPointSegs,(ix)))
#define AppSurfaceZone1st(app,ix) ((SurfaceZone)Group1st((app)->surfaceZones,(ix)))
#define AppSurfaceEx1st(app,ix) ((SurfaceEx)Group1st((app)->surfacesEx,(ix)))

#define IsUndoMark(ar) ((ar)==NULL || (ar)->actProc==(ActProc)ActUndoMark)
#define IsHighlighted(app,obj) InGroup((app)->highlight,(obj))

App CreateApp(void);
void* FreeApp(App a);
App AddApp(XApp xap);
void SetAppFName(App a,char* fName);
void SetAppCreationTime(App a,char* creaTime);
void SetAppOutputMode(App a,int newMode);
void* CreateActRec(size_t size,ActProc ap);
void* FreeActRec(void* ar);
void SetOutputFlags(App a,long outputFlags);
void SetTopologyName(App a,char* name);
char* GetTopologyName(App a);

void AddUndoRec(App a,ActRec rec);
void FreeUndoInfo(App a);
void Undo(App a);
void Redo(App a);
void Cancel(App a);
void CancelNonDestructive(App a);
void UndoMark(App a);
void EnableUndo(App a,int bEnable);

int RaiseChangeAppViewUndoRec(App a,int bTestOnly);
int GetPrevViewInfo(App a,double* pMinX,double* pMinY,double* pMaxX,
    double* pMaxY,double* pXyAngle,int bCheckOnly);
void MarkPrevViewDone(App a);

int IsAppUnsaved(App a);

void SetHighlightMode(App a,int mode);
void DrawAppObject(App a,void* obj,int mode);
void* DelObject(App a,void* obj);
void DrawAppHighlight(App a,int mode);
void AddAppUpdate(App a);
void LockAppUpdate(App a,int incr);
void DisableAppUpdate(App a,int incr);

void SetMeshSlidingMode(App a,int mode);
void SetMeshSlidingThreshold(App a,double value);
void SetDoubleMeshBorderFlag(App a,int flag);


void NotifyAppViews(App a,unsigned type,void* object);

#endif
