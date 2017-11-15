#include "ViewParameters.h"

ViewParameters::ViewParameters()
{
  InitDefaultColors();
  InitDefaultWidthes();
  InitDefaultDelays();
  InitDefaultSettings();

  InitPens();
}

void ViewParameters::InitDefaultColors()
{
  colors.noAlfaMask       = 0x00ffffff;
  colors.background       = QColor( Qt::black ).rgb();    // 0.
  colors.highlight        = QColor( Qt::green ).rgb();    // 0. 1.
  colors.errorHighlight   = QColor( Qt::red ).rgb();      // 0. 1.
  colors.node             = QColor( Qt::red ).rgb();      // .5
  colors.elem             = QColor( Qt::white ).rgb();    // .5
  colors.surface          = QColor( Qt::yellow ).rgb();   // .1 .5
  colors.highlightRect    = QColor( Qt::green ).rgb();    // 0. 1.
  colors.template_        = QColor( Qt::cyan ).rgb();     // .3
  colors.normal           = QColor( Qt::magenta ).rgb();  // .5
  colors.mark             = QColor( Qt::blue ).rgb();     // .5
  colors.xPoint           = QColor( Qt::red ).rgb();      // .7
  colors.equil1           = QColor( Qt::red ).rgb();      // .6 .1
  colors.equil2           = QColor( Qt::blue ).rgb();     // .8 .2
  colors.axes             = frgb( 1.0, 0.3, 0.0 );        // .3
  colors.grid             = frgb( 0.3, 0.3, 0.3 );        // .2
  colors.gridText         = QColor( Qt::white ).rgb();    // .3
  colors.elemNumber       = QColor( Qt::white ).rgb();    // .3
  colors.viewLabel        = QColor( Qt::green ).rgb();    // .9
  colors.sonnetData       = QColor( Qt::blue ).rgb();     // .3
  colors.gridPoint        = QColor( Qt::magenta ).rgb();  // .7
  colors.separator        = QColor( Qt::cyan ).rgb();     // .7
  colors.source           = QColor( Qt::white ).rgb();    // .7
  colors.chord            = QColor( Qt::cyan ).rgb();     // .7
  colors.xPointTest1      = frgb( 1.0, 1.0, 0.0 );        // .7
  colors.xPointTest2      = frgb( 0.4, 0.4, 0.4 );        // .7
  colors.xPointSeg        = frgb( 1.0, 0.4, 0.0 );        // .7

  colors.meshElement1     = frgb( 0.7, 0.7, 0.7 );
  colors.meshElement2     = frgb( 0.7, 0.7, 0.7 );
  colors.meshElement_x1   = QColor( Qt::yellow ).rgb();
  colors.meshElement_x2   = QColor( Qt::red ).rgb();
  colors.meshCellCenter1  = frgb( 0.7, 0.7, 0.7 );
  colors.meshCellCenter2  = frgb( 0.7, 0.7, 0.7 );
  colors.lTool            = QColor( Qt::blue ).rgb();     // .7
  colors.mTool            = QColor( Qt::magenta ).rgb();  // .7
  colors.rTool            = QColor( Qt::red ).rgb();      // .7
  colors.iTool            = frgb( 0.7, 0.7, 0.7 );        // 0.
}

void ViewParameters::InitDefaultWidthes()
{
  widthes.highlight = 1; // 0
  widthes.node = 1;
  widthes.elem = 1;
  widthes.surface = 1;
  widthes.highlightRect = 1; // 0
  widthes.template_ = 1;
  widthes.normal = 1;
  widthes.mark = 5;
  widthes.xPoint = 1;
  widthes.equil = 1;
  /*+*/widthes.axes = 1;
  /*+*/widthes.grid = 1;
  /*+*/widthes.gridText = 1;
  widthes.elemNumber = 1;
  widthes.viewLabel = 1;
  widthes.sonnetData = 1;
  widthes.gridPoint = 1;
  widthes.separator = 1;
  widthes.source = 1;
  widthes.chord = 1;
  widthes.xPointTest1 = 1;
  widthes.xPointTest2 = 1;
  widthes.xPointSeg = 1;
  widthes.meshElement1 = 1;
  widthes.meshElement2 = 1;
  widthes.meshCellCenter1 = 1;
  widthes.meshCellCenter2 = 1;
}

void ViewParameters::InitDefaultDelays()
{
  delays.update = 1000;
  delays.updateVars = 1000;
  delays.updateInfo = 1000;
  delays.updateMouse = 1000;
  delays.exposure = 300;
}

void ViewParameters::InitDefaultSettings()
{
  settings.nodeRadius = 4;
  settings.equilLen = 10;
  settings.minZoom =20;
  settings.normalLen = 30;
  settings.gridPointLen = 30;
  settings.sourceR = 20;
  settings.arrowSize = Point( 3., 30. );
  settings.gridCellMax = 330;
  settings.labelOffset = 30;
  settings.showPictureIncr = 5;
  settings.meshCellCenterLength = 3;
  settings.meshPointRadius = 3;
  settings.separatrixSegEndLen = 48;
  settings.useSquares = false;
}

void ViewParameters::InitPens()
{
  pens.resize( SViewModes::size );

  // MB: "colors.highlight" and "colors.highlightRect" colors in X-mode pens removed ternars
  //     "(pMainWnd->InHighlightMode() ? colors.highlight/*Rect*/ : colors.errorHighlight)"

  QPen penX( QColor::fromRgb( (colors.background & colors.noAlfaMask) ^ colors.highlight ),
             widthes.highlight );

  /* Pens used to draw highlighted items transparent for backlayered items */
  pens[ VM::NODE_X ] = penX;
  pens[ VM::ELEM_X ] = penX;
  pens[ VM::EQUIL_X ] = penX;
  pens[ VM::SURFACE_X ] = penX;
  pens[ VM::TEMPLATE_X ] = penX;
  pens[ VM::ELEMNORMAL_X ] = penX;
  pens[ VM::XPOINT_X ] = penX;
  pens[ VM::GPOINT_X ] = penX;
  pens[ VM::SEPARATOR_X ] = penX;
  pens[ VM::SOURCE_X ] = penX;
  pens[ VM::CHORD_X ] = penX;
  pens[ VM::XPOINTTEST_X ] = penX;
  pens[ VM::XPOINTSEG_X ] = penX;
  pens[ VM::MESHELEMENT_X ] = penX;
  pens[ VM::MESHCELLCENTER_X ] = penX;

  //pens[ VM::MESHELEMENT_X12 ] = QPen( QColor::fromRgb( ( (colors.background & colors.noAlfaMask) ^ colors.highlight) ^ colors.meshElement2), widthes.highlight );
  //pens[ VM::MESHELEMENT_X2 ] =  QPen( QColor::fromRgb( (colors.background & colors.noAlfaMask) ^ colors.meshElement2 ), widthes.meshElement2 );

  pens[ VM::HIGHLIGHTRECT_X ] = QPen( QColor::fromRgb( (colors.background & colors.noAlfaMask) ^ colors.highlightRect ), widthes.highlightRect );

  /* Pens used to overdraw removed items fast with background color. Not used since 3.0*/
  pens[ VM::NODE_0 ]            = QPen( QColor::fromRgb( colors.background ), widthes.node );
  pens[ VM::ELEM_0 ]            = QPen( QColor::fromRgb( colors.background ), widthes.elem );
  pens[ VM::EQUIL_0 ]           = QPen( QColor::fromRgb( colors.background ), widthes.equil );
  pens[ VM::HIGHLIGHTRECT_0 ]   = QPen( QColor::fromRgb( colors.background ), widthes.highlightRect );
  pens[ VM::SURFACE_0 ]         = QPen( QColor::fromRgb( colors.background ), widthes.surface );
  pens[ VM::TEMPLATE_0 ]        = QPen( QColor::fromRgb( colors.background ), widthes.template_ );
  pens[ VM::ELEMNORMAL_0 ]      = QPen( QColor::fromRgb( colors.background ), widthes.normal );
  pens[ VM::XPOINT_0 ]          = QPen( QColor::fromRgb( colors.background ), widthes.xPoint );
  pens[ VM::ELEMMARK_0 ]        = QPen( QColor::fromRgb( colors.background ), widthes.mark );
  pens[ VM::ELEMNUMBER_0 ]      = QPen( QColor::fromRgb( colors.background ), widthes.elemNumber );
  pens[ VM::VIEWLABEL_0 ]       = QPen( QColor::fromRgb( colors.background ), widthes.viewLabel );
  pens[ VM::SONNETDATA_0 ]      = QPen( QColor::fromRgb( colors.background ), widthes.sonnetData );
  pens[ VM::GPOINT_0 ]          = QPen( QColor::fromRgb( colors.background ), widthes.gridPoint );
  pens[ VM::SEPARATOR_0 ]       = QPen( QColor::fromRgb( colors.background ), widthes.separator );
  pens[ VM::SOURCE_0 ]          = QPen( QColor::fromRgb( colors.background ), widthes.source );
  pens[ VM::CHORD_0 ]           = QPen( QColor::fromRgb( colors.background ), widthes.chord );
  pens[ VM::XPOINTTEST_0 ]      = QPen( QColor::fromRgb( colors.background ), dg_max( widthes.xPointTest1, widthes.xPointTest2 ) );
  pens[ VM::XPOINTSEG_0 ]       = QPen( QColor::fromRgb( colors.background ), widthes.xPointSeg );
  pens[ VM::MESHELEMENT_0 ]     = QPen( QColor::fromRgb( colors.background ), dg_max( widthes.meshElement1, widthes.meshElement2 ) );
  pens[ VM::MESHCELLCENTER_0 ]  = QPen( QColor::fromRgb( colors.background ), dg_max( widthes.meshCellCenter1, widthes.meshCellCenter2 ) );

  /* Regular pens used to draw items*/
  pens[ VM::NODE_1 ]            = QPen( QColor::fromRgb( colors.node ),             widthes.node );
  pens[ VM::ELEM_1 ]            = QPen( QColor::fromRgb( colors.elem ),             widthes.elem );
  pens[ VM::EQUIL_1 ]           = QPen( QColor::fromRgb( colors.equil1 ),           widthes.equil );
  pens[ VM::EQUIL_2 ]           = QPen( QColor::fromRgb( colors.equil2 ),           widthes.equil );
  pens[ VM::EQUIL_3 ]           = QPen( QColor::fromRgb( colors.xPoint ),           widthes.xPoint );
  pens[ VM::HIGHLIGHTRECT_1 ]   = QPen( QColor::fromRgb( colors.highlightRect ),    widthes.highlightRect );
  pens[ VM::SURFACE_1 ]         = QPen( QColor::fromRgb( colors.surface ),          widthes.surface );
  pens[ VM::TEMPLATE_1 ]        = QPen( QColor::fromRgb( colors.template_ ),        widthes.template_ );
  pens[ VM::ELEMNORMAL_1 ]      = QPen( QColor::fromRgb( colors.normal ),           widthes.normal );
  pens[ VM::XPOINT_1 ]          = QPen( QColor::fromRgb( colors.xPoint ),           widthes.xPoint );
  pens[ VM::ELEMMARK_1 ]        = QPen( QColor::fromRgb( colors.mark ),             widthes.mark );
  pens[ VM::AXES_1 ]            = QPen( QColor::fromRgb( colors.axes ),             widthes.axes );
  pens[ VM::GRID_1 ]            = QPen( QColor::fromRgb( colors.grid ),             widthes.grid );
  pens[ VM::GRIDTEXT_1 ]        = QPen( QColor::fromRgb( colors.gridText ),         widthes.gridText );
  pens[ VM::ELEMNUMBER_1 ]      = QPen( QColor::fromRgb( colors.elemNumber ),       widthes.elemNumber );
  pens[ VM::VIEWLABEL_1 ]       = QPen( QColor::fromRgb( colors.viewLabel ),        widthes.viewLabel );
  pens[ VM::SONNETDATA_1 ]      = QPen( QColor::fromRgb( colors.sonnetData ),       widthes.sonnetData );
  pens[ VM::GPOINT_1 ]          = QPen( QColor::fromRgb( colors.gridPoint ),        widthes.gridPoint );
  pens[ VM::SEPARATOR_1 ]       = QPen( QColor::fromRgb( colors.separator ),        widthes.separator );
  pens[ VM::SOURCE_1 ]          = QPen( QColor::fromRgb( colors.source ),           widthes.source );
  pens[ VM::CHORD_1 ]           = QPen( QColor::fromRgb( colors.chord ),            widthes.chord );
  pens[ VM::XPOINTTEST_1 ]      = QPen( QColor::fromRgb( colors.xPointTest1 ),      widthes.xPointTest1 );
  pens[ VM::XPOINTTEST_2 ]      = QPen( QColor::fromRgb( colors.xPointTest2 ),      widthes.xPointTest2 );
  pens[ VM::XPOINTSEG_1 ]       = QPen( QColor::fromRgb( colors.xPointSeg ),        widthes.xPointSeg );
  pens[ VM::MESHELEMENT_1 ]     = QPen( QColor::fromRgb( colors.meshElement1 ),     widthes.meshElement1 );
  pens[ VM::MESHELEMENT_2 ]     = QPen( QColor::fromRgb( colors.meshElement2 ),     widthes.meshElement2 );
  pens[ VM::MESHELEMENT_X12 ]   = QPen( QColor::fromRgb( colors.meshElement_x1 ),   widthes.highlight );
  pens[ VM::MESHELEMENT_X2 ]    = QPen( QColor::fromRgb( colors.meshElement_x2 ),   widthes.meshElement2 );
  pens[ VM::MESHCELLCENTER_1 ]  = QPen( QColor::fromRgb( colors.meshCellCenter1 ),  widthes.meshCellCenter1 );
  pens[ VM::MESHCELLCENTER_2 ]  = QPen( QColor::fromRgb( colors.meshCellCenter2 ),  widthes.meshCellCenter2 );

  for( unsigned c = 0; c < SViewModes::size; c++ )
    pens[c].setCosmetic( true );
}

const QPen& ViewParameters::Pen( ViewMode mode ) const
{
  if( (int)mode > SViewModes::size || (int)mode < 0 ) {
    throw "unknown view mode";
    return pen_default;
  }

  return pens[ mode ];
}
