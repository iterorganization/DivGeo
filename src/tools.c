#include "dg.h"

/* "Tl_DBLCLK" actually corresponds to Shift+move */

/* Generic tool
void Tl(View w,int event,double x,double y) {
  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;

      if () {
        d=Free(d);
        break;
      }

      break;
    case TL_MOTION:
      if (d==NULL) break;

      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);

      UndoMark(w->app);
      if (w->app->highlightMode) ViewMsgEx(w,0,NULL);

      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      Highlight(w,,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}
*/


void TlExamine(View w,int event,double x,double y) {
  struct {
    int bEnh,hm;
    Group g;
    void* obj;
  }* d;
  void* p = NULL;
  Group g;
  MeshPoint mpt;
  MeshCell mc;
  Index ix;
  int i;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      break;
/*       
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->hm=1;
      d->obj=NULL;
      d->g=NULL;
      p=HitViewObject(w,x,y,SHW_SURFACES);

      if (p==NULL) {
        d=Free(d);
        break;
      }
      d->g=CreateGroup();
      goto Motion;
 */
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->hm=1;
      d->obj=NULL;
      p=HitViewObject(w,x,y,SHW_NODES|SHW_ELEMS|SHW_SURFACES|SHW_GRIDPOINTS|
          SHW_SEPARATORS|SHW_SOURCES|SHW_CHORDS|SHW_3DCHORDS|
          SHW_XPOINTTESTS|
          SHWX_MESHCELLS|SHWX_MESHELEMENTS|SHWX_MESHPOINTS);
      if (p==NULL) {
        d=Free(d);
        break;
      }

    case TL_MOTION:
    Motion:
      if (d==NULL) break;
      if (d->bEnh) {
/*
        if (d->obj!=NULL) p=HitViewObject(w,x,y,SHW_SURFACES);
        if (p==NULL || p==d->obj) break;
        g=NULL;

        switch (GetObjType(p)) {
          case T_SURFACE:
            s0=(Surface)p;
            if (!strcmp(GetSurfaceCreatorId(s0),"*")) {
              g=CreateGroup();
              d->hm=0;
              GroupAdd(g,s0);
            } else {
              if (d->obj!=NULL && GetObjType(d->obj)==T_SURFACE) {
                if (!strcmp(GetSurfaceCreatorId(s0),
                    GetSurfaceCreatorId((Surface)d->obj))) break;
              }
              g=CreateGroup();
              d->hm=1;
              for (s=AppSurface1st(w->app,&ix);s!=NULL;s=Next(&ix))
              if (!strcmp(GetSurfaceCreatorId(s),GetSurfaceCreatorId(s0)))
                GroupAdd(g,s);
            }
            d->obj=p;
            break;
          default:
            assert(0);
        }
        if (g==NULL) break;
        if (d->g!=NULL) d->g=FreeGroup(d->g);
        d->g=g;g=NULL;

        UnhighlightAll(w);
        SetHighlightMode(w->app,d->hm);
        for (p=Group1st(d->g,&ix);p!=NULL;p=Next(&ix))
          Highlight(w,p,1);
        SetExamineMsg(w,d->obj);
        SetViewMsg(w,"");
*/
      } else {
        if (d->obj!=NULL) p=HitViewObject(w,x,y,SHW_NODES|SHW_ELEMS|
        SHW_SURFACES|SHW_GRIDPOINTS|SHW_SEPARATORS|SHW_SOURCES|SHW_CHORDS|
        SHW_3DCHORDS|SHW_XPOINTTESTS|
        SHWX_MESHCELLS|SHWX_MESHELEMENTS|SHWX_MESHPOINTS);

        if (p==NULL || p==d->obj) break;
        d->obj=p;

        UnhighlightAll(w);
        SetHighlightMode(w->app,1);
        switch(GetObjType(p)) {
          case T_NODE:
          case T_ELEM:
          case T_SEPARATOR:
          case T_SOURCE:
          case T_CHORD:
          case T_MESHELEMENT:
          case T_XPOINTTEST:
          case T_XPOINTSEG:
          case T_SURFACEEX:
          case T_GRIDPOINTEX:
            Highlight(w,p,1);
            SetExamineMsg(w,p);
            break;
          case T_MESHPOINT:
            mpt=p;
            for (i=MCN_START;i<MCN_END;i++) if (mpt->elements[i]!=NULL)
              Highlight(w,mpt->elements[i],1);
            SetExamineMsg(w,p);
            break;
          case T_MESHCELL:
            mc=p;
            for (i=MCN_START;i<MCN_END;i++) Highlight(w,mc->elements[i],1);
            Highlight(w,mc,1);
            SetExamineMsg(w,p);
            break;

        }
      }
      break;

    case TL_RELEASE:
      if (d==NULL) break;
      if (d->bEnh) {
/*        if (d->g!=NULL) d->g=FreeGroup(d->g); */
      } else {
        if (d->obj!=NULL) NotifyExamine(w->app,d->obj);
      }
      UnhighlightAll(w);
      UndoMark(w->app);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      if (d->hm) SetHighlightMode(w->app,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      if (d->bEnh) {
/*        if (d->g!=NULL) d->g=FreeGroup(d->g); */
      }
      d=Free(d);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      Cancel(w->app);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlRotate(View w,int event,double x,double y) {
  struct {
    int bEnh;
    double refAngle,t0,t1;
  }* d;
  void* p = NULL;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->refAngle=atan2(y-w->centerY,x-w->centerX);
      d->t0=w->xyAngle;
      break;
    case TL_MOTION:
      if (d==NULL) break;
      w->xyAngle+=atan2(y-w->centerY,x-w->centerX)-d->refAngle;
      ClearView(w);
      RepaintView(w);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      d->t1=w->xyAngle;
      w->xyAngle=d->t0;
      SetViewAngle(w,d->t1);
      UndoMark(w->app);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      w->xyAngle=d->t0;
      d=Free(d);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      Cancel(w->app);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlStretch(View w,int event,double x,double y) {
  struct {
    int bEnh,bHoriz;
    double dx0,dy0,minX0,minY0,maxX0,maxY0,zoomX0,zoomY0,sgnX0,sgnY0;
    double dx,dy,sgnX,sgnY;
  }* d;
  void* p = NULL;
  double fMinX,fMinY,fMaxX,fMaxY;

  d=w->toolData;
  ScreenRotate(w,1,&x,&y);
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      if (~w->showFlags & SHW_STRETCH) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->dx0=d->dx=(x-w->centerX)*w->zoomX*w->xScaleSign;
      d->dy0=d->dy=(y-w->centerY)*w->zoomY*w->yScaleSign;
      d->sgnX0=d->dx0!=0 ? (d->dx0>0 ? 1 : -1) : 0;
      d->sgnY0=d->dy0!=0 ? (d->dy0>0 ? 1 : -1) : 0;
      if (!d->sgnX0 || !d->sgnY0 || fabs(d->dx)==fabs(d->dy)) {
	d=Free(d);
	break;
      }
      d->bHoriz=fabs(d->dx)>fabs(d->dy);
      d->zoomX0=w->zoomX;
      d->zoomY0=w->zoomY;
      d->minX0=w->minX;
      d->minY0=w->minY;
      d->maxX0=w->maxX;
      d->maxY0=w->maxY;
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->bHoriz) {
	d->dx=(x-w->centerX)*w->zoomX*w->xScaleSign;
      } else {
	d->dy=(y-w->centerY)*w->zoomY*w->yScaleSign;
      }
      d->sgnX=d->dx!=0 ? (d->dx>0 ? 1 : -1) : 0;
      d->sgnY=d->dy!=0 ? (d->dy>0 ? 1 : -1) : 0;
      if (d->sgnX!=d->sgnX0 || d->sgnY!=d->sgnY0) {
	break;
      }
      if (d->bHoriz) {
	w->zoomX=d->zoomX0/d->dx0*d->dx;
	w->minX=w->centerX-w->width/w->zoomX/2.;
	w->maxX=w->centerX+w->width/w->zoomX/2.;
      } else {
	w->zoomY=d->zoomY0/d->dy0*d->dy;
	w->minY=w->centerY-w->height/w->zoomY/2.;
	w->maxY=w->centerY+w->height/w->zoomY/2.;
      }
      ClearView(w);
      RepaintView(w);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      fMinX=w->minX;
      fMinY=w->minY;
      fMaxX=w->maxX;
      fMaxY=w->maxY;
      w->zoomX=d->zoomX0;
      w->zoomY=d->zoomY0;
      w->minX=d->minX0;
      w->minY=d->minY0;
      w->maxX=d->maxX0;
      w->maxY=d->maxY0;
      SetViewRect(w,fMinX,fMinY,fMaxX,fMaxY);
      UndoMark(w->app);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      w->zoomX=d->zoomX0;
      w->zoomY=d->zoomY0;
      w->minX=d->minX0;
      w->minY=d->minY0;
      w->maxX=d->maxX0;
      w->maxY=d->maxY0;
      d=Free(d);
      ClearView(w);
      RepaintView(w);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      Cancel(w->app);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlMark(View w,int event,double x,double y) {
  struct {
    int bEnh,bMoved;
    Elem e;
    int x1,y1,status;
    long flags;
    Group g;
  }* d;
  void* obj, *p;
  Elem e1;
  int i;
  Group g,g1;
  Index ix;
  /*Separator sep;*/
  /*double dE,dSep;*/

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->bMoved=0;
      d->e=NULL;
      d->g=NULL;
      d->status=0;
      d->flags=0;
      d->x1=x;
      d->y1=y;

      e1=HitViewObject(w,x,y,SHW_ELEMS);
      if (e1==NULL) break;
      i=!IsMarked(w->app,e1);
      g=GetElemChain(e1,NULL,NULL);
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      MarkGroup(w->app,g,i);
      g=FreeGroup(g);
      break;

    case TL_PRESS:
      if (d!=NULL) break;
      obj=HitViewObject(w,x,y,SHW_ELEMS|SHW_SEPARATORS|SHW_SOURCES|
          SHW_CHORDS|SHW_3DCHORDS|SHWX_MESHCELLS|SHWX_MESHELEMENTS);
      if (obj==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NOELEMS));
        break;
      }

      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->g=NULL;
      d->e=NULL;

      switch(GetObjType(obj)) {
        case T_SEPARATOR:
        case T_SOURCE:
        case T_CHORD:
        case T_MESHELEMENT:
        case T_MESHCELL:
          MarkObject(w->app,obj,!IsMarked(w->app,obj));
          break;
        case T_ELEM:
          d->e=obj;
          d->status=!IsMarked(w->app,d->e);
          MarkObject(w->app,d->e,d->status);
          break;
        default:
          assert(0);
      }
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->bEnh) {
	if (w->showFlags & SHW_TOPVIEW) break;
        if (!d->bMoved) {
          d->bMoved=1;
          CancelNonDestructive(w->app);
          d->g=CreateGroup();

          d->flags=0L;
          for (p=AppMark1st(w->app,&ix);p!=NULL;p=Next(&ix)) {
            switch (GetObjType(p)) {
              case T_ELEM:       d->flags |= SHW_ELEMS;        break;
              case T_SEPARATOR:  d->flags |= SHW_SEPARATORS;   break;
              case T_SOURCE:     d->flags |= SHW_SOURCES;      break;
              case T_CHORD:      d->flags |= SHW_CHORDS;       break;
              case T_MESHELEMENT:d->flags |= SHWX_MESHELEMENTS;break;
              case T_MESHCELL:   d->flags |= SHWX_MESHCELLS;   break;
              default: break;
            }
          }
          if (d->flags==0L) d->flags=
              SHW_ELEMS|SHW_SEPARATORS|SHW_SOURCES|SHW_CHORDS|SHWX_MESHCELLS|
              SHWX_MESHELEMENTS;
          SetHighlightMode(w->app,1);
        }

        SetHighlightRect(w,d->x1,d->y1,x,y);
        FlushView(w);

        for (p=Group1st(d->g,&ix);p!=NULL;p=Next(&ix)) {
          if (!ObjectInRectangle(p,d->x1,d->y1,x,y)) {
            GroupDel(d->g,p);
            MarkObject(w->app,p,0);
          }
        }

        g=CoveredViewObjects(w,d->x1,d->y1,x,y,d->flags);

        g1=CreateGroup();
        for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix)) {
          if (InGroup(d->g,p)) continue;
          if (IsMarked(w->app,p)) continue;
          GroupAdd(g1,p);
          GroupAdd(d->g,p);
        }

        MarkGroup(w->app,g1,1);
        g1=FreeGroup(g1);
        g=FreeGroup(g);

      } else if (d->e!=NULL) {
        e1=HitElem(w->app,x,y,NULL,NULL);
        if (e1==d->e || e1==NULL || !ElemsConnected(e1,d->e)) break;
        d->e=e1;
        MarkObject(w->app,d->e,d->status);
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      if (d->g!=NULL) d->g=FreeGroup(d->g);
      d=Free(d);
      ClearHighlightRect(w);
      UndoMark(w->app);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      if (d->g!=NULL) d->g=FreeGroup(d->g);
      d=Free(d);
      Cancel(w->app);
      ClearHighlightRect(w);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlZoom(View w,int event,double x,double y) {
  struct {
    int bEnh,bMoved;
    double cx,cy;
  }* d;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->bMoved=0;
      SetHighlightMode(w->app,1);
      d->cx=x;
      d->cy=y;
      SetHighlightRect(w,2*w->minX-w->maxX,2*w->minY-w->maxY,
        2*w->maxX-w->minX,2*w->maxY-w->minY);
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->bMoved=0;
      d->cx=x;
      d->cy=y;
      SetHighlightMode(w->app,1);
      SetHighlightRect(w,x,y,x,y);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      d->bMoved=1;
      if (d->bEnh) {
        SetHighlightRect(w,w->minX+x-d->cx,w->minY+y-d->cy,
          w->maxX+x-d->cx,w->maxY+y-d->cy);

      } else {
        SetHighlightRect(w,d->cx,d->cy,
          fabs(x-d->cx)<w->minZoomX/w->zoomX? d->cx : x,
          fabs(y-d->cy)<w->minZoomY/w->zoomY? d->cy : y);
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;

      if (d->bEnh) {
        if (!d->bMoved) SetViewFactor(w,w->centerX,w->centerY,
          w->zoomX/2,w->zoomY/2);
        else SetViewFactor(w,w->centerX+d->cx-x,w->centerY+d->cy-y,
          w->zoomX,w->zoomY);
      } else {
        if (w->hrMaxX==w->hrMinX || w->hrMaxY==w->hrMinY)
          SetViewFactor(w,d->cx,d->cy,w->zoomX,w->zoomY);
        else
          SetViewRect(w,w->hrMinX,w->hrMinY,w->hrMaxX,w->hrMaxY);
      }

      UnhighlightAll(w);

      d=Free(d);
      UndoMark(w->app);
      ViewMsgEx(w,0,NULL);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      UnhighlightAll(w);
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlMoveObject(View w,int event,double x,double y) {
  struct {
    int bEnh;
    void* obj;
    int hm;
    Group grp;
    double* values;
    double x0;
    int ptNum;
    int bSurfaceExVirtual;
    double x,y,r; /* used by enhanced/double-click/shift-move, etc. */
  }* d;
  int i,r;
  int area;
  double value,factor;
  Index ix;
  GridPointEx gpx;
  SurfaceEx sx,s0x;
  SurfaceZone sz;
  Chord ch;
  double chy1,chy2;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->hm=1;
      d->grp=NULL;
      d->values=NULL;
      d->obj=HitViewObject(w,x,y,SHW_NODES|SHW_SURFACES|
          SHW_GRIDPOINTS|SHW_SOURCES|SHW_CHORDS);
      switch(GetObjType(d->obj)) {
        case T_CHORD:
	  ch=HitChord(w->app,x,y,&d->ptNum,NULL);
	  d->x=x;
	  d->y=y;
	  d->r=(abs(d->ptNum)==1) ?
	    hypot(ch->x1,ch->z1) : hypot(ch->x2,ch->z2);
	  if (d->r > 1.e15) puts("really dig d->r");
	 break;
        default:
          SetHighlightMode(w->app,d->hm=0);
          ViewMsgEx(w,ERR_BADSTRETCHTYPE,"");
          break;
      }
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->obj=HitViewObject(w,x,y,SHW_NODES|SHW_IRRNODES|SHW_SURFACES|
          SHW_GRIDPOINTS|SHW_SOURCES|SHW_CHORDS);
      if (d->obj==NULL) {
        d=Free(d);
        break;
      }
      SetHighlightMode(w->app,d->hm=!IsLocked(d->obj));
      Highlight(w,d->obj,1);

      /* Store the "virtual" flag of SurfaceEx */

      if (GetObjType(d->obj)==T_SURFACEEX) {
        sx=d->obj;
        d->bSurfaceExVirtual=SurfaceExVirtual(sx);
      }

      /* Prohibit most movements of surfaces/grid points in Carre mode */

      if (IsCarreLocked(d->obj)) {
        if (GetObjType(d->obj)==T_GRIDPOINTEX &&
            w->app->outputMode==OUTPUTMODE_CARRE) {
          SetHighlightMode(w->app,d->hm=0);
          SetViewMsg(w,GetStr(w,ERR_CARRE_INHIBITS));
        }

        if (GetObjType(d->obj)==T_SURFACEEX && !d->bSurfaceExVirtual &&
            w->app->outputMode==OUTPUTMODE_CARRE) {
          s0x=d->obj;
          sz=FindSurfaceZone(s0x->app,s0x->zone);
          if (sz!=NULL && (sz->flags & SZF_LIMITBYSURFACE)) {
            for (sx=AppSurfaceEx1st(w->app,&ix);sx!=NULL;sx=Next(&ix))
              if (sx!=s0x && sx->zone==s0x->zone) DelSurfaceEx(sx);
          } else {
            SetHighlightMode(w->app,d->hm=0);
            SetViewMsg(w,GetStr(w,ERR_CARRE_INHIBITS));
          }
        }
      }

      if (GetObjType(d->obj)==T_NODE) HighlightDragNode(w,d->obj,1);
      if (GetObjType(d->obj)==T_CHORD) {
        HitChord(w->app,x,y,&d->ptNum,NULL);
      }

      if (d->hm) SetExamineMsg(w,d->obj);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (!d->bEnh) {
        i=0;
        switch(GetObjType(d->obj)) {
          case T_NODE:
            if (!w->app->highlightMode) break;
            ChangeNode(w->app,d->obj,x,y);
            break;
          case T_SURFACEEX:
            if (!w->app->highlightMode) break;
            sx=d->obj;
            if (SurfaceExVirtual(sx)) {
              ChangeSurfaceExXY(sx,x,y);
            } else {
              i=ChangeSurfaceExByXY(sx,x,y,&r);
              d->hm=sx->bCoordsOk && sx->zone!=SZN_XY;
              if (!d->hm) SetViewMsg(w,GetStr(w,r));
              SetHighlightMode(w->app,d->hm);
              i=r;
            }
            break;
          case T_GRIDPOINTEX:
            if (!w->app->highlightMode) break;
            if (HitGridPointExPos(w->app,x,y,&area,&value)) break;
            if (area!=((GridPointEx)d->obj)->zone) break;
            ChangeGridPointEx(d->obj,area,value);
            break;
          case T_SOURCE:
            if (!w->app->highlightMode) break;
            i=ChangeSource(w->app,d->obj,x,y);
            if (i) SetViewMsg(w,GetStr(w,i));
            break;
          case T_CHORD:
            if (!w->app->highlightMode) break;
            ch=d->obj;
	    chy1=(~w->showFlags & SHW_TOPVIEW) ? ch->y1 : ch->z1;
	    chy2=(~w->showFlags & SHW_TOPVIEW) ? ch->y2 : ch->z2;
            i=(abs(d->ptNum)==1) ?
              ChangeChord(w->app,ch,x,y,ch->x2,chy2) :
              ChangeChord(w->app,ch,ch->x1,chy1,x,y);
            if (i) SetViewMsg(w,GetStr(w,i));
            break;
          default:
            FatalError("TlMove()-type%d:fatal error1",GetObjType(d->obj));
        }
        if (!i) SetExamineMsg(w,d->obj);
      } else {
        if (!d->hm) break;
        switch(GetObjType(d->obj)) {
          case T_CHORD:
	    r=0;
	    if (!w->app->highlightMode) break;
	    ch=d->obj;
	    if (~w->showFlags & SHW_TOPVIEW) {
	      if (abs(d->ptNum)==1)
		ChangeChord(w->app,ch,ch->x1,y,ch->x2,ch->y2);
	      else ChangeChord(w->app,ch,ch->x1,ch->y1,ch->x2,y);
	      SetExamineMsg(w,ch);
	    } else {
	      factor=d->r/hypot(x,y);
	      if (abs(d->ptNum)==1)
		ChangeChord(w->app,ch,factor*x,factor*y,ch->x2,ch->z2);
	      else ChangeChord(w->app,ch,ch->x1,ch->z1,factor*x,factor*y);
	      SetExamineMsg(w,ch);
	    }
	    break;
        }
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      if (!d->bEnh) {
        if (!w->app->highlightMode) {
          if (IsLocked(d->obj)) SetViewMsg(w,WhyLocked(w,d->obj));
          Cancel(w->app);
        } else {
          UndoMark(w->app);
          ViewMsgEx(w,0,NULL);
        }
      } else {
        if (!d->hm) Cancel(w->app);
        else {
          UndoMark(w->app);
          ViewMsgEx(w,0,NULL);
        }
        if (d->values!=NULL) d->values=Free(d->values);
        if (d->grp!=NULL) d->grp=Free(d->grp);
      }
      d=Free(d);
      ViewMsgEx(w,0,NULL);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      if (d->hm) SetHighlightMode(w->app,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      if (d->bEnh) {
        if (d->values!=NULL) d->values=Free(d->values);
        if (d->grp!=NULL) d->grp=Free(d->grp);
      }
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlRemoveObject(View w,int event,double x,double y) {
  struct {
    int bEnh;
    void* obj;
    int hm;
  }* d;
/*  int i;*/
  void* p;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->obj=HitViewObject(w,x,y,SHW_NODES|SHW_ELEMS|SHW_XPOINTTESTS|
          SHW_SURFACES|SHW_GRIDPOINTS|SHW_SOURCES|SHW_CHORDS|SHW_3DCHORDS);
      if (d->obj==NULL) {
        d=Free(d);
        break;
      }
      d->hm=!IsLocked(d->obj);
      if (GetObjType(d->obj)==T_NODE && !IsEmptyNode(d->obj)) {
        d->hm=0;
        ViewMsgEx(w,ERR_NONEMPTYNODE,NULL);
      }

      /* Inhibit delete surfaces/grid points in Carre mode */

      if (IsCarreLocked(d->obj)) {
        d->hm=0;
        ViewMsgEx(w,ERR_CARRE_INHIBITS,NULL);
      }

      SetHighlightMode(w->app,d->hm);
      if (d->hm) SetExamineMsg(w,d->obj);
      Highlight(w,d->obj,1);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (!d->bEnh) {
        p=HitViewObject(w,x,y,SHW_NODES|SHW_ELEMS|SHW_XPOINTTESTS|
            SHW_SURFACES|SHW_GRIDPOINTS|SHW_SOURCES|SHW_CHORDS|SHW_3DCHORDS);
        if (p!=d->obj) {
          UnhighlightAll(w);
          /*Highlight(w,d->obj,0);*/
          d->obj=p;
          d->hm=!IsLocked(d->obj);
          if (GetObjType(d->obj)==T_NODE && !IsEmptyNode(d->obj)) {
            d->hm=0;
            ViewMsgEx(w,ERR_NONEMPTYNODE,NULL);
          }

          /* Inhibit delete surfaces/grid points in Carre mode */

          if (IsCarreLocked(d->obj)) {
            d->hm=0;
            ViewMsgEx(w,ERR_CARRE_INHIBITS,NULL);
          }

          SetHighlightMode(w->app,d->hm);
          if (d->hm) SetExamineMsg(w,d->obj);
          Highlight(w,d->obj,1);
        }
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      if (!d->bEnh) {
        if (!w->app->highlightMode) {
          if (IsLocked(d->obj)) SetViewMsg(w,WhyLocked(w,d->obj));
          Cancel(w->app);
        } else {
          switch(GetObjType(d->obj)) {
            case T_NODE:
              DelNode(w->app,d->obj);
              break;
            case T_ELEM:
              DelElem(w->app,d->obj);
              break;
            case T_SURFACEEX:
              DelSurfaceEx(d->obj);
              break;
            case T_GRIDPOINTEX:
              DelGridPointEx(d->obj);
              break;
            case T_SOURCE:
              DelSource(w->app,d->obj);
              break;
            case T_CHORD:
              DelChord(w->app,d->obj);
              break;
            case T_XPOINTTEST:
              DelXPointTest(w->app,d->obj);
              break;
            case T_XPOINTSEG:
              DelXPointSeg(w->app,d->obj);
              break;
            default:
              FatalError("TlRemove()-type%d:fatal error1",GetObjType(d->obj));
          }
          UndoMark(w->app);
          ViewMsgEx(w,0,NULL);
        }
      }
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      if (!d->bEnh) {
        if (d->hm) SetHighlightMode(w->app,event==TL_ENTER);
      }
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlAddElem(View w,int event,double x,double y) {
  Node n,n2;
  Elem e;
  void* d;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;


    case TL_DBLCLK:
      if (d!=NULL) break;
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      n=AddNode(w->app,x,y);
      SetHighlightMode(w->app,1);
      Highlight(w,n,1);
      d=n;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      n=HitNode(w->app,x,y,NULL);
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      if (n==NULL) n=AddNode(w->app,x,y);
      SetHighlightMode(w->app,1);
      Highlight(w,n,1);
      d=n;
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (GetObjType(d)==T_NODE) {
        n=d;
        n2=AddNode(w->app,x,y);
        d=e=AddElem(w->app,n,n2);
        Highlight(w,e,1);
        Highlight(w,e->n[2],1);
      } else {
        e=d;
        ChangeNode(w->app,e->n[2],x,y);
      }
      SetExamineMsg(w,d);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      if (GetObjType(d)==T_ELEM && w->app->highlightMode) {
        UnhighlightAll(w);
        UndoMark(w->app);
        ViewMsgEx(w,0,NULL);
      } else Cancel(w->app);
      d=NULL;
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,event==TL_ENTER);
      break;
    case TL_CANCEL:
      d=NULL;
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlSplitElem(View w,int event,double x,double y) {
  struct {
    Elem e;
    Node n;
  }* d;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->n=NULL;
      d->e=HitElem(w->app,x,y,NULL,NULL);
      if (d->e==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NOELEMS));
        d=Free(d);
        break;
      }
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      SetHighlightMode(w->app,!IsLocked(d->e));
      if (IsLocked(d->e)) {
        Highlight(w,d->e,1);
        SetViewMsg(w,WhyLocked(w,d->e));
      } else {
        d->n=AddNode(w->app,x,y);
        AddElem(w->app,d->e->n[1],d->n);
        AddElem(w->app,d->n,d->e->n[2]);
        d->e=DelElem(w->app,d->e);
        HighlightDragNode(w,d->n,1);
      }
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->n==NULL) break;
      ChangeNode(w->app,d->n,x,y);
      SetExamineMsg(w,d->n);
      break;
    case TL_RELEASE:
      if (d==NULL) break;

      if (w->app->highlightMode) {
        UnhighlightAll(w);
        ViewMsgEx(w,0,NULL);
        UndoMark(w->app);
      } else Cancel(w->app);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      if (d->n==NULL) Highlight(w,d->e,event==TL_ENTER); else
        HighlightDragNode(w,d->n,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlJoinElems(View w,int event,double x,double y) {
  Node n,n1;
/*  Elem e; */
  int err;

  n=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (n!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_PRESS:
      if (n!=NULL) break;
      n=HitNode(w->app,x,y,NULL);
      if (n==NULL) {SetViewMsg(w,GetStr(w,ERR_NONODES));break;}
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      SetHighlightMode(w->app,JoinElems(NULL,n,NULL)!=NULL);
      HighlightDragNode(w,n,1);
      break;
    case TL_MOTION:
      if (n==NULL) break;
      n1=HitNode(w->app,x,y,NULL);
      if (n1==NULL || n1==n) break;
      HighlightDragNode(w,n,0);
      n=n1;
      SetHighlightMode(w->app,JoinElems(NULL,n,NULL)!=NULL);
      HighlightDragNode(w,n,1);
      SetExamineMsg(w,n);
      break;
    case TL_RELEASE:
      if (n==NULL) break;
      UnhighlightAll(w);
      if (w->app->highlightMode) {
        JoinElems(w->app,n,NULL);
        UndoMark(w->app);
      } else {
        JoinElems(NULL,n,&err);
        SetViewMsg(w,GetStr(w,err));
      }
      n=NULL;
      if (w->app->highlightMode) ViewMsgEx(w,0,NULL);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (n==NULL) break;
      HighlightDragNode(w,n,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (n==NULL) break;
      n=NULL;
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=n;
}

void TlConnectPoints(View w,int event,double x,double y) {
  struct {
    Node n;
    Elem e;
  }* d;
  Node n1;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->n=HitNode(w->app,x,y,NULL);
      if (d->n==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NONODES));
        d=Free(d);
        break;
      }
      d->e=NULL;
      SetViewFlags(w,w->showFlags|SHW_ELEMS);
      SetHighlightMode(w->app,1);
      Highlight(w,d->n,1);
      SetExamineMsg(w,d->n);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      n1=HitNode(w->app,x,y,NULL);
      if (n1==NULL || n1==d->n) break;
      if (d->e==NULL) {
        if (NodesConnected(d->n,n1)) break;
        Highlight(w,d->n,0);
        d->e=AddElem(w->app,d->n,n1);
        d->n=n1;
        Highlight(w,d->e,1);
        Highlight(w,d->n,1);
      } else {
        if (n1==d->e->n[1] || NodesConnected(d->e->n[1],n1)) break;
        Highlight(w,d->n,0);
        ChangeElem(w->app,d->e,2,d->n=n1);
        Highlight(w,d->n,1);
      }
      if (d->e!=NULL) SetExamineMsg(w,d->e);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      UndoMark(w->app);
      d=Free(d);
      if (w->app->highlightMode) ViewMsgEx(w,0,NULL);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      Highlight(w,d->n,event==TL_ENTER);
      if (d->e!=NULL) Highlight(w,d->e,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlRepositionElem(View w,int event,double x,double y) {
  struct {
    int bEnh,bMoved,hm;
    Elem e;
    Separator sep;
    Node n;
    double x0,y0,tx,ty;
    int i;
    ViewShape vl;
  }* d;
  Node n;
  double distE,distSep;

  /* Template part */

  double h,scale,angle1,angle2,
    xa,ya,xb,yb,   /* Template coords of both handles */
    x1,y1,x2,y2;   /* Screen coords of both handles */
  int i;
  struct _XY xy;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;

      if (w->app->template==NULL) {
        ViewMsgEx(w,ERR_NOTEMPLATE,NULL);
        break;
      }

      d=Malloc(sizeof(*d));
      d->bEnh=1;d->bMoved=0;d->hm=1;
      d->n=NULL;
      d->vl=NULL;

      if (GetNearestTemplatePoint(w->app->template,x,y,&d->tx,&d->ty,
          &d->x0,&d->y0)) {
        ViewMsgEx(w,ERR_EMPTY_TEMPLATE,NULL);
        d=Free(d);
        break;
      }

      if (w->app->template->dragStatus && d->tx==w->app->template->dragX &&
          d->ty==w->app->template->dragY) {
        ViewMsgEx(w,ERR_REPOS_TEMPLATE_1,NULL);
        d=Free(d);
        break;
      }

      AddViewShapeLine(w,d->x0-w->nodeR/w->zoomX,d->y0,
          d->x0,d->y0-w->nodeR/w->zoomY);
      AddViewShapeLine(w,d->x0-w->nodeR/w->zoomX,d->y0,
          d->x0,d->y0+w->nodeR/w->zoomY);
      AddViewShapeLine(w,d->x0+w->nodeR/w->zoomX,d->y0,
          d->x0,d->y0-w->nodeR/w->zoomY);
      AddViewShapeLine(w,d->x0+w->nodeR/w->zoomX,d->y0,
          d->x0,d->y0+w->nodeR/w->zoomY);

      SetViewFlags(w,w->showFlags|SHW_CHORDS|SHW_3DCHORDS|SHW_TEMPLATE);
      SetHighlightMode(w->app,d->hm);

      ViewMsgEx(w,w->app->template->dragStatus?
          MSG_REPOS_HANDLE_2 : MSG_REPOS_HANDLE_1,NULL);

      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=d->bMoved=0;
      d->e=HitElem(w->app,x,y,&d->i,&distE);
      d->sep=HitSeparator(w->app,x,y,&distSep);

      if (d->e==NULL && d->sep==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NOELEMS));
        d=Free(d);
        break;
      }
      if (d->e!=NULL && d->sep!=NULL)
        if (distE<distSep) d->sep=NULL; else d->e=NULL;

      SetViewFlags(w,w->showFlags|SHW_ELEMS|SHW_SEPARATORS);
      if (d->e!=NULL) {
        d->hm=!IsLocked(d->e);
        d->i=abs(d->i);
        Highlight(w,d->e,1);
        Highlight(w,d->e->n[d->i],1);
      } else {
        d->hm=!IsLocked(d->sep);
        Highlight(w,d->sep,1);
        Highlight(w,d->sep->n,1);
      }
      SetHighlightMode(w->app,d->hm);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->bEnh) {
        if (!d->bMoved) {
          d->bMoved=1;
        }

        n=HitNode(w->app,x,y,NULL);
        if (n!=d->n) {
          if (d->n!=0) Highlight(w,d->n,0);
          d->n=n;
          Highlight(w,d->n,1);
          if (d->vl!=NULL) DelViewShape(w,d->vl);
          d->vl=AddViewShapeLine(w,d->x0,d->y0,d->n->x,d->n->y);
        }
      } else {
        n=HitNode(w->app,x,y,NULL);
        if (d->e!=NULL) {
          if (n==NULL||n==d->e->n[3-d->i]||
              NodesConnected(n,d->e->n[3-d->i]))
            break;
          if (!w->app->highlightMode) {
            UnhighlightAll(w);
            if (d->e!=NULL) SetViewMsg(w,WhyLocked(w,d->e)); else
            if (d->sep!=NULL) SetViewMsg(w,WhyLocked(w,d->sep));
            d=Free(d);
            break;
          }
          Highlight(w,d->e->n[d->i],0);
          ChangeElem(w->app,d->e,d->i,n);
          Highlight(w,d->e->n[d->i],1);
        } else {
          if (n==NULL || n==d->sep->n) break;
          Highlight(w,d->sep->n,0);
          ChangeSeparator(w->app,d->sep,n);
          Highlight(w,d->sep->n,1);
        }
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      if (d->bEnh) {
        if (d->bMoved && d->n!=NULL) {
          d->x0=d->n->x-d->x0;
          d->y0=d->n->y-d->y0;
          if (w->app->template->dragStatus) {

            /* Apply formulas */

            xa=w->app->template->dragX;
            ya=w->app->template->dragY;
            xy.x=xa;xy.y=ya;
            x1=TemplateXY2X(w->app->template,&xy);
            y1=TemplateXY2Y(w->app->template,&xy);
            xb=d->tx;
            yb=d->ty;
            x2=d->n->x;
            y2=d->n->y;

            i=PlaceTemplateByHandles(w->app,xa,ya,x1,y1,xb,yb,x2,y2);
            if (!i) SetTemplateDragStatus(w->app,0,0,0);
          } else {
            i=PlaceTemplateByHandles(w->app,d->tx,d->ty,d->n->x,d->n->y,
                d->tx,d->ty,d->n->x,d->n->y);
            if (!i) SetTemplateDragStatus(w->app,1,d->tx,d->ty);
          }

          if (i) Cancel(w->app);
          else UndoMark(w->app);

          ViewMsgEx(w,i,NULL);
        } else {
          Cancel(w->app);
          ViewMsgEx(w,MSG_CANCELED,NULL);
        }
        d=Free(d);
      } else {
        UndoMark(w->app);
        d=Free(d);
        if (w->app->highlightMode) ViewMsgEx(w,0,NULL);
      }
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,d->hm && event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      UnhighlightAll(w);
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlAddSurface(View w,int event,double x,double y) {
  struct {
    int bEnh,hm;
    SurfaceEx s;
  }* d;
  int i,r;
  SurfaceZone sz;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      if (w->app->equil==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NOEQUIL));
        break;
      }

      SetViewFlags(w,w->showFlags|SHW_SURFACES);

      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->s=AddSurfaceExXY(w->app,x,y,&i);

      if (d->s==NULL) {
        SetViewMsg(w,GetStr(w,i));
        d=Free(d);
        break;
      }

      d->hm=1;

      SetHighlightMode(w->app,d->hm);
      Highlight(w,d->s,1);
      if (d->hm) SetExamineMsg(w,d->s);
      else SetViewMsg(w,GetStr(w,i));   /* Should never happen */
     break;
    case TL_PRESS:
      if (d!=NULL) break;
      if (w->app->equil==NULL) {
        SetViewMsg(w,GetStr(w,ERR_NOEQUIL));
        break;
      }

      SetViewFlags(w,w->showFlags|SHW_SURFACES);

      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->s=AddSurfaceExByXY(w->app,x,y,&i);

      if (d->s==NULL) {
        SetViewMsg(w,GetStr(w,i));
        d=Free(d);
        break;
      }

      d->hm=d->s->zone!=SZN_XY && d->s->bCoordsOk;
      if (SurfaceZoneBoundsCached(w->app)) {
        ViewMsgEx(w,MSG_CALCULATING_SZ_BOUNDS,"");
        FlushView(w);
       }

      sz=FindSurfaceZone(w->app,d->s->zone);
      if (d->hm && w->app->outputMode==OUTPUTMODE_CARRE &&
          sz!=NULL && CountSurfaces(w->app,d->s->zone)>
          !!(sz->flags & SZF_LIMITBYSURFACE)) {
        d->hm=0;
        i=ERR_CARRE_INHIBITS;
      }

      SetHighlightMode(w->app,d->hm);
      Highlight(w,d->s,1);
      if (d->hm) SetExamineMsg(w,d->s);
      else SetViewMsg(w,GetStr(w,i));
      break;
    case TL_MOTION:
      if (d==NULL) break;

      if (d->bEnh) {
        r=ChangeSurfaceExXY(d->s,x,y);
        SetExamineMsg(w,d->s);
      } else {

        if (d->s==NULL) break;
        r=ChangeSurfaceExByXY(d->s,x,y,&i);

        d->hm=d->s->zone!=SZN_XY && d->s->bCoordsOk;
        sz=FindSurfaceZone(w->app,d->s->zone);
        if (d->hm && w->app->outputMode==OUTPUTMODE_CARRE &&
            sz!=NULL && CountSurfaces(w->app,d->s->zone)>
            !!(sz->flags & SZF_LIMITBYSURFACE)) {
          d->hm=0;
          i=ERR_CARRE_INHIBITS;
        }


        SetHighlightMode(w->app,d->hm);

        if (!d->hm) SetViewMsg(w,GetStr(w,i));
        else SetExamineMsg(w,d->s);
      }

      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      /*if (d->bEnh) {
        assert(0);
      } else { */

      if (d->hm) {
        UndoMark(w->app);
        ViewMsgEx(w,0,NULL);
      } else {
        Cancel(w->app);
      }
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,d->hm && event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      /*if (d->bEnh) {
        assert(0);
      } else { */

      Cancel(w->app);
      d=Free(d);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlAddGridPoint(View w,int event,double x,double y) {
  GridPointEx d;
  int zone;
  double value;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_PRESS:
      if (d!=NULL) break;
/*      if (w->app->xpoint==NULL) { -- $
        SetViewMsg(w,GetStr(w,ERR_NOXPOINT));
        break;
      } */
      if (w->app->outputMode==OUTPUTMODE_CARRE) {
        SetViewMsg(w,GetStr(w,ERR_CARRE_INHIBITS));
        break;
      }

      if (HitGridPointExPos(w->app,x,y,&zone,&value)) break; /* $ - error msg needed */
      SetViewFlags(w,w->showFlags|SHW_GRIDPOINTS);
      d=AddGridPointEx(w->app,zone,value);
      SetHighlightMode(w->app,1);
      Highlight(w,d,1);
      SetExamineMsg(w,d);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (HitGridPointExPos(w->app,x,y,&zone,&value)) break;
      ChangeGridPointEx(d,zone,value);
      SetExamineMsg(w,d);
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      d=NULL;
      UndoMark(w->app);
      if (w->app->highlightMode) ViewMsgEx(w,0,NULL);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      Highlight(w,d,event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=NULL;
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlMirrorNormals(View w,int event,double x,double y) {
  struct {
    int bEnh,hm,type;
    Elem e;
    Chord ch;
    Group g;
  }* d;
  Elem e1;
  Chord ch1;
  Index ix;
  int i;
  void* obj;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      /* reverses entire chain of elements */
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=1;
      d->hm=1;
      d->e=NULL;
      e1=HitViewObject(w,x,y,SHW_ELEMS);
      if (e1==NULL) {
        d=Free(d);
        break;
      }
      d->g=GetElemChain(e1,NULL,NULL);

      for (e1=Group1st(d->g,&ix);e1!=NULL;e1=Next(&ix)) {
        Highlight(w,e1,1);
        if (IsLocked(e1)) {
          d->hm=0;
          d->e=e1;
        }
      }
      if (d->hm) for (e1=Group1st(d->g,&ix);e1!=NULL;e1=Next(&ix))
        RevertElem(w->app,e1);

      SetViewFlags(w,w->showFlags | SHW_ELEMS|SHW_NORMALS);
      SetHighlightMode(w->app,d->hm);
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->hm=1;
      obj=HitViewObject(w,x,y,SHW_ELEMS|SHW_CHORDS);
      if (obj==NULL) {
        d=Free(d);
        break;
      }
      d->type=GetObjType(obj);
      assert(d->type==T_ELEM || d->type==T_CHORD);
      d->g=CreateGroup();
      GroupAdd(d->g,obj);
      SetViewFlags(w,w->showFlags | SHW_ELEMS|SHW_CHORDS|SHW_NORMALS);
      d->hm=!IsLocked(obj);
      SetHighlightMode(w->app,d->hm);
      Highlight(w,obj,1);
      if (!w->app->highlightMode) break;
      if (d->type==T_ELEM) {
	d->e=obj;
	RevertElem(w->app,d->e);
      }
      else {
	d->ch=obj;
	ChangeChord3D(w->app,d->ch,d->ch->x2,d->ch->y2,d->ch->z2,
          d->ch->x1,d->ch->y1,d->ch->z1);
      }
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (d->bEnh) {
      } else {
        if (!w->app->highlightMode) break;
	if (d->type==T_ELEM) {
	  e1=HitElem(w->app,x,y,NULL,NULL);
	  if (e1==NULL || e1==d->e || IsLocked(e1)) break;
	  i=ElemsConnected(d->e,e1);
	  if (!i) break;
	  if (IsRegularNode(e1->n[1],d->g)) break;
	  if (IsRegularNode(e1->n[2],d->g)) break;
	  d->e=e1;
	  if (InGroup(d->g,d->e)) break;
	  Highlight(w,d->e,1);
	  GroupAdd(d->g,d->e);
	  if (i==2) RevertElem(w->app,d->e);
	} else {
	  ch1=HitChord(w->app,x,y,NULL,NULL);
	  if (ch1==NULL || ch1==d->ch || IsLocked(ch1)) break;
	  d->ch=ch1;
	  if (InGroup(d->g,d->ch)) break;
	  Highlight(w,d->ch,1);
	  GroupAdd(d->g,d->ch);
	  ChangeChord3D(w->app,d->ch,d->ch->x2,d->ch->y2,d->ch->z2,
            d->ch->x1,d->ch->y1,d->ch->z1);
	}
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      d->g=FreeGroup(d->g);
      if (!d->hm) {
        Cancel(w->app);
        SetViewMsg(w,WhyLocked(w,d->e));
      } else {
        UndoMark(w->app);
        ViewMsgEx(w,0,NULL);
      }
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      SetHighlightMode(w->app,d->hm && event==TL_ENTER);
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d->g=FreeGroup(d->g);
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

void TlAddSource(View w,int event,double x,double y) {
  struct {
    int bEnh;
    Source src;
  }* d;

  d=w->toolData;
  switch(event) {
    case TL_DISABLE:
      if (d!=NULL) FatalError("Tool()-disable0: fatal error 2");
    case TL_ENABLE:
      break;
    case TL_DBLCLK:
      if (d!=NULL) break;
      break;
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->src=AddSource(w->app,x,y);
      SetViewFlags(w,w->showFlags | SHW_SOURCES);
      SetHighlightMode(w->app,1);
      Highlight(w,d->src,1);
      SetExamineMsg(w,d->src);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (!d->bEnh) {
        ChangeSource(w->app,d->src,x,y);
        SetExamineMsg(w,d->src);
      }
      break;
    case TL_RELEASE:
      if (d==NULL) break;
      UnhighlightAll(w);
      if (!d->bEnh) {
        UndoMark(w->app);
      }
      if (w->app->highlightMode) ViewMsgEx(w,0,NULL);
      d=Free(d);
      break;
    case TL_ENTER:
    case TL_LEAVE:
      if (d==NULL) break;
      if (!d->bEnh) {
        Highlight(w,d->src,event==TL_ENTER);
      }
      break;
    case TL_CANCEL:
      if (d==NULL) break;
      d=Free(d);
      Cancel(w->app);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

