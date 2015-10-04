#ifndef MODES_H
#define MODES_H

struct SViewModes 
{
  enum EViewModes
  {
    NODE_1           = 0,
    NODE_0           = 1,
    NODE_X           = 2,
    ELEM_1           = 3,
    ELEM_0           = 4,
    ELEM_X           = 5,
    EQUIL_1          = 6,
    EQUIL_0          = 7,
    EQUIL_X          = 8,
    SURFACE_1        = 9,
    SURFACE_0        = 10,
    SURFACE_X        = 11,
    HIGHLIGHTRECT_1  = 12,
    HIGHLIGHTRECT_0  = 13,
    HIGHLIGHTRECT_X  = 14,
    EQUIL_2          = 15,
    TEMPLATE_1       = 16,
    TEMPLATE_0       = 17,
    TEMPLATE_X       = 18,
    ELEMNORMAL_1     = 19,
    ELEMNORMAL_0     = 20,
    ELEMNORMAL_X     = 21,
    ELEMMARK_1       = 22,
    ELEMMARK_0       = 23,
    XPOINT_1         = 24,
    XPOINT_0         = 25,
    XPOINT_X         = 26,
    GPOINT_1         = 27,
    GPOINT_0         = 28,
    GPOINT_X         = 29,
    AXES_1           = 30,
    GRID_1           = 31,
    GRIDTEXT_1       = 32,
    ELEMNUMBER_1     = 33,
    ELEMNUMBER_0     = 34,
    VIEWLABEL_1      = 35,
    VIEWLABEL_0      = 36,
    SONNETDATA_1     = 37,
    SONNETDATA_0     = 38,
    SEPARATOR_1      = 39,
    SEPARATOR_0      = 40,
    SEPARATOR_X      = 41,
    EQUIL_3          = 42,
    SOURCE_1         = 43,
    SOURCE_0         = 44,
    SOURCE_X         = 45,
    CHORD_1          = 46,
    CHORD_0          = 47,
    CHORD_X          = 48,
    XPOINTTEST_1     = 49,
    XPOINTTEST_2     = 50,
    XPOINTTEST_0     = 51,
    XPOINTTEST_X     = 52,
    XPOINTSEG_1      = 53,
    XPOINTSEG_0      = 54,
    XPOINTSEG_X      = 55,
    MESHELEMENT_1    = 56,
    MESHELEMENT_0    = 57,
    MESHELEMENT_X    = 58,
    MESHELEMENT_2    = 59,
    MESHCELLCENTER_1 = 60,
    MESHCELLCENTER_2 = 61,
    MESHCELLCENTER_0 = 62,
    MESHCELLCENTER_X = 63,
    MESHELEMENT_X2   = 64,
    MESHELEMENT_X12  = 65
  };

  static const unsigned size = 66;
};

typedef SViewModes::EViewModes ViewMode;
typedef SViewModes VM;

struct SDrawModes
{
  enum EDrawModes
  {
    ON    = 1,
    OFF   = 2,
    ERASE = 3
  };

  struct SHighlight
  {
    enum EHighlight
    {
      LOCK = 4,
      UNLOCK = 5,
      CNDOFF = 6
    };
  };
};

typedef SDrawModes::EDrawModes DrawMode;
typedef SDrawModes DRAW;
typedef SDrawModes::SHighlight DRAWHI;

#endif
