#include "x_dg.h"
#include <stdint.h>

void DwNotifyUndoButton(Widget wg,View w,int evt,void* obj,void* d) {
  if (w->app==NULL) return;

  switch ((uintptr_t)d) {
    case 0:
      SetSensitiveEx(wg,!!GroupCount(w->app->undoStack));
      break;
    case 1:
      SetSensitiveEx(wg,!!GroupCount(w->app->redoStack));
      break;
    case 2:
      SetSensitiveEx(wg,!GetPrevViewInfo(w->app,NULL,NULL,NULL,NULL,NULL,
        True));

      break;
    default:
      assert(0);
  }
}

void DwNotifyIfExists(Widget wg,View w,int evt,void* obj,void* d) {
  Node n;
  SurfaceEx sx;
  Index ix;

  if (w->app==NULL) return;

  switch((uintptr_t)d) {
    case T_MARKING:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->mark));
      break;
    case T_NODE:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->nodes));
      break;
    case T_ELEM:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->elems));
      break;
    case T_SURFACEEX:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->surfacesEx));
      break;
    case T_DEL_VSURFACE:
      for (sx=AppSurfaceEx1st(w->app,&ix);sx!=NULL;sx=Next(&ix))
        if (SurfaceExVirtual(sx)) {SetSensitiveEx(wg,True);return;}
      SetSensitiveEx(wg,False);
      break;
    case T_GRIDPOINTEX:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->gridPointsEx));
      break;
    case T_SEPARATOR:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->separators));
      break;
    case T_SOURCE:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->sources));
      break;
    case T_CHORD:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->chords));
      break;
    case T_EQUIL:
      SetSensitiveEx(wg,w->app->equil!=NULL);
      break;
    case T_TEMPLATE:
      SetSensitiveEx(wg,w->app->template!=NULL);
      break;
    case T_SONNET:
      SetSensitiveEx(wg,w->app->sonnetData!=NULL);
      break;
    case T_XPOINTTEST:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->xpointTests));
      break;
    case T_XPOINTSEG:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->xPointSegs));
      break;
    case T_GRIDPOINTSEG:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->gridPointSegs));
      break;
    case T_SURFACEZONE:
      SetSensitiveEx(wg,!IsEmptyGroup(w->app->surfaceZones));
      break;
    case T_MESH:
      SetSensitiveEx(wg,w->app->mesh!=NULL);
      break;
    case T_EMPTYNODE:
      for (n=AppNode1st(w->app,&ix);n!=NULL;n=Next(&ix))
        if (IsEmptyNode(n)) {SetSensitiveEx(wg,True);return;}
      SetSensitiveEx(wg,False);
      break;
    default:
      assert(0);
  }
}

void DwNotifyIfNotExists(Widget wg,View w,int evt,void* obj,void* d) {
  Node n;
  SurfaceEx sx;
  Index ix;

  if (w->app==NULL) return;

  switch((uintptr_t)d) {
    case T_MARKING:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->mark));
      break;
    case T_NODE:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->nodes));
      break;
    case T_ELEM:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->elems));
      break;
    case T_SURFACEEX:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->surfacesEx));
      break;
    case T_DEL_VSURFACE:
      for (sx=AppSurfaceEx1st(w->app,&ix);sx!=NULL;sx=Next(&ix))
        if (SurfaceExVirtual(sx)) {SetSensitiveEx(wg,False);return;}
      SetSensitiveEx(wg,True);
      break;
    case T_GRIDPOINTEX:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->gridPointsEx));
      break;
    case T_SEPARATOR:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->separators));
      break;
    case T_SOURCE:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->sources));
      break;
    case T_CHORD:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->chords));
      break;
    case T_EQUIL:
      SetSensitiveEx(wg,w->app->equil==NULL);
      break;
    case T_TEMPLATE:
      SetSensitiveEx(wg,w->app->template==NULL);
      break;
    case T_SONNET:
      SetSensitiveEx(wg,w->app->sonnetData==NULL);
      break;
    case T_XPOINTTEST:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->xpointTests));
      break;
    case T_XPOINTSEG:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->xPointSegs));
      break;
    case T_MESH:
      SetSensitiveEx(wg,w->app->mesh==NULL);
      break;
    case T_EMPTYNODE:
      for (n=AppNode1st(w->app,&ix);n!=NULL;n=Next(&ix))
        if (IsEmptyNode(n)) {SetSensitiveEx(wg,False);return;}
      SetSensitiveEx(wg,True);
      break;
    case T_GRIDPOINTSEG:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->gridPointSegs));
      break;
    case T_SURFACEZONE:
      SetSensitiveEx(wg,IsEmptyGroup(w->app->surfaceZones));
      break;
    default:
      assert(0);
  }
}

void DwNotifyShowFlags(Widget wg,View w,int evt,void* obj,void* d) {
  if (w->app==NULL) return;

  switch ((uintptr_t)d) {
    case 0:
      SetSensitiveEx(wg,!!(w->showFlags & SHW_STRETCH));
      break;
    case 1:
      SetSensitiveEx(wg,!(w->showFlags & SHW_TOPVIEW));
      break;
    default:
      assert(0);
  }
}

void DwUnmap(Widget wg,View w,int evt,void* obj,void* d) {
  if (d!=NULL) wg=(Widget)d;
  else while (!XtIsShell(wg)) wg=XtParent(wg);
  XtPopdown(wg);                        /* relcheck_ignore_line */
}

void DwDestroy(Widget wg,View w,int evt,void* obj,void* d) {
  if (d!=NULL) wg=(Widget)d;
  else while (!XtIsShell(wg)) wg=XtParent(wg);
  XtDestroyWidget(wg);
}
