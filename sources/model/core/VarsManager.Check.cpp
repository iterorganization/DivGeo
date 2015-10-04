#include "VarsManager.h"

/*****************************************************************************
 * Check
 *****************************************************************************/

/* Check all values of a variable
   Returns     The number of encountered errors
   *pObjects  Offending object if VTF_HASGROUP;
               else objects with offending values
   *pErr       Error value or ERR::MULTIPLEERRORS if it is the case */

int VarsManager::CheckVar( IComponentListPtr _pObjects, VarDefPtr _pVD, VarSetPtr _pVS, int* _pErr ) const
{
  SENDER_NAME( "CheckVar" );
  int err = 0, r = 0;

  if( _pObjects != NULL )
    _pObjects->clear();

  if( _pErr != NULL )
    *_pErr = 0;

  if( !HasAnyFlag( _pVD->Flags(), VF::MULTIPLE ) ) {
    UPtr val = GetVar( _pVS, _pVD, _pVS );
    IVarItemPtr pErrVarItem;
    int e = CheckValue( val, _pVD->VarDefType(), &pErrVarItem );
    if( e != 0 ) {
      err = (err == 0) ? e : ERR::MULTIPLEERRORS;
      if( _pObjects != NULL && pErrVarItem != NULL ) {
        IComponentPtr pErrObject = null;
        switch( pErrVarItem->Type() ) {
        case OT::ELEMENT:     pErrObject = dgtype_cast< ElementPtr >( pErrVarItem ); break;
        case OT::CHORD:       pErrObject = dgtype_cast< ChordPtr >( pErrVarItem ); break;
        case OT::MESHELEMENT: pErrObject = dgtype_cast< MeshElementPtr >( pErrVarItem ); break;
        case OT::MESHCELL:    pErrObject = dgtype_cast< MeshCellPtr >( pErrVarItem ); break;
        default: break;
        }
        if( pErrObject != null )
          _pObjects->push_back( pErrObject );
      }
      r++;
    }
  }

  if( HasAnyFlag( _pVD->Flags(), VF::FORELEMS ) ) {
    FOREACHPTRCONST( ElementPtr, pElem, pModel->Elements() ) {
      UPtr val = GetVar( pElem, _pVD, _pVS );
      if( val.IsNull() )
        continue; //MB: error or not error?
      int e = CheckValue( val, _pVD->VarDefType(), null );
      if( e != 0 ) {
        err = (err == 0) ? e : ERR::MULTIPLEERRORS;
        if( _pObjects != null )
          _pObjects->push_back( pElem );
        r++;
      }
    }
  }

  if( HasAnyFlag( _pVD->Flags(), VF::FORSEPARATORS ) ) {
    FOREACHPTRCONST( SeparatorPtr, pSep, pModel->Separators() ) {
      UPtr val = GetVar( pSep, _pVD, _pVS );
      int e = CheckValue( val, _pVD->VarDefType(), null );
      if( e != 0 ) {
        err = (err == 0) ? e : ERR::MULTIPLEERRORS;
        if( _pObjects != null )
          _pObjects->push_back( pSep );
        r++;
      }
    }
  }

  if( HasAnyFlag( _pVD->Flags(), VF::FORSOURCES ) ) {
    FOREACHPTRCONST( SourcePtr, pSrc, pModel->Sources() ) {
      UPtr val = GetVar( pSrc, _pVD, _pVS );
      int e = CheckValue( val, _pVD->VarDefType(), null );
      if( e != 0 ) {
        err = (err == 0) ? e : ERR::MULTIPLEERRORS;
        if( _pObjects != null )
          _pObjects->push_back( pSrc );
        r++;
      }
    }
  }

  if( _pErr != NULL )
    *_pErr = err;

  if( err != 0 )
    pModel->SendMessage( LOG_WIP_WARN, SENDER, err );

  return r;
}

int VarsManager::CheckValue( const UPtr& _rValue, VarType _vt, IVarItemPtr *_ppErrObj ) const
{
  SENDER_NAME( "CheckValue" );
  double f;
  static char s1[DG_FNAME_LEN],s2[DG_FNAME_LEN];

  if( _ppErrObj != null )
    *_ppErrObj = null;

  /*if( HasAnyFlag( varType, VTF::HASELEMS ) && !_rValue.IsNull() ) {
    IComponentPtr FOREACHOBJCONST( pObj, _rValue.ToListRef() )
      if( pObj->Type() != OT::ELEMENT ) {
        *ppErrObj = pObj;
        return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::WRONGTYPE );
      }
  }
  if( HasAnyFlag( varType, VTF::HASCHORDS ) && !_rValue.IsNull() ) {
    IComponentPtr FOREACHOBJCONST( pObj, _rValue.ToListRef() )
        if( pObj->Type() != OT::CHORD ) {
      *ppErrObj = pObj;
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::WRONGTYPE );
    }
  }*/

  if( HasAnyFlag( _vt, VTF::HASGROUP ) && !_rValue.IsNull() ) {
    int r = CheckGroupForType( _rValue.ListPtr(), _vt, _ppErrObj );
    if( r != 0 ) return r;
  }

  switch( _vt ) {
  case VT::TEXT:
    return 0;

  case VT::INT: {
    int i = 0;
    if( sscanf( _rValue.StringRef().c_str(), "%d", &i ) != 1 )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::INVNUMBERS );
    if( sscanf( _rValue.StringRef().c_str(), "%s%s", s1, s2 ) != 1 )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::INVNUMBERS );
    //_rValue.SetValue( UPtr( std::string( s1 ) ) ); // MB:
    return 0;
  }

  case VT::FLOAT:
    if( sscanf( _rValue.StringRef().c_str(), "%lf", &f ) != 1)
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::INVNUMBERS );
    if( sscanf( _rValue.StringRef().c_str(), "%s%s", s1, s2 ) != 1)
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::INVNUMBERS );
    //_rValue.SetValue( UPtr( std::string( s1 ) ) ); //MB:
    return 0;

  case VT::FILENAME: {
    FILE* file = fopen( _rValue.StringRef().c_str(), "" );
    if( file == NULL )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::FILENOTFOUND, ARGS( _rValue.StringRef() ) );
    fclose( file );
    return 0;
  }

  case VT::ELEM:
    if( _rValue.IsNull() || _rValue.ListRef().empty() )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::NOELEMS );
    if( _rValue.ListRef().size() > 1 )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::MANYSELELEMS );
    return 0;

  case VT::ELEMS:
    return 0;

  case VT::TARGET1:
  case VT::TARGET2: {
    if( _rValue.IsNull() || _rValue.ListRef().empty() )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::NOELEMS );
    /*for (Surface s=AppSurface1st(a,&ix);s!=NULL;s=Next(&ix))
      if (!s->closed&&(i=CheckSurfaceTargetIntersection(s->line,value))!=0)
        return i; */
    ulong result = Element::OrderTargetElems( _rValue.ListPtr(), _ppErrObj );
    if( result != 0 )
      pModel->SendMessage( LOG_WIP_WARN, "Element::OrderTargetElems", result );
    return result;
  }

  case VT::STRUCTURE: {
    if( _rValue.IsNull() || _rValue.ListRef().empty() )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::NOELEMS );

    FOREACHPTRCONST( VarDefPtr, pVD, varDefs ) {
      if( !HasEachFlag( pVD->VarDefType(), VT::STRUCTPART ) )
        continue;
      FOREACH_CONST( VarIterConst, iV, pVD->Vars() ) {
        if( (*iV)->Value().IsNull() ) continue;
        bool result = CheckStructurePart( _rValue.ListRef(), (*iV)->Value().ListRef(), _ppErrObj );
        if( !result )
          return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::NOTSTRUCTPART );
      }
    }

    IVarItemList elemsTargets, elemsClosed, elemsBroken;
    FindTargets( &elemsTargets );
    int r = Element::OrderStructureElems( _rValue.ListPtr(), &elemsTargets, &elemsClosed, &elemsBroken, _ppErrObj, pModel->CarreOutputMode() );
    if( r == 0 ) {
      _rValue.ListRef().clear();
      MergeGroupOfGroups( _rValue.ListPtr(), &elemsTargets );
      MergeGroupOfGroups( _rValue.ListPtr(), &elemsClosed );
      MergeGroupOfGroups( _rValue.ListPtr(), &elemsBroken );
      FreeContainerList( &elemsTargets );
      FreeContainerList( &elemsClosed );
      FreeContainerList( &elemsBroken );
    }
    else
      pModel->SendMessage( LOG_WIP_WARN, "Element::OrderStructureElems", r );
    return r;
  }

  case VT::STRUCTPART: {
    if( _rValue.IsNull() || _rValue.ListRef().empty() )
      return 0;

    VarPtr pV = GetVarPtrByType( VT::STRUCTURE );
    if( CheckValue( pV->Value(), VT::STRUCTURE, NULL ) )
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::BADSTRUCTURE );
    bool result = CheckStructurePart( pV->Value().ListRef(), _rValue.ListRef(), _ppErrObj );
    return ( result ) ? 0 : pModel->SendMessage( LOG_ERROR, SENDER, ERR::NOTSTRUCTPART );
  }

  case VT::MESH_H_ELEMENTS:
  case VT::MESH_V_ELEMENTS:
    {
      if( _rValue.IsNull() )
      return 0;
      FOREACHVARITEMCONST( MeshElementPtr, pME, _rValue.ListRef() ) {
        if( !pME->IsVertical() != !(_vt == VT::MESH_V_ELEMENTS ) ) {
          *_ppErrObj = pME;
          return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::MESH_ELEMENT_DIR );
        }
      }
      return 0;
    }
  case VT::CHORDS:
  case VT::TOPVIEW:
  case VT::MESH_CELLS:
  case VT::MESH_ELEMENTS:
    return 0;

  default:
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::WRONG_VAR_OBJECT_TYPE );
    return 0;
  }
}

int VarsManager::CheckGroupForType( IVarItemListPtr _pObjects, VarType _vt, IVarItemPtr* _ppErrObj ) const
{
  SENDER_NAME( "CheckGroupForType" );
  if( _ppErrObj != NULL )
    *_ppErrObj = NULL;

  /* Detect ObjectType */
  ObjectType ot;
  if( !HasAnyFlag( _vt, VTF::HASGROUP ) )
    return 0;
  else if( HasAnyFlag( _vt, VTF::HASELEMS ) )
    ot = OT::ELEMENT;
  else if( HasAnyFlag( _vt, VTF::HASCHORDS ) )
    ot = OT::CHORD;
  else if( HasAnyFlag( _vt, VTF::HAS_MESH_OBJECTS ) ) {
    switch( _vt ) {
      case VT::MESH_CELLS:
        ot = OT::MESHCELL;
        break;

      case VT::MESH_ELEMENTS:
      case VT::MESH_H_ELEMENTS:
      case VT::MESH_V_ELEMENTS:
        ot = OT::MESHELEMENT;
        break;

      default: assert(0);
    }
  }
  else
    assert(0);

  if( _pObjects == NULL )
    return 0;

  /* Check each varItem's type */
  FOREACH_CONST( IVarItemIterConst, itVI, *_pObjects ) {
    if( ((IVarItemPtr)*itVI)->Type() != ot ) {
      if( _ppErrObj != null )
        *_ppErrObj = (*itVI);
      return pModel->SendMessage( LOG_WIP_WARN, SENDER, ERR::WRONGTYPE );
    }
  }

  return 0;
}

int VarsManager::CheckAllVars( VarPairList* pErrorPairs ) const
{
  int r = 0;
  FOREACHPTRCONST( VarDefPtr, pVD, varDefs ) {
    r += pVD->VSD()->CheckAllVars( pErrorPairs, pVD );
  }

  return r;
}


int VarsManager::CheckSurfaceTargetIntersection( PointArray _points, IComponentListPtr _targets )
{
  SENDER_NAME( "CheckSurfaceTargetIntersection" );
  if( _targets == NULL || _targets->empty() )
    return 0;

  if( _points.empty() )
    return ERR::SURFTARGETINS;

  for( PointIter itXY1 = _points.begin()+1, itXY1End = _points.end();
       itXY1 != itXY1End; ++itXY1 ) {
    Point xy = *(itXY1 - 1);
    Point xy1 = *itXY1;
    FOREACHPTRCONST( ElementPtr, pElem, (*_targets) )
      if( VIntersect( xy, xy1, pElem->Node(1)->Position(), pElem->Node(2)->Position(), null, null ) == 0 )
        return 0;
  }
  return ERR::SURFTARGETINS;
}
