#ifndef DGDATAMODEL_TOKENNAMES_H
#define DGDATAMODEL_TOKENNAMES_H

#include "../common/Types.h"

namespace dm {

struct Tokens {
enum ETokens {
  Equil100,
  EquilHints114,
  Template100,
  Template110,
  SonnetData100,
  Mesh112,
  MeshFingerprint112,
  MeshPoint112,
  Nodes100,
  Elems100,
  Surfaces100,
  Surfaces108,
  GridPoints100,
  GridPoints108,
  XPointsEx114,
  GridPointSegs114,
  SurfaceZones114,
  SurfacesEx114,
  GridPointsEx114,
  Separators101,
  Sources104,
  Chords115,
  Chords106,
  MarkedElems100,
  MarkedSeparators104,
  MarkedSources104,
  MarkedChords110,
  MarkedMeshElements112,
  MarkedMeshCells112,
  VarSetDefs100,
  VarSetDef100, // loader
  VarDefs100,
  VarDef100, // loader
  VarSetDefs101,
  VarSetDef101, // loader
  VarDefs101,
  VarDef101, // loader
  VarDefs102,
  VarDef102, // loader
  VarSets100,
  XPoint100,
  MaxElemId101,
  CreaTime101,
  OutputMode109,
  TopoName114,
  ViewAttr103,
  ViewAttr105,
  ViewAttr115,
  OutputFlags111,
  MeshSlidingOptions112,
  MeshFileHeader113,
  Comments316,
  Properties316,

  DgFile,
  text101,
  Vars110,
  Var,
};

static const _NameRec names[];
};

typedef Tokens::ETokens Token;
typedef Tokens TKN;

} // namespace dm

#endif // DGDATAMODEL_TOKENNAMES_H

