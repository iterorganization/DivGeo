/* PostScript View */

#ifndef _psview_h
#define _psview_h

View CreatePsView(App a,double width,double height,double margin);
int PsViewOutput(View w,char* fName,int fd,char* title);

#endif
