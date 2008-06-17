/* Element & node data types & fns
*/

#ifndef _elmnodes_h
#define _elmnodes_h

struct _Node {
  int type;
  double x,y,z;
  int locks;
  Group elems,separators;
};

struct _Elem {
  int type;
  App app;
  Node n[3];
  int id,locks;
  Group vars;
  Group varsContaining;
};

struct _Template {
  int type,locks;
  char fName[DG_FNAME_LEN];
  double xIncr,yIncr,angle,scale;
  int dragStatus;
  double dragX,dragY;
  Group points;
};

#define NodeElem1st(n,ix) Group1st((n)->elems,(ix))
#define TemplatePoint1st(t,ix) Group1st((t)->points,(ix))
#define ElemVar1st(e,ix) Group1st((e)->vars,(ix))
#define IsMarked(a,e) InGroup((a)->mark,(e))
#define IsRegularNode(node,group) (!IsIrregularNode((node),(group)))
#define GetUnusedNumberCount(a) ((a)->maxElemId-GroupCount((a)->elems)-\
    GroupCount((a)->separators))

Node AddNode(App a,double x,double y);
void ChangeNode(App a,Node n,double x,double y);
void* DelNode(App a,Node n);
Elem AddElem(App a,Node n1,Node n2);
void* DelElem(App a,Elem e);
void ChangeElem(App a,Elem e,int ix,Node n);
void ChangeElemId(App a,Elem e,int id);
void RevertElem(App a,Elem e);

int AddTemplate(App a,char* fName,double xIncr,double yIncr,double angle,
    double scale);
void ChangeTemplate(App a,double xIncr,double yIncr,double angle,double
    scale);
void DelTemplate(App a);
void SetTemplateDragStatus(App a,int dragStatus,double dragX,double dragY);
int GetNearestTemplatePoint(Template t,double x,double y,
    double* ptx,double* pty,double* psx,double* psy);

void MarkObject(App a,void* object,int status);
void MarkGroup(App a,Group g,int status);

int IsEmptyNode(Node n);
int IsIrregularNode(Node n,Group g);
int IsEndNode(Node n,Group g);
int NodesConnected(Node n1,Node n2);
int ElemsConnected(Elem e1,Elem e2);
Elem GetNextElem(Elem e,Group g,int node);
Group GetElemChain(Elem e,Group gElems,Group gCheckRegular);

int OrderStructureElems(App a,Group g0,Group* pggTargets,Group* pggClosed,
    Group* pggBroken,void** errObj);
void FreeStructureInfo(Group* pggTargets,Group* pggClosed,Group* pggBrokn);
int OrderTargetElems(Group g,void** errObj);
int CheckStructurePart(Group gStructure,Group gPart,void** pErrObj);
void RenumberElems(App a);
int GlueNodes(App a,Group g,double maxDist,int* prCount);
int GlueElems(App a,Group g,double maxDist,double maxLen,int bCutLonger,
    int* prCount);
int GlueNormals(App a,Group g,int* prCount);

void HighlightDragNode(View w,Node n,int flag);
int AppendTemplate(App a);
int ConvertChordsToElems(App a,Group chords,void** pErrObj);
Node SplitElem(App a,Elem e,int count,int* pErr);
Elem JoinElems(App a,Node n,int* pErr);
int WriteElemsAsDgTemplate(Group gElems,char* fName);

double TemplateXY2X(Template t,XY xy);
double TemplateXY2Y(Template t,XY xy);

int PlaceTemplateByHandles(App a,double xa,double ya,double x1,double y1,
    double xb,double yb,double x2,double y2);

void SetMaxElemId(App a,int id);
int GetNextElemId(App a);

/* Private functions - only for Act_xx
*/
int LoadDgTemplateFile(Template t);
int LoadOgrTemplateFile(Template t);
int LoadHpglTemplateFile(Template t);
int CheckForHpglFile(FILE* f);
int Load2ColumnsTemplateFile(Template t);
void LockTarget(Var v,int n);

#endif
