#include "Separator.h"
#include "../core/Model.h"

Separator::Separator( ModelPtr _pModel, Point _position, NodePtr _pN ): 
  IComponent( OT::SEPARATOR, _pModel )
{
  position = _position;
  pNode = DG_SHARE( _pN );
  id = pModel->GetNextElemId();
}

Separator::~Separator()
{
  if( pNode != null ) {
    pNode->ExcludeSeparator( this );
    ReleaseObject( (IComponentPtr*)&pNode );
  }
  FreeVars();
}

std::string Separator::Description() const
{ return pModel->GetStr( STR::SEPARATOR ); }

std::string Separator::ShortInfo() const
{ return std::string( "[" ) + id + "]"; }

std::string Separator::DetailedInfo() const
{ return Description() + ShortInfo() + " " +
      position.ToString() + " - " + pNode->Position().ToString(); }

double Separator::DistToPoint(const Point& _crPnt, int* _pPosFlag ) const
{ return Point2VectorDist( pNode->Position(), position, _crPnt, _pPosFlag, null ); }

bool Separator::InRectangle( const Point& _p1, const Point& _p2 ) const
{ return VectorInRectangle( position, pNode->Position(), _p1, _p2 ); }

Point Separator::CentralPoint() const
{ return (pNode->Position() + position) / 2.; }

void Separator::IncludeToNode()
{ pNode->IncludeSeparator( this ); }

void Separator::ExcludeFromNode() 
{ pNode->ExcludeSeparator( this ); }

void Separator::Delete()
{
  SENDER_NAME( "Separator::Delete" );
  if( pModel->HasHighlighted( this ) )
    pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_HIGHLIGHTED );

  Mark( false );

  DeleteVars();

  NodePtr pN = pNode;

  ActDelSeparator( pModel, this, DO_AT_ONCE );

  if( pN->IsEmpty() )
    pN->Delete();
}


void Separator::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  pNode->CalcExtens( _pMin, _pMax );
  *_pMin = Point( dg_min( _pMin->x, position.x ), dg_min( _pMin->y, position.y ) );
  *_pMax = Point( dg_max( _pMax->x, position.x ), dg_max( _pMax->y, position.y ) );
}


void Separator::Change( NodePtr _pNode )
{
  ValidatePtr( _pNode, "ChangeSeparator__");

  NodePtr pN = pNode;

  ActChangeSeparator( pModel, this, position, _pNode, id, DO_AT_ONCE );

  if( pN->IsEmpty() )
    pN->Delete();
}

void Separator::ChangeId( int _id )
{
  if( _id > pModel->MaxElemId() )
    pModel->SetMaxElemId( id );

  ActChangeSeparator( pModel, this, position, pNode, _id, DO_AT_ONCE );
}

void Separator::ExcludeNode( NodePtr _pNode )
{
  if( pNode == _pNode )
    ReleaseObject( &pNode );
}
