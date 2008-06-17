#ifndef _chords_h
#define _chords_h

/* Chords */

struct _Chord {
  int type,locks;
  double x1,y1,z1,x2,y2,z2;
  Group vars,varsContaining;
};

Chord AddChord(App a,double x1,double y1,double x2,double y2);
int ChangeChord(App a,Chord ch,double x1,double y1,double x2,double y2);
void* DelChord(App a,Chord ch);
Chord AddChord3D(App a,double x1,double y1,double z1,double x2,double y2,
  double z2);
int ChangeChord3D(App a,Chord ch,double x1,double y1,double z1,double x2,
  double y2,double z2);

void DrawChord(View w,Chord ch,int mode);

void TlAddChord(View w,int event,double x,double y);
void TlExtChord(View w,int event,double x,double y);
void TlAdjustChord(View w,int event,double x,double y);

int ConvertElemsToChords(App a,Group elems,void** pErrObj);
int ConvertTemplateToChords(App a);

int ExtendChord(App a,Chord ch); /* extend the chord to end on an element */

/* Direction does not matter! */
Chord FindChord(App a,double x1,double y1,double x2,double y2);
#endif
