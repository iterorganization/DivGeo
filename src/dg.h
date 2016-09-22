/* DivGeo core definitions
*/

#ifndef _dg_h
#define _dg_h

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <pthread.h>

#define MAXDOUBLE DBL_MAX
#define MINDOUBLE DBL_MIN
#define MAXINT INT_MAX
#define DG_FNAME_LEN 256
#define DG_VERSION 21100

#ifdef PC_VERSION

#include <mem.h>
#include <alloc.h>
#include <graphics.h>
#include <dos.h>
#define TL_KEYPRESS 1000

#define double float
#define SCANFLT "%f"

#else

#define SCANFLT "%lf"
#define XPoint Xpoint

#endif

#include "ngroups.h"

typedef struct _XY* XY;
typedef struct _PtrTable* PtrTable;
typedef struct _ActRec* ActRec;
typedef struct _App* App;
typedef struct _View* View;
typedef struct _Node* Node;
typedef struct _Elem* Elem;
typedef struct _Equil* Equil;
typedef struct _Template* Template;
typedef struct _VarSetDef* VarSetDef;
typedef struct _VarDef* VarDef;
typedef struct _VarSet* VarSet;
typedef struct _Var* Var;
typedef struct _XApp* XApp;
typedef struct _SonnetData* SonnetData;
typedef struct _Separator* Separator;
typedef struct _Source* Source;
typedef struct _Chord* Chord;
typedef struct _ViewExt* ViewExt;
typedef struct _XAppExt* XAppExt;
typedef struct _XPointTest* XPointTest;
typedef struct _XPointSeg* XPointSeg;
typedef struct _Mesh* Mesh;
typedef struct _MeshCell* MeshCell;
typedef struct _MeshElement* MeshElement;
typedef struct _MeshPoint* MeshPoint;
typedef struct _StringSource* StringSource;
typedef struct _GridPointSeg* GridPointSeg;
typedef struct _GridPointEx* GridPointEx;
typedef struct _SurfaceZone* SurfaceZone;
typedef struct _SurfaceEx* SurfaceEx;

struct _ViewExt;
struct _XAppExt;

typedef void (*ActProc)(App,ActRec);

#include "spline.h"
#include "vars.h"
#include "view.h"
#include "draw.h"
#include "actions.h"
#include "app.h"
#include "elmnodes.h"
#include "tools.h"
#include "calc.h"
#include "equil.h"
#include "xpoint.h"
#include "xapp.h"
#include "dgfile.h"
#include "msg.h"
#include "sonnet.h"
#include "dgupdate.h"
#include "dgoutput.h"
#include "sources.h"
#include "chords.h"
#include "mesh.h"
#include "nequil.h"

#endif

