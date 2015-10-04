#ifndef USERPREFERENCES_H
#define USERPREFERENCES_H

#include "SettingsManager.h"
#include "../dialogs/PropertyTableWidget.h"

struct SStyleIndex
{
  enum EStyleIndices
  {
    EMPTY = 0,

    AXES, GRID, GRIDTEXT,
    LABEL, COMMENT, TOOL,
    MARK, HIGHLIGHT, ERROR,
    TEXT, NORMAL,

    NODE, ELEM, SONNETDATA,
    SEPARATOR, SOURCE, CHORD, TEMPLATE,

    EQUIL_POS, EQUIL_NEG,

    SURFACE, XPOINT, GPOINT,
    XPOINTTEST, XPOINTTEST_BOUND,
    XPOINTSEG,

    MESHELEMENT,
    MESHELEM_IRREG,
    MESHELEM_IRREG_HL,
    MESHELEM_IRREG_ERR,
    MESHCELLCENTER
  };

  static const unsigned size = 32;
};

typedef SStyleIndex::EStyleIndices StyleIndex;
typedef SStyleIndex STYLE;

class UserPreferences:
    public SettingsManager
{
public:
  /* Properties data fields */
  struct Common {
    ulong autosaveInterval_sec;
    ulong recentList_len;

    Common():
      autosaveInterval_sec( 5*60 ),
      recentList_len( 10 ) {}
  } common;

  struct Visual {
    QVector< QPen* > pens;
    QPen pen_default;

    QColor background_color;

    ulong nodeRadius, equilLen, normalLen, gridPointLen, sourceRadius, gridCellMax,
    showPictureIncr, meshCellCenterLength, meshPointRadius, separatrixSegEndLen,
    labelTextSize, commentTextSize;

    NPoint labelOffset, arrowSize;

    bool useSquares;

    Visual():
      background_color( Qt::black ),

      nodeRadius( 4 ),
      normalLen( 10 ),
      gridPointLen( 30 ),
      sourceRadius( 20 ),
      gridCellMax( 330 ),
      meshCellCenterLength( 3 ),
      meshPointRadius( 10 ), //dg2: 3
      separatrixSegEndLen( 48 ),
      labelTextSize( 11 ),
      commentTextSize( 9 ),

      labelOffset( 30 ),
      arrowSize( 3., 30. ),

      useSquares( false )
    {
      pens.resize( SStyleIndex::size );
      pens[ STYLE::EMPTY ]               = new QPen();
      pens[ STYLE::AXES ]                = new QPen( frgb( 1.0, 0.3, 0.0 ),  1 );
      pens[ STYLE::GRID ]                = new QPen( frgb( 0.3, 0.3, 0.3 ),  1 );
      pens[ STYLE::GRIDTEXT ]            = new QPen( QColor( Qt::white ),    8 );
      pens[ STYLE::LABEL ]               = new QPen( QColor( Qt::green ),    1 );
      pens[ STYLE::COMMENT ]             = new QPen( frgb( 0.5, 1.0, 1.0 ),  1 );
      pens[ STYLE::TOOL ]                = new QPen( QColor( Qt::green ),    1 );
      pens[ STYLE::MARK ]                = new QPen( QColor( Qt::blue ),     5 );
      pens[ STYLE::HIGHLIGHT ]           = new QPen( QColor( Qt::green ),    1 );
      pens[ STYLE::ERROR ]               = new QPen( QColor( Qt::red ),      1 );
      pens[ STYLE::TEXT ]                = new QPen( QColor( Qt::white ),    8 );
      pens[ STYLE::NORMAL ]              = new QPen( QColor( Qt::magenta ),  1 );

      pens[ STYLE::NODE ]                = new QPen( QColor( Qt::red ),      1 );
      pens[ STYLE::ELEM ]                = new QPen( QColor( Qt::white ),    1 );

      pens[ STYLE::SONNETDATA ]          = new QPen( QColor( Qt::blue ),     1 );
      pens[ STYLE::SEPARATOR ]           = new QPen( QColor( Qt::cyan ),     1 );
      pens[ STYLE::SOURCE ]              = new QPen( QColor( Qt::white ),    1 );
      pens[ STYLE::CHORD ]               = new QPen( QColor( Qt::cyan ),     1 );
      pens[ STYLE::TEMPLATE ]            = new QPen( QColor( Qt::cyan ),     1 );

      pens[ STYLE::EQUIL_POS ]           = new QPen( QColor( Qt::blue ),     1 );
      pens[ STYLE::EQUIL_NEG ]           = new QPen( QColor( Qt::red ),      1 );

      pens[ STYLE::SURFACE ]             = new QPen( QColor( Qt::yellow ),   1 );
      pens[ STYLE::XPOINT ]              = new QPen( QColor( Qt::red ),      1 );
      pens[ STYLE::GPOINT ]              = new QPen( QColor( Qt::magenta ),  1 );
      pens[ STYLE::XPOINTTEST_BOUND ]    = new QPen( frgb( 1.0, 1.0, 0.0 ),  1 );
      pens[ STYLE::XPOINTTEST ]          = new QPen( frgb( 0.4, 0.4, 0.4 ),  1 );
      pens[ STYLE::XPOINTSEG ]           = new QPen( frgb( 1.0, 0.4, 0.0 ),  1 );

      pens[ STYLE::MESHELEMENT ]         = new QPen( frgb( 0.5, 0.5, 1.0 ),  1 );
      pens[ STYLE::MESHELEM_IRREG ]      = new QPen( frgb( 1.0, 0.5, 0.5 ),  3 );
      pens[ STYLE::MESHELEM_IRREG_HL ]   = new QPen( QColor( Qt::yellow ),   3 );
      pens[ STYLE::MESHELEM_IRREG_ERR ]  = new QPen( QColor( Qt::red ),      3 );
      pens[ STYLE::MESHCELLCENTER ]      = new QPen( frgb( 0.5, 0.5, 1.0 ),  1 );

      for( unsigned c = 0; c < SStyleIndex::size; c++ )
        pens[c]->setCosmetic( true );
    }
  } visual;

  UserPreferences( const QString& _crsProfileDir = "", const QString& _crsName = "" );
};

#endif // USERPREFERENCES_H
