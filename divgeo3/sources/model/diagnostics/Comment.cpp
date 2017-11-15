#include "Comment.h"
#include "../core/Model.h"

/* Constructors */
Comment::Comment( ModelPtr _pModel, ObjectType _ot, const Point& _crPosEnd,
         const Point& _crPos, const std::wstring& _crwsText, int _parentScene ):
  IComponent( OT::COMMENT, _pModel ),
  position( _crPos ),
  pos_end( _crPosEnd ),
  wsText( _crwsText ),
  objType( _ot ),
  parentScene( _parentScene )
{ DG_PROPERTIES_INIT; }

Comment::~Comment() {}

/* IComponent implementation */
void Comment::Delete()
{
  ActDelComment( pModel, this, DO_AT_ONCE );
}

double Comment::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{
  UNUSED( _pPosFlag );
  return hypot( _crPnt.x - position.x, _crPnt.y - position.y );
}

bool Comment::InRectangle( const Point& _p1, const Point& _p2 ) const
{
  return PointInRectangle( position, _p1, _p2 );
}

void Comment::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin != null )
    *_pMin = position;
  if( _pMax != null )
    *_pMax = position;
}

Point Comment::CentralPoint() const { return position; }

std::string Comment::Description() const
{
  return pModel->GetStr( DG3::COMMENT );
}

std::string Comment::DetailedInfo() const
{
  return Description();
}

std::string Comment::ShortInfo() const
{
  return "";
}

/* Actions */
void Comment::ChangePosition( const Point& _crPos )
{
  ActChangeValue( pModel, this, this, p_position, _crPos, NO_REDRAW, DO_AT_ONCE );
}

void Comment::ChangePosEnd( const Point& _crPosEnd )
{
  ActChangeValue( pModel, this, this, p_pos_end, _crPosEnd, NO_REDRAW, DO_AT_ONCE );
}

void Comment::ChangeText( const std::wstring& _crwsText )
{
  ActChangeValue( pModel, this, this, p_text, _crwsText, NO_REDRAW, DO_AT_ONCE );
}

void Comment::ChangeObjectType( ObjectType _ot )
{
  ActChangeValue( pModel, this, this, p_obj_type, _ot, NO_REDRAW, DO_AT_ONCE );
}
