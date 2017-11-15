#include "Structure.h"

#include "../core/Model.h"

Structure::Structure( ModelPtr _pModel ):
  IComponent( OT::STRUCTURE, _pModel )
{
  DG_HIERARCHY_INIT;
}


Structure::~Structure()
{
  Release();
}


NodePtr Structure::AddNode( Point _pnt, bool checkIfExists )
{
  if( checkIfExists ) {
    FOREACHPTRCONST( NodePtr, pNode, nodes )
      if( pNode->Position() == _pnt )
        return pNode;
  }

  NodePtr pN = new Node( pModel, _pnt );
  ActAddNode( pModel, pN, DO_AT_ONCE );
  return pN;
}


ElementPtr Structure::AddElem( NodePtr _pN1, NodePtr _pN2 )
{
  SENDER_NAME( "AddElem" );
  if( _pN1 == null || _pN2 == null )
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::NULL_POINTERS );

  if( _pN1 == _pN2 )
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::SAME_NODES );

  if( _pN1->IsConnectedWith( _pN2 ) )
    pModel->SendMessage( MT::WINDOW, AL::FATAL, SENDER, DG3::NODES_ALREADY_CONNECTED );

  ElementPtr pE = new Element( pModel, _pN1, _pN2 );
  ActAddElem( pModel, pE, DO_AT_ONCE );
  return pE;
}


SeparatorPtr Structure::AddSeparator( Point _position, NodePtr _pN )
{
  ValidatePtr( _pN, "AddSeparator___" );

  SeparatorPtr pS = new Separator( pModel, _position, _pN );
  ActAddSeparator( pModel, pS, DO_AT_ONCE );
  return pS;
}


/* in normal view, sets x-y coordinates.
   in top view, sets x-z coordinates, with "y" = z. */
ChordPtr Structure::AddChord( Point _p1, Point _p2, bool b3d )
{
  if( _p1 == _p2 )
    return NULL;

  ChordPtr pC = new Chord( pModel, _p1, _p2, b3d ? false : false /* a->activeAppView && (a->activeAppView->showFlags & SHW_TOPVIEW) */ );
  ActAddChord( pModel, pC, DO_AT_ONCE );
  return pC;
}


void Structure::DeleteAllSeparators()
{
  IComponentList vSeps_copy = separators;
  FOREACH_CONST( IComponentIter, itSep, vSeps_copy )
    (*itSep)->Delete();
}


ElementPtr Structure::FindElementByID( int id ) const
{
  if( id >= 0 ) {
    FOREACHPTRCONST( ElementPtr, pElem, elements )
      if( pElem->Id() == id )
        return pElem;
  }
  return null;
}

IVarOriginPtr Structure::FindObject( int _id ) const
{
  if( _id < 0 )
    return null;

  FOREACHPTRCONST( ElementPtr, pElem, elements )
    if( pElem->Id() == _id )
      return pElem;

  FOREACHPTRCONST( SeparatorPtr, pSep, separators )
    if( pSep->Id() == _id )
      return pSep;

  return null;
}

IComponentList Structure::GetMarkedElements() const
{
  IComponentList resultList = pModel->MarkedObjects();
  RestrictToType( resultList, OT::ELEMENT );
  if( resultList.empty() )
    resultList = elements;
  return resultList;
}

/******************************************************************************/


void Structure::GlueNodes( double _maxDist, bool _markedOnly, int* _prCount )
{
  /* Get all end nodes of elements */
  IComponentList elems_copy; /*gE*/
  if( _markedOnly ) {
    elems_copy = pModel->MarkedObjects();
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

  /* Create elements between closest pairs in the array, eventually recalculating pairs */
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
}


void Structure::GlueElems( double _maxDist, double _maxLen, bool _cutLonger, bool _markedOnly, int* _prCount)
{
  assert( _maxLen > 0 );

  /* The group of unprocessed elements */
  IComponentList elems_copy; /*g*/
  if( _markedOnly ) {
    elems_copy = pModel->MarkedObjects();
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
}


void Structure::GlueNormals( bool _markedOnly, int* _prCount )
{
  /* The group of unprocessed elements */
  IComponentList elems_copy; /*g*/
  if( _markedOnly ) {
    elems_copy = pModel->MarkedObjects();
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
}

void Structure::RenumberElements()
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

  pModel->SetMaxElemId( i-1 );
}

int Structure::GetNextElemId() const
{
  int maxElemId = pModel->MaxElemId();
  int unused = maxElemId - elements.size() - separators.size();
  if( unused != 0 ) {
    std::vector< bool > used_elems;
    used_elems.resize( pModel->MaxElemId() + 1, false );

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

  pModel->SetMaxElemId( ++maxElemId );
  return maxElemId;
}

IComponentList Structure::UnusedNodes() const
{
  IComponentList nodes_unused;
  FOREACHPTRCONST( NodePtr, pNode, nodes ) {
    if( !pNode->HasElements() &&
        !pNode->HasSeparators() )
      nodes_unused.push_back( pNode );
  }
  return nodes_unused;
}

NodePtr Structure::FindNode( const Point& pnt ) const
{
  FOREACHPTRCONST( NodePtr, pNode, nodes ) {
    if( pNode->Position() == pnt )
      return pNode;
  }
  return null;
}

ChordPtr Structure::FindChord( const Point& p1, const Point& p2, bool checkOrder ) const
{
  FOREACHPTRCONST( ChordPtr, pCh, chords ) {
    if( (pCh->Point_1() == p1 && pCh->Point_2() == p2) ||
        (!checkOrder && pCh->Point_1() == p2 && pCh->Point_2() == p1) )
      return pCh;
  }
  return null;
}

IComponentPtr Structure::LockedNode() const
{
  FOREACH_CONST( IComponentIterConst, it, nodes ) {
    if( (*it)->IsLocked() )
      return *it;
  }
  return null;
}


int Structure::ConvertChordsToElems( const IComponentList& _chords, IComponentPtr* ppErrObj )
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
      pElem->Mark( Contains( pModel->MarkedObjects(), (IComponentPtr)pCh ) );
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


int Structure::ConvertElemsToChords( const IComponentList& _elems, IComponentPtr* ppErrObj )
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
      return pModel->SendMessage( MT::WINDOW, AL::ERROR, SENDER, ERR::LOCKED );
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
