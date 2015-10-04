/* Error & information messages */

#ifndef _msg_h
#define _msg_h

#include "Resources.Macro.h"

ENUM_STRUCT( DGMSG )
  OK                 = 0000,    SOURCESREMOVED         = 1025,
  EXAMNODE           = 1000,    SURFACESREMOVED        = 1026,
  EXAMELEM           = 1001,    GPOINTSREMOVED         = 1027,
  EXAMSURFACE        = 1002,    ABOUT                  = 1028,
  EXAMGRIDPOINT      = 1003,    EXAMCHORD              = 1029,
  TEMPLATELOADED     = 1004,    CHORDSREMOVED          = 1030,
  TEMPLATEREMOVED    = 1005,    ALLVARSVALID           = 1031,
  EQUILLOADED        = 1006,    SOMEVARSINVALID        = 1032,
  EQUILREMOVED       = 1007,    FILEPRINTED            = 1033,
  NEWFILE            = 1008,    WORKING                = 1034,
  FILEOPENED         = 1009,    XPOINTSFOUND           = 1035,
  FILESAVED          = 1010,    XPSEGSFOUND            = 1036,
  SONNETLOADED       = 1011,    REPOS_HANDLE_1         = 1037,
  SONNETREMOVED      = 1012,    REPOS_HANDLE_2         = 1038,
  XPOINTREMOVED      = 1013,    MARKED_REMOVED         = 1039,
  TEMPLAPPENDED      = 1014,    EXAM_MESH_CELL         = 1040,
  EXAMSEPARATOR      = 1015,    EXAM_MESH_ELEMENT      = 1041,
  SEPARATORSADDED    = 1016,    EXAM_MESH_POINT        = 1042,
  SEPARATORSREMOVED  = 1017,    EXAM_XPOINTTEST        = 1043,
  RENUMBERED         = 1018,    EXAM_XPOINTSEG         = 1044,
  OUTPUT             = 1019,    EXAM_SURFACE_XY        = 1045,
  EXAMSOURCE         = 1020,    CALCULATING_SZ_BOUNDS  = 1046,
  CANCELED           = 1021,    FILEAUTOSAVED          = 1047,
  VERSION            = 1022,    NOBOUNDINGELEMS        = 1048,
  EMPTYNODESREMOVED  = 1023,    NOMARKEDCHORDS         = 1049,
  ELEMSREMOVED       = 1024
END_ENUM_STRUCT

ENUM_STRUCT( ERR )
  FILENOTFOUND      =  1,    BADGRIDPOINTS   = 41,    CARRE_IGPOINTS          =  81,
  BADFILE           =  2,    NOFNAME         = 42,    CARRE_PEAK              =  82,
  BADFILETYPE       =  3,    NOSELECTION     = 43,    CARRE_INHIBITS          =  83,
  OUTOFEQUIL        =  4,    MINTARGETELEMS  = 44,    CARRE_EQUALLEVELS       =  84,
  BADTARGETS        =  5,    WRONGTYPE       = 45,    CARRE_AREA1SURFACES     =  85,
  NOTARGETS         =  6,                             CARRE_OUTOFEQUIL        =  86,
  BADXPOINT         =  7,                             CARRE_MODE_NEEDED       =  87,
  BADFILEVERSION    =  8,                             CARRE_BROKENTARGET      =  88,
  USEDBY            =  9,                             CARRE_TARGETXSTRUCTURE  =  89,
  NOVIEW            = 10,    JOINCONNECTED   = 50,    NO_MARKED_ELEMS         =  90,
  NOXPOINT          = 11,    BADSURFORIGIN   = 51,    NO_MARKED_CHORDS        =  91,
  IRREGULAR         = 12,    TWISTEDSONNET   = 52,    EMPTY_TEMPLATE          =  92,
  NOEQUIL           = 13,    NOSEPARATORS    = 53,    REPOS_TEMPLATE_1        =  93,
  NONODES           = 14,    SONNETSTRUCTINS = 54,    REPOS_TEMPLATE_2        =  94,
  NOELEMS           = 15,    SEPARATORORDER  = 55,    FLAT_EQUIL              =  95,
  INVNUMBERS        = 16,    INTERNALCELLS   = 56,    DUP_MESH_ELNO           =  96,
  MINMAXVARSETS     = 17,    BADCELLS        = 57,    MOVE_CUT_MESH_PT        =  97,
  BADNAME           = 18,    SURFTARGETINS   = 58,    MESH_NO_EQUIL           =  98,
  DUPNAME           = 19,    BADSTRETCHTYPE  = 59,    MESH_OUT_OF_EQUIL       =  99,
  CLOSED            = 20,    NONEMPTYNODE    = 60,    DEL_MARKED_OBJECT_TYPE  = 100,
  BROKEN            = 21,    NOTSTRUCTPART   = 61,    MESH_ELEMENT_DIR        = 101,
  FWRITE            = 22,    INTERNALGP      = 62,    MESH_CELL_FIND          = 102,
  NOSTRUCTURE       = 23,    NOHELP          = 63,    BAD_OBJECT_TYPE         = 103,
  NOCLOSEDSURFS     = 24,    NOTOPIC         = 64,    MESH_HDR_NO_LF          = 104,
  TEMPLATEINCLUDED  = 25,    BADMASK         = 65,    MESH_HDR_NO_ID          = 105,
  NOTEMPLATE        = 26,    BADSTRUCTURE    = 66,    EQUIL_SPLINE            = 106,
  SINGLE            = 27,    MULTIPLEERRORS  = 67,
  MANYSELELEMS      = 28,    PIPECREATE      = 68,
  MULTGROUP         = 29,    FORK            = 69,
  NOSONNET          = 30,    PSVIEWSIZE      = 70,
  NOCONFIG          = 31,    BADXPOINTSEG    = 71,
  NOTHINGTODO       = 32,    LOOPNOTFOUND    = 72,
  ALREADYEXISTS     = 33,    LEVELNOTINAREA  = 73,
  INVAREA           = 34,    LEVELNOTFOUND   = 74,
  LOCKED            = 35,    NOPREFS         = 75,
  NOSAVECONFIG      = 36,    CARRE_SURFACES  = 76,
  EMPTYVARS         = 37,    CARRE_XSURFACES = 77,
  EMPTYTARGETS      = 38,    CARRE_TANGENT   = 78,
  EMPTYSTRUCTURE    = 39,    CARRE_GPOINTS   = 79,
  BADSURFACES       = 40,    CARRE_ISURFACES = 80,

  /* in FindSurfaceOriginPointEx() */
  BADSURFZONENUMBER     = 107,
  BADGPZONENUMBER       = 108,
  FSOPX_1               = 109,
  OUTOFSURFZONE         = 110,
  BADSURFACEZONE1       = 111,
  /* In GetSurfaceZoneByXY */
  NOSURFZONE            = 112,
  /* In LoadTopology() */
  OLD_FILE_NO_TOPOLOGY  = 113,
  /* In WriteTargetsFile() */
  TARGET_CROSSED_2X     = 114,
  TARGET_NOT_CROSSED    = 115,
  SURFACE_XY            = 116,
  /* In ExtendChord */
  NOELEMSINPATH         = 117,

  BADGPZONENUMBER_LVL   = 118,

END_ENUM_STRUCT

ENUM_STRUCT( STR )
  LOCKED       = 2000,  SURFAREA0        = 2023,  DGRPOINTAREABASE      = 2044,
  UNLOCKED     = 2001,  SURFAREA1        = 2024,  DGRPOINTAREA0         = 2044,
  ERRLABEL     = 2002,  SURFAREA2        = 2025,  DGRPOINTAREA1         = 2045,
  VSDEFAULTS   = 2003,  GRPOINTAREA0     = 2026,  DGRPOINTAREA2         = 2046,
  NOELEMS      = 2004,  GRPOINTAREA1     = 2027,
  MANYELEMS    = 2005,  GRPOINTAREA2     = 2028,  CHORD                 = 2047,
  NOSEL        = 2006,  OUTPUTFNAME      = 2029,  XPOINTTEST            = 2048,
  DIFF         = 2007,  STRUCTUREFNAME   = 2030,  XPOINTSEG             = 2049,
  NODE         = 2008,  TARGETSFNAME     = 2031,
  ELEM         = 2009,  HELPCHANGED      = 2032,  MESH_CELL_TWISTED     = 2050,
  SURFACE      = 2010,  ENUMCHANGED      = 2033,  MESH_CELL_BAD_CENTER  = 2051,
  GRIDPOINT    = 2011,  SOURCE           = 2034,  MESH_ELEMENT_CUT      = 2052,
  SEPARATOR    = 2012,  IRRNORMALS       = 2035,  MESH_CELL             = 2053,
  EQUIL        = 2013,  IRRSEPARATORS    = 2036,  MESH_ELEMENT          = 2054,
  TEMPLATE     = 2014,  IRRTOOMANY       = 2037,  MESH_POINT            = 2055,
  SONNET       = 2015,  IRRTOOFEW        = 2038,  MESH_CELL_CONCAVE     = 2056,
  XPOINT       = 2016,  REGULAR          = 2039,  GRID_SEGMENT          = 2100,
  NEWLABEL     = 2017,                            SURFACE_ZONE          = 2101,
  REMOVED      = 2018,  DSURFAREABASE    = 2040,  VAR                   = 2102,
  ADDED        = 2019,  DSURFAREANOSIGN  = 2040,  VARSET                = 2103,
  CHANGED      = 2020,  DSURFAREA1       = 2041,  VARDEF                = 2104,
  VARSLOST     = 2021,  DSURFAREA2       = 2042,  VARSETDEF             = 2105,
  DESCRCHANGED = 2022,  DSURFAREA3       = 2043
END_ENUM_STRUCT

ENUM_STRUCT( FSTR )
  CFGEXT       = 3007,
  OUTPUTEXT    = 3008,
  STRUCTUREEXT = 3009,
  TARGETSEXT   = 3010,
  HELPEXT      = 3011
END_ENUM_STRUCT

ENUM_STRUCT( QUE )
  WITHGEOMETRY = 4000,
  UPDATEAPPS   = 4001
END_ENUM_STRUCT

ENUM_STRUCT( WRN )
  NOEQUIL           = 5001,
  NOTEMPL           = 5002,
  BADTYPE           = 5003,
  SYNTAX            = 5004,
  NOSONNET          = 5005,
  BADTARG1          = 5006,
  BADTARG2          = 5007,
  BADSTRUC          = 5008,
  BADXPT            = 5009,
  MESH_FP           = 5010,
  OLDTOPO           = 5011,
  OLD_SURFACES_LOST = 5012
END_ENUM_STRUCT

ENUM_STRUCT( ENV )
  CFGFILE         = 6000,
  LOADMASK        = 6001,
  SAVEMASK        = 6002,
  EQUILMASK       = 6003,
  TEMPLATEMASK    = 6004,
  SONNETMASK      = 6005,

  CFGFILE2        = 6006,
  LOADMASK2       = 6007,
  SAVEMASK2       = 6008,
  EQUILMASK2      = 6009,
  TEMPLATEMASK2   = 6010,
  SONNETMASK2     = 6011,

  VARSFILESETMASK = 6012
END_ENUM_STRUCT

/* Model errors and messages since v3.0 */
ENUM_STRUCT( DG3 )

  WAIT = 10000,

  DONE, FAIL,
  ENABLE, DISABLE,
  MODE_ON, MODE_OFF,

  DG_FILE_SYNTAX_ERROR,

  SAVING_DG_FILE,             WRONG_UNDO_MODE,
  SAVING_MESH_FILE,           BAD_FILE_MODE,
  WRITING_OUTPUT_FILE,        WRONG_VAR_OBJECT_TYPE,
  WRITING_TARGETS_FILE,       UNKNOWN_OBJECT_TYPE,
  WRITING_TEMPLATE_FILE,      WRONG_VARSET,
  WRITING_STRUCTURE_FILE,     MULTIPLE_VARDEFS,
  WRITING_EQUIL_FILE,         SAME_NODES,
  WRITING_SPLINE_FILE,        NODES_ALREADY_CONNECTED,
                              MODEL_IS_LOCKED,
  LOADING_DG_FILE,            NEGATIVE_UPDATELOCKS,
  LOADING_MESH_FILE,          OBJECT_IS_LOCKED,
  LOADING_TEMPLATE_FILE,      UNKNOWN_ORIGIN_TYPE,
  LOADING_EQUIL_FILE,         ALREADY_HAS_SONNETDATA,
  LOADING_TOPOLOGY_FILE,
  LOADING_SONNETDATA,         CANNOT_EXTEND_HRZ_CHORD,
                              LEVEL_PROBLEM,
  ACTIONS_REVERTED,           LINE_IS_EMPTY,
  NEW_MODEL_CREATED,          VALUE_OUT_OF_RANGE,
  MODEL_LOADED_SUCCESSFULLY,  OBJECT_IS_HIGHLIGHTED,
  MODEL_LOADED_WITH_ERRORS,   WRONG_NODE_STATE,
  MODEL_RENAMED,              TOO_MANY_VARSETS,
  ELEMENTS_RENUMBERED,        LOCKING_OBJECT_ERROR,
  TOPOLOGY_LD_SUCCESSFULLY,   WRONG_VARSETS_NUMBER,
  TOPOLOGY_LD_WITH_ERRORS,    NODE_HAS_ELEMENTS,
                              NODE_HAS_SEPARATORS,
  VIEW_CREATED,               BAD_SURFACE,
  VIEW_CLOSED,                MP_CHAIN_IS_TOO_SHORT,
  VIEW_SCALE_CHANGED,         GROUP_ERROR,
  VIEW_ANGLE_CHANGED,         NO_TOPOLOGY,
  VIEW_STRETCH_MODE,
  VIEW_RADIAL_GRID,           COMMENT,
  VIEW_COLORED_EQUIL,
  VIEW_SCENE,
  VIEW_CENTER_CHANGED,
  VIEW_STATE,

  ACT_ADDED,
  ACT_DELETED,
  ACT_CHANGED,
  ACT_UNDOMARK,
  ACT_GROUP,
  ACT_INCLUDE,
  ACT_EXCLUDE,
  ACT_MARKED,
  ACT_MARK,
  ACT_UNMARK,
  ACT_HIGHLIGHTED,
  ACT_HIGHLIGHT,
  ACT_UNHIGHLIGHT,

END_ENUM_STRUCT

#endif
