#include "dg.h"
#include "unistd.h"
/*#pragma hdrstop*/

static int VectorInRectangle(double x1,double y1,double x2,double y2,
    double rx1,double ry1,double rx2,double ry2);

#define PointInRectangle(x,y,rx1,ry1,rx2,ry2) \
  ((x)>=min((rx1),(rx2)) && (x)<=max((rx1),(rx2)) && \
   (y)>=min((ry1),(ry2)) && (y)<=max((ry1),(ry2)))

void AddXY(Group g,double x,double y) {
  XY xy;

  xy=Malloc(sizeof(*xy));
  xy->x=x;
  xy->y=y;
  GroupAdd(g,xy);
}

void Rotate(double t,double centerX,double centerY,double* px,double* py) {
  double x=*px,y=*py,ct=cos(t),st=sin(t);
  *px = centerX + (ct*(x - centerX) - st*(y - centerY));
  *py = centerY + (st*(x - centerX) + ct*(y - centerY));
}

double Point2PointDist(double x1,double y1,double x2,double y2) {
  return sqrt(sqr(x1-x2)+sqr(y1-y2));
}

double Point2VectorDist(double x1,double y1,double x2, double y2,
    double x,double y,int* posflag,double* pos) {
  double sq,k;

  x2-=x1;y2-=y1;
  x-=x1;y-=y1;
  sq=x2*x2+y2*y2;
  k=x2*x+y2*y;
  if (posflag!=NULL)
    *posflag= (k<sq/2) ? ( k<0 ? -1: 1 ) : ( k>sq ? -2 : 2 );
  if (pos!=NULL)
    *pos=min(max(k,0),sq)/sq;
  if (sq>0 && k>0 && k<sq) return fabs(x2*y-y2*x)/sqrt(sq);
  if (k>0) {x+=x1-(x2+x1);y+=y1-(y2+y1);}
/*printf("pvdd:%g %g %g\n",x,y,sqrt(x*x+y*y));*/
  return sqrt(x*x+y*y);
}

/* Return -1 on non-intersection, 0 on intersection */
/* (x1,y1,x2,y2) is the first segment, and (x3,y3,x4,y4) is the second.
   *ar and *br are how far along the intersection is on each segment. */
int VIntersect(double x1,double y1,double x2,double y2,
    double x3,double y3,double x4,double y4,double* ar,double* br) {
  double a,b,d;

  d=(y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);
  if (d==0) return -2;
  a=(y4-y3)*(x3-x1)-(x4-x3)*(y3-y1);
  b=(y2-y1)*(x3-x1)-(x2-x1)*(y3-y1);
  a=a/d;
  b=b/d;

  if (ar!=NULL) *ar=a;
  if (br!=NULL) *br=b;
  if (a<0 || a>1 || b<0 || b>1) return -1;
  return 0;
}

static int VectorInRectangle(double x1,double y1,double x2,double y2,
    double rx1,double ry1,double rx2,double ry2) {

  if (rx2<rx1) swap(rx2,rx1);
  if (ry2<ry1) swap(ry2,ry1);

  if (x1<rx1 && x2>rx1)
    y1+=(y2-y1)*(rx1-x1)/(x2-x1),x1=rx1; else
  if (x2<rx1 && x1>rx1)
    y2+=(y1-y2)*(rx1-x2)/(x1-x2),x2=rx1;
  if (x1>rx2 && x2<rx2)
    y1+=(y2-y1)*(rx2-x1)/(x2-x1),x1=rx2; else
  if (x2>rx2 && x1<rx2)
    y2+=(y1-y2)*(rx2-x2)/(x1-x2),x2=rx2;

  if (y1<ry1 && y2>ry1)
    x1+=(x2-x1)*(ry1-y1)/(y2-y1),y1=ry1; else
  if (y2<ry1 && y1>ry1)
    x2+=(x1-x2)*(ry1-y2)/(y1-y2),y2=ry1;
  if (y1>ry2 && y2<ry2)
    x1+=(x2-x1)*(ry2-y1)/(y2-y1),y1=ry2; else
  if (y2>ry2 && y1<ry2)
    x2+=(x1-x2)*(ry2-y2)/(y1-y2),y2=ry2;

  if (x1<rx1 && x2<rx1) return 0;
  if (y1<ry1 && y2<ry1) return 0;
  if (x1>rx2 && x2>rx2) return 0;
  if (y1>ry2 && y2>ry2) return 0;

  return 1;
}

double SurroundingCircleRadius(double dx1,double dy1,double dx2,double dy2){
  double s;

  s=fabs(dx1*dy2-dx2*dy1)/2;
  if (s==0) return MAXDOUBLE;

  return hypot(dx1,dy1)*hypot(dx2,dy2)*hypot(dx1-dx2,dy1-dy2)/(4*s);
}

double CalcLineLength(Group g) {
  double l;
  XY xy,xy1;
  Index ix;

  for (l=0,xy=Group1st(g,&ix);(xy1=Next(&ix))!=NULL;xy=xy1)
    l+=hypot(xy->x-xy1->x,xy->y-xy1->y);

  return l;
}

#ifdef __TURBOC__
#pragma argsused
#endif
static double StretchCompress(double x,double l,double l1,double pwr) {
  return x*l1/l;
}

#ifdef __TURBOC__
#pragma argsused
#endif
static double StretchExpand(double x,double l,double l1,double pwr) {
  return x*l1/l;
}

double StretchValue2(double val,double a,double b,double x0,double x,
    double pwr) {

   pwr=0.2;
   if (a==b) return a;
   if (a>b) swap(a,b);
   if (val<x0)
     if (x<x0) return a+StretchCompress(val-a,x0-a,x-a,pwr);
     else return a+StretchExpand(val-a,x0-a,x-a,pwr);
   else
     if (x>x0) return b-StretchCompress(b-val,b-x0,b-x,pwr);
     else return b-StretchExpand(b-val,b-x0,b-x,pwr);
}

double StretchValue(double val,double a,double b,double x0,double x,
    double pwr) {
  int points=100;
  int i;
  double xC,f;

  if (a==b) return a;
  if (a>b) swap(a,b);

  for (i=0;i<points;i++) {
    if (val<=a) break;
    if (val>=b) break;
    xC=x0+(2*i+1)*(x-x0)/(2*points);

    if (val==xC) f=1;
    else  f=(val<xC ? (val-a)/(xC-a) : (b-val)/(b-xC));
    f=pow(1-(1-f)*(1-f),pwr);

    val+=f*(x-x0)/points;
  }
  val=max(val,a);
  val=min(val,b);
  return val;
}

Node HitNode(App a,double x,double y,double* pDist) {
  Node n,nHit;
  double dist,distHit=0;
  Index ix;

  for (nHit=NULL,n=AppNode1st(a,&ix);n!=NULL;n=Next(&ix)) {
    dist=hypot(x-n->x,y-n->y);
    if (nHit==NULL || dist<distHit) {
      nHit=n;
      distHit=dist;
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return nHit;
}

static Source HitSource(App a,double x,double y,double* pDist) {
  Source src,srcHit;
  double dist,distHit=0;
  Index ix;

  for (srcHit=NULL,src=AppSource1st(a,&ix);src!=NULL;src=Next(&ix)) {
    dist=hypot(x-src->x,y-src->y);
    if (srcHit==NULL || dist<distHit) {
      srcHit=src;
      distHit=dist;
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return srcHit;
}

static XPointTest HitXPointTest(App a,double x,double y,double* pDist) {
  XPointTest obj,objHit;
  double dist,distHit=0;
  Index ix;

  for (objHit=NULL,obj=AppXPointTest1st(a,&ix);obj!=NULL;obj=Next(&ix)) {
    dist=hypot(x-obj->centerX,y-obj->centerY);
    if (objHit==NULL || dist<distHit) {
      objHit=obj;
      distHit=dist;
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return objHit;
}

Elem HitElem(App a,double x,double y,int* pos,double* pDist) {
  Elem e,eHit;
  double dist,distHit=0;
  int posHit = 0,p;
  Index ix;

  for (eHit=NULL,e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix)) {
    dist=Point2VectorDist(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
      x,y,&p,NULL);
    if (eHit==NULL || dist<distHit) {
      distHit=dist;
      eHit=e;
      posHit=p;
    }
  }

  if (pos!=NULL) *pos=posHit;
  if (pDist!=NULL) *pDist=distHit;
  return eHit;
}

Separator HitSeparator(App a,double x,double y,double* pDist) {
  Separator sep,sepHit;
  double dist,distHit=0;
  Index ix;

  for (sepHit=NULL,sep=AppSeparator1st(a,&ix);sep!=NULL;sep=Next(&ix)) {
    dist=Point2VectorDist(sep->n->x,sep->n->y,sep->x,sep->y,
      x,y,NULL,NULL);
    if (sepHit==NULL || dist<distHit) {
      distHit=dist;
      sepHit=sep;
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return sepHit;
}

#define CHORD_RES 20

Chord HitChord(App a,double x,double y,int* pos,double* pDist) {
  Chord ch,chHit;
  double dist,distHit=0;
  int posHit = 0,p;
  double i,x1,y1,x2,y2;
  Index ix;

  for (chHit=NULL,ch=AppChord1st(a,&ix);ch!=NULL;ch=Next(&ix)) {
    if (~a->showFlags & SHW_TOPVIEW) {
      if (a->showFlags & SHW_CHORDS) {
	dist=Point2VectorDist(ch->x1,ch->y1,ch->x2,ch->y2,
            x,y,&p,NULL);
	if (chHit==NULL || dist<distHit) {
	  distHit=dist;
	  chHit=ch;
	  posHit=p;
	}
      }
      if ((a->showFlags & SHW_3DCHORDS) && (ch->z1!=0 || ch->z2!=0)) {
	x1=hypot(ch->x1,ch->z1);y1=ch->y1;
	for (i=1./CHORD_RES;i<1;i+=1./CHORD_RES) {
	  x2=hypot(ch->x1+i*(ch->x2-ch->x1),ch->z1+i*(ch->z2-ch->z1));
	  y2=ch->y1+i*(ch->y2-ch->y1);
	  dist=Point2VectorDist(x1,y1,x2,y2,x,y,&p,NULL);
	  if (chHit==NULL || dist<distHit) {
	    distHit=dist;
	    chHit=ch;
	    posHit=p;
	  }
	  x1=x2;y1=y2;
	}
	dist=Point2VectorDist(x1,y1,hypot(ch->x2,ch->z2),ch->y2,x,y,&p,NULL);
	if (chHit==NULL || dist<distHit) {
	  distHit=dist;
	  chHit=ch;
	  posHit=p;
	}
      }
    } else if (a->showFlags & SHW_CHORDS) {
      dist=Point2VectorDist(ch->x1,ch->z1,ch->x2,ch->z2,
          x,y,&p,NULL);
      if (chHit==NULL || dist<distHit) {
	distHit=dist;
	chHit=ch;
	posHit=p;
      }
    }
  }

  if (pos!=NULL) *pos=posHit;
  if (pDist!=NULL) *pDist=distHit;
  return chHit;
}

static SurfaceEx HitSurfaceEx(App a,double x,double y,double* pDist) {
  SurfaceEx s,sHit;
  XY xy1,xy2;
  double dist,distHit=0;
  Index ix,ixy;

  for (sHit=NULL,s=AppSurfaceEx1st(a,&ix);s!=NULL;s=Next(&ix)) {
    if (!s->bCoordsOk) continue;
    for (xy1=Group1st(s->line,&ixy);(xy2=Next(&ixy))!=NULL;xy1=xy2) {
      dist=Point2VectorDist(xy1->x,xy1->y,xy2->x,xy2->y,
        x,y,NULL,NULL);
      if (sHit==NULL || dist<distHit) {
        distHit=dist;
        sHit=s;
      }
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return sHit;
}

XPointSeg HitXPointSeg(App a,double x,double y,double* pDist) {
  XPointSeg s,sHit;
  XY xy1,xy2;
  double dist,distHit=0;
  Index ix,ixy;

  for (sHit=NULL,s=AppXPointSeg1st(a,&ix);s!=NULL;s=Next(&ix)) {
    if (s->line==NULL) continue;
    for (xy1=Group1st(s->line,&ixy);(xy2=Next(&ixy))!=NULL;xy1=xy2) {
      dist=Point2VectorDist(xy1->x,xy1->y,xy2->x,xy2->y,
        x,y,NULL,NULL);
      if (sHit==NULL || dist<distHit) {
        distHit=dist;
        sHit=s;
      }
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return sHit;
}

static GridPointEx HitGridPointEx(App a,double x,double y,double* pDist) {
  GridPointEx gpx,gpxHit;
  double d,dHit=0;
  Index ix;

  for (gpxHit=NULL,gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix)) {
    if (!gpx->bCoordsOk) continue;
    d=hypot(x-gpx->x,y-gpx->y);
    if (gpxHit==NULL || d<dHit) {
      gpxHit=gpx;
      dHit=d;
    }
  }

  if (pDist!=NULL) *pDist=dHit;
  return gpxHit;
}

int HitGridPointExPos(App a,double x,double y,int* pzone,double* pvalue) {
  GridPointSeg gps,gpsHit=NULL;
  XY xy,xy1;
  Index ix,ix_gps;
  int zHit=-1;
  double s,d,v,dHit=0,vHit=0;

  for (gps=AppGridPointSeg1st(a,&ix_gps);gps!=NULL;gps=Next(&ix_gps)) {
    if (!(gps->flags & GPSF_USED)) continue;
    xy=Group1st(gps->line,&ix);
    ValidatePtr(xy,"HitGridPointExPos.loop");
    for (s=0;(xy1=Next(&ix))!=NULL;xy=xy1) {
      d=Point2VectorDist(xy->x,xy->y,xy1->x,xy1->y,x,y,NULL,&v);
      v=s+v*hypot(xy->x-xy1->x,xy->y-xy1->y)/gps->lineLength;
      if (zHit<0 || d<dHit) {
        zHit=gps->zone;
        dHit=d;
        vHit=v;
        gpsHit=gps;
      }
      s+=hypot(xy->x-xy1->x,xy->y-xy1->y)/gps->lineLength;
    }
  }
  if (vHit<0) vHit=0;
  if (vHit>1) vHit=1;
  if (gpsHit!=NULL && gpsHit->dir<0) vHit=1-vHit;

  if (zHit>=0) {
    if (pvalue!=NULL) *pvalue=vHit;
    if (pzone!=NULL) *pzone=zHit;
  }
  return zHit<0 ? -1 : 0;
}

static MeshPoint HitMeshPoint(App a,double x,double y,double* pDist) {
  MeshPoint mpt,mptHit;
  double d,dHit=0;
  int i;

  if (a->mesh==NULL) return NULL;

  for (mptHit=NULL,i=0;i<a->mesh->pointCount;i++) {
    mpt=a->mesh->points+i;
    d=hypot(x-mpt->x,y-mpt->y);
    if (mptHit==NULL || d<dHit) {
      mptHit=mpt;
      dHit=d;
    }
  }

  if (pDist!=NULL) *pDist=dHit;
  return mptHit;
}

static MeshElement HitMeshElement(App a,double x,double y,int* pos,
    double* pDist) {
  MeshElement me,meHit;
  double dist,distHit=0;
  int posHit = 0,p,i;
  Index ix;

  if (a->mesh==NULL) return NULL;

  for (meHit=NULL,i=0;i<a->mesh->elementCount;i++) {
    me=a->mesh->elements+i;
    dist=Point2VectorDist(me->points[0]->x,me->points[0]->y,
        me->points[1]->x,me->points[1]->y,
        x,y,&p,NULL);
    if (meHit==NULL || dist<distHit) {
      distHit=dist;
      meHit=me;
      posHit=p;
    }
  }

  if (pos!=NULL) *pos=posHit;
  if (pDist!=NULL) *pDist=distHit;
  return meHit;
}

static MeshCell HitMeshCell(App a,double x,double y,double* pDist) {
  MeshCell mc,mcHit;
  double dist,distHit=0;
  int i;

  if (a->mesh==NULL) return NULL;


  for (mcHit=NULL,i=0;i<a->mesh->cellCount;i++) {
    mc=a->mesh->cells+i;
    dist=hypot(mc->centerX-x,mc->centerY-y);
    if (mcHit==NULL || dist<distHit) {
      mcHit=mc;
      distHit=dist;
    }
  }

  if (pDist!=NULL) *pDist=distHit;
  return mcHit;
}

/* treats SHW_TOPVIEW as a special case.  to add more 3D objects,
   handle them under the if clause.  to add functionality to handle
   any object in topview, get rid of the conditional and handle
   SHW_TOPVIEW in each function individually. (this is already done
   for chords). basic behavior would be just to return when topview is
   on, but more behavior could be added on if desired. */
void* HitViewObject(View w,double x,double y,long flags) {
  void* p,*pHit;
  double d,dHit=0;
  XPointTest xpt;
  long mask;

  if (flags & SHW_NODES) flags |= SHW_IRRNODES;

  mask=w->showFlags;
  if (mask & SHW_MESH)
    mask |= SHWX_MESHCELLS|SHWX_MESHELEMENTS|SHWX_MESHPOINTS;

  if (!w->bEditTopology) mask=mask & ~(SHW_XPOINTTESTS);

  flags &= mask;
  pHit=NULL;

  if (w->showFlags & SHW_TOPVIEW) {
    if (flags & (SHW_CHORDS)) {
      p=HitChord(w->app,x,y,NULL,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }
  } else {

    if (flags & SHW_ELEMS) {
      p=HitElem(w->app,x,y,NULL,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }

    if (flags & SHW_SURFACES) {
      p=HitSurfaceEx(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }

    if (flags & SHW_SEPARATORS) {
      p=HitSeparator(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }

    if (flags & SHW_SOURCES) {
      p=HitSource(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }

    if (flags & (SHW_CHORDS | SHW_3DCHORDS)) {
      p=HitChord(w->app,x,y,NULL,&d);
      if (p!=NULL && (pHit==NULL || d<dHit)) {pHit=p;dHit=d;}
    }

    if (flags & SHW_XPOINTTESTS) {
      p=HitXPointSeg(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<=dHit)) {pHit=p;dHit=d;}
    }

    if (flags & SHWX_MESHELEMENTS) {
      p=HitMeshElement(w->app,x,y,NULL,&d);
      if (p!=NULL && (pHit==NULL || d<=dHit))
	{pHit=p;dHit=d;}
    }

    if (flags & SHWX_MESHCELLS) {
      p=HitMeshCell(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<=dHit))
	{pHit=p;dHit=d;}
    }

    if (flags & SHWX_MESHPOINTS) {
      p=HitMeshPoint(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<=dHit || d<=w->meshPointRadius/w->zoomX))
	{pHit=p;dHit=d;}
    }

    if (flags & (SHW_IRRNODES | SHW_NODES)) {
      p=HitNode(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<=dHit || d<w->nodeR/w->zoomX))
	{pHit=p;dHit=d;}
    }

    if (flags & SHW_XPOINTTESTS) {
      p=HitXPointTest(w->app,x,y,&d);
      xpt=p;
      if (p!=NULL && (pHit==NULL || d<=dHit ||
	  d<=fabs(w->app->equil->x[xpt->cx2]-w->app->equil->x[xpt->cx1])))
	{pHit=p;dHit=d;}
    }

    if (flags & SHW_GRIDPOINTS) {
      p=HitGridPointEx(w->app,x,y,&d);
      if (p!=NULL && (pHit==NULL || d<dHit || d<w->gridPointLen/2/w->zoomX)) {
	pHit=p;dHit=d;}
    }
  }

  return pHit;
}

Group CoveredViewObjects(View w,double x1,double y1,double x2,double y2,
    long flags) {
  void* p;
  Group g;
  Index ix;
  int i;
  long mask;

  mask=w->showFlags;
  if (mask & SHW_MESH)
    mask |= SHWX_MESHCELLS|SHWX_MESHELEMENTS|SHWX_MESHPOINTS;


  if (flags & SHW_NODES) flags |= SHW_IRRNODES;
  flags &= mask;

  g=CreateGroup();

  if (flags & SHW_ELEMS) {
    for (p=AppElem1st(w->app,&ix);p!=NULL;p=Next(&ix))
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  if (flags & SHW_SEPARATORS) {
    for (p=AppSeparator1st(w->app,&ix);p!=NULL;p=Next(&ix))
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  if (flags & SHW_SOURCES) {
    for (p=AppSource1st(w->app,&ix);p!=NULL;p=Next(&ix))
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  if (flags & SHW_CHORDS) {
    for (p=AppChord1st(w->app,&ix);p!=NULL;p=Next(&ix))
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  if (flags & SHWX_MESHCELLS && w->app->mesh!=NULL) {
    for (i=0;p=w->app->mesh->cells+i,i<w->app->mesh->cellCount;i++)
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  if (flags & SHWX_MESHELEMENTS && w->app->mesh!=NULL) {
    for (i=0;p=w->app->mesh->elements+i,i<w->app->mesh->elementCount;i++)
      if (ObjectInRectangle(p,x1,y1,x2,y2)) GroupAdd(g,p);
  }

  return g;
}

void CalcObjExtents(void* obj,double* pMinX,double* pMinY,double* pMaxX,
    double* pMaxY) {
  Index ix;
  Node n;
  Source src;
  Chord ch;
  Elem e;
  Separator sep;
  Equil eq;
  Template t;
  SonnetData sd;
  Mesh m;
  MeshPoint mpt;
  MeshCell mc;
  MeshElement me;
  XY xy;
  GridPointSeg gps;
  SurfaceEx sx;
  GridPointEx gpx;
  double x,y;
  int i;

  switch(GetObjType(obj)) {
    case T_NODE:
      n=obj;
      if (*pMinX>*pMaxX) {*pMinX=*pMaxX=n->x;*pMinY=*pMaxY=n->y;}
      *pMinX=min(*pMinX,n->x);*pMaxX=max(*pMaxX,n->x);
      *pMinY=min(*pMinY,n->y);*pMaxY=max(*pMaxY,n->y);
      break;
    case T_SOURCE:
      src=obj;
      if (*pMinX>*pMaxX) {*pMinX=*pMaxX=src->x;*pMinY=*pMaxY=src->y;}
      *pMinX=min(*pMinX,src->x);*pMaxX=max(*pMaxX,src->x);
      *pMinY=min(*pMinY,src->y);*pMaxY=max(*pMaxY,src->y);
      break;
    case T_CHORD:
      ch=obj;
      if (*pMinX>*pMaxX) {*pMinX=*pMaxX=ch->x1;*pMinY=*pMaxY=ch->y1;}
      *pMinX=min(*pMinX,ch->x1);*pMaxX=max(*pMaxX,ch->x1);
      *pMinY=min(*pMinY,ch->y1);*pMaxY=max(*pMaxY,ch->y1);
      *pMinX=min(*pMinX,ch->x2);*pMaxX=max(*pMaxX,ch->x2);
      *pMinY=min(*pMinY,ch->y2);*pMaxY=max(*pMaxY,ch->y2);
      break;
    case T_ELEM:
      e=obj;
      CalcObjExtents(e->n[1],pMinX,pMinY,pMaxX,pMaxY);
      CalcObjExtents(e->n[2],pMinX,pMinY,pMaxX,pMaxY);
      break;
    case T_SEPARATOR:
      sep=obj;
      CalcObjExtents(sep->n,pMinX,pMinY,pMaxX,pMaxY);
      *pMinX=min(*pMinX,sep->x);*pMaxX=max(*pMaxX,sep->x);
      *pMinY=min(*pMinY,sep->y);*pMaxY=max(*pMaxY,sep->y);
      break;
    case T_SURFACEEX:
      sx=obj;
      if (sx->line!=NULL) {
        for (xy=Group1st(sx->line,&ix);xy!=NULL;xy=Next(&ix)) {
          if (*pMinX>*pMaxX) {
            *pMinX=*pMinY=MAXDOUBLE;
            *pMaxX=*pMaxY=-MAXDOUBLE;
          }
          *pMinX=min(*pMinX,xy->x);*pMaxX=max(*pMaxX,xy->x);
          *pMinY=min(*pMinY,xy->y);*pMaxY=max(*pMaxY,xy->y);
        }
      }
      break;
    case T_GRIDPOINTEX:
      gpx=obj;
      if (gpx->bCoordsOk) {
        if (*pMinX>*pMaxX) {
          *pMinX=*pMinY=MAXDOUBLE;
          *pMaxX=*pMaxY=-MAXDOUBLE;
        }
        *pMinX=min(*pMinX,gpx->x);*pMaxX=max(*pMaxY,gpx->x);
        *pMinY=min(*pMinY,gpx->y);*pMaxY=max(*pMaxY,gpx->y);
      }
      break;
    case T_GRIDPOINTSEG:
      gps=obj;
      if (gps->line!=NULL) {
        for (xy=Group1st(gps->line,&ix);xy!=NULL;xy=Next(&ix)) {
          if (*pMinX>*pMaxX) {
            *pMinX=*pMinY=MAXDOUBLE;
            *pMaxX=*pMaxY=-MAXDOUBLE;
          }
          *pMinX=min(*pMinX,xy->x);*pMaxX=max(*pMaxX,xy->x);
          *pMinY=min(*pMinY,xy->y);*pMaxY=max(*pMaxY,xy->y);
        }
      }
      break;
    case T_SURFACEZONE: /* $ */
      break;
    case T_TEMPLATE:
      t=obj;
      xy=Group1st(t->points,&ix);
      if (*pMinX>*pMaxX) {*pMinX=*pMaxX=xy->x;*pMinY=*pMaxY=xy->y;}
      while (xy!=NULL) {
        x=TemplateXY2X(t,xy);
        y=TemplateXY2Y(t,xy);
        *pMinX=min(*pMinX,x);*pMaxX=max(*pMaxX,x);
        *pMinY=min(*pMinY,y);*pMaxY=max(*pMaxY,y);
        xy=Next(&ix);
      }
      break;
    case T_EQUIL:
      eq=obj;
      if (*pMinX>*pMaxX) {*pMinX=*pMaxX=eq->x[0];*pMinY=*pMaxY=eq->y[0];}
      for (i=0;i<eq->sx;i++) {
        *pMinX=min(*pMinX,eq->x[i]);*pMaxX=max(*pMaxX,eq->x[i]);
      }
      for (i=0;i<eq->sy;i++) {
        *pMinY=min(*pMinY,eq->y[i]);*pMaxY=max(*pMaxY,eq->y[i]);
      }
      break;
    case T_SONNET:
      sd=obj;
      if (*pMinX>*pMaxX) {
        *pMinX=*pMaxX=sd->points[0].x;
        *pMinY=*pMaxY=sd->points[0].y;
      }
      for (i=sd->sx*sd->sy*4-1;i>0;i--) {
        xy=sd->points+i;
        *pMinX=min(*pMinX,xy->x);*pMaxX=max(*pMaxX,xy->x);
        *pMinY=min(*pMinY,xy->y);*pMaxY=max(*pMaxY,xy->y);
      }
      break;
    case T_MESH:
      m=obj;
      for (i=0;i<m->pointCount;i++)
        CalcObjExtents(m->points+i,pMinX,pMinY,pMaxX,pMaxY);
      break;
    case T_MESHELEMENT:
      me=obj;
      for (i=0;i<2;i++)
        CalcObjExtents(me->points[i],pMinX,pMinY,pMaxX,pMaxY);
      break;
    case T_MESHCELL:
      mc=obj;
      for (i=0;i<4;i++)
        CalcObjExtents(mc->points[i],pMinX,pMinY,pMaxX,pMaxY);
      break;
    case T_MESHPOINT:
      mpt=obj;
      if (*pMinX>*pMaxX) {
        *pMinX=*pMaxX=mpt->x;
        *pMinY=*pMaxY=mpt->y;
      }
      *pMinX=min(*pMinX,mpt->x);*pMaxX=max(*pMaxX,mpt->x);
      *pMinY=min(*pMinY,mpt->y);*pMaxY=max(*pMaxY,mpt->y);
      break;
    default:
      FatalError("CalcObjExtents()-type%d: fatal error 1",GetObjType(obj));
  }
}

void CalcGroupExtents(Group g,double* pMinX,double* pMinY,double* pMaxX,
    double* pMaxY) {
  void* obj;
  Index ix;

  ValidatePtr(g,"CalcGroupExtents");
  for (obj=Group1st(g,&ix);obj!=NULL;obj=Next(&ix))
    CalcObjExtents(obj,pMinX,pMinY,pMaxX,pMaxY);
}

int DetectFileType(char* fName) {
  FILE* f;
  char s[256];
  int i,ft;
  char c;
  double float1,float2,float3;

  f=fopen(fName,"r");
  if (f==NULL) return FT_NOTFOUND;

  for (ft=i=0;!ft && i<10 && fgets(s,sizeof(s)-1,f)!=NULL;i++) {
    if (sscanf(s,SCANFLT""SCANFLT""SCANFLT,&float1,&float2,&float3)==2)
      ft=FT_2COLUMNS;
    if (sscanf(s,"   Element output%c",&c)==1 && i==1) ft=FT_SONNET;
    if (sscanf(s,"DivGeo temporary file>%c",&c)==1) ft=FT_DG_TMP;
    if (sscanf(s,"DivGeo data file>%c",&c)==1) ft=FT_DG_APP;
    if (sscanf(s,"DivGeo config file>%c",&c)==1) ft=FT_DG_CONFIG;
    if (sscanf(s,"DivGeo equilibrium file>%c",&c)==1) ft=FT_DG_EQUIL;
    if (sscanf(s,"DivGeo template file>%c",&c)==1) ft=FT_DG_TEMPLATE;
    if (i==1 && sscanf(s,
        "     km   :=  no. of grid points in vertical direction%c",&c)==1)
      ft=FT_1_EQUIL;
    if (sscanf(s,"P3/PATRAN Neutral File%c",&c)==1) ft=FT_OGR_TEMPLATE;
  }

  if (!ft) {
    fseek(f,0,SEEK_SET);
    if (!CheckForHpglFile(f)) ft=FT_HPGL;
  }

  fclose(f);

  return ft ? ft : FT_UNKNOWN;
}

int LockObject(void* obj,int incr) {
  Node n;
  Elem e;
  Separator sep;
  Var v;
  VarSet vs;
  VarDef vd;
  VarSetDef vsd;

  switch(GetObjType(obj)) {
    case T_NODE:
      n=obj;
      return n->locks+=incr;
    case T_ELEM:
      e=obj;
      return e->locks+=incr;
    case T_SEPARATOR:
      sep=obj;
      return sep->locks+=incr;
    case T_SOURCE:
      return ((Source)obj)->locks+=incr;
    case T_CHORD:
      return ((Chord)obj)->locks+=incr;
    case T_EQUIL:
      return ((Equil)obj)->locks+=incr;
    case T_SONNET:
      return ((SonnetData)obj)->locks+=incr;
    case T_XPOINTTEST:
      return ((XPointTest)obj)->locks+=incr;
    case T_XPOINTSEG:
      return ((XPointSeg)obj)->locks+=incr;
    case T_VARSETDEF:
      vsd=obj;
      return vsd->locks+=incr;
    case T_VARDEF:
      vd=obj;
      LockObject(vd->varSetDef,incr);
      return vd->locks+=incr;
    case T_VARSET:
      vs=obj;
      LockObject(vs->def,incr);
      return vs->locks+=incr;
    case T_VAR:
      v=obj;
      LockObject(v->def,incr);
      LockObject(v->origin,incr);
      return v->locks+=incr;
    default:
      FatalError("LockObject()-type%d: fatal error 1",GetObjType(obj));
      return 0;
  }
}

char* GetShortFName(char* fName) {
  int i;

  for (i=strlen(fName)-1;i>=0;i--)
    if (fName[i]=='/' || fName[i]=='\\' || fName[i]==':') return fName+i+1;
  return fName;
}

char* GetFileExt(char* fName) {
  int i;

  fName=GetShortFName(fName);

  for (i=strlen(fName)-1;i>=0;i--)
    if (fName[i]=='.') return fName+i;

  return fName+strlen(fName);
}

char* GetFilePath(char* fName) {
  *GetShortFName(fName)=0;

  return fName;
}

void* TranslateId(PtrTable t,int id) {
  size_t i;

  for (i=0;t[i].p!=NULL;i++) if (t[i].id==id) return t[i].p;
  FatalError("TranslateId()-notFound%d: fatal error 1",id);
  return NULL;
}

int CountSurfaces(App a,int area) {
  SurfaceEx sx;
  Index ix;
  int i;

/*  if (a->equil==NULL) return 0; */
  for (i=0,sx=AppSurfaceEx1st(a,&ix);sx!=NULL;sx=Next(&ix))
    if (SurfaceExOk(sx) && sx->zone==area) i++;

  return i;
}

int CountGridPoints(App a,int area) {
  GridPointEx gpx;
  Index ix;
  int i;

  for (i=0,gpx=AppGridPointEx1st(a,&ix);gpx!=NULL;gpx=Next(&ix))
    if (GridPointExOk(gpx) && gpx->zone==area) i++;

  return i;
}

void* GetLockingObject(App a,void* object) {
  Index ix,ix1;
  Separator sep;
  VarSetDef vsd;
  VarSet vs;
  VarDef vd;
  Var v;
  void* p;

  switch(GetObjType(object)) {
    case T_NODE:
/*      n=object;*/
      break;
    case T_SOURCE:
    case T_SEPARATOR:
    case T_ELEM:
      if (!a->bStrict) {
        assert(0);
      }

      for (vd=AppVarDef1st(a,&ix);vd!=NULL;vd=Next(&ix))
        if (vd->varType & VTM_HASGROUP) /* $$$ - did not understand well */
          for (v=Group1st(vd->vars,&ix1);v!=NULL;v=Next(&ix1))
            if (v->val!=NULL && InGroup(v->val,object)) return IsLocked(v) ?
                GetLockingObject(a,v) : v;
      break;
    case T_EQUIL:
      p=AppXPointTest1st(a,NULL);
      if (p!=NULL) return IsLocked(p) ? GetLockingObject(a,p) : p;

      p=AppXPointSeg1st(a,NULL);
      if (p!=NULL) return IsLocked(p) ? GetLockingObject(a,p) : p;

      break;
    case T_XPOINTTEST:
      if (a->bStrict) {
        p=Group1st(((XPointTest)object)->segs,NULL);
        if (p!=NULL) return IsLocked(p) ? GetLockingObject(a,p) : p;
      }
      break;
    case T_SONNET:
      sep=AppSeparator1st(a,NULL);
      if (sep!=NULL) return IsLocked(sep) ? GetLockingObject(a,sep) : sep;
      break;
    case T_VAR:
      v=object;
      break;
    case T_VARDEF:
      vd=object;
      for (v=Group1st(vd->vars,&ix);v!=NULL;v=Next(&ix))
        if (IsLocked(v)) return GetLockingObject(a,v);
      break;
    case T_VARSET:
      vs=object;
      for (v=Group1st(vs->vars,&ix);v!=NULL;v=Next(&ix))
        if (IsLocked(v)) return GetLockingObject(a,v);
      break;
    case T_VARSETDEF:
      vsd=object;
      for (vd=Group1st(vsd->varDefs,&ix);vd!=NULL;vd=Next(&ix))
        if (IsLocked(vd)) return GetLockingObject(a,vd);
      break;
  }
  FatalError("GetLockingObject()-type%d:fatal error 1",GetObjType(object));
  return NULL;
}

int IsLocked(void* obj) {
  Elem e;
  Equil eq;
  SonnetData sd;
  XPointTest xpt;

  switch(GetObjType(obj)) {
    case T_NODE:return ((Node)obj)->locks;
    case T_ELEM:
      e=(Elem)obj;
      if (e->app->bStrict && !IsEmptyGroup(e->varsContaining)) return 1;
      return ((Elem)obj)->locks;
    case T_SURFACEEX:return ((SurfaceEx)obj)->locks;
    case T_GRIDPOINTEX:return ((GridPointEx)obj)->locks;
    case T_SEPARATOR:return ((Separator)obj)->locks;
    case T_SOURCE:return ((Source)obj)->locks;
    case T_CHORD:return ((Chord)obj)->locks;
    case T_EQUIL:
      eq=(Equil)obj;
      if (eq->app->bStrict && (/*eq->app->xpoint!=NULL || */
        
        !IsEmptyGroup(eq->app->xpointTests)
      ) ) return 1;

      return ((Equil)obj)->locks;
    case T_TEMPLATE:return ((Template)obj)->locks;
    case T_SONNET:
      sd=(SonnetData)obj;
      if (sd->app->bStrict &&
        !IsEmptyGroup(sd->app->separators) ) return 1;
      return sd->locks;
    case T_XPOINTTEST:
      xpt=obj;
      if (xpt->app->bStrict && !IsEmptyGroup(xpt->segs)) return 1;
      return ((XPointTest)obj)->locks;
    case T_XPOINTSEG:return ((XPointSeg)obj)->locks;
    case T_MESH:return ((Mesh)obj)->locks;
    case T_MESHCELL:return ((MeshCell)obj)->locks;
    case T_MESHELEMENT:return ((MeshElement)obj)->locks;
    case T_MESHPOINT:return ((MeshPoint)obj)->locks;
    case T_VARSETDEF:return ((VarSetDef)obj)->locks;
    case T_VARSET:return ((VarSet)obj)->locks;
    case T_VARDEF:return ((VarDef)obj)->locks;
    case T_VAR:return ((Var)obj)->locks;
  }
  assert(0);
  return 0;
}

int IsCarreLocked(void* object) {
  SurfaceEx sx;
  GridPointEx gpx;
  
  switch (GetObjType(object)) {
    case T_SURFACEEX:
      sx=(SurfaceEx)object;
      return sx->app->outputMode==OUTPUTMODE_CARRE && !SurfaceExVirtual(sx);
    case T_GRIDPOINTEX:
      gpx=(GridPointEx)object;
      return gpx->app->outputMode==OUTPUTMODE_CARRE;
    default:
      return 0;
  }
  return 0; /* Make the compiler happy */
}

int CompStrings(char* str1,char* str2) {
  if ((str1==NULL || !*str1) && (str2==NULL || !*str2)) return 0;
  if (str1!=NULL && str2==NULL) return -1;
  if (str1==NULL && str2!=NULL) return 1;
  return strcmp(str1,str2);
}

char* Flags2Str(unsigned long flags,FlagsRec fr) {
  static char buf[400];
  int i;

  for (i=0;fr[i].c;i++) {
    buf[2*i]=(char)((flags & fr[i].mask) ? '+' : '-');
    buf[2*i+1]=fr[i].c;
  }
  buf[2*i]=0;

  return buf;
}

void Str2Flags(char* s,unsigned long* flags,FlagsRec fr) {
  int i,j;

  for (i=0;s[i];i+=2) {
    for (j=0;fr[j].c;j++) if (fr[j].c==s[i+1]) break;
    if (!fr[j].c) FatalError("Str2Flags()-flag%c: fatal error 1",s[i+1]);
    s[i]=='+' ? (*flags |=fr[j].mask) : (*flags &=~ fr[j].mask);
  }
}

char* Int2Name(int val,NameRec nr) {
  int i;

  for (i=0;nr[i].s!=NULL;i++)
    if (val==nr[i].val) return nr[i].s;
  FatalError("Int2Name()-notFound%d: fatal error 1",val);
  return NULL;
}

int Name2Int(char* name,NameRec nr) {
  int i;

  for (i=0;nr[i].s!=NULL;i++)
    if (!strcmp(name,nr[i].s)) return nr[i].val;
  FatalError("Name2Int()-notFound%s: fatal error 1",name);
  return -1;
}

int CompPolyLines(Group line1,Group line2) {
  Index ix1,ix2;
  XY xy1,xy2;

  if (GroupCount(line1)!=GroupCount(line2)) return -1;

  xy1=Group1st(line1,&ix1);
  xy2=Group1st(line2,&ix2);
  for(;xy1!=NULL;xy1=Next(&ix1),xy2=Next(&ix2))
    if (xy1->x!=xy2->x || xy1->y!=xy2->y) break;

  if (xy1==NULL) return 0;

  RevertGroup(line2);

  xy1=Group1st(line1,&ix1);
  xy2=Group1st(line2,&ix2);
  for(;xy1!=NULL;xy1=Next(&ix1),xy2=Next(&ix2))
    if (xy1->x!=xy2->x || xy1->y!=xy2->y) break;

  RevertGroup(line2);

  if (xy1==NULL) return 0;

  return -1;
}

int CompPolyLinesEx(Group line1,Group line2,double maxError) {
  Index ix1,ix2;
  XY xy1,xy2;

  if (GroupCount(line1)!=GroupCount(line2)) return -1;

  xy1=Group1st(line1,&ix1);
  xy2=Group1st(line2,&ix2);
  for(;xy1!=NULL;xy1=Next(&ix1),xy2=Next(&ix2))
    if (hypot(xy1->x-xy2->x,xy1->y-xy2->y)>maxError) break;

  if (xy1==NULL) return 0;

  RevertGroup(line2);

  xy1=Group1st(line1,&ix1);
  xy2=Group1st(line2,&ix2);
  for(;xy1!=NULL;xy1=Next(&ix1),xy2=Next(&ix2))
    if (hypot(xy1->x-xy2->x,xy1->y-xy2->y)>maxError) break;

  RevertGroup(line2);

  if (xy1==NULL) return 0;

  return -1;
}

/* Returns 0 on success */
int PolyLinesIntersect(Group line1,Group line2,double* pos1,double* pos2) {
  XY xy1a,xy1b,xy2a,xy2b;
  Index ix1,ix2;
  double r1,r2,r1t,r2t,len1=0,len2=0;

  if (GroupCount(line1)<2 || GroupCount(line2)<2) return -1;

  r1=r2=100;
  for (xy1a=Group1st(line1,&ix1);(xy1b=Next(&ix1))!=NULL;xy1a=xy1b) {
    len2=0;
    for (xy2a=Group1st(line2,&ix2);(xy2b=Next(&ix2))!=NULL;xy2a=xy2b) {
      if (VIntersect(xy1a->x,xy1a->y,xy1b->x,xy1b->y,
          xy2a->x,xy2a->y,xy2b->x,xy2b->y,&r1t,&r2t)) continue;

      /* Intersection detected */

      r1=min(r1,r1t);
      r2=min(r2,r2t);
      len2+=hypot(xy2b->x-xy2a->x,xy2b->y-xy2a->y)*min(r2,1);
    }

    len1+=hypot(xy1b->x-xy1a->x,xy1b->y-xy1a->y)*min(r1,1);

    if (r1<=1) break;
  }

  if (pos1!=NULL) *pos1=len1;
  if (pos2!=NULL) *pos2=len2;

  return r1<=1? 0 : -1;
}

void CutPolyLine(Group line,double pos,int bTail) {
  XY xy,xy1;
  Index ix;
  double nx,ny,l1,l,h;

  if (GroupCount(line)<2) return;

  for (l1=l=0,xy1=Group1st(line,&ix);(xy=Next(&ix))!=NULL;xy1=xy,l1=l) {
    l+=(h=hypot(xy->x-xy1->x,xy->y-xy1->y));
    if (h==0) continue;
    if (l1<=pos && l>=pos) {
      nx=xy1->x+(xy->x-xy1->x)*(pos-l1)/h;
      ny=xy1->y+(xy->y-xy1->y)*(pos-l1)/h;

      if (bTail) {
        xy1->x=nx;
        xy1->y=ny;
        for (xy=Group1st(line,&ix);xy!=xy1;xy=Next(&ix)) {
          GroupDel(line,xy);
          Free(xy);
        }
      } else {
        xy->x=nx;
        xy->y=ny;
        while ((xy=Next(&ix))!=NULL) {
          GroupDel(line,xy);
          Free(xy);
        }
      }
      break;
    }
  }

  return;
}

double ProjectPointToPolyLine(Group gxy,double x,double y) {
  XY xy,xy1;
  Index ix;
  double s,d,v,dHit=0,vHit=0;

  if (GroupCount(gxy)<2) return 0;

  dHit=MAXDOUBLE;

  xy=Group1st(gxy,&ix);

  for (s=0;(xy1=Next(&ix))!=NULL;xy=xy1) {
    d=Point2VectorDist(xy->x,xy->y,xy1->x,xy1->y,x,y,NULL,&v);
    v=s+v*hypot(xy->x-xy1->x,xy->y-xy1->y);
    if (d<dHit) {
      dHit=d;
      vHit=v;
    }
    s+=hypot(xy->x-xy1->x,xy->y-xy1->y);
  }
  if (vHit<0) vHit=0;
  if (vHit>s) vHit=s;

  return vHit;
}

double PointToPolyLineDist(Group gxy,double x,double y) {
  XY xy;
  Index ix;
  double d,dMin=MAXDOUBLE;

  for (xy=Group1st(gxy,&ix);xy!=NULL;xy=Next(&ix)) {
    d=hypot(x-xy->x,y-xy->y);
    if (d<dMin) dMin=d;
  }

  return dMin;
}


int GetPolyLinePoint(Group gxy,double length,double* x,double* y) {
  double s,l/*,x,y*/;
  XY xy,xy1;
  Index ix;

  assert(!IsEmptyGroup(gxy));

  xy=Group1st(gxy,&ix);

  for (s=0;(xy1=Next(&ix))!=NULL;xy=xy1) {
    l=hypot(xy->x-xy1->x,xy->y-xy1->y);
    if (l+s>=length) {
      *x=xy->x+(xy1->x-xy->x)*(length-s)/l;
      *y=xy->y+(xy1->y-xy->y)*(length-s)/l;
      return 0;
    }
    s+=l;
  }
  *x=xy->x;*y=xy->y;
  return -1;
}


void FollowRectCW(int* px,int* py,int cx1,int cy1,int cx2,int cy2) {
  if (cx2<cx1) swap(cx2,cx1);
  if (cy2<cy1) swap(cy2,cy1);

  if ((*py)==cy1) (*px)==cx2 ? (*py)++ : (*px)++; else  /* (cx1 cy1)      */
  if ((*px)==cx2) (*py)==cy2 ? (*px)-- : (*py)++; else  /*                */
  if ((*py)==cy2) (*px)==cx1 ? (*py)-- : (*px)--; else  /*      (cx2 cy2) */
  if ((*px)==cx1) (*py)==cy1 ? (*px)++ : (*py)--; else
  assert(0);

/*printf("followRect: %d %d <> %d %d %d %d\n",*px,*py,cx1,cy1,cx2,cy2);*/

}

void FollowRectCCW(int* px,int* py,int cx1,int cy1,int cx2,int cy2) {
  if (cx2<cx1) swap(cx2,cx1);
  if (cy2<cy1) swap(cy2,cy1);

  if ((*py)==cy2) (*px)==cx2 ? (*py)-- : (*px)++; else  /* (cx1 cy1)     */
  if ((*px)==cx2) (*py)==cy1 ? (*px)-- : (*py)--; else  /*                */
  if ((*py)==cy1) (*px)==cx1 ? (*py)++ : (*px)--; else  /*      (cx2 cy2) */
  if ((*px)==cx1) (*py)==cy2 ? (*px)++ : (*py)++; else
  assert(0);

}

void RestrictGroupToType(Group grp,int type) {
  void* p;
  Index ix;

  for (p=Group1st(grp,&ix);p!=NULL;p=Next(&ix))
    if (GetObjType(p)!=type) GroupDel(grp,p);
}

char* StripPrefix(char* str,char* prefix) {
  assert(strlen(prefix)==1);

  if (*str==*prefix) str++;

  return str;
}

static int CalcDeltaCoeffs(double d1,double dn,int cnt,
    double* paa,double* pbb,double* pcc) {
  double aa,bb,cc,l,denom,t;
  int n;

  l=1;
  n=max(1,cnt+1);

  if (d1<=0 || dn<=0) return -1;

  if (n<=1) {
    aa=l;
    bb=0.;
    cc=0.;
  } else if (n==2) {
    t=(3.*(1.-d1/dn)-2.*(1.-2.*d1/dn));
    if (t==0) return -1;
    bb=l/t;
    aa=-(1.-2.*d1/dn)*bb;
    bb=(1.-d1/dn)*bb;
    cc=0.;
  } else {
    denom = -((n-1)*(n-1))*(n-2)/6.;
    if (denom==0) return -1;
    aa = (d1*(-n)*(n+1)*(n-1)/6. - dn*(n+1)*(n-1)/3. + l*(n-1))
          /denom;
    bb = (d1*(-(n+1)*(2*n+1)/6. + n*n) + dn*((n+1)*(2*n+1)/6. - 1)
          + l*(1-n*n)/n)/denom;
    cc = (n-1)*(-d1/2. - dn/2. + l/n)/denom;
  }

  *paa=aa;
  *pbb=bb;
  *pcc=cc;

  return 0;
}

double DistributeLaw(double x,int law,double alpha,double alpha2,int cnt) {
  double aa,bb,cc,d1,dn,l,denom,t;
  int n,n0,i;

  switch(law) {
    case DGLAW_NORMAL:
      if (x<0) return 1;
      return pow(x,alpha);
    case DGLAW_FLIPPED:
      if (x<0) return 1;
      return 1-pow(x,alpha);
    case DGLAW_2ALPHA:
      if (x<0) return 1;
      return x<0.5? pow(x*2,alpha)/2 : 1-pow((1-x)*2,alpha2)/2;
    case DGLAW_SYMMETRIC:
      if (x<0) return 1;
      return x<0.5? pow(x*2,alpha)/2 : 1-pow((1-x)*2,alpha)/2;
    case DGLAW_DELTA:
      d1=alpha;
      dn=alpha2;
      l=1;
      n=n0=max(1,cnt+1);

      if (CalcDeltaCoeffs(alpha,alpha2,cnt,&aa,&bb,&cc)) return -1;

      if (x<0) {
        for (i=1;i<=n;i++) if (aa+bb*i+cc*i*i<=0) return -1;
        return 1;
      }

      n=min(n,x*n+1);
      for (t=0,i=1;i<n;i++) t+=aa+bb*i+cc*i*i;
      t+=(aa+bb*n+cc*n*n)*(x*n0+1-n);

      return t;
    default:
      assert(0);
  }

  assert(0);
  return 0; /* Make the compiler happy */
}

#define SCNT 10
#define SLEN 256

static char* GetLocalStaticStr(void) {
  static int index=0;
  static char buf[SCNT][SLEN+1];

  index=(index+1)%SCNT;
  return buf[index];
}

char* GetVersionStr(int v) {
  static char* buf;
  int l;

  buf=GetLocalStaticStr();

  if (v<=0) return "<?>";

  if (v<1000) v*=100;

  sprintf(buf,"%g",(v/100)/100.0);
  if (v/100%100==0) strcat(buf,".0");

  l=strlen(buf);
  if (v%100/10!=0) buf[l++]=(char)('a'+v%100/10-1);

  if (v%10==9) buf[l++]='$';
  else assert(v%10==0);

  buf[l]=0;

  return buf;
}

int GetGroupType(Group g) {
  int type=T_NONE;
  void* p;
  Index ix;

  for (p=Group1st(g,&ix);p!=NULL;p=Next(&ix))
    if (type==T_NONE) type=GetObjType(p);
    else if (GetObjType(p)!=type) {type=T_MULTIPLE;break;}

  return type;
}

int ObjectInRectangle(void* object,double x1,double y1,double x2,double y2){
  Elem e;
  Source src;
  Chord ch;
  Separator sep;
  MeshCell mc;
  MeshElement me;

  switch (GetObjType(object)) {
    case T_ELEM:
      e=(Elem)object;
      return VectorInRectangle(e->n[1]->x,e->n[1]->y,e->n[2]->x,e->n[2]->y,
          x1,y1,x2,y2);
    case T_CHORD:
      ch=(Chord)object;
      return VectorInRectangle(ch->x1,ch->y1,ch->x2,ch->y2,x1,y1,x2,y2);
    case T_SEPARATOR:
      sep=(Separator)object;
      return VectorInRectangle(sep->x,sep->y,sep->n->x,sep->n->y,
          x1,y1,x2,y2);
    case T_MESHELEMENT:
      me=(MeshElement)object;
      return VectorInRectangle(me->points[0]->x,me->points[0]->y,
          me->points[1]->x,me->points[1]->y,
          x1,y1,x2,y2);
    case T_SOURCE:
      src=(Source)object;
      return PointInRectangle(src->x,src->y,x1,y1,x2,y2);
    case T_MESHCELL:
      mc=(MeshCell)object;
      return PointInRectangle(mc->centerX,mc->centerY,x1,y1,x2,y2);
    default: assert(0);
  }
  return 0; /* Make the compiler happy */
}


/* Expands a file name to a canonical form.
 *
 * 1. Converts relative paths to absolute.
 * 2. Removes references to '.' directories.
 * 3. Resolves references to '..' directories.
 *
 * Limitations: Supposed to resolve file names only. Directory names
 *              containing '..'/'.' at the very end are not expanded.
                Does not fully resolve DOS filenames.
*/

char* ExpandFilename(char* name) {
  static char s[DG_FNAME_LEN+1];
  char* s1,* s2;
  int i,k;

  if (name==s) return name; /* Avoid interferences if already processed */

  /* Check for an absolute name */

  if (name[0] && name[1]==':') goto noExpand; /* DOS name, d:... */
  if (name[0]=='\\' || name[0]=='/') goto noExpand;

  *s=0;
  getcwd(s,DG_FNAME_LEN);
  if (*s==0) goto noExpand;
  if (s[0]=='\\') goto noExpand;  /* Do not handle DOS names */

  if (strlen(name)+strlen(s)>sizeof(s)-1) goto noExpand;

  if (*s && s[strlen(s)-1]!='/') strcat(s,"/");
  strcat(s,name);

  handleDots:

  /* Kill '.' references */

  while (1) {
    s1=strstr(s,"/./");
    if (s1==NULL) break;

    k=2;
    for (i=s1-s;s[i]=s[i+k],s[i+k];i++);
  }

  /* Kill '..' references */

  while (1) {
    s2=s1=strstr(s,"/../");
    if (s1==NULL) break;
    while (--s1>=s) if (*s1=='/') break;
    if (s1<s) break;

    k=s2-s1+3;
    for (i=s1-s;s[i]=s[i+k],s[i+k];i++);
  }

  return s;

  noExpand:

  strcpy(s,name);
  goto handleDots;
}

