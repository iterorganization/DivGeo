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
      {
        IComponentList vSeps_copy = separators;
        FOREACH_CONST( IComponentIter, itSep, vSeps_copy )
          (*itSep)->Delete();
        vSeps_copy.clear();
        return;
      }

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

IComponentPtr Model::GetLockingObject( IComponentPtr pObject )
{
  SENDER_NAME( "GetLockingObject" );
  switch( pObject->Type() )
  {
  case OT::NODE:
    /*n=object;*/
    break;
  case OT::SOURCE:
  case OT::SEPARATOR:
  case OT::ELEMENT:
  {
    if( !bStrict )
      assert(0);

    return pVars->GetLockingObject( pObject );
    break;
  }

  case OT::EQUIL:
    if( pFlux->HasTopology() )
      return pFlux->GetTopology()->GetLockingObject();
    else
      return null;

  case OT::XPOINTTEST:
  {
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );
    return pXPT->GetLockingObject();
  }

  case OT::SONNET:
  {
    IComponentPtr pS = separators.front();
    if( pS != NULL )
      return pS->IsLocked() ? GetLockingObject( pS ) : pS;
    break;
  }
    // VarTypes' GetLockingObject() called separately
  default:
    SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::UNKNOWN_OBJECT_TYPE );
    break;
  }  
  return NULL;
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

NodePtr Model::AddNode( Point _pnt, bool checkIfExists )
{
  if( checkIfExists ) {
    FOREACHPTRCONST( NodePtr, pNode, nodes )
      if( pNode->Position() == _pnt )
        return pNode;
  }

  NodePtr pN = new Node( this, _pnt );
  ActAddNode( this, pN, DO_AT_ONCE );
  return pN;
}


ElementPtr Model::AddElem( NodePtr _pN1, NodePtr _pN2 )
{
  SENDER_NAME( "AddElem" );
  ValidatePtr( _pN1, "AddElem_" );
  ValidatePtr( _pN2, "AddElem__" );

  if( _pN1 == _pN2 )
    SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::SAME_NODES );

  if( _pN1->IsConnectedWith( _pN2 ) )
    SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::NODES_ALREADY_CONNECTED );

  ElementPtr pE = new Element( this, _pN1, _pN2 );
  ActAddElem( this, pE, DO_AT_ONCE );
  return pE;
}

SeparatorPtr Model::AddSeparator( Point _position, NodePtr pN )
{
  ValidatePtr( pN, "AddSeparator___" );

  SeparatorPtr pS = new Separator( this, _position, pN );
  ActAddSeparator( this, pS, DO_AT_ONCE );
  return pS;
}

SourcePtr Model::AddSource( Point _position )
{
  SourcePtr pS = new Source( this, _position );
  ActAddSource( this, pS, DO_AT_ONCE );
  return pS;
}

/* in normal view, sets x-y coordinates.
   in top view, sets x-z coordinates, with "y" = z. */
ChordPtr Model::AddChord( Point _p1, Point _p2, bool b3d )
{
  if( _p1 == _p2 )
    return NULL;

  ChordPtr pC = new Chord( this, _p1, _p2, b3d ? false : false /* a->activeAppView && (a->activeAppView->showFlags & SHW_TOPVIEW) */ );
  ActAddChord( this, pC, DO_AT_ONCE );
  return pC;
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
  if( (bStrict && HasSeparators()) || pSonnetData->IsLocked() )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::LOCKED );

  IComponentList vSeps_copy = separators;
  FOREACH_CONST( IComponentIter, itSep, vSeps_copy )
    (*itSep)->Delete();
  vSeps_copy.clear();

  ActDelSonnetData act( this, pSonnetData, DO_AT_ONCE );

  return 0;
}

int Model::AppendTemplate()
{
  int i = 0;

  ValidatePtr( pTemplate, "AppendTemplate.template" );

  pTemplate->Points_Reset();
  while( !pTemplate->Points_End() ) {
    NodePtr pN1 = AddNode( pTemplate->Points_Next( true ) );
    NodePtr pN2 = AddNode( pTemplate->Points_Next( true ) );

    if( !pN1->IsConnectedWith( pN2 ) && pN1 != pN2 ) {
      AddElem( pN1, pN2 );
      i++;
    } 
    else {
      if( pN1->IsEmpty() ) {
        pN1->Delete();
        pN1 = null;
      }
      if( pN1 != pN2 && pN2->IsEmpty() ) {
        pN2->Delete();
        pN2 = null;
      }
    }
  }

  return i;
}

ElementPtr Model::FindElementByID( int id ) const
{
  if( id >= 0 ) {
    FOREACHPTRCONST( ElementPtr, pElem, elements )
      if( pElem->Id() == id )
        return pElem;
  }
  return null;
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

int Model::GetNextElemId()
{
  int unused = maxElemId - elements.size() - separators.size();
  if( unused != 0 ) {
    std::vector< bool > used_elems;
    used_elems.resize( maxElemId + 1, false );

    FOREACHPTRCONST( ElementPtr, pElem, elements ) {
      assert( pElem->Id() >= 0 && pElem->Id() <= maxElemId );
      used_elems[pElem->Id()] = true;
    }

    FOREACHPTRCONST( SeparatorPtr, pSep, separators ) {
      assert( pSep->Id() >= 0 && pSep->Id() <= maxElemId );
      used_elems[pSep->Id()] = true;
    }

    for( int i = 1; i <= maxElemId; i++ )
      if( !used_elems[i] ) {
      return i;
    }

    assert( 0 );
  }

  int i = maxElemId + 1;
  SetMaxElemId( i );

  return i;
}

void Model::SetMaxElemId( int id )
{
  ActChangeValue( this, null, this, p_max_elem_id, id, NO_REDRAW, DO_AT_ONCE );
}


int Model::ConvertChordsToElems( const IComponentList& _chords, IComponentPtr* ppErrObj )
{
  if( ppErrObj != NULL )
    *ppErrObj = NULL;

  IComponentList vChords_copy = _chords;
  FOREACHPTRCONST( ChordPtr, pCh, vChords_copy ) {
    NodePtr pN1 = AddNode( pCh->Point_1(), true );
    NodePtr pN2 = AddNode( pCh->Point_2(), true );
    if( !pN1->IsConnectedWith( pN2 ) && pN1 != pN2 ) {
      pCh->Delete();
      ElementPtr pElem = AddElem( pN1, pN2 );
      if( pElem == NULL )
        continue;
      pElem->Mark( Contains( markedList, (IComponentPtr)pCh ) );
    } 
    else {
      if( pN1->IsEmpty() ) {
        pN1->Delete();
        pN1 = null;
      }
      if( pN2->IsEmpty() ) {
        pN2->Delete();
        pN2 = null;
      }
    }
  }
  vChords_copy.clear();

  return 0;
}


int Model::ConvertElemsToChords( const IComponentList& _elems, IComponentPtr* ppErrObj )
{
  SENDER_NAME( "ConvertElemsToChords" );
  if( ppErrObj != NULL )
    *ppErrObj = NULL;

  FOREACHPTRCONST( ElementPtr, pElem, _elems ) {
    Point pnt1( pElem->Node(1)->Position() );
    Point pnt2( pElem->Node(2)->Position() );

    if( pElem->IsLocked() ) {
      if( ppErrObj != NULL )
        *ppErrObj = pElem;
      return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::LOCKED );
    }

    pElem->Delete();

    ChordPtr pCh = FindChord( pnt1, pnt2 );
    if( pCh != NULL )
      continue;
    pCh = AddChord( pnt1, pnt2 );
    if( pCh == NULL )
      continue;
    if( pElem->IsMarked() )
      pCh->Mark();
  }

  return 0;
}


int Model::ConvertTemplateToChords()
{
  SENDER_NAME( "ConvertTemplateToChords" );
  if( pTemplate == NULL )
    return SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::NOTEMPLATE );

  pTemplate->Points_Reset();
  Point xy = pTemplate->Points_Next();
  while( !pTemplate->Points_End() ) {
    Point xy1 = pTemplate->Points_Next();

    if( FindChord( xy, xy1 ) == NULL )
      AddChord( xy, xy1 );

    xy = xy1;
  }
  return 0;
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

  if( separators.empty() ) {
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
  for( unsigned i = 0; i < separators.size() + 2; i++ )
    pCellInfo->push_back( new ComponentListContainer( new IComponentList() ) );
  pCellInfo->push_back( new ComponentListContainer( null ) );

  // Fill containers
  pElem = elements0[0];
  int nodeN = 1;
  int stopN = 1;
  IComponentIterConst itSep = separators.begin();
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

IVarOriginPtr Model::FindObject( int _id ) const
{
  FOREACHPTRCONST( ElementPtr, pElem, elements )
    if( pElem->Id() == _id )
      return pElem;

  FOREACHPTRCONST( SeparatorPtr, pSep, separators )
    if( pSep->Id() == _id )
      return pSep;

  return null;
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

IComponentList Model::GetMarkedElements()
{
  IComponentList resultList = markedList;
  RestrictToType( resultList, OT::ELEMENT );
  if( resultList.empty() )
    resultList = elements;
  return resultList;
}

const IComponentList& Model::UnusedNodes() const
{
  nodes_unused.clear();
  FOREACHPTRCONST( NodePtr, pNode, nodes ) {
    if( !pNode->HasElements() &&
        !pNode->HasSeparators() )
      nodes_unused.push_back( pNode );
  }
  return nodes_unused;
}

NodePtr Model::FindNode( const Point& pnt ) const
{
  FOREACHPTRCONST( NodePtr, pNode, nodes ) {
    if( pNode->Position() == pnt )
      return pNode;
  }
  return null;
}

SourcePtr Model::FindSource( const Point& pnt ) const
{
  FOREACHPTRCONST( SourcePtr, pSrc, sources ) {
    if( pSrc->Position() == pnt )
      return pSrc;
  }
  return null;
}

ChordPtr Model::FindChord( const Point& p1, const Point& p2, bool checkOrder ) const
{
  FOREACHPTRCONST( ChordPtr, pCh, chords ) {
    if( (pCh->Point_1() == p1 && pCh->Point_2() == p2) ||
        (!checkOrder && pCh->Point_1() == p2 && pCh->Point_2() == p1) )
      return pCh;
  }
  return null;
}

IComponentPtr Model::LockedNode() const
{
  FOREACH_CONST( IComponentIterConst, it, nodes ) {
    if( (*it)->IsLocked() )
      return *it;
  }
  return null;
}

int Model::GlueNodes( double _maxDist, bool _markedOnly, int* _prCount )
{
  /* Get all end nodes of elements */
  IComponentList elems_copy; /*gE*/
  if( _markedOnly ) {
    elems_copy = markedList;
    RestrictToType( elems_copy, OT::ELEMENT );
  }
  else
    elems_copy = elements;

  IComponentList nodes_selected; /*g*/
  FOREACHPTRCONST( ElementPtr, pElem, elems_copy ) {
    if( pElem->Node(1)->ElementsCount() == 1 &&
        !Contains( nodes_selected, (IComponentPtr)pElem->Node(1) ) )
      nodes_selected.push_back( pElem->Node(1) );

    if( pElem->Node(2)->ElementsCount() == 1 &&
        !Contains( nodes_selected, (IComponentPtr)pElem->Node(2) ) )
      nodes_selected.push_back( pElem->Node(2) );
  }
  elems_copy.clear();

  /* For each node in, find its nearest node, then sort the array */
  NearestNodesList nodes_nearest; /*gN*/
  FOREACHPTR( NodePtr, pNode, nodes_selected ) {
    NearestNode nn = Node::FindNearestNode( pNode, nodes_selected, _maxDist );
    if( nn.pN1 != NULL )
      nodes_nearest.push_back( nn );
    else
      ERASE_CURRENTPTR( nodes_selected );
  }
  nodes_nearest.sort( NearestNodes_Compare );

  /* Create elements between closest pairs in the array, eventually
     recalculating pairs */
  int ctCount = 0;
  while( !nodes_nearest.empty() ) {
    NearestNode nn = nodes_nearest.front();
    ElementPtr pE = AddElem( nn.pN1, nn.pN2 );

    if( nn.pN1->IsIrregular() == STR::IRRNORMALS )
      pE->Revert();

    ctCount++;
    nodes_nearest.remove( nn );
    nodes_selected.remove( nn.pN1 );
    nodes_selected.remove( nn.pN2 );

    FOREACH( NearestNodeIter, itnn1, nodes_nearest ) {
      if( itnn1->pN1 == nn.pN2 )
        ERASE( itnn1, nodes_nearest );
      else if( itnn1->pN2 == nn.pN1 || itnn1->pN2 == nn.pN2 ) {
        ERASE( itnn1, nodes_nearest );
        NearestNode nn2 = Node::FindNearestNode( itnn1->pN1, nodes_selected, _maxDist );
        if( nn2.pN1 != null ) {
          NearestNodeIter itnn3, ENDITER = nodes_nearest.end();
          FOREACH_NODECL_CONST( itnn3, nodes_nearest )
            if( itnn3->dist > nn2.dist )
              break;
          /* MB: in dg2.ngroup elements were inserted AFTER iterator,
                 in std - BEFORE iterator, so... */
          //itnn3--;
          nodes_nearest.insert( itnn3, 1, nn2 );
        }
      }
    }
  }

  assert( nodes_nearest.empty() );
  nodes_nearest.clear();
  nodes_selected.clear();

  if( _prCount != null )
    *_prCount = ctCount;

  return 0;
}

int Model::GlueElems( double _maxDist, double _maxLen,
                      bool _cutLonger, bool _markedOnly, int* _prCount)
{
  assert( _maxLen > 0 );

  /* The group of unprocessed elements */
  IComponentList elems_copy; /*g*/
  if( _markedOnly ) {
    elems_copy = markedList;
    RestrictToType( elems_copy, OT::ELEMENT );
  }
  else
    elems_copy = elements;

  int joinedCount = 0;

  /* Process every chain */
  while( !elems_copy.empty() ) {
    ElementPtr pElemFirst = dgtype_cast< ElementPtr >( elems_copy.front() );
    IComponentList elems_chain;/*g1*/
    pElemFirst->GetChain( &elems_copy, null, &elems_chain );
    assert( !elems_chain.empty() );

    /* Remove the chain from the group of unprocessed elements */
    FOREACH_CONST( IComponentIterConst, it, elems_chain )
      elems_copy.remove( *it );

    /* Follow the whole chain */
    while( elems_chain.size() >= 1 ) {
      /* Find out how many elements at the beginning can be glued.
         The last one to be glued is stored in eMax, or NULL */

      ElementPtr pEMax = null;
      ElementPtr pE0 = dgtype_cast< ElementPtr >( elems_chain.front() );
      Point pos1_e0 = pE0->Node(1)->Position(); /*x0*/

      FOREACHPTRCONST( ElementPtr, pE, elems_chain ) {
        if( pE == pE0 )
          continue; // Begin from second element
        Point pos2_e = pE->Node(2)->Position();
        if( hypot( pos1_e0 - pos2_e ) > _maxLen )
          continue; // "TooFarAway"

        bool tooFarAway = false;
        FOREACHPTRCONST( ElementPtr, pE1, elems_chain ) {
          if( pE1 == pE )
            break; // Stop when pE reached
          Point pos2_e1 = pE1->Node(2)->Position();
          if( Point2VectorDist( pos1_e0, pos2_e, pos2_e1, null, null ) > _maxDist ) {
            tooFarAway = true;
            break;
          }
        }

        if( !tooFarAway )
          pEMax = pE;
      }

      if( pEMax != null ) {
        /* Glue elements */
        FOREACHPTR( ElementPtr, pE, elems_chain ) {
          if( pE == pEMax )
            break; // Stop when pEMax reached
          ERASE_CURRENTPTR( elems_chain );
          ElementPtr pE_new = pE->Node(2)->JoinElements();
          if( pE_new != null )
            joinedCount++;
        }
        elems_chain.remove( pEMax );
      }
      else {
        /* Remove the 1st element from the group and try to glue the rest */
        elems_chain.remove( pE0 );

        /* Cut the only remaining element, if necessary */
        double h = hypot( pE0->Node(1)->Position() - pE0->Node(2)->Position() );
        if( _cutLonger && h > _maxLen )
          pE0->Split( (int)(h / _maxLen) );
      }
    }

    /* Free what remained from the chain */
    elems_chain.clear();
  }

  elems_copy.clear();
  if( _prCount != null )
    *_prCount = joinedCount;

  return 0;
}

int Model::GlueNormals( bool _markedOnly, int* _prCount )
{
  /* The group of unprocessed elements */
  IComponentList elems_copy; /*g*/
  if( _markedOnly ) {
    elems_copy = markedList;
    RestrictToType( elems_copy, OT::ELEMENT );
  }
  else
    elems_copy = elements;

  int revCnt = 0;

  while( !elems_copy.empty() ) {
    /* Pick up some element and follow its chain back to the beginning */
    ElementPtr pE = dgtype_cast< ElementPtr >( elems_copy.front() );
    ElementPtr pE0 = pE;

    int node_i = 1;
    while( true ) {
      ElementPtr pE1 = pE->GetNextElem( &elems_copy, node_i );
      if( pE1 == null )
        break;
      if( pE1->Node(node_i) == pE->Node(node_i) )
        node_i = 3 - node_i;
      if( pE1 == pE0 )
        break;
      pE = pE1;
    }

    /* Now follow the chain towards its other end, reversing normals that
       point to a wrong directions and counting their number */
    pE0 = pE;
    node_i = 3 - node_i;
    IComponentList elems_chain; /*g1*/
    ulong reverted_count = 0;
    while (1) {
      ElementPtr pE1 = pE->GetNextElem( &elems_copy, node_i );
      elems_chain.push_back( pE );
      if( pE1 == null || pE1 == pE0 )
        break;
      if( pE1->Node(node_i) == pE->Node(node_i) ) {
        pE1->Revert();
        reverted_count++;
      }
      if( pE1->Node(node_i) == pE->Node(node_i) )
        node_i = 3 - node_i;
      pE = pE1;
    }

    /* If more than 1/2 of all normals were reversed, reverse all normals
       once more, so that they point to the MAJOR direction */

    if( reverted_count > elems_chain.size() / 2 ) {
      FOREACHPTRCONST( ElementPtr, pE2, elems_chain )
        pE2->Revert();
      reverted_count = elems_chain.size() - reverted_count;
    }

    /* Remove elements from the list of unprocessed ones and clean up */
    FOREACHPTRCONST( ElementPtr, pE2, elems_chain )
      elems_copy.remove( pE2 );
    revCnt += reverted_count;
    elems_chain.clear();
  }

  elems_copy.clear();

  if( _prCount != null )
    *_prCount = revCnt;
  return 0;
}

void Model::RenumberElements()
{
  int i = 1;
  /* Renumber separators first */
  FOREACHPTRCONST( SeparatorPtr, pSep, separators )
      pSep->ChangeId( i++ );

  /* Renumbers non-chord elements */

  IComponentList elems_copy = elements;

  /* g=CreateGroup();
    for (e=AppElem1st(a,&ix);e!=NULL;e=Next(&ix))
      if (!IsChordElem(e)) GroupAdd(g,e);
  */

  while( !elems_copy.empty() ) {
    ElementPtr pE_0 = dgtype_cast< ElementPtr >( elems_copy.front() );
    ElementPtr pE = pE_0;

    /* Find last element of chain or check it is closed */
    while( pE->Node(1)->IsRegular( &elems_copy ) ) {
      pE = pE->GetNextElem( &elems_copy, 1 );
      if( pE == pE_0)
        break;
    }

    /* In reverse order renumber and remove elements from list */
    while( pE != null ) {
      pE->ChangeId( i++ );
      pE_0 = pE;
      pE = pE_0->GetNextElem( &elems_copy, 2 );
      elems_copy.remove( pE_0 );
    }
    /* Now one chain is completely renumbered and removed. Get next chain.. */
  }
  elems_copy.clear();

  SetMaxElemId( i-1 );
}
