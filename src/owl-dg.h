////////////////////////////////////////////////////////////////////////
//                                                                    //
// OWL interface to DG                                                //
// Copyright (C) Konstantin Kukushkin, 1996                           //
//                                                                    //
////////////////////////////////////////////////////////////////////////
//                                                                    //
// Requirement: #include <owl.h>  BEFORE calling this file            //
//                                                                    //
////////////////////////////////////////////////////////////////////////

class TDgViewWindow;

struct _ViewExt {
  TDgViewWindow* dgw;
};

struct _XAppExt {
  void* dummy;
};

extern "C" {
  #include "dg.h"
}

#include "dg-owl.rh"

class TDgViewWindow : public TWindow {
  public:

  TDgViewWindow(App a,TWindow* parent=NULL);
  ~TDgViewWindow();

  protected:

  View w;
  int drawMode,mouseInside;
  ToolProc curTool;
  TColor curColor;
  int curROP2,curWidth;

  virtual void SetupWindow();

  int IsRealized() {
    return HWindow!=NULL;
  }

// Events

  virtual void Paint(TDC& dc,BOOL erase,TRect& rc);
  virtual void EvSize(uint sizeType,TSize& size);
  virtual void EvLButtonDown(uint modKeys, TPoint& point);
  virtual void EvLButtonUp(uint modKeys, TPoint& point);
  virtual void EvMouseMove(uint modKeys, TPoint& point);

// Command functions

  void CmTlExamine();
  void EnTlExamine(TCommandEnabler& en);
  void CmTlZoomPan();
  void EnTlZoomPan(TCommandEnabler& en);
  void CmTlMark();
  void EnTlMark(TCommandEnabler& en);
  void CmTlMove();
  void EnTlMove(TCommandEnabler& en);
  void CmTlDelete();
  void EnTlDelete(TCommandEnabler& en);
  void CmTlAddElem();
  void EnTlAddElem(TCommandEnabler& en);
  void CmTlConnect();
  void EnTlConnect(TCommandEnabler& en);

// DG virtual vunctions

  static void UnconfigureAbortProc(View w) {
    assert(0);
  }

  static void UnconfigureProc(View w) {
    delete w->x;
    w->x=NULL;
  }

  static void AdjustAspectRatioProc(View w) {
    w->zoomX=w->zoomY=min(w->zoomX,w->zoomY);
  }

  static void FlushProc(View w) {
  }

  static void NotifyProc(View w,int msg,void* object) {
  }

  static void SetModeProc(View w,int mode);
  static void DrawLineProc(View w,double x1,double y1,double x2,double y2);
  static void DrawRectProc(View w,double x1,double y1,double x2,double y2);
  static void DrawCircleProc(View w,double x,double y,double r);
  static void DrawTextProc(View w,double x,double y,char* text);
  static void SetMsgProc(View w,char* msg);
  static void ClearProc(View w);
  static char* GetStrProc(View w,int id);

// Static data members

  static TColor pxBackground,pxHighlight,pxNode,pxElem,pxSurface,pxZoomRect,
    pxErrorHighlight,pxSeparator,pxSource,pxTemplate,pxNormal,pxMark,
    pxXPoint,pxEquil1,pxEquil2,pxGridPoint,pxAxes,pxGrid,pxGridText,
    pxElemNumber,pxViewLabel,pxSonnet;

  static int whHighlight,whNode,whElem,whSurface,whZoomRect,whTemplate,whNormal,
    whMark,whXPoint,whEquil,whAxes,whGrid,whGridText,whElemNumber,whViewLabel,
    whSonnet,whGridPoint,whSeparator,whSource;

  DECLARE_RESPONSE_TABLE(TDgViewWindow);
};

