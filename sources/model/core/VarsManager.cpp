#include "VarsManager.h"

VarsManager::VarsManager( Model* _pModel ):
  pModel( _pModel )
{
  DG_HIERARCHY_INIT;
}

/* IManager implementation */
int VarsManager::ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode )
{
  char sToken[ 32 ];
  char sLine[ DG_LINE_LEN ];
  char sName[ DG_STR_LEN ], sDescr[ DG_STR_LEN ];
  char sFlags[ DG_STR_LEN ], sTypes[ DG_STR_LEN ];
  uint count;
  ulong flags;
  NPoint index;
  int minVarSets, maxVarSets;

  int errorFlags = 0;

  switch( _token ) {
  case TKN::VarSetDefs100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 )
      REPEAT( count ) {
        fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
        fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d %d\n", &minVarSets, &maxVarSets ) != 2 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        pVSD_tmp = AddVarSetDef( sName, sDescr, minVarSets, maxVarSets );

        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        std::string sLineCopy( sLine );
        char* psToken = strtok( sLine, " " );
        int token = Name2Int( psToken, TKN::names );
        if( token != TKN::VarDefs100 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        else
          errorFlags |= ReadObjects( (Token)token, sLineCopy.c_str(), _pFile, _rLine, _mode );

        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( ReadVars( _pFile, pVSD_tmp ) )
          errorFlags |= DGFE::SYNTAX;
      }
    break;
  case TKN::VarDefs100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
        fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%s %s %d %d\n",
                    sTypes, sFlags, &index.x, &index.y ) != 4 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        flags = 0;
        Str2Flags( sFlags, &flags, VarDefFlags::flags );
        VarType vt = (VarType)Name2Int( sTypes, VarTypes::flags );
        pVSD_tmp->AddVarDef( sName, sDescr, vt, flags, index );
      }
    }
    else
      errorFlags |= DGFE::SYNTAX;
    break;
  case TKN::VarSetDefs101:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 )
      REPEAT( count ) {
        fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
        fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
        std::string sText = Model::ReadText( _pFile,_rLine );
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%d %d\n", &minVarSets, &maxVarSets ) != 2 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        pVSD_tmp = AddVarSetDef( sName, sDescr, minVarSets, maxVarSets );
        pVSD_tmp->ChangeHelpString( sText );

        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        std::string sLineCopy( sLine );
        char* psToken = strtok( sLine, " " );
        int token = Name2Int( psToken, TKN::names );
        if( token != TKN::VarDefs101 &&
            token != TKN::VarDefs102) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        else
          errorFlags |= ReadObjects( (Token)token, sLineCopy.c_str(), _pFile, _rLine, _mode );

        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( ReadVars( _pFile, pVSD_tmp ) != 0 )
          errorFlags |= DGFE::SYNTAX;
      }
    break;
  case TKN::VarDefs101:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
        fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
        std::string sText = Model::ReadText( _pFile, _rLine );
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%s %s %d %d\n",
                    sTypes, sFlags, &index.x, &index.y ) != 4 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        flags = 0;
        Str2Flags( sFlags, &flags, VarDefFlags::flags );
        VarType vt = (VarType)Name2Int( sTypes, VarTypes::flags );
        pVSD_tmp->AddVarDef( sName, sDescr, vt, flags, index );
        pVSD_tmp->ChangeHelpString( sText );
      }
    }
    else
      errorFlags |= DGFE::SYNTAX;
    break;

  case TKN::VarDefs102:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sName, DG_STR_LEN - 1, _pFile ); RemoveLF( sName ); _rLine++;
        fgets( sDescr, DG_STR_LEN - 1, _pFile ); RemoveLF( sDescr ); _rLine++;
        std::string sText = Model::ReadText( _pFile, _rLine );
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%s %s %d %d\n",
                    sTypes, sFlags, &index.x, &index.y ) != 4 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        flags = 0;
        Str2Flags( sFlags, &flags, VarDefFlags::flags );
        VarType vt = (VarType)Name2Int( sTypes, VarTypes::flags );
        VarDefPtr pVD = pVSD_tmp->AddVarDef( sName, sDescr, vt, flags, index );
        pVD->ChangeHelpString( sText );
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); RemoveLF( sLine ); _rLine++;
        if( strcmp( sLine, "EnumText102" ) == 0 ) {
          sText = Model::ReadText( _pFile,_rLine );
          pVD->ChangeEnumString( sText );
        }
      }
    }
    else
      errorFlags |= DGFE::SYNTAX;
    break;

  case TKN::VarSets100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      int idVSD;
      uint vars_count;
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%u", &idVSD ) != 1 ) {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        VarSetPtr pVS = null;
        VarSetDefPtr pVSD = GetVarSetDef( idVSD );
        if( pVSD != null )
          pVS = pVSD->AddVarSet();

        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "Vars100 %u", &vars_count ) != 1 &&
            sscanf( sLine, "Vars104 %u", &vars_count ) != 1 &&
            sscanf( sLine, "Vars110 %u", &vars_count ) != 1 )
        {
          errorFlags |= DGFE::SYNTAX;
          break;
        }
        if( ReadVars( _pFile, pVS ) )
          errorFlags |= DGFE::SYNTAX;
    }
    }
    break;
  default:
    errorFlags |= DGFE::SYNTAX;
    break;
  }

  return errorFlags;
}

int VarsManager::WriteObjects( Token _token, FILE* _pFile, int _mode ) const
{
  const char* sToken = TKN::names[ _token ].s;

  switch( _token ) {
  case TKN::VarSetDefs100: break; /* obsolete */
  case TKN::VarSetDefs101: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)varSetDefs.size() );
    FOREACHPTRCONST( VarSetDefPtr, pVSD, varSetDefs ) {
      fprintf( _pFile, "%s\n%s\n", pVSD->Name().c_str(), pVSD->Descr().c_str() );
      WriteText( pVSD->Help(), _pFile );
      fprintf( _pFile, "%d %d\n", (int)pVSD->MinVarSets(), (int)pVSD->MaxVarSets() );
      pVSD_tmp = pVSD;

      WriteObjects( TKN::VarDefs102, _pFile, _mode );
      WriteVars( _pFile, pVSD );
    }
    break;
  }
  case TKN::VarDefs100: break; /* obsolete */
  case TKN::VarDefs101: break; /* obsolete */
  case TKN::VarDefs102: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)pVSD_tmp->VarDefsNum() );
    FOREACHPTRCONST( VarDefPtr, pVD, pVSD_tmp->VarDefs() ) {
      fprintf( _pFile, "%s\n%s\n", pVD->Name().c_str(), pVD->Descr().c_str() );
      WriteText( pVD->Help().c_str(), _pFile );
      fprintf( _pFile, "%s %s %d %d\n",
        Int2Name( pVD->VarDefType(), VarTypes::flags ),
        Flags2Str( pVD->Flags(), VarDefFlags::flags ),
        pVD->Column(), pVD->Row() );
      fprintf( _pFile, "EnumText102\n" );
      WriteText( pVD->EnumValues(), _pFile );
    }
    break;
  }
  case TKN::VarSets100: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)varSets.size() );
    FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
      fprintf( _pFile, "%u\n", (uint)IndexOf( varSetDefs, pVS->VSD() ) );
      WriteVars( _pFile, pVS );
    }
    break;
  }
  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }

  return 0;
}

/*****************************************************************************
 * Service
 ****************************************************************************/

int VarsManager::CompValues( VarType _vt, const UPtr& _crVal1, const UPtr& _crVal2 )
{
  if( HasAnyFlag( _vt, VTF::HASGROUP ) ) {
    if( _crVal1.IsNull() && _crVal2.IsNull() )
      return 0;
    if( _crVal1.IsNull() && _crVal2.ListRef().empty() )
      return 0;
    if( _crVal2.IsNull() && _crVal1.ListRef().empty() )
      return 0;

    assert( 0 ); /* $$$ Not supported yet */
    return 0;
  }
  else {
    if( _crVal1.IsEmpty() && _crVal2.IsEmpty() )
      return 0;
    if( _crVal1.IsNull() )
      return -1;
    if( _crVal2.IsNull() )
      return 1;
    return _crVal1.StringRef().compare( _crVal2.StringRef() );
  }
}


std::string VarsManager::GetVarValueDescr( VarType _vt, const UPtr& _crValue ) const
{
  char buf[200];
  if( HasAnyFlag( _vt, VTF::HASGROUP ) ) {
    IVarItemListPtr pList = _crValue.ListPtr();
    if( pList == null || pList->empty() )
      return pModel->GetStr( STR::NOELEMS );
      /*if (vd->varType==VT_ELEM) {
      sprintf(s,"#%d",((Elem)Group1st(g,NULL))->id);
      return s;
    } */
    if( HasAnyFlag( _vt, VTF::HASELEMS ) )
      sprintf( buf, "%d %s", (int)pList->size(), pModel->GetStr( STR::ELEM ).c_str() );
    else if( HasAnyFlag( _vt, VTF::HASCHORDS ) )
      sprintf( buf, "%d %s", (int)pList->size(), pModel->GetStr( STR::CHORD ).c_str() );
    else if( HasAnyFlag( _vt, VTF::HAS_MESH_OBJECTS ) )
      sprintf( buf, "%d %s", (int)pList->size(),
               pModel->GetStr( HasAnyFlag( _vt, VT::MESH_CELLS ) ?
                               STR::MESH_CELL : STR::MESH_ELEMENT ).c_str() );
    else
      assert( false );
    strcat( buf, "s" );
    return std::string( buf );
  }
  else {
    if( _crValue.IsNOSEL() )
      return pModel->GetStr( STR::NOSEL );
    else if( _crValue.IsDIFF() )
      return pModel->GetStr( STR::DIFF );
    else
      return _crValue.StringRef();
  }
}




/*******************************************************************************
 * Actions
 ******************************************************************************/

VarSetDefPtr VarsManager::AddVarSetDef( const std::string& _crsName, const std::string& _crsDescr,
                                        int _minVarSets, int _maxVarSets )
{
  SENDER_NAME( "AddVarSetDef" );
  VarSetDefPtr pVSD = new VarSetDef( pModel, _minVarSets, _maxVarSets );
  ActAddVarSetDef act( pModel, pVSD );

  if( act.Do() != 0 )
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::MODEL_IS_LOCKED );

  ActChangeValue( pModel, pVSD, pVSD, VarSetDef::p_name, _crsName, REDRAW, DO_AT_ONCE );
  ActChangeValue( pModel, pVSD, pVSD, VarSetDef::p_descr, _crsDescr, REDRAW, DO_AT_ONCE );
  return pVSD;
}

VarPtr VarsManager::AddVar( IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS )
{
  SENDER_NAME( "AddVar" );
  VarPtr pV = new Var( pModel, _pOrigin, _pVD, _pVS );
  ActAddVar act( pModel, pV );

  if( act.Do() != 0 )
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::MODEL_IS_LOCKED );

  return pV;
}


/*****************************************************************************
 * Set
 *****************************************************************************/

int VarsManager::SetVar( IVarOriginPtr _pOrigin, VarDefPtr _pVD, VarSetPtr _pVS, const UPtr& _crNewVal )
{
  ValidatePtr( _pOrigin, "SetVar_" );
  ValidatePtr( _pVD, "SetVar__" );

  if( pModel->IsStricted() && !_crNewVal.IsEmpty() ) {
    int i = CheckValue( _crNewVal, _pVD->VarDefType(), NULL );
    if( i != 0  ) return i;
  }
  else if( HasAnyFlag( _pVD->VarDefType(), VTF::HASGROUP ) ) {
    IVarItemListPtr pList = _crNewVal.IsNull() ? null : _crNewVal.ListPtr();
    int i = CheckGroupForType( pList, _pVD->VarDefType(), NULL );
    if( i != 0 ) return i;
  }

  VarPtr pV = _pOrigin->GetVar( _pVD, _pVS );
  if( pV != NULL && pV->Value().IsEqual( _crNewVal ) )
    return 0;

  if( pV == NULL || pV->Origin() != _pOrigin ) {
    if( _crNewVal.IsNull() )
      return 0;
    else
      pV = AddVar( _pOrigin, _pVD, _pVS );
  }

  _crNewVal.IsNull() ? pV->Delete() : pV->Change( _crNewVal );

  return 0;
}


int VarsManager::SetVarEx( VarSetPtr pVS, VarDefPtr pVD, const UPtr& _crNewVal )
{
  assert( !_crNewVal.IsNull() );
  IVarOriginList origins;
  GetVarOriginGroup( pVS, pVD, true, origins );
  int r = 0;
  FOREACH_CONST( IVarOriginIter, itOrigin, origins ) {
    r = SetVar( *itOrigin, pVD, pVS, _crNewVal );
    if( r != 0 )
      break;
  }
  return r;
}

int VarsManager::UpdateVarSetDef( VarSetDefPtr _pVSD, VarSetDefPtr _pVSD_new )
{
  //Update a VarSetDef structure
  // _pVSD!=NULL _pVSD_new==NULL  - delete pVSD
  if( _pVSD_new == NULL )
  {
    ValidatePtr(_pVSD,"UpdateVarSetDef.pVSD&vsdC");
    while( _pVSD->IsLocked() )
      pModel->DelObject( _pVSD->GetLockingObject() );
    _pVSD->Delete();
  }
  // _pVSD==NULL _pVSD_new!=NULL  - create a VarSetDef with attributes from _pVSD_new
  else if( _pVSD == NULL )
  {
    _pVSD = AddVarSetDef( _pVSD_new->Name(), _pVSD_new->Descr(),
                          _pVSD_new->MinVarSets(), _pVSD_new->MaxVarSets() );
  }
  // _pVSD!=NULL _pVSD_new!=NULL  - override parameters of pVSD with those of _pVSD_new
  else if( !_pVSD->IsEqualTo( _pVSD_new ) )
  {
    while( _pVSD->IsLocked() )
      pModel->DelObject( _pVSD->GetLockingObject() );

    _pVSD->Change( _pVSD_new->GetData() );
  }

  // If _pVSD was not deleted
  if( _pVSD != NULL )
    _pVSD->AdjustVarSetList();

  // Return: 0 = success; STR::VARSLOST = vars lost
  return 0;
}


int VarsManager::UpdateVarDef(VarDefPtr _pVD, VarDefPtr _pVD_new )
{
  int r = 0;
  // Update a VarDef structure:
  // vd!=NULL vdC==NULL  - delete vd
  if( _pVD_new == NULL ) {
    ValidatePtr( _pVD, "UpdateVar.vd&vdc" );
    while( _pVD->IsLocked() )
      pModel->DelObject( _pVD->GetLockingObject() );
    _pVD->Delete();
    return 0;
  }

  // vd==NULL vdC!=NULL  - create a VarDef with attributes from vdC
  if( _pVD == NULL ) {
    std::string c_vsd_name = _pVD_new->VSD()->GetValue( VarSetDef::p_name ).StringRef();
    VarSetDefPtr pVSD = FindVarSetDef( c_vsd_name );
    ValidatePtr( pVSD, "UpdateVar+.vsd" );

    pVSD->AddVarDef( _pVD_new->GetData() );
  }

  // vd!=NULL vdC!=NULL  - override parameters of vd with those of vdC
  else if( !_pVD->IsEqualTo( _pVD_new ) ) {
    while( _pVD->IsLocked() )
      pModel->DelObject( _pVD->GetLockingObject() );

    r = _pVD->Change( _pVD->VSD(), _pVD_new->GetData() );

    std::string c_descr = _pVD_new->GetValue( VarDef::p_descr ).StringRef();
    if( _pVD->Descr() != c_descr )
      ActChangeValue( pModel, _pVD, _pVD, VarDef::p_descr, c_descr, REDRAW, DO_AT_ONCE );
  }

  if( _pVD != NULL && !r ) {
    std::string c_enum  = _pVD_new->GetValue( VarDef::p_enum ).StringRef();
    std::string c_help  = _pVD_new->GetValue( VarDef::p_help ).StringRef();
    _pVD->ChangeHelpString( c_help );
    _pVD->ChangeEnumString( c_enum );
    if( !HasAnyFlag( _pVD->VarDefType(), VTF::HASGROUP ) ) {
      SetVar( _pVD->VSD(), _pVD, NULL, GetVar( _pVD_new->VSD(), _pVD_new, NULL ) );
    }
  }

  // Return: 0 = success; STR::VARSLOST = vars lost
  return r;
}

/*****************************************************************************
 * Get
 *****************************************************************************/

VarPtr VarsManager::GetVarPtrByType( int type ) const
{
  SENDER_NAME( "GetVarPtrByType" );
  VarSetPtr pVSx = null;
  VarDefPtr pVDx = null;
  FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
    VarDefPtr pVD = pVS->VSD()->GetVarDefByType( type );
    if( pVD != null ) {
      if( pVDx != null )
        pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::MULTIPLE_VARDEFS );
      else {
        // if new pVD would be found after this then FatalError will be involved
        pVDx = pVD;
        pVSx = pVS;
      }
    }
  }

  return (pVDx == null) ? null : pVSx->GetVar( pVDx );
}

UPtr VarsManager::GetVar( IVarOriginInPtr _inpOrigin, VarDefInPtr _inpVD, VarSetInPtr _inpVS ) const
{
  UPtr val;
  VarPtr pV = _inpOrigin->GetVar( _inpVD, _inpVS );
  if( pV != null )
    return pV->Value().Share();
    //val = pV->Value().Share();
  else if( _inpVD->HasGroup() )
    return UPtr( UPtr::LIST );
    //val.SetEmptyList(); /*GetEmptyStaticGroup()*/
  else if( _inpOrigin->Type() != OT::VARSETDEF ) {
    pV = _inpVD->VSD()->GetVar( _inpVD );
    if( pV != null )
      return pV->Value().Share();
      //val = pV->Value().Share();
    else if( _inpVD->HasGroup() )
      return UPtr( UPtr::LIST );
      //val.SetEmptyList(); /*GetEmptyStaticGroup()*/
  }

  //assert( !val.IsNull() );
  return val;
}


UPtr VarsManager::GetVarEx( const VarSetPtr pVS, const VarDefPtr pVD ) const
{
  IVarOriginList origins;
  GetVarOriginGroup( pVS, pVD, true, /*out*/ origins );
  if( origins.empty() )
    return UPtr( (void*)UPtr::NOSEL );

  UPtr val0 = GetVar( origins.front(), pVD, pVS );
  FOREACH_CONST( IVarOriginIterConst, itOrigin, origins ) {
    if( itOrigin == origins.begin() )
      continue;
    UPtr val = GetVar( *itOrigin, pVD, pVS );
    if( CompValues( pVD->VarDefType(), val0, val ) != 0 ) {
      return UPtr( (void*)UPtr::DIFF );
    }
  }
  return val0.Copy(); // val0 can be   cshared or not
}


bool VarsManager::GetVarExLocks( VarSetPtr pVS, VarDefPtr pVD, std::string& reason ) const
{
  SENDER_NAME( "GetVarExLocks" );
  reason.clear();
  if( !pVD->IsMultiple() ) {
    VarPtr pV = pVS->GetVar( pVD );
    if( pV == null )
      return false;
    //reason = pV->GetLockingObject()->Description(); // TODO: fatal error
    reason = pV->Description(); // MB: Var is no locking object itself
    return pV->IsLocked();
  }

  if( pVD->HasGroup() )
    pModel->SendMessage( FATAL_ERROR, SENDER /*FULL_SENDER*/, DG3::GROUP_ERROR );

  if( pVD->HasLockedVars( pVS, reason ) )
    return true;

  return false;
}

void VarsManager::GetVarOriginGroup( VarSetPtr pVS, VarDefPtr pVD,
                        bool markedOnly, IVarOriginList& _rOrigins ) const
{
  SENDER_NAME( "GetVarOriginGroup" );
  _rOrigins.clear();
  if( !pVD->IsMultiple() )
    return _rOrigins.push_back( pVS );

  if( pVD->HasGroup() )
    pModel->SendMessage( FATAL_ERROR, SENDER /*FULL_SENDER*/, DG3::GROUP_ERROR );

  bool forElements    = HasAnyFlag( pVD->Flags(), VF::FORELEMS );
  bool forSeparators  = HasAnyFlag( pVD->Flags(), VF::FORSEPARATORS );
  bool forSources     = HasAnyFlag( pVD->Flags(), VF::FORSOURCES );
  bool forChords      = HasAnyFlag( pVD->Flags(), VF::FORCHORDS );

  if( markedOnly ) {
    IComponentPtr FOREACHOBJCONST( pObj, pModel->MarkedObjects() ) {
      IVarOriginPtr pOrigin = null;
      switch( pObj->Type() ) {
      case OT::ELEMENT:   if( forElements )
          pOrigin = dgtype_cast< ElementPtr >( pObj ); break;
      case OT::SEPARATOR: if( forSeparators )
          pOrigin = dgtype_cast< SeparatorPtr >( pObj ); break;
      case OT::SOURCE:    if( forSources )
          pOrigin = dgtype_cast< SourcePtr >( pObj ); break;
      case OT::CHORD:     if( forChords )
          pOrigin = dgtype_cast< ChordPtr >( pObj ); break;
      default: break;
      }
      if( pOrigin != null )
        _rOrigins.push_back( pOrigin );
    }
  }
  else {
    if( forElements ) {
      FOREACHPTRCONST( ElementPtr, pElem, pModel->Elements() )
        _rOrigins.push_back( pElem );
    }
    if( forSeparators ) {
      FOREACHPTRCONST( SeparatorPtr, pSep, pModel->Separators() )
        _rOrigins.push_back( pSep );
    }
    if( forSources ) {
      FOREACHPTRCONST( SourcePtr, pSrc, pModel->Sources() )
        _rOrigins.push_back( pSrc );
    }
    if( forChords ) {
      FOREACHPTRCONST( ChordPtr, pCh, pModel->Chords() )
        _rOrigins.push_back( pCh );
    }
  }
}

VarSetDefPtr VarsManager::FindVarSetDef( const std::string& _crsName ) const
{
  FOREACHPTRCONST( VarSetDefPtr, pVSD, varSetDefs )
    if( pVSD->Name() == _crsName )
      return pVSD;
  return NULL;
}

VarSetDefPtr VarsManager::GetVarSetDef( int _n ) const
{
  IComponentIterConst itVSD = varSetDefs.begin();
  std::advance( itVSD, _n );
  if( itVSD != varSetDefs.end() )
    return dgtype_cast< VarSetDefPtr >( *itVSD );
  return null;
}

IComponentPtr VarsManager::GetLockingObject( IComponentPtr pObject ) const
{
  // Find first VarDef with group
  FOREACHPTRCONST( VarDefPtr, pVD, varDefs )
  {
    if( HasAnyFlag( pVD->VarDefType(), VTF::HASGROUP ) ) /* $$$ - didn't understand well */
    {
      VarPtr pV = pVD->GetVarWithObject( dgtype_cast< ElementPtr >( pObject ) );
      if( pV != null )
        return pV->IsLocked() ? pV->GetLockingObject() : pV;
    }
  }
  return null;
}


/*****************************************************************************
 * Targets
 *****************************************************************************/

IVarItemListPtr VarsManager::GetStructure(bool _check) const
{
  VarPtr v = GetVarPtrByType( VT::STRUCTURE );
  UPtr val = (v == null) ? UPtr() : v->Value().Share();
  if( _check && CheckValue( val, VT::STRUCTURE, null ) != 0)
    return null;
  return val.ListPtr();
}

IVarItemListPtr VarsManager::GetIntersectedTarget( const PointArray* _pLine ) const
{
  VarDefPtr pVD = null;
  VarSetPtr pVS = null;

  // Instead of RecalcGridPointSegLine
  /*double len = */CalcTargetIntersection( _pLine, &pVD, &pVS );
  //pEquil->UpdateSegment( _pGPS, len ); - not const method. TODO: updating on target change
  if( pVD == null|| pVS == null )
    return null;

  UPtr var = GetVar( pVS, pVD, pVS );
  return var.ListPtr();
}

void VarsManager::FindTargets( IVarItemListPtr _pTargets ) const
{
  _pTargets->clear();
  FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
    FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
      if( !HasAnyFlag( pVD->VarDefType(), VTF::TARGET ) )
        continue;
      UPtr var = GetVar( pVS, pVD, pVS );
      if( !var.ListRef().empty() )
        _pTargets->push_back( new VarItemListContainer( var.ListPtr() ) );
    }
  }
}

void VarsManager::FindTargetsEx(VarItemListContainerPtrArray& _rTargets ) const
{
  _rTargets.clear();
  FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
    FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
      if( HasAnyFlag( pVD->VarDefType(), VTF::TARGET ) ) {
        UPtr value = GetVar( pVS, pVD, pVS );
        int result = CheckValue( value, VT::TARGET1, null );
        if( result != 0 )
          result = result /*ERR::BADTARGETS*/;
        else if( !value.ListRef().empty() )
          _rTargets.push_back( new VarItemListContainer( value.ListPtr() ) );
      }
    }
  }
}

double VarsManager::CalcTargetIntersection( const PointArray* _pLine, VarDefPtr* _ppVD, VarSetPtr* _ppVS ) const
{
  double l, l_min = MAXDOUBLE;
  VarDefPtr pVD_min = null;
  VarSetPtr pVS_min = null;

  assert( _pLine != null );
  if( _pLine->empty() )
    return l_min;
  FOREACHPTRCONST( VarSetPtr, pVS, varSets ) {
    FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
      if( !HasAnyFlag( pVD->VarDefType(), VTF::TARGET ) )
        continue;

      UPtr val = GetVar( pVS, pVD, pVS );
      IVarItemListPtr pTargets = val.ListPtr();
      if( pTargets == null )
        continue;

      FOREACHVARITEMCONST( ElementPtr, pElem, *pTargets ) {
        PointArray line = pElem->GetLine();
        if( PolyLinesIntersect( *_pLine, line, &l, null ) != 0 )
          continue;
        if( l < l_min ) {
          l_min = l;
          pVD_min = pVD;
          pVS_min = pVS;
        }
      }
    }
  }

  if( l_min == MAXDOUBLE )
    l_min = CalcLineLength( *_pLine );

  if( _ppVD != null )
    *_ppVD = pVD_min;
  if( _ppVS != null )
    *_ppVS = pVS_min;

  return l_min;
}
