#include "StringsManager.h"

void StringsManager::DialogsKeys()
{
  INIT_TREE;

  GROUP( "dlg/btn" );
  KEY( "ok",                  DLG::BTN::OK );
  KEY( "create",              DLG::BTN::CREATE );
  KEY( "save",                DLG::BTN::SAVE );
  KEY( "load",                DLG::BTN::LOAD );
  KEY( "import",              DLG::BTN::IMPORT );
  KEY( "export",              DLG::BTN::EXPORT );
  KEY( "yes",                 DLG::BTN::YES );
  KEY( "proceed",             DLG::BTN::PROCEED );
  KEY( "apply",               DLG::BTN::APPLY );
  KEY( "cancel",              DLG::BTN::CANCEL );
  KEY( "close",               DLG::BTN::CLOSE );
  KEY( "discard",             DLG::BTN::DISCARD );
  KEY( "no",                  DLG::BTN::NO );
  KEY( "help",                DLG::BTN::HELP );
  KEY( "browse",              DLG::BTN::BROWSE );

  GROUP( "dlg/msg" );
  KEY( "normals_grouped",     DLG::MSG::NORMALS_GROUPED );

  /****************************************************************************
   *  COMMON
   ****************************************************************************/

  GROUP( "dlg/model_close" );
  KEY( "noviews",         DLG::MODEL_CLOSE::NOVIEWS );
  KEY( "unsaved",         DLG::MODEL_CLOSE::UNSAVED );
  KEY( "dontclose",       DLG::MODEL_CLOSE::DONOTCLOSE );
  KEY( "saveandclose",    DLG::MODEL_CLOSE::SAVEANDCLOSE );
  KEY( "closeunsaved",    DLG::MODEL_CLOSE::CLOSEUNSAVED );

  GROUP( "dlg/file_overwrite" );
  KEY( "title",           DLG::FILE_OVERWRITE::TITLE );
  KEY( "overwrite",       DLG::FILE_OVERWRITE::OVERWRITE );
  KEY( "selectanother",   DLG::FILE_OVERWRITE::SELECTANOTHER );

  GROUP( "dlg/model_open" );
  KEY( "title",           DLG::MODEL_OPEN::TITLE );
  KEY( "message",         DLG::MODEL_OPEN::MESSAGE );
  KEY( "same_name",       DLG::MODEL_OPEN::SAME_NAME );

  GROUP( "dlg/session" );
  KEY( "title",           DLG::SESSION::TITLE );
  KEY( "message",         DLG::SESSION::MESSAGE );

  GROUP( "dlg/new_model" );
  KEY( "title",           DLG::NEW_MODEL::TITLE );
  KEY( "no_config",       DLG::NEW_MODEL::NO_CONFIG );
  KEY( "use_config",      DLG::NEW_MODEL::USE_CONFIG );

  /****************************************************************************
   *   MENU > FILE
   ****************************************************************************/

  GROUP( "dlg/equil" );
  KEY( "title",               DLG::EQUIL::TITLE );
  KEY( "filter",              DLG::EQUIL::FILTER );
  KEY( "help",                DLG::EQUIL::HELP );

  GROUP( "dlg/template" );
  KEY( "title",               DLG::TEMPLATE::TITLE );
  KEY( "filter",              DLG::TEMPLATE::FILTER );
  KEY( "help",                DLG::TEMPLATE::HELP );

  GROUP( "dlg/sonnet" );
  KEY( "title",               DLG::SONNET::TITLE );
  KEY( "filter",              DLG::SONNET::FILTER );
  KEY( "help",                DLG::SONNET::HELP );

  GROUP( "dlg/file_open" );
  KEY( "title",               DLG::FILEOPEN::TITLE );
  KEY( "filter",              DLG::FILEOPEN::FILTER );
  KEY( "help",                DLG::FILEOPEN::HELP );

  GROUP( "dlg/file_save" );
  KEY( "title",               DLG::FILESAVE::TITLE );
  KEY( "filter",              DLG::FILESAVE::FILTER );
  KEY( "help",                DLG::FILESAVE::HELP );

  GROUP( "dlg/export_mesh" );
  KEY( "title",               DLG::EXPORT_MESH::TITLE );
  KEY( "filter",              DLG::EXPORT_MESH::FILTER );
  KEY( "help",                DLG::EXPORT_MESH::HELP );
  KEY( "message",             DLG::EXPORT_MESH::MESSAGE );

  GROUP( "dlg/export_elems_as_template" );
  KEY( "title",               DLG::EXPORT_ELEMS_TEMPLATE::TITLE );
  KEY( "filter",              DLG::EXPORT_ELEMS_TEMPLATE::FILTER );
  KEY( "message",             DLG::EXPORT_ELEMS_TEMPLATE::MESSAGE );
  KEY( "help",                DLG::EXPORT_ELEMS_TEMPLATE::HELP );
  KEY( "label.all",           DLG::EXPORT_ELEMS_TEMPLATE::LBL::ALL );
  KEY( "label.marked",        DLG::EXPORT_ELEMS_TEMPLATE::LBL::MARKED );

  GROUP( "dlg/import_topo" );
  KEY( "title",                 DLG::IMPORTTOPO::TITLE );
  KEY( "filter",                DLG::IMPORTTOPO::FILTER );
  KEY( "message",               DLG::IMPORTTOPO::MESSAGE );
  KEY( "help",                  DLG::IMPORTTOPO::HELP );
  KEY( "label.detect_xpoints",  DLG::IMPORTTOPO::LBL::DETECT_XPNT );

  GROUP( "dlg/app_unsaved" );
  KEY( "title",               DLG::APP_UNSAVED::TITLE );
  KEY( "message",             DLG::APP_UNSAVED::MESSAGE );
  KEY( "help",                DLG::APP_UNSAVED::HELP );

  GROUP( "dlg/mesh_unsaved" );
  KEY( "title",               DLG::MESH_UNSAVED::TITLE );
  KEY( "message",             DLG::MESH_UNSAVED::MESSAGE );
  KEY( "help",                DLG::MESH_UNSAVED::HELP );

  GROUP( "dlg/print" );
  KEY( "title",               DLG::PRINT::TITLE );
  KEY( "help",                DLG::PRINT::HELP );
  KEY( "btn.print",           DLG::PRINT::BTN::PRINT );
  GROUP( "dlg/print/label" );
  KEY( "command",             DLG::PRINT::LBL::COMMAND );
  KEY( "file",                DLG::PRINT::LBL::FILE );
  KEY( "width",               DLG::PRINT::LBL::WIDTH );
  KEY( "height",              DLG::PRINT::LBL::HEIGHT );
  KEY( "margin",              DLG::PRINT::LBL::MARGIN );
  KEY( "fit",                 DLG::PRINT::LBL::FIT );

  /****************************************************************************
   *   MENU > EDIT
   ****************************************************************************/

  GROUP( "dlg/create_node" );
  KEY( "title",               DLG::CREATE_NODE::TITLE );
  KEY( "help",                DLG::CREATE_NODE::HELP );
  GROUP( "dlg/create_node/label" );
  KEY( "x",                   DLG::CREATE_NODE::LBL::X );
  KEY( "y",                   DLG::CREATE_NODE::LBL::Y );

  GROUP( "dlg/create_source" );
  KEY( "title",               DLG::CREATE_SOURCE::TITLE );
  KEY( "help",                DLG::CREATE_SOURCE::HELP );
  GROUP( "dlg/create_node/label" );
  KEY( "x",                   DLG::CREATE_SOURCE::LBL::X );
  KEY( "y",                   DLG::CREATE_SOURCE::LBL::Y );

  GROUP( "dlg/create_surface" );
  KEY( "title",               DLG::CREATE_SURFACE::TITLE );
  KEY( "help",                DLG::CREATE_SURFACE::HELP );
  KEY( "multiple.title",      DLG::CREATE_SURFACE::MULTIPLE::TITLE );
  KEY( "single.title",        DLG::CREATE_SURFACE::SINGLE::TITLE );
  GROUP( "dlg/create_surface/multiple/label" );
  KEY( "level1",              DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL1 );
  KEY( "level2",              DLG::CREATE_SURFACE::MULTIPLE::LBL::LEVEL2 );
  KEY( "remove_old",          DLG::CREATE_SURFACE::MULTIPLE::LBL::REMOVE_OLD );
  GROUP( "dlg/create_surface/multiple/btn" );
  KEY( "copy",                DLG::CREATE_SURFACE::MULTIPLE::BTN::COPY );
  KEY( "mark_bound_elem",     DLG::CREATE_SURFACE::MULTIPLE::BTN::MARK_BOUND_ELEM );
  GROUP( "dlg/create_surface/single" );
  KEY( "by_level",            DLG::CREATE_SURFACE::SINGLE::BY_LEVEL );
  KEY( "by_point",            DLG::CREATE_SURFACE::SINGLE::BY_POINT );
  KEY( "level",               DLG::CREATE_SURFACE::SINGLE::LEVEL );
  KEY( "x",                   DLG::CREATE_SURFACE::SINGLE::X );
  KEY( "y",                   DLG::CREATE_SURFACE::SINGLE::Y );
  GROUP( "dlg/create_surface/area" );
  KEY( "label",               DLG::CREATE_SURFACE::AREA::TITLE );
  KEY( "area1",               DLG::CREATE_SURFACE::AREA::AREA1 );
  KEY( "area2",               DLG::CREATE_SURFACE::AREA::AREA2 );
  KEY( "area3",               DLG::CREATE_SURFACE::AREA::AREA3 );
  GROUP( "dlg/create_surface/err" );
  KEY( "no_remembered_surf",  DLG::CREATE_SURFACE::ERR::NO_REMEMBERED_SURFACE );
  KEY( "bad_creator_id",      DLG::CREATE_SURFACE::ERR::BAD_CREATOR_ID );
  KEY( "equal_levels",        DLG::CREATE_SURFACE::ERR::EQUAL_LEVELS );
  KEY( "old_surf_not_found",  DLG::CREATE_SURFACE::ERR::OLD_SURFACES_NOT_FOUND );
  KEY( "carre_old_surf",      DLG::CREATE_SURFACE::ERR::CARRE_OLD_SURFACES );
  KEY( "non_carre_mode",      DLG::CREATE_SURFACE::ERR::NON_CARRE_MODE );

  GROUP( "dlg/create_gpoint" );
  KEY( "title",               DLG::CREATE_GPOINT::TITLE );
  KEY( "help",                DLG::CREATE_GPOINT::HELP );
  KEY( "multiple.title",      DLG::CREATE_GPOINT::MULTIPLE::TITLE );
  KEY( "single.title",        DLG::CREATE_GPOINT::SINGLE::TITLE );
  GROUP( "dlg/create_gpoint/single" );
  KEY( "value",               DLG::CREATE_GPOINT::SINGLE::VALUE );
  GROUP( "dlg/create_gpoint/area" );
  KEY( "label",               DLG::CREATE_GPOINT::AREA::TITLE );
  KEY( "area0",               DLG::CREATE_GPOINT::AREA::AREA0 );
  KEY( "area1",               DLG::CREATE_GPOINT::AREA::AREA1 );
  KEY( "area2",               DLG::CREATE_GPOINT::AREA::AREA2 );
  GROUP( "dlg/create_gpoint/err" );
  KEY( "no_remembered_gpoint",DLG::CREATE_GPOINT::ERR::NO_REMEMBERED_GRIDPOINT );
  KEY( "bad_creator_id",      DLG::CREATE_GPOINT::ERR::BAD_CREATOR_ID );
  KEY( "non_carre_mode",      DLG::CREATE_GPOINT::ERR::NON_CARRE_MODE );

  GROUP( "dlg/distr_graph/label" );
  KEY( "count",               DLG::DISTR_GRAPH::LBL::COUNT );
  KEY( "alpha1",              DLG::DISTR_GRAPH::LBL::A1 );
  KEY( "alpha2",              DLG::DISTR_GRAPH::LBL::A2 );
  KEY( "delta1",              DLG::DISTR_GRAPH::LBL::D1 );
  KEY( "delta2",              DLG::DISTR_GRAPH::LBL::D2 );
  GROUP( "dlg/distr_graph/btn" );
  KEY( "copy",                DLG::DISTR_GRAPH::BTN::COPY );
  KEY( "reset",               DLG::DISTR_GRAPH::BTN::RESET );
  GROUP( "dlg/distr_graph/law" );
  KEY( "label",               DLG::DISTR_GRAPH::LAW::TITLE );
  KEY( "normal",              DLG::DISTR_GRAPH::LAW::NORM );
  KEY( "flipped",             DLG::DISTR_GRAPH::LAW::FLIP );
  KEY( "symmetric",           DLG::DISTR_GRAPH::LAW::SYMM );
  KEY( "twoalpha",            DLG::DISTR_GRAPH::LAW::TWOA );
  KEY( "delta",               DLG::DISTR_GRAPH::LAW::DELTA );
  GROUP( "dlg/distr_graph/err" );
  KEY( "count_bad_range",     DLG::DISTR_GRAPH::ERR::COUNT_BAD_RANGE );
  KEY( "alpha1_bad_range",    DLG::DISTR_GRAPH::ERR::A1_BAD_RANGE );
  KEY( "alpha2_bad_range",    DLG::DISTR_GRAPH::ERR::A2_BAD_RANGE );
  KEY( "bad_law",             DLG::DISTR_GRAPH::ERR::BAD_LAW );
  KEY( "wrong_carre_law",     DLG::DISTR_GRAPH::ERR::WRONG_CARRE_LAW );

  GROUP( "dlg/create_chord" );
  KEY( "title",               DLG::CREATE_CHORD::TITLE );
  KEY( "help",                DLG::CREATE_CHORD::HELP );
  GROUP( "dlg/create_chord/cartesian" );
  KEY( "label",               DLG::CREATE_CHORD::CARTESIAN::TITLE );
  KEY( "x1",                  DLG::CREATE_CHORD::CARTESIAN::X1 );
  KEY( "y1",                  DLG::CREATE_CHORD::CARTESIAN::Y2 );
  KEY( "z1",                  DLG::CREATE_CHORD::CARTESIAN::Z1 );
  KEY( "x2",                  DLG::CREATE_CHORD::CARTESIAN::X2 );
  KEY( "y2",                  DLG::CREATE_CHORD::CARTESIAN::Y2 );
  KEY( "z2",                  DLG::CREATE_CHORD::CARTESIAN::Z2 );
  GROUP( "dlg/create_chord/cylindrical" );
  KEY( "lable",               DLG::CREATE_CHORD::CYLINDRICAL::TITLE );
  KEY( "r1",                  DLG::CREATE_CHORD::CYLINDRICAL::R1 );
  KEY( "z1",                  DLG::CREATE_CHORD::CYLINDRICAL::Z1 );
  KEY( "phi1",                DLG::CREATE_CHORD::CYLINDRICAL::PHI1 );
  KEY( "r2",                  DLG::CREATE_CHORD::CYLINDRICAL::R2 );
  KEY( "z2",                  DLG::CREATE_CHORD::CYLINDRICAL::Z2 );
  KEY( "phi2",                DLG::CREATE_CHORD::CYLINDRICAL::PHI2 );

  /****************************************************************************
   *   MENU > VIEW
   ****************************************************************************/

  GROUP( "dlg/rot_move" );
  KEY( "title",               DLG::ROTMOVE::TITLE );
  KEY( "help",                DLG::ROTMOVE::HELP );
  KEY( "err.bad_scale",       DLG::ROTMOVE::ERR::BAD_SCALE );
  GROUP( "dlg/rot_move/label" );
  KEY( "x",                   DLG::ROTMOVE::LBL::X );
  KEY( "y",                   DLG::ROTMOVE::LBL::Y );
  KEY( "scale",               DLG::ROTMOVE::LBL::SCALE );
  KEY( "elems",               DLG::ROTMOVE::LBL::ELEMS );
  KEY( "template",            DLG::ROTMOVE::LBL::TEMPLATE );
  GROUP( "dlg/rot_move/btn" );
  KEY( "move",                DLG::ROTMOVE::BTN::MOVE );
  KEY( "rotate",              DLG::ROTMOVE::BTN::ROTATE );

  GROUP( "dlg/stretch" );
  KEY( "title",               DLG::STRETCH::TITLE );
  KEY( "btn.reset",           DLG::STRETCH::BTN::RESET );
  GROUP( "dlg/stretch/label" );
  KEY( "x",                   DLG::STRETCH::LBL::X );
  KEY( "y",                   DLG::STRETCH::LBL::Y );

  GROUP( "dlg/set_angle" );
  KEY( "title",               DLG::SETANGLE::TITLE );
  KEY( "label.angle",         DLG::SETANGLE::LBL::ANGLE );

  GROUP( "dlg/mesh_header" );
  KEY( "title",               DLG::EDIT_MESH_HEADER::TITLE );
  KEY( "help",                DLG::EDIT_MESH_HEADER::HELP );
  KEY( "btn.change",          DLG::EDIT_MESH_HEADER::BTN::CHANGE );

  /****************************************************************************
   *   MENU > COMMANDS
   ****************************************************************************/

  GROUP( "dlg/glue_points" );
  KEY( "title",               DLG::GLUEPOINTS::TITLE );
  KEY( "help",                DLG::GLUEPOINTS::HELP );
  KEY( "msg_elems_added",     DLG::GLUEPOINTS::MSG::ELEMS_ADDED );
  KEY( "err.bad_max_dist",    DLG::GLUEPOINTS::ERR::BAD_MAX_DIST );
  GROUP( "dlg/glue_points/label" );
  KEY( "max_dist",            DLG::GLUEPOINTS::LBL::MAX_DIST );
  KEY( "millimeters",         DLG::GLUEPOINTS::LBL::MILLIMETERS );
  KEY( "marked_only",         DLG::GLUEPOINTS::LBL::MARKED_ONLY );

  GROUP( "dlg/glue_elems" );
  KEY( "title",               DLG::GLUEELEMS::TITLE );
  KEY( "help",                DLG::GLUEELEMS::HELP );
  KEY( "err.bad_max_dist",    DLG::GLUEELEMS::ERR::BAD_MAX_DIST );
  KEY( "err.bad_max_len",     DLG::GLUEELEMS::ERR::BAD_MAX_LEN );
  KEY( "msg_elems_removed",   DLG::GLUEELEMS::MSG::ELEMS_REMOVED );
  GROUP( "dlg/glue_elems/label" );
  KEY( "treshold",            DLG::GLUEELEMS::LBL::TRESHOLD );
  KEY( "millimeters",         DLG::GLUEELEMS::LBL::MILLIMETERS );
  KEY( "marked_only",         DLG::GLUEELEMS::LBL::MARKED_ONLY );
  KEY( "max_len",             DLG::GLUEELEMS::LBL::MAX_LEN );
  KEY( "cut_longer",          DLG::GLUEELEMS::LBL::CUT_LONGER );

  GROUP( "dlg/topology" );
  KEY( "title",               DLG::TOPOLOGY::TITLE );
  KEY( "help",                DLG::TOPOLOGY::HELP );
  KEY( "xpoints.title",       DLG::TOPOLOGY::XPOINTS::TITLE );
  KEY( "sections.title",      DLG::TOPOLOGY::SECTIONS::TITLE );
  KEY( "zones.title",         DLG::TOPOLOGY::ZONES::TITLE );
  KEY( "name.title",          DLG::TOPOLOGY::NAME::TITLE );
  GROUP( "dlg/topology/xpoints/label" );
  KEY( "xpt_level",           DLG::TOPOLOGY::XPOINTS::LBL::XPT_LEVEL );
  GROUP( "dlg/topology/xpoints/btn" );
  KEY( "detect_all",          DLG::TOPOLOGY::XPOINTS::BTN::DETECT_ALL );
  KEY( "change",              DLG::TOPOLOGY::XPOINTS::BTN::CHANGE );
  GROUP( "dlg/topology/sections/label" );
  KEY( "title",               DLG::TOPOLOGY::SECTIONS::LBL::TITLE );
  KEY( "id",                  DLG::TOPOLOGY::SECTIONS::LBL::ID );
  KEY( "short",               DLG::TOPOLOGY::SECTIONS::LBL::SHORT );
  KEY( "long",                DLG::TOPOLOGY::SECTIONS::LBL::LONG );
  KEY( "used",                DLG::TOPOLOGY::SECTIONS::LBL::USED );
  KEY( "targetcw",            DLG::TOPOLOGY::SECTIONS::LBL::TARGETCW );
  GROUP( "dlg/topology/sections/btn" );
  KEY( "detect_all",          DLG::TOPOLOGY::SECTIONS::BTN::DETECT_ALL );
  KEY( "change",              DLG::TOPOLOGY::SECTIONS::BTN::CHANGE );
  KEY( "revert",              DLG::TOPOLOGY::SECTIONS::BTN::REVERT );
  GROUP( "dlg/topology/zones/label" );
  KEY( "title",               DLG::TOPOLOGY::ZONES::LBL::TITLE );
  KEY( "id",                  DLG::TOPOLOGY::ZONES::LBL::ID );
  KEY( "short",               DLG::TOPOLOGY::ZONES::LBL::SHORT );
  KEY( "long",                DLG::TOPOLOGY::ZONES::LBL::LONG );
  KEY( "gps1",                DLG::TOPOLOGY::ZONES::LBL::GPS1 );
  KEY( "gps2",                DLG::TOPOLOGY::ZONES::LBL::GPS2 );
  KEY( "orient",              DLG::TOPOLOGY::ZONES::LBL::ORIENT );
  KEY( "left",                DLG::TOPOLOGY::ZONES::LBL::LEFT );
  KEY( "right",               DLG::TOPOLOGY::ZONES::LBL::RIGHT );
  KEY( "closed",              DLG::TOPOLOGY::ZONES::LBL::CLOSED );
  GROUP( "dlg/topology/zones/btn" );
  KEY( "set",                 DLG::TOPOLOGY::ZONES::BTN::SET );
  KEY( "add",                 DLG::TOPOLOGY::ZONES::BTN::ADD );
  KEY( "delete",              DLG::TOPOLOGY::ZONES::BTN::DELETE );
  GROUP( "dlg/topology/name" );
  KEY( "lbl.title",           DLG::TOPOLOGY::NAME::LBL::TITLE );
  KEY( "btn.set",             DLG::TOPOLOGY::NAME::BTN::SET );
  GROUP( "dlg/topology/err" );
  KEY( "cur_object_gone",     DLG::TOPOLOGY::ERR::CUR_OBJ_GONE );
  KEY( "bad_xpt_level",       DLG::TOPOLOGY::ERR::BAD_XPT_LEVEL );
  KEY( "bad_new_level",       DLG::TOPOLOGY::ERR::BAD_NEW_LEVEL );
  KEY( "bad_xps_zone",        DLG::TOPOLOGY::ERR::BAD_XPS_ZONE );
  KEY( "dup_xps_zone",        DLG::TOPOLOGY::ERR::DUP_XPS_ZONE );
  KEY( "bad_szone",           DLG::TOPOLOGY::ERR::BAD_SZONE );
  KEY( "dup_szone",           DLG::TOPOLOGY::ERR::DUP_SZONE );
  KEY( "sz_bad_gp_zone",      DLG::TOPOLOGY::ERR::SZ_BAD_GP_ZONE );
  KEY( "sz_not_found_gp_zone",DLG::TOPOLOGY::ERR::SZ_NOT_FOIND_GP_ZONE );
  KEY( "sz_bad_sign",         DLG::TOPOLOGY::ERR::SZ_BAD_SIGN );
  KEY( "bad_topo_name",       DLG::TOPOLOGY::ERR::BAD_TOPO_NAME );
  GROUP( "dlg/topology/msg" );
  KEY( "xpt_level_adjusted",  DLG::TOPOLOGY::MSG::XPT_LEVEL_ADJUSTED );
  KEY( "xseg_changed",        DLG::TOPOLOGY::MSG::XSEG_CHANGED );
  KEY( "xseg_reverted",       DLG::TOPOLOGY::MSG::XSEG_REVERTED );
  KEY( "sz_changed",          DLG::TOPOLOGY::MSG::SZ_CHANGED );
  KEY( "topo_name_changed",   DLG::TOPOLOGY::MSG::TOPO_NAME_CHANGED );

  /****************************************************************************
   *   MENU > VARIABLES
   ****************************************************************************/

  GROUP( "dlg/vars_edit" );
  KEY( "title",               DLG::VARSEDIT::TITLE );
  KEY( "help",                DLG::VARSEDIT::HELP );
  GROUP( "dlg/vars_edit/label" );
  KEY( "hold",                DLG::VARSEDIT::LBL::HOLD );
  KEY( "message",             DLG::VARSEDIT::LBL::MESSAGE );
  KEY( "old_value",           DLG::VARSEDIT::LBL::OLD_VAL );
  KEY( "new_value",           DLG::VARSEDIT::LBL::NEW_VAL );
  KEY( "obj_match",           DLG::VARSEDIT::LBL::OBJECT_MATCHING );
  GROUP( "dlg/vars_edit/btn" );
  KEY( "set_all",             DLG::VARSEDIT::BTN::SET_ALL );
  KEY( "reset_all",           DLG::VARSEDIT::BTN::RESET_ALL );
  KEY( "close",               DLG::VARSEDIT::BTN::CLOSE );
  KEY( "help",                DLG::VARSEDIT::BTN::HELP );
  KEY( "reset_var",           DLG::VARSEDIT::BTN::MARK );
  KEY( "set",                 DLG::VARSEDIT::BTN::SET );
  KEY( "set_file",            DLG::VARSEDIT::BTN::SET_FILE );
  KEY( "var_help",            DLG::VARSEDIT::BTN::VAR_HELP );
  GROUP( "dlg/vars_edit/err" );
  KEY( "bad_scope",           DLG::VARSEDIT::ERR::BAD_SCOPE );
  KEY( "bad_number",          DLG::VARSEDIT::ERR::BAD_NUMBER );
  GROUP( "dlg/vars_edit/vars_popup" );
  KEY( "reset",               DLG::VARSEDIT::VARS_POPUP::RESET );
  KEY( "reset_help",          DLG::VARSEDIT::VARS_POPUP::RESET_HELP );
  KEY( "compare",             DLG::VARSEDIT::VARS_POPUP::COMPARE );
  KEY( "display",             DLG::VARSEDIT::VARS_POPUP::DISPLAY );
  KEY( "display_help",        DLG::VARSEDIT::VARS_POPUP::DISPLAY_HELP );
  KEY( "values",              DLG::VARSEDIT::VARS_POPUP::VALUES );
  KEY( "values_help",         DLG::VARSEDIT::VARS_POPUP::VALUES_HELP );
  KEY( "help",                DLG::VARSEDIT::VARS_POPUP::HELP );
  KEY( "help_help",           DLG::VARSEDIT::VARS_POPUP::HELP_HELP );
  GROUP( "dlg/vars_edit/compare" );
  KEY( "equal",               DLG::VARSEDIT::COMPARE::EQUAL );
  KEY( "not_equal",           DLG::VARSEDIT::COMPARE::NOT_EQUAL );
  KEY( "gt",                  DLG::VARSEDIT::COMPARE::GT );
  KEY( "ge",                  DLG::VARSEDIT::COMPARE::GE );
  KEY( "lt",                  DLG::VARSEDIT::COMPARE::LT );
  KEY( "le",                  DLG::VARSEDIT::COMPARE::LE );
  KEY( "help",                DLG::VARSEDIT::COMPARE::HELP );

  GROUP( "dlg/vars_file_set" );
  KEY( "title",               DLG::VARSFILESET::TITLE );
  KEY( "filter",              DLG::VARSFILESET::FILTER );
  KEY( "help",                DLG::VARSFILESET::HELP );

  GROUP( "dlg/invalid_vars" );
  KEY( "title",               DLG::INVALIDVARS::TITLE );
  KEY( "help",                DLG::INVALIDVARS::HELP );
  KEY( "btn.show",            DLG::INVALIDVARS::BTN::SHOW );

  /****************************************************************************
   *   MENU > SETUP
   ****************************************************************************/

  GROUP( "dlg/vsd_list" );
  KEY( "title",               DLG::VARSETDEFLIST::TITLE );
  KEY( "help",                DLG::VARSETDEFLIST::HELP );
  KEY( "label.add",           DLG::VARSETDEFLIST::BTN::ADD );
  KEY( "label.modify",        DLG::VARSETDEFLIST::BTN::MODIFY );
  KEY( "label.remove",        DLG::VARSETDEFLIST::BTN::REMOVE );
  KEY( "label.cancel",        DLG::VARSETDEFLIST::BTN::CANCEL );

  GROUP( "dlg/vsd_create" );
  KEY( "title",               DLG::VARSETDEFCREATE::TITLE );
  KEY( "help",                DLG::VARSETDEFCREATE::HELP );
  KEY( "label_name",          DLG::VARSETDEFCREATE::LBL::NAME );

  GROUP( "dlg/vsd_edit" );
  KEY( "help",                DLG::VARSETDEFEDIT::HELP );
  KEY( "title",               DLG::VARSETDEFEDIT::TITLE );
  KEY( "title_ex",            DLG::VARSETDEFEDIT::TITLE_EX );
  KEY( "label.descr",         DLG::VARSETDEFEDIT::LBL::DESCR );
  KEY( "label.min",           DLG::VARSETDEFEDIT::LBL::MIN );
  KEY( "label.max",           DLG::VARSETDEFEDIT::LBL::MAX );
  KEY( "label.empty",         DLG::VARSETDEFEDIT::LBL::EMPTY );

  GROUP( "dlg/vd_create" );
  KEY( "help",                DLG::VARDEFCREATE::HELP );
  KEY( "title",               DLG::VARDEFCREATE::TITLE );
  KEY( "title_ex",            DLG::VARDEFCREATE::TITLE_EX );
  KEY( "label_name",          DLG::VARDEFCREATE::LBL::NAME );


  GROUP( "dlg/vd_edit" );
  KEY( "help",                DLG::VARDEFEDIT::HELP );
  KEY( "title",               DLG::VARDEFEDIT::TITLE );
  KEY( "title_ex",            DLG::VARDEFEDIT::TITLE_EX );
  KEY( "type",                DLG::VARDEFEDIT::TYPE::TITLE );
  KEY( "scope",               DLG::VARDEFEDIT::SCOPE::TITLE );
  GROUP( "dlg/vd_edit/label" );
  KEY( "descr",               DLG::VARDEFEDIT::LBL::DESCR );
  KEY( "no_export",           DLG::VARDEFEDIT::LBL::NO_EXPORT );
  KEY( "layer",               DLG::VARDEFEDIT::LBL::LAYER );
  KEY( "default_value",       DLG::VARDEFEDIT::LBL::DEFAULT );
  GROUP( "dlg/vd_edit/type" );
  KEY( "int",                 DLG::VARDEFEDIT::TYPE::INT );
  KEY( "float",               DLG::VARDEFEDIT::TYPE::FLOAT );
  KEY( "text",                DLG::VARDEFEDIT::TYPE::TEXT );
  KEY( "filename",            DLG::VARDEFEDIT::TYPE::FILENAME );
  KEY( "elem",                DLG::VARDEFEDIT::TYPE::ELEMENT );
  KEY( "elemset",             DLG::VARDEFEDIT::TYPE::ELEMSET );
  KEY( "target1",             DLG::VARDEFEDIT::TYPE::TARGET1 );
  KEY( "target2",             DLG::VARDEFEDIT::TYPE::TARGET2 );
  KEY( "structure",           DLG::VARDEFEDIT::TYPE::STRUCTURE );
  KEY( "struct_part",         DLG::VARDEFEDIT::TYPE::STRUCTPART );
  KEY( "chords",              DLG::VARDEFEDIT::TYPE::CHORDS );
  KEY( "top_view",            DLG::VARDEFEDIT::TYPE::TOPVIEW );
  KEY( "mesh_cells",          DLG::VARDEFEDIT::TYPE::MESH_CELLS );
  KEY( "mesh_elems",          DLG::VARDEFEDIT::TYPE::MESH_ELEMS );
  KEY( "mesh_h_elems",        DLG::VARDEFEDIT::TYPE::MESH_HELEMS );
  KEY( "mesh_v_elems",        DLG::VARDEFEDIT::TYPE::MESH_VELEMS );
  GROUP( "dlg/vd_edit/scope" );
  KEY( "with_varset",         DLG::VARDEFEDIT::SCOPE::WITH_VARSET );
  KEY( "with_elems",          DLG::VARDEFEDIT::SCOPE::WITH_ELEMS );
  KEY( "with_source",         DLG::VARDEFEDIT::SCOPE::WITH_SOURCES );
  KEY( "with_chords",         DLG::VARDEFEDIT::SCOPE::WITH_CHORDS );
  GROUP( "dlg/vd_edit/btn" );
  KEY( "edit_help",           DLG::VARDEFEDIT::BTN::EDIT_HELP );
  KEY( "remove",              DLG::VARDEFEDIT::BTN::REMOVE );
  GROUP( "dlg/vd_edit/err" );
  KEY( "index_2nd",           DLG::VARDEFEDIT::ERR::SECOND );
  KEY( "index_multiple",      DLG::VARDEFEDIT::ERR::MULTIPLE );
  KEY( "index_group",         DLG::VARDEFEDIT::ERR::GROUP );

  GROUP( "dlg/setup_warning" );
  KEY( "title",               DLG::SETUPWARNING::TITLE );
  KEY( "str_message",         DLG::SETUPWARNING::MESSAGE );
  KEY( "help",                DLG::SETUPWARNING::HELP );

  GROUP( "dlg/save_config" );
  KEY( "title",               DLG::SAVECONFIG::TITLE );
  KEY( "str_message",         DLG::SAVECONFIG::MESSAGE );
  KEY( "help",                DLG::SAVECONFIG::HELP );

  GROUP( "dlg/help_edit" );
  KEY( "help",                DLG::HELPEDIT::HELP );
  KEY( "title",               DLG::HELPEDIT::TITLE );
  KEY( "title_ex",            DLG::HELPEDIT::TITLE_EX );

  GROUP( "dlg/output_mode" );
  KEY( "help",                DLG::OUTPUTMODE::HELP );
  KEY( "title",               DLG::OUTPUTMODE::TITLE );
  GROUP( "dlg/output_mode/label" );
  KEY( "sonnet",              DLG::OUTPUTMODE::LBL::SONNET );
  KEY( "carre",               DLG::OUTPUTMODE::LBL::CARRE );
  KEY( "vars",                DLG::OUTPUTMODE::LBL::VARS );
  KEY( "structure",           DLG::OUTPUTMODE::LBL::STRUCTURE );
  KEY( "targets",             DLG::OUTPUTMODE::LBL::TARGETS );
  KEY( "surfaces",            DLG::OUTPUTMODE::LBL::SURFACES );
  KEY( "gridpoints",          DLG::OUTPUTMODE::LBL::GRIDPOINTS );
  KEY( "cells",               DLG::OUTPUTMODE::LBL::CELLS );
  KEY( "flags",               DLG::OUTPUTMODE::LBL::FLAGS );

  GROUP( "dlg/settings" );
  KEY( "title",               DLG::SETTINGS::TITLE );
  GROUP( "dlg/settings/common" );
  KEY( "title",               DLG::SETTINGS::COMMON::TITLE );
  KEY( "saveinterval",        DLG::SETTINGS::COMMON::SAVEINTERVAL );
  KEY( "recent_length",       DLG::SETTINGS::COMMON::RECENT_LENGTH );
  KEY( "axislabels",          DLG::SETTINGS::COMMON::AXISLABELS );
  GROUP( "dlg/settings/visual" );
  KEY( "title",               DLG::SETTINGS::VISUAL::TITLE );
  KEY( "color",               DLG::SETTINGS::VISUAL::COLOR );
  KEY( "width",               DLG::SETTINGS::VISUAL::WIDTH );
  KEY( "radius",              DLG::SETTINGS::VISUAL::RADIUS );
  KEY( "fontsize",            DLG::SETTINGS::VISUAL::FONTSIZE );
  KEY( "offset",              DLG::SETTINGS::VISUAL::OFFSET );
  KEY( "x",                   DLG::SETTINGS::VISUAL::X );
  KEY( "y",                   DLG::SETTINGS::VISUAL::Y );
  KEY( "background",          DLG::SETTINGS::VISUAL::BACKGROUND );
  KEY( "label",               DLG::SETTINGS::VISUAL::LABEL );
  KEY( "tool",                DLG::SETTINGS::VISUAL::TOOL );
  KEY( "error",               DLG::SETTINGS::VISUAL::ERROR );
  KEY( "sonnetdata",          DLG::SETTINGS::VISUAL::SONNETDATA );
  KEY( "separator",           DLG::SETTINGS::VISUAL::SEPARATOR );
  KEY( "source",              DLG::SETTINGS::VISUAL::SOURCE );
  KEY( "chord",               DLG::SETTINGS::VISUAL::CHORD );
  KEY( "template",            DLG::SETTINGS::VISUAL::TEMPLATE );
  KEY( "surface",             DLG::SETTINGS::VISUAL::SURFACE );
  KEY( "xpointseg",           DLG::SETTINGS::VISUAL::XPOINTSEG );
  KEY( "arrow_size",          DLG::SETTINGS::VISUAL::ARROW_SIZE );
  KEY( "use_squares",         DLG::SETTINGS::VISUAL::USESQUARES );
  KEY( "comment",             DLG::SETTINGS::VISUAL::COMMENT );
  GROUP( "dlg/settings/visual/text_normal" );
  KEY( "title",               DLG::SETTINGS::VISUAL::TEXT_NORM::TITLE );
  KEY( "text",                DLG::SETTINGS::VISUAL::TEXT_NORM::TEXT );
  KEY( "normal",              DLG::SETTINGS::VISUAL::TEXT_NORM::NORMAL );
  KEY( "normal_len",          DLG::SETTINGS::VISUAL::TEXT_NORM::NORM_LEN );
  GROUP( "dlg/settings/visual/mark_hl" );
  KEY( "title",                DLG::SETTINGS::VISUAL::MARK_HL::TITLE );
  KEY( "mark",                DLG::SETTINGS::VISUAL::MARK_HL::MARK );
  KEY( "highlight",           DLG::SETTINGS::VISUAL::MARK_HL::HL );
  GROUP( "dlg/settings/visual/xpointtest" );
  KEY( "title",               DLG::SETTINGS::VISUAL::XPOINTTEST::TITLE );
  KEY( "xpointtest",          DLG::SETTINGS::VISUAL::XPOINTTEST::XPT );
  KEY( "xpt_bound",           DLG::SETTINGS::VISUAL::XPOINTTEST::XPT_BOUND );
  GROUP( "dlg/settings/visual/xgpoint" );
  KEY( "title",               DLG::SETTINGS::VISUAL::XGPOINT::TITLE );
  KEY( "xpoint",              DLG::SETTINGS::VISUAL::XGPOINT::XPOINT );
  KEY( "gpoint",              DLG::SETTINGS::VISUAL::XGPOINT::GPOINT );
  KEY( "gpointlen",           DLG::SETTINGS::VISUAL::XGPOINT::GPOINTLEN );
  GROUP( "dlg/settings/visual/grid_axes" );
  KEY( "title",               DLG::SETTINGS::VISUAL::GRID_AXES::TITLE );
  KEY( "axes",                DLG::SETTINGS::VISUAL::GRID_AXES::AXES );
  KEY( "grid",                DLG::SETTINGS::VISUAL::GRID_AXES::GRID );
  KEY( "text",                DLG::SETTINGS::VISUAL::GRID_AXES::TEXT );
  GROUP( "dlg/settings/visual/elem_node" );
  KEY( "title",               DLG::SETTINGS::VISUAL::ELEM_NODE::TITLE );
  KEY( "node",                DLG::SETTINGS::VISUAL::ELEM_NODE::NODE );
  KEY( "elem",                DLG::SETTINGS::VISUAL::ELEM_NODE::ELEM );
  GROUP( "dlg/settings/visual/equil" );
  KEY( "title",               DLG::SETTINGS::VISUAL::EQUIL::TITLE );
  KEY( "pos",                 DLG::SETTINGS::VISUAL::EQUIL::POS );
  KEY( "neg",                 DLG::SETTINGS::VISUAL::EQUIL::NEG );
  GROUP( "dlg/settings/visual/mesh" );
  KEY( "title",               DLG::SETTINGS::VISUAL::MESH::TITLE );
  KEY( "element",             DLG::SETTINGS::VISUAL::MESH::ELEMENT );
  KEY( "irreg",               DLG::SETTINGS::VISUAL::MESH::IRREG );
  KEY( "irreg_hl",            DLG::SETTINGS::VISUAL::MESH::IRREG_HL );
  KEY( "cellcenter",          DLG::SETTINGS::VISUAL::MESH::CELLCENTER );
  KEY( "cc_length",           DLG::SETTINGS::VISUAL::MESH::CCLENGTH );

  GROUP( "dlg/comment_edit" );
  KEY( "title",               DLG::COMMENTEDIT::TITLE );
  KEY( "linked_object",       DLG::COMMENTEDIT::LINKED_OBJECT );
  KEY( "insert_descr",        DLG::COMMENTEDIT::INSERT_DESCR );

  GROUP( "dlg/mesh_options" );
  KEY( "help",                DLG::MESHOPTIONS::HELP );
  KEY( "title",               DLG::MESHOPTIONS::TITLE );
  GROUP( "dlg/mesh_options/mode" );
  KEY( "label",               DLG::MESHOPTIONS::MODE::TITLE );
  KEY( "surfaces",            DLG::MESHOPTIONS::MODE::SURFACES );
  KEY( "splines",             DLG::MESHOPTIONS::MODE::SPLINES );
  GROUP( "dlg/mesh_options/label" );
  KEY( "slide",               DLG::MESHOPTIONS::LBL::SLIDE );
  KEY( "border",              DLG::MESHOPTIONS::LBL::BORDER );
  GROUP( "dlg/mesh_options/err" );
  KEY( "bad_sliding_tresh",   DLG::MESHOPTIONS::ERR::BAD_SLIDING_TRESHOLD );

  GROUP( "dlg/save_interval" );
  KEY( "title",               DLG::SAVE_INTERVAL::TITLE );
  KEY( "label.save",          DLG::SAVE_INTERVAL::LBL::SAVE );

  GROUP( "dlg/project_settings" );
  KEY( "title",               DLG::PRJSETTINGS::TITLE );
  KEY( "topology",            DLG::PRJSETTINGS::TOPOLOGY::TITLE );
  GROUP( "dlg/project_settings/sz_restr" );
  KEY( "title",               DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TITLE );
  KEY( "targets",             DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TARGETS );
  KEY( "tchains",             DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TCHAINS );
  KEY( "tpoints",             DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TPOINTS );
  KEY( "b2carre",             DLG::PRJSETTINGS::TOPOLOGY::RESTRICTB2::B2CARRE );
  KEY( "b2eirene",            DLG::PRJSETTINGS::TOPOLOGY::RESTRICTB2::B2EIRENE );


  /****************************************************************************
   *   MENU > HELP
   ****************************************************************************/

  GROUP( "dlg/about" );
  KEY( "title",               DLG::ABOUT::TITLE );
  KEY( "help",                DLG::ABOUT::HELP );
  KEY( "text",                DLG::ABOUT::TEXT );

  GROUP( "dlg/info" );
  KEY( "title",               DLG::INFO::TITLE );
  KEY( "help",                DLG::INFO::HELP );
  GROUP( "dlg/info/label" );
  KEY( "node",                DLG::INFO::LBL::NODES );
  KEY( "elems",               DLG::INFO::LBL::ELEMS );
  KEY( "unused",              DLG::INFO::LBL::UNUSEDNUM );
  KEY( "sources",             DLG::INFO::LBL::SOURCES );
  KEY( "surfaces",            DLG::INFO::LBL::SURFACES );
  KEY( "gridpoints",          DLG::INFO::LBL::GRIDPOINTS );
  KEY( "separators",          DLG::INFO::LBL::SEPARATORS );
  KEY( "chords",              DLG::INFO::LBL::CHORDS );

  GROUP( "dlg/new_prefs_file" );
  KEY( "title",               DLG::NEWPREFSFILE::TITLE );
  KEY( "help",                DLG::NEWPREFSFILE::HELP );
  KEY( "text",                DLG::NEWPREFSFILE::TEXT );

  GROUP( "dlg/new_version" );
  KEY( "title",               DLG::NEWVERSION::TITLE );
  KEY( "help",                DLG::NEWVERSION::HELP );
  KEY( "text",                DLG::NEWVERSION::TEXT );
  KEY( "btn.win",             DLG::NEWVERSION::BTN::WIN );

  GROUP( "dlg/help" );
  KEY( "title",               DLG::DGHELP::TITLE );

}
