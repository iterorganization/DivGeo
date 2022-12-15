/* view common & implementation-specific functions
*/

#ifndef _view_h
#define _view_h

typedef void (*ToolProc)(View,int,double,double);

struct _View {
  int type;

  ViewExt x;
  XApp xapp;
  App app;
  int width,height;
  double centerX,centerY,zoomX,zoomY,minX,minY,maxX,maxY;
  double xyAngle;
  int xScaleSign,yScaleSign;
  unsigned long showFlags;
  void* toolData;
  ToolProc activeTool;
  int updateFlag;
  Group labels,shapes;
  double stretchPower;
  int bEditTopology; /* Affects visual appearance & HitObject() */

  double hrMinX,hrMinY,hrMaxX,hrMaxY;  /* Highlight rectangle */
  int bHrShown;

  SurfaceEx lastExaminedSurfaceEx;
  GridPointEx lastExaminedGridPointEx;

/* Implementation-independent cfg variables */
  int nodeR,equilLen,minZoomX,minZoomY,normalLen,gridPointLen,srcR,
    arrowWidth,arrowLength,gridCellMax,labelOffsetX,labelOffsetY,shpIncr,
    meshCellCenterLen,meshPointRadius,gridPointSegEndLength;

/* Virtual functions */

  void (*UnconfigureProc)(View w);
  void (*AdjustAspectRatioProc)(View w);
  void (*SetModeProc)(View w,int mode);
  void (*DrawLineProc)(View w,double x1,double y1,double x2,double y2);
  void (*DrawRectProc)(View w,double x1,double y1,double x2,double y2);
  void (*DrawCircleProc)(View w,double x,double y,double r);
  void (*DrawTextProc)(View w,double x,double y,char* text);
  void (*SetMsgProc)(View w,char* msg);
  void (*ClearProc)(View w);
  void (*FlushProc)(View w);
  void (*NotifyProc)(View w,int msg,void* object);
  char*(*GetStrProc)(View w,int id);
};

#ifndef NO_DUMMIES
struct _ViewExt {
  int dummy;
};
#endif

typedef struct _ViewLabel {
  int type;
  double x,y;
  char* label;
}* ViewLabel;

typedef struct _ViewShape {
  int type,shapeType,bDrawn;
  double x1,y1,x2,y2;
}* ViewShape;

struct _StringSource {
  int type;
  char* (*GetStringProc)(StringSource strS,int id);
  void* userData;
};

#define UPDATE_MAX    1000
#define UPDATE_INCR   100

#define T_ELEM          1
#define T_NODE          2
#define T_EQUIL         3
#define T_TEMPLATE      7
#define T_VARSETDEF     8
#define T_VARDEF        9
#define T_VARSET       10
#define T_VAR          11
#define T_APP          14
#define T_XAPP         15
#define T_VIEW         16
#define T_VIEWLABEL    17
#define T_SONNET       18
#define T_SEPARATOR    19
#define T_VARSDLG      20
#define T_SOURCE       21
#define T_CHORD        22
#define T_XPOINTTEST   23
#define T_XPOINTSEG    24
#define T_VIEWSHAPE    25
#define T_MESH         26
#define T_MESHCELL     27
#define T_MESHPOINT    28
#define T_MESHELEMENT  29
#define T_STRINGSOURCE 30
#define T_GRIDPOINTSEG 31
#define T_GRIDPOINTEX  32
#define T_SURFACEZONE  33
#define T_SURFACEEX    34

#define T_USER        10000
#define T_EMPTYNODE   10001  /* For DwIfExists() */
#define T_MULTIPLE    10002  /* For GetGroupType */
#define T_NONE        10003  /* For GetGroupType */
#define T_MARKING     10004  /* For Edit|Delete marked */
#define T_DEL_VSURFACE 10005 /* For Edit|Delete virtual surfaces */

/* Modes for DrawObject()/Draw_xxx()/DrawAppHighlight
*/
#define DRAW_ON       1
#define DRAW_OFF      2
#define DRAW_ERASE    3
#define DRAWHI_LOCK   4
#define DRAWHI_UNLOCK 5
#define DRAWHI_CNDOFF 6

/* Tool events
*/
#define TL_ENABLE     1
#define TL_DISABLE    2
#define TL_PRESS      3
#define TL_MOTION     4
#define TL_ENTER      5
#define TL_LEAVE      6
#define TL_RELEASE    7
#define TL_CANCEL     8
#define TL_DBLCLK     9

/* ShowFlags
*/
#define SHW_AXES        0x00000001L
#define SHW_GRID        0x00000002L
#define SHW_ELEMS       0x00000004L
#define SHW_NORMALS     0x00000008L
#define SHW_NUMBERS     0x00000010L
#define SHW_NODES       0x00000020L
#define SHW_IRRNODES    0x00000040L
#define SHW_TEMPLATE    0x00000080L
#define SHW_EQUIL       0x00000100L
#define SHW_SURFACES    0x00000200L
#define SHW_GRIDPOINTS  0x00000400L
#define SHW_LABELS      0x00000800L
#define SHW_MESH        0x00001000L
#define SHW_SEPARATORS  0x00002000L
#define SHW_SOURCES     0x00004000L
#define SHW_CHORDS      0x00008000L
#define SHW_MENU        0x00010000L
#define SHW_TOOLBAR     0x00020000L
#define SHW_XPOINTTESTS 0x00040000L
#define SHW_MESHDETAILS 0x00080000L
#define SHW_STRETCH     0x00100000L
#define SHW_TOPVIEW     0x00200000L
#define SHW_3DCHORDS    0x00400000L
#define SHW_IRREGCELLS  0x00800000L

#define SHWX_MESHPOINTS   0x80000000L
#define SHWX_MESHELEMENTS 0x40000000L
#define SHWX_MESHCELLS    0x20000000L

#define SHWM_XPOINT (SHW_SURFACES | SHW_GRIDPOINTS)

/* Notification types
*/

#define N_ADD           0x0001
#define N_ADDED         0x0002
#define N_CHANGE        0x0004
#define N_CHANGED       0x0008
#define N_DEL           0x0010
#define N_DELETED       0x0020
#define N_ALT           0x0040
#define N_NEWAPP        0x0080
#define N_MARK          0x0100
#define N_DESTROYVIEW   0x0200
#define N_EXAMINE       0x0400

#define N_RECENTFILES   0x1000
#define N_NEWTOOL       0x2000

#define N_NOW           0x4000
#define N_USER          0x8000

#define NMASK_HASOBJECT (N_ADD|N_ADDED|N_CHANGE|N_CHANGED|N_DEL|N_DELETED)

#define SHAPE_LINE 1

/* Implementation-independent functions - VIEW.C
*/
View CreateAbstractView();
void* FreeView(View w);

int ScreenX(View w,double x);
int ScreenY(View w,double y);
double FScreenX(View w,double x);
double FScreenY(View w,double y);
double RealX(View w,int x);
double RealY(View w,int y);

void ScreenRotate(View w,int sign,double* px,double* py);

void DrawObject(View w,void* obj,int mode);
void SetViewFactor(View w,double cx,double cy,double zx,double zy);
void SetViewRect(View w,double x1,double y1,double x2,double y2);
void SetRotatedViewFactor(View w,double cx,double cy,double zx,double zy,
    double th);
void SetViewAngle(View w,double th);
void SetView(View w,double x1,double y1,double x2,double y2,double th);
void SetViewFlags(View w,unsigned long flags);
void RepaintView(View w);
void SetViewApp(View w,App a);
void CallToolProc(View w,ToolProc tool,int event,int mouseX,int mouseY);
void Highlight(View w,void* obj,int flag);
void UnhighlightAll(View w);
void DrawHighlight(View w,int mode);
void SetActiveTool(View w,ToolProc tool);
void SetActiveView(View w);
void SetExamineMsg(View w,void* obj);
void ShowPicture(View w);
void AddViewLabel(View w,double x,double y,char* label,int bShowIt);
void RemoveAllViewLabels(View w);
void LabelObject(View w,void* obj,char* label,int bShowIt);
void DrawHighlightRect(View w,int mode);

void SetViewEditTopology(View w,int bEnable);

ViewShape AddViewShapeLine(View w,double x1,double y1,double x2,double y2);
void* DelViewShape(View w,ViewShape vs);
void ClearViewShapes(View w);
void DrawViewShape(View w,ViewShape vs,int mode);
void DrawAllViewShapes(View w,int mode);
void AddViewArrow(View w,double x1,double y1,double x2,double y2);
void DrawPolyLine(View w,Group g,double startPos,double endPos);

char* GetStrVaEx(View w,int id,char* fmt,va_list args);

char* GetStrEx(View w,int id,char* fmt,...);
void ViewMsgEx(View w,int id,char* fmt,...);

char* GetObjDescription(View w,void* obj);
char* WhyLocked(View w,void* object);

void SetHighlightRect(View w,double x1,double y1,double x2,double y2);
#define ClearHighlightRect(w) (SetHighlightRect((w),0,0,0,0))

/* Display macros */

#define AdjustViewAspectRatio(w) ((w)->AdjustAspectRatioProc(w))
#define SetViewMode(w,m) ((w)->SetModeProc(w,m))
#define DrawViewLine(w,x1,y1,x2,y2) ((w)->DrawLineProc(w,x1,y1,x2,y2))
#define DrawViewRect(w,x1,y1,x2,y2) ((w)->DrawRectProc(w,x1,y1,x2,y2))
#define DrawViewCircle(w,x,y,r) ((w)->DrawCircleProc(w,x,y,r))
#define DrawViewText(w,x,y,text) ((w)->DrawTextProc(w,x,y,text))
#define SetViewMsg(w,msg) ((w)->SetMsgProc(w,msg))
#define ClearView(w) ((w)->ClearProc(w))
#define FlushView(w) ((w)->FlushProc(w))
#define NotifyView(w,msg,object) ((w)->NotifyProc(w,msg,object))
#define GetStr(w,id) ((w)->GetStrProc(w,id))

/* Notification macros */

#define NotifyAdd(a,obj)     NotifyAppViews((a),N_ADD,(obj))
#define NotifyAdded(a,obj)   NotifyAppViews((a),N_ADDED,(obj))
#define NotifyChange(a,obj)  NotifyAppViews((a),N_CHANGE,(obj))
#define NotifyChanged(a,obj) NotifyAppViews((a),N_CHANGED,(obj))
#define NotifyDel(a,obj)     NotifyAppViews((a),N_DEL,(obj))
#define NotifyDeleted(a,obj) NotifyAppViews((a),N_DELETED,(obj))
#define NotifyAlt(a)         NotifyAppViews((a),N_ALT,NULL)
#define NotifyMarked(a,obj)  NotifyAppViews((a),N_MARK,(obj))
#define NotifyNewApp(w)      NotifyView((w),    N_NEWAPP,NULL)
#define NotifyNewTool(w)     NotifyView((w),    N_NEWTOOL,NULL)
#define NotifyExamine(a,obj) NotifyAppViews((a),N_EXAMINE,(obj))

/* Modes for SetViewMode
*/

#define VM1_NODE             0
#define VM0_NODE             1
#define VMX_NODE             2
#define VM1_ELEM             3
#define VM0_ELEM             4
#define VMX_ELEM             5
#define VM1_EQUIL            6
#define VM0_EQUIL            7
#define VMX_EQUIL            8
#define VM1_SURFACE          9
#define VM0_SURFACE          10
#define VMX_SURFACE          11
#define VM1_HIGHLIGHTRECT    12
#define VM0_HIGHLIGHTRECT    13
#define VMX_HIGHLIGHTRECT    14
#define VM2_EQUIL            15
#define VM1_TEMPLATE         16
#define VM0_TEMPLATE         17
#define VMX_TEMPLATE         18
#define VM1_ELEMNORMAL       19
#define VM0_ELEMNORMAL       20
#define VMX_ELEMNORMAL       21
#define VM1_ELEMMARK         22
#define VM0_ELEMMARK         23
#define VM1_XPOINT           24
#define VM0_XPOINT           25
#define VMX_XPOINT           26
#define VM1_GPOINT           27
#define VM0_GPOINT           28
#define VMX_GPOINT           29
#define VM1_AXES             30
#define VM1_GRID             31
#define VM1_GRIDTEXT         32
#define VM1_ELEMNUMBER       33
#define VM0_ELEMNUMBER       34
#define VM1_VIEWLABEL        35
#define VM0_VIEWLABEL        36
#define VM1_SONNETDATA       37
#define VM0_SONNETDATA       38
#define VM1_SEPARATOR        39
#define VM0_SEPARATOR        40
#define VMX_SEPARATOR        41
#define VM3_EQUIL            42
#define VM1_SOURCE           43
#define VM0_SOURCE           44
#define VMX_SOURCE           45
#define VM1_CHORD            46
#define VM0_CHORD            47
#define VMX_CHORD            48
#define VM1_XPOINTTEST       49
#define VM2_XPOINTTEST       50
#define VM0_XPOINTTEST       51
#define VMX_XPOINTTEST       52
#define VM1_XPOINTSEG        53
#define VM0_XPOINTSEG        54
#define VMX_XPOINTSEG        55
#define VM1_MESHELEMENT      56
#define VM0_MESHELEMENT      57
#define VMX_MESHELEMENT      58
#define VM2_MESHELEMENT      59
#define VM1_MESHCELLCENTER   60
#define VM2_MESHCELLCENTER   61
#define VM0_MESHCELLCENTER   62
#define VMX_MESHCELLCENTER   63
#define VMX2_MESHELEMENT     64
#define VMX12_MESHELEMENT    65

#endif
