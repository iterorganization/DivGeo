#include "MeshPoint.h"
#include "../core/Model.h"

MeshPoint::MeshPoint( ModelPtr _pModel, MeshPtr m, Point _position ):
  IComponent(OT::MESHPOINT, _pModel ),
  pMesh( DG_SHARE( m ) ),
  position( _position ),
  backup( _position ),
  cutFlag( 0 ),
  index( -1 )
{
  DG_PROPERTIES_INIT;

  for( int i = MCN_START; i < MCN_END; i++ )
  {
    neighbours[i] = NULL;
    elements[i] = NULL;
    cells[i] = NULL;
  }
}

MeshPoint::~MeshPoint()
{
  ReleaseObject( (IComponentPtr*)&pMesh );

  for( int i = MCN_START; i < MCN_END; i++ )
  {
    if( neighbours[i] != null ) {
      neighbours[i]->ExcludePoint( this );
      ReleaseObject( (IComponentPtr*)&neighbours[i] );
    }
    if( elements[i] != null ) {
      elements[i]->ExcludePoint( this );
      ReleaseObject( (IComponentPtr*)&elements[i] );
    }
    if( cells[i] != null ) {
      cells[i]->ExcludePoint( this );
      ReleaseObject( (IComponentPtr*)&cells[i] );
    }
  }
  cells_ic.clear();
  elements_ic.clear();
  neighbours_ic.clear();
}

std::string MeshPoint::Description() const
{ return pModel->GetStr( STR::MESH_POINT ); }

std::string MeshPoint::ShortInfo() const
{ return index.ToString() + " " + position.ToString(); }

std::string MeshPoint::DetailedInfo() const
{ return Description() + " " + ShortInfo(); }

int MeshPoint::Change( Point _position )
{
  if( position == _position )
    return 0;

   Actions::ActChangeMeshPoint act( pModel, this, _position );
  int r = act.Do();

  if( r == 0 ) {
    for( int i = 0; i < 4; i++ ) {
      if( cells[i] != NULL)
        cells[i]->RecalculateCenter();
    }
  }

  if( r == 0 )
    pMesh->SetAlt( pMesh->Alt() + 1 );

  return r;
}

void MeshPoint::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  if( _pMin->x > _pMax->x )
    *_pMax = *_pMin = position;

  *_pMin = Point( dg_min( _pMin->x, position.x ), dg_min( _pMin->y, position.y ) );
  *_pMax = Point( dg_max( _pMax->x, position.x ), dg_max( _pMax->y, position.y ) );
}

void MeshPoint::HighlightDrag( bool include )
{
  Highlight( include );

  for( int i = 0; i < 4; i++ )
    if( cells[i] != null )
      cells[i]->Highlight( include );

  for( int i = MCN_START; i < MCN_END; i++ )
    if( elements[i] != null )
      elements[i]->Highlight( include );
}

std::list< IComponent* > MeshPoint::HighlightDragV( bool include )
{
  MeshPointPtr pMP1 = null;

  if( pModel->HasDoubleMeshBorder() ) {
    if( index.y == 0 )
      pMP1 = neighbours[MCN_BOTTOM];
    else if( index.y == pMesh->Size().y )
      pMP1 = neighbours[MCN_TOP];
  }
  
  IComponentList processed_points;

  HighlightDrag( include );
  processed_points.push_back( this );

  if( pMP1 != NULL) {
    pMP1->HighlightDrag( include );
    processed_points.push_back( pMP1 );
  }
  return processed_points;
}

IComponentList MeshPoint::HighlightDragH( bool include )
{
  MeshPointPtr pMP1 = null;

  if( pModel->HasDoubleMeshBorder() ) {
    if( index.x == 0 )
      pMP1 = neighbours[MCN_RIGHT];
    else if( index.x == pMesh->Size().x )
      pMP1 = neighbours[MCN_LEFT];
  }
  
  IComponentList processed_points = HighlightDragV( include );

  if( pMP1 != NULL) {
    IComponentList tmp = pMP1->HighlightDragV( include );
    processed_points.insert( processed_points.end(), tmp.begin(), tmp.end() );
  }

  return processed_points;
}


MeshPointPtr MeshPoint::OutermostPoint()
{
  MeshPointPtr pMP = this;
  if( index.x == 1 )
    pMP = neighbours[MCN_LEFT];
  else if( index.x == pMesh->Size().x - 1 )
    pMP = neighbours[MCN_RIGHT];
  if( pMP == null )
    return null;

  if( pMP->Index().y == 1 )
    pMP = pMP->Neighbour( MCN_TOP );
  else if( pMP->Index().y == pMesh->Size().y - 1 )
    pMP = pMP->Neighbour( MCN_BOTTOM );
  if( pMP == null )
    return null;

  return pMP;
}


MeshPointList MeshPoint::GetChain( bool stopOnCuts )
{
  MeshPointList chain;
  MeshPointPtr pMP1 = neighbours[MCN_LEFT];
  while( pMP1 != null ) {
    chain.push_back( pMP1 );
    if( pMP1 == this )
      return chain;
    if( pMP1->CutFlag() && stopOnCuts )
      break;
    pMP1 = pMP1->Neighbour( MCN_LEFT );
  }

  std::reverse( chain.begin(), chain.end() );
  chain.push_back( this );

  pMP1 = neighbours[MCN_RIGHT];
  while( pMP1 != null ) {
    if( pMP1 == this )
      break;
    chain.push_back( pMP1 );
    if( pMP1->CutFlag() && stopOnCuts )
      break;
    pMP1 = pMP1->Neighbour( MCN_RIGHT );
  }

  return chain;
}

int MeshPoint::SetBackupCoords( Point coords )
{
  assert( locks == 0 );

  if( coords != backup )
    ActChangeValue act1( pModel, this, this, p_backup_coords, coords, NO_REDRAW, DO_AT_ONCE );

  return 0;
}

const IComponentList& MeshPoint::ConnectedElements() const
{
  elements_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( elements[c] != null )
      elements_ic.push_back( elements[c] );
  return elements_ic;
}

const IComponentList& MeshPoint::ConnectedCells() const
{
  cells_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( cells[c] != null )
      cells_ic.push_back( cells[c] );
  return cells_ic;
}

const IComponentList& MeshPoint::NeighbourPoints() const
{
  neighbours_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( neighbours[c] != null )
      neighbours_ic.push_back( neighbours[c] );
  return neighbours_ic;
}


double MeshPoint::DistanceTo( MeshPointPtr pMP )
{
  return hypot( position.x - pMP->Position().x,
                position.y - pMP->Position().y );
}



void MeshPoint::MoveV( const Point& _crPos )
{
  MeshPointPtr pMP1 = null, pMP2 = null;

  if( pModel->HasDoubleMeshBorder() ) {
    if( index.y == 0 ) {
      pMP1 = neighbours[MCN_BOTTOM];
      if( pMP1 != null )
        pMP2 = pMP1->Neighbour(MCN_BOTTOM);
    }
    else if( index.y == pMesh->Size().y ) {
      pMP1 = neighbours[MCN_TOP];
      if( pMP1 != null )
        pMP2 = pMP1->Neighbour(MCN_TOP);
    }
  }

  double dist1 = 0.;
  if( pMP1 != null )
    dist1 = DistanceTo( pMP1 );

  Change( _crPos );

  if( pMP1 != null && pMP2 != null ) {
    double dist2 = 0.;
    dist2 = DistanceTo( pMP2 );
    Point pos1 = position + (pMP2->Position() - position) / dist2 * dist1;
    pMP1->Change( pos1 );
  }
}


void MeshPoint::MoveH( const Point& _crPos )
{
  MeshPointPtr pMP1 = null, pMP2 = null;

  if( pModel->HasDoubleMeshBorder() ) {
    if( index.x == 0 ) {
      pMP1 = neighbours[MCN_RIGHT];
      if( pMP1 != null )
        pMP2 = pMP1->Neighbour(MCN_RIGHT);
    }
    else if( index.x == pMesh->Size().x ) {
      pMP1 = neighbours[MCN_LEFT];
      if( pMP1 != null )
        pMP2 = pMP1->Neighbour(MCN_LEFT);
    }
  }

  double dist1 = 0.;
  if( pMP1 != null )
    dist1 = DistanceTo( pMP1 );

  MoveV( _crPos );

  if( pMP1 != null && pMP2 != null ) {
    double dist2 = 0.;
    dist2 = DistanceTo( pMP2 );
    Point pos1 = position + (pMP2->Position() - position) / dist2 * dist1;
    pMP1->MoveV( pos1 );
  }
}

bool MeshPoint::IsOnBorder() const
{
  return index.x == 1 || index.x == pMesh->Size().x - 1;
}

void MeshPoint::SetCell( MeshCellPtr mc, int i )
{
  if( i < 0 || i > 3 )
    return;
  ReleaseObject( (IComponentPtr*)&cells[i] );
  cells[i] = DG_SHARE( mc );
}

void MeshPoint::SetElement( MeshElementPtr me, int i )
{
  if( i < 0 || i > 3 )
    return;
  ReleaseObject( (IComponentPtr*)&elements[i] );
  elements[i] = DG_SHARE( me );
}

void MeshPoint::SetNeighbor( MeshPointPtr mp, int i)
{
  if( i < 0 || i > 3 )
    return;
  ReleaseObject( (IComponentPtr*)&neighbours[i] );
  neighbours[i] = DG_SHARE( mp );
}
