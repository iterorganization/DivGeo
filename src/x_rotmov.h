/* Rotate/Move dialog routines */
/* Also Stretch/Rotate dialog routines */

#ifndef _x_rotmov_h
#define _x_rotmov_h

Widget OpenRotMoveDlg(View w);
void CbGluePoints(Widget,XtPointer xtpV,XtPointer);
void CbReduceElements(Widget,XtPointer xtpV,XtPointer);
void CbGroupNormals(Widget,XtPointer xtpV,XtPointer);

Widget OpenStretchRotateDlg(View w);
Widget OpenSetAngleDlg(View w);

#endif
