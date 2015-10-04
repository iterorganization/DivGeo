#ifndef VAR_H
#define VAR_H

#include "VarsDecl.h"
#include "VarConst.h"

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"
#include "../../common/Includes.h"
#include "../../common/strlib.h"

class Var:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(Var)
  DG_PROPERTIES { p_value = 0 };
  DG_PROPERTIES_TABLE { AddProperty( "value", &value ); }

  IVarOriginPtr pOrigin;
  VarDefPtr pVD;
  VarSetPtr pVS;

  UPtr value;

public:
  /* Constructors */
  Var( ModelPtr _pModel, IVarOriginPtr _pOrigin, VarDefPtr _pDef, VarSetPtr _pSet );
  ~Var();

  virtual void Delete();
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  void AddComponent( IVarItemPtr _pComp )
  { value.ListRef().push_back( _pComp->GetPtr() ); }

  void DeleteComponent( IVarItemPtr _pComp )
  { value.ListRef().remove( _pComp ); ReleaseObject( _pComp ); }

  bool HasComponent( IVarItemPtr _pComp ) const
  { return Contains( value.ListRef(), _pComp ); }

  /* Actions */
  void Set( IVarOriginPtr _pOrigin, const UPtr& _crNewVal );
  void Change( const UPtr& _crNewVal );
  void ChangeEx( IVarItemPtr _pItem, bool _include = true );
  void FreeValue();

  /* Getters */
  const UPtr& Value() const { return value; }
  UPtr* ValuePtr() { return &value; }

  IComponentPtr GetLockingObject() const;
  IVarOriginPtr Origin()  const { return pOrigin; }
  VarDefPtr VD()          const { return pVD; }
  VarSetPtr VS()          const { return pVS; }

  /* Setters */
  void ReleaseVarDef() { ReleaseObject( (IComponentPtr*)&pVD ); }
  void ReleaseVarSet() { ReleaseObject( (IComponentPtr*)&pVS ); }
  void ReleaseOrigin() { ReleaseObject( (IComponentPtr*)&pOrigin ); }
};

#endif
