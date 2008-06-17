/* Create Object dialog box definitions */

#ifndef _x_creobj_h
#define _x_creobj_h

void CbCmCreateAt(Widget wg,XtPointer arg,XtPointer pcbs);

/* Needed for x_xapp.c in order to install the action */
void XtActDistrGraph(Widget wg,XEvent* xev,String* args,Cardinal* argn);

#endif
