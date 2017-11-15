#include "StringsManager.h"

void StringsManager::MenusKeys()
{
  INIT_TREE;

  GROUP( "menu/file" );

  KEY( "menu",                  MENU::FILES::TITLE );
  KEY( "new",                   MENU::FILES::NEW );
  KEY( "open",                  MENU::FILES::OPEN );
  KEY( "save",                  MENU::FILES::SAVE );
  KEY( "save_as",               MENU::FILES::SAVEAS );
  KEY( "save_copy",             MENU::FILES::SAVECOPY );
  KEY( "output",                MENU::FILES::OUTPUT );
  KEY( "print",                 MENU::FILES::PRINT );
  KEY( "exit",                  MENU::FILES::EXIT );
  KEY( "editor",                MENU::FILES::EDITOR );
  KEY( "recent.menu",           MENU::FILES::RECENT::TITLE );

  GROUP( "menu/file/import" );

  KEY( "menu",                MENU::FILES::IMPORT::TITLE );
  KEY( "template",            MENU::FILES::IMPORT::TEMPLATE );
  KEY( "equilibrium",         MENU::FILES::IMPORT::EQUILIBRIUM );
  KEY( "mesh",                MENU::FILES::IMPORT::MESH );
  KEY( "topology",            MENU::FILES::IMPORT::TOPOLOGY );
  KEY( "sonnetgrid",          MENU::FILES::IMPORT::SONNETGRID );

  GROUP( "menu/file/export" );

  KEY( "menu",                MENU::FILES::EXPORT::TITLE );
  KEY( "mesh",                MENU::FILES::EXPORT::MESH );
  KEY( "element",             MENU::FILES::EXPORT::ELEMENT );

  GROUP( "menu/edit" );

  KEY( "menu",                  MENU::EDIT::TITLE );
  KEY( "undo",                  MENU::EDIT::UNDO );
  KEY( "redo",                  MENU::EDIT::REDO );
  KEY( "redo_all",              MENU::EDIT::REDO_ALL );
  KEY( "clear_action_stack",    MENU::EDIT::CLEAR_ASTACK );
  KEY( "mark_all_elements",     MENU::EDIT::MARK_ALL_ELEMENTS );
  KEY( "mark_all_separators",   MENU::EDIT::MARK_ALL_SEPARATORS );
  KEY( "mark_all_chords",       MENU::EDIT::MARK_ALL_CHORDS );
  KEY( "unmark_all",            MENU::EDIT::UNMARK_ALL );
  KEY( "extend_chords",         MENU::EDIT::EXTEND_CHORDS );
  KEY( "move_rotate",           MENU::EDIT::MOVE_ROTATE );

  GROUP( "menu/edit/create" );

  KEY( "menu",                MENU::EDIT::CREATE::TITLE );
  KEY( "point",               MENU::EDIT::CREATE::POINT );
  KEY( "source",              MENU::EDIT::CREATE::SOURCE );
  KEY( "chord",               MENU::EDIT::CREATE::CHORD );
  KEY( "surface",             MENU::EDIT::CREATE::SURFACE );
  KEY( "gridpoints",          MENU::EDIT::CREATE::GRIDPOINTS );
  KEY( "separators",          MENU::EDIT::CREATE::SEPARATORS );

  GROUP( "menu/edit/delete" );

  KEY( "menu",                MENU::EDIT::DELETE::TITLE );
  KEY( "marked_objects",      MENU::EDIT::DELETE::MARKED_OBJECTS );
  KEY( "unused_points",       MENU::EDIT::DELETE::UNUSED_POINTS );
  KEY( "virtual_surfaces",    MENU::EDIT::DELETE::VIRTUAL_SURFACES );
  KEY( "elements",            MENU::EDIT::DELETE::ELEMENTS );
  KEY( "sources",             MENU::EDIT::DELETE::SOURCES );
  KEY( "chords",              MENU::EDIT::DELETE::CHORDS );
  KEY( "surfaces",            MENU::EDIT::DELETE::SURFACES );
  KEY( "gridpoints",          MENU::EDIT::DELETE::GRIDPOINTS );
  KEY( "separators",          MENU::EDIT::DELETE::SEPARATORS );
  KEY( "equilibrium",         MENU::EDIT::DELETE::EQUILIBRIUM );
  KEY( "template",            MENU::EDIT::DELETE::TEMPLATE );
  KEY( "mesh",                MENU::EDIT::DELETE::MESH );
  KEY( "xpoint",              MENU::EDIT::DELETE::XPOINT );
  KEY( "sonnetgrid",          MENU::EDIT::DELETE::SONNETGRID );

  GROUP( "menu/view" );

  KEY( "menu",                  MENU::VIEW::TITLE );
  KEY( "refresh",               MENU::VIEW::REFRESH );
  KEY( "zoom_in",               MENU::VIEW::ZOOM_IN );
  KEY( "zoom_out",              MENU::VIEW::ZOOM_OUT );
  KEY( "picture_view",          MENU::VIEW::PICTURE_VIEW );
  KEY( "selection_view",        MENU::VIEW::SELECTION_VIEW );
  KEY( "previous_view",         MENU::VIEW::PREVIOUS_VIEW );
  KEY( "remove_labels",         MENU::VIEW::REMOVE_LABELS );
  KEY( "mesh_headers",          MENU::VIEW::MESH_HEADER );

  GROUP( "menu/view/rotate" );

  KEY( "menu",                MENU::VIEW::ROTATE::TITLE );
  KEY( "increase_angle",      MENU::VIEW::ROTATE::INCREASE_ANGLE );
  KEY( "decrease_angle",      MENU::VIEW::ROTATE::DECREASE_ANGLE );
  KEY( "set_angle",           MENU::VIEW::ROTATE::SET_ANGLE );
  KEY( "rectify",             MENU::VIEW::ROTATE::RECTIFY );

  GROUP( "menu/view/strshr" );

  KEY( "menu",                MENU::VIEW::STR_SHR::TITLE );
  KEY( "stretch_horizontal",  MENU::VIEW::STR_SHR::STRETCH_HORIZONTAL );
  KEY( "stretch_vertical",    MENU::VIEW::STR_SHR::STRETCH_VERTICAL );
  KEY( "shrink_horizontal",   MENU::VIEW::STR_SHR::SHRINK_HORIZONTAL );
  KEY( "shrink_vertical",     MENU::VIEW::STR_SHR::SHRINK_VERTICAL );
  KEY( "stretch",             MENU::VIEW::STR_SHR::STRETCH );
  KEY( "justify",             MENU::VIEW::STR_SHR::JUSTIFY );

  GROUP( "menu/view/mode" );

  KEY( "menu",                  MENU::VIEW::MODE::TITLE );
  KEY( "stretch",               MENU::VIEW::MODE::STRETCH );
  KEY( "topdown_view",          MENU::VIEW::MODE::TOPDOWN_VIEW );
  KEY( "radial_grid",           MENU::VIEW::MODE::RADIAL_GRID );
  KEY( "colored_equil",         MENU::VIEW::MODE::COLORED_EQUIL );

  GROUP( "menu/view/display" );

  KEY( "menu",               MENU::VIEW::DISPLAY::TITLE );
  KEY( "nodes",              MENU::VIEW::DISPLAY::NODES );
  KEY( "irregular_nodes",    MENU::VIEW::DISPLAY::IRRNODES );
  KEY( "elements",           MENU::VIEW::DISPLAY::ELEMENTS );
  KEY( "sources",            MENU::VIEW::DISPLAY::SOURCES );
  KEY( "chords",             MENU::VIEW::DISPLAY::CHORDS );
  KEY( "chords3d",           MENU::VIEW::DISPLAY::CHORDS3D );
  KEY( "separators",         MENU::VIEW::DISPLAY::SEPARATORS );
  KEY( "normals",            MENU::VIEW::DISPLAY::NORMALS );
  KEY( "numbers",            MENU::VIEW::DISPLAY::NUMBERS );
  KEY( "surfaces",           MENU::VIEW::DISPLAY::SURFACES );
  KEY( "gridpoints",         MENU::VIEW::DISPLAY::GRIDPOINTS );
  KEY( "xpoints",            MENU::VIEW::DISPLAY::XPOINTS );
  KEY( "mesh",               MENU::VIEW::DISPLAY::MESH );
  KEY( "meshdetails",        MENU::VIEW::DISPLAY::MESHDETAILS );
  KEY( "equilibrium",        MENU::VIEW::DISPLAY::EQUILIBRIUM );
  KEY( "template",           MENU::VIEW::DISPLAY::TEMPLATE );
  KEY( "axes",               MENU::VIEW::DISPLAY::AXES );
  KEY( "grid",               MENU::VIEW::DISPLAY::GRID );
  KEY( "sonnetgrid",         MENU::VIEW::DISPLAY::SONNETGRID );
  KEY( "comments",           MENU::VIEW::DISPLAY::COMMENTS );

  GROUP( "menu/commands" );

  KEY( "menu",                  MENU::COMMANDS::TITLE );
  KEY( "renumber_elements",     MENU::COMMANDS::RENUMBER_ELEMENTS );
  KEY( "check_variables",       MENU::COMMANDS::CHECK_VARIABLES );
  KEY( "rebuild_carre_objects", MENU::COMMANDS::REBUILD_CARRE_OBJECTS );
  KEY( "find_xpoints",          MENU::COMMANDS::FIND_XPOINTS );
  KEY( "find_xpointsegs",       MENU::COMMANDS::FIND_XPOINTSEGS );
  KEY( "edit_topology",         MENU::COMMANDS::EDIT_TOPOLOGY );
  KEY( "update_topology",       MENU::COMMANDS::UPDATE_TOPOLOGY );
  KEY( "test",                  MENU::COMMANDS::TEST );

  GROUP( "menu/commands/convert" );

  KEY( "menu",                 MENU::COMMANDS::CONVERT::TITLE );
  KEY( "template_to_elements", MENU::COMMANDS::CONVERT::TEMPLATE_TO_ELEMENTS );
  KEY( "elements_to_chords",   MENU::COMMANDS::CONVERT::ELEMENTS_TO_CHORDS );
  KEY( "chords_to_elements",   MENU::COMMANDS::CONVERT::CHORDS_TO_ELEMENTS );

  GROUP( "menu/commands/simplify" );

  KEY( "menu",                        MENU::COMMANDS::SIMPLIFY::TITLE );
  KEY( "fill_gaps",                   MENU::COMMANDS::SIMPLIFY::FILL_GAPS );
  KEY( "merge_split_elements",        MENU::COMMANDS::SIMPLIFY::MERGE_SPLIT_ELEMENTS );
  KEY( "lineup.menu",                 MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::TITLE );
  KEY( "lineup.all_normals",          MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::ALL );
  KEY( "lineup.marked_normals",       MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::MARKED );

  GROUP( "menu/variables" );

  KEY( "menu",                  MENU::VARIABLES::TITLE );
  KEY( "add_layer",             MENU::VARIABLES::ADD_LAYER );
  KEY( "remove_layer",          MENU::VARIABLES::REMOVE_LAYER );
  KEY( "display_layer",         MENU::VARIABLES::DISPLAY_LAYER );
  KEY( "general_surface_data",  MENU::VARIABLES::GENERAL_SURFACE_DATA );
  KEY( "structure",             MENU::VARIABLES::STRUCTURE );
  KEY( "global_eirene_data",    MENU::VARIABLES::GLOBAL_EIRENE_DATA );
  KEY( "add.menu",              MENU::VARIABLES::ADD::TITLE );
  KEY( "remove.menu",           MENU::VARIABLES::REMOVE::TITLE );

  GROUP( "menu/options" );

  KEY( "menu",               MENU::OPTIONS::TITLE );
  KEY( "project_settings",   MENU::OPTIONS::PROJECT_SETTINGS );
  KEY( "output_mode",        MENU::OPTIONS::OUTPUT_MODE );
  KEY( "mesh_editing",       MENU::OPTIONS::MESH_EDITING );
  KEY( "display_menu",       MENU::OPTIONS::DISPLAY_MENU );
  KEY( "display_toolbar",    MENU::OPTIONS::DISPLAY_TOOLBAR );
  KEY( "manual_refresh",     MENU::OPTIONS::MANUAL_REFRESH );
  KEY( "save_interval",      MENU::OPTIONS::SAVE_INTERVAL );
  KEY( "settings",           MENU::OPTIONS::SETTINGS );
  KEY( "setup.menu",         MENU::OPTIONS::SETUP::TITLE );
  KEY( "setup.variables",    MENU::OPTIONS::SETUP::VARIABLES );
  KEY( "setup.save",         MENU::OPTIONS::SETUP::SAVE );

  GROUP( "menu/window" );

  KEY( "menu",                 MENU::WINDOW::TITLE );
  KEY( "hide_menu",            MENU::WINDOW::HIDE_MENU );
  KEY( "new_view",             MENU::WINDOW::NEW_VIEW );
  KEY( "close",                MENU::WINDOW::CLOSE );
  KEY( "statistics",           MENU::WINDOW::STATISTICS );
  KEY( "toolbox",              MENU::WINDOW::TOOLBOX );

  GROUP( "menu/window/panels" );

  KEY( "modelinfo",           MENU::WINDOW::PANELS::MODEL_INFO );
  KEY( "actioninfo",          MENU::WINDOW::PANELS::ACT_INFO );
  KEY( "views",               MENU::WINDOW::PANELS::VIEWS );
  KEY( "console",             MENU::WINDOW::PANELS::CONSOLE );
  KEY( "oldtools",            MENU::WINDOW::PANELS::OLDTOOLS );
  KEY( "tools",               MENU::WINDOW::PANELS::TOOLS );
  KEY( "tb_view",             MENU::WINDOW::PANELS::TB_VIEW );
  KEY( "tb_edit",             MENU::WINDOW::PANELS::TB_EDIT );

  GROUP( "menu/help" );

  KEY( "menu",                  MENU::HELP::TITLE );
  KEY( "whats_new",             MENU::HELP::WHATS_NEW );
  KEY( "manual",                MENU::HELP::MANUAL );
  KEY( "using_help",            MENU::HELP::USING_HELP );
  KEY( "on_main_window",        MENU::HELP::ON_MAIN_WINDOW );
  KEY( "about",                 MENU::HELP::ABOUT );

  GROUP( "tools" );

  KEY( "zoom_pan",             TOOLS::ZOOMPAN );
  KEY( "mark",                 TOOLS::MARK );
  KEY( "examine",              TOOLS::EXAMINE );
  KEY( "rotate",               TOOLS::ROTATE );
  KEY( "stretch",              TOOLS::STRETCH );
  KEY( "move",                 TOOLS::MOVE );
  KEY( "delete",               TOOLS::DELETE );
  KEY( "add_element",          TOOLS::ADDELEMENT );
  KEY( "add_source",           TOOLS::ADDSOURCES );
  KEY( "add_chords",           TOOLS::ADDCHORDS );
  KEY( "add_surface",          TOOLS::ADDSURFACE );
  KEY( "add_gridpoint",        TOOLS::ADDGRIDPOINT );
  KEY( "split_element",        TOOLS::SPLITELEMENT );
  KEY( "join_elemenets",       TOOLS::JOINELEMENTS );
  KEY( "connect_points",       TOOLS::CONNECTPOINTS );
  KEY( "reposition",           TOOLS::REPOSITION );
  KEY( "reverse_normals",      TOOLS::REVERSENORMALS );
  KEY( "move_mesh_point",      TOOLS::MOVEMESHPOINT );
  KEY( "set_xpoint",           TOOLS::SETXPOINT );
  KEY( "adjust_chord",         TOOLS::ADJUSTCHORD );
  KEY( "extend_chord",         TOOLS::EXTENDCHORD );
  KEY( "comments",             TOOLS::COMMENT );
}
