/**********************************************************************\
*                                                                      *
*  XmView "main" file                                                  *
*                                                                      *
\**********************************************************************/

/**********************************************************************\
*                                                                      *
*  Private includes/defines/prototypes/data                            *
*                                                                      *
\**********************************************************************/

#include "x_dg.h"

#define XTBGS XmToggleButtonGetState
#define XTBSS XmToggleButtonSetState

XtResource viewExtRes[]={
  XmNtranslations,XmCTranslations,XmRTranslationTable,sizeof(XtTranslations),
    XtOffset(ViewExt,translations),XmRString,"<Motion>:UseTool()",

  XmNbackground,XmCBackground,XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxBackground),XmRString,"RGBi: 0/0/0",
  "highlightColor","HighlightColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxHighlight),XmRString,"RGBi: 1/1/1",
  "errorHighlightColor","ErrorHighlightColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxErrorHighlight),XmRString,"RGBi: 1/1/1",
  "nodeColor","NodeColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxNode),XmRString,"RGBi: .5/.5/.5",
  "elemColor","ElemColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxElem),XmRString,"RGBi: .5/.5/.5",
  "surfaceColor","SurfaceColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxSurface),XmRString,"RGBi: .5/.5/.5",
  "highlightRectColor","HighlightRectColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxHighlightRect),XmRString,"RGBi: 1/1/1",
  "templateColor","TemplateColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxTemplate),XmRString,"RGBi: .3/.3/.3",
  "normalColor","NormalColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxNormal),XmRString,"RGBi: .5/.5/.5",
  "markColor","MarkColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxMark),XmRString,"RGBi: .5/.5/.5",
  "xPointColor","XPointColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxXPoint),XmRString,"RGBi: .7/.7/.7",
  "equilColor1","EquilColor1",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxEquil1),XmRString,"RGBi: .1/.1/.1",
  "equilColor2","EquilColor2",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxEquil2),XmRString,"RGBi: .2/.2/.2",
  "axesColor","AxesColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxAxes),XmRString,"RGBi: .3/.3/.3",
  "gridColor","GridColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxGrid),XmRString,"RGBi: .2/.2/.2",
  "gridTextColor","GridTextColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxGridText),XmRString,"RGBi: .3/.3/.3",
  "elemNumberColor","ElemNumberColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxElemNumber),XmRString,"RGBi: .3/.3/.3",
  "viewLabelColor","ViewLabelColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxViewLabel),XmRString,"RGBi: .9/.9/.9",
  "sonnetDataColor","SonnetDataColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxSonnet),XmRString,"RGBi: .3/.3/.3",
  "gridPointColor","GridPointColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxGridPoint),XmRString,"RGBi: .7/.7/.7",
  "separatorColor","SeparatorColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxSeparator),XmRString,"RGBi: .7/.7/.7",
  "sourceColor","SourceColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxSource),XmRString,"RGBi: .7/.7/.7",
  "chordColor","ChordColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxChord),XmRString,"RGBi: .7/.7/.7",
  "xpoint1Color","XPointColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxXPointTest1),XmRString,"RGBi: .7/.7/.7",
  "xpoint2Color","XPointColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxXPointTest2),XmRString,"RGBi: .7/.7/.7",
  "xpointSegColor","XPointColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxXPointSeg),XmRString,"RGBi: .7/.7/.7",
  "meshElementColor1","MeshElementColor1",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxMeshElement1),XmRString,"RGBi: 0.7/0.7/0.7",
  "meshElementColor2","MeshElementColor2",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxMeshElement2),XmRString,"RGBi: 0.7/0.7/0.7",
  "meshCellCenterColor1","MeshCellCenterColor1",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxMeshCellCenter1),XmRString,"RGBi: 0.7/0.7/0.7",
  "meshCellCenterColor2","MeshCellCenterColor2",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxMeshCellCenter2),XmRString,"RGBi: 0.7/0.7/0.7",

  "lToolColor","LToolColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxTool[0]),XmRString,"RGBi: .7/.7/.7",
  "mToolColor","MToolColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxTool[1]),XmRString,"RGBi: .7/.7/.7",
  "rToolColor","RToolColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxTool[2]),XmRString,"RGBi: .7/.7/.7",
  "iToolColor","IToolColor",XmRPixel,sizeof(Pixel),
    XtOffset(ViewExt,pxTool[3]),XmRString,"RGBi: 0/0/0",

  "highlightWidth","HighlightWidth",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whHighlight),XmRImmediate,(XtPointer)0,
  "nodeWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whNode),XmRImmediate,(XtPointer)1,
  "elemWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whElem),XmRImmediate,(XtPointer)1,
  "surfaceWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whSurface),XmRImmediate,(XtPointer)1,
  "highlightRectWidth","HighlightWidth",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whHighlightRect),XmRImmediate,(XtPointer)0,
  "templateWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whTemplate),XmRImmediate,(XtPointer)1,
  "normalWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whNormal),XmRImmediate,(XtPointer)1,
  "markWidth","MarkWidth",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whMark),XmRImmediate,(XtPointer)5,
  "xPointWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whXPoint),XmRImmediate,(XtPointer)1,
  "equilWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whEquil),XmRImmediate,(XtPointer)1,
  "axesWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whAxes),XmRImmediate,(XtPointer)1,
  "gridWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whGrid),XmRImmediate,(XtPointer)1,
  "gridTextWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whGridText),XmRImmediate,(XtPointer)1,
  "elemNumberWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whElemNumber),XmRImmediate,(XtPointer)1,
  "viewLabelWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whViewLabel),XmRImmediate,(XtPointer)1,
  "sonnetDataWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whSonnet),XmRImmediate,(XtPointer)1,
  "gridPointWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whGridPoint),XmRImmediate,(XtPointer)1,
  "separatorWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whSeparator),XmRImmediate,(XtPointer)1,
  "sourceWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whSource),XmRImmediate,(XtPointer)1,
  "chordWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whChord),XmRImmediate,(XtPointer)1,
  "xpoint1Width","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whXPointTest1),XmRImmediate,(XtPointer)1,
  "xpoint2Width","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whXPointTest2),XmRImmediate,(XtPointer)1,
  "xpointSegWidth","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whXPointSeg),XmRImmediate,(XtPointer)1,
  "meshElementWidth1","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whMeshElement1),XmRImmediate,(XtPointer)1,
  "meshElementWidth2","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whMeshElement2),XmRImmediate,(XtPointer)1,
  "meshCellCenterWidth1","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whMeshCellCenter1),XmRImmediate,(XtPointer)1,
  "meshCellCenterWidth2","Width",XmRDimension,sizeof(Dimension),
    XtOffset(ViewExt,whMeshCellCenter2),XmRImmediate,(XtPointer)1,

  "useSquares","UseSquares",XmRBoolean,sizeof(Boolean),
    XtOffset(ViewExt,bUseSquares),XmRImmediate,(XtPointer)False,

  "updateDelay","UpdateDelay",XmRInt,sizeof(int),
    XtOffset(ViewExt,updateDelay),XmRImmediate,(XtPointer)1000,
  "updateVarsDelay","UpdateDelay",XmRInt,sizeof(int),
    XtOffset(ViewExt,updateVarsDelay),XmRImmediate,(XtPointer)1000,
  "updateInfoDelay","UpdateDelay",XmRInt,sizeof(int),
    XtOffset(ViewExt,updateInfoDelay),XmRImmediate,(XtPointer)1000,
  "updateMouseDelay","UpdateDelay",XmRInt,sizeof(int),
    XtOffset(ViewExt,updateMouseDelay),XmRImmediate,(XtPointer)1000,
  "exposureDelay","ExposureDelay",XmRInt,sizeof(int),
    XtOffset(ViewExt,exposeDelay),XmRImmediate,(XtPointer)300,
};

static XtResource viewCfgRes[]={
  "nodeRadius","Radius",XmRInt,sizeof(int),
    XtOffset(View,nodeR),XmRImmediate,(XtPointer)4,
  "equilLen","EquilLen",XmRInt,sizeof(int),
    XtOffset(View,equilLen),XmRImmediate,(XtPointer)10,
  "minZoomX","MinZoomX",XmRInt,sizeof(int),
    XtOffset(View,minZoomX),XmRImmediate,(XtPointer)20,
  "minZoomY","MinZoomY",XmRInt,sizeof(int),
    XtOffset(View,minZoomY),XmRImmediate,(XtPointer)20,
  "normalLen","NormalLen",XmRInt,sizeof(int),
    XtOffset(View,normalLen),XmRImmediate,(XtPointer)30,
  "gridPointLen","GridPointLen",XmRInt,sizeof(int),
    XtOffset(View,gridPointLen),XmRImmediate,(XtPointer)30,
  "sourceR","SourceR",XmRInt,sizeof(int),
    XtOffset(View,srcR),XmRImmediate,(XtPointer)20,
  "arrowWidth","ArrowWidth",XmRInt,sizeof(int),
    XtOffset(View,arrowWidth),XmRImmediate,(XtPointer)3,
  "arrowLength","ArrowLength",XmRInt,sizeof(int),
    XtOffset(View,arrowLength),XmRImmediate,(XtPointer)30,
  "gridCellMax","GridCellMax",XmRInt,sizeof(int),
    XtOffset(View,gridCellMax),XmRImmediate,(XtPointer)330,
  "labelOffsetX","LabelOffsetX",XmRInt,sizeof(int),
    XtOffset(View,labelOffsetX),XmRImmediate,(XtPointer)30,
  "labelOffsetY","LabelOffsetY",XmRInt,sizeof(int),
    XtOffset(View,labelOffsetY),XmRImmediate,(XtPointer)30,
  "showPictureIncr","ShowPictureIncr",XmRInt,sizeof(int),
    XtOffset(View,shpIncr),XmRImmediate,(XtPointer)5,
  "meshCellCenterLength","MeshCellCenterLength",XmRInt,sizeof(int),
    XtOffset(View,meshCellCenterLen),XmRImmediate,(XtPointer)3,
  "meshPointRadius","MeshPointRadius",XmRInt,sizeof(int),
    XtOffset(View,meshPointRadius),XmRImmediate,(XtPointer)3,
};

static void GetXViewSize(View w);
static void ProcessXViewShowFlags(View w);
static void UpdateXViewShowFlags(View w);
static void UpdateXViewTitle(View w);
static void UpdateXViewVarsMenu(View w);
static void CbUpdateViewTimer(View w,XtIntervalId* xtp);
static void CbExposeView(Widget wg,View w,void* xtp);
static void CbResizeView(Widget wg,View w,void* xtp);
static void CbChangeTool(Widget wg,View w,void* xtp);
static void CbChangeFlags(Widget wg,View w,void* pcbs);
static void CbViewExposeTimer(XtPointer pView,XtIntervalId* xtiid);

static void UnconfigureXmView(View w);
static void ClearXmView(View w);
static void SetXmViewMode(View w,int mode);
static void DrawXmViewLine(View w,double x1,double y1,double x2,double y2);
static void DrawXmViewRect(View w,double x1,double y1,double x2,double y2);
static void DrawXmViewCircle(View w,double x,double y,double r);
static void DrawXmViewText(View w,double x,double y,char* text);
static void FlushXmView(View w);
static void AdjustXmViewAspectRatio(View w);
static void SetXmViewMsg(View w,char* msg);
static void NotifyXmView(View w,int type,void* obj);
static char* GetXmViewStr(View w,int id);

static void CbDestroyViewShell(Widget wg,XtPointer xtpXapp,XtPointer pcbs);

static void AddDw2UndoInfo(Widget wg,XtPointer w,XtPointer value);
static void AddDw2Exists(Widget wg,XtPointer w,XtPointer value);
static void AddDw2NotExists(Widget wg,XtPointer w,XtPointer value);

static void DwUpdateToolBar(Widget wg,View w,int evt,void*obj,void*udt);
static void DwRebuildCarreInfo(Widget wg,View w,int evt,void*obj,void*udt);

/**********************************************************************\
*                                                                      *
*  Creation/Destruction                                                *
*                                                                      *
\**********************************************************************/

View CreateXmView(XApp xap,App app) {
  View w;
  Widget w1,w2,w3;
  Widget wMnRecent,wCmUndo,wCmRedo,wCmRedoAll,
    wCmCrSurface,wCmCrGPoint,wCmCrSepar,
    wCmRmElems,wCmRmSources,wCmRmSurfaces,wCmRmGPoints,
    wCmRmSepars,wCmRmXpoint,wCmRmEquil,
    wCmRmTemp,wCmRmSonnet,
    wBnHelp,

    wSwElems,wSwNormals,wSwNumbers,wSwNodes,wSwIrrnodes,wSwTemp,wSwEquil,
    wSwSurfaces,wSwGPoints,wSwSonnet,wSw;

  int i;
  char toolWidgetName[]="o?:toolX",toolMenuName[]="*?:toolMenuX";
  Atom a;

/**********************************************************************/

  w=CreateAbstractView();

  /* Initialize extensions */

  w->yScaleSign=-1;

  w->UnconfigureProc=UnconfigureXmView;
  w->AdjustAspectRatioProc=AdjustXmViewAspectRatio;
  w->SetModeProc=SetXmViewMode;
  w->DrawLineProc=DrawXmViewLine;
  w->DrawRectProc=DrawXmViewRect;
  w->DrawCircleProc=DrawXmViewCircle;
  w->DrawTextProc=DrawXmViewText;
  w->SetMsgProc=SetXmViewMsg;
  w->ClearProc=ClearXmView;
  w->FlushProc=FlushXmView;
  w->NotifyProc=NotifyXmView;
  w->GetStrProc=GetXmViewStr;

  w->xapp=xap;
  GroupAdd(w->xapp->views,w);

  w->x=Malloc(sizeof(*w->x));
  w->x->changes=0;
  w->x->discardFlags=0;
  for (i=0;i<3;i++) w->x->tools[i]=NULL;
  w->x->dependentWidgets=CreateStack();
  w->x->varsDialogs=CreateGroup();

  w->x->exposeIntervalId=w->x->updateViewIntervalId=(XtIntervalId)NULL;
  w->x->wToolBar=NULL;
  w->x->wBnShowMenu=NULL;
  w->x->varsMenuWidgets=NULL;

  w->xapp->x->xViewShellsCount++;

  /* Create widgets ***************************************************/

  /* Windows */

/*  w->x->wShell=XtCreatePopupShell("viewShell",topLevelShellWidgetClass,
    w->xapp->x->wShell,NULL,0); */

  w->x->wShell=XtAppCreateShell("DivGeo","DivGeo",
    applicationShellWidgetClass,XtDisplay(w->xapp->x->wShell),NULL,0);

  SetValues(w->x->wShell,XmNdeleteResponse,XmDO_NOTHING,
    NULL);

  XtAddCallback(w->x->wShell,XmNdestroyCallback,CbDestroyViewShell,
      (XtPointer)w->xapp);

  w->x->wMain=Cmw(XmCreateMainWindow,w->x->wShell,"main",
    XmNuserData,w,
    NULL);
  XtAddCallback(w->x->wMain,XmNhelpCallback,CbHelp,(XtPointer)w);

  /* Menu system */

  w->x->wMenu=Cmw(XmCreateMenuBar,w->x->wMain,"menu",
    NULL);

  CreateMenuSystem(w->x->wMenu,
    "c:file",
    "+:fileMenu",
    "bA:new",CbFileNew,w,
    "bA:open",CbFileOpen,w,
    "c:recent",
    "+?:recentMenu",&wMnRecent,
    "-:",
    "bA:save",CbFileSave,w,
    "s:separ",
    "bA:output",CbFileOutput,w,
    "s:separ",
    "bA:templ",CbTemplate,w,
    "bA:equil",CbEquil,w,
    "bA:sonnet",CbSonnet,w,
    "s:separ",
    "c:export",
    "+:exportMenu",
      "bA>:exportMesh",CmExportMesh,(XtPointer)w,
	AddDw2Exists,w,(XtPointer)T_MESH,
      "bA>:exportElemsAsTemplate",CmExportElemsAsTemplate,(XtPointer)w,
	AddDw2Exists,w,(XtPointer)T_ELEM,
    "-:",
    "s:separ",
    "bA:print",CbFilePrint,w,
    "s:separ",
    "bA:quit",CbQuit,w,
    "-:",
    "c:edit",
    "+:editMenu",
    "bA>:undo",CbUndo,w,AddDw2UndoInfo,w,(XtPointer)0,
    "bA>:redo",CbRedo,w,AddDw2UndoInfo,w,(XtPointer)1,
    "bA>:redoAll",CbRedoAll,w,AddDw2UndoInfo,w,(XtPointer)1,
    "s:separ",
    "bA:selAll",CbMarkAll,w,
    "bA:unSelAll",CbUnmarkAll,w,
    "s:separ",
    "c:create",
    "+:createMenu",
    "bA@:node",CbCmCreateAt,w,T_NODE,
    "bA@:source",CbCmCreateAt,w,T_SOURCE,
    "bA@:chord",CbCmCreateAt,w,T_CHORD,
    "b>A@:surface",AddDw2Exists,w,(XtPointer)T_EQUIL,
      CbCmCreateAt,w,T_SURFACE,
    "b>A@:gridPoint",AddDw2Exists,w,(XtPointer)T_XPOINT,
      CbCmCreateAt,w,T_GRIDPOINT,
/*    "bA>:separators",CbCmInstallSeparators,w,
      AddDw2Exists,w,(XtPointer)T_SONNET, */
    "-:",
    "c:delete",
    "+:deleteMenu",
    "bA@>:markedObjects",CbCmDelObjects,w,(XtPointer)T_MARKING,
      AddDw2Exists,w,(XtPointer)T_MARKING,
    "s:separator",
    "bA@>:emptyNodes",CbCmDelObjects,w,(XtPointer)T_NODE,
      AddDw2Exists,w,(XtPointer)T_EMPTYNODE,
    "bA@>:elements",CbCmDelObjects,w,(XtPointer)T_ELEM,
      AddDw2Exists,w,(XtPointer)T_ELEM,
    "bA@>:sources",CbCmDelObjects,w,(XtPointer)T_SOURCE,
      AddDw2Exists,w,(XtPointer)T_SOURCE,
    "bA@>:chords",CbCmDelObjects,w,(XtPointer)T_CHORD,
      AddDw2Exists,w,(XtPointer)T_CHORD,
    "bA@>:surfaces",CbCmDelObjects,w,(XtPointer)T_SURFACE,
      AddDw2Exists,w,(XtPointer)T_SURFACE,
    "bA@>:gridPoints",CbCmDelObjects,w,(XtPointer)T_GRIDPOINT,
      AddDw2Exists,w,(XtPointer)T_GRIDPOINT,
    "bA>:separators",CbCmRemoveSeparators,w,
      AddDw2Exists,w,(XtPointer)T_SEPARATOR,
    "s:separ",
    "bA@>:xpoint",CbCmDelObjects,w,(XtPointer)T_XPOINT,
      AddDw2Exists,w,(XtPointer)T_XPOINT,
    "s:separ",
    "bA@>:equil",CbCmDelObjects,w,(XtPointer)T_EQUIL,
      AddDw2Exists,w,(XtPointer)T_EQUIL,
    "bA@>:template",CbCmDelObjects,w,(XtPointer)T_TEMPLATE,
      AddDw2Exists,w,(XtPointer)T_TEMPLATE,
    "bA@>:sonnet",CbCmDelObjects,w,(XtPointer)T_MESH,
      AddDw2Exists,w,(XtPointer)T_MESH,
    "-:",
    "s:separ",
    "bA:rotMove",CbCmRotMove,w,
    "-:",
    "c:view",
    "+:viewMenu",
    "b?_A:refresh",&w->x->wCmRefresh,CbViewRefresh,w,
    "s?_:separ",&w->x->wSepRefresh,
    "bA:zoomIn",CbZoomIn,w,
    "bA:zoomOut",CbZoomOut,w,
    "bA:showPicture",CbShowPicture,w,
    "bA:showSelection",CbShowSelection,w,
    "s:separ",
    "bA>:prevView",CmPrevZoom,(XtPointer)w,AddDw2UndoInfo,w,(XtPointer)2,
    "s:separ",
    "bA:removeLabels",CmRemoveLabels,(XtPointer)w,
    "s:separ",
    "c:display",
    "+:displayMenu",
    "t?T:nodes",&w->x->wShNodes,          CbChangeFlags,w,
    "t?T:irrNodes",&w->x->wShIrrNodes,    CbChangeFlags,w,
    "t?T:elements",&w->x->wShElems,       CbChangeFlags,w,
    "t?T:sources",&w->x->wShSources,      CbChangeFlags,w,
    "t?T:chords",&w->x->wShChords,        CbChangeFlags,w,
    "t?T:separators",&w->x->wShSeparators,CbChangeFlags,w,
    "s:separator",
    "t?T:normals",&w->x->wShNormals,      CbChangeFlags,w,
    "t?T:numbers",&w->x->wShNumbers,      CbChangeFlags,w,
    "s:separator",
    "t?T:surfaces",&w->x->wShSurfaces,    CbChangeFlags,w,
    "t?T:gridPoints",&w->x->wShGridPoints,CbChangeFlags,w,
    "t?T:sonnet",&w->x->wShSonnet,        CbChangeFlags,w,
    "t?T:meshDetails",&w->x->wShMeshDetails,CbChangeFlags,w,
    "s:separator",
    "t?T:equil",&w->x->wShEquil,          CbChangeFlags,w,
    "t?T:template",&w->x->wShTemplate,    CbChangeFlags,w,
    "s:separator",
    "t?T:axes",&w->x->wShAxes,            CbChangeFlags,w,
    "t?T:grid",&w->x->wShGrid,            CbChangeFlags,w,
    "-:",
    "s:separator",
    "bA:meshHeader",CmViewMeshHeader,(XtPointer)w,
    "-:",
    "c:commands",
    "+:commandsMenu",
    "c:convert",
    "+:convertMenu",
    "bA>:appendTempl",CbCmAppendTemplate,w,
	  AddDw2Exists,w,(XtPointer)T_TEMPLATE,
    /*"bA>:templateToChords",CmConvertTemplateToChords,(XtPointer)w,
	  AddDw2Exists,w,(XtPointer)T_TEMPLATE, */
    "bA>:elemsToChords",CmConvertElemsToChords,(XtPointer)w,
	  AddDw2Exists,w,(XtPointer)T_ELEM,
    "bA>:chordsToElems",CmConvertChordsToElems,(XtPointer)w,
	  AddDw2Exists,w,(XtPointer)T_CHORD,
    "-:",
    "c:simplify",
    "+:simplifyMenu",
    "bA>:gluePoints",CbGluePoints,(XtPointer)w,
	  AddDw2Exists,w,(XtPointer)T_ELEM,
    "c>:glueNormals",AddDw2Exists,w,(XtPointer)T_ELEM,
    "+:glueNormalsMenu",
    "bA@:glueAllNormals",CbGroupNormals,(XtPointer)w,(XtPointer)0,
    "bA@:glueMarkedNormals",CbGroupNormals,(XtPointer)w,(XtPointer)1,
    "-:",
    "bA>:glueElements",CbReduceElements,(XtPointer)w,
	  AddDw2Exists,w,(XtPointer)T_ELEM,
    "-:",
    "bA:renumber",CbCmRenumber,w,
    "bA:invalidVars",CbCmShowInvalidVars,w,
    "bA&:rebuildCarre",CmRebuildCarreInfo,(XtPointer)w,
	(XtPointer)AddDWProc,5,
	(XtPointer)w,(XtPointer)(N_ALT|N_NEWAPP),
	NULL,(XtPointer)DwRebuildCarreInfo,NULL,
/*    "s:separ",
    "bA>:findXPoints",CbCmFindXPoints,w,
	  AddDw2Exists,w,(XtPointer)T_EQUIL,
    "bA>:findXPointSegs",CbCmFindXPointSegs,w,
	  AddDw2Exists,w,(XtPointer)T_XPOINTTEST,
    "bA:test",CmTest,(XtPointer)w, */
    "-:",
    "c:vars",
    "+?:varsMenu",&w->x->wMnVars,
    "c?:add",&w->x->wCmAddVarSet,
    "+?:addMenu",&w->x->wMnAddVarSet,
    "-:",
    "c?:delete",&w->x->wCmDelVarSet,
    "+?:deleteMenu",&w->x->wMnDelVarSet,
    "-:",
    "s:separ",
    "-:",
    "c:options",
    "+:optionsMenu",
    "bA:outputModeX",CmSetOutputModeX,(XtPointer)w,
    "bA:meshOptions",CmSetMeshOptions,(XtPointer)w,
    "s:separ",
    "t?T=:menu",&w->x->wSwMenu,CbSwMenu,w,True,
    "t?=T:toolbar",&w->x->wSwToolBar,True,CbSwToolbar,w,
    "t?T=:manualRefresh",&w->x->wSwManualRefresh,CbSwManualRefresh,w,False,
    "s:separ",
/*    "bA:setup",CbProgramSetup,w,*/
    "c:setup",
    "+:setupMenu",
    "bA:varSetDefs",CbEditVarSetDefs,(XtPointer)w,
    "bA:saveConfig",CbSaveConfig,(XtPointer)w,
    "-:",
    "-:",
    "c:window",
    "+:windowMenu",
    "bA:open",CbWindowOpen,w,
    "bA:close",CbWindowClose,w,
    "s:separ",
    "bA:info",CbWindowInfo,w,
    "bA:toolBar",CbWindowToolBar,w,
    "-:",
    "c?:help",&wBnHelp,
    "+:helpMenu",
    "bA:whatsNew",CbHelpMenu,w,
    "s:separ",
/*    "bA:manual",CbHelpMenu,w, */
    "bA:usingHelp",CbHelpMenu,w,
    "bA:mainWindow",CbHelpMenu,w,
    "s:separ",
    "bA:about",CbAbout,w,
    "-:",
    NULL);

  SetValues(w->x->wMenu,XmNmenuHelpWidget,wBnHelp,NULL);
  SetValues(w->x->wMnVars,XmNuserData,w,NULL);
  SetValues(w->x->wMnAddVarSet,XmNuserData,w,NULL);
  SetValues(w->x->wMnDelVarSet,XmNuserData,w,NULL);

  /* Create toolbar */

  w->x->wCommand=Cmw(XmCreateRowColumn,w->x->wMain,"command",
    XmNorientation,XmHORIZONTAL,
    NULL);
  for (i=0;i<3;i++) {
    toolWidgetName[7]=i+'0';
    toolMenuName[11]=i+'0';
    CreateWidgetSystem(w->x->wCommand,
      toolMenuName,&w->x->wToolMenus[i],
      "bA@:zoom",    CbChangeTool,w,TlZoom,
      "bA@:mark",    CbChangeTool,w,TlMark,
      "bA@:examine", CbChangeTool,w,TlExamine,
      "s:separ",
      "bA@:move",    CbChangeTool,w,TlMoveObject,
      "bA@:remove",  CbChangeTool,w,TlRemoveObject,
      "s:separ",
      "bA@:addElem", CbChangeTool,w,TlAddElem,
      "bA@:addSource",CbChangeTool,w,TlAddSource,
      "bA@:addChord",CbChangeTool,w,TlAddChord,
      "bA@:addSurf", CbChangeTool,w,TlAddSurface,
      "bA@:addGP",   CbChangeTool,w,TlAddGridPoint,
      "s:separ",
      "bA@:setXpt",  CbChangeTool,w,TlSetXPoint,
      "s:separ",
      "bA@:moveMeshPoint",CbChangeTool,w,TlMoveMeshPoint,
      "s:separ",
      "bA@:splitElem",CbChangeTool,w,TlSplitElem,
      "bA@:joinElem",CbChangeTool,w,TlJoinElems,
      "bA@:ctPts",   CbChangeTool,w,TlConnectPoints,
      "bA@:chElem",  CbChangeTool,w,TlRepositionElem,
      "bA@:chNormals",CbChangeTool,w,TlMirrorNormals,
      "-:",
      toolWidgetName,&w->x->wTools[i],
      NULL);
  }

  /* Status bar */

  w1=Cmw(XmCreateForm,w->x->wMain,"msgForm",
    XmNresizePolicy,XmRESIZE_GROW,
    NULL);

  w->x->wMsg=Cmw(XmCreateLabel,w1,"msg",
    XmNtopAttachment,XmATTACH_FORM,
    XmNbottomAttachment,XmATTACH_FORM,
    XmNleftAttachment,XmATTACH_FORM,
    XmNrightAttachment,XmATTACH_FORM,
    NULL);

  SetValues(w->x->wMain,
    XmNmenuBar,w->x->wMenu,
    XmNcommandWindow,w->x->wCommand,
    XmNmessageWindow,w1,
    NULL);

  /* DrawingArea */

  w->x->wDraw=Cmw(XmCreateDrawingArea,w->x->wMain,"draw",
    NULL);
  XtAddCallback(w->x->wDraw,XmNexposeCallback,(XtCallbackProc)CbExposeView,w);
  XtAddCallback(w->x->wDraw,XmNresizeCallback,(XtCallbackProc)CbResizeView,w);

  /* Finish initialization/display the window *************************/

  /* Install additional callbacks */

  AddCallbackToTree(w->x->wMain,xmPushButtonWidgetClass,
    XmNarmCallback,CbDisplayHelpLine,(XtPointer)w->x->wMsg);
  AddCallbackToTree(w->x->wMain,xmPushButtonWidgetClass,
    XmNdisarmCallback,(XtCallbackProc)CbClearHelpLine,(XtPointer)w->x->wMsg);
  AddCallbackToTree(w->x->wMain,xmToggleButtonWidgetClass,
    XmNarmCallback,CbDisplayHelpLine,(XtPointer)w->x->wMsg);
  AddCallbackToTree(w->x->wMain,xmToggleButtonWidgetClass,
    XmNdisarmCallback,(XtCallbackProc)CbClearHelpLine,(XtPointer)w->x->wMsg);

  /* Create and display the window */

  XtRealizeWidget(w->x->wShell);
  XtPopup(w->x->wShell,XtGrabNone);     /* relcheck_ignore_line */

  /* Allocate X resources/add WM callbacks */

  a=XmInternAtom(XtDisplay(w->x->wShell),"WM_DELETE_WINDOW",False);
  XmAddWMProtocolCallback(w->x->wShell,a,(XtCallbackProc)CbWindowClose,w);

  w->x->gc=XCreateGC(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw),0L,NULL);

  /* Retrieve user resources */

  XtGetApplicationResources(w->x->wDraw,w,
    viewCfgRes,XtNumber(viewCfgRes),NULL,0);
  XtGetApplicationResources(w->x->wDraw,w->x,
    viewExtRes,XtNumber(viewExtRes),NULL,0);
  GetXViewSize(w); /* $$$ - needed? */

  /* Manually install translations in order to overcome a Motif bug */

  XtOverrideTranslations(w->x->wDraw,w->x->translations);

  /* Start the update timer */

  CbUpdateViewTimer(w,NULL);

  /* Initialize tools */

  w->x->tools[0]=TlExamine;
  w->x->tools[1]=TlMark;
  w->x->tools[2]=TlZoom;
  NotifyNewTool(w);

  /* Initially update menus */

  UpdateXViewShowFlags(w); /* $$$ - bad */
  UpdateXViewTitle(w);

  /* Attach to the document, activate */

  SetViewApp(w,app);
  SetActiveView(w);

  /* Additional dependencies that need the associated App */

  AddDependentWidget(w,w->x->wCommand,N_NOW | N_NEWTOOL,NULL,
      DwUpdateToolBar,NULL);
  AddDependentWidget(w,wMnRecent,N_NOW|N_RECENTFILES,NULL,
      DwNotifyRecentFiles,NULL);

  /* Display welcome message */

  ViewMsgEx(w,MSG_ABOUT,"$(VERSION)%s",GetVersionStr(DG_VERSION));

  return w;
}

static void UnconfigureXmView(View w) {
  DependentWidget dw;
  Widget* pwg;
  Cardinal nwg;
  int i;

  GroupDel(w->xapp->views,w);

  if (w->x->exposeIntervalId!=(XtIntervalId)NULL)
    XtRemoveTimeOut(w->x->exposeIntervalId);

  if (w->x->updateViewIntervalId!=(XtIntervalId)NULL)
    XtRemoveTimeOut(w->x->updateViewIntervalId);

  while ((dw=Group1st(w->x->dependentWidgets,NULL))!=NULL) {
    DelDependentWidget(w,dw->widget);
  }
  assert(IsEmptyGroup(w->x->dependentWidgets));

  w->x->varsDialogs=FreeGroup(w->x->varsDialogs);

  w->x->dependentWidgets=FreeGroup(w->x->dependentWidgets);
  if (w->x->varsMenuWidgets!=NULL) FreeGroup(w->x->varsMenuWidgets);

  XFreeGC(XtDisplay(w->x->wDraw),w->x->gc);

  XtDestroyWidget(w->x->wShell);
  w->x=Free(w->x);
}

static void ClearXmView(View w) {
  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;

  XClearWindow(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw));
}

static void DrawXmViewLine(View w,double x1,double y1,double x2,double y2) {
  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;

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

  XDrawLine(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw),w->x->gc,
    ScreenX(w,x1),ScreenY(w,y1),ScreenX(w,x2),ScreenY(w,y2));
}

static void DrawXmViewRect(View w,double x1,double y1,double x2,double y2) {
  int sx1,sy1,sx2,sy2;

  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;

  sx1=ScreenX(w,x1);
  sy1=ScreenY(w,y1);
  sx2=ScreenX(w,x2);
  sy2=ScreenY(w,y2);

  if (sx1>sx2) swap(sx1,sx2);
  if (sy1>sy2) swap(sy1,sy2);

  XDrawRectangle(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw),w->x->gc,
    sx1,sy1,sx2-sx1,sy2-sy1);
}

static void DrawXmViewCircle(View w,double x,double y,double r) {
  int wh,h;

  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;
  if (x+r<w->minX) return;
  if (x-r>w->maxX) return;
  if (y+r<w->minY) return;
  if (y-r>w->maxY) return;

  if (w->x->bUseSquares) DrawXmViewRect(w,x-r,y-r,x+r,y+r);
  else {
    wh=r*w->zoomX;
    h=r*w->zoomY;
    XDrawArc(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw),w->x->gc,
      ScreenX(w,x)-wh,ScreenY(w,y)-h,
      wh*2-1,h*2-1,
      0*64,360*64);
  }
}

static void DrawXmViewText(View w,double x,double y,char* text) {
  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;

  XDrawString(XtDisplay(w->x->wDraw),XtWindow(w->x->wDraw),w->x->gc,
    ScreenX(w,x),ScreenY(w,y),text,strlen(text));
}

static void FlushXmView(View w) {
  if (w->app !=NULL && w->app->updateLocks) return;

  if (!XtIsRealized(w->x->wDraw)) return;
  XFlush(XtDisplay(w->x->wDraw));
}

static void AdjustXmViewAspectRatio(View w) {
  w->zoomX=w->zoomY=min(w->zoomX,w->zoomY);
}

static void SetXmViewMsg(View w,char* msg) {
  XmString xms;

  xms=XmStringCreateLtoR(msg,XmFONTLIST_DEFAULT_TAG);
  SetValues(w->x->wMsg,
    XmNlabelString,xms,
    XmNuserData,(XtPointer)'M',
    NULL);
  XmStringFree(xms);
}

static void NotifyXmView(View w,int type,void* obj) {
  Group g;
  DependentWidget dw;
  Index ix;
  Var v;
/*  VarsEditDlg dlg; */

  ValidatePtr(w,"NotifyView");

  g=CopyGroup(w->x->dependentWidgets,NULL);
  if (w->app!=NULL || type==N_DESTROYVIEW) {
    for (dw=Group1st(g,&ix);dw!=NULL;dw=Next(&ix)) {
      if (!(dw->eventMask & type)) continue;
      if (type & NMASK_HASOBJECT && dw->object!=NULL && dw->object!=obj)
        continue;
      dw->notifyProc(dw->widget,w,type,obj,dw->userData);
    }
  }
  FreeGroup(g);

  if (obj!=NULL) switch(GetObjType(obj)) {
    case  T_ELEM:
    case  T_NODE:
    case  T_SURFACE:
    case  T_GRIDPOINT:
    case  T_SEPARATOR:
    case  T_SOURCE:
    case  T_CHORD:
    case  T_XPOINT:
      w->x->changes |= CHF_GEOMETRY;
      break;
    case  T_EQUIL:
    case  T_TEMPLATE:
    case  T_SONNET:
      w->x->changes |= CHF_FILES;
      break;
    case  T_VAR:
      w->x->changes |= CHF_VARS;
      if (GetObjType(((Var)obj)->origin)==T_VARSETDEF)
	w->x->changes |= CHF_VARDEFS;
      break;
    case  T_VARSETDEF:
    case  T_VARDEF:
    case  T_VARSET:
      w->x->changes |= CHF_VARDEFS|CHF_VARSMENU;
      break;
    case  T_APP:
    case  T_VIEW:
      w->x->changes |= CHF_APPVIEW;
      break;
    case T_XPOINTTEST:
    case T_XPOINTSEG:
      break;
    case T_MESH:
    case T_MESHCELL:
    case T_MESHELEMENT:
    case T_MESHPOINT:
      w->x->changes |= CHF_MESH;
      break;
    default:
      assert(0);
  }

  switch(type) {
    case N_NEWAPP:
      w->x->changes |= ~0;
      break;
    case N_ALT:
      if (w->x->changes & CHF_APP) {
	UpdateXViewTitle(w);
	UpdateXViewVarsMenu(w);
      }
      if (w->x->changes & CHF_VARSMENU) {
	UpdateXViewVarsMenu(w);
      }
      w->x->changes=0;
      break;
    case N_ADD:
      break;
    case N_ADDED:
      switch(GetObjType(obj)) {
        case T_TEMPLATE:
          break;
        case T_XPOINT:
          break;
        case T_VARSETDEF:
        case T_VARSET:
	  /* UpdateXViewVarsMenu(w); */
          break;
        case T_VARDEF:
          break;
        case T_APP:
          UpdateXViewTitle(w);
	  /* UpdateXViewVarsMenu(w); */
          UpdateXViewShowFlags(w);
          break;
        case T_SEPARATOR:
          break;
        case T_VAR:
	  break;
      }
      break;
    case N_CHANGE:
      switch(GetObjType(obj)) {
        case T_VARSET:
        case T_VARSETDEF:
        case T_VARDEF:
          break;
      }
      break;
    case N_CHANGED:
      switch(GetObjType(obj)) {
        case T_VIEW:
	  UpdateXViewShowFlags(w);
	  UpdateXViewTitle(w);
          break;
        case T_APP:
	  w->x->changes |= CHF_APP;
	  break;
        case T_VARSETDEF:
        case T_VARSET:
	  /* UpdateXViewVarsMenu(w); */
          break;
        case T_VAR:
	  v=(Var)obj;
	  if (v->def->flags & VF_LAYERINDEX) w->x->changes |= CHF_VARSMENU;
          break;
      }
      break;
    case N_DEL:
      switch(GetObjType(obj)) {
        case T_VARSET:
        case T_VARSETDEF:
        case T_VARDEF:
          break;
      }
      break;
    case N_DELETED:
      switch(GetObjType(obj)) {
        case T_TEMPLATE:
          break;
        case T_XPOINT:
          break;
        case T_VARSETDEF:
        case T_VARSET:
	  /* UpdateXViewVarsMenu(w); */
          break;
        case T_SEPARATOR:
          break;
        case T_VAR:
/*        for (dlg=Group1st(w->x->varsDialogs,&ix);dlg!=NULL;dlg=Next(&ix))
	    dlg->updateFlag=UPDATE_MAX; */
          break;
      }
      break;
    case N_MARK:
      w->x->changes |= CHF_MARK;
/*    for (dlg=Group1st(w->x->varsDialogs,&ix);dlg!=NULL;dlg=Next(&ix))
	dlg->updateFlag=UPDATE_MAX; */
      break;
    case N_NEWTOOL:
    case N_RECENTFILES:
    case N_DESTROYVIEW:
      break;
    default:
      FatalError("NotifyView()-unknown event %d : fatal error 1",type);
  }

  if (type==N_NEWAPP) NotifyView(w,N_ALT,NULL);
}

static void SetXmViewMode(View w,int mode) {
  XGCValues v;

  if (w->app !=NULL && w->app->updateLocks) return;
  if (!XtIsRealized(w->x->wDraw)) return;

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
    case VMX_XPOINTTEST:
    case VMX_XPOINTSEG:
    case VMX_MESHELEMENT:
    case VMX_MESHCELLCENTER:
      v.function=GXxor;
      v.foreground=w->x->pxBackground^(w->app->highlightMode ?
        w->x->pxHighlight : w->x->pxErrorHighlight);
      v.line_width=w->x->whHighlight;
      break;
    case VMX12_MESHELEMENT:
      v.function=GXxor;
      v.foreground=w->x->pxBackground^(w->app->highlightMode ?
        w->x->pxHighlight : w->x->pxErrorHighlight)^w->x->pxMeshElement2;
      v.line_width=w->x->whHighlight;
      break;
    case VMX2_MESHELEMENT:
      v.function=GXxor;
      v.foreground=w->x->pxBackground^w->x->pxMeshElement2;
      v.line_width=w->x->whMeshElement2;
      break;

    case VMX_HIGHLIGHTRECT:
      v.function=GXxor;
      v.foreground=w->x->pxBackground^(w->app->highlightMode ?
	w->x->pxHighlightRect : w->x->pxErrorHighlight);
      v.line_width=w->x->whHighlightRect;
      break;

    case VM0_NODE:
      v.function=GXcopy;
      v.line_width=w->x->whNode;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_ELEM:
      v.function=GXcopy;
      v.line_width=w->x->whElem;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_EQUIL:
      v.function=GXcopy;
      v.line_width=w->x->whEquil;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_HIGHLIGHTRECT:
      v.function=GXcopy;
      v.line_width=w->x->whHighlightRect;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_SURFACE:
      v.function=GXcopy;
      v.line_width=w->x->whSurface;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_TEMPLATE:
      v.function=GXcopy;
      v.line_width=w->x->whTemplate;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_ELEMNORMAL:
      v.function=GXcopy;
      v.line_width=w->x->whNormal;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_XPOINT:
      v.function=GXcopy;
      v.line_width=w->x->whXPoint;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_ELEMMARK:
      v.function=GXcopy;
      v.line_width=w->x->whMark;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_ELEMNUMBER:
      v.function=GXcopy;
      v.line_width=w->x->whElemNumber;
      v.foreground=w->x->pxBackground;
     break;
    case VM0_VIEWLABEL:
      v.function=GXcopy;
      v.line_width=w->x->whViewLabel;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_SONNETDATA:
      v.function=GXcopy;
      v.line_width=w->x->whSonnet;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_GPOINT:
      v.function=GXcopy;
      v.line_width=w->x->whGridPoint;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_SEPARATOR:
      v.function=GXcopy;
      v.line_width=w->x->whSeparator;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_SOURCE:
      v.function=GXcopy;
      v.line_width=w->x->whSource;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_CHORD:
      v.function=GXcopy;
      v.line_width=w->x->whChord;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_XPOINTTEST:
      v.function=GXcopy;
      v.line_width=max(w->x->whXPointTest1,w->x->whXPointTest2);
      v.foreground=w->x->pxBackground;
      break;
    case VM0_XPOINTSEG:
      v.function=GXcopy;
      v.line_width=w->x->whXPointSeg;
      v.foreground=w->x->pxBackground;
      break;
    case VM0_MESHELEMENT:
      v.function=GXcopy;
      v.line_width=max(w->x->whMeshElement1,w->x->whMeshElement2);
      v.foreground=w->x->pxBackground;
      break;
    case VM0_MESHCELLCENTER:
      v.function=GXcopy;
      v.line_width=max(w->x->whMeshCellCenter1,w->x->whMeshCellCenter2);
      v.foreground=w->x->pxBackground;
      break;

    case VM1_NODE:
      v.function=GXcopy;
      v.line_width=w->x->whNode;
      v.foreground=w->x->pxNode;
      break;
    case VM1_ELEM:
      v.function=GXcopy;
      v.line_width=w->x->whElem;
      v.foreground=w->x->pxElem;
      break;
    case VM1_EQUIL:
      v.function=GXcopy;
      v.line_width=w->x->whEquil;
      v.foreground=w->x->pxEquil1;
      break;
    case VM2_EQUIL:
      v.function=GXcopy;
      v.line_width=w->x->whEquil;
      v.foreground=w->x->pxEquil2;
      break;
    case VM3_EQUIL:
      v.function=GXcopy;
      v.line_width=w->x->whXPoint;
      v.foreground=w->x->pxXPoint;
      break;
    case VM1_HIGHLIGHTRECT:
      v.function=GXcopy;
      v.line_width=w->x->whHighlightRect;
      v.foreground=w->x->pxHighlightRect;
      break;
    case VM1_SURFACE:
      v.function=GXcopy;
      v.line_width=w->x->whSurface;
      v.foreground=w->x->pxSurface;
      break;
    case VM1_TEMPLATE:
      v.function=GXcopy;
      v.line_width=w->x->whTemplate;
      v.foreground=w->x->pxTemplate;
      break;
    case VM1_ELEMNORMAL:
      v.function=GXcopy;
      v.line_width=w->x->whNormal;
      v.foreground=w->x->pxNormal;
      break;
    case VM1_XPOINT:
      v.function=GXcopy;
      v.line_width=w->x->whXPoint;
      v.foreground=w->x->pxXPoint;
      break;
    case VM1_ELEMMARK:
      v.function=GXcopy;
      v.line_width=w->x->whMark;
      v.foreground=w->x->pxMark;
      break;
    case VM1_AXES:
      v.function=GXcopy;
      v.line_width=w->x->whAxes;
      v.foreground=w->x->pxAxes;
      break;
    case VM1_GRID:
      v.function=GXcopy;
      v.line_width=w->x->whGrid;
      v.foreground=w->x->pxGrid;
      break;
    case VM1_GRIDTEXT:
      v.function=GXcopy;
      v.line_width=w->x->whGridText;
      v.foreground=w->x->pxGridText;
      break;
    case VM1_ELEMNUMBER:
      v.function=GXcopy;
      v.line_width=w->x->whElemNumber;
      v.foreground=w->x->pxElemNumber;
      break;
    case VM1_VIEWLABEL:
      v.function=GXcopy;
      v.line_width=w->x->whViewLabel;
      v.foreground=w->x->pxViewLabel;
      break;
    case VM1_SONNETDATA:
      v.function=GXcopy;
      v.line_width=w->x->whSonnet;
      v.foreground=w->x->pxSonnet;
      break;
    case VM1_GPOINT:
      v.function=GXcopy;
      v.line_width=w->x->whGridPoint;
      v.foreground=w->x->pxGridPoint;
      break;
    case VM1_SEPARATOR:
      v.function=GXcopy;
      v.line_width=w->x->whSeparator;
      v.foreground=w->x->pxSeparator;
      break;
    case VM1_SOURCE:
      v.function=GXcopy;
      v.line_width=w->x->whSource;
      v.foreground=w->x->pxSource;
      break;
    case VM1_CHORD:
      v.function=GXcopy;
      v.line_width=w->x->whChord;
      v.foreground=w->x->pxChord;
      break;
    case VM1_XPOINTTEST:
      v.function=GXcopy;
      v.line_width=w->x->whXPointTest1;
      v.foreground=w->x->pxXPointTest1;
      break;
    case VM2_XPOINTTEST:
      v.function=GXcopy;
      v.line_width=w->x->whXPointTest2;
      v.foreground=w->x->pxXPointTest2;
      break;
    case VM1_XPOINTSEG:
      v.function=GXcopy;
      v.line_width=w->x->whXPointSeg;
      v.foreground=w->x->pxXPointSeg;
      break;
    case VM1_MESHELEMENT:
      v.function=GXcopy;
      v.line_width=w->x->whMeshElement1;
      v.foreground=w->x->pxMeshElement1;
      break;
    case VM2_MESHELEMENT:
      v.function=GXcopy;
      v.line_width=w->x->whMeshElement2;
      v.foreground=w->x->pxMeshElement2;
      break;
    case VM1_MESHCELLCENTER:
      v.function=GXcopy;
      v.line_width=w->x->whMeshCellCenter1;
      v.foreground=w->x->pxMeshCellCenter1;
      break;
    case VM2_MESHCELLCENTER:
      v.function=GXcopy;
      v.line_width=w->x->whMeshCellCenter2;
      v.foreground=w->x->pxMeshCellCenter2;
      break;

    default:
      FatalError("SetXmViewMode()-mode%d: fatal error 1",mode);
  }
  XChangeGC(XtDisplay(w->x->wDraw),w->x->gc,
    GCFunction | GCLineWidth | GCForeground,
    &v);
}

/* Private functions - only for view_x*.c
*/

static void GetXViewSize(View w) {
  Dimension wh,h;

  GetValues(w->x->wDraw,XmNwidth,&wh,XmNheight,&h,NULL);
  w->width=wh? wh : 1;
  w->height=h? h : 1;
}

static void DwUpdateToolBar(Widget wg,View w,int evt,void*obj,void*udt) {
  int i,j,n;
  Cardinal cn;
  WidgetList wl=NULL;
  XtPointer xtp;
  ToolProc tp;

  for (n=0;n<3;n++) {
    GetValues(w->x->wToolMenus[n],
      XmNchildren,&wl,
      XmNnumChildren,&cn,
      NULL);

    for (i=0;i<cn;i++) {
      if (!XmIsPushButton(wl[i])) continue;
      GetValues(wl[i],XmNuserData,&tp,NULL);
      if (tp==w->x->tools[n])
        SetValues(w->x->wTools[n],XmNmenuHistory,wl[i],NULL);
    }
  }
}

static void DwRebuildCarreInfo(Widget wg,View w,int evt,void*obj,void*udt) {
  assert(w->type==T_VIEW);

  SetSensitiveEx(wg,w->app->outputMode==OUTPUTMODE_CARRE &&
      (GroupCount(w->app->surfaces) || GroupCount(w->app->gridPoints)));
}

View GetWidgetXView(Widget wg) {
  void* p=NULL;

  while (p==NULL) {
    GetValues(wg,XmNuserData,&p,NULL);
    if (p!=NULL && GetObjType(p)!=T_VIEW) p=NULL;
    if (XtIsApplicationShell(wg)) break; else wg=XtParent(wg);
  }

  return p;
}

static void UpdateXViewShowFlags(View w) {
  ValidatePtr(w,"UpdateXViewShowFlags");

  XTBSS(w->x->wShAxes,       !!(w->showFlags & SHW_AXES),      False);
  XTBSS(w->x->wShGrid,       !!(w->showFlags & SHW_GRID),      False);
  XTBSS(w->x->wShElems,      !!(w->showFlags & SHW_ELEMS),     False);
  XTBSS(w->x->wShNormals,    !!(w->showFlags & SHW_NORMALS),   False);
  XTBSS(w->x->wShNumbers,    !!(w->showFlags & SHW_NUMBERS),   False);
  XTBSS(w->x->wShNodes,      !!(w->showFlags & SHW_NODES),     False);
  XTBSS(w->x->wShIrrNodes,   !!(w->showFlags & SHW_IRRNODES),  False);
  XTBSS(w->x->wShTemplate,   !!(w->showFlags & SHW_TEMPLATE),  False);
  XTBSS(w->x->wShEquil,      !!(w->showFlags & SHW_EQUIL),     False);
  XTBSS(w->x->wShSurfaces,   !!(w->showFlags & SHW_SURFACES),  False);
  XTBSS(w->x->wShGridPoints, !!(w->showFlags & SHW_GRIDPOINTS),False);
  XTBSS(w->x->wShSonnet,     !!(w->showFlags & SHW_MESH),      False);
  XTBSS(w->x->wShSeparators, !!(w->showFlags & SHW_SEPARATORS),False);
  XTBSS(w->x->wShSources,    !!(w->showFlags & SHW_SOURCES),   False);
  XTBSS(w->x->wShChords,     !!(w->showFlags & SHW_CHORDS),    False);
  XTBSS(w->x->wShMeshDetails,!!(w->showFlags & SHW_MESHDETAILS),False);
}

static void ProcessXViewShowFlags(View w) {
  unsigned long f;

  ValidatePtr(w,"ProcessXViewShowFlags");

  f=w->showFlags;
  XTBGS(w->x->wShAxes)       ? (f |= SHW_AXES)       : (f &=~ SHW_AXES);
  XTBGS(w->x->wShGrid)       ? (f |= SHW_GRID)       : (f &=~ SHW_GRID);
  XTBGS(w->x->wShElems)      ? (f |= SHW_ELEMS)      : (f &=~ SHW_ELEMS);
  XTBGS(w->x->wShNormals)    ? (f |= SHW_NORMALS)    : (f &=~ SHW_NORMALS);
  XTBGS(w->x->wShNumbers)    ? (f |= SHW_NUMBERS)    : (f &=~ SHW_NUMBERS);
  XTBGS(w->x->wShNodes)      ? (f |= SHW_NODES)      : (f &=~ SHW_NODES);
  XTBGS(w->x->wShIrrNodes)   ? (f |= SHW_IRRNODES)   : (f &=~ SHW_IRRNODES);
  XTBGS(w->x->wShTemplate)   ? (f |= SHW_TEMPLATE)   : (f &=~ SHW_TEMPLATE);
  XTBGS(w->x->wShEquil)      ? (f |= SHW_EQUIL)      : (f &=~ SHW_EQUIL);
  XTBGS(w->x->wShSurfaces)   ? (f |= SHW_SURFACES)   : (f &=~ SHW_SURFACES);
  XTBGS(w->x->wShGridPoints) ? (f |= SHW_GRIDPOINTS) : (f &=~ SHW_GRIDPOINTS);
  XTBGS(w->x->wShSonnet)     ? (f |= SHW_MESH)       : (f &=~ SHW_MESH);
  XTBGS(w->x->wShSeparators) ? (f |= SHW_SEPARATORS) : (f &=~ SHW_SEPARATORS);
  XTBGS(w->x->wShSources)    ? (f |= SHW_SOURCES)    : (f &=~ SHW_SOURCES);
  XTBGS(w->x->wShChords)     ? (f |= SHW_CHORDS)     : (f &=~ SHW_CHORDS);
  XTBGS(w->x->wShMeshDetails)? (f |= SHW_MESHDETAILS): (f &=~ SHW_MESHDETAILS);

  SetViewFlags(w,f);
}

static void UpdateXViewTitle(View w) {
  char s[DG_FNAME_LEN*2],s1[DG_FNAME_LEN*2];

  ValidatePtr(w,"UpdateXViewTitle");

  strcpy(s,GetShortFName(w->xapp->argv[0]));
  strcpy(s1,s);
  if (w->app!=NULL && w->app->fName!=NULL) {
    strcat(s," ");
    strcat(s,GetShortFName(w->app->fName));
    strcpy(s1,GetShortFName(w->app->fName));
  }
  SetValues(w->x->wShell,XmNtitle,s,NULL);
  SetValues(w->x->wShell,XmNiconName,s1,NULL);
}

static void AddXViewVarsMenuItem(View w,VarSetDef vsd,XtCallbackProc cbp,
    Widget wParent,char* wgName) {
  int i;
  VarSet vs;
  Index ix;
  Widget wg,wMenu,wCascade;
  char buf[256],* s;

  i=GroupCount(vsd->varSets);
  if (i>1) {
    wMenu=Cw(XmCreatePulldownMenu,wParent,"vsSubMenu",
      XmNuserData,(XtPointer)w,
      NULL);
    GroupAdd(w->x->varsMenuWidgets,(void*)(wMenu));

    wCascade=Cmw(XmCreateCascadeButton,wParent,"vsSubMenuButton",
      XmNsubMenuId,wMenu,
      NULL);
    SetLabelString(wCascade,vsd->descr);

    for (vs=Group1st(vsd->varSets,&ix);vs!=NULL;vs=Next(&ix)) {
      wg=Cmw(XmCreatePushButton,wMenu,wgName,
	NULL);
/*      GroupAdd(w->x->varsMenuWidgets,(void*)wg); -- Will be deleted with RowCol */
      XtAddCallback(wg,XmNactivateCallback,cbp,(XtPointer)vs);
      s=GetIndexVarValue(vs);
      if (s==NULL) {
	sprintf(buf,"#%d",GroupIndex(vsd->varSets,vs)+1);
	s=buf;
      }
      SetLabelString(wg,s);
    }
    GroupAdd(w->x->varsMenuWidgets,(void*)wCascade);
  } else {
    wg=Cmw(XmCreatePushButton,wParent,wgName,
      NULL);
    GroupAdd(w->x->varsMenuWidgets,(void*)wg);
    vs=Group1st(vsd->varSets,NULL);
    XtAddCallback(wg,XmNactivateCallback,cbp,(XtPointer)vs);
    SetLabelString(wg,GetObjDescription(w,vs));
  }
}

static void UpdateXViewVarsMenu(View w) {
/*  VarsEditDlg dlg; */
  VarSetDef vsd;
  VarSet vs;
  Index ix,ix1;
  Widget wg;
  XmString xms;
  int i,j;

  ValidatePtr(w,"UpdateXViewVarsMenu");

  if (w->x->varsMenuWidgets!=NULL) {
    for (wg=Group1st(w->x->varsMenuWidgets,&ix);wg!=NULL;wg=Next(&ix)) {
      if (XmIsCascadeButton(wg))
        SetValues(wg,XmNsubMenuId,NULL,NULL); /* $$$ Avoid crashes on SGI */
      XtDestroyWidget(wg);
    }
    w->x->varsMenuWidgets=FreeGroup(w->x->varsMenuWidgets);
  }
  if (w->app==NULL) return;

  w->x->varsMenuWidgets=CreateGroup();

  for (j=0,vsd=AppVarSetDef1st(w->app,&ix);vsd!=NULL;vsd=Next(&ix)) {
    i=GroupCount(vsd->varSets);
    if (i<vsd->maxVarSets) {
      j++;
      xms=MakeXmString(GetObjDescription(w,vsd));
      wg=Cmw(XmCreatePushButton,w->x->wMnAddVarSet,"addVarSet",
        XmNlabelString,xms,
        NULL);
      XtAddCallback(wg,XmNactivateCallback,
        (XtCallbackProc)CbAddVarSet,vsd);
      GroupAdd(w->x->varsMenuWidgets,(void*)wg);
      XmStringFree(xms);
    }
  }
  XtSetSensitive(w->x->wCmAddVarSet,!!j); /* $$$ Crashes Motif ? */

  for (j=0,vsd=AppVarSetDef1st(w->app,&ix);vsd!=NULL;vsd=Next(&ix)) {
    i=GroupCount(vsd->varSets);
    if (!i) continue;

    AddXViewVarsMenuItem(w,vsd,(XtCallbackProc)CbEditVarSet,w->x->wMnVars,
      "editVarSet");
    if (i>vsd->minVarSets) {
      j++;
      AddXViewVarsMenuItem(w,vsd,(XtCallbackProc)CbDelVarSet,
	w->x->wMnDelVarSet,"delVarSet");
    }
  }

  XtSetSensitive(w->x->wCmDelVarSet,!!j); /* $$$ Crashes Motif? */
}

static void CbDestroyDependentWidget(Widget wg,XtPointer arg,XtPointer pcbs){
  View w=(View)arg;

  DelDependentWidgetEx(w,wg,True);
}

void AddDependentWidget(View w,Widget wg,int eventMask,void* object,
    DwNotifyProc notifyProc,void* userData) {
  DependentWidget dw;

  dw=Malloc(sizeof(*dw));
  dw->widget=wg;
  dw->eventMask=eventMask;
  dw->object=object;
  dw->notifyProc=notifyProc;
  dw->userData=userData;
  XtAddCallback(wg,XmNdestroyCallback,CbDestroyDependentWidget,w);
  GroupAdd(w->x->dependentWidgets,dw);

  if (eventMask & N_NOW) {
    if (w->app!=NULL) notifyProc(wg,w,N_NOW,NULL,userData);
  }
}

void DelDependentWidgetEx(View w,Widget wg,int bWidgetDestroyed) {
  DependentWidget dw;
  Index ix;

  assert(w->type==T_VIEW);

  for (dw=Group1st(w->x->dependentWidgets,&ix);dw!=NULL;dw=Next(&ix)) {
    if (dw->widget==wg) {
      GroupDel(w->x->dependentWidgets,dw);
      dw=Free(dw);
      if (!bWidgetDestroyed) {
	XtRemoveCallback(wg,XmNdestroyCallback,CbDestroyDependentWidget,w);
      }
    }
  }
}

void AddDWProc(Widget wg,XtPointer xtpV,XtPointer mask,XtPointer obj,
    XtPointer xtpProc,XtPointer userData) {
  AddDependentWidget((View)xtpV,wg,(int)mask,(void*)obj,
    (DwNotifyProc)xtpProc,(void*)userData);
}

static char* GetXmViewStr(View w,int id) {
  return XAppStr(w->xapp,id);
}

static void CbUpdateViewTimer(View w,XtIntervalId* xtp) {
  Index ix;

  if (w->updateFlag) {
    w->updateFlag-=UPDATE_INCR;
    if (w->updateFlag<=0) {
      if (!XmToggleButtonGetState(w->x->wSwManualRefresh)) RepaintView(w);
    }
  }
  w->x->updateViewIntervalId=XtAppAddTimeOut(w->xapp->x->appContext,
    w->x->updateDelay/10+1,(XtTimerCallbackProc)CbUpdateViewTimer,w);
}

static void CbDestroyViewShell(Widget wg,XtPointer xtpXApp,XtPointer pcbs) {
  XApp xa=(XtPointer)xtpXApp;

  xa->x->xViewShellsCount--;
  if (xa->x->xViewShellsCount==0) FreeXApp(xa);
}

void CbDisplayHelpLine(Widget wg,XtPointer pWidget,XtPointer pcbs) {
  XmString xms;

  xms=MakeXmString(GetResourceString(wg,"helpLine","HelpLine","No help"));
  SetValues((Widget)pWidget,
    XmNuserData,(XtPointer)'H',
    XmNlabelString,xms,
    NULL);
  XmStringFree(xms);
}

void CbDisplayDataLine(Widget wg,XtPointer pWidget,XtPointer pcbs) {
  XmString xms;

  xms=MakeXmString((char*)GetUserData(wg));
  SetValues((Widget)pWidget,
    XmNuserData,(XtPointer)'H',
    XmNlabelString,xms,
    NULL);
  XmStringFree(xms);
}

void CbClearHelpLine(Widget wg,XtPointer pWidget,XtPointer pcbs) {
  XmString xms;
  XtPointer msgType;

  GetValues((Widget)pWidget,XmNuserData,&msgType,NULL);
  if (msgType!=(XtPointer)'H') return;

  xms=MakeXmString("");
  SetValues((Widget)pWidget,XmNlabelString,xms,NULL);
  XmStringFree(xms);
}

static void AddDw2UndoInfo(Widget wg,XtPointer w,XtPointer value) {
  AddDependentWidget((View)w,wg,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyUndoButton,value);
}

static void AddDw2Exists(Widget wg,XtPointer w,XtPointer value) {
  AddDependentWidget((View)w,wg,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyIfExists,value);
}

static void AddDw2NotExists(Widget wg,XtPointer w,XtPointer value) {
  AddDependentWidget((View)w,wg,N_NOW | N_ALT | N_NEWAPP,NULL,
    DwNotifyIfNotExists,value);
}

static void CbExposeView(Widget wg,View w,void* pcbs) {
  if (XmToggleButtonGetState(w->x->wSwManualRefresh)) return;

  if (w->x->exposeDelay) {
    w->updateFlag=UPDATE_MAX;
    if (w->x->exposeIntervalId!=(XtIntervalId)NULL) {
      XtRemoveTimeOut(w->x->exposeIntervalId);
      w->x->exposeIntervalId=(XtIntervalId)NULL;
    }
    w->x->exposeIntervalId=XtAppAddTimeOut(w->xapp->x->appContext,
      w->x->exposeDelay,CbViewExposeTimer,w);
  } else
    RepaintView(w);
}

static void CbViewExposeTimer(XtPointer pView,XtIntervalId* xtiid) {
  View w=(View)pView;

  w->x->exposeIntervalId=(XtIntervalId)NULL;
  if (w->updateFlag) RepaintView(w);
}

static void CbResizeView(Widget wg,View w,void* pcbs) {
  GetXViewSize(w);
  if (w->app!=NULL) Cancel(w->app);
  SetViewFactor(w,w->centerX,w->centerY,w->zoomX,w->zoomY);
  if (w->app!=NULL) UndoMark(w->app);
}

static void CbChangeTool(Widget wg,View w,void* pcbs) {
  ToolProc tp;
  int i;

  GetValues(wg,XmNuserData,&tp,NULL);
  for (i=0;i<3;i++) if (XtParent(wg)==w->x->wToolMenus[i]) break;
  if (i>2) FatalError("CbChangeTool()-i: fatal error 1");
  w->x->tools[i]=tp;
  NotifyNewTool(w);
}

static void CbChangeFlags(Widget wg,View w,void* pcbs) {
  SetActiveView(w);
  ProcessXViewShowFlags(w);
  if (w->app!=NULL) UndoMark(w->app);
}

/* Xt action routines
*/

void XtActUseTool(Widget wg,XEvent* xev,String* args,Cardinal* argn) {
  View w;
  ToolProc tp=NULL;
  int tl=TL_CANCEL,x=0,y=0;

  if (*argn!=1) {
    XtWarning("UseTool() action called with too many arguments");
    return;
  }

  w=GetWidgetXView(wg);
  if (w==NULL) return;

/*  printf("xaut: %s\n",*args);*/

  switch(xev->type) {
    case ButtonPress:
        SetActiveView(w);
    case ButtonRelease:
        x=xev->xbutton.x;
        y=xev->xbutton.y;
      break;
    case MotionNotify:
        if (!w->x->mouseInside) return;
        x=xev->xmotion.x;
        y=xev->xmotion.y;
      break;
    case EnterNotify:
      w->x->mouseInside=1;
      break;
    case LeaveNotify:
      w->x->mouseInside=0;
      break;
  }

  if (!strcmp(*args,"PressLeft"))    {tl=TL_PRESS;tp=w->x->tools[0];} else
  if (!strcmp(*args,"PressMiddle"))  {tl=TL_PRESS;tp=w->x->tools[1];} else
  if (!strcmp(*args,"PressRight"))   {tl=TL_PRESS;tp=w->x->tools[2];} else
  if (!strcmp(*args,"DblClkLeft"))   {tl=TL_DBLCLK;tp=w->x->tools[0];} else
  if (!strcmp(*args,"DblClkMiddle")) {tl=TL_DBLCLK;tp=w->x->tools[1];} else
  if (!strcmp(*args,"DblClkRight"))  {tl=TL_DBLCLK;tp=w->x->tools[2];} else
  if (!strcmp(*args,"Motion"))        tl=TL_MOTION; else
  if (!strcmp(*args,"Release"))       tl=TL_RELEASE; else
  if (!strcmp(*args,"RelCancel"))
    tl=w->x->mouseInside ? TL_RELEASE:TL_CANCEL; else
  if (!strcmp(*args,"Cancel"))        tl=TL_CANCEL; else
  if (!strcmp(*args,"Enter"))         tl=TL_ENTER; else
  if (!strcmp(*args,"Leave"))         tl=TL_LEAVE; else {
    XtWarning("UseTool() called with wrong arguments");
    return;
  }

  if (tp==NULL) tp=w->activeTool;
  if (tp==NULL) return;

  CallToolProc(w,tp,tl,xev->xbutton.x,xev->xbutton.y);
}

/* Unused
*/
void XtActCancelTool(Widget wg,XEvent* xev,String* args,Cardinal* argn) {
  View w;

  w=GetWidgetXView(wg);
  if (w!=NULL && w->app!=NULL) Cancel(w->app);
}

void XtActCirculTool(Widget wg,XEvent* xev,String* args,Cardinal* argn) {
  View w;
  ToolProc tp;

  w=GetWidgetXView(wg);
  if (w==NULL) return;
  tp=w->x->tools[0];
  w->x->tools[0]=w->x->tools[1];
  w->x->tools[1]=w->x->tools[2];
  w->x->tools[2]=tp;
  NotifyNewTool(w);
}

void CbHelp(Widget _wg,XtPointer xtpView,XtPointer pcbs) {
  View w=(View)xtpView;
  Widget wg;
  XmAnyCallbackStruct* cbs=(XmAnyCallbackStruct*)pcbs;
  String s;

  wg=XtWindowToWidget(cbs->event->xany.display,cbs->event->xany.window);

  for(s=NULL;wg!=NULL;wg=XtParent(wg)) {
    s=GetResourceString(wg,"helpTopic","HelpTopic","");
    if (s!=NULL && *s) break;
  }

  if (s==NULL) {
    ErrorBox(_wg,GetStr(w,ERR_NOTOPIC));
    return;
  }

  DisplayHelpTopic(w,s);
}


