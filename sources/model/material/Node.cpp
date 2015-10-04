#include "Node.h"
#include "../core/Model.h"

Node::~Node()
{
  FOREACH_CONST( ElementIterConst, itE, elements ) {
    ((ElementPtr)*itE)->ExcludeNode( this );
    ReleaseObject( (IComponentPtr)*itE );
  }
  elements.clear();
  elements_ic.clear();
  FOREACH_CONST( SeparatorIterConst, itS, separators ) {
    ((SeparatorPtr)*itS)->ExcludeNode( this );
    ReleaseObject( (IComponentPtr)*itS );
  }
  separators.clear();
  separators_ic.clear();
}

std::string Node::Description() const
{ return pModel->GetStr( STR::NODE ); }

std::string Node::ShortInfo() const
{ return pnt.ToString(); }

std::string Node::DetailedInfo() const
{ return Description() + " " + pnt.ToString() /*+ " "
     + elements.size() + "el, " + separators.size() + "sep"*/; }

bool NearestNodes_Compare(const NearestNode& n1, const NearestNode& n2 ) {
  return ( n1.dist < n2.dist );
}

void Node::Delete()
{
  SENDER_NAME( "Node::Delete" );
  if( pModel->HasHighlighted( this ) )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_HIGHLIGHTED );

  ActDelNode( pModel, this, DO_AT_ONCE );
}


/* Check for irregular node:
     Exactly 2 elements from the group or from the app must be attached
     The end of one of then must be the beginning of the other
     No separators must be attached
   pObjects!=NULL: only members of pObjects (elems & separators) are considered
   Return: 0 = regular node
           ERR::xxx: error code showing why it is irregular
*/

int Node::IsIrregular( IComponentListPtr pObjects ) const
{
  SENDER_NAME( "Node::IsIrregular" );
  ElementPtr pElemSelected = null;
  int i = 0, j = 0;
  FOREACH_CONST( ElementIterConst, itE, elements ) {
    if( pObjects != null && !Contains( *pObjects, (IComponentPtr)*itE ) )
      continue;

    switch( i++ ) {
    case 0: 
      pElemSelected = *itE;
      break;
    case 1:
      if( pElemSelected->Node(1) == (*itE)->Node(2) ) j = 1;
      if( pElemSelected->Node(2) == (*itE)->Node(1) ) j = 2;
      break;
    case 2:
      return STR::IRRTOOMANY;
    default:
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::WRONG_NODE_STATE );
      return -1;
    }
  }
  if( i < 2 )
    return STR::IRRTOOFEW;

  FOREACH_CONST( SeparatorIterConst, itS, separators ) {
    if( pObjects != null && std::find( pObjects->begin(), pObjects->end(), *itS ) != pObjects->end() )
       return STR::IRRSEPARATORS;
  }

  return j != 0 ? 0 : STR::IRRNORMALS;
}


/* Return 1 if there is exactly 1 element and no separators attached
   pObjects!=NULL: only members of pObjects are considered
*/

bool Node::IsEndNode( IComponentListPtr pObjects ) const
{
  if( pObjects == null )
    return (elements.size() == 1 && separators.empty());

  int i = 0;
  FOREACH_CONST( ElementIterConst, itE, elements )
    if( std::find( pObjects->begin(), pObjects->end(), *itE ) != pObjects->end() )
      i++;

  FOREACH_CONST( SeparatorIterConst, itS, separators )
    if( std::find( pObjects->begin(), pObjects->end(), *itS ) != pObjects->end() )
      return false;

  return (i == 1);
}

bool Node::IsConnectedWith( NodePtr _pN ) const
{
  FOREACH_CONST( ElementIterConst, itE, elements )
    if( _pN->HasElement( *itE ) )
      return true;
  return false;
}

bool Node::IsInTarget() const
{
  FOREACH_CONST( ElementIterConst, itE, elements )
    if( (*itE)->IsInTarget() )
      return true;
  return false;
}

void Node::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMin = *_pMax = pnt;
  *_pMin = Point( dg_min( _pMin->x, pnt.x ), dg_min( _pMin->y, pnt.y ) );
  *_pMax = Point( dg_max( _pMax->x, pnt.x ), dg_max( _pMax->y, pnt.y ) );
}


void Node::Change( Point xy )
{
  ActChangeNode( pModel, this, xy, DO_AT_ONCE );
}


void Node::HighlightDrag( bool include )
{
  Highlight( include );

  FOREACH_CONST( ElementIterConst, itElem, elements )
    pModel->Highlight( *itElem, include );

  FOREACH_CONST( SeparatorIterConst, itSep, separators )
    pModel->Highlight( *itSep, include );
}

const IComponentList& Node::ConnectedElements() const
{
  elements_ic.clear();
  FOREACH_CONST( ElementIterConst, itE, elements )
    elements_ic.push_back( *itE );
  return elements_ic;
}

const IComponentList& Node::ConnectedSeparators() const
{
  separators_ic.clear();
  FOREACH_CONST( SeparatorIterConst, itS, separators )
    separators_ic.push_back( *itS );
  return separators_ic;
}

int Node::CheckJoinPossibility() const
{
  int result = IsIrregular();
  if( result != 0 )
    return result;
  ElementPtr elem[] = { elements.front(), *(++elements.begin()) };

  int offset = (elem[0]->Node(1) == elem[1]->Node(2)) ? 0 : 1;
  if( elem[offset]->Node(2)->IsConnectedWith( elem[1-offset]->Node(1) ) )
    return ERR::JOINCONNECTED;

   if( elem[0]->IsLocked() || elem[1]->IsLocked() || this->IsLocked() )
     return ERR::LOCKED;

   return 0;
}


ElementPtr Node::JoinElements()
{
  int result = CheckJoinPossibility();
  if( result != 0 ) {
    pModel->SendMessage( WND_ERROR, "Node::JoinElements", result );
    return null;
  }

  ElementPtr elem[] = { elements.front(), *(++elements.begin()) };
  int offset = (elem[0]->Node(1) == elem[1]->Node(2)) ? 0 : 1;

  ElementPtr pNewElement = pModel->AddElem( elem[1-offset]->Node(1), elem[offset]->Node(2) );
  if( elem[0]->IsMarked() || elem[1]->IsMarked() )
    pNewElement->Mark();

  elem[0]->Delete();
  elem[1]->Delete();

  return pNewElement;
}


NearestNode Node::FindNearestNode( NodePtr pN,
                             const IComponentList& nodes, double maxDist )
{
  double dMin=MAXDOUBLE;
  NodePtr pNMin = NULL;

  FOREACHPTRCONST( NodePtr, pNode, nodes ) {
    if( pNode == pN || pNode->IsConnectedWith( pN ) )
      continue;
    assert( pNode->ElementsCount() == 1 );
    double dist = hypot( pNode->X() - pN->X(), pNode->Y() - pN->Y() );
    if( dist <= maxDist && dist < dMin) {
      dMin = dist;
      pNMin = pNode;
    }
  }

  if( pNMin == null )
    return NearestNode();

  return NearestNode( pN, pNMin, dMin );
}


void Node::IncludeElement( ElementPtr _pElem )
{
  elements.push_back( DG_SHARE( _pElem ) );
}

void Node::ExcludeElement( ElementPtr _pElem )
{
  if( Remove( elements, _pElem ) )
    ReleaseObject( _pElem );
}

void Node::IncludeSeparator( SeparatorPtr _pSep )
{
  separators.push_back( DG_SHARE( _pSep ) );
}

void Node::ExcludeSeparator( SeparatorPtr _pSep )
{
  if( Remove( separators, _pSep ) )
    ReleaseObject( _pSep );
}
