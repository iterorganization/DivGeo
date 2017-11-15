#include "VarConst.h"


const _FlagsRec VarDefFlags::flags[] = {
  _FlagsRec( NOEXPORT,      'n' ),
  _FlagsRec( FORELEMS,      'm' ),
  _FlagsRec( FORSEPARATORS, 'S' ),
  _FlagsRec( FORSOURCES,    's' ),
  _FlagsRec( FORCHORDS,     'h' ),
  _FlagsRec( LAYERINDEX,    'i' ),
  _FlagsRec( 0, 0 )
};


const _NameRec VarTypes::flags[] = {
  _NameRec( INT,             "Int" ),
  _NameRec( FLOAT,           "Float" ),
  _NameRec( TEXT,            "Text" ),
  _NameRec( FILENAME,        "Filename" ),
  _NameRec( ELEM,            "Elem" ),
  _NameRec( ELEMS,           "Elems" ),
  _NameRec( STRUCTPART,      "StructPart" ),
  _NameRec( TARGET1,         "Target1" ),
  _NameRec( TARGET2,         "Target2" ),
  _NameRec( STRUCTURE,       "Structure" ),
  _NameRec( ELEMS,           "Chords" ),         /* To fix problems with 'dirty' files */
  _NameRec( CHORDS,          "SetOfChords" ),
  _NameRec( MESH_CELLS,      "SetOfMeshCells" ),
  _NameRec( MESH_ELEMENTS,   "SetOfMeshElements" ),
  _NameRec( MESH_H_ELEMENTS, "SetOfMeshElementsH" ),
  _NameRec( MESH_V_ELEMENTS, "SetOfMeshElementsV" ),
  _NameRec( TOPVIEW,         "TopViewObjects" ),
  _NameRec( 0, NULL ),
};