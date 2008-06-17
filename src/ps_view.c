#define NO_DUMMIES
#include "dg.h"

struct _ViewExt {
  FILE* f;
  int bDraw;
  double r,g,b,lw;
  double lwNode,lwElem,lwSurface,lwGPoint,lwSeparator,lwSource,lwChord,
    lwMark,lwEquil,lwTemplate,lwSonnet,lwXPoint,lwNormal,lwNumber,
    lwGridText,lwAxes,lwGrid,lwLabel,lwXPointTest,lwXPointTest2,
    lwXPointSeg,lwMeshElement1,
    lwMeshElement2,lwMeshCellCenter1,lwMeshCellCenter2;
  double margin;
};

static void UnconfigurePsView(View w);
static void ClearPsView(View w);
static void SetPsViewMode(View w,int mode);
static void DrawPsViewLine(View w,double x1,double y1,double x2,double y2);
static void DrawPsViewRect(View w,double x1,double y1,double x2,double y2);
static void DrawPsViewCircle(View w,double x,double y,double r);
static void DrawPsViewText(View w,double x,double y,char* text);
static void FlushPsView(View w);
static void AdjustPsViewAspectRatio(View w);
static void SetPsViewMsg(View w,char* msg);
static void NotifyPsView(View w,int type,void* obj);
static char* GetPsViewStr(View w,int id);
static void ApplyViewMode(View w);

View CreatePsView(App a,double width,double height,double margin) {
  View w;

  if (width-2*margin<=0 || height-2*margin<=0) return NULL;

  w=CreateAbstractView();

  w->UnconfigureProc=UnconfigurePsView;
  w->AdjustAspectRatioProc=AdjustPsViewAspectRatio;
  w->SetModeProc=SetPsViewMode;
  w->DrawLineProc=DrawPsViewLine;
  w->DrawRectProc=DrawPsViewRect;
  w->DrawCircleProc=DrawPsViewCircle;
  w->DrawTextProc=DrawPsViewText;
  w->SetMsgProc=SetPsViewMsg;
  w->ClearProc=ClearPsView;
  w->FlushProc=FlushPsView;
  w->NotifyProc=NotifyPsView;
  w->GetStrProc=GetPsViewStr;

  w->x=Malloc(sizeof(*w->x));
  w->x->f=NULL;
  w->x->bDraw=0;

  w->x->lwNode=0;
  w->x->lwElem=0.3;
  w->x->lwSurface=0;
  w->x->lwGPoint=0;
  w->x->lwSeparator=0.3;
  w->x->lwSource=0.3;
  w->x->lwChord=0.3;
  w->x->lwMark=5;
  w->x->lwEquil=0;
  w->x->lwTemplate=0;
  w->x->lwSonnet=0;
  w->x->lwXPoint=1;
  w->x->lwNormal=0;
  w->x->lwNumber=0;
  w->x->lwGridText=0;
  w->x->lwAxes=1;
  w->x->lwGrid=0;
  w->x->lwLabel=0;
  w->x->lwXPointTest=.6;
  w->x->lwXPointTest2=.2;
  w->x->lwXPointSeg=.4;
  w->x->lwMeshElement1=0;
  w->x->lwMeshElement2=.5;
  w->x->lwMeshCellCenter1=0;
  w->x->lwMeshCellCenter2=.5;

  w->width=width-margin*2;
  w->height=height-margin*2;
  w->x->margin=margin;

  w->nodeR=4;
  w->equilLen=10;
  w->normalLen=4;
  w->gridPointLen=10;
  w->srcR=6;
  w->arrowWidth=2;
  w->arrowLength=10;
  w->gridCellMax=72;
  w->labelOffsetX=36;
  w->labelOffsetY=36;
  w->shpIncr=5;
  w->meshCellCenterLen=3;
  w->meshPointRadius=3;

  SetViewApp(w,a);

  return w;
}

int PsViewOutput(View w,char* fName,int fd,char* title) {
  assert(w->UnconfigureProc==UnconfigurePsView);

  if (fName!=NULL) w->x->f=fopen(fName,"w");
  else w->x->f=fdopen(fd,"w");

  if (w->x->f==NULL) return ERR_FWRITE;

  fprintf(w->x->f,
    "%%!PS-Adobe-3.0\n"
    "%%%%BoundingBox: %g %g %g %g\n"    /* relcheck_ignore_line */
    "%%%%Creator: XDivGeo version %s\n"
    "%%%%Pages: %d\n"
    "%%%%EndComments\n",
    w->x->margin+FScreenX(w,w->minX),w->x->margin+FScreenY(w,w->minY),
    w->x->margin+FScreenX(w,w->maxX),w->x->margin+FScreenY(w,w->maxY),
    GetVersionStr(DG_VERSION),
    1
  );

  fprintf(w->x->f,
    "%%%%Page: %d %d\n"
    "%%%%BeginPageSetup\n"
    "1 setlinecap\n"
    "%g %g translate\n"                 /* relcheck_ignore_line */
    "%g %g moveto\n"                    /* relcheck_ignore_line */
    "%g %g lineto\n"                    /* relcheck_ignore_line */
    "%g %g lineto\n"                    /* relcheck_ignore_line */
    "%g %g lineto\n"                    /* relcheck_ignore_line */
    "closepath\n"
/*    "0.8 setgray\n"
    "fill\n"
    "0 setgray\n" */
    "clip\n"
    "newpath\n"

    "%%%%EndPageSetup\n",
    1,1,
    (double)w->x->margin,
    (double)w->x->margin,
    FScreenX(w,w->minX),FScreenY(w,w->minY),
    FScreenX(w,w->maxX),FScreenY(w,w->minY),
    FScreenX(w,w->maxX),FScreenY(w,w->maxY),
    FScreenX(w,w->minX),FScreenY(w,w->maxY)
  );
/*
  fprintf(w->x->f,
    "0 setgray\n"
    "4 setlinewidth\n"
    "%g %g moveto\n"                    -- relcheck_ignore_line --
    "%g %g lineto\n"                    -- relcheck_ignore_line --
    "%g %g lineto\n"                    -- relcheck_ignore_line --
    "%g %g lineto\n"                    -- relcheck_ignore_line --
    "%g %g lineto\n"                    -- relcheck_ignore_line --
    "%g %g moveto\n"                    -- relcheck_ignore_line --
    "stroke\n"
    "1 setgray\n"
    "/Times-Roman findfont\n"
    "4 scalefont\n"
    "setfont\n"
    "(  XDivGeo  ) show\n"
    "stroke\n",
    (float)w->width,(float)0,
    (float)w->width,(float)w->height,
    (float)0,(float)w->height,
    (float)0,(float)0,
    (float)w->width,(float)0,
    (float)0,(float)0
  );
*/
  RepaintView(w);

  fprintf(w->x->f,
    "showpage\n"
  );

  fprintf(w->x->f,
    "%%%%EOF\n"
  );

  fclose(w->x->f);

  w->x->f=NULL;
  w->x->bDraw=0;

  return 0;
}

static void UnconfigurePsView(View w) {

  assert(w->x->f==NULL);
  w->x=Free(w->x);
}

static void ClearPsView(View w) {
  if (!w->x->bDraw) return;

  assert(0);
}

static void DrawPsViewLine(View w,double x1,double y1,double x2,double y2) {
  if (!w->x->bDraw) return;

  if (x1<w->minX && x2>w->minX)
    y1+=(y2-y1)*(w->minX-x1)/(x2-x1),x1=w->minX; else
  if (x2<w->minX && x1>w->minX)
    y2+=(y1-y2)*(w->minX-x2)/(x1-x2),x2=w->minX;
  if (x1>w->maxX && x2<w->maxX)
    y1+=(y2-y1)*(w->maxX-x1)/(x2-x1),x1=w->maxX; else
  if (x2>w->maxX && x1<w->maxX)
    y2+=(y1-y2)*(w->maxX-x2)/(x1-x2),x2=w->maxX;

  if (y1<w->minY && y2>w->minY)
    x1+=(x2-x1)*(w->minY-y1)/(y2-y1),y1=w->minY; else
  if (y2<w->minY && y1>w->minY)
    x2+=(x1-x2)*(w->minY-y2)/(y1-y2),y2=w->minY;
  if (y1>w->maxY && y2<w->maxY)
    x1+=(x2-x1)*(w->maxY-y1)/(y2-y1),y1=w->maxY; else
  if (y2>w->maxY && y1<w->maxY)
    x2+=(x1-x2)*(w->maxY-y2)/(y1-y2),y2=w->maxY;

  if (x1<w->minX && x2<w->minX) return;
  if (y1<w->minY && y2<w->minY) return;
  if (x1>w->maxX && x2>w->maxX) return;
  if (y1>w->maxY && y2>w->maxY) return;

  ApplyViewMode(w);
  fprintf(w->x->f,
    "%g %g moveto\n"                    /* relcheck_ignore_line */
    "%g %g lineto\n"                    /* relcheck_ignore_line */
    "stroke\n"
  ,FScreenX(w,x1),FScreenY(w,y1),FScreenX(w,x2),FScreenY(w,y2));
}

static void DrawPsViewRect(View w,double x1,double y1,double x2,double y2) {

  if (!w->x->bDraw) return;

  DrawViewLine(w,x1,y1,x2,y1);
  DrawViewLine(w,x2,y1,x2,y2);
  DrawViewLine(w,x2,y2,x1,y2);
  DrawViewLine(w,x1,y2,x1,y1);
}

static void DrawPsViewCircle(View w,double x,double y,double r) {
  int wh,h;

  if (!w->x->bDraw) return;

  if (x+r<w->minX) return;
  if (x-r>w->maxX) return;
  if (y+r<w->minY) return;
  if (y-r>w->maxY) return;

  ApplyViewMode(w);
  fprintf(w->x->f,
    "%g %g %g 0 360 arc\n",             /* relcheck_ignore_line */
    FScreenX(w,x),FScreenY(w,y),r*(w->zoomX+w->zoomY)/2
  );
}

static void DrawPsViewText(View w,double x,double y,char* text) {
  if (!w->x->bDraw) return;

  ApplyViewMode(w);
  fprintf(w->x->f,
    "/Times-Roman findfont\n"
    "6 scalefont\n"
    "setfont\n"
    "%g %g moveto\n"                    /* relcheck_ignore_line */
    "(%s) show\n"
    "stroke\n",
    FScreenX(w,x),FScreenY(w,y),text
  );
}

static void FlushPsView(View w) {
}

static void AdjustPsViewAspectRatio(View w) {
  w->zoomX=w->zoomY=min(w->zoomX,w->zoomY);
}

static void SetPsViewMsg(View w,char* msg) {
  assert(0);
}

static void NotifyPsView(View w,int type,void* obj) {
}

static char* GetPsViewStr(View w,int id) {
  assert(0);
}

static void SetPsViewMode(View w,int mode) {
  if (w->x->f==NULL) return;

  switch(mode) {
    case VMX_NODE:
    case VMX_ELEM:
    case VMX_EQUIL:
    case VMX_SURFACE:
    case VMX_TEMPLATE:
    case VMX_ELEMNORMAL:
    case VMX_XPOINT:
    case VMX_GPOINT:
    case VMX_SEPARATOR:
    case VMX_SOURCE:
    case VMX_CHORD:
    case VM0_NODE:
    case VM0_ELEM:
    case VM0_EQUIL:
    case VM0_SURFACE:
    case VM0_TEMPLATE:
    case VM0_ELEMNORMAL:
    case VM0_XPOINT:
    case VM0_ELEMMARK:
    case VM0_ELEMNUMBER:
    case VM0_VIEWLABEL:
    case VM0_SONNETDATA:
    case VM0_GPOINT:
    case VM0_SEPARATOR:
    case VM0_SOURCE:
    case VM0_CHORD:
    case VM0_XPOINTTEST:
    case VMX_XPOINTTEST:
    case VM0_XPOINTSEG:
    case VMX_XPOINTSEG:
    case VM0_MESHELEMENT:
    case VMX_MESHELEMENT:
    case VM0_MESHCELLCENTER:
    case VMX_MESHCELLCENTER:
    case VMX2_MESHELEMENT:
    case VMX12_MESHELEMENT:

    case VMX_HIGHLIGHTRECT: /* $$$-fix */
    case VM0_HIGHLIGHTRECT:
    case VM1_HIGHLIGHTRECT:

      w->x->bDraw=0;
      break;

    case VM1_NODE:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwNode;
      break;
    case VM1_ELEM:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwElem;
      break;
    case VM1_EQUIL:
      w->x->bDraw=1;w->x->r=100;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwEquil;
      break;
    case VM2_EQUIL:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=100;
      w->x->lw=w->x->lwEquil;
      break;
    case VM3_EQUIL:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwEquil;
      break;
    case VM1_SURFACE:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwSurface;
      break;
    case VM1_TEMPLATE:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwTemplate;
      break;
    case VM1_ELEMNORMAL:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwNormal;
      break;
    case VM1_XPOINT:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwXPoint;
      break;
    case VM1_ELEMMARK:
      w->x->bDraw=1;w->x->r=70;w->x->g=70;w->x->b=100;
      w->x->lw=w->x->lwMark;
      break;
    case VM1_AXES:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwAxes;
      break;
    case VM1_GRID:
      w->x->bDraw=1;w->x->r=50;w->x->g=50;w->x->b=50;
      w->x->lw=w->x->lwGrid;
      break;
    case VM1_GRIDTEXT:
      w->x->bDraw=1;w->x->r=0;w->x->g=0;w->x->b=0;
      w->x->lw=w->x->lwGridText;
      break;
    case VM1_ELEMNUMBER:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwNumber;
      break;
    case VM1_VIEWLABEL:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwLabel;
      break;
    case VM1_SONNETDATA:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwSonnet;
      break;
    case VM1_GPOINT:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwGPoint;
      break;
    case VM1_SEPARATOR:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwSeparator;
      break;
    case VM1_SOURCE:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwSource;
      break;
    case VM1_CHORD:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwChord;
      break;
    case VM1_XPOINTTEST:
      w->x->bDraw=1;w->x->r=000;w->x->g=100;w->x->b=000;
      w->x->lw=w->x->lwXPointTest;
      break;
    case VM2_XPOINTTEST:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwXPointTest2;
      break;
    case VM1_XPOINTSEG:
      w->x->bDraw=1;w->x->r=000;w->x->g=100;w->x->b=000;
      w->x->lw=w->x->lwXPointSeg;
      break;
    case VM1_MESHELEMENT:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwMeshElement1;
      break;
    case VM2_MESHELEMENT:
      w->x->bDraw=1;w->x->r=100;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwMeshElement2;
      break;
    case VM1_MESHCELLCENTER:
      w->x->bDraw=1;w->x->r=000;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwMeshCellCenter1;
      break;
    case VM2_MESHCELLCENTER:
      w->x->bDraw=1;w->x->r=100;w->x->g=000;w->x->b=000;
      w->x->lw=w->x->lwMeshCellCenter2;
      break;

    default:
      assert(0);
  }
}

static void ApplyViewMode(View w) {
  assert(w->x->bDraw);

  fprintf(w->x->f,
/*    "0 setgray\n" */
    "%g %g %g setrgbcolor\n"            /* relcheck_ignore_line */
    "%g setlinewidth\n",                /* relcheck_ignore_line */
    w->x->r/100.0,w->x->g/100.0,w->x->b/100.0,
    w->x->lw
  );
}
