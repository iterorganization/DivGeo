#ifndef NODE_H
#define NODE_H

#include "../core/IComponent.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "MaterialDecl.h"

struct NearestNode {
  NodePtr pN1, pN2;
  double dist;
  NearestNode( NodePtr _pN1 = null, NodePtr _pN2 = null, double _dist = MAXDOUBLE ):
    pN1( _pN1 ), pN2( _pN2 ), dist( _dist ) { }

  bool operator == ( const NearestNode& nn ) {
    return (nn.pN1 == pN1 && nn.pN2 == pN2 && nn.dist == dist ); }
};
typedef std::list< NearestNode > NearestNodesList;
typedef NearestNodesList::iterator NearestNodeIter;
bool NearestNodes_Compare( const NearestNode& n1, const NearestNode& n2 );

class Node:
  public IComponent    
{
  DG_OBJECT(Node)

  Point pnt;
  ElementList elements;
  SeparatorList separators;

  mutable IComponentList elements_ic;
  mutable IComponentList separators_ic;

public:
  /* Constructors */
  Node( ModelPtr _pModel, Point _pnt ) : IComponent( OT::NODE, _pModel ), pnt( _pnt ) {}
  ~Node();

  /* IComponent implementation */
  virtual void Delete();
  virtual double DistToPoint( Point _pnt, int* pPosFlag ) { UNUSED(pPosFlag); return hypot( _pnt.x - pnt.x, _pnt.y - pnt.y ); }
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const { return pnt; }

  /* IObject implementation */
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  void Change( Point xy );

  static NearestNode FindNearestNode( NodePtr pN,
                               const IComponentList& nodes, double maxDist );

  void HighlightDrag( bool include = true );

  int IsIrregular( IComponentListPtr pObjects = null ) const;
  bool IsRegular( IComponentListPtr pObjects, int* pReason = null ) const {
    int reason = IsIrregular( (pObjects) );
    if( pReason != null )
      *pReason = reason;
    return reason == 0;
  }
  bool IsEndNode( IComponentListPtr pObjects )  const;
  bool IsEmpty() const { return elements.empty() && separators.empty(); }
  bool IsConnectedWith( NodePtr _pN ) const;
  bool IsInTarget() const;

  int CheckJoinPossibility() const;
  ElementPtr JoinElements();

  /* Getters */
  bool HasElements()                    const { return !elements.empty(); }
  bool HasSeparators()                  const { return !separators.empty(); }
  bool HasElement( ElementPtr _pE )     const { return Contains( elements, _pE ); }
  bool HasSeparator( SeparatorPtr _pS ) const { return Contains( separators, _pS ); }
  int SeparatorsCount()                 const { return separators.size(); }
  int ElementsCount()                   const { return elements.size(); }

  Point Position()  const { return pnt; }
  double X()        const { return pnt.x; }
  double Y()        const { return pnt.y; }
  double Z()        const { return pnt.z; }

  void IncludeElement( ElementPtr _pElem );
  void ExcludeElement( ElementPtr _pElem );
  void IncludeSeparator( SeparatorPtr _pSep );
  void ExcludeSeparator( SeparatorPtr _pSep );

  const IComponentList& ConnectedElements() const;
  const IComponentList& ConnectedSeparators() const;

  /* Setters */
  void SetXYZ( Point _pnt ) { pnt = _pnt; }
};

#endif
