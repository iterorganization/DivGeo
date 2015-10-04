#ifndef VARCONST_H
#define VARCONST_H

#include "../../common/Types.h"

/* VarDef flags */
struct VarDefFlags
{
  enum EVarDefFlags
  {
    NOEXPORT =      0x0001,
    FORELEMS =      0x0002,
    FORSEPARATORS = 0x0004,
    FORSOURCES =    0x0008,
    FORCHORDS =     0x0010,
    LAYERINDEX =    0x0020,

    MULTIPLE = ( FORELEMS | FORSEPARATORS | FORSOURCES | FORCHORDS )
  };
  
  static const _FlagsRec flags[];
};
typedef VarDefFlags::EVarDefFlags VarDefFlag;
typedef VarDefFlags VF;


/* Variable types */
struct VarTypes
{
  /* VarDef bits in the type field */
  struct VarTypeFlags
  {
    enum EVarTypeFlags
    {
      HASELEMS         = 0x0100,
      LOCKGROUP        = 0x0200,
      SINGLE           = 0x0400,
      STRUCTPART       = 0x0800,
      HASCHORDS        = 0x1000,
      HAS_MESH_OBJECTS = 0x2000,
      TARGET           = 0x4000,
      FILENAME         = 0x8000,
    
      HASGROUP         = (HASELEMS|HASCHORDS|HAS_MESH_OBJECTS)
    };
  };

  enum EVarTypes
  {
    INT =              0x00,
    FLOAT =            0x01,
    TEXT =             0x02,
    FILENAME =         (0x03|VarTypeFlags::FILENAME),
    ELEM =             (0x04|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP),
    ELEMS =            (0x05|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP),
    TARGET1 =          (0x06|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP|VarTypeFlags::TARGET),
    TARGET2 =          (0x07|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP|VarTypeFlags::TARGET),
    STRUCTURE =        (0x08|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP|VarTypeFlags::SINGLE),
    STRUCTPART =       (0x09|VarTypeFlags::HASELEMS|VarTypeFlags::LOCKGROUP|VarTypeFlags::STRUCTPART),
    CHORDS =           (0x0A|VarTypeFlags::HASCHORDS),
    MESH_CELLS =       (0x0B|VarTypeFlags::HAS_MESH_OBJECTS),
    MESH_ELEMENTS =    (0x0C|VarTypeFlags::HAS_MESH_OBJECTS),
    MESH_H_ELEMENTS =  (0x0D|VarTypeFlags::HAS_MESH_OBJECTS),
    MESH_V_ELEMENTS =  (0x0E|VarTypeFlags::HAS_MESH_OBJECTS),
    TOPVIEW =          (0x0F|VarTypeFlags::HASELEMS)
  };

  static const _NameRec flags[];
};
typedef VarTypes::EVarTypes VarType;
typedef VarTypes VT;
typedef VarTypes::VarTypeFlags::EVarTypeFlags VarTypeFlag;
typedef VarTypes::VarTypeFlags VTF;



/* Variable values
*/
#define VV_UNDEF             NULL
#define VV_DIFF              ((void*)-1L)
#define VV_NOSEL             ((void*)-2L)

#endif
