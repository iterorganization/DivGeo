#include "StringConverters.h"

QString ToQString( const QString& s )
{
  return s;
}

QString ToQString( const std::string& s )
{
  return QString::fromStdString( s );
}

QString ToQString( int v )
{
  return QString( "%1" ).arg( v );
}

QString ToQString( ulong v )
{
  return QString( "%1" ).arg( v );
}

QString ToQString( const NPoint& p, bool size )
{
  return (size ? QString( "%1 x %2" ) :
                 QString( "[%1, %2]" )).arg( p.x ).arg( p.y );
}

QString ToQString( double v1, double v2 )
{
  return QString( "[%1, %2]" ).arg( v1 ).arg( v2 );
}

QString ToQString( double v )
{
  return QString( "%1" ).arg( v );
}

QString ToQString( const Point& p )
{
  return QString( "(%1, %2)" ).arg( p.x ).arg( p.y );
}

QString ToQString( const QPointF& p )
{
  return QString( "(%1, %2)" ).arg( p.x() ).arg( p.y() );
}

QString ToQString( bool b, StringsManager* pSM )
{
  return b ? SM_GUI( GUI::COMMON::YES ) : SM_GUI( GUI::COMMON::NO );
}

QString ToQString( const CreatorId& crid, StringsManager* pSM )
{
  if( !crid.IsGood() )
    return SM_GUI( GUI::MODEL_INFO::MODEL::BAD_CREATORID );

  int area = crid.Area();
  if( crid.Type() == CreatorId::TOROIDAL ) {
    QString area_str;
    switch( area ) {
    case 1: area_str = SM_DLG( DLG::CREATE_SURFACE::AREA::AREA1 ); break;
    case 2: area_str = SM_DLG( DLG::CREATE_SURFACE::AREA::AREA2 ); break;
    case 3: area_str = SM_DLG( DLG::CREATE_SURFACE::AREA::AREA3 ); break;
    default:area_str = QString( "%1" ).arg( crid.Area() ); break;
    }

    return QString( "%2 %3\n%4 %5\n%6 %7\n%8 %9\n%10 %11\n%12 %13\n%14 %15\n%16 %17" )
        //.arg( QString::fromStdString( crid.ToStdString() ) )
        .arg( SM_GUI( GUI::MODEL_INFO::MODEL::UNCHANGED ) )
        .arg( ToQString( crid.IsUnchanged() ) )
        .arg( SM_GUI( GUI::MODEL_INFO::MODEL::CARREMODE ) )
        .arg( ToQString( crid.CarreFlag() != 0 ) )
        .arg( SM_DLG( DLG::CREATE_SURFACE::AREA::TITLE ) )          .arg( area_str )
        .arg( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL1 ) ).arg( crid.level1 )
        .arg( SM_DLG( DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL2 ) ).arg( crid.level2 )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::COUNT ) )              .arg( crid.Count() )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::A1 ) )                 .arg( crid.Alpha1() )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::A2 ) )                 .arg( crid.Alpha2() );
  }
  else if( crid.Type() == CreatorId::POLOIDAL ) {
    QString area_str;
    switch( area ) {
    case 1: area_str = SM_DLG( DLG::CREATE_GPOINT::AREA::AREA0 ); break;
    case 2: area_str = SM_DLG( DLG::CREATE_GPOINT::AREA::AREA1 ); break;
    case 3: area_str = SM_DLG( DLG::CREATE_GPOINT::AREA::AREA2 ); break;
    default:area_str = QString( "%1" ).arg( crid.Area() );
    }

    return QString( "%2 %3\n%4 %5\n%6 %7\n%8 %9\n%10 %11\n%12 %13" )
        //.arg( QString::fromStdString( crid.ToStdString() ) )
        .arg( SM_GUI( GUI::MODEL_INFO::MODEL::UNCHANGED ) )
        .arg( ToQString( crid.IsUnchanged() ) )
        .arg( SM_GUI( GUI::MODEL_INFO::MODEL::CARREMODE ) )
        .arg( ToQString( crid.CarreFlag() != 0 ) )
        .arg( SM_DLG( DLG::CREATE_GPOINT::AREA::TITLE ) ) .arg( area_str )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::COUNT ) )    .arg( crid.Count() )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::A1 ) )       .arg( crid.Alpha1() )
        .arg( SM_DLG( DLG::DISTR_GRAPH::LBL::A2 ) )       .arg( crid.Alpha2() );
  }
  else
    return QString::fromStdString( crid.ToStdString() );
}
