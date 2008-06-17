/* XApp data type & fns */

#ifndef _xapp_h
#define _xapp_h

struct _XApp {
  int type;

  XAppExt x;
  Group views;
  char* configFileName;
  App config;
  StringSource stringSource;

  int* pargc;
  char** argv;
  int bLoadConfig;
};

#ifndef NO_DUMMIES
struct _XAppExt {
  int dummy;
};
#endif

/* Implementation-independent part
*/
XApp CreateXApp(int* pargc,char** argv);
void* FreeXApp(XApp xap);

/* Implementation-dependent part
*/
void ConfigureXApp(XApp xap);
void UnconfigureXApp(XApp xap);
char* XAppStr(XApp xap,int id);

#endif
