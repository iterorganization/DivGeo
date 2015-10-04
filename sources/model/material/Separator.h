#ifndef SEPARATOR_H
#define SEPARATOR_H

#include "../core/IComponent.h"
#include "../core/IVarOrigin.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "../mesh/SonnetDecl.h"
#include "../material/MaterialDecl.h"

namespace Actions { class ActChangeSeparator; }

class Separator:
  public IComponent,
  public IVarOrigin
{
  DG_OBJECT(Separator)

  Point position;
  NodePtr pNode;
  int id;

public:
  /* Constructors */
  Separator( ModelPtr _pModel, Point _position, NodePtr _pN );
  ~Separator();

  /* IComponent implementation */
  virtual void Delete();
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point CentralPoint() const;
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  void Change( NodePtr _pNode );
  void ChangeId( int _id );

  void IncludeToNode();
  void ExcludeFromNode();

  /* Getters */
  int Id()              const { return id; }
  Point Position()      const { return position; }
  double X()            const { return position.x; }
  double Y()            const { return position.y; }
  NodePtr Node()        const { return pNode; }
  bool HasNode( NodePtr _pNode ) const { return pNode == _pNode; }

  /* Setters */
  void ExcludeNode( NodePtr _pNode );
  friend class Actions::ActChangeSeparator;
private:
  void SetProperties( const Point& _crPosition, NodePtr _pNode, int _id )
  { position = _crPosition; pNode = _pNode; id = _id; }
};

#endif
