/* Motif toolkit View implementation
*/

#ifndef _x_view_h
#define _x_view_h

struct _ViewExt {
  int mouseInside;
  ToolProc tools[3];
  Group dependentWidgets;
  Group varsDialogs,varsMenuWidgets;
  unsigned changes;
  unsigned discardFlags;

  Pixel pxBackground,pxHighlight,pxNode,pxElem,pxSurface,pxHighlightRect,
    pxErrorHighlight,pxSeparator,pxTool[4],pxSource,pxChord,
    pxTemplate,pxNormal,pxMark,pxXPoint,pxEquil1,pxEquil2,pxGridPoint,
    pxAxes,pxGrid,pxGridText,pxElemNumber,pxViewLabel,pxSonnet,
    pxXPointTest1,pxXPointTest2,pxXPointSeg,pxMeshElement1,pxMeshElement2,
    pxMeshCellCenter1,pxMeshCellCenter2;

  Dimension whHighlight,whNode,whElem,whSurface,whHighlightRect,whTemplate,
    whNormal,whMark,whXPoint,whEquil,whAxes,whGrid,whGridText,whElemNumber,
    whViewLabel,whSonnet,whGridPoint,whSeparator,whSource,whChord,
    whXPointTest1,whXPointTest2,whXPointSeg,whMeshCell,
    whMeshElement1,whMeshElement2,whMeshCellCenter1,whMeshCellCenter2;

  XtTranslations translations; /* To fix Motif bug */

  Boolean bUseSquares;

  int updateDelay,updateVarsDelay,updateInfoDelay,updateMouseDelay,
    exposeDelay;

  GC gc;
  XtIntervalId exposeIntervalId,updateViewIntervalId;

  Widget wShell,wMain,wMenu,wCommand,wMsg,wDraw,
    wToolBar,
    wBnShowMenu,
/*    wSwMarkGroup,wSwMarkSingle, */
    wCmRefresh,wSepRefresh,
    wShAxes,wShGrid,wShElems,wShNormals,wShNumbers,wShNodes,wShIrrNodes,
    wShTemplate,wShEquil,wShSurfaces,wShGridPoints,wShSonnet,wShSeparators,
    wShSources,wShChords,wShMeshDetails,wShStretch,wShTopView,wSh3DChords,
    wCmAddVarSet,wCmDelVarSet,wMnAddVarSet,wMnDelVarSet,wMnVars,
    wSwMenu,wSwToolBar,wSwManualRefresh,
  /*  *wVarsMenuWidgets, -- Replaced with varsMenuWidgets group */
    wToolMenus[3],wTools[3];
};

typedef void (*DwNotifyProc)(Widget,View,int evt,void* obj,void* userData);

typedef struct _DependentWidget {
  Widget widget;
  int eventMask;
  DwNotifyProc notifyProc;
  void* userData,* object;
}* DependentWidget;

#define MakeXmString(s) XmStringCreateLtoR((s),XmFONTLIST_DEFAULT_TAG)
#define DelDependentWidget(w,wg) DelDependentWidgetEx(w,wg,False)

extern ToolProc menuTools[];

View CreateXmView(XApp xap,App a);

void AddDependentWidget(View w,Widget wg,int eventMask,void* object,
  DwNotifyProc notifyProc,void* userData);
void DelDependentWidgetEx(View w,Widget wg,int bWidgetDestroyed);

void AddDWProc(Widget wg,XtPointer xtpV,XtPointer mask,XtPointer obj,
    XtPointer xtpProc,XtPointer userData);

View GetWidgetXView(Widget wg);

void ToggleXViewMenu(View w);
void ToggleXViewToolBar(View w);

void XtActUseTool(Widget wg,XEvent* xev,String* args,Cardinal* argn);
void XtActCancelTool(Widget wg,XEvent* xev,String* args,Cardinal* argn);
void XtActCirculTool(Widget wg,XEvent* xev,String* args,Cardinal* argn);

void CbUpdateVarsTimer(View w,XtIntervalId* xtp);
void CbHelp(Widget wg,XtPointer pView,XtPointer pcbs);

void CbDisplayHelpLine(Widget wg,XtPointer pWidget,XtPointer pcbs);
void CbDisplayDataLine(Widget wg,XtPointer pWidget,XtPointer pcbs);
void CbClearHelpLine(Widget wg,XtPointer pWidget,XtPointer pcbs);

void CreateAutosaveInfo(View w);

#define DISCARD_MESH    0x0001
#define DISCARD_DOC     0x0002

#endif

