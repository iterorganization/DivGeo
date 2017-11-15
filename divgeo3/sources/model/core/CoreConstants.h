#ifndef CORECONSTANTS_H
#define CORECONSTANTS_H

#include "../../common/Types.h"

// Notification types
struct NotificationTypes
{
  enum ENotificationTypes
  {
    ADD           = 0x0001,
    ADDED         = 0x0002,
    CHANGE        = 0x0004,
    CHANGED       = 0x0008,
    DEL           = 0x0010,
    DELETED       = 0x0020,
    ALT           = 0x0040,
    NEWAPP        = 0x0080,
    MARK          = 0x0100,
    DESTROYVIEW   = 0x0200,
    EXAMINE       = 0x0400,

    RECENTFILES   = 0x1000,
    NEWTOOL       = 0x2000,

    NOW           = 0x4000,
    USER          = 0x8000,

    NMASK_HASOBJECT = (ADD|ADDED|CHANGE|CHANGED|DEL|DELETED)
  };
};
typedef NotificationTypes::ENotificationTypes NotificationType;
typedef NotificationTypes NT;


struct MeshSlidingModes
{
  enum EMeshSlidingModes
  {
    USE_SPLINE = 0x0001,
    USE_SURFACE = 0x0002,
    USE_POINTS = 0x0004,

    SPLINE = (USE_SPLINE | USE_POINTS),
    SURFACE = (USE_SURFACE)
  };

static const _NameRec names[];
};
typedef MeshSlidingModes::EMeshSlidingModes MeshSlidingMode;
typedef MeshSlidingModes MSM;

/* Advanced notification flags */

struct SChangeFlags {
  enum EChangeFlags {
    APPVIEW    = 0x0001,
    GEOMETRY   = 0x0002,
    FILES      = 0x0004,
    VARS       = 0x0008,
    VARDEFS    = 0x0010,
    MARK       = 0x0020,
    APP        = 0x0040,
    VARSMENU   = 0x0080,
    MESH       = 0x0100,
    TOPOLOGY   = 0x0200,
    TARGETS    = 0x0400
  };
};
typedef SChangeFlags::EChangeFlags ChangeFlag;
typedef SChangeFlags CHF;


/* Error flags for LoadApp */

struct ErrorFlags {
  enum EErrorFlags {
    NOEQUIL           = 0x0001,
    NOTEMPL           = 0x0002,
    BADTYPE           = 0x0004,
    SYNTAX            = 0x0008,
    NOSONNET          = 0x0010,
    BADTARG1          = 0x0020,
    BADTARG2          = 0x0040,
    BADSTRUC          = 0x0080,
    BADXPT            = 0x0080,
    MESH_FP           = 0x0100,
    NEQUIL            = 0x0200,
    OLDTOPO           = 0x0400,
    OLD_SURFACES_LOST = 0x0800,
    UNKNOWN_TOKEN     = 0x1000
  };
};

typedef ErrorFlags DGFE;
typedef ErrorFlags::EErrorFlags ErrorFlag;


struct FileModes {
  enum EFileModes {
    APP    = 1,
    CONFIG = 2,
    TMP    = 3
  };
};
typedef FileModes DGFM;
typedef FileModes::EFileModes FileMode;


struct OutputModes {
  enum EOutputModes {
    SONNET = 1,
    CARRE = 2
  };
  static const _NameRec names[];
};
typedef OutputModes OUTPUTMODE;

struct Visibility {
  enum EVisibility {
    HIDE = 0,
    SHOW = 1
  };
  static const _NameRec names[];
};

struct SurfaceZonesRestriction { // dg3
  enum ESurfaceZonesRestriction {
    B2CARRE = 0,
    B2EIRENE = 1,

    TARGETS = 0,
    TCHAINS = 1,
    TPOINTS = 2
  };
  static const _NameRec names[];
};
typedef SurfaceZonesRestriction SZR;

struct OutputFlags {
  enum EOutputFlags {
    VARS        = 0x0001,
    STRUCTURE   = 0x0002,
    TARGETS     = 0x0004,
    SURFACES    = 0x0008,
    GRIDPOINTS  = 0x0010,
    CELLS       = 0x0020,

    ALL         = VARS | STRUCTURE | TARGETS |
                  SURFACES | GRIDPOINTS | CELLS
  };
  static const _FlagsRec flags[];
};
typedef OutputFlags::EOutputFlags OutputFlag;
typedef OutputFlags OF; //OF_NC;
typedef OutputFlags AF; //AF_VALID;


struct SurfaceZoneFlags {
  enum ESurfaceZoneFlags {
    LIMITBYSURFACE = 0x0001
  };
  static const _FlagsRec flags[];
};
typedef SurfaceZoneFlags SZF;


struct GridPointSegFlags {
  enum EGridPointSegFlags {
    USED = 0x0001,
    TARGET_CW = 0x0002
  };
  static const _FlagsRec flags[];
};
typedef GridPointSegFlags GPSF;
typedef GridPointSegFlags::EGridPointSegFlags GridPointSegFlag;


#endif
