#include "Element.h"
#include "../core/Model.h"

Element::Element( ModelPtr _pModel, NodePtr _pN1, NodePtr _pN2 ): 
  IComponent( OT::ELEMENT, _pModel )
{ 
  nodes[0] = null; 
  nodes[1] = DG_SHARE( _pN1 );
  nodes[2] = DG_SHARE( _pN2 );
  id = pModel->GetNextElemId();
}

Element::~Element()
{
  Lock();
  if( nodes[1] != null ) {
    nodes[1]->ExcludeElement( this );
    ReleaseObject( (IComponentPtr*)&nodes[1] );
  }
  if( nodes[2] != null ) {
    nodes[2]->ExcludeElement( this );
    ReleaseObject( (IComponentPtr*)&nodes[2] );
  }
  FreeVars();
  FreeContainers();
  Unlock();
}

std::string Element::Description() const
{ return pModel->GetStr( STR::ELEM ); }

std::string Element::ShortInfo() const
{ return std::string( "[" ) + id + "]"; }

std::string Element::DetailedInfo() const
{ return Description() + " [" + id + "] "/* +
      nodes[1]->Position().ToString() + " - " + nodes[2]->Position().ToString()*/; }

double Element::DistToPoint(const Point &_crPnt, int*_pPosFlag ) const
{ return Point2VectorDist( nodes[1]->Position(), nodes[2]->Position(), _crPnt, _pPosFlag, null ); }

bool Element::InRectangle( const Point& _p1, const Point& _p2 ) const
{ return VectorInRectangle( nodes[1]->Position(), nodes[2]->Position(), _p1, _p2 ); }

Point Element::CentralPoint() const
{ return (nodes[1]->Position() + nodes[2]->Position()) / 2.; }

void Element::Delete()
{
  SENDER_NAME( "Element::Delete" );
  if( pModel->HasHighlighted( this ) )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_HIGHLIGHTED );
  Mark( false );

  DeleteVars();
  DeleteContainers();

  NodePtr pN1 = nodes[1];
  NodePtr pN2 = nodes[2];

  ActDelElem( pModel, this, DO_AT_ONCE );

  if( pN1->IsEmpty() )
    pN1->Delete();

  if( pN2->IsEmpty() )
    pN2->Delete();
}

void Element::ExcludeNode( NodePtr _pN )
{
  if( nodes[1] == _pN )
    ReleaseObject( &nodes[1] );
  else if( nodes[2] == _pN )
    ReleaseObject( &nodes[2] );
}

double Element::Length() const
{
  return hypot( nodes[1]->X() - nodes[2]->X(), nodes[1]->Y() - nodes[2]->Y() );
}

/* Order elements for a Target variable
   Return: error code or 0 for success
   errObj!=NULL: in case of error *errObj points to an object causing the error
*/

int Element::OrderTargetElems( IVarItemListPtr _pElems, IVarItemPtr* _ppErrObj,
                               ElementPtr* _ppElem1, ElementPtr* _ppElem2 )
{
  SENDER_NAME( "Element::OrderTargetElems" );
  ValidatePtr( _pElems, "OrderTargetElems" );

  IVarItemPtr pErrBuf;
  if( _ppErrObj == NULL )
    _ppErrObj = &pErrBuf;
  *_ppErrObj = NULL;

  if( _pElems->size() < 2 )
    return ERR::MINTARGETELEMS;

  ElementPtr pElemSelected = dgtype_cast< ElementPtr >( _pElems->front() );
  if( pElemSelected == null ) // ?? //TODO:
    return ERR::NOELEMS;

  /* Convert to IComponentList */
  IComponentList elems_ic;
  FOREACHVARITEMCONST( ElementPtr, pElem, *_pElems )
    elems_ic.push_back( pElem );

  int i = 0;
  /* Check if chain is closed */
  for( i = elems_ic.size(); i != 0 && pElemSelected->Node(1)->IsRegular( &elems_ic ); i-- )
    pElemSelected = pElemSelected->GetNextElem( &elems_ic, 1 );
  if( i == 0 )
    return ERR::CLOSED;

  if( !pElemSelected->Node(1)->IsEndNode( &elems_ic ) ) {
    *_ppErrObj = pElemSelected;/*->Node(1);*/
    return ERR::IRREGULAR;
  }

  if( _ppElem1 != null )
    *_ppElem1 = pElemSelected;

  _pElems->clear();

  while( pElemSelected->Node(2)->IsRegular( &elems_ic ) ) {
    _pElems->push_back( pElemSelected );
    pElemSelected = pElemSelected->GetNextElem( &elems_ic, 2 );
  }
  _pElems->push_back( pElemSelected );


  if( _pElems->size() != elems_ic.size() ) {
    *_ppErrObj = pElemSelected;/*->Node(2);*/
    _pElems->clear();
    return pElemSelected->Node(2)->IsEndNode( &elems_ic ) ? ERR::BROKEN : ERR::IRREGULAR;
  }

  if( _ppElem2 != null )
    *_ppElem2 = pElemSelected;
  return 0;
}

/* renamed:
  g0 - pElems
  e - pElemCurrent (while)
  e0 - pElemSelected (while)
  g - pElemsTmp
  g1 ---
  gt - ElemTargets
  gt1 - pElemTargetsTmp (for)
*/

/* Order elements for a Structure variable */
/* Arguments: */
/*   g0      unformatted group of elements */
/*   errObj  pointer to pointer to the object causing an error */
/* Returns 0 if no error, otherwise error code */
/*   errObj  points to the object causing an error */
/*   g0      contains formatted group */


int Element::OrderStructureElems(
    IVarItemListPtr _pElems,
    IVarItemListPtr _pTargets /* in-out */,
    IVarItemListPtr _pClosed,
    IVarItemListPtr _pBroken,
    IVarItemPtr* _ppErrObj, bool inCarreOutputMode )
{
  SENDER_NAME( "Element::OrderStructureElems" );

  IVarItemPtr pErrBuf = null;
  if( _ppErrObj == NULL )
    _ppErrObj = &pErrBuf;
  *_ppErrObj = NULL;

  /* No elements means no structure */
  if( _pElems->empty() )
    return ERR::NOELEMS;

  /* Create temporary groups */
  IComponentList elemsTmp;
  FOREACHVARITEMCONST( ElementPtr, pElem_i, *_pElems )
    elemsTmp.push_back( pElem_i );

  /* Save model targets. Targets are used only in Carre mode */
  IVarItemList elemsTargets = *_pTargets;

  /* Create result groups */
  _pTargets->clear();
  _pClosed->clear();
  _pBroken->clear();

/*  v=GetVarPtrByType(a,VT_TARGET1);
  if (v==NULL || v->val==NULL || IsEmptyGroup(v->val)) t1=NULL;
  else t1=v->val;

  v=GetVarPtrByType(a,VT_TARGET2);
  if (v==NULL || v->val==NULL || IsEmptyGroup(v->val)) t2=NULL;
  else t2=v->val; */

/* Order all structure rings starting from the outermost one */
  while( !elemsTmp.empty() ) {

    /* Find the element with the outermost origin */
    /* It always belongs to the outermost ring */
    double yMax = 0;
    ElementPtr pElemSelected = null;
    FOREACHPTRCONST( ElementPtr, pElem, elemsTmp ) {
      if( pElemSelected == null || pElem->Node(1)->Y() > yMax ) {
        yMax = pElem->Node(1)->Y();
        pElemSelected = pElem;
      }
    }
    assert( pElemSelected != NULL );

    /* Trace the structure ring connected to the found element */
    NodePtr pBreakNode = null;
    bool bClosed = true;
    ElementPtr pElemCurrent = pElemSelected;
    int count = 1;
    for( count = 1; ; count++ ) {
      if( pElemCurrent->Node(2)->IsEndNode( &elemsTmp ) ) {
        pBreakNode = pElemCurrent->Node(2);
        if( inCarreOutputMode ) {
          bClosed = false;
          break;
        }

        *_ppErrObj = pElemCurrent/*->Node(2)*/;
        FreeContainerList( _pTargets );
        FreeContainerList( _pClosed );
        FreeContainerList( _pBroken );
        return ERR::BROKEN;
      }

      if( !pElemCurrent->Node(2)->IsRegular( &elemsTmp ) ) {
        *_ppErrObj = pElemCurrent/*->Node(2)*/;
        FreeContainerList( _pTargets );
        FreeContainerList( _pClosed );
        FreeContainerList( _pBroken );
        return ERR::IRREGULAR;
      }

      pElemCurrent = pElemCurrent->GetNextElem( &elemsTmp, 2 );
      if( pElemCurrent == pElemSelected )
        break;
    }

    /* Broken chain in Carre mode: find the beginning */
    if( !bClosed ) {
      assert( inCarreOutputMode );
      for( count = 1; pElemCurrent->Node(1)->IsRegular( &elemsTmp ); count++ )
        pElemCurrent = pElemCurrent->GetNextElem( &elemsTmp, 1 );

      if( !pElemCurrent->Node(1)->IsEndNode( &elemsTmp ) ) {
        *_ppErrObj = pElemCurrent/*->Node(1)*/;
        FreeContainerList( _pTargets );
        FreeContainerList( _pClosed );
        FreeContainerList( _pBroken );
        return ERR::IRREGULAR;
      }
    }

    /* Copy the ring into g1 and remove all its elements from g */
    bool bTarget = false;
    IVarItemListPtr pElemTargetsTmp = new IVarItemList();
    for( ; count != 0; count-- ) {
/*      if (t1!=NULL && InGroup(t1,e)) bT1=bTarget=1;
      if (t2!=NULL && InGroup(t2,e)) bT2=bTarget=1; */
      pElemTargetsTmp->push_back( pElemCurrent );
      pElemSelected = pElemCurrent;
      pElemCurrent = pElemCurrent->GetNextElem( &elemsTmp, 2 );
      Remove( elemsTmp, (IComponentPtr)pElemSelected );
    }

    /* Additional tests */
    FOREACH_CONST( IVarItemIterConst, it1, elemsTargets ) {
      bool bT = false;
      VarItemListContainerPtr pElemsTargetsSublist = dgtype_cast< VarItemListContainerPtr >( *it1 );
      FOREACH_CONST( IVarItemIterConst, it2, (*pElemTargetsTmp) )
        if( Contains( *pElemsTargetsSublist->pList, *it2 ) )
          bT = bTarget = true;

      if( inCarreOutputMode ) {
        if( bT && !ContainsAll( *pElemsTargetsSublist->pList, *pElemTargetsTmp ) ) {
          /* *ppErrObj = pElemSelected; */
          FreeContainerList( _pTargets );
          FreeContainerList( _pClosed );
          FreeContainerList( _pBroken );

          return ERR::CARRE_TARGETXSTRUCTURE;
        }
      }
    }

    if( inCarreOutputMode ) {
      if( bTarget && !bClosed ) {
        *_ppErrObj = dgtype_cast< ElementPtr >( pBreakNode->ConnectedElements().front() ); /* Node is not IVarItem */
        FreeContainerList( _pTargets );
        FreeContainerList( _pClosed );
        FreeContainerList( _pBroken );

        return ERR::CARRE_BROKENTARGET;
      }
    }

/*      if (bT1 && !GroupInGroup(t1,g1)) { */
        /* *errObj=e0; */ /*FreeGroup(g);FreeGroup(g1);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR::CARRE_TARGETXSTRUCTURE;
      }

      if (bT2 && !GroupInGroup(t2,g1)) {  */
        /* *errObj=e0 */ /*;FreeGroup(g);FreeGroup(g1);
        FreeStructureInfo(pggTargets,pggClosed,pggBroken);
        return ERR::CARRE_TARGETXSTRUCTURE;
      } */

    /* Add g1 to one of the result groups */

    if( bTarget )
      _pTargets->push_back( new VarItemListContainer( pElemTargetsTmp ) );
    else if( bClosed )
      _pClosed->push_back( new VarItemListContainer( pElemTargetsTmp ) );
    else 
      _pBroken->push_back( new VarItemListContainer( pElemTargetsTmp ) );
  }

  /* Free temporary groups */

  return 0;
}


void Element::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  nodes[1]->CalcExtens( _pMin, _pMax );
  nodes[2]->CalcExtens( _pMin, _pMax );
}


void Element::Change( int ix, NodePtr n )
{
  SENDER_NAME( "Element::Change" );
  ValidatePtr(n,"ChangeElem___");
  assert( ix == 1 || ix == 2 );
  if( nodes[3 - ix] == n )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::SAME_NODES );

  assert( !nodes[3-ix]->IsConnectedWith( n ) );

  NodePtr n1 = nodes[ix];

  ActChangeElem( pModel, this, (ix == 1) ? n : nodes[1], (ix != 1) ? n : nodes[2], id, DO_AT_ONCE );

  if( n1->IsEmpty() )
    n1->Delete();
}

void Element::Revert()
{
  ActChangeElem( pModel, this, nodes[2], nodes[1], id, DO_AT_ONCE );
}

void Element::ChangeId( int _id )
{
  if( _id > pModel->MaxElemId() )
    pModel->SetMaxElemId( _id );

  ActChangeElem( pModel, this, nodes[1], nodes[2], _id, DO_AT_ONCE );
}


/* Return group containing chain of elements connected by regular nodes
   g!=NULL: only members of g are considered */

IComponentListPtr Element::GetChain( IComponentListPtr pElements, IComponentListPtr pElemsCheckRegular, IComponentListPtr pResult )
{
  const unsigned CHAIN_LIMIT = 10000; /* Prevent bugs from locking up the program */

  assert( pResult != null );

  ElementPtr pElemCurrent = this;
  while( pElemCurrent->Node(1)->IsRegular( pElements ) &&
    ( pElemsCheckRegular == pElements || pElemCurrent->Node(1)->IsRegular( pElemsCheckRegular ) ) ) {
    pElemCurrent = pElemCurrent->GetNextElem( pElements, 1 );
    if( pElemCurrent == this )
      break;
  }

  pResult->clear();
  ElementPtr pElemFirst = pElemCurrent;

  do {
    pResult->push_back( pElemCurrent );
    if( !pElemCurrent->Node(2)->IsRegular( pElements ) ||
      ( pElemsCheckRegular != pElements && !pElemCurrent->Node(2)->IsRegular( pElemsCheckRegular ) ) )
      break;

    pElemCurrent = pElemCurrent->GetNextElem( pElements, 2 );
    if( pResult->size() > CHAIN_LIMIT ) {
      fprintf( stderr, "Warning - GetElemChain() - Limit crossed.\n" );
      return pResult;
    }
  } while( pElemCurrent != pElemFirst );

  return pResult;
}

PointArray Element::GetLine() const
{
  PointArray array;
  array.push_back( nodes[1]->Position() );
  array.push_back( nodes[2]->Position() );
  return array;
}


int Element::IsConnectedWith( ElementPtr _pElem ) const
{
  if( nodes[1] == _pElem->Node(2) ||
      nodes[2] == _pElem->Node(1) ) return 1;
  if( nodes[1] == _pElem->Node(1) ||
      nodes[2] == _pElem->Node(2) ) return 2;
  return 0;
}

bool Element::IsInTarget() const
{
  FOREACH_CONST( VarIterConst, itV, containers )
    if( HasAnyFlag( (*itV)->VD()->VarDefType(), VTF::TARGET ) )
      return true;
  return false;
}

int Element::Split( int _count )
{
  if( IsLocked() )
    return ERR::LOCKED;

  assert( _count > 0 );
  bool isMarked = IsMarked();

  NodePtr pN0 = nodes[1];
  for( int i = 1; i < _count; i++ ) {
    NodePtr pN = pModel->AddNode( nodes[1]->Position() +
              (nodes[2]->Position() - nodes[1]->Position()) * (double)i/_count );
    assert( pN != null );

    ElementPtr pE = pModel->AddElem( pN0, pN );
    pE->Mark( isMarked );
    pN0 = pN;
  }
  ElementPtr pE = pModel->AddElem( pN0, nodes[2] );
  pE->Mark( isMarked );

  Delete();

  return 0;
}

void Element::SetParams( NodePtr _n1, NodePtr _n2, int _id )
{
  ReleaseObject( (IComponentPtr*)&nodes[1] );
  nodes[1] = DG_SHARE( _n1 );
  ReleaseObject( (IComponentPtr*)&nodes[2] );
  nodes[2] = DG_SHARE( _n2 );
  id = _id;
}

const IComponentList& Element::Node_Elements( int _node ) const
{
  return nodes[_node]->ConnectedElements();
}
