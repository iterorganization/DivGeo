#include "dg.h"

typedef struct _ChangeChordRec {
  ActProc actProc;
  void* obj;
  Chord ch;
  double x1,y1,x2,y2,z1,z2;
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
          if (~w->showFlags & SHW_TOPVIEW)
	    ChangeChord(w->app,d->ch,d->ch->x1,d->ch->y1,x,y);
          else ChangeChord(w->app,d->ch,d->ch->x1,d->ch->z1,x,y);
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

/* extends the chord so that its second endpoint hits the closest element in the set of marked elements, or all elements if none are marked. */
void TlExtChord(View w,int event,double x,double y) {
  struct {
    int bEnh,hm;
    Group g;
    Chord ch;
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
    case TL_PRESS:
      if (d!=NULL) break;
      d=Malloc(sizeof(*d));
      d->bEnh=0;
      d->hm=1;
      d->ch=NULL;
      SetViewFlags(w,w->showFlags | SHW_CHORDS|SHW_3DCHORDS);
      p=HitViewObject(w,x,y,SHW_CHORDS|SHW_3DCHORDS);
      if (p==NULL) {
        d=Free(d);
        break;
      }

    case TL_MOTION:
      if (d==NULL) break;
      if (d->bEnh) {
      } else {
        if (d->ch!=NULL) p=HitViewObject(w,x,y,SHW_CHORDS|SHW_3DCHORDS);

        if (p==NULL || p==d->ch) break;
        d->ch=p;

        UnhighlightAll(w);
        SetHighlightMode(w->app,1);
      }
      break;

    case TL_RELEASE:
      if (d==NULL) break;
      if (d->bEnh) {
      } else {
        if (d->ch!=NULL) /* eventually replace this with an error thing */
	  if (ExtendChord(w->app,d->ch))
	    SetViewMsg(w,GetStr(w,ERR_NOELEMSINPATH));
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
      }
      d=Free(d);
      ViewMsgEx(w,MSG_CANCELED,NULL);
      Cancel(w->app);
      break;
  }
  FlushView(w);
  w->toolData=d;
}

/* moves chord while preserving ch->x1^2+ch->z1^2 and ch->x2^2+ch->z2^2. */
void TlAdjustChord(View w,int event,double x,double y) {
  struct {
    int bEnh,ptNum,hm;
    Chord ch;
    double x,y,r;
  }* d;
  Chord ch;
  double factor;
  int r;

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
      d->ch=ch=HitChord(w->app,x,y,&d->ptNum,NULL);
      if (d->ch==NULL) {
        d=Free(d);
        break;
      }
      d->x=x;
      d->y=y;
      d->r=(abs(d->ptNum)==1) ? hypot(ch->x1,ch->z1) : hypot(ch->x2,ch->z2);
      SetHighlightMode(w->app,d->hm=!IsLocked(d->ch));
      Highlight(w,d->ch,1);

      if (d->hm) SetExamineMsg(w,d->ch);
      break;
    case TL_MOTION:
      if (d==NULL) break;
      if (!d->bEnh) {
        r=0;
	if (!w->app->highlightMode) break;
	ch=d->ch;
	if (~w->showFlags & SHW_TOPVIEW) {
	  if (abs(d->ptNum)==1) ChangeChord(w->app,ch,ch->x1,y,ch->x2,ch->y2);
	  else ChangeChord(w->app,ch,ch->x1,ch->y1,ch->x2,y);
	  SetExamineMsg(w,d->ch);
	} else {
	  factor=d->r/hypot(x,y);
	  if (abs(d->ptNum)==1)
	    ChangeChord(w->app,ch,factor*x,factor*y,ch->x2,ch->z2);
	  else ChangeChord(w->app,ch,ch->x1,ch->z1,factor*x,factor*y);
	  SetExamineMsg(w,d->ch);
	}
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

/* in normal view, sets x-y coordinates.
   in top view, sets x-z coordinates, with "y" = z. */
Chord AddChord(App a,double x1,double y1,double x2,double y2) {
  Chord ch;
  struct _ActRec ar;

  if (x1==x2 && y1==y2) return NULL;

  ch=Malloc(sizeof(*ch));

  ch->type=T_CHORD;
  ch->locks=0;
  ch->x1=x1;
  ch->x2=x2;
  if (a->activeAppView && (a->activeAppView->showFlags & SHW_TOPVIEW)) {
    ch->y1=0;
    ch->y2=0;
    ch->z1=y1;
    ch->z2=y2;
  } else {
    ch->y1=y1;
    ch->y2=y2;
    ch->z1=0;
    ch->z2=0;
  }
  ch->vars=ch->varsContaining=NULL;

  ar.obj=ch;
  ActAddChord(a,&ar);

  return ch;
}

/* in normal view, changes x-y coordinates.
   in top view, changes x-z coordinates, with "y" = z. */
int ChangeChord(App a,Chord ch,double x1,double y1,double x2,double y2) {
  struct _ChangeChordRec ar;

  ar.ch=ch;
  ar.x1=x1;
  ar.x2=x2;
  if (~a->activeAppView->showFlags & SHW_TOPVIEW) {
    ar.y1=y1;
    ar.y2=y2;
    ar.z1=ch->z1;
    ar.z2=ch->z2;
  } else {
    ar.y1=ch->y1;
    ar.y2=ch->y2;
    ar.z1=y1;
    ar.z2=y2;
  }

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

/* general add and change functions, impossible to specify by mouse */

Chord AddChord3D(App a,double x1,double y1,double z1,double x2,double y2,
    double z2) {
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
  ch->z1=z1;
  ch->z2=z2;
  ch->vars=ch->varsContaining=NULL;

  ar.obj=ch;
  ActAddChord(a,&ar);

  return ch;
}

int ChangeChord3D(App a,Chord ch,double x1,double y1,double z1,double x2,
    double y2,double z2) {
  struct _ChangeChordRec ar;

  ar.ch=ch;
  ar.x1=x1;
  ar.x2=x2;
  ar.y1=y1;
  ar.y2=y2;
  ar.z1=z1;
  ar.z2=z2;

  ActChangeChord(a,&ar);

  return 0;
}

#define CHORD_RES 20

/* drawing subroutine generalizes 2D procedure */
static void DrawChord3D(View w,Chord ch) {
  double i,x1,y1,x2,y2;

  if (~w->showFlags & SHW_TOPVIEW) {
    if (w->showFlags & SHW_CHORDS)
      DrawViewLine(w,ch->x1,ch->y1,ch->x2,ch->y2);
    if (w->showFlags & SHW_3DCHORDS) {
      if (ch->z1==0 && ch->z2==0) return;
      x1=hypot(ch->x1,ch->z1);y1=ch->y1;
      for (i=1./CHORD_RES;i<1;i+=1./CHORD_RES) {
	x2=hypot(ch->x1+i*(ch->x2-ch->x1),ch->z1+i*(ch->z2-ch->z1));
	y2=ch->y1+i*(ch->y2-ch->y1);
	DrawViewLine(w,x1,y1,x2,y2);
	x1=x2;y1=y2;
      }
      x2=hypot(ch->x2,ch->z2);y2=ch->y2;
      DrawViewLine(w,x1,y1,x2,y2);
    }
  } else if (w->showFlags & (SHW_CHORDS | SHW_3DCHORDS))
    DrawViewLine(w,ch->x1,ch->z1,ch->x2,ch->z2);
}

void DrawChord(View w,Chord ch,int mode) {
  if (!(w->showFlags & (SHW_CHORDS | SHW_3DCHORDS))) return;

  if (w->showFlags & SHW_NORMALS) DrawNormal(w,ch,mode);
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
    DrawChord3D(w,ch);
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
  DrawChord3D(w,ch);
}

/*#define EXTCHORD_2D*/
#define EXTCHORD_OLD
#ifdef EXTCHORD_2D

int ExtendChord(App a,Chord ch) {
  double r1,r2,d,d0,d1,ra,rb;
  void* obj;
  Elem e;
  Group g;
  Index ix,ixe;

  g=CreateGroup();

  for (obj=AppMark1st(a,&ix);obj!=NULL;obj=Next(&ix))
    if (GetObjType(obj)==T_ELEM) GroupAdd(g,obj);
  if (IsEmptyGroup(g))  g=CopyGroup(a->elems,g);

  if (ch->x1*ch->z2==ch->x2*ch->z1) {
    r1=hypot(ch->x1,ch->z1);
    r2=hypot(ch->x2,ch->z2);
    d0=Point2PointDist(r1,ch->y1,r2,ch->y2);

    d1=MAXDOUBLE;
    for (e=Group1st(g,&ixe);e!=NULL;e=Next(&ixe)) {
      VIntersect(r1,ch->y1,r2,ch->y2,
        e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,&ra,&rb);
      if (ra<0 || rb<0 || rb>1) continue; /* the element does not intersect */
      d=Point2PointDist(r1,ch->y1,r1+(r2-r1)*ra,
        ch->y1+(ch->y2-ch->y1)*ra);
      if (d<d1) d1=d;
    }
    FreeGroup(g);

    if (d1==MAXDOUBLE) return ERR_NOELEMSINPATH;

    ChangeChord3D(a,ch,ch->x1,ch->y1,ch->z1,ch->x1+d1/d0*(ch->x2-ch->x1),
      ch->y1+d1/d0*(ch->y2-ch->y1),ch->z1+d1/d0*(ch->z2-ch->z1));
  } else SetViewMsg(a->activeAppView,"3D chord extension disabled.");
  return 0;
}

#elif defined EXTCHORD_OLD

/* ignores horizontal elements. parallel case not yet tested. */
int ExtendChord(App a,Chord ch) {
  double x,y,z,r1,r2,y2,dy,x0,y0,z0,xe,ye;
  double d,d0,d1,ra,rb;
  double A,B,C,w,D,yPos,yNeg,yMin,yMax;
  Group g;
  void* obj;
  Elem e;
  Index ix,ixe;

  g=CreateGroup();

  for (obj=AppMark1st(a,&ix);obj!=NULL;obj=Next(&ix))
    if (GetObjType(obj)==T_ELEM) GroupAdd(g,obj);
  if (IsEmptyGroup(g))  g=CopyGroup(a->elems,g);

  x0=ch->x1; y0=ch->y1; z0=ch->z1;
  A=ch->x2 - ch->x1;
  B=ch->y2 - ch->y1;
  C=ch->z2 - ch->z1;
  if (B==0) {
    SetViewMsg(a->activeAppView,"Cannot extend a horizontal chord.");
    return 0;
  }

  dy=MAXDOUBLE;
  for (e=Group1st(g,&ixe);e!=NULL;e=Next(&ixe)) {
    xe=e->n[1]->x; ye=e->n[1]->y;
    w=(e->n[2]->x - e->n[1]->x)/(e->n[2]->y - e->n[1]->y);

    /* the constraints used to obtain y are:
       (x - x0)/A = (y - y0)/B = (z - z0)/C, from the chord line, and
       x^2 + z^2 = (xe + w*(y - ye))^2, for the element, along with
       the condition that y lie between e->n[1]->y and e->n[2]->y.
       x and z are eliminated, to be calculated by the line equation
       only after the final value of y has been determined.
     */

    D=((-C*C)*(x0 + xe + w*y0 - w*ye)*(x0 - xe - w*y0 + w*ye)
       + (B*w*x0 - A*(xe + w*y0 - w*ye))*(B*w*x0 - A*(xe + w*y0 - w*ye))
       + 2*C*(A*x0 - B*w*(xe + w*y0 - w*ye))*z0
       + ((-A*A) + B*B*w*w)*z0*z0);
    if (D<0) continue;
    D=B*sqrt(D);
    yPos=((-A)*B*x0 + A*A*y0 + C*C*y0 + B*B*w*((xe - w*ye)) - B*C*z0 + D)/
      (A*A + C*C - B*B*w*w);
    yNeg=((-A)*B*x0 + A*A*y0 + C*C*y0 + B*B*w*((xe - w*ye)) - B*C*z0 - D)/
      (A*A + C*C - B*B*w*w);

    yMin=e->n[1]->y; yMax=e->n[2]->y;
    if (yMin>yMax) swap(yMin,yMax);
    if (inrange(yPos,e->n[1]->y,e->n[2]->y))
      y=inrange(yNeg,e->n[1]->y,e->n[2]->y) ?
	(fabs(yPos - ch->y1) < fabs(yNeg - ch->y1) ? yPos : yNeg) : yPos;
    else if (inrange(yNeg,e->n[1]->y,e->n[2]->y)) y=yNeg;
    else continue;

    if (fabs(y-ch->y1)<dy && sign(y-ch->y1)==sign(B)) dy=fabs(y-ch->y1);
  }
  FreeGroup(g);

  if (dy==MAXDOUBLE) return ERR_NOELEMSINPATH;

  y2=ch->y1+dy/fabs(B)*(ch->y2-ch->y1);
  ChangeChord3D(a,ch,ch->x1,ch->y1,ch->z1,ch->x1+A/B*(y2-ch->y1),y2,
    ch->z1+C/B*(y2-ch->y1));
  return 0;
}

#else

/* minimum neighborhood surrounding the chord's first endpoint. */
#define VIEW_NBRHOOD 0.001

/* may not work for elements that are exactly parallel to ch */
int ExtendChord(App a,Chord ch) {
  double x,y,z,r,x2,y2,dx,dy,x0,y0,z0,xe,ye;
  double d,d0,d1,pos,neg;
  double A,B,C,w,D,rangeMin,rangeMax;
  Group g;
  void* obj;
  Elem e,e1;
  Index ix,ixe;

  g=CreateGroup();

  for (obj=AppMark1st(a,&ix);obj!=NULL;obj=Next(&ix))
    if (GetObjType(obj)==T_ELEM) GroupAdd(g,obj);
  if (IsEmptyGroup(g))  g=CopyGroup(a->elems,g);

  x0=ch->x1; y0=ch->y1; z0=ch->z1;
  A=ch->x2 - ch->x1;
  B=ch->y2 - ch->y1;
  C=ch->z2 - ch->z1;
  d0=sqrt(A*A+B*B+C*C);

  /* find the distance at which the chord hits the element, if it exists */
  d1=MAXDOUBLE;
  if (B==0) { /* horizontal chord */
    if (A==0 && C==0) return -1; /* chord has length 0 */
    for (e=Group1st(g,&ixe);e!=NULL;e=Next(&ixe)) {
      xe=e->n[1]->x; ye=e->n[1]->y;

      if (e->n[1]->y==e->n[2]->y) continue; /* chord is parallel to element */
      else {
	w=(e->n[2]->x - e->n[1]->x)/(e->n[2]->y - e->n[1]->y);

	rangeMin=e->n[1]->y; rangeMax=e->n[2]->y;
	if (rangeMin>rangeMax) swap(rangeMin,rangeMax);
	if (y0<rangeMin || y0>rangeMax) continue; /* y-range out of bounds */

	r=e->n[1]->x+w*(y0-e->n[1]->y);
	D=C*C*(r-x0)*(r+x0) + 2*A*C*x0*z0 + A*A*(r-z0)*(r+z0);
	if (D<0) continue; /* does not intersect element's cone */

	rangeMin=e->n[1]->x; rangeMax=e->n[2]->x;
	if (rangeMin>rangeMax) swap(rangeMin,rangeMax);
	if (A==0) {
	  D=C*sqrt(D);
	  pos=A*A*z0 - A*C*x0 + D;
	  pos=A*A*z0 - A*C*x0 - D;

	  if (rangeMin<=pos && pos<=rangeMax)
	    z=rangeMin<=neg && neg<=rangeMax ?
	      (fabs(pos - z0) < fabs(neg - z0) ? pos : neg) : pos;
	  else if (rangeMin<=neg && neg<=rangeMax) z=neg;
	  else continue; /* z-range out of bounds */
	  d=d0/C*(z-ch->z1);
	  puts("horizontal chord, A==0");
	}
	else {
	  D=A*sqrt(D);
	  pos=(C*C*x0 - A*C*z0 + D)/(A*A + C*C);
	  neg=(C*C*x0 - A*C*z0 - D)/(A*A + C*C);

	  if (rangeMin<=pos && pos<=rangeMax)
	    x=rangeMin<=neg && neg<=rangeMax ?
	      (fabs(pos - x0) < fabs(neg - x0) ? pos : neg) : pos;
	  else if (rangeMin<=neg && neg<=rangeMax) x=neg;
	  else continue; /* x-range out of bounds */
	  puts("horizontal chord, A!=0");
	  d=d0/A*(x-ch->x1);
	}
      }
    }
  }
  else {
    for (e=Group1st(g,&ixe);e!=NULL;e=Next(&ixe)) {
      xe=e->n[1]->x; ye=e->n[1]->y;

      if (e->n[1]->y==e->n[2]->y) { /* horizontal element */
	/*printf("%d: horizontal\n",e->id);*/
	r=hypot(x0*B + A*(ye-y0),z0*B + C*(ye-y0))/fabs(B);
	rangeMin=e->n[1]->x; rangeMax=e->n[2]->x;
	if (rangeMin>rangeMax) swap(rangeMin,rangeMax);
	if (r<rangeMin || r>rangeMax) continue; /* r-range out of bounds */
	/*printf("  r = %g\n",r);*/

	if (A==0 && C==0) d=sign(B)*(ye-y0);
	else d=(A==0 ? sign(C) : sign(A))*d0/hypot(A,C)*(r-hypot(x0,z0));
	puts("horizontal element");
	/*printf("  distance = %g\n",d);*/
      }
      else {
	/* in the following system, the constraints used to obtain y are:
	   (x - x0)/A = (y - y0)/B = (z - z0)/C, from the chord line, and
	   x^2 + z^2 = (xe + w*(y - ye))^2, for the element, along with
	   the condition that y lie between e->n[1]->y and e->n[2]->y.
	   x and z are eliminated, to be calculated by the line equation
	   only after the final value of y has been determined.
	*/

	w=(e->n[2]->x - e->n[1]->x)/(e->n[2]->y - e->n[1]->x);
	D=((-C*C)*(x0 + xe + w*y0 - w*ye)*(x0 - xe - w*y0 + w*ye)
	   + (B*w*x0 - A*(xe + w*y0 - w*ye))*(B*w*x0 - A*(xe + w*y0 - w*ye))
	   + 2*C*(A*x0 - B*w*(xe + w*y0 - w*ye))*z0
	   + ((-A*A) + B*B*w*w)*z0*z0);
	if (D<0) continue; /* does not intersect element's cone */

	if (A*A + C*C == B*B*w*w) continue; /* parallel */
	D=B*sqrt(D);
	pos=((-A)*B*x0 + A*A*y0 + C*C*y0 + B*B*w*(xe - w*ye) - B*C*z0 + D)/
	  (A*A + C*C - B*B*w*w);
	neg=((-A)*B*x0 + A*A*y0 + C*C*y0 + B*B*w*(xe - w*ye) - B*C*z0 - D)/
	  (A*A + C*C - B*B*w*w);

	rangeMin=e->n[1]->y; rangeMax=e->n[2]->y;
	if (rangeMin>rangeMax) swap(rangeMin,rangeMax);
	if (rangeMin<=pos && pos<=rangeMax)
	  y=rangeMin<=neg && neg<=rangeMax ?
	    (fabs(pos - y0) < fabs(neg - y0) ? pos : neg) : pos;
	else if (rangeMin<=neg && neg<=rangeMax) y=neg;
	else continue; /* y-range out of bounds */

	d=d0/B*(y-y0);
	printf("%d: normal %g = d0/%g*(%g - %g)\n  out of %g, %g with D=%g\n",
	       e->id,d,B,y,y0,pos,neg,D);
      }

      if (d>VIEW_NBRHOOD && d<d1) {d1=d;e1=e;}
    }
  }
  FreeGroup(g);

  if (d1==MAXDOUBLE) return ERR_NOELEMSINPATH;

  printf("%d: %g\n\n",e1->id,d1);
  ChangeChord3D(a,ch,ch->x1,ch->y1,ch->z1,ch->x1+d1/d0*(ch->x2-ch->x1),
    ch->y1+d1/d0*(ch->y2-ch->y1),ch->z1+d1/d0*(ch->z2-ch->z1));
  return 0;
}

#endif

static int ActAddChord(App a,ActRec ar) {
  DelRec ur;
  Chord ch;

  if (AppLocked(a)) return 0;

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

  if (AppLocked(a)) return 0;

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

  if (AppLocked(a)) return 0;

  assert(!ar->ch->locks);

  ur=CreateActRec(sizeof(*ur),(ActProc)ActChangeChord);
  ur->ch=ar->ch;

  ur->x1=ar->ch->x1;
  ur->y1=ar->ch->y1;
  ur->x2=ar->ch->x2;
  ur->y2=ar->ch->y2;
  ur->z1=ar->ch->z1;
  ur->z2=ar->ch->z2;

  NotifyChange(a,ar->ch);
  DrawAppHighlight(a,DRAW_OFF);

  DrawAppObject(a,ar->ch,DRAW_OFF);

  ar->ch->x1=ar->x1;
  ar->ch->y1=ar->y1;
  ar->ch->x2=ar->x2;
  ar->ch->y2=ar->y2;
  ar->ch->z1=ar->z1;
  ar->ch->z2=ar->z2;

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

  return 0;
}

Chord FindChord(App a,double x1,double y1,double x2,double y2) {
  Chord ch;
  Index ix;

  for (ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix)) {
    if ((ch->x1==x1 && ch->y1==y1 && ch->x2==x2 && ch->y2==y2) ||
        (ch->x1==x2 && ch->y1==y2 && ch->x2==x1 && ch->y2==y1))
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

