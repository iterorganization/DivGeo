/* Draw - drawing functions for different objects
*/

#ifndef _draw_h
#define _draw_h

void DrawAxes(View w);
void DrawGrid(View w);

void DrawEquil(View w,Equil eq,int mode);
void DrawTemplate(View w,Template t,int mode);
void DrawSonnetData(View w,SonnetData sd,int mode);
void DrawXPointTest(View w,XPointTest xpt,int mode);
void DrawXPointSeg(View w,XPointSeg xps,int mode);

void DrawNode(View w,Node n,int mode);
void DrawElem(View w,Elem e,int mode);
void DrawSource(View w,Source src,int mode);
void DrawSeparator(View w,Separator sep,int mode);

void DrawViewLabel(View w,ViewLabel vl,int mode);

#endif
