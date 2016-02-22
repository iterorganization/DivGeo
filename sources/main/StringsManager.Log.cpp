#include "StringsManager.h"

void StringsManager::LogKeys()
{
  INIT_TREE;

  GROUP( "log/menu/file" );

  KEY( "new",                 LOG::MENU::FILE::NEW );
  KEY( "open",                LOG::MENU::FILE::OPEN );
  KEY( "save",                LOG::MENU::FILE::SAVE );
  KEY( "save_as",             LOG::MENU::FILE::SAVEAS );
  KEY( "save_copy",           LOG::MENU::FILE::SAVECOPY );
  KEY( "output",              LOG::MENU::FILE::OUTPUT );
  KEY( "print",               LOG::MENU::FILE::PRINT );
  KEY( "exit",                LOG::MENU::FILE::EXIT );
  KEY( "recent",              LOG::MENU::FILE::RECENT );
  KEY( "editor",              LOG::MENU::FILE::EDITOR );

  GROUP( "log/menu/file/import" );

  KEY( "template",            LOG::MENU::FILE::IMPORT::TEMPLATE );
  KEY( "equilibrium",         LOG::MENU::FILE::IMPORT::EQUILIBRIUM );
  KEY( "mesh",                LOG::MENU::FILE::IMPORT::MESH );
  KEY( "topology",            LOG::MENU::FILE::IMPORT::TOPOLOGY );
  KEY( "sonnetgrid",          LOG::MENU::FILE::IMPORT::SONNETGRID );

  GROUP( "log/menu/file/export" );

  KEY( "mesh",                LOG::MENU::FILE::EXPORT::MESH );
  KEY( "element",             LOG::MENU::FILE::EXPORT::ELEMENT );

  GROUP( "log/menu/edit" );

  KEY( "undo",                LOG::MENU::EDIT::UNDO );
  KEY( "redo",                LOG::MENU::EDIT::REDO );
  KEY( "redo_all",            LOG::MENU::EDIT::REDO_ALL );
  KEY( "clear_action_stack",  LOG::MENU::EDIT::CLEAR_ASTACK );
  KEY( "mark_all_elements",   LOG::MENU::EDIT::MARK_ALL_ELEMENTS );
  KEY( "mark_all_separators", LOG::MENU::EDIT::MARK_ALL_SEPARATORS );
  KEY( "mark_all_chords",     LOG::MENU::EDIT::MARK_ALL_CHORDS );
  KEY( "unmark_all",          LOG::MENU::EDIT::UNMARK_ALL );
  KEY( "extend_chords",       LOG::MENU::EDIT::EXTEND_CHORDS );
  KEY( "move_rotate",         LOG::MENU::EDIT::MOVE_ROTATE );

  GROUP( "log/menu/edit/create" );

  KEY( "point",               LOG::MENU::EDIT::CREATE::POINT );
  KEY( "source",              LOG::MENU::EDIT::CREATE::SOURCE );
  KEY( "chord",               LOG::MENU::EDIT::CREATE::CHORD );
  KEY( "surface",             LOG::MENU::EDIT::CREATE::SURFACE );
  KEY( "gridpoints",          LOG::MENU::EDIT::CREATE::GRIDPOINTS );
  KEY( "separators",          LOG::MENU::EDIT::CREATE::SEPARATORS );

  GROUP( "log/menu/edit/delete" );

  KEY( "marked_objects",      LOG::MENU::EDIT::DELETE::MARKED_OBJECTS );
  KEY( "unused_points",       LOG::MENU::EDIT::DELETE::UNUSED_POINTS );
  KEY( "virtual_surfaces",    LOG::MENU::EDIT::DELETE::VIRTUAL_SURFACES );
  KEY( "elements",            LOG::MENU::EDIT::DELETE::ELEMENTS );
  KEY( "sources",             LOG::MENU::EDIT::DELETE::SOURCES );
  KEY( "chords",              LOG::MENU::EDIT::DELETE::CHORDS );
  KEY( "surfaces",            LOG::MENU::EDIT::DELETE::SURFACES );
  KEY( "gridpoints",          LOG::MENU::EDIT::DELETE::GRIDPOINTS );
  KEY( "separators",          LOG::MENU::EDIT::DELETE::SEPARATORS );
  KEY( "equilibrium",         LOG::MENU::EDIT::DELETE::EQUILIBRIUM );
  KEY( "template",            LOG::MENU::EDIT::DELETE::TEMPLATE );
  KEY( "mesh",                LOG::MENU::EDIT::DELETE::MESH );
  KEY( "xpoint",              LOG::MENU::EDIT::DELETE::XPOINT );
  KEY( "sonnetgrid",          LOG::MENU::EDIT::DELETE::SONNETGRID );

  GROUP( "log/menu/view" );

  KEY( "refresh",             LOG::MENU::VIEW::REFRESH );
  KEY( "zoom_in",             LOG::MENU::VIEW::ZOOM_IN );
  KEY( "zoom_out",            LOG::MENU::VIEW::ZOOM_OUT );
  KEY( "picture_view",        LOG::MENU::VIEW::PICTURE_VIEW );
  KEY( "selection_view",      LOG::MENU::VIEW::SELECTION_VIEW );
  KEY( "previous_view",       LOG::MENU::VIEW::PREVIOUS_VIEW );
  KEY( "remove_labels",       LOG::MENU::VIEW::REMOVE_LABELS );
  KEY( "mesh_headers",        LOG::MENU::VIEW::MESH_HEADER );
  KEY( "display",             LOG::MENU::VIEW::DISPLAY );

  GROUP( "log/menu/view/rotate" );

  KEY( "increase_angle",      LOG::MENU::VIEW::ROTATE::INCREASE_ANGLE );
  KEY( "decrease_angle",      LOG::MENU::VIEW::ROTATE::DECREASE_ANGLE );
  KEY( "set_angle",           LOG::MENU::VIEW::ROTATE::SET_ANGLE );
  KEY( "rectify",             LOG::MENU::VIEW::ROTATE::RECTIFY );

  GROUP( "log/menu/view/strshr" );

  KEY( "stretch_horizontal",  LOG::MENU::VIEW::STR_SHR::STRETCH_HORIZONTAL );
  KEY( "stretch_vertical",    LOG::MENU::VIEW::STR_SHR::STRETCH_VERTICAL );
  KEY( "shrink_horizontal",   LOG::MENU::VIEW::STR_SHR::SHRINK_HORIZONTAL );
  KEY( "shrink_vertical",     LOG::MENU::VIEW::STR_SHR::SHRINK_VERTICAL );
  KEY( "stretch",             LOG::MENU::VIEW::STR_SHR::STRETCH );
  KEY( "justify",             LOG::MENU::VIEW::STR_SHR::JUSTIFY );

  GROUP( "log/menu/view/mode" );

  KEY( "stretch",             LOG::MENU::VIEW::MODE::STRETCH );
  KEY( "topdown_view",        LOG::MENU::VIEW::MODE::TOPDOWN_VIEW );
  KEY( "radial_grid",         LOG::MENU::VIEW::MODE::RADIAL_GRID );
  KEY( "colored_equil",       LOG::MENU::VIEW::MODE::COLORED_EQUIL );

  GROUP( "log/menu/commands" );

  KEY( "renumber_elements",     LOG::MENU::COMMANDS::RENUMBER_ELEMENTS );
  KEY( "check_variables",       LOG::MENU::COMMANDS::CHECK_VARIABLES );
  KEY( "rebuild_carre_objects", LOG::MENU::COMMANDS::REBUILD_CARRE_OBJECTS );
  KEY( "find_xpoints",          LOG::MENU::COMMANDS::FIND_XPOINTS );
  KEY( "find_xpointsegs",       LOG::MENU::COMMANDS::FIND_XPOINTSEGS );
  KEY( "edit_topology",         LOG::MENU::COMMANDS::EDIT_TOPOLOGY );
  KEY( "update_topology",       LOG::MENU::COMMANDS::UPDATE_TOPOLOGY );

  GROUP( "log/menu/commands/convert" );

  KEY( "template_to_elements",  LOG::MENU::COMMANDS::CONVERT::TEMPLATE_TO_ELEMENTS );
  KEY( "elements_to_chords",    LOG::MENU::COMMANDS::CONVERT::ELEMENTS_TO_CHORDS );
  KEY( "chords_to_elements",    LOG::MENU::COMMANDS::CONVERT::CHORDS_TO_ELEMENTS );

  GROUP( "log/menu/commands/simplify" );

  KEY( "fill_gaps",             LOG::MENU::COMMANDS::SIMPLIFY::FILL_GAPS );
  KEY( "merge_split_elements",  LOG::MENU::COMMANDS::SIMPLIFY::MERGE_SPLIT_ELEMENTS );
  KEY( "lineup.all_normals",    LOG::MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::ALL );
  KEY( "lineup.marked_normals", LOG::MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::MARKED );

  GROUP( "log/menu/variables" );

  KEY( "add_layer",             LOG::MENU::VARIABLES::ADD_LAYER );
  KEY( "remove_layer",          LOG::MENU::VARIABLES::REMOVE_LAYER );
  KEY( "display_layer",         LOG::MENU::VARIABLES::DISPLAY_LAYER );
  KEY( "general_surface_data",  LOG::MENU::VARIABLES::GENERAL_SURFACE_DATA );
  KEY( "structure",             LOG::MENU::VARIABLES::STRUCTURE );
  KEY( "global_eirene_data",    LOG::MENU::VARIABLES::GLOBAL_EIRENE_DATA );

  GROUP( "log/menu/options" );

  KEY( "project_settings",    LOG::MENU::OPTIONS::PROJECT_SETTINGS );
  KEY( "output_mode",         LOG::MENU::OPTIONS::OUTPUT_MODE );
  KEY( "mesh_editing",        LOG::MENU::OPTIONS::MESH_EDITING );
  KEY( "settings",            LOG::MENU::OPTIONS::SETTINGS );
  KEY( "setup.variables",     LOG::MENU::OPTIONS::SETUP::VARIABLES );
  KEY( "setup.save",          LOG::MENU::OPTIONS::SETUP::SAVE );

  GROUP( "log/menu/window" );

  KEY( "new_view",            LOG::MENU::WINDOW::NEW_VIEW );
  KEY( "close",               LOG::MENU::WINDOW::CLOSE );
  KEY( "statistics",          LOG::MENU::WINDOW::STATISTICS );

  GROUP( "log/menu/window/panels" );

  KEY( "modelinfo",           LOG::MENU::WINDOW::PANELS::MODEL_INFO );
  KEY( "actioninfo",          LOG::MENU::WINDOW::PANELS::ACT_INFO );
  KEY( "views",               LOG::MENU::WINDOW::PANELS::VIEWS );
  KEY( "console",             LOG::MENU::WINDOW::PANELS::CONSOLE );
  KEY( "oldtools",            LOG::MENU::WINDOW::PANELS::OLDTOOLS );
  KEY( "tools",               LOG::MENU::WINDOW::PANELS::TOOLS );
  KEY( "tb_view",             LOG::MENU::WINDOW::PANELS::TB_VIEW );
  KEY( "tb_edit",             LOG::MENU::WINDOW::PANELS::TB_EDIT );

  GROUP( "log/menu/help" );

  KEY( "whats_new",           LOG::MENU::HELP::WHATS_NEW );
  KEY( "manual",              LOG::MENU::HELP::MANUAL );
  KEY( "using_help",          LOG::MENU::HELP::USING_HELP );
  KEY( "about",               LOG::MENU::HELP::ABOUT );

  GROUP( "log/tools" );

  KEY( "zoom_pan",            LOG::TOOLS::ZOOMPAN );
  KEY( "mark",                LOG::TOOLS::MARK );
  KEY( "examine",             LOG::TOOLS::EXAMINE );
  KEY( "rotate",              LOG::TOOLS::ROTATE );
  KEY( "stretch",             LOG::TOOLS::STRETCH );
  KEY( "move",                LOG::TOOLS::MOVE );
  KEY( "delete",              LOG::TOOLS::DELETE );
  KEY( "add_element",         LOG::TOOLS::ADDELEMENT );
  KEY( "add_source",          LOG::TOOLS::ADDSOURCES );
  KEY( "add_chords",          LOG::TOOLS::ADDCHORDS );
  KEY( "add_surface",         LOG::TOOLS::ADDSURFACE );
  KEY( "add_gridpoint",       LOG::TOOLS::ADDGRIDPOINT );
  KEY( "split_element",       LOG::TOOLS::SPLITELEMENT );
  KEY( "join_elemenets",      LOG::TOOLS::JOINELEMENTS );
  KEY( "connect_points",      LOG::TOOLS::CONNECTPOINTS );
  KEY( "reposition",          LOG::TOOLS::REPOSITION );
  KEY( "reverse_normals",     LOG::TOOLS::REVERSENORMALS );
  KEY( "move_mesh_point",     LOG::TOOLS::MOVEMESHPOINT );
  KEY( "set_xpoint",          LOG::TOOLS::SETXPOINT );
  KEY( "adjust_chord",        LOG::TOOLS::ADJUSTCHORD );
  KEY( "extend_chord",        LOG::TOOLS::EXTENDCHORD );
  KEY( "comments",            LOG::TOOLS::COMMENTS );

  KEY( "tool",                LOG::TOOLS::TOOL );
  KEY( "selected",            LOG::TOOLS::SELECTED );
  KEY( "pressed",             LOG::TOOLS::PRESSED );
  KEY( "shiftpressed",        LOG::TOOLS::SHIFTPRESSED );
  KEY( "released",            LOG::TOOLS::RELEASED );
  KEY( "moved",               LOG::TOOLS::MOVED );
  KEY( "left_button",         LOG::TOOLS::LEFT_BUTTON );
  KEY( "right_button",        LOG::TOOLS::RIGHT_BUTTON );
  KEY( "middle_button",       LOG::TOOLS::MIDDLE_BUTTON );
}
