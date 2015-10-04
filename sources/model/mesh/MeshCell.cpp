#include "MeshCell.h"
#include "../core/Model.h"

MeshCell::MeshCell( ModelPtr _pModel, MeshPtr m, int _eN, NPoint _index,
                     MeshPointPtr mp1, MeshPointPtr mp2, MeshPointPtr mp3, MeshPointPtr mp4,
                     Point _center, double _ratio):
  IComponent(OT::MESHCELL, _pModel ),
  mesh( DG_SHARE( m ) ),
  index( _index ),
  eN( _eN ),
  breakFlags( 0 ),

  center( _center ),
  center_backup( _center ),
  ratio( _ratio ),  
  backupRatio( _ratio )
{ 
  DG_PROPERTIES_INIT;

  points[0] = DG_SHARE( mp1 );
  points[1] = DG_SHARE( mp2 );
  points[2] = DG_SHARE( mp3 );
  points[3] = DG_SHARE( mp4 );

  for( int i = MCN_START; i < MCN_END; i++ )
    elements[i] = NULL;
  for( int i = MCN_START; i < MCN_END; i++ )
    neighbours[i] = NULL;
  for( int i = MCN_START; i < MCN_END; i++ )
    cutFlags[i] = 0;
}

MeshCell::~MeshCell()
{
  FreeContainers();
  for( int i = MCN_START; i < MCN_END; i++ ) {
    if( elements[i] != null ) {
      elements[i]->ExcludeCell( this );
      ReleaseObject( (IComponentPtr*)&elements[i] );
    }
    if( neighbours[i] != null ) {
      neighbours[i]->ExcludeCell( this );
      ReleaseObject( (IComponentPtr*)&neighbours[i] );
    }
    if( points[i] != null ) {
      points[i]->ExcludeCell( this );
      ReleaseObject( (IComponentPtr*)&points[i] );
    }
  }

  neighbours_ic.clear();
  elements_ic.clear();
  points_ic.clear();

  ReleaseObject( (IComponentPtr*)&mesh );
}

std::string MeshCell::Description() const
{ return pModel->GetStr( STR::MESH_CELL ); }

std::string MeshCell::ShortInfo() const
{ return ToStdString( eN ) + " " + index.ToString(); }

std::string MeshCell::DetailedInfo() const
{ return Description() + " " + ShortInfo() + " " + center.ToString(); }

bool MeshCell::InRectangle( const Point& _p1, const Point& _p2 ) const
{
  return PointInRectangle( center, _p1, _p2 );
}

bool MeshCell::IsChanged() const
{
  for( int i = 0; i < 4; i++ )
    if( points[i]->IsChanged() )
      return true;

  return false;
}

void MeshCell::SetCenter( Point _center )
{
  if( center != _center )
    ActChangeValue( pModel, this, this, p_center, _center, REDRAW, DO_AT_ONCE );
}

int MeshCell::RecalculateCenter()
{
  Point pnt;
  double ra, r;
  int k;

  EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();

  if( pEquil == null )
    return ERR::MESH_NO_EQUIL;

  /* In CARRE mode or when the diagonals are parallel, use the mass center */

  if( pModel->CarreOutputMode() ||
      VIntersect( points[0]->Position(), points[3]->Position(), points[1]->Position(), points[2]->Position(), &ra, NULL) == -2 )
  {
    for( pnt = 0, k = 0; k < 4; k++ )
    {
      pnt.x += points[k]->X();
      pnt.y += points[k]->Y();
    }

    pnt.x /= 4;
    pnt.y /= 4;
  }
  else
  {
  /* Otherwise (SONNET), use the intersection of diagonals */

    pnt.x = points[0]->X() + (points[3]->X() - points[0]->X()) * ra;
    pnt.y = points[0]->Y() + (points[3]->Y() - points[0]->Y()) * ra;
  }

  SetCenter( pnt );

  /* Calculate the gradient at the "old" cell center */

  k = pEquil->CalcGradient( center_backup, &ra );
  if( k )
    return k;

  /* Calculate the gradient at the "new" cell center */

  k = pEquil->CalcGradient( center, &r );
  if( k )
    return k;

  /* Multiply the old field ratio by the ratio of gradients */

  if( ra != 0 )
    SetFieldRatio( r / ra * backupRatio );

  return 0;
}

void MeshCell::SetFieldRatio( double _ratio )
{
  if( ratio != _ratio)
    Actions::ActChangeValue act( pModel, this, this, p_field_ratio, _ratio, NO_REDRAW, DO_AT_ONCE );
}

void MeshCell::CalcExtens( Point* _pMin, Point* _pMax ) const
{
  for( int i = 0; i < 4; i++ )
    points[i]->CalcExtens( _pMin, _pMax );
}

int MeshCell::IsIrregular() const
{
  if( VIntersect( points[0]->Position(), points[1]->Position(),
                  points[2]->Position(), points[3]->Position(), null, null ) == 0 )
    return STR::MESH_CELL_TWISTED;

  if( VIntersect( points[0]->Position(), points[2]->Position(),
                  points[1]->Position(), points[3]->Position(), null, null ) == 0 )
    return STR::MESH_CELL_TWISTED;

  if( !PointInside2( center, false ) )
    return STR::MESH_CELL_BAD_CENTER;

  for( int k = 0; k < 4; k++ ) {
    /* find adjacent mesh points; must be cycled through in order (0,1,3,2) */
    int i = (k ^ ((k ^ k) >> 1)) % 2 + 1;
    int j = (i ^ ((i ^ i) >> 1)) % 2 + 1;
    Point ik = points[i]->Position() - points[k]->Position();
    Point ji = points[j]->Position() - points[i]->Position();
    if( ik.x * ji.y < ji.x * ik.y )
      return STR::MESH_CELL_CONCAVE;
  }

  return 0;
}


bool MeshCell::PointInside( Point point, bool bUseBackups ) const
{
  int i;

  if( bUseBackups ) {
    Point p;
    for( i = 0; i < 4; i++ )
      p += fabs( points[i]->Backup() ) + 1;

    if( VIntersect( point, p, points[0]->Backup(), points[1]->Backup(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[1]->Backup(), points[3]->Backup(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[3]->Backup(), points[2]->Backup(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[2]->Backup(), points[0]->Backup(), null, null ) == 0 ) i++;
  } 
  else {
    Point p;
    for( i = 0; i < 4; i++ )
      p += fabs( points[i]->Position() ) + 1;

    i = 0;
    if( VIntersect( point, p, points[0]->Position(), points[1]->Position(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[1]->Position(), points[3]->Position(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[3]->Position(), points[2]->Position(), null, null ) == 0 ) i++;
    if( VIntersect( point, p, points[2]->Position(), points[0]->Position(), null, null ) == 0 ) i++;
  }

  return i % 2 == 1;
}

bool MeshCell::PointInside2( Point point, bool bUseBackups ) const
{
  Point p[4];

  if( bUseBackups ) {
    p[0] = points[0]->Backup();
    p[1] = points[2]->Backup();
    p[2] = points[3]->Backup();
    p[3] = points[1]->Backup();
  }
  else {
    p[0] = points[0]->Position();
    p[1] = points[2]->Position();
    p[2] = points[3]->Position();
    p[3] = points[1]->Position();
  }

  // Sector p1-p0-p3 detection
  if( CrossProductZ( p[0], p[1], point ) > 0 ||
      CrossProductZ( p[0], p[3], point ) < 0 )
    return false; // outside

  if( CrossProductZ( p[0], p[2], point ) > 0 ) {
    // Sector p1-p2
    if( CrossProductZ( p[1], p[2], point ) > 0 )
      return false; // outside
  }
  else {
    // Sector p2-p3
    if( CrossProductZ( p[3], p[2], point ) < 0 )
      return false; // outside
  }

  return true; // inside
}

int MeshCell::SetBackups(Point _center, double _ratio )
{
  assert( locks == 0 );
  if( _center != center_backup )
    ActChangeValue act1( pModel,this, this, p_center_backup, _center, NO_REDRAW, DO_AT_ONCE );
  if( _ratio != backupRatio )
    ActChangeValue act2( pModel,this, this, p_backup_ratio, _ratio, NO_REDRAW, DO_AT_ONCE );
  return 0;
}

const IComponentList& MeshCell::ConnectedElements() const
{
  elements_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( elements[c] != null )
      elements_ic.push_back( elements[c] );
  return elements_ic;
}

const IComponentList& MeshCell::NeighbourCells() const
{
  neighbours_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( neighbours[c] != null )
      neighbours_ic.push_back( neighbours[c] );
  return neighbours_ic;
}

const IComponentList& MeshCell::ConnectedPoints() const
{
  points_ic.clear();
  for( ulong c = 0; c < neighbours_count; c++ )
    if( points[c] != null )
      points_ic.push_back( points[c] );
  return points_ic;
}

void MeshCell::SetCutFlag(int cf, int i)
{
  if( i < 0 || i > 3 )
    return;
  cutFlags[i] = cf;
}

void MeshCell::SetNeighbor(MeshCellPtr mc, int i)
{
  if( i < 0 || i > 3 )
    return;
  ReleaseObject( (IComponentPtr*)&neighbours[i] );
  neighbours[i] = DG_SHARE( mc );
}

void MeshCell::SetElement( MeshElementPtr me, int i )
{
  if( i < 0 || i > 3 )
    return;
  ReleaseObject( (IComponentPtr*)&elements[i] );
  elements[i] = DG_SHARE( me );
}

// Force exclude object without releasing
void MeshCell::ExcludeCell( MeshCellPtr _pMC )
{
  for( uint c = 0; c < 4; c++ )
    if( neighbours[c] != null && neighbours[c] == _pMC )
      ReleaseObject( (IComponentPtr*)&neighbours[c] );
}

void MeshCell::ExcludeElement( MeshElementPtr _pME )
{
  for( uint c = 0; c < 4; c++ )
    if( elements[c] != null && elements[c] == _pME )
      ReleaseObject( (IComponentPtr*)&elements[c] );
}

void MeshCell::ExcludePoint( MeshPointPtr _pMP )
{
  for( uint c = 0; c < 4; c++ ) {
    if( points[c] != null && points[c] == _pMP ) {
      if( _pMP->CutFlag() != 0 ) {
        if( c == 0 || c == 3 ) {
          if( points[1] != null ) points[1]->ExcludePoint( _pMP );
          if( points[2] != null ) points[2]->ExcludePoint( _pMP );
        }
        if( c == 1 || c == 2 ) {
          if( points[0] != null ) points[0]->ExcludePoint( _pMP );
          if( points[3] != null ) points[3]->ExcludePoint( _pMP );
        }
      }
      ReleaseObject( (IComponentPtr*)&points[c] );
    }
  }
}
