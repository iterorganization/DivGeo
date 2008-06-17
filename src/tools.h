/* Tool functions
*/

#ifndef _tools_h
#define _tools_h

void TlAddElem(View w,int event,double x,double y);
void TlSplitElem(View w,int event,double x,double y);
void TlJoinElems(View w,int event,double x,double y);
void TlConnectPoints(View w,int event,double x,double y);
void TlRepositionElem(View w,int event,double x,double y);
void TlConnectElems(View w,int event,double x,double y);
void TlZoom(View w,int event,double x,double y);
void TlAddSurface(View w,int event,double x,double y);
void TlMark(View w,int event,double x,double y);
void TlMarkSingle(View w,int event,double x,double y);
void TlZoom(View w,int event,double x,double y);
void TlSetXPoint(View w,int event,double x,double y);
void TlAddGridPoint(View w,int event,double x,double y);
void TlExamine(View w,int event,double x,double y);
void TlMirrorNormals(View w,int event,double x,double y);
void TlAddSource(View w,int event,double x,double y);
void TlMoveObject(View w,int event,double x,double y);
void TlRemoveObject(View w,int event,double x,double y);

#endif
