#include "MeshElement.h"
#include "../core/Model.h"

MeshElement::MeshElement( ModelPtr _pModel, MeshPtr m, MeshPointPtr mp1, MeshPointPtr mp2, bool isVertical ):
  IComponent( OT::MESHELEMENT, _pModel ),
  mesh( DG_SHARE( m ) ),
  cutFlag( 0 ),
  bVertical( isVertical )
{ 
  points[0] = DG_SHARE( mp1 );
  points[1] = DG_SHARE( mp2 );

  cells[0] = NULL;
  cells[1] = NULL;
}

MeshElement::~MeshElement()
{
  FreeContainers();

  for( int i = 0; i < 2; i++ ) {
    if( points[i] != null ) {
      points[i]->ExcludeElement( this );
      ReleaseObject( (IComponentPtr*)&points[i] );
    }
    if( cells[i] != null ) {
      cells[i]->ExcludeElement( this );
      ReleaseObject( (IComponentPtr*)&cells[i] );
    }
  }

  ReleaseObject( (IComponentPtr*)&mesh );
}

std::string MeshElement::Description() const
{ return pModel->GetStr( STR::MESH_ELEMENT ); }

std::string MeshElement::ShortInfo() const
{ return ToStdString( GetID_1() ) + ":" + GetID_2(); }

std::string MeshElement::DetailedInfo() const
{
  std::stringstream ss;
  NPoint index[2] = { cells[0] == null ? NPoint( MAXINT ) : cells[0]->Index(),
                      cells[1] == null ? NPoint( MAXINT ) : cells[1]->Index() };
  ss << Description() << " " << (bVertical ? "V" : "H");
  if( cells[0] == null ) ss << "[?] | ";
  else                   ss << " " << index[0].ToString() << " | ";
  if( cells[1] == null ) ss << "[?]";
  else                   ss << index[1].ToString();
  return ss.str();
}

double MeshElement::DistToPoint( const Point& _crPnt, int* _pPosFlag ) const
{
  return Point2VectorDist( points[0]->Position(), points[1]->Position(), _crPnt, _pPosFlag, null );
}

bool MeshElement::InRectangle( const Point& _p1, const Point& _p2 ) const
{
  return VectorInRectangle( points[0]->Position(), points[1]->Position(), _p1, _p2 );
}

void MeshElement::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  for( int i = 0; i < 2; i++ )
    points[i]->CalcExtens( _pMin, _pMax );
}

Point MeshElement::CentralPoint() const
{
  return (points[0]->Position() + points[1]->Position()) / 2.;
}

int MeshElement::GetID_1() const
{
  assert( cells[0] != NULL );
  return cells[0]->eNumber();
}

int MeshElement::GetID_2() const
{
  assert( cells[0] != NULL );
  int i;
  for( i = MCN_START; i < MCN_END; i++ )
    if( cells[0]->Element(i) == this )
      break;

  assert( i < MCN_END );
  return i;
}

const IComponentList& MeshElement::ConnectedPoints() const
{
  points_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( points[c] != null )
      points_ic.push_back( points[c] );
  return points_ic;
}

const IComponentList& MeshElement::ConnectedCells() const
{
  cells_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( cells[c] != null )
      cells_ic.push_back( cells[c] );
  return cells_ic;
}

void MeshElement::SetCell( MeshCellPtr mc, int i )
{
  if( i < 0 || i > 1 )
    return;
  ReleaseObject( (IComponentPtr*)&cells[i] );
  cells[i] = DG_SHARE( mc );
}

void MeshElement::ExcludeCell( MeshCellPtr _pMC )
{
  for( uint c = 0; c < 2; c++ )
    if( cells[c] != null && cells[c] == _pMC )
      ReleaseObject( (IComponentPtr*)&cells[c] );
}

void MeshElement::ExcludePoint( MeshPointPtr _pMP )
{
  for( uint c = 0; c < 2; c++ )
    if( points[c] != null && points[c] == _pMP ) {
      /*if( points[c]->CutFlag() != 0 ) { // See MeshCell::ExcludePoint()
        int another = c == 0 ? 1 : 0;
        if( points[another] != null )
          points[another]->ExcludePoint( _pMP );
      }*/
      ReleaseObject( (IComponentPtr*)&points[c] );
    }
}
