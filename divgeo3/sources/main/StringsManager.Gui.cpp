#include "StringsManager.h"

void StringsManager::GuiKeys()
{
  INIT_TREE;

  GROUP( "gui/common" );

  KEY( "no",              GUI::COMMON::NO );
  KEY( "yes",             GUI::COMMON::YES );

  GROUP( "gui/model_info" );

  KEY( "not_loaded",      GUI::MODEL_INFO::NOT_LOADED );
  KEY( "property",        GUI::MODEL_INFO::PROPERTY );
  KEY( "value",           GUI::MODEL_INFO::VALUE );
  KEY( "id",              GUI::MODEL_INFO::ID );
  KEY( "length",          GUI::MODEL_INFO::LENGTH );
  KEY( "position",        GUI::MODEL_INFO::POSITION );
  KEY( "selected",        GUI::MODEL_INFO::SELECTED );
  KEY( "no_selected",     GUI::MODEL_INFO::NO_SELECTED );
  KEY( "marked",          GUI::MODEL_INFO::MARKED );
  KEY( "vars",            GUI::MODEL_INFO::VARS );
  KEY( "vars_cont",       GUI::MODEL_INFO::VARSCONT );
  KEY( "regular",         GUI::MODEL_INFO::REGULAR );

  GROUP( "gui/model_info/model" );

  KEY( "title",           GUI::MODEL_INFO::TITLE );
  KEY( "name",            GUI::MODEL_INFO::MODEL::NAME );
  KEY( "filename",        GUI::MODEL_INFO::MODEL::FILENAME );
  KEY( "creation_time",   GUI::MODEL_INFO::MODEL::CREATION_TIME );
  KEY( "template_file",   GUI::MODEL_INFO::MODEL::TEMPLATE_FILE );
  KEY( "no_template",     GUI::MODEL_INFO::MODEL::NO_TEMPLATE );
  KEY( "output_mode",     GUI::MODEL_INFO::MODEL::OUTPUT_MODE );
  KEY( "sonnet_mode",     GUI::MODEL_INFO::MODEL::SONNET );
  KEY( "carre_mode",      GUI::MODEL_INFO::MODEL::CARRE );
  KEY( "node1",           GUI::MODEL_INFO::MODEL::NODE_1 );
  KEY( "node2",           GUI::MODEL_INFO::MODEL::NODE_2 );
  KEY( "point1",          GUI::MODEL_INFO::MODEL::POINT_1 );
  KEY( "point2",          GUI::MODEL_INFO::MODEL::POINT_2 );
  KEY( "intarget",        GUI::MODEL_INFO::MODEL::INTARGET );
  KEY( "bad_creatorid",   GUI::MODEL_INFO::MODEL::BAD_CREATORID );
  KEY( "unchanged",       GUI::MODEL_INFO::MODEL::UNCHANGED );
  KEY( "carremode",       GUI::MODEL_INFO::MODEL::CARREMODE );
  KEY( "listof",          GUI::MODEL_INFO::MODEL::LISTOF );
  KEY( "emptylist",       GUI::MODEL_INFO::MODEL::EMPTYLIST );
  KEY( "empty",           GUI::MODEL_INFO::MODEL::EMPTY );
  KEY( "is3d",            GUI::MODEL_INFO::MODEL::IS3D );
  KEY( "isend",           GUI::MODEL_INFO::MODEL::ISEND );
  KEY( "unused_nums",     GUI::MODEL_INFO::MODEL::UNUSED_NUMS );//1408

  GROUP( "gui/model_info/template" );

  KEY( "incr",            GUI::MODEL_INFO::TEMPLATE::INCR );
  KEY( "scale",           GUI::MODEL_INFO::TEMPLATE::SCALE );
  KEY( "angle",           GUI::MODEL_INFO::TEMPLATE::ANGLE );

  GROUP( "gui/model_info/topology" );

  KEY( "title",           GUI::MODEL_INFO::TOPOLOGY::TITLE );
  KEY( "name",            GUI::MODEL_INFO::TOPOLOGY::NAME );
  KEY( "descr",           GUI::MODEL_INFO::TOPOLOGY::DESCR );
  KEY( "origin",          GUI::MODEL_INFO::TOPOLOGY::ORIGIN );
  KEY( "filename",        GUI::MODEL_INFO::TOPOLOGY::FILENAME );
  KEY( "topology_name",   GUI::MODEL_INFO::TOPOLOGY::TOPOLOGY_NAME );
  KEY( "poloidal",        GUI::MODEL_INFO::TOPOLOGY::POLOIDAL );
  KEY( "toroidal",        GUI::MODEL_INFO::TOPOLOGY::TOROIDAL );
  KEY( "level",           GUI::MODEL_INFO::TOPOLOGY::LEVEL );
  KEY( "verticies",       GUI::MODEL_INFO::TOPOLOGY::VERTICIES );
  KEY( "start",           GUI::MODEL_INFO::TOPOLOGY::START );
  KEY( "end",             GUI::MODEL_INFO::TOPOLOGY::END );
  KEY( "zone",            GUI::MODEL_INFO::TOPOLOGY::ZONE );
  KEY( "isused",          GUI::MODEL_INFO::TOPOLOGY::ISUSED );
  KEY( "dir",             GUI::MODEL_INFO::TOPOLOGY::DIR );
  KEY( "dirtarget",       GUI::MODEL_INFO::TOPOLOGY::DIRTARGET );
  KEY( "crid",            GUI::MODEL_INFO::TOPOLOGY::CRID );
  KEY( "isdrawn",         GUI::MODEL_INFO::TOPOLOGY::ISDRAWN );
  KEY( "isok",            GUI::MODEL_INFO::TOPOLOGY::ISOK );
  KEY( "gpzones",         GUI::MODEL_INFO::TOPOLOGY::GPZONES );
  KEY( "zonesign",        GUI::MODEL_INFO::TOPOLOGY::ZONESIGN );
  KEY( "orient",          GUI::MODEL_INFO::TOPOLOGY::ORIENT );
  KEY( "limited",         GUI::MODEL_INFO::TOPOLOGY::LIMITED );
  KEY( "isclosed",        GUI::MODEL_INFO::TOPOLOGY::ISCLOSED );
  KEY( "derivative",      GUI::MODEL_INFO::TOPOLOGY::DERIVATIVE );
  KEY( "virtual",         GUI::MODEL_INFO::TOPOLOGY::VIRTUAL );
  KEY( "innermost",       GUI::MODEL_INFO::TOPOLOGY::INNERMOST );

  GROUP( "gui/model_info/equil" );

  KEY( "title",           GUI::MODEL_INFO::EQUIL::TITLE );
  KEY( "filename",        GUI::MODEL_INFO::EQUIL::FILENAME );
  KEY( "size",            GUI::MODEL_INFO::EQUIL::SIZE );
  KEY( "value",           GUI::MODEL_INFO::EQUIL::VALUE );

  GROUP( "gui/model_info/mesh" );

  KEY( "title",           GUI::MODEL_INFO::MESH::TITLE );
  KEY( "filename",        GUI::MODEL_INFO::MESH::FILENAME );
  KEY( "header",          GUI::MODEL_INFO::MESH::HEADER );
  KEY( "size",            GUI::MODEL_INFO::MESH::SIZE );
  KEY( "sliding_mode",    GUI::MODEL_INFO::MESH::SLIDING_MODE );
  KEY( "sliding_treshold",GUI::MODEL_INFO::MESH::SLIDING_THRSHLD );
  KEY( "double_border",   GUI::MODEL_INFO::MESH::DOUBLE_BORDER );
  KEY( "fingerprint",     GUI::MODEL_INFO::MESH::FINGERPRINT );
  KEY( "by_surfaces",     GUI::MODEL_INFO::MESH::BY_SURFACES );
  KEY( "by_splines",      GUI::MODEL_INFO::MESH::BY_SPLINES );
  KEY( "index",           GUI::MODEL_INFO::MESH::INDEX );
  KEY( "backup",          GUI::MODEL_INFO::MESH::BACKUP );
  KEY( "changed",         GUI::MODEL_INFO::MESH::CHANGED );
  KEY( "cutflag",         GUI::MODEL_INFO::MESH::CUTFLAG );
  KEY( "border",          GUI::MODEL_INFO::MESH::BORDER );
  KEY( "neighbours",      GUI::MODEL_INFO::MESH::NEIGHBOURS );
  KEY( "outermost",       GUI::MODEL_INFO::MESH::OUTERMOST );
  KEY( "number",          GUI::MODEL_INFO::MESH::NUMBER );
  KEY( "center",          GUI::MODEL_INFO::MESH::CENTER );
  KEY( "ratio",           GUI::MODEL_INFO::MESH::RATIO );
  KEY( "vertical",        GUI::MODEL_INFO::MESH::VERTICAL );

  GROUP( "gui/model_info/vars" )

  KEY( "name",            GUI::MODEL_INFO::VARS::NAME );
  KEY( "descr",           GUI::MODEL_INFO::VARS::DESCR );
  KEY( "help",            GUI::MODEL_INFO::VARS::HELP );
  KEY( "enumvalues",      GUI::MODEL_INFO::VARS::ENUMVALUES );
  KEY( "minmaxsets",      GUI::MODEL_INFO::VARS::MINMAXSETS );
  KEY( "origin",          GUI::MODEL_INFO::VARS::ORIGIN );
  KEY( "vartype",         GUI::MODEL_INFO::VARS::VARTYPE );
  KEY( "rowcolumn",       GUI::MODEL_INFO::VARS::ROWCOLUMN );
  KEY( "multiple",        GUI::MODEL_INFO::VARS::MULTIPLE );
  KEY( "hasgroup",        GUI::MODEL_INFO::VARS::HASGROUP );
  KEY( "flags",           GUI::MODEL_INFO::VARS::FLAGS );
  KEY( "typeflags",       GUI::MODEL_INFO::VARS::TYPEFLAGS );

  GROUP( "gui/model_info/vars/vdflags" );

  KEY( "noexport",        GUI::MODEL_INFO::VARS::VDFLAGS::NOEXPORT );
  KEY( "forelems",        GUI::MODEL_INFO::VARS::VDFLAGS::FORELEMS );
  KEY( "forseparators",   GUI::MODEL_INFO::VARS::VDFLAGS::FORSEPARATORS );
  KEY( "forsources",      GUI::MODEL_INFO::VARS::VDFLAGS::FORSOURCES );
  KEY( "forchords",       GUI::MODEL_INFO::VARS::VDFLAGS::FORCHORDS );
  KEY( "layerindex",      GUI::MODEL_INFO::VARS::VDFLAGS::LAYERINDEX );

  GROUP( "gui/model_info/vars/typeflags" );

  KEY( "haselems",        GUI::MODEL_INFO::VARS::TYPEFLAGS::HASELEMS );
  KEY( "lockgroup",       GUI::MODEL_INFO::VARS::TYPEFLAGS::LOCKGROUP );
  KEY( "single",          GUI::MODEL_INFO::VARS::TYPEFLAGS::SINGLE );
  KEY( "structpart",      GUI::MODEL_INFO::VARS::TYPEFLAGS::STRUCTPART );
  KEY( "haschords",       GUI::MODEL_INFO::VARS::TYPEFLAGS::HASCHORDS );
  KEY( "hasmesh",         GUI::MODEL_INFO::VARS::TYPEFLAGS::HASMESH );
  KEY( "target",          GUI::MODEL_INFO::VARS::TYPEFLAGS::TARGET );
  KEY( "filename",        GUI::MODEL_INFO::VARS::TYPEFLAGS::FILENAME );

  GROUP( "gui/model_info/vars/types" );

  KEY( "int",             GUI::MODEL_INFO::VARS::TYPES::INT );
  KEY( "float",           GUI::MODEL_INFO::VARS::TYPES::FLOAT );
  KEY( "text",            GUI::MODEL_INFO::VARS::TYPES::TEXT );
  KEY( "filename",        GUI::MODEL_INFO::VARS::TYPES::FILENAME );
  KEY( "element",         GUI::MODEL_INFO::VARS::TYPES::ELEM );
  KEY( "elems",           GUI::MODEL_INFO::VARS::TYPES::ELEMS );
  KEY( "target1",         GUI::MODEL_INFO::VARS::TYPES::TARGET1 );
  KEY( "target2",         GUI::MODEL_INFO::VARS::TYPES::TARGET2 );
  KEY( "structure",       GUI::MODEL_INFO::VARS::TYPES::STRUCTURE );
  KEY( "structpart",      GUI::MODEL_INFO::VARS::TYPES::STRUCTPART );
  KEY( "chords",          GUI::MODEL_INFO::VARS::TYPES::CHORDS );
  KEY( "meshcells",       GUI::MODEL_INFO::VARS::TYPES::MESHCELLS );
  KEY( "meshelems",       GUI::MODEL_INFO::VARS::TYPES::MESHELEMS );
  KEY( "meshhelems",      GUI::MODEL_INFO::VARS::TYPES::MESH_HELEMS );
  KEY( "meshvelems",      GUI::MODEL_INFO::VARS::TYPES::MESH_VELEMS );
  KEY( "topview",         GUI::MODEL_INFO::VARS::TYPES::TOPVIEW );

  GROUP( "gui/model_info/type" );

  KEY( "element",         GUI::MODEL_INFO::TYPE::ELEMENT );
  KEY( "separator",       GUI::MODEL_INFO::TYPE::SEPARATOR );
  KEY( "source",          GUI::MODEL_INFO::TYPE::SOURCE );
  KEY( "chord",           GUI::MODEL_INFO::TYPE::CHORD );
  KEY( "node",            GUI::MODEL_INFO::TYPE::NODE );
  KEY( "meshpoint",       GUI::MODEL_INFO::TYPE::MESHPOINT );
  KEY( "meshcell",        GUI::MODEL_INFO::TYPE::MESHCELL );
  KEY( "meshelement",     GUI::MODEL_INFO::TYPE::MESHELEMENT );
  KEY( "xpointtest",      GUI::MODEL_INFO::TYPE::XPOINTTEST );
  KEY( "xpointseg",       GUI::MODEL_INFO::TYPE::XPOINTSEG );
  KEY( "surfacezone",     GUI::MODEL_INFO::TYPE::SURFACEZONE );
  KEY( "surfaceex",       GUI::MODEL_INFO::TYPE::SURFACEEX );
  KEY( "gridpointseg",    GUI::MODEL_INFO::TYPE::GRIDPOINTSEG );
  KEY( "gridpointex",     GUI::MODEL_INFO::TYPE::GRIDPOINTEX );
  KEY( "unknown",         GUI::MODEL_INFO::TYPE::UNKNOWN );
  KEY( "container",       GUI::MODEL_INFO::TYPE::CONTAINER );
  KEY( "var",             GUI::MODEL_INFO::TYPE::VAR );
  KEY( "varsetdef",       GUI::MODEL_INFO::TYPE::VARSETDEF );
  KEY( "vardef",          GUI::MODEL_INFO::TYPE::VARDEF );
  KEY( "varset",          GUI::MODEL_INFO::TYPE::VARSET );
  KEY( "template",        GUI::MODEL_INFO::TYPE::TEMPLATE );

  KEY( "model",           GUI::MODEL_INFO::TYPE::MODEL );
  KEY( "mesh",            GUI::MODEL_INFO::TYPE::MESH );
  KEY( "topology",        GUI::MODEL_INFO::TYPE::TOPOLOGY );
  KEY( "equil",           GUI::MODEL_INFO::TYPE::EQUIL );

  GROUP( "gui/model_info/actions" );

  KEY( "undostack",       GUI::MODEL_INFO::ACTIONS::UNDOSTACK );
  KEY( "redostack",       GUI::MODEL_INFO::ACTIONS::REDOSTACK );

  KEY( "change_value",    GUI::MODEL_INFO::ACTIONS::CHANGE_VALUE );
  KEY( "change_group",    GUI::MODEL_INFO::ACTIONS::CHANGE_OBJGROUP );
  KEY( "mark_object",     GUI::MODEL_INFO::ACTIONS::MARK_OBJECT );
  KEY( "highlight_object",GUI::MODEL_INFO::ACTIONS::HIGHLIGHT_OBJECT );
  KEY( "add_object",      GUI::MODEL_INFO::ACTIONS::ADD_X );
  KEY( "del_object",      GUI::MODEL_INFO::ACTIONS::DEL_X );
  KEY( "change_object",   GUI::MODEL_INFO::ACTIONS::CHG_X );

  KEY( "parameter",       GUI::MODEL_INFO::ACTIONS::PARAMETER );
  KEY( "value",           GUI::MODEL_INFO::ACTIONS::VALUE );
  KEY( "action",          GUI::MODEL_INFO::ACTIONS::ACTION );

  GROUP( "gui/view" );

  KEY( "scene",           GUI::VIEW::SCENE );
  KEY( "center",          GUI::VIEW::CENTER );
  KEY( "scale",           GUI::VIEW::SCALE );
  KEY( "angle",           GUI::VIEW::ANGLE );
  KEY( "add",             GUI::VIEW::ADD );
}
