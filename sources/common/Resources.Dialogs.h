#ifndef CONSTANTS_DIALOG_H
#define CONSTANTS_DIALOG_H

#define APPUNSAVED APP_UNSAVED

#include "Resources.Macro.h"

#define ENUM_DIALOG( name, base ) \
  ENUM_STRUCT( name ) DIALOG = (base), TITLE, HELP,

#define ENUM_FILEDIALOG( name, base ) \
  ENUM_DIALOG( name, base ) FILTER END_ENUM_STRUCT

#define ENUM_QUESTDIALOG( name, base ) \
  ENUM_DIALOG( name, base ) MESSAGE END_ENUM_STRUCT


namespace DLG {
  ENUM_STRUCT( BTN )
    OK = 100, CREATE, OPEN, SAVE, LOAD, IMPORT, EXPORT, YES, PROCEED, APPLY,
    CANCEL, CLOSE, DISCARD, NO, HELP, BROWSE
  END_ENUM_STRUCT

  ENUM_STRUCT( MSG )
    NORMALS_GROUPED
  END_ENUM_STRUCT

  /****************************************************************************
   *  COMMON
   ****************************************************************************/
  ENUM_DIALOG(      MODEL_CLOSE,    1000 )
    NOVIEWS, UNSAVED, DONOTCLOSE, SAVEANDCLOSE, CLOSEUNSAVED /*, CLOSE*/
  END_ENUM_STRUCT

  ENUM_DIALOG(      FILE_OVERWRITE, 2000 )
    OVERWRITE, SELECTANOTHER /*, CANCEL*/
  END_ENUM_STRUCT

  ENUM_DIALOG(      MODEL_OPEN,     3000 )
    MESSAGE, SAME_NAME
  END_ENUM_STRUCT

  ENUM_QUESTDIALOG( SESSION,        4000 )

  ENUM_DIALOG(      NEW_MODEL,      5000 )
    NO_CONFIG, USE_CONFIG
  END_ENUM_STRUCT

  /****************************************************************************
   *   MENU > FILE
   ****************************************************************************/
  ENUM_QUESTDIALOG( APP_UNSAVED,  101000 ) // dlgAppUnsaved
  ENUM_QUESTDIALOG( MESH_UNSAVED, 102000 ) // dlgMeshUnsaved

  ENUM_FILEDIALOG(  FILEOPEN,     103000 ) // dlgFileOpen
  ENUM_FILEDIALOG(  FILESAVE,     104000 ) // dlgFileSave
  ENUM_FILEDIALOG(  EQUIL,        105000 ) // dlgEquil
  ENUM_FILEDIALOG(  TEMPLATE,     106000 ) // dlgTemplate
  ENUM_FILEDIALOG(  SONNET,       107000 ) // dlgSonnet

  /* dlgExportMesh */
  ENUM_DIALOG(      EXPORT_MESH,  108000 )
    FILTER, MESSAGE
  END_ENUM_STRUCT

  /* dlgExportElemsAsTemplate */
  ENUM_DIALOG( EXPORT_ELEMS_TEMPLATE, 110000 )
    FILTER, MESSAGE
    ENUM( LBL,      111100 ) ALL, MARKED    END_ENUM
  END_ENUM_STRUCT

  /* dlgPrint */
  ENUM_DIALOG(   PRINT, 112000 )
    ENUM( LBL,      112100 ) COMMAND, FILE, WIDTH, HEIGHT, MARGIN, FIT  END_ENUM
    ENUM( BTN,      112200 ) PRINT END_ENUM
  END_ENUM_STRUCT

  /* dlgImportTopology */
  ENUM_DIALOG( IMPORTTOPO,  113000 )
    FILTER, MESSAGE
    ENUM( LBL,      113100 ) DETECT_XPNT    END_ENUM
  END_ENUM_STRUCT


  /****************************************************************************
   *   MENU > EDIT
   ****************************************************************************/

  /* dlgCreateNode */
  ENUM_DIALOG( CREATE_NODE, 201000 )
    ENUM( LBL,      201100 ) X, Y END_ENUM
  END_ENUM_STRUCT

  /* dlgCreateSource */
  ENUM_DIALOG( CREATE_SOURCE, 202000 )
    ENUM( LBL,      202100 ) X, Y END_ENUM
  END_ENUM_STRUCT

  /* dlgCreateSurface */
  ENUM_DIALOG(  CREATE_SURFACE, 203000 )
    ENUM( MULTIPLE, 203100 )
      ENUM( LBL,    203110 ) LEVEL1, LEVEL2, REMOVE_OLD   END_ENUM
      ENUM( BTN,    203120 ) COPY, MARK_BOUND_ELEM        END_ENUM
    END_ENUM

    ENUM( SINGLE,   203200 ) BY_LEVEL, BY_POINT, LEVEL, X, Y  END_ENUM
    ENUM( AREA,     203300 ) AREA1, AREA2, AREA3              END_ENUM
    ENUM( ERR,      203400 )
      EQUAL_LEVELS, NO_REMEMBERED_SURFACE, BAD_CREATOR_ID,
      OLD_SURFACES_NOT_FOUND, CARRE_OLD_SURFACES, NON_CARRE_MODE
    END_ENUM
  END_ENUM_STRUCT

  /* dlgCreateGridPoint */
  ENUM_DIALOG( CREATE_GPOINT, 204000 )
    ENUM( MULTIPLE, 204100 )                      END_ENUM
    ENUM( SINGLE,   204200 ) VALUE                END_ENUM
    ENUM( AREA,     204300 ) AREA0, AREA1, AREA2  END_ENUM
    ENUM( ERR,      204400 )
      NO_REMEMBERED_GRIDPOINT, BAD_CREATOR_ID, NON_CARRE_MODE
    END_ENUM
  END_ENUM_STRUCT

  /* distributeForm */
  ENUM_STRUCT( DISTR_GRAPH )
    ENUM( LBL,      204600 ) COUNT, A1, A2, D1, D2          END_ENUM
    ENUM( BTN,      204700 ) COPY, RESET                    END_ENUM
    ENUM( LAW,      204800 ) NORM, FLIP, SYMM, TWOA, DELTA  END_ENUM
    ENUM( ERR,      204900 )
      COUNT_BAD_RANGE, A1_BAD_RANGE, A2_BAD_RANGE, BAD_LAW, WRONG_CARRE_LAW
    END_ENUM
  END_ENUM_STRUCT

  /* dlgCreateChord */
  ENUM_DIALOG( CREATE_CHORD, 205000 )
    ENUM( CARTESIAN,    205100 ) X1, Y1, Z1, X2, Y2, Z2     END_ENUM
    ENUM( CYLINDRICAL,  205200 ) R1, Z1, PHI1, R2, Z2, PHI2 END_ENUM
  END_ENUM_STRUCT


  /****************************************************************************
   *   MENU > VIEW
   ****************************************************************************/

  /* dlgRotMove */
  ENUM_DIALOG( ROTMOVE, 301000 )
    ENUM( LBL,    301100 ) X, Y, SCALE, ELEMS, TEMPLATE END_ENUM
    ENUM( BTN,    301200 ) MOVE, ROTATE                 END_ENUM
    ENUM( ERR,    301200 ) BAD_SCALE                    END_ENUM
  END_ENUM_STRUCT

  /* dlgStretch */
  ENUM_DIALOG( STRETCH, 302000 )
    ENUM( LBL,    302100 ) X, Y   END_ENUM
    ENUM( BTN,    302200 ) RESET  END_ENUM
  END_ENUM_STRUCT

  /* dlgSetAngle */
  ENUM_DIALOG( SETANGLE, 303000 )
  ENUM( LBL,    303100 ) ANGLE    END_ENUM
  END_ENUM_STRUCT

  /* dlgEditMeshHeader */
  ENUM_DIALOG( EDIT_MESH_HEADER, 304000 )
    ENUM( BTN,    304100 ) CHANGE  END_ENUM
  END_ENUM_STRUCT


  /****************************************************************************
   *   MENU > COMMANDS
   ****************************************************************************/

  /* dlgGluePoints */
  ENUM_DIALOG( GLUEPOINTS, 401000 )
    ENUM( LBL,    401100 ) MAX_DIST, MILLIMETERS, MARKED_ONLY END_ENUM
    ENUM( ERR,    401200 ) BAD_MAX_DIST                       END_ENUM
    ENUM( MSG,    401300 ) ELEMS_ADDED                        END_ENUM
  END_ENUM_STRUCT

  /* dlgGlueElems */
  ENUM_DIALOG( GLUEELEMS, 402000 )
    ENUM( LBL,    402100 ) TRESHOLD, MILLIMETERS, MARKED_ONLY,
                           MAX_LEN, CUT_LONGER                END_ENUM
    ENUM( ERR,    402200 ) BAD_MAX_DIST, BAD_MAX_LEN          END_ENUM
    ENUM( MSG,    402300 ) ELEMS_REMOVED                      END_ENUM
  END_ENUM_STRUCT

  /* dlgTopology */
  ENUM_DIALOG(  TOPOLOGY, 403000 )
    ENUM(       XPOINTS,  403100 )
      ENUM( LBL,          403110 ) XPT_LEVEL          END_ENUM
      ENUM( BTN,          403120 ) DETECT_ALL, CHANGE END_ENUM
    END_ENUM

    ENUM(       SECTIONS, 403200 )
      ENUM( LBL,          403210 ) ID, SHORT, LONG, USED, TARGETCW  END_ENUM
      ENUM( BTN,          403220 ) DETECT_ALL, CHANGE, REVERT       END_ENUM
    END_ENUM

    ENUM(       ZONES,    403300 )
      ENUM( LBL,          403310 ) ID, SHORT, LONG, GPS1, GPS2,
                                   ORIENT, LEFT, RIGHT, CLOSED  END_ENUM
      ENUM( BTN,          403320 ) SET, ADD, DELETE             END_ENUM
    END_ENUM

    ENUM(       NAME,     403400 )
      ENUM( LBL,          403410 )      END_ENUM
      ENUM( BTN,          403420 ) SET  END_ENUM
    END_ENUM
    //TODO: Differ errors by tabs?
    ENUM( ERR,            403500 )
      CUR_OBJ_GONE, BAD_XPT_LEVEL, BAD_NEW_LEVEL, BAD_XPS_ZONE, DUP_XPS_ZONE,
      BAD_SZONE, DUP_SZONE, SZ_BAD_GP_ZONE, SZ_NOT_FOIND_GP_ZONE, SZ_BAD_SIGN,
      BAD_TOPO_NAME
    END_ENUM

    ENUM( MSG,            403600 )
      XPT_LEVEL_ADJUSTED, XSEG_CHANGED, XSEG_REVERTED, SZ_CHANGED,
      TOPO_NAME_CHANGED
    END_ENUM
  END_ENUM_STRUCT


  /****************************************************************************
   *   MENU > VARIABLES
   ****************************************************************************/

  /* dlgVarsEdit */
  ENUM_DIALOG( VARSEDIT, 501000 )
    ENUM( LBL,        501100 ) HOLD, MESSAGE, OLD_VAL, NEW_VAL,
                               OBJECT_MATCHING                        END_ENUM
    ENUM( BTN,        501200 ) SET_ALL, RESET_ALL, CLOSE, HELP, MARK, SET,
                               VAR_HELP, SET_FILE                     END_ENUM
    ENUM( ERR,        501300 ) BAD_SCOPE, BAD_NUMBER                  END_ENUM
    ENUM( VARS_POPUP, 501400 ) RESET, RESET_HELP, COMPARE, DISPLAY,
                               DISPLAY_HELP, VALUES, VALUES_HELP,
                               HELP, HELP_HELP                        END_ENUM
    ENUM( COMPARE,    501500 ) EQUAL, NOT_EQUAL, GT, GE, LT, LE, HELP END_ENUM
  END_ENUM_STRUCT

  /* dlgVarsFileSet */
  ENUM_FILEDIALOG( VARSFILESET, 502000 )

  /* dlgInvalidVars */
  ENUM_DIALOG( INVALIDVARS, 503000 )
    ENUM( BTN,        503100 ) SHOW END_ENUM
  END_ENUM_STRUCT


  /****************************************************************************
   *   MENU > SETUP
   ****************************************************************************/

  /* dlgVarSetDefList */
  ENUM_DIALOG( VARSETDEFLIST, 601000 )
    ENUM( BTN,            601100 ) ADD, MODIFY, REMOVE, CANCEL  END_ENUM
  END_ENUM_STRUCT

  /* dlgVarSetDefCreate */
  ENUM_DIALOG( VARSETDEFCREATE, 602000 )
    ENUM( LBL,            602200 ) NAME END_ENUM
  END_ENUM_STRUCT

  /* dlgVarSetDefEdit */
  ENUM_DIALOG( VARSETDEFEDIT, 603000 )
    TITLE_EX
    ENUM( LBL,            603100 ) DESCR, MIN, MAX, EMPTY END_ENUM
  END_ENUM_STRUCT

  /* dlgVarDefCreate */
  ENUM_DIALOG( VARDEFCREATE, 604000 )
    TITLE_EX
    ENUM( LBL,            604200 ) NAME END_ENUM
  END_ENUM_STRUCT

  /* dlgVarDefEdit */
  ENUM_DIALOG( VARDEFEDIT, 605000 )
    TITLE_EX

    ENUM( TYPE,           605100 )
      INT, FLOAT, TEXT, FILENAME, ELEMENT, ELEMSET, TARGET1, TARGET2,
      STRUCTURE, STRUCTPART, CHORDS, TOPVIEW, MESH_CELLS, MESH_ELEMS,
      MESH_HELEMS, MESH_VELEMS
    END_ENUM

    ENUM( SCOPE,          605200 )
      WITH_VARSET, WITH_ELEMS, WITH_SOURCES, WITH_CHORDS
    END_ENUM

    ENUM( LBL,            605300 ) DESCR, DEFAULT, NO_EXPORT, LAYER END_ENUM
    ENUM( BTN,            605400 ) EDIT_HELP, REMOVE                END_ENUM
    ENUM( ERR,            605500 ) SECOND, MULTIPLE, GROUP          END_ENUM
  END_ENUM_STRUCT

  /* dlgSaveConfig */
  ENUM_QUESTDIALOG( SAVECONFIG, 606000 )

  /* dlgSetupWarning */
  ENUM_QUESTDIALOG( SETUPWARNING, 607000 )

  /* dlgHelpEdit */
  ENUM_DIALOG( HELPEDIT, 608000 ) TITLE_EX END_ENUM_STRUCT

  /* dlgOutputMode */
  ENUM_DIALOG( OUTPUTMODE, 609000 )
    ENUM( LBL,            609100 ) SONNET, CARRE, VARS, STRUCTURE, TARGETS,
                                   SURFACES, GRIDPOINTS, CELLS, FLAGS  END_ENUM
  END_ENUM_STRUCT

  /* dlgMeshOptions */
  ENUM_DIALOG( MESHOPTIONS, 610000 )
    ENUM( MODE,           610100 ) SURFACES, SPLINES    END_ENUM
    ENUM( LBL,            610200 ) SLIDE, BORDER        END_ENUM
    ENUM( ERR,            610300 ) BAD_SLIDING_TRESHOLD END_ENUM
  END_ENUM_STRUCT

  /* dlgSaveInterval */
  ENUM_DIALOG( SAVE_INTERVAL, 611000 )
    ENUM( LBL,            611100 ) SAVE       END_ENUM
  END_ENUM_STRUCT

  /* dlgSettings (dg3) */
  ENUM_DIALOG( SETTINGS,  612000 )
    ENUM( COMMON,         612100 ) SAVEINTERVAL, RECENT_LENGTH, AXISLABELS,
                                   EXTMODEL, EXTTEMPL, EXTTOPO, EXTEQUIL, EXTMESH
    END_ENUM

    ENUM( MESH,           612200 )
      ENUM( MODE,         612100 ) SURFACES, SPLINES    END_ENUM
      ENUM( LBL,          612200 ) SLIDE, BORDER        END_ENUM
      ENUM( ERR,          612300 ) BAD_SLIDING_TRESHOLD END_ENUM
    END_ENUM

    ENUM( VISUAL,         613000 )  COLOR, WIDTH, RADIUS, ERROR, FONTSIZE,
                                    LABEL, TOOL, OFFSET, X, Y, BACKGROUND,
                                    SONNETDATA, SEPARATOR, SOURCE,
                                    CHORD, TEMPLATE, SURFACE, XPOINTSEG,
                                    ARROW_SIZE, USESQUARES, COMMENT
      ENUM( GRID_AXES,    613100 ) AXES, GRID, TEXT     END_ENUM
      ENUM( ELEM_NODE,    613200 ) NODE, ELEM           END_ENUM
      ENUM( EQUIL,        613300 ) POS, NEG             END_ENUM
      ENUM( MESH,         613400 ) ELEMENT, IRREG, IRREG_HL,
                                   CELLCENTER, CCLENGTH END_ENUM
      ENUM( XGPOINT,      613500 ) XPOINT, GPOINT, GPOINTLEN END_ENUM
      ENUM( XPOINTTEST,   613600 ) XPT, XPT_BOUND       END_ENUM
      ENUM( MARK_HL,      613700 ) MARK, HL             END_ENUM
      ENUM( TEXT_NORM,    613800 ) TEXT, NORMAL, NORM_LEN END_ENUM
    END_ENUM
  END_ENUM_STRUCT

  /* dlgCommentEdit */
  ENUM_DIALOG( COMMENTEDIT, 614000 )
    LINKED_OBJECT, INSERT_DESCR
  END_ENUM_STRUCT

  /* dlgProjectSettings */
  ENUM_DIALOG( PRJSETTINGS, 615000 )
    ENUM( TOPOLOGY,         615100 )
      ENUM( RESTRICTB2,       615110 ) B2CARRE, B2EIRENE END_ENUM
      ENUM( RESTRICT,         615120 ) TARGETS, TCHAINS, TPOINTS END_ENUM
    END_ENUM
  END_ENUM_STRUCT

  /****************************************************************************
   *   MENU > HELP
   ****************************************************************************/

  /* dlgAbout */
  ENUM_DIALOG( ABOUT, 701000 ) TEXT END_ENUM_STRUCT

  /* dlgInfo */
  ENUM_DIALOG( INFO, 702000 )
    ENUM( LBL,        702100 ) NODES, ELEMS, UNUSEDNUM, SOURCES, SURFACES,
                               GRIDPOINTS, SEPARATORS, CHORDS   END_ENUM
  END_ENUM_STRUCT

  /* dlgNewPrefsFile */
  ENUM_DIALOG( NEWPREFSFILE, 703000 ) TEXT END_ENUM_STRUCT

  /* dlgNewVersion */
  ENUM_DIALOG( NEWVERSION, 704000 )
    TEXT
    ENUM( BTN, 704100 ) WIN END_ENUM
  END_ENUM_STRUCT

  /* dlgHelp */
  ENUM_DIALOG( DGHELP, 705000 ) END_ENUM_STRUCT
  ENUM_DIALOG( TOOLBOX, 706000 ) END_ENUM_STRUCT
}

#endif // CONSTANTS_DIALOG_H
