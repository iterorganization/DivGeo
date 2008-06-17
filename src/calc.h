#ifndef _calc_h
#define _calc_h

/* ////////////////////////////////////////////////////////////////// */
/* //                                                              // */
/* // Miscellaneous arithmetic functions & data types              // */
/* //                                                              // */
/* ////////////////////////////////////////////////////////////////// */

/* Data types /////////////////////////////////////////////////////// */

struct _XY {
  double x,y;
};

struct _PtrTable {
  int id;void* p;
};

typedef struct _FlagsRec {unsigned long mask;char c;}* FlagsRec;
typedef struct _NameRec {int val;char* s;}* NameRec;

/* Macros /////////////////////////////////////////////////////////// */

#ifndef swap
#define swap(x,y) ((x)+=(y),(y)=(x)-(y),(x)-=(y))
#endif
#ifndef minmax
#define minmax(x,y) ((x)>(y) ? swap((x),(y)) : (x))
#endif
#ifndef min
#define min(x,y) ((x)<(y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x)>(y) ? (x) : (y))
#endif
#ifndef sqr
#define sqr(x) ((x)*(x))
#endif
#ifndef sign
#define sign(a) ((a)<0 ? -1 : ((a)>0 ? 1 : 0))
#endif
#ifndef setmin
#define setmin(x,y) ((x)>(y) ? ((x)=(y)) : (x))
#endif
#ifndef setmax
#define setmax(x,y) ((x)<(y) ? ((x)=(y)) : (x))
#endif
#ifndef inrange
#define inrange(a,b1,b2) ((a)<=max((b1),(b2)) && (a)>=min((b1),(b2)))
#endif
#ifndef inrange_s
#define inrange_s(a,b1,b2) ((a)<max((b1),(b2)) && (a)>min((b1),(b2)))
#endif

/* Functions //////////////////////////////////////////////////////// */

#define GetObjType(object) (*(int*)(object))

#define GetOffset(struc,field) \
    ((char*)&((struc)NULL)->field-(char*)((struc)NULL))

void RestrictGroupToType(Group grp,int type);
int GetGroupType(Group g); /* Returns the type or T_MULTIPLE/T_NONE */

void AddXY(Group g,double x,double y);

/* rotate (*px, *py) t radians about (centerX, centerY) */
void Rotate(double t,double centerX,double centerY,double* px,double* py);

/* Maths //////////////////////////////////////////////////////////// */

void FollowRectCW(int* px,int* py,int cx1,int cy1,int cx2,int cy2);
void FollowRectCCW(int* px,int* py,int cx1,int cy1,int cx2,int cy2);

double Point2PointDist(double x1,double y1,double x2,double y2);
double Point2VectorDist(double x1,double y1,double x2, double y2,
    double x,double y,int* posflag,double* pos);
int VIntersect(double x1,double y1,double x2,double y2,
    double x3,double y3,double x4,double y4,double* ar,double* br);
double CalcLineLength(Group g);
double StretchValue(double val,double a,double b,double x0,double x,
    double pwr);
double SurroundingCircleRadius(double dx1,double dy1,double dx2,double dy2);

/* PolyLines //////////////////////////////////////////////////////// */

int CompPolyLines(Group line1,Group line2);
int CompPolyLinesEx(Group line1,Group line2,double maxError);
int PolyLinesIntersect(Group line1,Group line2,double* pos1,double* pos2);
void CutPolyLine(Group line,double pos,int bTail);
double ProjectPointToPolyLine(Group gxy,double x,double y);
int GetPolyLinePoint(Group gxy,double length,double* x,double* y);
double PointToPolyLineDist(Group gxy,double x,double y);

/* Hit test for objects ///////////////////////////////////////////// */

Node HitNode(App a,double x,double y,double* dist);
Elem HitElem(App a,double x,double y,int* pos,double* dist);
Chord HitChord(App a,double x,double y,int* pos,double* dist);
Separator HitSeparator(App a,double x,double y,double* pDist);
int HitGridPointExPos(App a,double x,double y,int* pzone,double* pvalue);
void* HitViewObject(View w,double x,double y,long flags);
Group CoveredViewObjects(View w,double x1,double y1,double x2,double y2,
    long flags);

int ObjectInRectangle(void* object,double x1,double y1,double x2,double y2);

/* Object size calculation ////////////////////////////////////////// */

void CalcObjExtents(void* obj,double* pMinX,double* pMinY,double* pMaxX,
  double* pMaxY);
void CalcGroupExtents(Group g,double* pMinX,double* pMinY,double* pMaxX,
  double* pMaxY);

/* Statistics /////////////////////////////////////////////////////// */

int CountSurfaces(App a,int area);
int CountGridPoints(App a,int area);

/* Locking ////////////////////////////////////////////////////////// */

void* GetLockingObject(App a,void* object);
int IsLocked(void* object);
int IsCarreLocked(void* object); /* Handles surfaces, grid pts only */

/* String manipulation ////////////////////////////////////////////// */

char* GetShortFName(char* fName);
char* GetFileExt(char* fName);
char* GetFilePath(char* fName);
char* StripPrefix(char* str,char* prefixes);
char* ExpandFilename(char* str);

int DetectFileType(char* fName);


int CompStrings(char* str1,char* str2);
int LockObject(void* obj,int incr);
void* TranslateId(PtrTable t,int id);

char* Flags2Str(unsigned long flags,FlagsRec fr);
void Str2Flags(char* s,unsigned long* flags,FlagsRec fr);
char* Int2Name(int val,NameRec nr);
int Name2Int(char* name,NameRec nr);

char* GetVersionStr(int versionInt);

/* Distribution laws //////////////////////////////////////////////// */

double DistributeLaw(double x,int law,double alpha,double alpha2,int cnt);

#define DGLAW_NORMAL    0       /* !!!!! Warning !!!!!                   */
#define DGLAW_FLIPPED   1       /* As part of Creator ID, the law #      */
#define DGLAW_2ALPHA    2       /* is written to and read from DG files. */
#define DGLAW_SYMMETRIC 3       /* These constants may NOT be removed or */
#define DGLAW_DELTA     4       /* changed without installing additional */
                                /* tests.                                */

/* File types /////////////////////////////////////////////////////// */

#define FT_NOTFOUND 1
#define FT_UNKNOWN 2
#define FT_DG 3
#define FT_DG_OLD 4
#define FT_DG_EQUIL 5
#define FT_OGR_TEMPLATE 6
#define FT_1_EQUIL 7
#define FT_SNN_GRID 8
#define FT_DG_TEMPLATE 9
#define FT_DG_APP 10
#define FT_DG_CONFIG 11
#define FT_DG_TMP 12
#define FT_SONNET 13
#define FT_2COLUMNS 14
#define FT_HPGL 15

#endif
