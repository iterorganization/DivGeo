#include "Model.h"

void Model::DelObject( IComponentPtr obj )
{
  SENDER_NAME( "DelObject" );
  ValidatePtr(obj,"DelObject_");

  switch( obj->Type() )
  {
    case OT::NODE:
    case OT::ELEMENT:
    case OT::SURFACEEX:
    case OT::GRIDPOINTEX:
    case OT::SOURCE:
    case OT::CHORD:
    case OT::TEMPLATE:
    case OT::EQUIL:
    case OT::XPOINTTEST:
    case OT::XPOINTSEG:
    case OT::GRIDPOINTSEG:
    case OT::SURFACEZONE:
    case OT::MESH:
    case OT::VARDEF:
    case OT::VARSET:
    case OT::VARSETDEF:
      return obj->Delete();

    case OT::SONNET:
      DeleteSonnetData(); return;

    case OT::SEPARATOR:
      pStruct->DeleteAllSeparators(); return;

    case OT::VAR:
      {
        VarPtr pVar = dgtype_cast< VarPtr >( obj );
        pVars->SetVar( pVar->Origin(), pVar->VD(), pVar->VS(), UPtr() );
        return;
      }

    default:
      SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
  }
}

ulong Model::GetMarkedTypes() const
{
  ulong typeFlags = 0;
  FOREACH_CONST( IComponentIterConst, it, markedList ) {
    switch( (*it)->Type() ) {
    case OT::ELEMENT:     typeFlags |= SHW::ELEMS;          break;
    case OT::SEPARATOR:   typeFlags |= SHW::SEPARATORS;     break;
    case OT::SOURCE:      typeFlags |= SHW::SOURCES;        break;
    case OT::CHORD:       typeFlags |= SHW::CHORDS;         break;
    case OT::MESHELEMENT: typeFlags |= SHWX::MESHELEMENTS;  break;
    case OT::MESHCELL:    typeFlags |= SHWX::MESHCELLS;     break;
    default: break;
    }
  }
  return typeFlags;
}

void Model::MarkObject(IComponentPtr _pObject, bool _include )
{
  SENDER_NAME( "MarkObject" );
  ValidatePtr( _pObject, "MarkObject_" );

  switch( _pObject->Type() ) {
    case OT::ELEMENT:
    case OT::SEPARATOR:
    case OT::SOURCE:
    case OT::CHORD:
    case OT::MESHELEMENT:
    case OT::MESHCELL:
      break;
    default:
      SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
  }

  Actions::ActMarkObject( this, _pObject, _include, DO_AT_ONCE );
}

void Model::MarkVarItem( IVarItemPtr _pObject, bool _include )
{
  SENDER_NAME( "MarkObject" );
  ValidatePtr( _pObject, "MarkObject_" );

  IComponentPtr pObject;
  switch( _pObject->Type() ) {
  case OT::ELEMENT:     pObject = dgtype_cast< ElementPtr >( _pObject ); break;
  case OT::CHORD:       pObject = dgtype_cast< ChordPtr >( _pObject ); break;
  case OT::MESHELEMENT: pObject = dgtype_cast< MeshElementPtr >( _pObject ); break;
  case OT::MESHCELL:    pObject = dgtype_cast< MeshCellPtr >( _pObject ); break;
  default:
    SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
    return;
  }

  Actions::ActMarkObject( this, pObject, _include, DO_AT_ONCE );
}


//TODO: place "CViewWnd::RemoveAllViewLabels();" before
void Model::Highlight( IComponentPtr pObject, bool include )
{
  ActHighlightObject( this, pObject, include, DO_AT_ONCE );
}


// MarkGroup(status=10) => UnmarkAll() + MarkGroup(true)
void Model::MarkGroup(const IComponentList& _crGroup, bool _include )
{
  IComponentList group_copy = _crGroup;
  FOREACH_CONST( IComponentIter, itObj, group_copy )
    MarkObject( *itObj, _include );
}

void Model::MarkGroup( const IVarItemList& _crGroup, bool _include )
{
  IVarItemList group_copy( _crGroup );
  FOREACH_CONST( IVarItemIter, itObj, group_copy )
    MarkVarItem( *itObj, _include );
}


void Model::MarkGroup( ObjectType groupType, bool include )
{
  IComponentListPtr pList = SelectList( groupType );
  FOREACH_CONST( IComponentIter, itObj, *pList )
    MarkObject( *itObj, include );
}

void Model::UnhighlightAll()
{
  IComponentList highlightedList_copy = highlightedList;
  FOREACH_CONST( IComponentIter, it, highlightedList_copy )
    ActHighlightObject( this, (*it), EXCLUDE, DO_AT_ONCE );
  highlightedList_copy.clear();
}

ulong Model::AddTemplate( const std::string& _fName, Point _incr, double angle, double scale )
{
  SENDER_NAME( "AddTemplate" );
  if( pTemplate != NULL ) {
    /*i=*/pTemplate->Delete();
    /*if (i) return i;*/
  }

  const char* cpShortName = GetShortFName( _fName.data() );

  SendMessage( WND_PROCESS, FULL_SENDER,
                       DG3::LOADING_TEMPLATE_FILE, ARGS( cpShortName ) );

  TemplatePtr pT_tmp = new Template( this, _fName, TemplateParams( _incr, angle, scale ) );
  ActAddTemplate AAT( this, pT_tmp );
  ulong result = AAT.Do();
  if( result == ERR::FILENOTFOUND && !fileName.empty() ) {
    std::string fname_tmp = CombinateFilePath( fileName, _fName );
    pT_tmp->SetFileName( fname_tmp );
    result = AAT.Do();
    if( result == ERR::FILENOTFOUND ) {
      pT_tmp->SetFileName( SplitFilePath( _fName, 1 ) );
      result = AAT.Do();
      if( result == ERR::FILENOTFOUND ) {
        SendMessage( WND_ERROR, SENDER, ERR::FILENOTFOUND, ARGS( cpShortName ) );
        pT_tmp = null;
      }
    }
  }

  if( result == 0 )
    SendMessage( LOG_RESULT, FULL_SENDER, DG3::DONE );

  return result;
}

SourcePtr Model::AddSource( Point _position )
{
  SourcePtr pS = new Source( this, _position );
  ActAddSource( this, pS, DO_AT_ONCE );
  return pS;
}

CommentPtr Model::AddComment( ObjectType _ot, const Point& _crPosEnd,
  const Point& _crPos, const std::wstring& _crwsText, int _parentScene )
{
  CommentPtr pC = new Comment( this, _ot, _crPosEnd, _crPos, _crwsText, _parentScene );
  ActAddComment( this, pC, DO_AT_ONCE );
  return pC;
}


ulong Model::AddSonnetData( const std::string& _fName )
{
  SENDER_NAME( "AddSonnetData" );
  if( pSonnetData != NULL )
    DeleteSonnetData();

  const char* cpShortName = GetShortFName( _fName.data() );

  SendMessage( WND_PROCESS, FULL_SENDER,
                       DG3::LOADING_SONNETDATA, ARGS( cpShortName ) );

  SonnetDataPtr pSD = new SonnetData( _fName );
  ActAddSonnetData actAddSD( this, pSD );
  ulong result = actAddSD.Do();

  if( result == ERR::FILENOTFOUND && !fileName.empty() ) {
    std::string fName2 = SplitFilePath( _fName, 0 ); // the same path
    fName2 += SplitFilePath( fileName, 1 ); // with model name
    pSD->SetFileName( fName2 );
    result = actAddSD.Do();

    if( result == ERR::FILENOTFOUND ) {
      pSD->SetFileName( SplitFilePath( _fName, 1 ) ); // argument name only
      result = actAddSD.Do();

      if( result == ERR::FILENOTFOUND ) {
        SendMessage( WND_ERROR, SENDER, ERR::FILENOTFOUND, ARGS( cpShortName ) );
        pSD = null;
      }
    }
  }

  if( result == 0 )
    SendMessage( LOG_RESULT, FULL_SENDER, DG3::DONE );

  return result;
}

int Model::DeleteSonnetData()
{
  SENDER_NAME( "DeleteSonnetData" );
  if( (bStrict && not pStruct->Separators().empty()) || pSonnetData->IsLocked() )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::LOCKED );

  pStruct->DeleteAllSeparators();
  ActDelSonnetData( this, pSonnetData, DO_AT_ONCE );
  return 0;
}

int Model::AddMesh( const char* _fName )
{
  SENDER_NAME( "AddMesh" );

  SendMessage( WND_PROCESS, FULL_SENDER, DG3::LOADING_MESH_FILE, ARGS( _fName ) );

  int result = LoadMesh( _fName );

  if( result == ERR::FILENOTFOUND )
  {
    const char* pShortName = GetShortFName( _fName );

    if( !fileName.empty() ) {
      char pModelPath[DG_FNAME_LEN];
      strcpy( pModelPath, fileName.c_str() );
      char pPath[DG_FNAME_LEN];
      GetFilePath( pModelPath, pPath );
      strcat( pPath, pShortName );
      result = LoadMesh( pPath );
    }
    else
      result = ERR::FILENOTFOUND;

    if( result == ERR::FILENOTFOUND ) {
      result = LoadMesh( pShortName );
      if( result != 0 ) {
        SendMessage( WND_ERROR, SENDER, result, ARGS( pShortName ) );
      }
    }
  }

  if( result == 0 )
    SendMessage( LOG_RESULT, SENDER, DG3::DONE );

  return result;
}

void Model::SetMaxElemId( int id )
{
  ActChangeValue( this, null, this, p_max_elem_id, id, NO_REDRAW, DO_AT_ONCE );
}




ComponentListContainerPtrArray* Model::CreateCellsInfo( int* pErr, IComponentPtr* ppObj ) const
{
  SENDER_NAME( "CreateCellsInfo" );
  // Errors
  int errBuf;
  if( pErr == null )
    pErr = &errBuf;
  *pErr = 0;

  IComponentPtr pObjBuf;
  if( ppObj == null )
    ppObj = &pObjBuf;
  *ppObj = null;

  if( pSonnetData == null ) {
    *pErr = SendMessage( LOG_WARNING, SENDER, ERR::NOSONNET );
    return null;
  }

  if( pStruct->Separators().empty() ) {
    *pErr = SendMessage( LOG_WARNING, SENDER, ERR::NOSEPARATORS );
    return NULL;
  }

  // Select elements
  IVarItemListPtr pElements = pVars->GetStructure( true );
  if( pElements == null ) {
    *pErr= SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::BADSTRUCTURE );
    return null;
  }

  ElementPtr elements0[4];
  for( int i = 0; i < 4; i++ )
    elements0[i] = null;

  NPoint sdSize = pSonnetData->Size();
  ElementPtr pElem = dgtype_cast< ElementPtr >( pElements->front() );
  for( int i = pElements->size(); i != 0; i-- ) {

    int index = 4;
    Point pt0 = pSonnetData->GetPoint( index );
    Point pt1 = pSonnetData->GetPoint( index + 1 );
    if( !VIntersect( pElem->Node(1)->Position(), pElem->Node(2)->Position(), pt0 * 2 - pt1, pt1 * 2 - pt0, null, null ) )
      elements0[3] = pElem;
    
    index = sdSize.x * 4 - 8;
    pt0 = pSonnetData->GetPoint( index );
    pt0 = pSonnetData->GetPoint( index + 1 );
    if( !VIntersect( pElem->Node(1)->Position(), pElem->Node(2)->Position(), pt0 * 2 - pt1, pt1 * 2 - pt0, null, null ) )
      elements0[2] = pElem;

    index = sdSize.x * (sdSize.y - 1) * 4 + 6;
    pt0 = pSonnetData->GetPoint( index );
    pt0 = pSonnetData->GetPoint( index + 1 );
    if( !VIntersect( pElem->Node(1)->Position(), pElem->Node(2)->Position(), pt0 * 2 - pt1, pt1 * 2 - pt0, null, null ) )
      elements0[0] = pElem;

    index = sdSize.x * sdSize.y * 4 - 6;
    pt0 = pSonnetData->GetPoint( index );
    pt0 = pSonnetData->GetPoint( index + 1 );
    if( !VIntersect( pElem->Node(1)->Position(), pElem->Node(2)->Position(), pt0 * 2 - pt1, pt1 * 2 - pt0, null, null ) )
      elements0[1] = pElem;

    pElem = pElem->GetNextElem( pElements, 2 );
  }

  for( int i = 0; i < 4; i++ )
    if( elements0[i] == NULL ) {
    *pErr = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::SONNETSTRUCTINS );
    return NULL;
  }

  unsigned elems_num = 0;
  pElem = elements0[0];
  for( int i = 0; i <= 4; ) {
    if( pElem == elements0[i%4] ) {
      i++;
      continue;
    }
    pElem = pElem->GetNextElem( pElements, 1 );
    elems_num++;
  }
  if( elems_num != pElements->size() ) {
    *pErr = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::SONNETSTRUCTINS );
    return NULL;
  }

  // Prepare containers
  ComponentListContainerPtrArray* pCellInfo = new ComponentListContainerPtrArray(); /* size = separators.size() + 3 */
  for( unsigned i = 0; i < pStruct->Separators().size() + 2; i++ )
    pCellInfo->push_back( new ComponentListContainer( new IComponentList() ) );
  pCellInfo->push_back( new ComponentListContainer( null ) );

  // Fill containers
  pElem = elements0[0];
  int nodeN = 1;
  int stopN = 1;
  IComponentIterConst itSep = pStruct->Separators().begin();
  SeparatorPtr pSep = dgtype_cast< SeparatorPtr >( *itSep );
  int i = 0;
  int j = 0;

  for( ComponentListContainerPtrItem itGroup = pCellInfo->begin(); (*itGroup)->pList != null; ) {
    if( !j++ )
      (*itGroup)->pList->push_back( pElem );

    if( pElem != elements0[stopN] ) {
      if( pSep != NULL && pElem->Node(nodeN)->HasSeparator( pSep ) ) {
        (*itGroup)->pList->push_back( pSep );
        ComponentListContainerPtrItem itNext = itGroup;
        itNext++;
        (*itNext)->pList->push_back( pSep );
        itSep++;
        pSep = dgtype_cast< SeparatorPtr >( *itSep );
        i++;
        continue;
      }
      if( pElem->Node(nodeN)->SeparatorsCount() != j - 1 ) {
        FOREACH_CONST( ComponentListContainerPtrItem, it, (*pCellInfo) )
          delete (*it)->pList;
        FreeContainersArray( *pCellInfo );
        delete pCellInfo;
        *pErr = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::SEPARATORORDER );
        *ppObj = pElem->Node(nodeN);
        return NULL;
      }
      pElem = pElem->GetNextElem( pElements, nodeN );
      j = 0;
    }
    else if( stopN == 1 ) {
      pElem = elements0[3];
      nodeN = 2;
      stopN = 2;
      i++;
      j = 0;
    }
    else if( stopN == 2 ) {
      i++;
      if( (*itGroup)->pList != null ) {
        FOREACH_CONST( ComponentListContainerPtrItem, it, (*pCellInfo) )
          delete (*it)->pList;
        FreeContainersArray( *pCellInfo );
        delete pCellInfo;
        *pErr = SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::INTERNALCELLS );
        return NULL;
      }
    }
    else 
      assert( false );
  }

  return pCellInfo;
}


void Model::ShowLockReasonOf( IComponentPtr _pObject ) const
{
  pAgent->send( MT::WINDOW, AL::ERROR, sender_name, ERR::USEDBY,
                   ARGS( _pObject->Description() ) );
}


int Model::GetObjectId( const IComponentPtr _pObj ) const
{
  int vL = MAXINT;
  int vR = MAXINT;

  switch( _pObj->Type() ) {
  case OT::ELEMENT:
    return dgtype_cast< ElementPtr >( _pObj )->Id();
  case OT::SEPARATOR:
    return dgtype_cast< SeparatorPtr >( _pObj )->Id();
  case OT::MESHELEMENT: {
    MeshElementPtr pME = dgtype_cast< MeshElementPtr >( _pObj );
    for( int i = 0; i < 2; i++ ) {
      if( pME->Cell(i) != null ) {
        if( pME->Cell(i)->Element( pME->IsVertical() ? MCN_LEFT : MCN_TOP ) == pME )
          vR = pME->Cell(i)->eNumber();
        else if( pME->Cell(i)->Element( pME->IsVertical() ? MCN_RIGHT : MCN_BOTTOM ) == pME )
          vL = pME->Cell(i)->eNumber();
        }
    }
    if( vR != MAXINT )
      return vR;
    else if( vL != MAXINT )
      return -vL - 1;
    else
      assert( 0 );
  }
  default:
      assert( 0 );
  }
  return 0;
}


SourcePtr Model::FindSource( const Point& pnt ) const
{
  FOREACHPTRCONST( SourcePtr, pSrc, sources ) {
    if( pSrc->Position() == pnt )
      return pSrc;
  }
  return null;
}
