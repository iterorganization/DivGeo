#include "XPointTest.h"
#include "../core/Model.h"

XPointTest::XPointTest( ModelPtr _pModel, NPoint _c1, NPoint _c2, double _level, int _id ):
  IComponent( OT::XPOINTTEST, _pModel ),
  c1( _c1 ),
  c2( _c2 ), 
  level( _level ), 
  id( _id )
{ DG_PROPERTIES_INIT; }

XPointTest::XPointTest( const XPointTest& _crOther ):
  IComponent( OT::XPOINTTEST, pModel ),
  c1( _crOther.c1 ),
  c2( _crOther.c2 ),
  center( _crOther.center ),
  lvlMin( _crOther.lvlMin ),
  lvlMax( _crOther.lvlMax ),
  level( _crOther.level ),
  id( _crOther.id )
{
  DG_PROPERTIES_INIT;

  memcpy( minMax, _crOther.minMax, sizeof( XPointMinMax )*4 );

  for( int c = 0; c < 4; c++ ) {
    gradients[c] = new PointArray;
    *gradients[c] = *_crOther.gradients[c];
  }
}

XPointTest::~XPointTest()
{
  Lock();
  FOREACH_CONST( XPointSegIterConst, it, segs )
    ReleaseObject( (IComponentPtr)*it );
  segs.clear();
  segs_ic.clear();
  Unlock();
}

std::string XPointTest::Description() const
{ return pModel->GetStr( STR::XPOINTTEST ); }

std::string XPointTest::ShortInfo() const
{ return ToStdString( id ); }

std::string XPointTest::DetailedInfo() const
{ return Description() << "[" << id << "] " << center.ToString(); }

IComponentPtr XPointTest::GetLockingObject()
{
  if( segs.empty() )
    return null;

  IComponentPtr pXPS = segs.front();
  return pXPS->IsLocked() ? throw "XPointSeg can not be locked" : pXPS;
}

void XPointTest::SetMinMax( XPointMinMax _minMax[4] )
{
  for( int i = 0; i < 4; i++ ) 
    minMax[i] = _minMax[i];

  lvlMin = dg_max( minMax[0].lvl, minMax[2].lvl );
  lvlMax = dg_min( minMax[1].lvl, minMax[3].lvl );
}

void XPointTest::FreeArrays()
{
  for( int i = 0; i < 4; i++ )
    if( gradients[i] != null )
      delete gradients[i];
  segs.clear();
}

void XPointTest::Delete()
{
  XPointSegList segs_copy = segs;
  FOREACH_CONST( XPointSegIterConst, itXPS, segs_copy )
    (*itXPS)->Delete();
  segs_copy.clear();

  ActDelXPointTest actDelXPS( pModel, this );
  int result = actDelXPS.Do();

  assert( result == 0 );
}

const IComponentList& XPointTest::Segments()
{
  segs_ic.clear();
  FOREACH_CONST( XPointSegIterConst, itXPS, segs )
    segs_ic.push_back( *itXPS );
  return segs_ic;
}

void XPointTest::IncludeXPointSeg( XPointSegPtr _pXPS )
{
  segs.push_back( DG_SHARE( _pXPS ) );
}

void XPointTest::ExcludeXPointSeg( XPointSegPtr _pXPS )
{
  if( Remove( segs, _pXPS ) )
    ReleaseObject( (IComponentPtr)_pXPS );
}

bool XPointTest::HasXPointSeg( XPointSegPtr _pXPS )
{
  return Contains( segs, _pXPS );
}
