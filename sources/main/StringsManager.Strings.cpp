#include "StringsManager.h"

void StringsManager::StringsKeys()
{
  INIT_TREE;

  GROUP( "msg/msg" );

  KEY( "ok",                     DGMSG::OK );                  KEY( "sources_removed",        DGMSG::SOURCESREMOVED );
  KEY( "exam_node",              DGMSG::EXAMNODE );            KEY( "surfaces_removed",       DGMSG::SURFACESREMOVED );
  KEY( "exam_element",           DGMSG::EXAMELEM );            KEY( "gridpoints_removed",     DGMSG::GPOINTSREMOVED );
  KEY( "exam_surface",           DGMSG::EXAMSURFACE );         KEY( "about",                  DGMSG::ABOUT );
  KEY( "exam_gridpoint",         DGMSG::EXAMGRIDPOINT );       KEY( "exam_chord",             DGMSG::EXAMCHORD );
  KEY( "template_loaded",        DGMSG::TEMPLATELOADED );      KEY( "chords_removed",         DGMSG::CHORDSREMOVED );
  KEY( "template_removed",       DGMSG::TEMPLATEREMOVED );     KEY( "all_vars_valid",         DGMSG::ALLVARSVALID );
  KEY( "equil_loaded",           DGMSG::EQUILLOADED );         KEY( "some_vars_invalid",      DGMSG::SOMEVARSINVALID );
  KEY( "equil_removed",          DGMSG::EQUILREMOVED );        KEY( "file_printed",           DGMSG::FILEPRINTED );
  KEY( "new_file",               DGMSG::NEWFILE );             KEY( "working",                DGMSG::WORKING );
  KEY( "file_opened",            DGMSG::FILEOPENED );          KEY( "xpoints_found",          DGMSG::XPOINTSFOUND );
  KEY( "file_saved",             DGMSG::FILESAVED );           KEY( "xpointsegs_found",       DGMSG::XPSEGSFOUND );
  KEY( "sonnet_loaded",          DGMSG::SONNETLOADED );        KEY( "repos_handle_1",         DGMSG::REPOS_HANDLE_1 );
  KEY( "sonnet_removed",         DGMSG::SONNETREMOVED );       KEY( "repos_handle_2",         DGMSG::REPOS_HANDLE_2 );
  KEY( "xpoint_removed",         DGMSG::XPOINTREMOVED );       KEY( "marked_removed",         DGMSG::MARKED_REMOVED );
  KEY( "template_appended",      DGMSG::TEMPLAPPENDED );       KEY( "exam_mesh_cell",         DGMSG::EXAM_MESH_CELL );
  KEY( "exam_separator",         DGMSG::EXAMSEPARATOR );       KEY( "exam_mesh_element",      DGMSG::EXAM_MESH_ELEMENT );
  KEY( "separators_added",       DGMSG::SEPARATORSADDED );     KEY( "exam_mesh_point",        DGMSG::EXAM_MESH_POINT );
  KEY( "separators_removed",     DGMSG::SEPARATORSREMOVED );   KEY( "exam_xpointtest",        DGMSG::EXAM_XPOINTTEST );
  KEY( "renumbered",             DGMSG::RENUMBERED );          KEY( "exam_xpointseg",         DGMSG::EXAM_XPOINTSEG );
  KEY( "output",                 DGMSG::OUTPUT );              KEY( "exam_surface_xy",        DGMSG::EXAM_SURFACE_XY );
  KEY( "exam_source",            DGMSG::EXAMSOURCE );          KEY( "calculating_sz_bounds",  DGMSG::CALCULATING_SZ_BOUNDS );
  KEY( "canceled",               DGMSG::CANCELED );            KEY( "file_autosaved",         DGMSG::FILEAUTOSAVED );
  KEY( "version",                DGMSG::VERSION );             KEY( "no_bounding_elements",   DGMSG::NOBOUNDINGELEMS );
  KEY( "empty_nodes_removed",    DGMSG::EMPTYNODESREMOVED );   KEY( "no_marked_chords",       DGMSG::NOMARKEDCHORDS );
  KEY( "elements_removed",       DGMSG::ELEMSREMOVED );

  GROUP( "msg/err" );

  KEY( "file_not_found",         ERR::FILENOTFOUND );          KEY( "internal_cells",         ERR::INTERNALCELLS );
  KEY( "bad_file",               ERR::BADFILE );               KEY( "badcells",               ERR::BADCELLS );
  KEY( "bad_file_type",          ERR::BADFILETYPE );           KEY( "surface_target_ins",     ERR::SURFTARGETINS );
  KEY( "out_of_equil",           ERR::OUTOFEQUIL );            KEY( "bad_stretch_type",       ERR::BADSTRETCHTYPE );
  KEY( "bad_targets",            ERR::BADTARGETS );            KEY( "non_empty_node",         ERR::NONEMPTYNODE );
  KEY( "no_targets",             ERR::NOTARGETS );             KEY( "not_structure_part",     ERR::NOTSTRUCTPART );
  KEY( "bad_xpoint",             ERR::BADXPOINT );             KEY( "internal_gridpoint",     ERR::INTERNALGP );
  KEY( "bad_file_version",       ERR::BADFILEVERSION );        KEY( "no_help",                ERR::NOHELP );
  KEY( "used_by",                ERR::USEDBY );                KEY( "no_topic",               ERR::NOTOPIC );
  KEY( "no_view",                ERR::NOVIEW );                KEY( "bad_mask",               ERR::BADMASK );
  KEY( "no_xpoint",              ERR::NOXPOINT );              KEY( "bad_structure",          ERR::BADSTRUCTURE );
  KEY( "irregular",              ERR::IRREGULAR );             KEY( "multiple_errors",        ERR::MULTIPLEERRORS );
  KEY( "no_equil",               ERR::NOEQUIL );               KEY( "pipe_create",            ERR::PIPECREATE );
  KEY( "no_nodes",               ERR::NONODES );               KEY( "fork",                   ERR::FORK );
  KEY( "no_elements",            ERR::NOELEMS );               KEY( "ps_view_size",           ERR::PSVIEWSIZE );
  KEY( "invalid_numbers",        ERR::INVNUMBERS );            KEY( "bad_xpoint_seg",         ERR::BADXPOINTSEG );
  KEY( "min_max_varsets",        ERR::MINMAXVARSETS );         KEY( "loop_not_found",         ERR::LOOPNOTFOUND );
  KEY( "bad_name",               ERR::BADNAME );               KEY( "level_not_in_area",      ERR::LEVELNOTINAREA );
  KEY( "duplicate_name",         ERR::DUPNAME );               KEY( "level_not_found",        ERR::LEVELNOTFOUND );
  KEY( "closed",                 ERR::CLOSED );                KEY( "no_preferences",         ERR::NOPREFS );
  KEY( "broken",                 ERR::BROKEN );                KEY( "carre_surfaces",         ERR::CARRE_SURFACES );
  KEY( "fwrite",                 ERR::FWRITE );                KEY( "carre_xsurfaces",        ERR::CARRE_XSURFACES );
  KEY( "no_structure",           ERR::NOSTRUCTURE );           KEY( "carre_tangent",          ERR::CARRE_TANGENT );
  KEY( "no_closed_surfaces",     ERR::NOCLOSEDSURFS );         KEY( "carre_gridpoints",       ERR::CARRE_GPOINTS );
  KEY( "template_included",      ERR::TEMPLATEINCLUDED );      KEY( "carre_isurfaces",        ERR::CARRE_ISURFACES );
  KEY( "no_template",            ERR::NOTEMPLATE );            KEY( "carre_igridpoints",      ERR::CARRE_IGPOINTS );
  KEY( "single",                 ERR::SINGLE );                KEY( "carre_peak",             ERR::CARRE_PEAK );
  KEY( "many_selected_elems",    ERR::MANYSELELEMS );          KEY( "carre_inhibits",         ERR::CARRE_INHIBITS );
  KEY( "mult_group",             ERR::MULTGROUP );             KEY( "carre_equal_levels",     ERR::CARRE_EQUALLEVELS );
  KEY( "no_sonnet",              ERR::NOSONNET );              KEY( "carre_area1_surfaces",   ERR::CARRE_AREA1SURFACES );
  KEY( "no_config",              ERR::NOCONFIG );              KEY( "carre_out_of_equil",     ERR::CARRE_OUTOFEQUIL );
  KEY( "nothing_to_do",          ERR::NOTHINGTODO );           KEY( "carre_mode_needed",      ERR::CARRE_MODE_NEEDED );
  KEY( "already_exists",         ERR::ALREADYEXISTS );         KEY( "carre_broken_target",    ERR::CARRE_BROKENTARGET );
  KEY( "invalid_area",           ERR::INVAREA );               KEY( "carre_target_xstruct",   ERR::CARRE_TARGETXSTRUCTURE );
  KEY( "locked",                 ERR::LOCKED );                KEY( "no_marked_elements",     ERR::NO_MARKED_ELEMS );
  KEY( "no_save_config",         ERR::NOSAVECONFIG );          KEY( "no_marked_chords",       ERR::NO_MARKED_CHORDS );
  KEY( "empty_vars",             ERR::EMPTYVARS );             KEY( "empty_template",         ERR::EMPTY_TEMPLATE );
  KEY( "empty_targets",          ERR::EMPTYTARGETS );          KEY( "repos_template_1",       ERR::REPOS_TEMPLATE_1 );
  KEY( "empty_structure",        ERR::EMPTYSTRUCTURE );        KEY( "repos_template_2",       ERR::REPOS_TEMPLATE_1 );
  KEY( "bad_surfaces",           ERR::BADSURFACES );           KEY( "flat_equil",             ERR::FLAT_EQUIL );
  KEY( "bad_grid_points",        ERR::BADGRIDPOINTS );         KEY( "duplicate_mesh_elem_no", ERR::DUP_MESH_ELNO );
  KEY( "no_file_name",           ERR::NOFNAME );               KEY( "move_cut_mesh_point",    ERR::MOVE_CUT_MESH_PT );
  KEY( "no_selection",           ERR::NOSELECTION );           KEY( "mesh_no_equil",          ERR::MESH_NO_EQUIL );
  KEY( "min_target_elements",    ERR::MINTARGETELEMS );        KEY( "mesh_out_of_equil",      ERR::MESH_OUT_OF_EQUIL );
  KEY( "wrong_type",             ERR::WRONGTYPE );             KEY( "del_marked_object_type", ERR::DEL_MARKED_OBJECT_TYPE );
  KEY( "join_connected",         ERR::JOINCONNECTED );         KEY( "mesh_element_dir",       ERR::MESH_ELEMENT_DIR );
  KEY( "bad_surface_origin",     ERR::BADSURFORIGIN );         KEY( "mesh_cell_find",         ERR::MESH_CELL_FIND );
  KEY( "twisted_sonnet",         ERR::TWISTEDSONNET );         KEY( "bad_object_type",        ERR::BAD_OBJECT_TYPE );
  KEY( "no_separators",          ERR::NOSEPARATORS );          KEY( "mesh_hdr_no_lf",         ERR::MESH_HDR_NO_LF );
  KEY( "sonnet_struct_ins",      ERR::SONNETSTRUCTINS );       KEY( "mesh_hdr_no_id",         ERR::MESH_HDR_NO_ID );
  KEY( "separator_order",        ERR::SEPARATORORDER );        KEY( "equil_spline",           ERR::EQUIL_SPLINE );

  KEY( "bad_surface_zone_no",    ERR::BADSURFZONENUMBER );
  KEY( "bad_gridpoint_zone_no",  ERR::BADGPZONENUMBER );
  KEY( "bad_gp_zone_lvl",        ERR::BADGPZONENUMBER_LVL );
  KEY( "fsopx1",                 ERR::FSOPX_1 );
  KEY( "out_of_surface_zone",    ERR::OUTOFSURFZONE );
  KEY( "bad_surface_zone_1",     ERR::BADSURFACEZONE1 );
  KEY( "no_surface_zone",        ERR::NOSURFZONE );
  KEY( "old_file_no_topology",   ERR::OLD_FILE_NO_TOPOLOGY );
  KEY( "target_crossed_2x",      ERR::TARGET_CROSSED_2X );
  KEY( "target_not_crossed",     ERR::TARGET_NOT_CROSSED );
  KEY( "surface_xy",             ERR::SURFACE_XY );
  KEY( "no_elements_in_path",    ERR::NOELEMSINPATH );

  GROUP( "msg/str" );

  KEY( "locked",                 STR::LOCKED );                KEY( "structure_file_name",    STR::STRUCTUREFNAME );
  KEY( "unlocked",               STR::UNLOCKED );              KEY( "targets_file_name",      STR::TARGETSFNAME );
  KEY( "error_label",            STR::ERRLABEL );              KEY( "help_changed",           STR::HELPCHANGED );
  KEY( "vsdefaults",             STR::VSDEFAULTS );            KEY( "enum_changed",           STR::ENUMCHANGED );
  KEY( "no_elements",            STR::NOELEMS );               KEY( "source",                 STR::SOURCE );
  KEY( "many_elements",          STR::MANYELEMS );             KEY( "irregular_normals",      STR::IRRNORMALS );
  KEY( "no_selection",           STR::NOSEL );                 KEY( "irregular_separators",   STR::IRRSEPARATORS );
  KEY( "diff",                   STR::DIFF );                  KEY( "irregular_too_many",     STR::IRRTOOMANY );
  KEY( "node",                   STR::NODE );                  KEY( "irregular_too_few",      STR::IRRTOOFEW );
  KEY( "element",                STR::ELEM );                  KEY( "regular",                STR::REGULAR );
  KEY( "surface",                STR::SURFACE );               KEY( "dsurface_area_base",     STR::DSURFAREABASE );
  KEY( "gridpoint",              STR::GRIDPOINT );             KEY( "dsurface_area_no_sign",  STR::DSURFAREANOSIGN );
  KEY( "separator",              STR::SEPARATOR );             KEY( "dsurface_area_1",        STR::DSURFAREA1 );
  KEY( "equilibrium",            STR::EQUIL );                 KEY( "dsurface_area_2",        STR::DSURFAREA2 );
  KEY( "template",               STR::TEMPLATE );              KEY( "dsurface_area_3",        STR::DSURFAREA3 );
  KEY( "sonnet",                 STR::SONNET );                KEY( "dgridpoint_area_base",   STR::DGRPOINTAREABASE );
  KEY( "xpoint",                 STR::XPOINT );                KEY( "dgridpoint_area_0",      STR::DGRPOINTAREA0 );
  KEY( "new_label",              STR::NEWLABEL );              KEY( "dgridpoint_area_1",      STR::DGRPOINTAREA1 );
  KEY( "removed",                STR::REMOVED );               KEY( "dgridpoint_area_2",      STR::DGRPOINTAREA2 );
  KEY( "added",                  STR::ADDED );                 KEY( "chord",                  STR::CHORD );
  KEY( "changed",                STR::CHANGED );               KEY( "xpointtest",             STR::XPOINTTEST );
  KEY( "vars_lost",              STR::VARSLOST );              KEY( "xpointseg",              STR::XPOINTSEG );
  KEY( "description_changed",    STR::DESCRCHANGED );          KEY( "mesh_cell_twisted",      STR::MESH_CELL_TWISTED );
  KEY( "surface_area_0",         STR::SURFAREA0 );             KEY( "mesh_cell_bad_center",   STR::MESH_CELL_BAD_CENTER );
  KEY( "surface_area_1",         STR::SURFAREA1 );             KEY( "mesh_element_cut",       STR::MESH_ELEMENT_CUT );
  KEY( "surface_area_2",         STR::SURFAREA2 );             KEY( "mesh_cell",              STR::MESH_CELL );
  KEY( "gridpoint_area_0",       STR::GRPOINTAREA0 );          KEY( "mesh_element",           STR::MESH_ELEMENT );
  KEY( "gridpoint_area_1",       STR::GRPOINTAREA1 );          KEY( "mesh_point",             STR::MESH_POINT );
  KEY( "gridpoint_area_2",       STR::GRPOINTAREA2 );          KEY( "mesh_cell_concave",      STR::MESH_CELL_CONCAVE );
  KEY( "output_file_name",       STR::OUTPUTFNAME );           KEY( "grid_segment",           STR::GRID_SEGMENT );
                                                               KEY( "surface_zone",           STR::SURFACE_ZONE );
  KEY( "var",                    STR::VAR );
  KEY( "vardef",                 STR::VARDEF );
  KEY( "varset",                 STR::VARSET );
  KEY( "varsetdef",              STR::VARSETDEF );


  GROUP( "msg/fstr" );

  KEY( "config_ext",            FSTR::CFGEXT );
  KEY( "output_ext",            FSTR::OUTPUTEXT );
  KEY( "structure_ext",         FSTR::STRUCTUREEXT );
  KEY( "target_ext",            FSTR::TARGETSEXT );
  KEY( "help_ext",              FSTR::HELPEXT );

  GROUP( "msg/que" );

  KEY( "with_geometry",          QUE::WITHGEOMETRY );
  KEY( "update_apps",            QUE::UPDATEAPPS );

  GROUP( "msg/wrn" );

  KEY( "no_equil",               WRN::NOEQUIL );
  KEY( "no_template",            WRN::NOTEMPL );
  KEY( "bad_type",               WRN::BADTYPE );
  KEY( "syntax",                 WRN::SYNTAX );
  KEY( "no_sonnet",              WRN::NOSONNET );
  KEY( "bad_target_1",           WRN::BADTARG1 );
  KEY( "bad_target_2",           WRN::BADTARG2 );
  KEY( "bad_structure",          WRN::BADSTRUC );
  KEY( "bad_xpointtest",         WRN::BADXPT );
  KEY( "mesh_fp",                WRN::MESH_FP );
  KEY( "old_topology",           WRN::OLDTOPO );
  KEY( "old_surfaces_lost",      WRN::OLD_SURFACES_LOST );

  GROUP( "msg/env" );

  KEY( "config_file",            ENV::CFGFILE );
  KEY( "load_mask",              ENV::LOADMASK );
  KEY( "save_mask",              ENV::SAVEMASK );
  KEY( "equil_mask",             ENV::EQUILMASK );
  KEY( "template_mask",          ENV::TEMPLATEMASK );
  KEY( "sonnet_mask",            ENV::SONNETMASK );
  KEY( "vars_file_set_mask",     ENV::VARSFILESETMASK );
  KEY( "config_file_2",          ENV::CFGFILE2 );
  KEY( "load_mask_2",            ENV::LOADMASK2 );
  KEY( "save_mask_2",            ENV::SAVEMASK2 );
  KEY( "equil_mask_2",           ENV::EQUILMASK2 );
  KEY( "template_mask_2",        ENV::TEMPLATEMASK2 );
  KEY( "sonnet_mask_2",          ENV::SONNETMASK2 );

  GROUP( "msg/dg3" );

  KEY( "wait",      DG3::WAIT );

  KEY( "done",      DG3::DONE );
  KEY( "fail",      DG3::FAIL );

  KEY( "enabled",   DG3::ENABLE );
  KEY( "disabled",  DG3::DISABLE );

  KEY( "mode_on",   DG3::MODE_ON );
  KEY( "mode_off",  DG3::MODE_OFF );

  KEY( "dg_file_syntax_error",  DG3::DG_FILE_SYNTAX_ERROR );

  KEY( "saving_dg_file",        DG3::SAVING_DG_FILE );
  KEY( "saving_mesh_file",      DG3::SAVING_MESH_FILE );
  KEY( "writing_output_file",   DG3::WRITING_OUTPUT_FILE );
  KEY( "writing_targets_file",  DG3::WRITING_TARGETS_FILE );
  KEY( "writing_templ_file",    DG3::WRITING_TEMPLATE_FILE );
  KEY( "writing_struct_file",   DG3::WRITING_STRUCTURE_FILE );
  KEY( "writing_equil_file",    DG3::WRITING_EQUIL_FILE );
  KEY( "writing_spline_file",   DG3::WRITING_SPLINE_FILE );

  KEY( "loading_dg_file",       DG3::LOADING_DG_FILE );
  KEY( "loading_mesh_file",     DG3::LOADING_MESH_FILE );
  KEY( "loading_templ_file",    DG3::LOADING_TEMPLATE_FILE );
  KEY( "loading_equil_file",    DG3::LOADING_EQUIL_FILE );
  KEY( "loading_topology_file", DG3::LOADING_TOPOLOGY_FILE );
  KEY( "loading_sonnetdata",    DG3::LOADING_SONNETDATA );

  KEY( "actions_reverted",      DG3::ACTIONS_REVERTED );
  KEY( "new_model_created",     DG3::NEW_MODEL_CREATED );
  KEY( "model_load_success",    DG3::MODEL_LOADED_SUCCESSFULLY );
  KEY( "model_load_errors",     DG3::MODEL_LOADED_WITH_ERRORS );
  KEY( "model_renamed",         DG3::MODEL_RENAMED );
  KEY( "elems_renumbered",      DG3::ELEMENTS_RENUMBERED );
  KEY( "topology_success",      DG3::TOPOLOGY_LD_SUCCESSFULLY );
  KEY( "topology_errors",       DG3::TOPOLOGY_LD_WITH_ERRORS );

  KEY( "view_created",          DG3::VIEW_CREATED );
  KEY( "view_closed",           DG3::VIEW_CLOSED );
  KEY( "view_scale_changed",    DG3::VIEW_SCALE_CHANGED );
  KEY( "view_angle_changed",    DG3::VIEW_ANGLE_CHANGED );
  KEY( "view_stretch_mode",     DG3::VIEW_STRETCH_MODE );
  KEY( "view_radial_grid",      DG3::VIEW_RADIAL_GRID );
  KEY( "view_colored_equil",    DG3::VIEW_COLORED_EQUIL );
  KEY( "view_scene",            DG3::VIEW_SCENE );
  KEY( "view_center_changed",   DG3::VIEW_CENTER_CHANGED );
  KEY( "view_state",            DG3::VIEW_STATE );

  KEY( "wrong_undo_mode",       DG3::WRONG_UNDO_MODE );
  KEY( "bad_file_name",         DG3::BAD_FILE_MODE );
  KEY( "wrong_var_type",        DG3::WRONG_VAR_OBJECT_TYPE );
  KEY( "unknown_type",          DG3::UNKNOWN_OBJECT_TYPE );
  KEY( "wrong_varset",          DG3::WRONG_VARSET );
  KEY( "multiple_vardefs",      DG3::MULTIPLE_VARDEFS );
  KEY( "same_nodes",            DG3::SAME_NODES );
  KEY( "nodes_connected",       DG3::NODES_ALREADY_CONNECTED );
  KEY( "model_is_locked",       DG3::MODEL_IS_LOCKED );
  KEY( "negative_updatelocks",  DG3::NEGATIVE_UPDATELOCKS );
  KEY( "object_is_locked",      DG3::OBJECT_IS_LOCKED );
  KEY( "unknown_origin_type",   DG3::UNKNOWN_ORIGIN_TYPE );
  KEY( "sonnetdata_exists",     DG3::ALREADY_HAS_SONNETDATA );
  KEY( "cannot_extend_hchord",  DG3::CANNOT_EXTEND_HRZ_CHORD );
  KEY( "level_problem",         DG3::LEVEL_PROBLEM );
  KEY( "line_is_empty",         DG3::LINE_IS_EMPTY );
  KEY( "value_out_of_range",    DG3::VALUE_OUT_OF_RANGE );
  KEY( "object_highlighted",    DG3::OBJECT_IS_HIGHLIGHTED );
  KEY( "wrong_node_state",      DG3::WRONG_NODE_STATE );
  KEY( "too_many_varsets",      DG3::TOO_MANY_VARSETS );
  KEY( "locking_object_err",    DG3::LOCKING_OBJECT_ERROR );
  KEY( "wrong_varsets_num",     DG3::WRONG_VARSETS_NUMBER );
  KEY( "node_has_elements",     DG3::NODE_HAS_ELEMENTS );
  KEY( "node_has_separators",   DG3::NODE_HAS_SEPARATORS );
  KEY( "bad_surface",           DG3::BAD_SURFACE );
  KEY( "mp_chain_too_short",    DG3::MP_CHAIN_IS_TOO_SHORT );
  KEY( "group_error",           DG3::GROUP_ERROR );
  KEY( "no_topology",           DG3::NO_TOPOLOGY );
  KEY( "null_pointers",         DG3::NULL_POINTERS );

  KEY( "comment",               DG3::COMMENT );

  GROUP( "msg/dg3/act" );

  KEY( "added",                 DG3::ACT_ADDED );
  KEY( "deleted",               DG3::ACT_DELETED );
  KEY( "changed",               DG3::ACT_CHANGED );
  KEY( "undomark",              DG3::ACT_UNDOMARK );
  KEY( "group",                 DG3::ACT_GROUP );
  KEY( "include",               DG3::ACT_INCLUDE );
  KEY( "exclude",               DG3::ACT_EXCLUDE );
  KEY( "marked",                DG3::ACT_MARKED );
  KEY( "mark",                  DG3::ACT_MARK );
  KEY( "unmark",                DG3::ACT_UNMARK );
  KEY( "highlighted",           DG3::ACT_HIGHLIGHTED );
  KEY( "highlight",             DG3::ACT_HIGHLIGHT );
  KEY( "unhighlight",           DG3::ACT_UNHIGHLIGHT );
}
