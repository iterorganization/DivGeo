#ifndef COMMENT_H
#define COMMENT_H

/* MB: 'Comment' class added in dg 3.0.1315.0
 *    It is user comment that can be written to *.dg file.
 *    Comments are wide character string text placed
 * at 'position' and linked with object of 'objType'
 *    Due to 'HitObject' was moved to view
 * linked object must be found by associated 'ViewItem'
 * using 'pos_end' coordinates as nearest point
 */

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

class Comment:
    public IComponent,
    public IValueHolder
{
  DG_OBJECT(Comment)

  DG_PROPERTIES
  {
    p_position = 0,
    p_pos_end  = 1,
    p_text     = 2,
    p_obj_type = 3
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "position",  &position );
    AddProperty( "pos_end",   &pos_end );
    AddProperty( "text",      &wsText );
    AddProperty( "obj_type",  (int*)&objType );
  }

  Point position, pos_end;
  std::wstring wsText;
  ObjectType objType;
  int parentScene;

public:
  /* Constructors */
  Comment( ModelPtr _pModel, ObjectType _ot, const Point& _crPosEnd,
           const Point& _crPos, const std::wstring& _crwsText, int _parentScene );
  ~Comment();

  /* IComponent implementation */
  virtual void    Delete();
  virtual double  DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual bool    InRectangle( const Point& _p1, const Point& _p2 ) const;
  virtual void    CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual Point   CentralPoint() const;
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */
  void ChangePosition( const Point& _crPos );
  void ChangePosEnd( const Point& _crPosEnd );
  void ChangeText( const std::wstring& _crwsText );
  void ChangeObjectType( ObjectType _ot );

  /* Getters */
  const Point& Position() const { return position; }
  const Point& PosEnd() const { return pos_end; }
  const std::wstring& Text() const { return wsText; }
  ObjectType ObjType() const { return objType; }
  int ParentScene() const { return parentScene; }
};

#endif // COMMENT_H
