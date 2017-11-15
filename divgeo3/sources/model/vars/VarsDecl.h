#include <list>

#include "Var.h"
class Var;
//DG_DEFINE_PTR(Var) //TODO: Strange compiler behaviour
typedef Var* VarPtr;
typedef const Var*       VarConstPtr;
typedef const Var* const VarConstPtrConst;
typedef const Var* const VarInPtr;
//DG_DEFINE_LIST(Var)
typedef std::list< VarPtr >      VarList;
typedef VarList*                 VarListPtr;
typedef VarList::iterator        VarIter;
typedef VarList::const_iterator  VarIterConst;

#include "VarDef.h"
class VarDef;
DG_DEFINE_PTR(VarDef)
DG_DEFINE_LIST(VarDef)
struct VarDefData;

#include "VarSet.h"
class VarSet;
DG_DEFINE_PTR(VarSet)
DG_DEFINE_LIST(VarSet)
//typedef std::pair< VarDefPtr, VarSetPtr > VarPair;
struct VarPair;
typedef std::list< VarPair > VarPairList;

#include "VarSetDef.h"
class VarSetDef;
DG_DEFINE_PTR(VarSetDef)
DG_DEFINE_LIST(VarSetDef)
struct VarSetDefData;
