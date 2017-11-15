#ifndef IVARORIGIN_H
#define IVARORIGIN_H

#include "../../common/Macroes.h"
#include "IObject.h"

DG_DEFINE_QUICK(Var)
DG_DEFINE_QUICK(VarSet)
DG_DEFINE_QUICK(VarDef)

class IVarOrigin: virtual public IObject
{
  DG_OBJECT(IVarOrigin)

protected:
  VarList variables;

public:
  void IncludeVar( VarPtr _pV );
  void ExcludeVar( VarPtr _pV );

  bool HasVars()        const { return !variables.empty(); }
  ulong VarsCount()     const { return variables.size(); }
  const VarList& Vars() const { return variables; }

  void FreeVars();
  void DeleteVars();
  virtual VarPtr GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS = null ) const;

  // IObject implementation
  virtual std::string Description() const { return ""; }
  virtual std::string ShortInfo() const { return ""; }
  virtual std::string DetailedInfo() const { return ""; }

  virtual ~IVarOrigin() {}
};
DG_DEFINE_PTR(IVarOrigin)
DG_DEFINE_LIST(IVarOrigin)

#endif // IVARORIGIN_H
