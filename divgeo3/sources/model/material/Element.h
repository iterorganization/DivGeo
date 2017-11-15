#ifndef ELEMENT_H
#define ELEMENT_H

#include "../core/IComponent.h"
#include "../core/IVarItem.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "MaterialDecl.h"

class Element:
  public IComponent,
  public IVarItem,
  public IVarOrigin
{
  DG_OBJECT(Element)

  NodePtr nodes[3];
  int id;

public:
  /* Constructors */
  Element( ModelPtr _pModel, NodePtr _pN1, NodePtr _pN2 );
  ~Element();

  /* IComponent implementation */
  virtual void    Delete();
  virtual double  DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool    InRectangle(const Point& _p1, const Point& _p2 ) const;
  virtual void    CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point   CentralPoint() const;
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  void Change( int ix, NodePtr n );
  void Revert();
  void ChangeId( int _id );

  /* Return another element attached to a regular node.
   * If pElems != NULL: only members of pElems are considered */
  template< typename T >
  ElementPtr GetNextElem( std::list< T* >* _pElems, int _node ) const;
  IComponentListPtr GetChain( IComponentListPtr pElements,
           IComponentListPtr pElemsCheckRegular, IComponentListPtr pResult );
  static int OrderTargetElems(IVarItemListPtr _pElems, IVarItemPtr *_ppErrObj,
                              ElementPtr* _ppElem1 = null, ElementPtr* _ppElem2 = null );
  static int OrderStructureElems(IVarItemListPtr _pElems,
                        IVarItemListPtr _pTargets, IVarItemListPtr _pClosed,
    IVarItemListPtr _pBroken, IVarItemPtr *_ppErrObj, bool inCarreOutputMode);

  /* Split element to several new elements. Return last added node.
     Count: # of intermediate nodes to produce */
  int Split( int _count );

  /* Setters */
  void SetParams( NodePtr _n1, NodePtr _n2, int _id );
  void ExcludeNode( NodePtr _pN );

  /* Getters */
  int Id()              const { return id; }
  double Length()       const;
  NodePtr Node( int i ) const { return ( i < 0 || i > 2 ) ? NULL : nodes[i]; }
  PointArray GetLine()  const;

  /* Check for a node to which both elements are attached.
   * Return 1 if connected >>; 2 if connected <> or >< */
  int IsConnectedWith( ElementPtr _pElem ) const;

  bool IsInTarget() const;

private:
  const IComponentList& Node_Elements( int _node ) const;
};

#include "Element.t.hpp"

#endif
