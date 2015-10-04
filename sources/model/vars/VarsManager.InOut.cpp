#include "VarsManager.h"

#include "../core/Model.h"

/*******************************************************************************
 * Read
 ******************************************************************************/

bool VarsManager::ReadVars( FILE* _pFile, VarSetDefPtr _pVSD )
{
  bool err = false;
  FOREACHPTRCONST( VarDefPtr, pVD, _pVSD->VarDefs() ) {
    UPtr val = ReadVar( _pFile, pVD->VarDefType() );
    if( val.IsNull() ) { err = true; continue; }
    SetVar( _pVSD, pVD, null, val );
  }
  return err;
}

bool VarsManager::ReadVars( FILE* _pFile, VarSetPtr _pVS )
{
  bool err = false;
  VarSetDefPtr pVSD = _pVS->VSD();

  FOREACHPTRCONST( VarDefPtr, pVD, pVSD->VarDefs() ) {
    if( pVD->IsMultiple() ) {

      if( HasAnyFlag( pVD->Flags(), VF::FORELEMS ) ) {
        FOREACHPTRCONST( ElementPtr, pElem, pModel->Struct()->Elements() ) {
          UPtr val = ReadVar( _pFile, pVD->VarDefType() );
          if( val.IsNull() ) { err = true; continue; }
          SetVar( pElem, pVD, _pVS, val );
        }
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORSEPARATORS ) ) {
        FOREACHPTRCONST( SeparatorPtr, pSep, pModel->Struct()->Separators() ) {
          UPtr val = ReadVar( _pFile, pVD->VarDefType() );
          if( val.IsNull() ) { err = true; continue; }
          SetVar( pSep, pVD, _pVS, val );
        }
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORSOURCES ) ) {
        FOREACHPTRCONST( SourcePtr, pSrc, pModel->Sources() ) {
          UPtr val = ReadVar( _pFile, pVD->VarDefType() );
          if( val.IsNull() ) { err = true; continue; }
          SetVar( pSrc, pVD, _pVS, val );
        }
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORCHORDS ) ) {
        FOREACHPTRCONST( ChordPtr, pChord, pModel->Struct()->Chords() ) {
          UPtr val = ReadVar( _pFile, pVD->VarDefType() );
          if( val.IsNull() ) { err = true; continue; }
          SetVar( pChord, pVD, _pVS, val );
        }
      }
    }
    else {
      UPtr val = ReadVar( _pFile, pVD->VarDefType() );
      if( val.IsNull() ) { err = true; continue; }
      SetVar( _pVS, pVD, null, val );
    }
  }
  return err;
}

UPtr VarsManager::ReadVar( FILE* _pFile, VarType _vt )
{
  char s[500];
  unsigned i,n;
  int i1,i2;

  if( HasAnyFlag( _vt, VTF::HASGROUP ) ) {
    fgets( s, sizeof( s ) - 1, _pFile );
    if( sscanf( s, "Group100 %u", &n ) != 1 )
      return UPtr();
    UPtr upValue( UPtr::LIST );

    ObjectType ot = OT::APP; /*Unknown*/
    if( HasAnyFlag( _vt, VTF::HASELEMS ) ) ot = OT::ELEMENT;
    if( HasAnyFlag( _vt, VTF::HASCHORDS ) ) ot = OT::CHORD;
    if( HasEachFlag( _vt, VT::MESH_CELLS ) ) ot = OT::MESHCELL;
    if( HasEachFlag( _vt, VT::MESH_ELEMENTS ) ) ot = OT::MESHELEMENT;

    while( n-- ) {
      fgets( s, sizeof( s ) - 1, _pFile );
      IVarItemPtr pVarItem = null;

      switch( ot ) {
      case OT::ELEMENT:
        if( sscanf( s, "%u", &i ) == 1 ) {
          IComponentIterConst itElem = pModel->Struct()->Elements().begin();
          std::advance( itElem, i );
          pVarItem = dgtype_cast< ElementPtr >( (*itElem)->GetPtr() );
        }
        break;
      case OT::CHORD:
        if( sscanf( s, "%u", &i) == 1 ) {
          IComponentIterConst itChord = pModel->Struct()->Chords().begin();
          std::advance( itChord, i );
          pVarItem = dgtype_cast< ElementPtr >( (*itChord)->GetPtr() );
        }
        break;
      case OT::MESHCELL:
        if( pModel->HasMesh() && sscanf( s, "%d" , &i1 ) == 1 ) {
          MeshCellPtr pMC = pModel->GetMesh()->FindMeshCellByNumber( i1 );
          if( pMC != NULL )
            pVarItem = dgtype_cast< MeshCellPtr >( pMC->GetPtr() );
        }
        break;
      case OT::MESHELEMENT:
        if( pModel->HasMesh() && sscanf( s, "%d %d", &i1, &i2 ) == 2 ) {
          MeshElementPtr pME = pModel->GetMesh()->FindMeshElementById( i1, i2 );
          if( pME != NULL )
            pVarItem = dgtype_cast< MeshElementPtr >( pME->GetPtr() );
        }
        break;
      default: assert(0); break;
      }

      upValue.ListRef().push_back( pVarItem );
    }
    return upValue.Move();
  }
  else {
    fgets( s, sizeof( s ) - 1, _pFile );
    return UPtr( std::string( RemoveLF( s ) ) );
  }
  return UPtr();
}


/*******************************************************************************
 * Write
 ******************************************************************************/

void VarsManager::WriteVar( FILE* _pFile, IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS ) const
{
  if( HasAnyFlag( _pVD->VarDefType(), VTF::HASGROUP ) ) {
    const char csGroupHeader[] = "Group100 %u\n";

    if( HasAnyFlag( _pVD->VarDefType(), VTF::HASELEMS ) ) {
      UPtr var = GetVar( _pOrigin, _pVD, _pVS );
      IVarItemList objects = var.ListRef(); // Check if list in var exists and use its ptr
      fprintf( _pFile, csGroupHeader, (unsigned)objects.size() );

      FOREACH_CONST( IVarItemIterConst, itElem, objects ) {
        IComponentPtr pElem = dgtype_cast< ElementPtr >( *itElem );
        fprintf( _pFile, "%u\n", (uint)IndexOf( pModel->Struct()->Elements(), pElem ) );
      }
    }
    else if( HasAnyFlag( _pVD->VarDefType(), VTF::HASCHORDS ) ) {
      UPtr var = GetVar( _pOrigin, _pVD, _pVS );
      IVarItemList objects = var.ListRef();
      fprintf( _pFile, csGroupHeader, (unsigned)objects.size() );

      FOREACH_CONST( IVarItemIterConst, itChord, objects ) {
        IComponentPtr pChord = dgtype_cast< ChordPtr >( *itChord );
        fprintf( _pFile, "%u\n", (uint)IndexOf( pModel->Struct()->Chords(), pChord ) );
      }
    }
    else if( HasAnyFlag( _pVD->VarDefType(), VTF::HAS_MESH_OBJECTS ) ) {
      UPtr var = GetVar( _pOrigin, _pVD, _pVS );
      IVarItemList objects = var.ListRef();
      fprintf( _pFile, csGroupHeader, (uint)objects.size() );

      if( _pVD->VarDefType() == VT::MESH_CELLS ) {
        FOREACH_CONST( IVarItemIterConst, itMC, objects ) {
          MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( *itMC );
          fprintf( _pFile, "%d\n", pMC->eNumber() );
        }
      }
      else {
        FOREACH_CONST( IVarItemIterConst, itME, objects ) {
          MeshElementPtr pME = dgtype_cast< MeshElementPtr >( *itME );
          fprintf( _pFile, "%d %d\n", pME->GetID_1(), pME->GetID_2() );
        }
      }
    }
    else assert(  false );
  }
  else {
    const std::string& str = GetVar( _pOrigin, _pVD, _pVS ).StringRef();
    /* if (s==NULL) fprintf(f,"\n"); else */
    fprintf( _pFile, "%s\n", str.c_str() );
  }
}



void VarsManager::WriteVars( FILE* _pFile, IVarOriginPtr _pOrigin ) const
{
  SENDER_NAME( "WriteVars" );
  VarSetPtr pVS = null;
  VarSetDefPtr pVSD = null;
  switch( _pOrigin->Type() ) {
  case OT::VARSETDEF:
    pVSD = dgtype_cast< VarSetDefPtr >( _pOrigin );
    break;
  case OT::VARSET:
    pVS = dgtype_cast< VarSetPtr >( _pOrigin );
    pVSD = pVS->VSD();
    break;
  default:
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::WRONG_VAR_OBJECT_TYPE );
    return;
  }

  fprintf( _pFile, "Vars110 %u\n", (unsigned)pVSD->VarDefsNum() );
  FOREACHPTRCONST( VarDefPtr, pVD, pVSD->VarDefs() ) {
    if( _pOrigin->Type() == OT::VARSET && ( HasAnyFlag( pVD->Flags(), VF::MULTIPLE ) ) ) {
      if( HasAnyFlag( pVD->Flags(), VF::FORELEMS ) ) {
        FOREACHPTRCONST( ElementPtr, pElem, pModel->Struct()->Elements() )
          WriteVar( _pFile, pElem, pVD, pVS );
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORSEPARATORS ) ) {
        FOREACHPTRCONST( SeparatorPtr, pSep, pModel->Struct()->Separators() )
          WriteVar( _pFile, pSep, pVD, pVS );
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORSOURCES ) ) {
        FOREACHPTRCONST( SourcePtr, pSrc, pModel->Sources() )
          WriteVar( _pFile, pSrc, pVD, pVS );
      }

      if( HasAnyFlag( pVD->Flags(), VF::FORCHORDS ) ) {
        FOREACHPTRCONST( ChordPtr, pCh, pModel->Struct()->Chords() );
          WriteVar( _pFile, pCh, pVD, pVS );
      }
    }
    else
      WriteVar( _pFile, _pOrigin, pVD, NULL );
  }
}


/*****************************************************************************
 * Output
 *****************************************************************************/

void VarsManager::GetOutputValidationFlags( ulong _outputFlags, ulong& _rValidFlags ) const
{
  if( HasAnyFlag( _outputFlags, OF::STRUCTURE | OF::TARGETS ) ) {
    FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
      FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
        if( HasAnyFlag( pVD->VarDefType(), VTF::TARGET ) ) {
          if( CheckValue( GetVar( pVS, pVD, null ), pVD->VarDefType(), null ) != 0 )
            _rValidFlags &= ~AF::TARGETS;
        }
        else if( pVD->VarDefType() == VT::STRUCTURE ) {
          if( CheckValue( GetVar( pVS, pVD, null ), pVD->VarDefType(), null ) != 0 )
            _rValidFlags &= ~AF::STRUCTURE;
        }
      }
    }
  }

  if( HasAnyFlag( _outputFlags, OF::STRUCTURE ) &&
      GetVarPtrByType( VT::STRUCTURE ) == null )
    _rValidFlags &= ~AF::STRUCTURE;

  /* Unknown code
  v=GetVarPtrByType(a,VT_TARGET1);
  if (CheckValue(a,v->val,VT_TARGET1,NULL))
    r&=~AF_VALIDTARGETS;
  v=GetVarPtrByType(a,VT_TARGET2);
  if (CheckValue(a,v->val,VT_TARGET2,NULL))
    r&=~AF_VALIDTARGETS;

  r|=AF_VALIDSTRUCTURE;
  v=GetVarPtrByType(a,VT_STRUCTURE);
  if (CheckValue(a,v->val,VT_STRUCTURE,NULL))
    r&=~AF_VALIDSTRUCTURE; -- old

  if (a->equil!=NULL && a->equil->signInside &&
    CountSurfaces(a,2) && CountSurfaces(a,1)==CountSurfaces(a,3))
    valid_flags |= AF::SURFACES;

  if (a->xpoint!=NULL)
    valid_flags |= AF::GRIDPOINTS;*/

  if( HasAnyFlag( _outputFlags, OF::VARS ) && CheckAllVars( null ) != 0 )
    _rValidFlags &= ~AF::VARS;
}

void VarsManager::OutputVars( FILE* _pFile, int _maxId ) const
{
  FOREACHPTRCONST( VarSetDefPtr, pVSD, varSetDefs ) {
    int i = 1;
    FOREACHPTRCONST( VarSetPtr, pVS, pVSD->VarSets() ) {
      bool first = true;
      IComponentList varDefsSorted = pVSD->GetSortedVarDefs();

      FOREACHPTRCONST( VarDefPtr, pVD, varDefsSorted ) {
        if( HasAnyFlag( pVD->Flags(), VF::NOEXPORT ) )
          continue;
        if( first )
          zfprintf( _pFile, "%s %d\n", pVSD->Name().c_str(), i );
        first = false;
        bool hasMultOrGroup = HasAnyFlag( pVD->Flags(), VF::MULTIPLE ) ||
                              HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP );
        zfprintf( _pFile, "%s%c", pVD->Name().c_str(), hasMultOrGroup ? '\n' : ' ' );
        if( !HasAnyFlag( pVD->Flags(), VF::MULTIPLE ) )
          OutputVar( _pFile, pVS, pVD, null );
        else {
          if( HasAnyFlag( pVD->Flags(), VF::FORELEMS | VF::FORSEPARATORS ) ) {

            for( int id = 0; id <= _maxId; id++ ) {
              IVarOriginPtr pObj = pModel->Struct()->FindObject( id );
              if( pObj != null ) {
                if( pObj->Type() == OT::ELEMENT &&
                    !HasAnyFlag( pVD->Flags(), VF::FORELEMS ) )
                  pObj = null;
                if( pObj->Type() == OT::SEPARATOR &&
                    !HasAnyFlag( pVD->Flags(), VF::FORSEPARATORS ) )
                  pObj = null;
              }
              OutputVar( _pFile, pObj, pVD, pVS );
            }
          }
          if( HasAnyFlag( pVD->Flags(), VF::FORSOURCES) )
            FOREACH_CONST( IComponentIterConst, it, pModel->Sources() ) {
              IVarOriginPtr pOrigin = dgtype_cast< SourcePtr >( *it );
              OutputVar( _pFile, pOrigin, pVD, pVS );
            }
          if( HasAnyFlag( pVD->Flags(), VF::FORCHORDS ) )
            FOREACH_CONST( IComponentIterConst, it, pModel->Struct()->Chords() ) {
              IVarOriginPtr pOrigin = dgtype_cast< ChordPtr >( *it );
              OutputVar( _pFile, pOrigin, pVD, pVS );
            }
        }
      }
      i++;
    }
  }
}

void VarsManager::OutputVar( FILE* _pFile, IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS ) const
{
  SENDER_NAME( "OutputVar" );
  ValidatePtr( _pVD, "OutputVar__" );

  if( _pOrigin == null ) {
    switch( _pVD->VarDefType() ) {
    case VT::INT:       zfprintf( _pFile, "  0\n" ); break;
    case VT::FLOAT:     zfprintf( _pFile, "  0\n" ); break;
    case VT::TEXT:      zfprintf( _pFile, "  dummy\n" ); break;
    case VT::FILENAME:  zfprintf( _pFile, "  dummy\n" ); break;
    default:
      pModel->SendMessage( MT::WINDOW, AL::ERROR, SENDER, DG3::WRONG_VAR_OBJECT_TYPE );
      return;
    }
  }
  else if( HasAnyFlag( _pVD->VarDefType(), VTF::HASELEMS ) ) {
    UPtr var = GetVar( _pOrigin, _pVD, _pVS );
    if( var.IsNull() )
      return;
    if( HasAnyFlag( _pVD->VarDefType(), VTF::STRUCTPART ) ) {
      int i = 0;
      UPtr value = GetVarPtrByType( VT::STRUCTURE )->Value().Share();
      const IVarItemList& crVarStruct = value.ListRef();
      FOREACH_CONST( IVarItemIterConst, it, crVarStruct ) {
        if( Contains( var.ListRef(), *it ) )
          zfprintf( _pFile, "  %d\n", i + 1 );
        i++;
      }
    }
    else {
      FOREACHVARITEMCONST( ElementPtr, pElem, var.ListRef() )
        zfprintf( _pFile, "  %d\n", pElem->Id() );
    }
  }
  else if( HasAnyFlag( _pVD->VarDefType(), VTF::HASCHORDS ) ) {
    UPtr var = GetVar( _pOrigin, _pVD, _pVS );
    if( var.IsNull() )
      return;
    FOREACH_CONST( IVarItemIter, it, var.ListRef() ) {
      IComponentPtr pChord = dgtype_cast< ChordPtr >( *it );
      zfprintf( _pFile, "  %d\n", IndexOf( pModel->Struct()->Chords(), pChord ) + 1 );
    }
  }
  else if( HasAnyFlag( _pVD->VarDefType(), VTF::HAS_MESH_OBJECTS ) ) {
    UPtr var = GetVar( _pOrigin, _pVD, _pVS );
    switch( _pVD->VarDefType() ) {
    case VT::MESH_CELLS: {
      FOREACHVARITEMCONST( MeshCellPtr, pMC, var.ListRef() )
        zfprintf( _pFile,"  %d\n", pMC->eNumber() );
        break;
    }
    case VT::MESH_ELEMENTS: {
      int num = 0;
      FOREACHVARITEMCONST( MeshElementPtr, pME1, var.ListRef() )
        if( !pME1->IsVertical() ) num++;
      zfprintf( _pFile, "  %d\n", num ); /* # of horizontal elements */
      FOREACHVARITEMCONST( MeshElementPtr, pME2, var.ListRef() )
        if( !pME2->IsVertical() )
          zfprintf( _pFile, "  %d\n", pModel->GetObjectId( pME2 ) );
      FOREACHVARITEMCONST( MeshElementPtr, pME3, var.ListRef() )
        if( pME3->IsVertical() )
          zfprintf( _pFile, "  %d\n", pModel->GetObjectId( pME3 ) );
      break;
    }
    case VT::MESH_H_ELEMENTS:
    case VT::MESH_V_ELEMENTS: {
      FOREACHVARITEMCONST( MeshElementPtr, pME4, var.ListRef() )
        if( !pME4->IsVertical() == !(_pVD->VarDefType() == VT::MESH_V_ELEMENTS) )
          zfprintf( _pFile, "  %d\n", pModel->GetObjectId( pME4 ) );
      break;
    }
    default:
      assert( 0 );
    }
  }
  else if( !HasAnyFlag( _pVD->VarDefType(), VTF::HASGROUP ) ) {
    UPtr var = GetVar( _pOrigin, _pVD, _pVS );
    zfprintf( _pFile, "  %s\n", (var.IsNull()) ? "" : var.StringRef().c_str() );
  }
  else
    assert( 0 );
}
