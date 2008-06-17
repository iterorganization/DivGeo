/* Sources - structures and functions
*/

#ifndef _sources_h
#define _sources_h

struct _Source {
  int type,locks;
  double x,y;
  Group vars;
};

Source AddSource(App a,double x,double y);
int ChangeSource(App a,Source src,double x,double y);
void* DelSource(App a,Source src);

#endif
