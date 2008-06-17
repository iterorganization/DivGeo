/* Error & information messages
*/

#ifndef _msg_h
#define _msg_h

#define ERR_FILENOTFOUND     1
#define ERR_BADFILE          2
#define ERR_BADFILETYPE      3
#define ERR_OUTOFEQUIL       4
#define ERR_BADTARGETS       5
#define ERR_NOTARGETS        6
#define ERR_BADXPOINT        7
#define ERR_BADFILEVERSION   8
#define ERR_USEDBY           9
#define ERR_NOVIEW           10
#define ERR_NOXPOINT         11
#define ERR_IRREGULAR        12
#define ERR_NOEQUIL          13
#define ERR_NONODES          14
#define ERR_NOELEMS          15
#define ERR_INVNUMBERS       16
#define ERR_MINMAXVARSETS    17
#define ERR_BADNAME          18
#define ERR_DUPNAME          19
#define ERR_CLOSED           20
#define ERR_BROKEN           21
#define ERR_FWRITE           22
#define ERR_NOSTRUCTURE      23
#define ERR_NOCLOSEDSURFS    24
#define ERR_TEMPLATEINCLUDED 25
#define ERR_NOTEMPLATE       26
#define ERR_SINGLE           27
#define ERR_MANYSELELEMS     28
#define ERR_MULTGROUP        29
#define ERR_NOSONNET         30
#define ERR_NOCONFIG         31
#define ERR_NOTHINGTODO      32
#define ERR_ALREADYEXISTS    33
#define ERR_INVAREA          34
#define ERR_LOCKED           35
#define ERR_NOSAVECONFIG     36
#define ERR_EMPTYVARS        37
#define ERR_EMPTYTARGETS     38
#define ERR_EMPTYSTRUCTURE   39
#define ERR_BADSURFACES      40
#define ERR_BADGRIDPOINTS    41
#define ERR_NOFNAME          42
#define ERR_NOSELECTION      43
#define ERR_MINTARGETELEMS   44
#define ERR_WRONGTYPE        45
#define ERR_JOINCONNECTED    50
#define ERR_BADSURFORIGIN    51
#define ERR_TWISTEDSONNET    52
#define ERR_NOSEPARATORS     53
#define ERR_SONNETSTRUCTINS  54
#define ERR_SEPARATORORDER   55
#define ERR_INTERNALCELLS    56
#define ERR_BADCELLS         57
#define ERR_SURFTARGETINS    58
#define ERR_BADSTRETCHTYPE   59
#define ERR_NONEMPTYNODE     60
#define ERR_NOTSTRUCTPART    61
#define ERR_INTERNALGP       62
#define ERR_NOHELP           63
#define ERR_NOTOPIC          64
#define ERR_BADMASK          65
#define ERR_BADSTRUCTURE     66
#define ERR_MULTIPLEERRORS   67
#define ERR_PIPECREATE       68
#define ERR_FORK             69
#define ERR_PSVIEWSIZE       70
#define ERR_BADXPOINTSEG     71
#define ERR_LOOPNOTFOUND     72
#define ERR_LEVELNOTINAREA   73
#define ERR_LEVELNOTFOUND    74
#define ERR_NOPREFS          75
#define ERR_CARRE_SURFACES   76
#define ERR_CARRE_XSURFACES  77
#define ERR_CARRE_TANGENT    78
#define ERR_CARRE_GPOINTS    79
#define ERR_CARRE_ISURFACES  80
#define ERR_CARRE_IGPOINTS   81
#define ERR_CARRE_PEAK       82
#define ERR_CARRE_INHIBITS   83
#define ERR_CARRE_EQUALLEVELS   84
#define ERR_CARRE_AREA1SURFACES 85
#define ERR_CARRE_OUTOFEQUIL 86
#define ERR_CARRE_MODE_NEEDED 87
#define ERR_CARRE_BROKENTARGET 88
#define ERR_CARRE_TARGETXSTRUCTURE 89
#define ERR_NO_MARKED_ELEMS 90
#define ERR_NO_MARKED_CHORDS 91
#define ERR_EMPTY_TEMPLATE 92
#define ERR_REPOS_TEMPLATE_1 93   /* To be used only in TlReposition */
#define ERR_REPOS_TEMPLATE_2 94   /* To be used only in TlReposition */
#define ERR_FLAT_EQUIL 95
#define ERR_DUP_MESH_ELNO 96      /* Duplicate element number in a mesh file */
#define ERR_MOVE_CUT_MESH_PT 97   /* Can't move a "cut" mesh point */
#define ERR_MESH_NO_EQUIL    98   /* Equilibrium reqd for mesh operations */
#define ERR_MESH_OUT_OF_EQUIL 99
#define ERR_DEL_MARKED_OBJECT_TYPE 100 /* Deleting a marked mesh element/cell */
#define ERR_MESH_ELEMENT_DIR 101  /* Mesh element with a wrong orientation*/
#define ERR_MESH_CELL_FIND 102    /* Couldn't find the nearest mesh cell */
#define ERR_BAD_OBJECT_TYPE 103   /* For "export elements as template" */

#define ERR_MESH_HDR_NO_LF 104    /* Used in                 */
#define ERR_MESH_HDR_NO_ID 105    /* CheckMeshHeaderString() */

#define MSG_EXAMNODE         1000
#define MSG_EXAMELEM         1001
#define MSG_EXAMSURFACE      1002
#define MSG_EXAMGRIDPOINT    1003
#define MSG_TEMPLATELOADED   1004
#define MSG_TEMPLATEREMOVED  1005
#define MSG_EQUILLOADED      1006
#define MSG_EQUILREMOVED     1007
#define MSG_NEWFILE          1008
#define MSG_FILEOPENED       1009
#define MSG_FILESAVED        1010
#define MSG_SONNETLOADED     1011
#define MSG_SONNETREMOVED    1012
#define MSG_XPOINTREMOVED    1013
#define MSG_TEMPLAPPENDED    1014
#define MSG_EXAMSEPARATOR    1015
#define MSG_SEPARATORSADDED  1016
#define MSG_SEPARATORSREMOVED 1017
#define MSG_RENUMBERED       1018
#define MSG_OUTPUT           1019
#define MSG_EXAMSOURCE       1020
#define MSG_CANCELED         1021
#define MSG_VERSION          1022
#define MSG_EMPTYNODESREMOVED 1023
#define MSG_ELEMSREMOVED     1024
#define MSG_SOURCESREMOVED   1025
#define MSG_SURFACESREMOVED  1026
#define MSG_GPOINTSREMOVED   1027
#define MSG_ABOUT            1028
#define MSG_EXAMCHORD        1029
#define MSG_CHORDSREMOVED    1030
#define MSG_ALLVARSVALID     1031
#define MSG_SOMEVARSINVALID  1032
#define MSG_FILEPRINTED      1033
#define MSG_WORKING          1034
#define MSG_XPOINTSFOUND     1035
#define MSG_XPSEGSFOUND      1036
#define MSG_REPOS_HANDLE_1   1037  /* Used in TlRepos() only */
#define MSG_REPOS_HANDLE_2   1038  /* Used in TlRepos() only */
#define MSG_MARKED_REMOVED   1039  /* Marked objects removed */
#define MSG_EXAM_MESH_CELL   1040
#define MSG_EXAM_MESH_ELEMENT 1041
#define MSG_EXAM_MESH_POINT  1042

#define STR_LOCKED           2000
#define STR_UNLOCKED         2001
#define STR_ERRLABEL         2002
#define STR_VSDEFAULTS       2003
#define STR_NOELEMS          2004
#define STR_MANYELEMS        2005
#define STR_NOSEL            2006
#define STR_DIFF             2007
#define STR_NODE             2008
#define STR_ELEM             2009
#define STR_SURFACE          2010
#define STR_GRIDPOINT        2011
#define STR_SEPARATOR        2012
#define STR_EQUIL            2013
#define STR_TEMPLATE         2014
#define STR_SONNET           2015
#define STR_XPOINT           2016
#define STR_NEWLABEL         2017
#define STR_REMOVED          2018
#define STR_ADDED            2019
#define STR_CHANGED          2020
#define STR_VARSLOST         2021
#define STR_DESCRCHANGED     2022
#define STR_SURFAREA0        2023
#define STR_SURFAREA1        2024
#define STR_SURFAREA2        2025
#define STR_GRPOINTAREA0     2026
#define STR_GRPOINTAREA1     2027
#define STR_GRPOINTAREA2     2028
#define STR_OUTPUTFNAME      2029
#define STR_STRUCTUREFNAME   2030
#define STR_TARGETSFNAME     2031
#define STR_HELPCHANGED      2032
#define STR_ENUMCHANGED      2033
#define STR_SOURCE           2034
#define STR_IRRNORMALS       2035
#define STR_IRRSEPARATORS    2036
#define STR_IRRTOOMANY       2037
#define STR_IRRTOOFEW        2038
#define STR_REGULAR          2039

#define STR_DSURFAREABASE    2040
#define STR_DSURFAREANOSIGN  2040
#define STR_DSURFAREA1       2041
#define STR_DSURFAREA2       2042
#define STR_DSURFAREA3       2043

#define STR_DGRPOINTAREABASE 2044
#define STR_DGRPOINTAREA0    2044
#define STR_DGRPOINTAREA1    2045
#define STR_DGRPOINTAREA2    2046

#define STR_CHORD            2047
#define STR_XPOINTTEST       2048
#define STR_XPOINTSEG        2049

#define STR_MESH_CELL_TWISTED 2050
#define STR_MESH_CELL_BAD_CENTER 2051
#define STR_MESH_ELEMENT_CUT 2052
#define STR_MESH_CELL        2053
#define STR_MESH_ELEMENT     2054
#define STR_MESH_POINT       2055

#define FSTR_CFGEXT          3007
#define FSTR_OUTPUTEXT       3008
#define FSTR_STRUCTUREEXT    3009
#define FSTR_TARGETSEXT      3010
#define FSTR_HELPEXT         3011

#define QUE_WITHGEOMETRY     4000
#define QUE_UPDATEAPPS       4001

#define WRN_NOEQUIL          5001
#define WRN_NOTEMPL          5002
#define WRN_BADTYPE          5003
#define WRN_SYNTAX           5004
#define WRN_NOSONNET         5005
#define WRN_BADTARG1         5006
#define WRN_BADTARG2         5007
#define WRN_BADSTRUC         5008
#define WRN_BADXPT           5009
#define WRN_MESH_FP          5010

#define ENV_CFGFILE          6000
#define ENV_LOADMASK         6001
#define ENV_SAVEMASK         6002
#define ENV_EQUILMASK        6003
#define ENV_TEMPLATEMASK     6004
#define ENV_SONNETMASK       6005

#define ENV_CFGFILE2         6006
#define ENV_LOADMASK2        6007
#define ENV_SAVEMASK2        6008
#define ENV_EQUILMASK2       6009
#define ENV_TEMPLATEMASK2    6010
#define ENV_SONNETMASK2      6011

#endif
