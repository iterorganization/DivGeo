/* Menu callbacks */

#ifndef _x_menu_h
#define _x_menu_h

void CbFileNew(Widget wg,View w,void* xtp);
void CbFileOpen(Widget wg,View w,void* xtp);
void CbFileSave(Widget wg,View w,void* xtp);
void CbFileOutput(Widget wg,XtPointer xtpw,XtPointer pcbs);
void CbTemplate(Widget wg,View w,void* xtp);
void CbEquil(Widget wg,View w,void* xtp);
void CbSonnet(Widget wg,View w,void* xtp);
void CbFilePrint(Widget,XtPointer,XtPointer);
void CmExportMesh(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CmExportElemsAsTemplate(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CbQuit(Widget wg,View w,void* xtp);

void CbUndo(Widget wg,View w,void* xtp);
void CbRedo(Widget wg,View w,void* xtp);
void CbRedoAll(Widget wg,View w,void* xtp);
void CbMarkAll(Widget wg,View w,void* xtp);
void CbUnmarkAll(Widget wg,View w,void* xtp);

void CbViewRefresh(Widget wg,XtPointer xtpView,void* pcbs);
void CbZoomIn(Widget wg,View w,void* xtp);
void CbZoomOut(Widget wg,View w,void* xtp);
void CbShowPicture(Widget wg,View w,void* xtp);
void CbShowSelection(Widget wg,View w,void* xtp);
void CmPrevZoom(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CmRemoveLabels(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CmViewMeshHeader(Widget wg,XtPointer xtpV,XtPointer pcbs);

void CbCmAppendTemplate(Widget wg,View w,void* xtp);
void CmConvertTemplateToChords(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CmConvertElemsToChords(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CmConvertChordsToElems(Widget wg,XtPointer xtpV,XtPointer pcbs);

void CbCmAt(Widget wg,View w,void* xtp);
void CbCmRotMove(Widget wg,View w,void* xtp);
void CbCmRemoveXPoint(Widget wg,View w,void* xtp);
void CbCmRenumber(Widget wg,View w,void* xtp);
void CbCmRemoveEmptyNodes(Widget wg,View w,void* xtp);
void CbCmInstallSeparators(Widget wg,View w,void* xtp);
void CbCmRemoveSeparators(Widget wg,View w,void* xtp);
void CbCmDelObjects(Widget wg,View w,void* pcbs);
void CbCmShowInvalidVars(Widget,XtPointer,XtPointer);
void CbCmFindXPoints(Widget,XtPointer,XtPointer);
void CbCmFindXPointSegs(Widget,XtPointer,XtPointer);
void CmRebuildCarreInfo(Widget,XtPointer,XtPointer);
void CmTest(Widget,XtPointer,XtPointer);


void CbAddVarSet(Widget wg,VarSetDef vsd,void* xtp);
void CbDelVarSet(Widget wg,VarSet vs,void* xtp);
void CbEditVarSet(Widget wg,VarSet vs,void* xtp);

void CmSetOutputModeX(Widget wg,XtPointer xtpView,void* pcbs);
void CmSetOutputFlags(Widget wg,XtPointer xtpView,XtPointer pcbs);
void CmSetMeshOptions(Widget wg,XtPointer xtpV,XtPointer pcbs);
void CbOptionsMenuToggle(Widget wg,XtPointer xtpView,void* pcbs);
void CbSwMenu(Widget wg,XtPointer xtpView,void* pcbs);
void CbSwToolbar(Widget wg,XtPointer xtpView,void* pcbs);
void CbSwManualRefresh(Widget wg,XtPointer xtpView,void* pcbs);

void CbWindowToolBar(Widget wg,XtPointer arg,XtPointer pcbs);
void CbWindowOpen(Widget wg,View w,void* xtp);
void CbWindowClose(Widget wg,View w,void* xtp);
void CbWindowInfo(Widget wg,View w,void* xtp);
void CbAbout(Widget wg,View w,void* xtp);

void CbHelpMenu(Widget w,XtPointer xtpView,XtPointer pcbs);

#endif
