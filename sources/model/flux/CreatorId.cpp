#include "CreatorId.h"

CreatorId::CreatorId( int _area, int _count,
                      double _alpha1, double _alpha2,
                      DistrLaw _law, int _carreFlag ):
  isGood( true ), type( POLOIDAL ), unchanged( true ),
  area( _area ), count( _count ), alpha1( _alpha1 ), alpha2( _alpha2 ),
  law( _law ), carreFlag( _carreFlag ), level1( 0. ), level2( 1. ) {}

CreatorId::CreatorId( int _count, double _alpha1, double _alpha2, DistrLaw _law ):
  isGood( false ), type( UNKNOWN ), unchanged( true ),
  area( 0 ), count( _count ), alpha1( _alpha1 ), alpha2( _alpha2 ),
  law( _law ), carreFlag( 0 ), level1( 0. ), level2( 1. ) {}

CreatorId::CreatorId( int _area, int _count,
                      double _alpha1, double _alpha2,
                      double _level1, double _level2,
                      DistrLaw _law, int _carreFlag ):
  isGood( true ), type( TOROIDAL ), unchanged( true ),
  area( _area ), count( _count ),
  alpha1( _alpha1 ), alpha2( _alpha2 ),
  law( _law ), carreFlag( _carreFlag ),
  level1( _level1 ), level2( _level2 ) {}

/* Default values were taken from Equil::RebuildCarreGridPoints,
 * Equil::Carre_WriteGridPoints and Equil::Carre_WriteSurfaces
 * as kind of empty CreatorId */
CreatorId::CreatorId( int _area /* default: 0 */ ):
  isGood( false ), type( UNKNOWN ), unchanged( true ),
  area( _area ), count( 0 ), alpha1( 1. ), alpha2( 1. ),
  law( DGLAW::NORMAL ), carreFlag( 1 ), level1( 0. ), level2( 1. ) {}

CreatorId::CreatorId( const std::string& str )
{
  ParseString( str );
}

CreatorId CreatorId::ChangedCopy() const
{
  CreatorId crid_result = *this;
  crid_result.unchanged = false;
  return crid_result;
}

bool CreatorId::IsEqualTo( const CreatorId& _crid, bool exceptUnchangedFlag ) const
{
  const double eps = 1e-5;
  return (  ( unchanged == _crid.unchanged || exceptUnchangedFlag ) &&
            area == _crid.area &&
            count == _crid.count &&
            dg_about( alpha1, _crid.alpha1, eps ) &&
            dg_about( alpha2, _crid.alpha2, eps ) &&
            law == _crid.law &&
            carreFlag == _crid.carreFlag &&
            ( type == POLOIDAL ||
              ( dg_about( level1, _crid.level1, eps ) &&
                dg_about( level2, _crid.level2, eps ) ) ) );
}

bool CreatorId::operator == ( const CreatorId& _crid ) const
{
  return IsEqualTo( _crid );
}

bool CreatorId::operator != ( const CreatorId& _crid ) const
{
  return !IsEqualTo( _crid );
}


bool CreatorId::ParseString( const std::string& crid_str )
{
  const char* crid_cstr = crid_str.c_str();

  if( crid_cstr[0] == '*' ) {
    *this = CreatorId();
    return false;
  }

  if( !crid_str.empty() &&
      crid_cstr[0] == CID_UNCHANGEDFLAG ) {
    unchanged = true;
    crid_cstr++;
  }
  else
    unchanged = false;

  isGood = false;

  int _area = 0, _count = 0, _carreFlag = 0, _law = (int)DGLAW::NORMAL;
  double _alpha1 = 1., _alpha2 = 1., _level1 = 0., _level2 = 1.;

  switch( crid_cstr[0] ) {
  case 'z':
    isGood = (sscanf( crid_cstr, "z%dn%dh"SCANFLT"H"SCANFLT"w%dC%d",
        &_area, &_count, &_alpha1, &_alpha2, &_law, &_carreFlag ) >= 5 );
    type = POLOIDAL;
    break;
  case 'a':
    isGood = (sscanf( crid_cstr, "a%dl"SCANFLT"L"SCANFLT"n%dh"SCANFLT"H"SCANFLT"w%dC%d",
                &_area, &_level1, &_level2, &_count,
                &_alpha1, &_alpha2, &_law, &_carreFlag ) >= 7 );
    type = TOROIDAL;
    break;
  }
  area = _area;
  count = _count;
  alpha1 = _alpha1;
  alpha2 = _alpha2;
  law = (DistrLaw)_law;
  carreFlag = _carreFlag;
  level1 = _level1;
  level2 = _level2;

  return isGood;
}

std::string CreatorId::ToStdString() const
{
  if( !isGood )
    return "*";

  const size_t buffer_size = 1024;
  char* crid_cstr = new char[buffer_size]; //TODO: make c++ formatting
  memset( crid_cstr, 0, buffer_size );
  switch( type ) {
  case POLOIDAL:
    sprintf( crid_cstr, "+z%dn%dh%gH%gw%dC%d",
             area, count, alpha1, alpha2, (int)law, carreFlag );
    break;
  case TOROIDAL:
    sprintf( crid_cstr, "+a%dl%gL%gn%dh%gH%gw%dC%d",
             area, level1, level2, count, alpha1, alpha2, (int)law, carreFlag );
    break;
  case UNKNOWN:
    return "*";
  }

  std::string crid_str( crid_cstr );
  delete[] crid_cstr;
  return crid_str;
}

bool CreatorId::CalcDeltaCoeffs( double* paa,double* pbb,double* pcc) const
{
  double aa,bb,cc,l,denom,t;
  int n;

  l=1;
  n=dg_max(1,count+1);

  if (alpha1<=0 || alpha2<=0) return -1;

  if (n<=1) {
    aa=l;
    bb=0.;
    cc=0.;
  } else if (n==2) {
    t=(3.*(1.-alpha1/alpha2)-2.*(1.-2.*alpha1/alpha2));
    if( t == 0 )
      return false;
    bb=l/t;
    aa=-(1.-2.*alpha1/alpha2)*bb;
    bb=(1.-alpha1/alpha2)*bb;
    cc=0.;
  } else {
    denom = -((n-1)*(n-1))*(n-2)/6.;
    if (denom==0) return -1;
    aa = (alpha1*(-n)*(n+1)*(n-1)/6. - alpha2*(n+1)*(n-1)/3. + l*(n-1))
          /denom;
    bb = (alpha1*(-(n+1)*(2*n+1)/6. + n*n) + alpha2*((n+1)*(2*n+1)/6. - 1)
          + l*(1-n*n)/n)/denom;
    cc = (n-1)*(-alpha1/2. - alpha2/2. + l/n)/denom;
  }

  *paa=aa;
  *pbb=bb;
  *pcc=cc;

  return true;
}

double CreatorId::CalcValue( double x ) const
{
  switch( law ) {
    case DGLAW::NORMAL:
      if( x < 0 ) return 1;
      return pow(x,alpha1);
    case DGLAW::FLIPPED:
      if( x < 0 ) return 1;
      return 1 - pow( x, alpha1 );
    case DGLAW::TWOALPHA:
      if( x < 0 ) return 1;
      return x < 0.5 ? pow( x*2, alpha1 ) / 2 :
                       1 - pow( (1 - x)*2, alpha2 ) / 2;
    case DGLAW::SYMMETRIC:
      if( x < 0 ) return 1;
      return x < 0.5 ? pow( x*2, alpha1 ) / 2 :
                       1 - pow( (1 - x)*2, alpha1 ) / 2;
    case DGLAW::DELTA: {
      int n, n0;
      n = n0 = dg_max( 1, count+1 );

      double aa, bb, cc;
      if( !CalcDeltaCoeffs( &aa, &bb, &cc) )
        return -1;

      if (x<0) {
        for( int i = 1; i <= n; i++ )
          if( aa + bb*i + cc*i*i <= 0)
            return -1;
        return 1;
      }

      n=dg_min(n,x*n+1);
      double t = 0.;
      for( int i = 1; i < n; i++ )
        t += aa + bb*i + cc*i*i;
      t += (aa + bb*n + cc*n*n) * (x*n0 + 1 - n);

      return t;
    }
  }
  return 0.; /* Make the compiler happy */
}

