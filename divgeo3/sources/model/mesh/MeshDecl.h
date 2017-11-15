#include "Mesh.h"
class Mesh;
typedef Mesh* MeshPtr;

#include "MeshPoint.h"
class MeshPoint;
DG_DEFINE_PTR(MeshPoint)
DG_DEFINE_LIST(MeshPoint)
DG_DEFINE_ARRAY(MeshPoint)

#include "MeshElement.h"
class MeshElement;
DG_DEFINE_PTR(MeshElement)
DG_DEFINE_LIST(MeshElement)
DG_DEFINE_ARRAY(MeshElement)

#include "MeshCell.h"
class MeshCell;
DG_DEFINE_PTR(MeshCell)
DG_DEFINE_LIST(MeshCell)
DG_DEFINE_ARRAY(MeshCell)

struct MeshCellData;
DG_DEFINE_PTR(MeshCellData)
DG_DEFINE_ARRAY(MeshCellData)
