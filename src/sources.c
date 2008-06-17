#include "dg.h"

/* Routines for sources
*/

Source AddSource(App a,double x,double y) {
  Source src;
  struct _ActRec ar;

  ValidatePtr(a,"AddSource");

  src=Malloc(sizeof(*src));
  src->type=T_SOURCE;
  src->locks=0;
  src->x=x;
  src->y=y;

  ar.obj=src;
  ActAddSource(a,&ar);

  return src;
}

int ChangeSource(App a,Source src,double x,double y) {
  struct _ChangeSourceRec ar;

  ValidatePtr(a,"ChangeSource");
  ValidatePtr(src,"ChangeSource_");

  ar.src=src;
  ar.x=x;
  ar.y=y;

  return ActChangeSource(a,&ar);
}

void* DelSource(App a,Source src) {
  struct _DelRec ar;
  Var v;
  Index ix;

  ValidatePtr(a,"DelSource");
  ValidatePtr(src,"DelSource_");
  assert(!IsHighlighted(a,src));
  MarkObject(a,src,0);

  for (v=Group1st(src->vars,&ix);v!=NULL;v=Next(&ix))
    SetVar(a,src,v->def,v->varSet,NULL);

  ar.delete=src;
  ActDelSource(a,&ar);

  return NULL;
}
