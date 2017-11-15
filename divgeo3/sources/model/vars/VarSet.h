#ifndef VARSET_H
#define VARSET_H

#include "../core/IComponent.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"
#include "../../common/Includes.h"
#include "../../common/strlib.h"

#include "VarsDecl.h"
#include "VarConst.h"

class VarSet:
  public IComponent,
  public IVarOrigin
{
  DG_OBJECT(VarSet)

  VarSetDefPtr pVarSetDef;
  VarList associated;

public:
  /* Constructors */
  VarSet( ModelPtr _pModel ):
    IComponent( OT::VARSET, _pModel ),
    pVarSetDef( null ) {}
  ~VarSet();

  IComponentPtr GetLockingObject() const;
  virtual void Delete();

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  void IncludeToVSD();
  void ExcludeFromVSD();
  void ReleaseVSD();

  /* IVarOrigin overload */
  virtual VarPtr GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS = null ) const;

  /* Getters */
  VarSetDefPtr VSD() const { return pVarSetDef; }

  /* Setters */
  void SetVarSetDef( VarSetDefPtr _pVsd );

  void IncludeAssociated( VarPtr _pVar );
  void ExcludeAssociated( VarPtr _pVar );
};

#endif
