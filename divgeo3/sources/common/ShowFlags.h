#ifndef SHOWFLAGS_H
#define SHOWFLAGS_H

#include "Types.h"

typedef unsigned long ulong;

// ShowFlags
struct ShowFlags
{
  enum EShowFlags
  {
    AXES        = 0x00000001,
    GRID        = 0x00000002,
    ELEMS       = 0x00000004,
    NORMALS     = 0x00000008,
    NUMBERS     = 0x00000010,
    NODES       = 0x00000020,
    IRRNODES    = 0x00000040,
    TEMPLATE    = 0x00000080,
    EQUIL       = 0x00000100,
    SURFACES    = 0x00000200,
    GRIDPOINTS  = 0x00000400,
    LABELS      = 0x00000800,
    MESH        = 0x00001000,
    SEPARATORS  = 0x00002000,
    SOURCES     = 0x00004000,
    CHORDS      = 0x00008000,
    MENU        = 0x00010000,
    TOOLBAR     = 0x00020000,
    XPOINTTESTS = 0x00040000,
    MESHDETAILS = 0x00080000,
    STRETCH     = 0x00100000,
    TOPVIEW     = 0x00200000,
    CHORDS3D    = 0x00400000,

    XPOINTSEGS  = 0x00800000,

    COMMENTS    = 0x01000000,

    XPOINT = (SURFACES | GRIDPOINTS),

    INIT = (AXES|ELEMS|NODES|IRRNODES|TEMPLATE|SEPARATORS|MENU|TOOLBAR|
    CHORDS|CHORDS3D|EQUIL|SURFACES|GRIDPOINTS|GRID|LABELS|MESH|XPOINTTESTS),

    INIT2 = (AXES|GRID),

    ALL = 0xffffffff
  };

  static const ulong size = 25;

  struct ShowFlagsEx
  {
    enum EShowFlagsEx
    {
      MESHPOINTS =    0x80000000,
      MESHELEMENTS =  0x40000000,
      MESHCELLS =     0x20000000
    };
  };

  static const ulong MARKED = (ELEMS|SEPARATORS|SOURCES|CHORDS|
                                       ShowFlagsEx::MESHCELLS|
                                       ShowFlagsEx::MESHELEMENTS);

  static const ulong EXAMINABLE = (NODES|IRRNODES|ELEMS|SURFACES|GRIDPOINTS|SEPARATORS|
                                           SOURCES|CHORDS|CHORDS3D|XPOINTTESTS|XPOINTSEGS|
                                           ShowFlagsEx::MESHCELLS|
                                           ShowFlagsEx::MESHELEMENTS|
                                           ShowFlagsEx::MESHPOINTS);

  static const ulong MOVEABLE = (NODES|IRRNODES|SURFACES|GRIDPOINTS|SOURCES|CHORDS);

  static const ulong REMOVEABLE = (NODES|IRRNODES|ELEMS|XPOINTTESTS|SURFACES|GRIDPOINTS|SOURCES|CHORDS|COMMENTS);

  static const _FlagsRec flags[];
};
typedef ShowFlags::EShowFlags ShowFlag;
typedef ShowFlags SHW;
typedef ShowFlags::ShowFlagsEx SHWX;
typedef ShowFlags::ShowFlagsEx::EShowFlagsEx ShowFlagEx;

#endif // SHOWFLAGS_H
