#include "HelpManager.h"
#include "../main/Console.h"

const QString HelpManager::scsNoHelp = "Help topic not found";

void HelpManager::ConstructKeys()
{
  INIT_TREE;

  GROUP( "main" );

  KEY( "help_system",     DLG::DGHELP::DIALOG );
  KEY( "main_window",     HELP::DG2_HELP::MAIN_WINDOW );

  GROUP( "dlg" );

  KEY( "new_prefs",       DLG::NEWPREFSFILE::DIALOG );
  KEY( "new_version",     DLG::NEWVERSION::DIALOG );
  KEY( "unsaved",         DLG::APP_UNSAVED::DIALOG );
  KEY( "unsaved_mesh",    DLG::MESH_UNSAVED::DIALOG );

  GROUP( "dlg/file" );

  KEY( "open",            DLG::FILEOPEN::DIALOG);
  KEY( "save",            DLG::FILESAVE::DIALOG );
  KEY( "print",           DLG::PRINT::DIALOG );

  GROUP( "dlg/file/import" );

  KEY( "equilibrium",     DLG::EQUIL::DIALOG );
  KEY( "template",        DLG::TEMPLATE ::DIALOG);
  KEY( "mesh",            DLG::SONNET::DIALOG );
  KEY( "topology",        DLG::IMPORTTOPO::DIALOG );

  GROUP( "dlg/file/export" );

  KEY( "mesh",            DLG::EXPORT_MESH::DIALOG );
  KEY( "elements",        DLG::EXPORT_ELEMS_TEMPLATE::DIALOG );

  GROUP( "dlg/edit/create" );

  KEY( "node",            DLG::CREATE_NODE::DIALOG );
  KEY( "source",          DLG::CREATE_SOURCE::DIALOG );
  KEY( "chord",           DLG::CREATE_CHORD::DIALOG );
  KEY( "surface",         DLG::CREATE_SURFACE::DIALOG );
  KEY( "gridpoint",       DLG::CREATE_GPOINT::DIALOG );

  GROUP( "dlg/edit" );

  KEY( "move_rotate",     DLG::ROTMOVE::DIALOG );
  KEY( "mesh_header",     DLG::EDIT_MESH_HEADER::DIALOG );

  GROUP( "dlg/commands/simplify" );

  KEY( "fill_gaps",       DLG::GLUEPOINTS::DIALOG );
  KEY( "merge_split",     DLG::GLUEELEMS::DIALOG );

  GROUP( "dlg/commands" );

  KEY( "invalid_values",  DLG::INVALIDVARS::DIALOG );
  KEY( "topology",        DLG::TOPOLOGY::DIALOG );

  GROUP( "dlg/variables" );

  KEY( "variables",       DLG::VARSEDIT::DIALOG );

  GROUP( "dlg/options" );

  KEY( "warning",         DLG::SETUPWARNING::DIALOG );
  KEY( "layer_types",     DLG::VARSETDEFLIST::DIALOG );
  KEY( "new_layer",       DLG::VARSETDEFCREATE::DIALOG );
  KEY( "layer_options",   DLG::VARSETDEFEDIT::DIALOG );
  KEY( "new_var",         DLG::VARDEFCREATE::DIALOG );
  KEY( "var_options",     DLG::VARDEFEDIT::DIALOG );
  KEY( "edit_help",       DLG::HELPEDIT::DIALOG );
  KEY( "save_config",     DLG::SAVECONFIG::DIALOG );
  KEY( "output_mode",     DLG::OUTPUTMODE::DIALOG );
  KEY( "mesh_edit",       DLG::MESHOPTIONS::DIALOG );

  GROUP( "dlg/help" );

  KEY( "about",           DLG::ABOUT::DIALOG );
  KEY( "statistics",      DLG::INFO::DIALOG );
  KEY( "toolbox",         DLG::TOOLBOX::DIALOG ); //removed in v3.0

  GROUP( "tools" );

  KEY( "examine",         TOOLS::EXAMINE );
  KEY( "zoom_pan",        TOOLS::ZOOMPAN );
  KEY( "mark",            TOOLS::MARK );
  KEY( "move",            TOOLS::MOVE );
  KEY( "delete",          TOOLS::DELETE );
  KEY( "element",         TOOLS::ADDELEMENT );
  KEY( "source",          TOOLS::ADDSOURCES );
  KEY( "chord",           TOOLS::ADDCHORDS );
  KEY( "surface",         TOOLS::ADDSURFACE );
  KEY( "gridpoint",       TOOLS::ADDGRIDPOINT );
  KEY( "xpoint",          TOOLS::SETXPOINT );
  KEY( "split",           TOOLS::SPLITELEMENT );
  KEY( "join",            TOOLS::JOINELEMENTS );
  KEY( "connect",         TOOLS::CONNECTPOINTS );
  KEY( "reposition",      TOOLS::REPOSITION );
  KEY( "reverse",         TOOLS::REVERSENORMALS );
  KEY( "move_mp",         TOOLS::MOVEMESHPOINT );

  GROUP( "menu/file" );

  KEY( "new",             MENU::FILES::NEW );
  KEY( "open",            MENU::FILES::OPEN );
  KEY( "recent",          MENU::FILES::RECENT::TITLE );
  KEY( "save",            MENU::FILES::SAVE );
  KEY( "output",          MENU::FILES::OUTPUT );
  KEY( "print",           MENU::FILES::PRINT );
  KEY( "exit",            MENU::FILES::EXIT );

  GROUP( "menu/file/import" );

  KEY( "template",        MENU::FILES::IMPORT::TEMPLATE );
  KEY( "equilibrium",     MENU::FILES::IMPORT::EQUILIBRIUM );
  KEY( "mesh",            MENU::FILES::IMPORT::MESH );
  KEY( "topology",        MENU::FILES::IMPORT::TOPOLOGY );

  GROUP( "menu/file/export" );

  KEY( "mesh",            MENU::FILES::EXPORT::MESH );
  KEY( "elements",        MENU::FILES::EXPORT::ELEMENT );

  GROUP( "menu/edit" );

  KEY( "undo",            MENU::EDIT::UNDO );
  KEY( "redo",            MENU::EDIT::REDO );
  KEY( "redo_all",        MENU::EDIT::REDO_ALL );
  KEY( "mark_all",        MENU::EDIT::MARK_ALL_ELEMENTS );
  KEY( "unmark_all",      MENU::EDIT::UNMARK_ALL );
  KEY( "move_rotate",     MENU::EDIT::MOVE_ROTATE );

  GROUP( "menu/edit/create" );

  KEY( "point",           MENU::EDIT::CREATE::POINT );
  KEY( "source",          MENU::EDIT::CREATE::SOURCE );
  KEY( "chord",           MENU::EDIT::CREATE::CHORD );
  KEY( "surface",         MENU::EDIT::CREATE::SURFACE );
  KEY( "gridpoints",      MENU::EDIT::CREATE::GRIDPOINTS );
  KEY( "separators",      MENU::EDIT::CREATE::SEPARATORS );

  GROUP( "menu/edit/delete" );

  KEY( "marked_objects",  MENU::EDIT::DELETE::MARKED_OBJECTS );
  KEY( "unused_points",   MENU::EDIT::DELETE::UNUSED_POINTS );
  KEY( "virtual_sx",      MENU::EDIT::DELETE::VIRTUAL_SURFACES );
  KEY( "elements",        MENU::EDIT::DELETE::ELEMENTS );
  KEY( "sources",         MENU::EDIT::DELETE::SOURCES );
  KEY( "chords",          MENU::EDIT::DELETE::CHORDS );
  KEY( "surfaces",        MENU::EDIT::DELETE::SURFACES );
  KEY( "gridpoints",      MENU::EDIT::DELETE::GRIDPOINTS );
  KEY( "separators",      MENU::EDIT::DELETE::SEPARATORS );
  KEY( "equilibrium",     MENU::EDIT::DELETE::EQUILIBRIUM );
  KEY( "template",        MENU::EDIT::DELETE::TEMPLATE );
  KEY( "mesh",            MENU::EDIT::DELETE::MESH );
  KEY( "xpoint",          MENU::EDIT::DELETE::XPOINT );

  GROUP( "menu/view" );

  KEY( "refresh",         MENU::VIEW::REFRESH );
  KEY( "zoom_in",         MENU::VIEW::ZOOM_IN );
  KEY( "zoom_out",        MENU::VIEW::ZOOM_OUT );
  KEY( "picture",         MENU::VIEW::PICTURE_VIEW );
  KEY( "selection",       MENU::VIEW::SELECTION_VIEW );
  KEY( "previous",        MENU::VIEW::PREVIOUS_VIEW );
  KEY( "remove_labels",   MENU::VIEW::REMOVE_LABELS );
  KEY( "display",         MENU::VIEW::DISPLAY::TITLE );
  KEY( "mesh_header",     MENU::VIEW::MESH_HEADER );

  GROUP( "menu/commands" );

  KEY( "renumber",        MENU::COMMANDS::RENUMBER_ELEMENTS );
  KEY( "check_vars",      MENU::COMMANDS::CHECK_VARIABLES );
  KEY( "rebuild_carre",   MENU::COMMANDS::REBUILD_CARRE_OBJECTS );
  KEY( "edit_topology",   MENU::COMMANDS::EDIT_TOPOLOGY );

  GROUP( "menu/commands/convert" );

  KEY( "template",        MENU::COMMANDS::CONVERT::TEMPLATE_TO_ELEMENTS );
  KEY( "elements",        MENU::COMMANDS::CONVERT::ELEMENTS_TO_CHORDS );
  KEY( "chords",          MENU::COMMANDS::CONVERT::CHORDS_TO_ELEMENTS );

  GROUP( "menu/commands/simplify" );

  KEY( "fill_gaps",       MENU::COMMANDS::SIMPLIFY::FILL_GAPS );
  KEY( "merge_split",     MENU::COMMANDS::SIMPLIFY::MERGE_SPLIT_ELEMENTS );
  KEY( "lineup_normals",  MENU::COMMANDS::SIMPLIFY::LINEUP_NORMALS::TITLE );

  GROUP( "menu/variables" );

  KEY( "add_layer",       MENU::VARIABLES::ADD_LAYER );
  KEY( "remove_layer",    MENU::VARIABLES::REMOVE_LAYER );
  KEY( "display_layer",   MENU::VARIABLES::DISPLAY_LAYER );

  GROUP( "menu/options" );

  KEY( "output_mode",     MENU::OPTIONS::OUTPUT_MODE );
  KEY( "mesh_editing",    MENU::OPTIONS::MESH_EDITING );
  KEY( "display_menu",    MENU::OPTIONS::DISPLAY_MENU );
  KEY( "display_toolbar", MENU::OPTIONS::DISPLAY_TOOLBAR );
  KEY( "manual_refresh",  MENU::OPTIONS::MANUAL_REFRESH );
  KEY( "setup",           MENU::OPTIONS::SETUP::TITLE );
  KEY( "setup.variables", MENU::OPTIONS::SETUP::VARIABLES );
  KEY( "setup.save",      MENU::OPTIONS::SETUP::SAVE );

  GROUP( "menu/window" );

  KEY( "new_view",        MENU::WINDOW::NEW_VIEW );
  KEY( "close",           MENU::WINDOW::CLOSE );
  KEY( "statistics",      MENU::WINDOW::STATISTICS );
  KEY( "toolbox",         MENU::WINDOW::TOOLBOX );

  GROUP( "menu/help" );

  KEY( "menu",            MENU::HELP::TITLE );
  KEY( "whats_new",       MENU::HELP::WHATS_NEW );
  KEY( "manual",          MENU::HELP::MANUAL );
  KEY( "about",           MENU::HELP::ABOUT );
}
