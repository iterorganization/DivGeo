#ifndef _chords_h
#define _chords_h

/* Chords */

struct _Chord {
  int type,locks;
  double x1,y1,x2,y2;
  Group vars,varsContaining;
};

Chord AddChord(App a,double x1,double y1,double x2,double y2);
int ChangeChord(App a,Chord ch,double x1,double y1,double x2,double y2);
void* DelChord(App a,Chord ch);

void DrawChord(View w,Chord ch,int mode);

void TlAddChord(View w,int event,double x,double y);

int ConvertElemsToChords(App a,Group elems,void** pErrObj);
int ConvertTemplateToChords(App a);

/* Direction does not matter! */
Chord FindChord(App a,double x1,double y1,double x2,double y2);


#endif
