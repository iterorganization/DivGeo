#include "dg.h"

typedef struct _ChangeChordRec {
  ActProc actProc;
  void* obj;
  Chord ch;
  double x1,y1,x2,y2;
}* ChangeChordRec;

static int ActAddChord(App a,ActRec ar);
static int ActDelChord(App a,DelRec ar);
static int ActChangeChord(App a,ChangeChordRec ar);

void TlAddChord(View w,int event,double x,double y) {
  struct {
    int bEnh;
    Chord ch;
    double x;
    double y;
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
      d->ch=NULL;
      d->x=x;
      d->y=y;
      SetViewFlags(w,w->showFlags | SHW_CHORDS);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (!d->bEnh) {
	if (d->ch==NULL) {
	  d->ch=AddChord(w->app,d->x,d->y,x,y);
	  if (d->ch==NULL) break;
	  SetHighlightMode(w->app,1);
	  Highlight(w,d->ch,1);
	} else {
	  ChangeChord(w->app,d->ch,d->ch->x1,d->ch->y1,x,y);
	}
      SetExamineMsg(w,d->ch);
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
	Highlight(w,d->ch,event==TL_ENTER);
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


Chord AddChord(App a,double x1,double y1,double x2,double y2) {
  Chord ch;
  struct _ActRec ar;

  if (x1==x2 && y1==y2) return NULL;

  ch=Malloc(sizeof(*ch));

  ch->type=T_CHORD;
  ch->locks=0;
  ch->x1=x1;
  ch->y1=y1;
  ch->x2=x2;
  ch->y2=y2;
  ch->vars=ch->varsContaining=NULL;

  ar.obj=ch;
  ActAddChord(a,&ar);

  return ch;
}

int ChangeChord(App a,Chord ch,double x1,double y1,double x2,double y2) {
  struct _ChangeChordRec ar;

  ar.ch=ch;
  ar.x1=x1;
  ar.y1=y1;
  ar.x2=x2;
  ar.y2=y2;

  ActChangeChord(a,&ar);

  return 0;
}

void* DelChord(App a,Chord ch) {
  struct _DelRec ar;
  Var v;
  Index ix;

  assert(!IsHighlighted(a,ch));
  MarkObject(a,ch,0);

  for (v=Group1st(ch->vars,&ix);v!=NULL;v=Next(&ix))
    SetVar(a,ch,v->def,v->varSet,NULL);

  for (v=Group1st(ch->varsContaining,&ix);v!=NULL;v=Next(&ix))
    ChangeVarEx(a,v,ch,0);

  ar.delete=ch;

  ActDelChord(a,&ar);

  return NULL;
}

void DrawChord(View w,Chord ch,int mode) {
  double x,y,l;

  if (~w->showFlags & SHW_CHORDS) return;

  if (w->showFlags & SHW_NORMALS) {
    l=Point2PointDist(ch->x1,ch->y1,ch->x2,ch->y2);
    if (l>(double)w->normalLen/1e6) {
      switch(mode) {
	case DRAW_ON:
	  SetViewMode(w,
	    IsHighlighted(w->app,ch) ? VMX_ELEMNORMAL : VM1_ELEMNORMAL);
	  break;
	case DRAW_OFF:
	  if (IsHighlighted(w->app,ch)) {SetViewMode(w,VMX_ELEMNORMAL);break;}
	case DRAW_ERASE:
	  SetViewMode(w,VM0_ELEMNORMAL);
	  break;
      }
      x=(ch->x1+ch->x2)/2;
      y=(ch->y1+ch->y2)/2;
      DrawViewLine(w,x,y,x+(ch->y2-ch->y1)*w->normalLen/l/w->zoomX,
	y-(ch->x2-ch->x1)*w->normalLen/l/w->zoomY);
    }
  }
  if (IsMarked(w->app,ch) && !IsHighlighted(w->app,ch)) {
    switch(mode) {
      case DRAW_ON:
	SetViewMode(w,VM1_ELEMMARK);
	break;
      case DRAW_OFF:
      case DRAW_ERASE:
	SetViewMode(w,VM0_ELEMMARK);
	break;
    }
    DrawViewLine(w,ch->x1,ch->y1,ch->x2,ch->y2);
  }
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,ch) ? VMX_CHORD : VM1_CHORD);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,ch)) {SetViewMode(w,VMX_CHORD);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_CHORD);
      break;
  }
  DrawViewLine(w,ch->x1,ch->y1,ch->x2,ch->y2);
}

static int ActAddChord(App a,ActRec ar) {
  DelRec ur;
  Chord ch;

  if (AppLocked(a)) return;

  ur=CreateActRec(sizeof(*ur),(ActProc)ActDelChord);
  ur->delete=ar->obj;

  ch=ar->obj;
  ar->obj=NULL;

  NotifyAdd(a,ch);
  DrawAppHighlight(a,DRAW_OFF);

  ch->vars=CreateGroup();
  ch->varsContaining=CreateGroup();
  GroupAdd(a->chords,ch);

  DrawAppObject(a,ch,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyAdded(a,ch);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

static int ActDelChord(App a,DelRec ar) {
  ActRec ur;
  Chord ch;

  if (AppLocked(a)) return;

  ch=ar->delete;
  assert (!ch->locks);
  assert(IsEmptyGroup(ch->vars));
  assert(IsEmptyGroup(ch->varsContaining));

  ur=CreateActRec(sizeof(*ur),(ActProc)ActAddChord);
  ur->obj=ar->delete;

  NotifyDel(a,ar->delete);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ch,DRAW_ERASE);

  GroupDel(a->chords,ch);
  ch->varsContaining=FreeGroup(ch->varsContaining);
  ch->vars=FreeGroup(ch->vars);

  DrawAppHighlight(a,DRAW_ON);
  NotifyDeleted(a,ar->delete);

  AddAppUpdate(a);
  AddUndoRec(a,ur);
  return 0;
}

static int ActChangeChord(App a,ChangeChordRec ar) {
  ChangeChordRec ur;

  if (AppLocked(a)) return;

  assert(!ar->ch->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeChord);
  ur->ch=ar->ch;

  ur->x1=ar->ch->x1;
  ur->y1=ar->ch->y1;
  ur->x2=ar->ch->x2;
  ur->y2=ar->ch->y2;

  NotifyChange(a,ar->ch);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->ch,DRAW_OFF);

  ar->ch->x1=ar->x1;
  ar->ch->y1=ar->y1;
  ar->ch->x2=ar->x2;
  ar->ch->y2=ar->y2;

  DrawAppObject(a,ar->ch,DRAW_ON);

  DrawAppHighlight(a,DRAW_ON);
  NotifyChanged(a,ar->ch);

  AddAppUpdate(a);
  AddUndoRec(a,(ActRec)ur);
  return 0;
}

int ConvertElemsToChords(App a,Group elems,void** pErrObj) {
  Elem e;
  Index ix;
  Chord ch;
  double x1,y1,x2,y2;
  int i;

  if (pErrObj!=NULL) *pErrObj=NULL;

  for (e=Group1st(elems,&ix);e!=NULL;e=Next(&ix)) {
    x1=e->n[1]->x;
    y1=e->n[1]->y;
    x2=e->n[2]->x;
    y2=e->n[2]->y;
    i=IsMarked(a,e);

    if (IsLocked(e)) {
      if (pErrObj!=NULL) *pErrObj=e;
      return ERR_LOCKED;
    }

    DelElem(a,e);e=NULL;

    ch=FindChord(a,x1,y1,x2,y2);
    if (ch!=NULL) continue;
    ch=AddChord(a,x1,y1,x2,y2);
    if (ch==NULL) continue;
    if (i) MarkObject(a,ch,1);
  }
}

Chord FindChord(App a,double x1,double y1,double x2,double y2) {
  Chord ch;
  Index ix;

  for (ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix)) {
    if (ch->x1==x1 && ch->y1==y1 && ch->x2==x2 && ch->y2==y2 ||
	ch->x1==x2 && ch->y1==y2 && ch->x2==x1 && ch->y2==y1)
      return ch;
  }

  return NULL;
}

int ConvertTemplateToChords(App a) {
  XY xy,xy1;
  Index ix;

  if (a->template==NULL) return ERR_NOTEMPLATE;

  for (xy=TemplatePoint1st(a->template,&ix);xy!=NULL;xy=Next(&ix)) {
    xy1=Next(&ix);
    if (FindChord(a,xy->x,xy->y,xy1->x,xy1->y)==NULL) {
      AddChord(a,xy->x,xy->y,xy1->x,xy1->y);
    }
  }
  return 0;
}


