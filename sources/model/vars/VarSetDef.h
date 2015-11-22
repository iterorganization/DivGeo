#ifndef VARSETDEF_H
#define VARSETDEF_H

#include "../core/CoreConstants.h"
#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"
#include "../../common/Includes.h"
#include "../../common/strlib.h"

#include "VarsDecl.h"
#include "VarConst.h"

namespace Actions{
class ActDelVarSetDef;
class ActChangeVarSetDef;
}

struct VarPair
{
  VarDefPtr pVD;
  VarSetPtr pVS;
  int error;

  VarPair( VarDefPtr _pVD, VarSetPtr _pVS, int _error = 0 ):
    pVD( _pVD ), pVS( _pVS ), error( _error ) {}
};

bool VarDefCompare(const IComponentPtr _pVD1, const IComponentPtr _pVD2 );

struct VarSetDefData {
  std::string sName, sDescr;
  unsigned minVarSets, maxVarSets;

  VarSetDefData( const std::string& _crsName,
        const std::string& _crsDescr,
        unsigned _minVarSets,
        unsigned _maxVarSets ):
    sName( _crsName ),
    sDescr( _crsDescr ),
    minVarSets( _minVarSets ),
    maxVarSets( _maxVarSets ) {}

  bool operator == ( const VarSetDefData& _crOther ) const {
    return (sName == _crOther.sName) &&
           (sDescr == _crOther.sDescr) &&
           (minVarSets == _crOther.minVarSets) &&
           (maxVarSets == _crOther.maxVarSets);
  }
};

class VarSetDef:
  public IComponent,
  public IVarOrigin,
  public IValueHolder
{
  DG_OBJECT(VarSetDef)
  DG_PROPERTIES {
    p_name   = 0,
    p_descr  = 1,
    p_help   = 2
  };

  DG_PROPERTIES_TABLE {
    AddProperty( "name", &data.sName );
    AddProperty( "descr", &data.sDescr );
    AddProperty( "help", &sHelp );
  }

  VarSetDefData data;

  std::string sHelp;

  IComponentList varDefs;
  IComponentList varSets;

public:
  /* Constructors */

  VarSetDef( ModelPtr _pModel, int _minVarSets, int _maxVarSets ):
    IComponent( OT::VARSETDEF, _pModel ),
    data( "", "", _minVarSets, _maxVarSets ),
    sHelp( "" )
  { DG_PROPERTIES_INIT; }
  ~VarSetDef();

  int Change( const VarSetDefData& _crData );
  int Change( const std::string& _crsName, const std::string& _crsDescr,
      unsigned _minVarSets, unsigned _maxVarSets )
  { return Change( VarSetDefData( _crsName, _crsDescr, _minVarSets, _maxVarSets ) ); }

  /* IComponent implementation */

  virtual void Delete();

  /* IObject implementation */

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */

  void ChangeHelpString(const std::string& _crsHelp );

  VarDefPtr AddVarDef( const VarDefData& _crVdData );
  VarDefPtr AddVarDef( const std::string& _crsName, const std::string& _crsDescr,
                       VarType _varType, long _flags, const NPoint& _crIndex );

  VarSetPtr AddVarSet();

  /* Object managment */

  void IncludeVarDef( VarDefPtr pVD );
  void ExcludeVarDef( VarDefPtr _pVD );

  void IncludeVarSet( VarSetPtr pVS );
  void ExcludeVarSet( VarSetPtr _pVS );

  VarDefPtr GetVarDefByType( int type ) const;
  VarDefPtr GetVarDefByPosition( int _row, int _column ) const;
  IComponentList GetSortedVarDefs() const;
  int IndexOf( VarSetInPtr _inpVS ) const { return ::IndexOf( varSets, (IComponentInPtr)_inpVS ); }

  void AdjustVarSetList();

  /* IVarOrigin overload */

  virtual VarPtr GetVar( VarDefInPtr _inpVD, VarSetInPtr _inpVS = null ) const;

  /* Getters */

  std::string GetLayerLabel( VarSetPtr _pVS ) const; //1411
  const VarSetDefData& GetData() const { return data; }
  bool IsEqualTo( VarSetDefPtr _pVSD ) const { return data == _pVSD->data; }

  ulong VarDefsNum() const { return varDefs.size(); }
  ulong VarSetsNum() const { return varSets.size(); }

  const IComponentList& VarDefs() const { return varDefs; }
  const IComponentList& VarSets() const { return varSets; }

  IComponentPtr GetLockingObject() const;

  const std::string& Name()   const { return data.sName; }
  const std::string& Descr()  const { return data.sDescr; }
  const std::string& Help()   const { return sHelp; }

  long MinVarSets() const { return data.minVarSets; }
  long MaxVarSets() const { return data.maxVarSets; }

  bool CheckVarSetsNum() const { return data.minVarSets < varSets.size(); }
  int CheckAllVars( VarPairList* pErrorPairs, VarDefPtr pVD ) const;

  /* Setters */
private:
  friend class Actions::ActDelVarSetDef;
  void FreeLists();

  friend class Actions::ActChangeVarSetDef;
  void SetVarSetsLimits( int _minVarSets, int _maxVarSets ) {
    data.minVarSets = _minVarSets; data.maxVarSets = _maxVarSets; }

};

#endif
