#include "UserPreferences.h"

UserPreferences::UserPreferences( const QString& _crsProfileDir, const QString& _crsName ):
  SettingsManager( _crsProfileDir, _crsName )
{

  typedef DLG::SETTINGS::COMMON COM;
  vTable.append( PR( "Common",            COM::TITLE,                 PR::PT::BEGIN ) ); vTable.last().updateAll = true;
  vTable.append( PR( "AutosaveInterval",  COM::SAVEINTERVAL,          PR::PT::INT,    QVariant::fromValue( (IntPtr)&common.autosaveInterval_sec ) ) << 0 << 86400 );
  vTable.append( PR( "RecentListLength",  COM::RECENT_LENGTH,         PR::PT::INT,    QVariant::fromValue( (IntPtr)&common.recentList_len ) ) << 0 << 50 );
  vTable.append( PR( "AxesLabelsOnBorder",COM::AXISLABELS,            PR::PT::BOOL,   QVariant::fromValue( (BoolPtr)&common.axisLabelsAlwaysOnBorder ) ) );
  vTable.append( PR( "ExtFilterModel",    COM::EXTMODEL,              PR::PT::STRING, QVariant::fromValue( (QStringPtr)&common.sExtFilterModel ) ) );
  vTable.append( PR( "ExtFilterTemplate", COM::EXTTEMPL,              PR::PT::STRING, QVariant::fromValue( (QStringPtr)&common.sExtFilterTemplate ) ) );
  vTable.append( PR( "ExtFilterTopology", COM::EXTTOPO,               PR::PT::STRING, QVariant::fromValue( (QStringPtr)&common.sExtFilterTopology ) ) );
  vTable.append( PR( "ExtFilterEquil",    COM::EXTEQUIL,              PR::PT::STRING, QVariant::fromValue( (QStringPtr)&common.sExtFilterEquil ) ) );
  vTable.append( PR( "ExtFilterMesh",     COM::EXTMESH,               PR::PT::STRING, QVariant::fromValue( (QStringPtr)&common.sExtFilterMesh ) ) );
  vTable.append( PR( PR::PT::END ) );

  typedef DLG::SETTINGS::VISUAL VIS;
  vTable.append( PR( "Visual",            VIS::TITLE,                 PR::PT::BEGIN ) << PREVIEW::COMMON );
  vTable.append( PR( "BackgroundColor",   VIS::BACKGROUND,            PR::PT::COLOR,  QVariant::fromValue( (QColorPtr)&visual.background_color ) ) );
  vTable.append( PR( "UseSquares",        VIS::USESQUARES,            PR::PT::BOOL,   QVariant::fromValue( (BoolPtr)&visual.useSquares ) ) );
  vTable.append( PR( "ArrowSize",         VIS::ARROW_SIZE,            PR::PT::NPOINT, QVariant::fromValue( (NPointPtr)&visual.arrowSize ) ) << 0 << 50 );

  vTable.append( PR( "Mark_Highlight",    VIS::MARK_HL::TITLE,        PR::PT::BEGIN ) << PREVIEW::MARK_HL );
  vTable.append( PR( "Mark",              VIS::MARK_HL::MARK,         PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::MARK ] ) ) );
  vTable.append( PR( "Highlight",         VIS::MARK_HL::HL,           PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::HIGHLIGHT ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Tool",              VIS::TOOL,                  PR::PT::BEGIN ) << PREVIEW::TOOLS );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::TOOL ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Grid_Axes",         VIS::GRID_AXES::TITLE,      PR::PT::BEGIN ) << PREVIEW::GRID_AXES );
  vTable.append( PR( "Grid",              VIS::GRID_AXES::GRID,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::GRID ] ) ) );
  vTable.append( PR( "GridText",          VIS::GRID_AXES::TEXT,       PR::PT::FONT,   QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::GRIDTEXT ] ) ) );
  vTable.append( PR( "Axes",              VIS::GRID_AXES::AXES,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::AXES ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Labels",            VIS::LABEL,                 PR::PT::BEGIN ) << PREVIEW::LABELS );
  vTable.append( PR( "Line",              0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::LABEL ] ) ) );
  vTable.append( PR( "FontSize",          VIS::FONTSIZE,              PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.labelTextSize ) ) << 0 << 50 );
  vTable.append( PR( "Offset",            VIS::OFFSET,                PR::PT::NPOINT, QVariant::fromValue( (NPointPtr)&visual.labelOffset ) ) << -100 << 100 );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Comments",          VIS::COMMENT,               PR::PT::BEGIN ) << PREVIEW::COMMENTS ); vTable.last().updateAll = true;
  vTable.append( PR( "Line",              0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::COMMENT ] ) ) );
  vTable.append( PR( "FontSize",          VIS::FONTSIZE,              PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.commentTextSize ) ) << 0 << 50 );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Text_Normals",      VIS::TEXT_NORM::TITLE,      PR::PT::BEGIN ) << PREVIEW::TEXT_NORMALS );
  vTable.append( PR( "Text",              VIS::TEXT_NORM::TEXT,       PR::PT::FONT,   QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::TEXT ] ) ) );
  vTable.append( PR( "Normal",            VIS::TEXT_NORM::NORMAL,     PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::NORMAL ] ) ) );
  vTable.append( PR( "NormalLength",      VIS::TEXT_NORM::NORM_LEN,   PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.normalLen ) ) << 0 << 40 );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Nodes_Elements",    VIS::ELEM_NODE::TITLE,      PR::PT::BEGIN ) << PREVIEW::ELEMS_NODES );
  vTable.append( PR( "Node",              VIS::ELEM_NODE::NODE,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::NODE ] ) ) );
  vTable.append( PR( "Radius",            VIS::RADIUS,                PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.nodeRadius ) ) << 0 << 20 );
  vTable.append( PR( "Element",           VIS::ELEM_NODE::ELEM,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::ELEM ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Separators",        VIS::SEPARATOR,             PR::PT::BEGIN ) << PREVIEW::SEPARATORS );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::SEPARATOR ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Sources",           VIS::SOURCE,                PR::PT::BEGIN ) << PREVIEW::SOURCES );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::SOURCE ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Chords",            VIS::CHORD,                 PR::PT::BEGIN ) << PREVIEW::CHORDS );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::CHORD ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Template",          VIS::TEMPLATE,              PR::PT::BEGIN ) << PREVIEW::TEMPLATES );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::TEMPLATE ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Mesh",              VIS::MESH::TITLE,           PR::PT::BEGIN ) << PREVIEW::MESH );
  vTable.append( PR( "CellCenter",        VIS::MESH::CELLCENTER,      PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::MESHCELLCENTER ] ) ) );
  vTable.append( PR( "CellCenterRadius",  VIS::MESH::CCLENGTH,        PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.nodeRadius ) ) << 0 << 20 );
  vTable.append( PR( "Element",           VIS::MESH::ELEMENT,         PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::MESHELEMENT ] ) ) );
  vTable.append( PR( "Irregular",         VIS::MESH::IRREG,           PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::MESHELEM_IRREG ] ) ) );
  vTable.append( PR( "IrregularHighlight",VIS::MESH::IRREG_HL,        PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::MESHELEM_IRREG_HL ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Equil",             VIS::EQUIL::TITLE,          PR::PT::BEGIN ) << PREVIEW::EQUIL );
  vTable.append( PR( "PositiveColor",     VIS::EQUIL::POS,            PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::EQUIL_POS ] ) ) );
  vTable.append( PR( "NegativeColor",     VIS::EQUIL::NEG,            PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::EQUIL_NEG ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "XPoints_GPoints",   VIS::XGPOINT::TITLE,        PR::PT::BEGIN ) << PREVIEW::XG_POINTS );
  vTable.append( PR( "XPoint",            VIS::XGPOINT::XPOINT,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::XPOINT ] ) ) );
  vTable.append( PR( "GPoint",            VIS::XGPOINT::GPOINT,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::GPOINT ] ) ) );
  vTable.append( PR( "GPointLength",      VIS::XGPOINT::GPOINTLEN,    PR::PT::INT,    QVariant::fromValue( (IntPtr)&visual.gridPointLen ) ) << 0 << 100 );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "XPointTest",        VIS::XPOINTTEST::TITLE,     PR::PT::BEGIN ) << PREVIEW::XPT );
  vTable.append( PR( "XPointTest",        VIS::XPOINTTEST::XPT,       PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::XPOINTTEST ] ) ) );
  vTable.append( PR( "XPointTestBound",   VIS::XPOINTTEST::XPT_BOUND, PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::XPOINTTEST_BOUND ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Surfaces",          VIS::SURFACE,               PR::PT::BEGIN ) << PREVIEW::SURFACES );
  vTable.append( PR( "",                  0,                          PR::PT::PEN,    QVariant::fromValue( (QPenPtr)visual.pens[ STYLE::SURFACE ] ) ) );
  vTable.append( PR( PR::PT::END ) );

  Load();
}
