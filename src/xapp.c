#include "dg.h"

XApp CreateXApp(int* pargc,char** argv) {
  XApp xap;
  int err;
  char* msg;

  xap=Malloc(sizeof(*xap));
  xap->type=T_XAPP;

  xap->x=NULL;
  xap->views=CreateGroup();
  xap->configFileName=NULL;
  xap->config=NULL;
  xap->stringSource=NULL;

  xap->pargc=pargc;
  xap->argv=argv;
  xap->bLoadConfig=1;
  ConfigureXApp(xap);
  if (xap->bLoadConfig) {
    xap->config=LoadApp(xap,xap->configFileName,&msg,&err);
    if (msg!=NULL) msg=Free(msg);
    if (xap->config==NULL)
      FatalError("DivGeo: Error reading configuration from %s",
        xap->configFileName);
  } else
    xap->config=NULL;

  return xap;
}

void* FreeXApp(XApp xap) {

  if (!IsEmptyGroup(xap->views))
    FatalError("FreeXApp()-views: fatal error 1");
  if (xap->config!=NULL) xap->config=FreeApp(xap->config);

  UnconfigureXApp(xap);

  assert(xap->x==NULL);
  assert(xap->stringSource==NULL);
  FreeGroup(xap->views);

  return Free(xap);
}


char* XAppStr(XApp xap,int id) {
  return xap->stringSource==NULL? 
      "(null)" : xap->stringSource->GetStringProc(xap->stringSource,id);
}

