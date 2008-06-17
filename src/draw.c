#include "dg.h"

void DrawNode(View w,Node n,int mode) {
  if (!(w->showFlags&SHW_NODES ||
      (w->showFlags&SHW_IRRNODES && !IsRegularNode(n,NULL)))) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,n) ? VMX_NODE : VM1_NODE);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,n)) {SetViewMode(w,VMX_NODE);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_NODE);
      break;
  }
  DrawViewCircle(w,n->x,n->y,w->nodeR/w->zoomX);
}

void DrawElem(View w,Elem e,int mode) {
  double x,y,l;
  char s[10];

  if (w->showFlags & SHW_NORMALS) {
    l=Point2PointDist(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y);
    if (l>(double)w->normalLen/1e6) {
      switch(mode) {
        case DRAW_ON:
          SetViewMode(w,
            IsHighlighted(w->app,e) ? VMX_ELEMNORMAL : VM1_ELEMNORMAL);
          break;
        case DRAW_OFF:
          if (IsHighlighted(w->app,e)) {SetViewMode(w,VMX_ELEMNORMAL);break;}
        case DRAW_ERASE:
          SetViewMode(w,VM0_ELEMNORMAL);
          break;
      }
      x=(e->n[1]->x+e->n[2]->x)/2;
      y=(e->n[1]->y+e->n[2]->y)/2;
      DrawViewLine(w,x,y,x+(e->n[2]->y-e->n[1]->y)*w->normalLen/l/w->zoomX,
        y-(e->n[2]->x-e->n[1]->x)*w->normalLen/l/w->zoomY);
    }
  }
  if (w->showFlags & SHW_ELEMS) {
    if (IsMarked(w->app,e) && !IsHighlighted(w->app,e)) {
      switch(mode) {
        case DRAW_ON:
          SetViewMode(w,VM1_ELEMMARK);
          break;
        case DRAW_OFF:
        case DRAW_ERASE:
          SetViewMode(w,VM0_ELEMMARK);
          break;
      }
      DrawViewLine(w,e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y);
    }
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,IsHighlighted(w->app,e) ? VMX_ELEM : VM1_ELEM);
        break;
      case DRAW_OFF:
        if (IsHighlighted(w->app,e)) {SetViewMode(w,VMX_ELEM);break;}
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEM);
        break;
    }
    DrawViewLine(w,e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y);
  }
  if (w->showFlags & SHW_NUMBERS && !IsHighlighted(w->app,e)) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMNUMBER);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMNUMBER);
        break;
    }
    sprintf(s,"%d",e->id);
    DrawViewText(w,(e->n[1]->x+e->n[2]->x)/2,(e->n[1]->y+e->n[2]->y)/2,s);
  }
}

void DrawEquil(View w,Equil eq,int mode) {
  int i,j,h;
  double f,l,p0;

  if (~w->showFlags&SHW_EQUIL) return;
  h=!IsHighlighted(w->app,eq) && mode==DRAW_ON;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,eq) ? VMX_EQUIL : VM1_EQUIL);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,eq)) {SetViewMode(w,VMX_EQUIL);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_EQUIL);
      break;
  }
/*
  l=fabs(eq->x[eq->sx-1]-eq->x[0])/eq->sx;
*/
/*  p0=fabs((eq->maxVal-eq->minVal)*2/(eq->x[eq->sx-1]-eq->x[0]+
      eq->y[eq->sy-1]-eq->y[0])); */

  for (i=0;i<eq->sy/*-1*/;i++) for (j=0;j<eq->sx/*-1*/;j++) {
    if (mode==DRAW_ON) SetViewMode(w,EqCell(eq,j,i)>=0? VM2_EQUIL : VM1_EQUIL);
    if (j<eq->sx-1) DrawViewLine(w,eq->x[j],eq->y[i],eq->x[j+1],eq->y[i]);
    if (i<eq->sy-1) DrawViewLine(w,eq->x[j],eq->y[i],eq->x[j],eq->y[i+1]);

/*    l=(EqCell(eq,j,i+1)+EqCell(eq,j+1,i)-2*EqCell(eq,j,i))/
	(eq->y[i+1]-eq->y[i]+eq->x[j+1]-eq->x[j])/p0;

    l=max(l,-1);
    l=min(l,1);

    l=(l+1)/2;
*/

/*    l=(EqCell(eq,j,i)-eq->minVal)/(eq->maxVal-eq->minVal);


    if (mode==DRAW_ON) SetViewMode(w,VM2_EQUIL);
    DrawViewLine(w,eq->x[j],eq->y[i],eq->x[j]+(eq->x[j+1]-eq->x[j])*l,eq->y[i]);
    DrawViewLine(w,eq->x[j],eq->y[i],eq->x[j],eq->y[i]+(eq->y[i+1]-eq->y[i])*l);

    if (mode==DRAW_ON) SetViewMode(w,VM1_EQUIL);
    DrawViewLine(w,eq->x[j]+(eq->x[j+1]-eq->x[j])*l,eq->y[i],eq->x[j+1],eq->y[i]);
    DrawViewLine(w,eq->x[j],eq->y[i]+(eq->y[i+1]-eq->y[i])*l,eq->x[j],eq->y[i+1]);
*/
/*
    f=EqCell(eq,j,i);
    if (h) SetViewMode(w,f>0 ? VM1_EQUIL : VM2_EQUIL);
    DrawViewLine(w,eq->x[j]-fabs(f)/(eq->maxVal-eq->minVal)*l/2,eq->y[i],
        eq->x[j]+fabs(f)/(eq->maxVal-eq->minVal)*l/2,eq->y[i]);
*/

  }
}

void DrawSurface(View w,Surface s,int mode) {
  Index ix;
  XY xy1,xy2;

  if (~w->showFlags & SHW_SURFACES) return;
  xy1=SurfaceLine1st(s,&ix);
  if (xy1==NULL) FatalError("DrawSurface()-empty: fatal error 1");
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,s) ? VMX_SURFACE : VM1_SURFACE);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,s)) {SetViewMode(w,VMX_SURFACE);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_SURFACE);
      break;
  }
  while ((xy2=Next(&ix))!=NULL) {
    DrawViewLine(w,xy1->x,xy1->y,xy2->x,xy2->y);
    xy1=xy2;
  }
}

void DrawTemplate(View w,Template t,int mode) {
  XY xy1,xy2;
  Index ix;

  if (~w->showFlags & SHW_TEMPLATE) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,t) ? VMX_TEMPLATE : VM1_TEMPLATE);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,t)) {SetViewMode(w,VMX_TEMPLATE);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_TEMPLATE);
      break;
  }
  for (xy1=TemplatePoint1st(t,&ix);xy1!=NULL;xy1=Next(&ix)) {
    xy2=Next(&ix);

    DrawViewLine(w,TemplateXY2X(t,xy1),TemplateXY2Y(t,xy1),
      TemplateXY2X(t,xy2),TemplateXY2Y(t,xy2));
  }
}

void DrawXPoint(View w,XPoint xpt,int mode) {
  int i;
  XY xy,xy1;
  Index ix;

  if (!(w->showFlags & SHWM_XPOINT)) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,xpt) ? VMX_XPOINT : VM1_XPOINT);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,xpt)) {SetViewMode(w,VMX_XPOINT);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_XPOINT);
      break;
  }
  for (i=0;i<3;i++)
    for (xy=Group1st(xpt->line[i],&ix);xy!=NULL && (xy1=Next(&ix))!=NULL;
	xy=xy1) {
      DrawViewLine(w,xy->x,xy->y,xy1->x,xy1->y);
/*DrawViewLine(w,xy->x,xy->y,xy->x+(xy1->y-xy->y),xy->y-(xy1->x-xy->x));*/
    }
}

void DrawXPointTest(View w,XPointTest xpt,int mode) {
  int i;
  XY xy,xy1;
  Index ix;

  assert(w->app->equil!=NULL);

  if (!(w->showFlags & SHW_XPOINTTESTS)) return;

  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,xpt)? VMX_XPOINTTEST:VM2_XPOINTTEST);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,xpt)) {SetViewMode(w,VMX_XPOINTTEST);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_XPOINTTEST);
      break;
  }
  for (i=0;i<4;i++) if (xpt->gradients[i]!=NULL) {
    xy1=Group1st(xpt->gradients[i],&ix);
    if (xy1==NULL) continue;
    for (;(xy=Next(&ix))!=NULL;xy1=xy)
      DrawViewLine(w,xy1->x,xy1->y,xy->x,xy->y);
  }

  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,xpt)? VMX_XPOINTTEST:VM1_XPOINTTEST);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,xpt)) {SetViewMode(w,VMX_XPOINTTEST);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_XPOINTTEST);
      break;
  }

  DrawViewLine(w,w->app->equil->x[xpt->cx1],w->app->equil->y[xpt->cy1],
    w->app->equil->x[xpt->cx2],w->app->equil->y[xpt->cy1]);

  DrawViewLine(w,w->app->equil->x[xpt->cx1],w->app->equil->y[xpt->cy1],
    w->app->equil->x[xpt->cx1],w->app->equil->y[xpt->cy2]);

  DrawViewLine(w,w->app->equil->x[xpt->cx2],w->app->equil->y[xpt->cy2],
    w->app->equil->x[xpt->cx2],w->app->equil->y[xpt->cy1]);

  DrawViewLine(w,w->app->equil->x[xpt->cx2],w->app->equil->y[xpt->cy2],
    w->app->equil->x[xpt->cx1],w->app->equil->y[xpt->cy2]);
}

void DrawXPointSeg(View w,XPointSeg xps,int mode) {
  XY xy,xy1;
  Index ix;

  if (!(w->showFlags & (SHW_GRIDPOINTS|SHW_SURFACES|SHW_XPOINTTESTS))) return;

  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,xps)? VMX_XPOINTSEG:VM1_XPOINTSEG);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,xps)) {SetViewMode(w,VMX_XPOINTSEG);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_XPOINTSEG);
      break;
  }

  if (xps->line==NULL) return;

  for (xy1=Group1st(xps->line,&ix);(xy=Next(&ix))!=NULL;xy1=xy)
    DrawViewLine(w,xy1->x,xy1->y,xy->x,xy->y);
}

void DrawGridPoint(View w,GridPoint gp,int mode) {
  if (~w->showFlags & SHW_GRIDPOINTS) return;
  if (w->app->xpoint==NULL) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,gp) ? VMX_GPOINT : VM1_GPOINT);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,gp)) {SetViewMode(w,VMX_GPOINT);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_GPOINT);
      break;
  }
  DrawViewLine(w,gp->x-gp->dy/w->zoomX*w->gridPointLen/2,
    gp->y+gp->dx/w->zoomY*w->gridPointLen/2,
    gp->x+gp->dy/w->zoomX*w->gridPointLen/2,
    gp->y-gp->dx/w->zoomY*w->gridPointLen/2);
}

void DrawAxes(View w) {
  if (~w->showFlags & SHW_AXES) return;

  SetViewMode(w,VM1_AXES);

  DrawViewLine(w,0,w->maxY,0,w->minY);
  DrawViewLine(w,0,w->maxY,w->arrowWidth/w->zoomX,
    w->maxY-w->arrowLength/w->zoomY);
  DrawViewLine(w,0,w->maxY,-w->arrowWidth/w->zoomX,
    w->maxY-w->arrowLength/w->zoomY);

  DrawViewLine(w,w->maxX,0,w->minX,0);
  DrawViewLine(w,w->maxX,0,w->maxX-w->arrowLength/w->zoomX,
    w->arrowWidth/w->zoomX);
  DrawViewLine(w,w->maxX,0,w->maxX-w->arrowLength/w->zoomX,
    -w->arrowWidth/w->zoomX);
}

void DrawGrid(View w) {
  double c0x,c0y,csx,csy,t;
  char s[256];

  if (~w->showFlags & SHW_GRID) return;

/* Calculate maximal grid size
*/
  c0x=w->gridCellMax/w->zoomX;
  c0y=w->gridCellMax/w->zoomY;

/* Calculate rounded-up grid size
*/
  csx=pow(10.0,floor(log10(w->maxX-w->minX)));
  while (csx>c0x) csx/=10;
  if (csx*5<c0x) csx*=5; else
  if (csx*2.5<c0x) csx*=2.5;

  csy=pow(10.0,floor(log10(w->maxY-w->minY)));
  while (csy>c0y) csy/=10;
  if (csy*5<c0y) csy*=5; else
  if (csy*2.5<c0y) csy*=2.5;

  if (csx*1e6<(w->maxX-w->minX)) return;
  if (csy*1e6<(w->maxY-w->minY)) return;

  SetViewMode(w,VM1_GRID);
  for (t=csx*floor(w->minX/csx);t<w->maxX;t+=csx)
    DrawViewLine(w,t,w->minY,t,w->maxY);
  for (t=csy*floor(w->minY/csy);t<w->maxY;t+=csy)
    DrawViewLine(w,w->minX,t,w->maxX,t);

  SetViewMode(w,VM1_GRIDTEXT);
  for (t=csx*floor(w->minX/csx+1);t<w->maxX-csx;t+=csx) {
    sprintf(s,"%g",t);
    DrawViewText(w,t+2/w->zoomX,w->minY+2/w->zoomY,s);
  }
  for (t=csy*floor(w->minY/csy+1);t<w->maxY;t+=csy) {
    sprintf(s,"%g",t);
    DrawViewText(w,w->minX+2/w->zoomX,t+2/w->zoomY,s);
  }
}

void DrawViewLabel(View w,ViewLabel vl,int mode) {
  if (~w->showFlags & SHW_LABELS) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,VM1_VIEWLABEL);
      break;
    case DRAW_OFF:
    case DRAW_ERASE:
      SetViewMode(w,VM0_VIEWLABEL);
      break;
  }
  DrawViewLine(w,vl->x,vl->y,vl->x+w->labelOffsetX/w->zoomX,
    vl->y+w->labelOffsetY/w->zoomY);
  DrawViewText(w,vl->x+w->labelOffsetX/w->zoomX,
    vl->y+w->labelOffsetY/w->zoomY,vl->label);
}

void DrawSonnetData(View w,SonnetData sd,int mode) {
  int i,j,pos;

  if (~w->showFlags & SHW_MESH) return;
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,VM1_SONNETDATA);
      break;
    case DRAW_OFF:
    case DRAW_ERASE:
      SetViewMode(w,VM0_SONNETDATA);
      break;
  }
  for (i=0;i<sd->sy;i++) for (j=0;j<sd->sx;j++) {
    pos=(i*sd->sx+j)*4;
    DrawViewLine(w,sd->points[pos  ].x,sd->points[pos  ].y,
      sd->points[pos+1].x,sd->points[pos+1].y);
    DrawViewLine(w,sd->points[pos  ].x,sd->points[pos  ].y,
      sd->points[pos+2].x,sd->points[pos+2].y);
    DrawViewLine(w,sd->points[pos+3].x,sd->points[pos+3].y,
      sd->points[pos+1].x,sd->points[pos+1].y);
    DrawViewLine(w,sd->points[pos+3].x,sd->points[pos+3].y,
      sd->points[pos+2].x,sd->points[pos+2].y);

  }
}

void DrawSeparator(View w,Separator sep,int mode) {
  char s[10];

  if (w->showFlags & SHW_SEPARATORS) {
    if (IsMarked(w->app,sep) && !IsHighlighted(w->app,sep)) {
      switch(mode) {
        case DRAW_ON:
          SetViewMode(w,VM1_ELEMMARK);
          break;
        case DRAW_OFF:
        case DRAW_ERASE:
          SetViewMode(w,VM0_ELEMMARK);
          break;
      }
      DrawViewLine(w,sep->x,sep->y,sep->n->x,sep->n->y);
    }
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,IsHighlighted(w->app,sep) ?
          VMX_SEPARATOR : VM1_SEPARATOR);
        break;
      case DRAW_OFF:
        if (IsHighlighted(w->app,sep)) {SetViewMode(w,VMX_SEPARATOR);break;}
      case DRAW_ERASE:
        SetViewMode(w,VM0_SEPARATOR);
        break;
    }
    DrawViewLine(w,sep->x,sep->y,sep->n->x,sep->n->y);
  }
  if (w->showFlags & SHW_NUMBERS && !IsHighlighted(w->app,sep)) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMNUMBER);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMNUMBER);
        break;
    }
    sprintf(s,"%d",sep->id);
    DrawViewText(w,(sep->x+sep->n->x)/2,(sep->y+sep->n->y)/2,s);
  }
}

void DrawSource(View w,Source src,int mode) {
  double r,rx,ry;

  if (~w->showFlags & SHW_SOURCES) return;

  if (IsMarked(w->app,src) && !IsHighlighted(w->app,src)) {
    switch(mode) {
      case DRAW_ON:
        SetViewMode(w,VM1_ELEMMARK);
        break;
      case DRAW_OFF:
      case DRAW_ERASE:
        SetViewMode(w,VM0_ELEMMARK);
        break;
    }
    DrawViewCircle(w,src->x,src->y,w->srcR/w->zoomX);
  }
  switch(mode) {
    case DRAW_ON:
      SetViewMode(w,IsHighlighted(w->app,src) ? VMX_SOURCE : VM1_SOURCE);
      break;
    case DRAW_OFF:
      if (IsHighlighted(w->app,src)) {SetViewMode(w,VMX_SOURCE);break;}
    case DRAW_ERASE:
      SetViewMode(w,VM0_SOURCE);
      break;
  }
  r=w->srcR/(w->zoomX+w->zoomY)*2;
  rx=((int)(0.886*w->srcR))/w->zoomX;
  ry=((int)(0.500*w->srcR))/w->zoomX;
  DrawViewLine(w,src->x,src->y-r,src->x,src->y+r);
  DrawViewLine(w,src->x,src->y,src->x-rx,src->y-ry);
  DrawViewLine(w,src->x,src->y,src->x-rx,src->y+ry);
  DrawViewLine(w,src->x,src->y,src->x+rx,src->y-ry);
  DrawViewLine(w,src->x,src->y,src->x+rx,src->y+ry);
}
