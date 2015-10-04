#ifndef VARSMANAGER_H
#define VARSMANAGER_H

#include "../vars/VarsDecl.h"
#include "IManager.h"

//#include "Model.h"
class Model;
typedef Model* ModelPtr;

class VarsManager:
    public IManager
{
  DG_HIERARCHY_TABLE {
    RegisterType( OT::VARSETDEF,  &varSetDefs );
    RegisterType( OT::VARDEF,     &varDefs );
    RegisterType( OT::VARSET,     &varSets );
    //RegisterType( OT::VAR,          pVars );
  }

  ModelPtr pModel;

  IComponentList varSetDefs;
  IComponentList varDefs;
  IComponentList varSets;

  mutable VarSetDefPtr pVSD_tmp;

public:
  VarsManager( Model* _pModel );

  const IComponentList& VarSetDefs() const { return varSetDefs; }
  const IComponentList& VarSets()    const { return varSets; }
  const IComponentList& VarDefs()    const { return varDefs; }

  /* IManager implementation */
  virtual int ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 );
  virtual int WriteObjects( Token _token, FILE* _pFile, int _mode = 0 ) const;

  /*****************************************************************************
   * Service
   ****************************************************************************/
public:
  std::string GetVarValueDescr( VarType _vt, const UPtr& _crValue ) const;
private:
  static int CompValues( VarType _vt, const UPtr& _crVal1, const UPtr& _crVal2 );

  /*****************************************************************************
   * Read
   *****************************************************************************/
public:
  /* Call ReadVar() and SetVar() depend on variable type type */
  bool ReadVars( FILE* _pFile, VarSetDefPtr _pVSD ); /* default values */
  bool ReadVars( FILE* _pFile, VarSetPtr _pVS );     /* user values */
private:
  /* Parse dg file and load variable value: IVarItemList or std::string */
  UPtr ReadVar( FILE* _pFile, VarType _vt );

  /*****************************************************************************
   * Write
   *****************************************************************************/
private:
  void WriteVars( FILE* _pFile, IVarOriginPtr _pOrigin ) const;
  void WriteVar( FILE* _pFile, IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS ) const;

  /*****************************************************************************
   * Actions
   *****************************************************************************/
public:
  VarSetDefPtr AddVarSetDef( const std::string& _crsName, const std::string& _crsDescr,
                             int _minVarSets, int _maxVarSets );
private:
  VarPtr AddVar( IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS );

  /*****************************************************************************
   * Set
   *****************************************************************************/
public:
  int SetVar( IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS, const UPtr& _crNewVal );
  int SetVarEx( VarSetPtr _pVS, VarDefPtr _pVD, const UPtr& _crNewVal );
private:
  int UpdateVarSetDef( VarSetDefPtr _pVSD, VarSetDefPtr _pVSD_new );
  int UpdateVarDef( VarDefPtr _pVD, VarDefPtr _pVD_new );

  /*****************************************************************************
   * Get
   *****************************************************************************/
public:
  VarSetDefPtr GetVarSetDef( int _n ) const;
  IComponentPtr GetLockingObject( IComponentPtr pObject ) const;
  UPtr GetVar( IVarOriginInPtr _inpOrigin, VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const;
  VarPtr GetVarPtrByType( int type ) const;
  UPtr GetVarEx( VarSetPtr _pVS, VarDefPtr _pVD ) const; // Can create new UPtr objects
  bool GetVarExLocks( VarSetPtr _pVS, VarDefPtr _pVD, std::string& _rsReason ) const;
  void GetVarOriginGroup(VarSetPtr _pVS, VarDefPtr _pVD, bool _markedOnly, IVarOriginList& _rOrigins ) const;
private:
  VarPtr GetVarPtr( IVarOriginInPtr _inpOrigin, VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const;
  VarSetDefPtr FindVarSetDef( const std::string& _crsName ) const;

  /*****************************************************************************
   * Check
   *****************************************************************************/
public:
  /* Checks all variables for valid values. Returns the count of encountered errors */
  int CheckAllVars( VarPairList* _pErrorPairs ) const;
  int CheckVar( IComponentListPtr _pObjects, VarDefPtr _pVD, VarSetPtr _pVS, int* _pErr ) const;
  int CheckValue( const UPtr& _rValue, VarType _vt, IVarItemPtr *_ppErrObj ) const;
private:
  int CheckGroupForType( IVarItemListPtr _pObjects, VarType _vt, IVarItemPtr* _ppErrObj ) const;
  static int CheckSurfaceTargetIntersection( PointArray _points, IComponentListPtr _targets ); // Used in CheckValue() only

  /*****************************************************************************
   * Output
   *****************************************************************************/
public:
  void GetOutputValidationFlags( ulong _outputFlags, ulong& _rValidFlags ) const;
  void OutputVars( FILE* _pFile, int _maxId ) const;

private:
  void OutputVar( FILE* _pFile, IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS ) const;

  /*****************************************************************************
   * Targets
   *****************************************************************************/
public:
  IVarItemListPtr GetStructure( bool _check = false ) const;
  IVarItemListPtr GetIntersectedTarget( const PointArray* _pLine ) const;

  void FindTargets( IVarItemListPtr _pTargets ) const;
  void FindTargetsEx(VarItemListContainerPtrArray& _rTargets ) const;

  /* Find length of line where it intersects with target element. Extracted from RecalcGridPointSegLine. */
  double CalcTargetIntersection( const PointArray* _pLine, VarDefPtr* _ppVD, VarSetPtr* _ppVS ) const; // Unused. TODO:

};

typedef VarsManager* VarsManagerPtr;


template< typename SPtr, typename DPtr >
static void ConvertVarOriginList( const std::list< SPtr >& _crSrc, std::list< DPtr >& _rDest ) {
  _rDest.clear();
  FOREACH_CONST( typename std::list< SPtr >::const_iterator, it, _crSrc ) {
    DPtr pObject = null;
    switch( (*it)->Type() ) {
    case OT::ELEMENT:     pObject = dgtype_cast< ElementPtr >( *it ); break;
    case OT::CHORD:       pObject = dgtype_cast< ChordPtr >( *it ); break;
    default: break;
    }
    if( pObject != null )
      _rDest.push_back( pObject );
  }
}

#define CASTVARITEM_BODY { \
  switch( _pObject->Type() ) { \
  case OT::ELEMENT:     return dgtype_cast< ElementPtr >( _pObject ); break; \
  case OT::CHORD:       return dgtype_cast< ChordPtr >( _pObject ); break; \
  case OT::MESHCELL:    return dgtype_cast< MeshCellPtr >( _pObject ); break; \
  case OT::MESHELEMENT: return dgtype_cast< MeshElementPtr >( _pObject ); break; \
  default: return null; \
  } }
static IComponentPtr CastVarItem( IVarItemPtr _pObject ) CASTVARITEM_BODY
static IVarItemPtr CastVarItem( IComponentPtr _pObject ) CASTVARITEM_BODY
#undef CASTVARITEM_BODY

template< typename SPtr, typename DPtr >
static void ConvertVarItemList( const std::list< SPtr >& _crSrc, std::list< DPtr >& _rDest ) {
  _rDest.clear();
  FOREACH_CONST( typename std::list< SPtr >::const_iterator, it, _crSrc ) {
    DPtr pObject = CastVarItem( *it );
    if( pObject != null )
      _rDest.push_back( pObject );
  }
}

#define CASTVARORIGIN_BODY { \
  switch( _pObject->Type() ) { \
  case OT::ELEMENT:     return dgtype_cast< ElementPtr >( _pObject ); break; \
  case OT::CHORD:       return dgtype_cast< ChordPtr >( _pObject ); break; \
  case OT::SOURCE:      return dgtype_cast< SourcePtr >( _pObject ); break; \
  case OT::SEPARATOR:   return dgtype_cast< SeparatorPtr >( _pObject ); break; \
  case OT::VARSET:      return dgtype_cast< VarSetPtr >( _pObject ); break; \
  case OT::VARSETDEF:   return dgtype_cast< VarSetDefPtr >( _pObject ); break; \
  default: return null; \
  } }
static IComponentPtr CastVarOrigin( IVarOriginPtr _pObject ) CASTVARORIGIN_BODY
static IVarOriginPtr CastVarOrigin( IComponentPtr _pObject ) CASTVARORIGIN_BODY
#undef CASTVARORIGIN_BODY


#endif // VARSMANAGER_H

#include "Model.h"
