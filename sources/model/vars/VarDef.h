#ifndef VARDEF_H
#define VARDEF_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"
#include "../../common/Includes.h"
#include "../../common/strlib.h"

#include "VarsDecl.h"
#include "VarConst.h"

namespace Actions {
class ActDelVarDef;
class ActChangeVarDef;
}

struct VarDefData
{
  std::string sName;
  std::string sDescr;
  VarType varType;
  long flags;
  NPoint index; // [row, col]

  VarDefData( const std::string& _crsName,
        const std::string& _crsDescr,
        VarType _varType, long _flags, const NPoint& _crIndex ):
    sName( _crsName ),
    sDescr( _crsDescr ),
    varType( _varType ),
    flags( _flags ),
    index( _crIndex ) { }

  bool operator == ( const VarDefData& _crOther ) const
  { return  (sName == _crOther.sName) &&
            (sDescr == _crOther.sDescr) &&
            (varType == _crOther.varType) &&
            (flags == _crOther.flags) &&
            (index == _crOther.index); }
};


class VarDef:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(VarDef)
  DG_PROPERTIES
  {
    p_name   = 0,
    p_descr  = 1,
    p_help   = 2,
    p_enum   = 3
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "name", &data.sName );
    AddProperty( "descr", &data.sDescr );
    AddProperty( "help", &help );
    AddProperty( "enum", &enumValues );
  }

  VarSetDefPtr pVarSetDef;
  VarList vars;

  VarDefData data;
  std::string help;
  std::string enumValues;

public:
  /* Constructors */

  VarDef( ModelPtr _pModel, VarSetDefPtr _pVSD, VarType _varType,
          long _flags, const NPoint& _crIndex );
  ~VarDef();

  /* IComponent implementation */

  virtual void Delete();

  /* IObject implementation */

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Object managment */

  void IncludeToVSD();
  void ExcludeFromVSD();
  void ReleaseVSD();

  void IncludeVar( VarPtr pV );
  void ExcludeVar( VarPtr pV );

  /* Actions */

  int Change( VarSetDefPtr _pVSD, const VarDefData& _crOther );
  int Change( VarSetDefPtr _pVSD, VarType _varType, long _flags, const NPoint& _crIndex ) {
    return Change( _pVSD, VarDefData( data.sName, data.sDescr, _varType, _flags, _crIndex ) ); }

  void ChangeHelpString( const std::string& _help );
  void ChangeEnumString( const std::string& _enum );

  /* Getters */
  const VarDefData& GetData() const { return data; }
  bool IsEqualTo( VarDefPtr _pVD ) const { return data == _pVD->data; }

  const std::string& Name()   const { return data.sName; }
  const std::string& Descr()  const { return data.sDescr; }
  const std::string& Help()   const { return help; }

  const std::string& EnumValues() const { return enumValues; }

  VarSetDefPtr VSD() const { return pVarSetDef; }

  const NPoint& Index() const { return data.index; }
  int Row()     const { return data.index.y; }
  int Column()  const { return data.index.x; }
  
  bool HasVars() const { return !vars.empty(); }
  const VarList& Vars() const { return vars; }

  long Flags()         const { return data.flags; }
  bool IsMultiple()    const { return HasAnyFlag( data.flags, VF::MULTIPLE ); }
  VarType VarDefType() const { return data.varType; }
  bool HasGroup()      const { return HasAnyFlag( data.varType, VTF::HASGROUP ); }

  VarPtr GetVarWithObject( IVarItemPtr _pObj ) const;
  IComponentPtr GetLockingObject() const;
  bool HasLockedVars(VarSetPtr pVS, std::string& reason ) const;

  /* Setters */
private:
  friend class Actions::ActDelVarDef;
  void FreeLists();

  friend class Actions::ActChangeVarDef;
  void SetParameters( VarSetDefPtr _pVSD, const VarDefData& _crOther );

};

#endif
