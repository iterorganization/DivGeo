#ifndef RESOURCE_IDS_ONLY

#include "x_dg.h"

#define ENV_DG_NO_PREFS "DG_NO_PREFS_FILE"
#define ENV_DG_PREFS "DG_PREFS_FILE"
#define ENV_HOME "HOME"

#define DEFAULT_PREFS_FILE ".dg-preferences"
#define PREFS_FILE_LOCK_SUFFIX ".lock"
/*#define PREFS_FILE_TEMP_SUFFIX ".temp"*/
#define PREFS_FILE_OLD_SUFFIX ".old"

#define PSTR_RECENTFILES_COUNT "DG.RecentFilesCount"
#define PSTR_RECENTFILES_PREFIX "DG.RecentFile_"

static int LoadHelp(XApp xap,char* fileName);

static String XAppResName(XApp xap,int id);
static char* XAppGetStringProc(StringSource strS,int id);

static XtActionsRec xappAct[]={
  "UseTool",   XtActUseTool,
  "CirculTool",XtActCirculTool,
  "DistrGraph",XtActDistrGraph,
};

static XtResource xappRes[]={
  "loadConfig","LoadConfig",XmRInt,sizeof(int),
   XtOffset(XApp,bLoadConfig),XmRImmediate,(XtPointer)True,
/*  "configFileName","ConfigFileName",XmRString,sizeof(String),
   XtOffset(XApp,configFileName),XmRImmediate,(XtPointer)NULL, */
};

static XtResource xappExtRes[]={
  "displayHelp","DisplayHelp",XmRInt,sizeof(int),
   XtOffset(XAppExt,bDisplayHelp),XmRImmediate,(XtPointer)False,
  "checkResources","CheckResources",XmRInt,sizeof(int),
   XtOffset(XAppExt,bCheckResources),XmRImmediate,(XtPointer)True,
  "resourceFileVersion","ResourceFileVersion",XmRInt,sizeof(int),
   XtOffset(XAppExt,resourceFileVersion),XmRImmediate,(XtPointer)-1,
  "checkHelpFile","CheckHelpFile",XmRInt,sizeof(int),
   XtOffset(XAppExt,bCheckHelpFile),XmRImmediate,(XtPointer)True,
};

XrmOptionDescRec commandLineOptions[]={
  "--help","displayHelp",XrmoptionNoArg,"1",
  "-help","displayHelp",XrmoptionNoArg,"1",
  "/help","displayHelp",XrmoptionNoArg,"1",
  "-h","displayHelp",XrmoptionNoArg,"1",
  "/h","displayHelp",XrmoptionNoArg,"1",
  "-?","displayHelp",XrmoptionNoArg,"1",
  "/?","displayHelp",XrmoptionNoArg,"1",
  "?","displayHelp",XrmoptionNoArg,"1",

  "-nocfg","loadConfig",XrmoptionNoArg,"0",
  "-cfg","configFileName",XrmoptionSepArg,NULL,
  "-wid","embedWID",XrmoptionSepArg,NULL,
  "-nores","checkResources",XrmoptionNoArg,"0",
  "-nohelp","checkHelpFile",XrmoptionNoArg,"0",
};

void ConfigureXApp(XApp xap) {
  char helpFileName[DG_FNAME_LEN];

  xap->x=Malloc(sizeof(*xap->x));
  xap->x->xViewShellsCount=0;
  xap->x->bPrefsLocked=0;

  xap->x->wShell=XtOpenApplication(&xap->x->appContext,"DivGeo",
    commandLineOptions,XtNumber(commandLineOptions),
    xap->pargc,xap->argv,dgResources,sessionShellWidgetClass,NULL,0);

  XtAddCallback(xap->x->wShell,XmNdestroyCallback,CbExitApp,(XtPointer)1);

  XmRepTypeInstallTearOffModelConverter(); /* allows tear-off menus */

  if (xap->x->wShell==NULL)
    FatalError("ConfigureXApp()-unable to open display: fatal error 1");
  XtGetApplicationResources(xap->x->wShell,xap,
    xappRes,XtNumber(xappRes),NULL,0);
  XtGetApplicationResources(xap->x->wShell,xap->x,
    xappExtRes,XtNumber(xappExtRes),NULL,0);

  xap->x->strs=Malloc(sizeof(*xap->x->strs));
  xap->x->strs->type=T_STRINGSOURCE;
  xap->x->strs->GetStringProc=XAppGetStringProc;
  xap->x->strs->userData=xap;
  xap->stringSource=xap->x->strs;

  xap->configFileName=GetResourceString(xap->x->wShell,"configFileName",
    NULL,"");
  if (*xap->configFileName/*!=NULL*/)
    xap->configFileName=MallocString(xap->configFileName);
  else {
    xap->configFileName=Malloc(DG_FNAME_LEN);
    strcpy(xap->configFileName,xap->argv[0]);
    strcpy(GetFileExt(xap->configFileName),XAppStr(xap,FSTR_CFGEXT));
  }
  xap->winToEmbedInto = (Window)strtol(GetResourceString(xap->x->wShell,
                                             "embedWID", NULL,""), NULL, 0);
  if (xap->x->bDisplayHelp) {
    fprintf(stderr,"DivGeo version %s\n"
      "Usage: %s [-display [Host]{:Number}] [-nores] [-nocfg] [-nohelp]"
      " [-cfg CfgFile] [filename] ...\n"
      "-display      Specifies display to use\n"
      "-wid          Embed first DivGeo view into window ID\n"
      "-nores        Do not check resource file version\n"
      "-nocfg        Do not load configuration file\n"
      "-cfg          Specifies configuration file\n"
      "-nohelp       Do not require the help file\n"
      "filename...   Files to load\n\n"
      "Default configuration file is %s\n",
      GetVersionStr(DG_VERSION),
      GetShortFName(xap->argv[0]),
      xap->configFileName);
    exit(10);
  }

  if (xap->x->bCheckResources)
    if (xap->x->resourceFileVersion<0)
      FatalError("DivGeo: no resources"); else
    if (xap->x->resourceFileVersion!=DG_VERSION)
      FatalError("DivGeo: invalid resource file version\n"
        "DivGeo version:        %s\n"
        "Resource file version: %s\n",
        GetVersionStr(DG_VERSION),
        GetVersionStr(xap->x->resourceFileVersion));


  xap->x->helpFile=NULL;
  strcpy(helpFileName,xap->argv[0]);
  strcpy(GetFileExt(helpFileName),XAppStr(xap,FSTR_HELPEXT));
  if (LoadHelp(xap,helpFileName) && xap->x->bCheckHelpFile) {
    FatalError("\rDivGeo: error opening help file\n");
  }

  XtAppAddActions(xap->x->appContext,xappAct,XtNumber(xappAct));
  xap->x->wm_delete_window=XmInternAtom(XtDisplay(xap->x->wShell),
      "WM_DELETE_WINDOW",False);
}

void UnconfigureXApp(XApp xap) {
  assert(xap->x->xViewShellsCount==0);

  XtDestroyWidget(xap->x->wShell);
  /*XtDestroyApplicationContext(xap->x->appContext);*/

  if (xap->x->helpFile!=NULL) xap->x->helpFile=Free(xap->x->helpFile);
  xap->x->strs=Free(xap->x->strs);
  xap->configFileName=Free(xap->configFileName);
  xap->winToEmbedInto=None;
  xap->stringSource=NULL;
  xap->x=Free(xap->x);
}

static char* XAppGetStringProc(StringSource strS,int id) {
  XApp xap=(XApp)strS->userData;
  XtResource xtr={
    NULL,"DivGeoMsg",XtRString,sizeof(String),0,
    XmRImmediate,NULL};
  char* resourceName;
  String str;

  assert(xap->type==T_XAPP);

  resourceName=XAppResName(xap,id);
  if (resourceName[0]=='\r') return resourceName+1;

  xtr.resource_name=resourceName;
  XtGetApplicationResources(xap->x->wShell,&str,&xtr,1,NULL,0);

  return str==NULL ? resourceName : str;
}

static int LoadHelp(XApp xap,char* fileName) {
  FILE* f;
  size_t p;

  f=fopen(fileName,"r");
  if (f==NULL) return -1;

  fseek(f,0,SEEK_END);
  p=(size_t)ftell(f);
  fseek(f,0,SEEK_SET);

  xap->x->helpFile=Malloc(p+1);
  fread(xap->x->helpFile,p,1,f);
  xap->x->helpFile[p]=0;

  fclose(f);

  return 0;
}

/* Private part
*/

#endif /* RESOURCE_IDS_ONLY */

static struct _PtrTable viewMsgs[]={
  MSG_OK,"msgOk",

  ERR_FILENOTFOUND,"errNotFound",
  ERR_BADFILE,"errBadFile",
  ERR_BADFILETYPE,"errBadFileType",
  ERR_OUTOFEQUIL,"errOut",
  ERR_BADTARGETS,"errBadTargets",
  ERR_NOTARGETS,"errNoTargets",
  ERR_BADXPOINT,"errBadXPoint",
  ERR_BADFILEVERSION,"errBadVersion",
  ERR_USEDBY,"errUsedBy",
  ERR_NOVIEW,"errNoView",
  ERR_NOXPOINT,"errNoXPoint",
  ERR_IRREGULAR,"errIrregular",
  ERR_NOEQUIL,"errNoEquil",
  ERR_NONODES,"errNoNodes",
  ERR_NOELEMS,"errNoElems",
  ERR_INVNUMBERS,"errInvNumbers",
  ERR_MINMAXVARSETS,"errMinMaxVarSets",
  ERR_BADNAME,"errBadName",
  ERR_DUPNAME,"errDupName",
  ERR_CLOSED,"errClosed",
  ERR_BROKEN,"errBroken",
  ERR_FWRITE,"errFWrite",
  ERR_NOSTRUCTURE,"errNoStructure",
  ERR_NOCLOSEDSURFS,"errNoClosedSurfaces",
  ERR_TEMPLATEINCLUDED,"errTemplateIncluded",
  ERR_NOTEMPLATE,"errNoTemplate",
  ERR_SINGLE,"errSingle",
  ERR_MANYSELELEMS,"errManySelectedElems",
  ERR_MULTGROUP,"errMultipleGroup",
  ERR_NOSONNET,"errNoSonnet",
  ERR_NOCONFIG,"errNoConfig",
  ERR_NOTHINGTODO,"errNothingToDo",
  ERR_ALREADYEXISTS,"errAlreadyExists",
  ERR_INVAREA,"errInvArea",
  ERR_LOCKED,"errLocked",
  ERR_NOSAVECONFIG,"errNoSaveConfig",
  ERR_EMPTYVARS,"errEmptyVars",
  ERR_EMPTYTARGETS,"errEmptyTargets",
  ERR_EMPTYSTRUCTURE,"errEmptyStructure",
  ERR_BADSURFACES,"errBadSurfaces",
  ERR_BADGRIDPOINTS,"errBadGridPoints",
  ERR_NOFNAME,"errNoFileName",
  ERR_NOSELECTION,"errNoSelection",
  ERR_MINTARGETELEMS,"errMinTargetElems",
  ERR_WRONGTYPE,"errWrongType",
  ERR_JOINCONNECTED,"errJoinConnected",
  ERR_BADSURFORIGIN,"errBadSurfOrigin",
  ERR_TWISTEDSONNET,"errTwistedSonnet",
  ERR_NOSEPARATORS,"errNoSeparators",
  ERR_SONNETSTRUCTINS,"errSonnetStructIns",
  ERR_SEPARATORORDER,"errSeparatorOrder",
  ERR_INTERNALCELLS,"errInternalCells",
  ERR_BADCELLS,"errBadCells",
  ERR_SURFTARGETINS,"errSurfTargetIns",
  ERR_BADSTRETCHTYPE,"errBadStretchType",
  ERR_NONEMPTYNODE,"errNonEmptyNode",
  ERR_NOTSTRUCTPART,"errNotStructurePart",
  ERR_INTERNALGP,"errInternalGridPoint",
  ERR_NOHELP,"errNoHelp",
  ERR_NOTOPIC,"errNoTopic",
  ERR_BADMASK,"errBadMask",
  ERR_BADSTRUCTURE,"errBadStructure",
  ERR_MULTIPLEERRORS,"errMultipleErrors",
  ERR_PIPECREATE,"errCreatePipe",
  ERR_FORK,"errFork",
  ERR_PSVIEWSIZE,"errPsViewSize",
  ERR_BADXPOINTSEG,"errBadXPointSeg",
  ERR_LOOPNOTFOUND,"errLoopNotFound",
  ERR_LEVELNOTINAREA,"errLevelNotInArea",
  ERR_LEVELNOTFOUND,"errLevelNotFound",
  ERR_NOPREFS,"errNoPrefs",
  ERR_CARRE_SURFACES,"errCarreSurfaces",
  ERR_CARRE_XSURFACES,"errCarreXSurfaces",
  ERR_CARRE_TANGENT,"errCarreTangent",
  ERR_CARRE_GPOINTS,"errCarreGridPoints",
  ERR_CARRE_ISURFACES,"errCarreSurfacesInternal",
  ERR_CARRE_IGPOINTS,"errCarreGridPointsInternal",
  ERR_CARRE_PEAK,"errCarrePeak",
  ERR_CARRE_INHIBITS,"errCarreInhibits",
  ERR_CARRE_EQUALLEVELS,"errCarreEqualLevels",
  ERR_CARRE_AREA1SURFACES,"errCarreNoArea1Surfaces",
  ERR_CARRE_OUTOFEQUIL,"errCarreOutOfEquil",
  ERR_CARRE_MODE_NEEDED,"errCarreModeNeeded",
  ERR_CARRE_BROKENTARGET,"errCarreBrokenTarget",
  ERR_CARRE_TARGETXSTRUCTURE,"errCarreTargetXStructure",
  ERR_NO_MARKED_ELEMS,"errNoMarkedElems",
  ERR_NO_MARKED_CHORDS,"errNoMarkedChords",
  ERR_EMPTY_TEMPLATE,"errEmptyTemplate",
  ERR_REPOS_TEMPLATE_1,"errReposTemplate1",
  ERR_REPOS_TEMPLATE_2,"errReposTemplate2",
  ERR_FLAT_EQUIL,"errFlatEquil",
  ERR_DUP_MESH_ELNO,"errDupMeshElmNo",
  ERR_MOVE_CUT_MESH_PT,"errMoveCutMeshPt",
  ERR_MESH_NO_EQUIL,"errMeshNoEquil",
  ERR_MESH_OUT_OF_EQUIL,"errMeshOutOfEquil",
  ERR_DEL_MARKED_OBJECT_TYPE,"errDelMarkedObjectType",
  ERR_MESH_ELEMENT_DIR,"errMeshElementDir",
  ERR_MESH_CELL_FIND,"errMeshCellFind",
  ERR_BAD_OBJECT_TYPE,"errBadObjectType",
  ERR_MESH_HDR_NO_LF,"errMeshHeaderNoLF",
  ERR_MESH_HDR_NO_ID,"errMeshHeaderNoID",
  ERR_EQUIL_SPLINE,"errEquilSpline",
  ERR_BADSURFZONENUMBER,"errBadSurfaceZoneNumber",
  ERR_BADGPZONENUMBER,"errBadGridPointZoneNumber",
  ERR_FSOPX_1,"errFSOPX_i1",
  ERR_OUTOFSURFZONE,"errOutOfSurfaceZone",
  ERR_BADSURFACEZONE1,"errBadSurfaceZone1",
  ERR_NOSURFZONE,"errNoSurfaceZoneAtXY",
  ERR_OLD_FILE_NO_TOPOLOGY,"errOldFileNoTopology",
  ERR_TARGET_CROSSED_2X,"errTargetCrossed2x",
  ERR_TARGET_NOT_CROSSED,"errTargetNotCrossed",
  ERR_SURFACE_XY,"errOutputSurfaceXY",
  ERR_NOELEMSINPATH,"errNoElementsInPath",

  WRN_NOEQUIL,"wrnNoEquil",
  WRN_NOTEMPL,"wrnNoTemplate",
  WRN_BADTYPE,"wrnBadType",
  WRN_SYNTAX,"wrnSyntax",
  WRN_NOSONNET,"wrnNoSonnet",
  WRN_BADTARG1,"wrnBadTarget1",
  WRN_BADTARG2,"wrnBadTarget2",
  WRN_BADSTRUC,"wrnBadStructure",
  WRN_BADXPT,"wrnBadXpoint",
  WRN_MESH_FP,"wrnMeshFingerprint",
  WRN_OLDTOPO,"wrnOldTopo",
  WRN_OLD_SURFACES_LOST,"wrnOldSurfacesLost",

  MSG_EXAMNODE,"msgExamNode",
  MSG_EXAMELEM,"msgExamElem",
  MSG_EXAMSURFACE,"msgExamSurface",
  MSG_EXAMGRIDPOINT,"msgExamGridPoint",
  MSG_EXAMSEPARATOR,"msgExamSeparator",
  MSG_EXAMSOURCE,"msgExamSource",
  MSG_EXAM_MESH_CELL,"msgExamMeshCell",
  MSG_EXAM_MESH_ELEMENT,"msgExamMeshElement",
  MSG_EXAM_MESH_POINT,"msgExamMeshPoint",
  MSG_EXAM_XPOINTTEST,"msgExamXPointEx",
  MSG_EXAM_XPOINTSEG,"msgExamXPointSeg",
  MSG_EXAM_SURFACE_XY,"msgExamSurfaceXY",


  MSG_TEMPLATELOADED,"msgTemplateLoaded",
  MSG_TEMPLATEREMOVED,"msgTemplateRemoved",
  MSG_EQUILLOADED,"msgEquilLoaded",
  MSG_EQUILREMOVED,"msgEquilRemoved",
  MSG_NEWFILE,"msgNewFile",
  MSG_FILEOPENED,"msgFileOpened",
  MSG_FILESAVED,"msgFileSaved",
  MSG_SONNETLOADED,"msgSonnetLoaded",
  MSG_SONNETREMOVED,"msgSonnetRemoved",
  MSG_XPOINTREMOVED,"msgXPointRemoved",
  MSG_TEMPLAPPENDED,"msgTemplateAppended",
  MSG_SEPARATORSADDED,"msgSeparatorsAppended",
  MSG_SEPARATORSREMOVED,"msgSeparatorsRemoved",
  MSG_RENUMBERED,"msgRenumbered",
  MSG_OUTPUT,"msgOutput",
  MSG_CANCELED,"msgCanceled",
  MSG_VERSION,"msgVersion",
  MSG_EMPTYNODESREMOVED,"msgEmptyNodesRemoved",
  MSG_ELEMSREMOVED,"msgElemsRemoved",
  MSG_SOURCESREMOVED,"msgSourcesRemoved",
  MSG_SURFACESREMOVED,"msgSurfacesRemoved",
  MSG_GPOINTSREMOVED,"msgGridPointsRemoved",
  MSG_ABOUT,"msgAbout",
  MSG_EXAMCHORD,"msgExamChord",
  MSG_CHORDSREMOVED,"msgChordsRemoved",
  MSG_ALLVARSVALID,"msgAllVarsValid",
  MSG_SOMEVARSINVALID,"msgSomeVarsInvalid",
  MSG_FILEPRINTED,"msgFilePrinted",
  MSG_REPOS_HANDLE_1,"msgReposHandle1",
  MSG_REPOS_HANDLE_2,"msgReposHandle2",
  MSG_MARKED_REMOVED,"msgMarkedObjectsRemoved",
  MSG_CALCULATING_SZ_BOUNDS,"msgCalculatingSurfaceZoneBounds",
  MSG_FILEAUTOSAVED,"msgFileAutoSaved",
  MSG_NOBOUNDINGELEMS,"msgNoBoundingElems",
  MSG_NOMARKEDCHORDS,"msgNoMarkedChords",

  STR_LOCKED,"strLocked",
  STR_UNLOCKED,"strUnlocked",
  STR_ERRLABEL,"strErrLabel",
  STR_VSDEFAULTS,"strVarSetDefaults",
  STR_NOELEMS,"strNoElems",
  STR_MANYELEMS,"strManyElems",
  STR_NOSEL,"strNoSelection",
  STR_DIFF,"strDifferentValues",
  STR_NODE,"strNode",
  STR_ELEM,"strElem",
  STR_SURFACE,"strSurface",
  STR_GRIDPOINT,"strGridPoint",
  STR_SEPARATOR,"strSeparator",
  STR_EQUIL,"strEquil",
  STR_TEMPLATE,"strTemplate",
  STR_SONNET,"strSonnet",
  STR_XPOINT,"strXPoint",
  STR_NEWLABEL,"strNewLabel",
  STR_REMOVED,"strRemoved",
  STR_ADDED,"strAdded",
  STR_CHANGED,"strChanged",
  STR_VARSLOST,"strVarsLost",
  STR_DESCRCHANGED,"strDescrChanged",
  STR_SURFAREA0,"strSurfaceArea1",
  STR_SURFAREA1,"strSurfaceArea2",
  STR_SURFAREA2,"strSurfaceArea3",
  STR_GRPOINTAREA0,"strGridPointZone0",
  STR_GRPOINTAREA1,"strGridPointZone1",
  STR_GRPOINTAREA2,"strGridPointZone2",
  STR_OUTPUTFNAME,"strOutputFileName",
  STR_STRUCTUREFNAME,"strStructureFileName",
  STR_TARGETSFNAME,"strTargetsFileName",
  STR_HELPCHANGED,"strHelpChanged",
  STR_ENUMCHANGED,"strEnumChanged",
  STR_SOURCE,"strSource",
  STR_IRRNORMALS,"strIrrNormals",
  STR_IRRSEPARATORS,"strIrrSeparators",
  STR_IRRTOOMANY,"strIrrTooMany",
  STR_IRRTOOFEW,"strIrrTooFew",
  STR_REGULAR,"strRegular",
  STR_DSURFAREANOSIGN,"strDSurfAreaNoSign",
  STR_DSURFAREA1,"strDSurfArea1",
  STR_DSURFAREA2,"strDSurfArea2",
  STR_DSURFAREA3,"strDSurfArea3",
  STR_DGRPOINTAREA0,"strDGridPointArea0",
  STR_DGRPOINTAREA1,"strDGridPointArea1",
  STR_DGRPOINTAREA2,"strDGridPointArea2",
  STR_CHORD,"strChord",
  STR_XPOINTTEST,"strXPointTest",
  STR_XPOINTSEG,"strXPointSeg",
  STR_MESH_CELL_TWISTED,"strMeshCellTwisted",
  STR_MESH_CELL_BAD_CENTER,"strMeshCellCenterOutside",
  STR_MESH_ELEMENT_CUT,"strMeshElementCut",
  STR_MESH_CELL,"strMeshCell",
  STR_MESH_ELEMENT,"strMeshElement",
  STR_MESH_POINT,"strMeshPoint",
  STR_MESH_CELL_CONCAVE,"strMeshCellConcave",

  FSTR_CFGEXT,"\r.dgc",
  FSTR_OUTPUTEXT,"\r.dgo",
  FSTR_STRUCTUREEXT,"\r.str",
  FSTR_TARGETSEXT,"\r.trg",
  FSTR_HELPEXT,"\r.dgh",

  ENV_CFGFILE,"\rDGCONFIG",
  ENV_CFGFILE2,"\rDG_CONFIG",
  ENV_LOADMASK,"\rDGOPENMASK",
  ENV_LOADMASK2,"\rDG_OPEN_MASK",
  ENV_SAVEMASK,"\rDGSAVEMASK",
  ENV_SAVEMASK2,"\rDG_SAVE_MASK",
  ENV_EQUILMASK,"\rDGEQUILMASK",
  ENV_EQUILMASK2,"\rDG_EQUIL_MASK",
  ENV_TEMPLATEMASK,"\rDGTEMPLATEMASK",
  ENV_TEMPLATEMASK2,"\rDG_TEMPLATE_MASK",
  ENV_SONNETMASK,"\rDGSONNETMASK",
  ENV_SONNETMASK2,"\rDG_MESH_MASK",

  ENV_VARSFILESETMASK,"\rDG_VARS_FILE_SET_MASK",

  QUE_WITHGEOMETRY,"queWithGeometry",
  QUE_UPDATEAPPS,"queUpdateApps",
  0,"Ok",

  0,NULL
};

#ifndef RESOURCE_IDS_ONLY

static char* XAppResName(XApp xap,int id) {
  return TranslateId(viewMsgs,id);
}

XAppPause(XApp xap,char* str) {
  printf(str);                          /* relcheck_ignore_line */
  ProcessPending(xap->x->wShell);
  getchar();
}

char* GetUserPrefsFileName(XApp xap) {
  char* s;
  static char buf[1024];

  s=getenv(ENV_DG_NO_PREFS);
  if (s!=NULL) return NULL;

  s=getenv(ENV_DG_PREFS);
  if (s!=NULL) return s;

  s=getenv(ENV_HOME);
  if (s==NULL || !*s) FatalError("The $HOME variable cannot be read");

  strncpy(buf,s,sizeof(buf)-100);
  if (buf[strlen(buf)-1]!='/') strcat(buf,"/");
  strcat(buf,DEFAULT_PREFS_FILE); 

  return buf;
}

char* GetUserPrefsString(XApp xa,char* str) {
  char* fn;
  FILE* f;
  static char buf[1024],strEx[1024],oldFn[1024];
  int l;

  fn=GetUserPrefsFileName(xa);
  if (fn==NULL) return "";

  f=fopen(fn,"r");
  if (f==NULL) return "";

  strncpy(strEx,str,sizeof(strEx)-2);
  strcat(strEx,"=");
  l=strlen(strEx);

  while (fgets(buf,sizeof(buf)-1,f)!=NULL) {
    if (!strncmp(buf,strEx,l)) {
      if (*buf && buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]=0;
      fclose(f);
      return buf+l;
    }
  }

  fclose(f);

  return "";
}

int SetUserPrefsString(XApp xa,char* str,char* value) {
  char* fn;
  FILE* f;
  char buf[1024],strEx[1024];
  char* oldStrings,* osCur;
  int l;

  assert(xa->x->bPrefsLocked);

  if (value==NULL) value="";

  /* Obtain the preferences filename */

  fn=GetUserPrefsFileName(xa);
  if (fn==NULL) return ERR_NOPREFS;

  /* Try opening the preferences file */

  f=fopen(fn,"r");

  /* Read the existing preferences file, if any */

  if (f!=NULL) {
    fseek(f,0,SEEK_END);
    l=ftell(f);
    fseek(f,0,SEEK_SET);

    oldStrings=Malloc(l+1);
    oldStrings[l]=0;
    fread(oldStrings,l,1,f);
    fclose(f);
  } else oldStrings=osCur=NULL;

  /* Open output file */

  f=fopen(fn,"w");
  if (f==NULL) return ERR_FWRITE;

  /* Build the search string as '<str>=' */

  strncpy(strEx,str,sizeof(strEx)-2);
  strcat(strEx,"=");
  l=strlen(strEx);

  /* Write existing preferences, if any, except for the string to be set */

  if (oldStrings!=NULL) {
    osCur=strtok(oldStrings,"\n");
    if (osCur!=NULL) do {
      if (strncmp(osCur,strEx,l)) {
        fputs(osCur,f);
        fputs("\n",f);
      }
      osCur=strtok(NULL,"\n");
    } while (osCur!=NULL);
    oldStrings=Free(oldStrings);
  }

  /* Append the newly set string at the end, if not empty */

  if (*value) {
    fputs(strEx,f);
    fputs(value,f);
    fputs("\n",f);
  }

  /* Close the file */

  fclose(f);

  return 0;
}


int LockUserPrefsFile(XApp xa) {  /*** DUMMY ***/
  char s,buf[1024];

  assert(!xa->x->bPrefsLocked);

  /*s=GetUserPrefsFileName(xa);
  if (s==NULL) assert(NULL); */

  xa->x->bPrefsLocked++;

  return 0;
}


void UnlockUserPrefsFile(XApp xa) { /*** DUMMY ***/
  assert(xa->x->bPrefsLocked);

  xa->x->bPrefsLocked--;
}

int GetUserPrefsInt(XApp xa,char* name) {
  return atol(GetUserPrefsString(xa,name));
}

int SetUserPrefsInt(XApp xa,char* name,int value) {
  char buf[256];

  sprintf(buf,"%d",value);

  return SetUserPrefsString(xa,name,buf);
}

Stack GetRecentFilesStack(XApp xap) {
  Stack g;
  char* s;
  char buf[256];
  int i,maxCount;

  g=CreateStack();

  i=0;
  maxCount=GetUserPrefsInt(xap,PSTR_RECENTFILES_COUNT);
  while (i<maxCount) {
    sprintf(buf,"%s%d",PSTR_RECENTFILES_PREFIX,i++);
    s=GetUserPrefsString(xap,buf);
    if (s==NULL || !*s) break;
    GroupAdd(g,MallocString(s));
  }
  RevertGroup(g);

  return g;
}

static void WriteRecentFilesStack(XApp xap,Stack g) {
  char* s;
  char buf[256];
  int i,maxCount;
  Index ix;

  LockUserPrefsFile(xap);

  maxCount=GetResourceInt(xap->x->wShell,"recentFilesCount",NULL,4);

  for (s=Group1st(g,&ix),i=0;s!=NULL && i<maxCount;s=Next(&ix),i++) {
    sprintf(buf,"%s%d",PSTR_RECENTFILES_PREFIX,i);
    SetUserPrefsString(xap,buf,s);
  }
  SetUserPrefsInt(xap,PSTR_RECENTFILES_COUNT,i);

  UnlockUserPrefsFile(xap);
}

void AddXAppRecentFile(XApp xap,char* fileName) {
  Stack g;
  View w;
  char* s;
  Index ix;

  g=GetRecentFilesStack(xap);

  /* Remove the filename from the stack, if it was already there */

  for (s=Group1st(g,&ix);s!=NULL;s=Next(&ix)) {
    if (!strcmp(s,fileName)) {
      GroupDel(g,s);
      s=Free(s);
    }
  }

  /* Add to the top of the stack */

  GroupAdd(g,MallocString(fileName));

  WriteRecentFilesStack(xap,g);
  FreeMallocedGroup(g);

  for (w=Group1st(xap->views,&ix);w!=NULL;w=Next(&ix))
    NotifyView(w,N_RECENTFILES,NULL);
}

View FindViewByFilename(XApp xap,char* fileName) {
  View w;
  Index ix;

  for (w=Group1st(xap->views,&ix);w!=NULL;w=Next(&ix))
    if (w->app!=NULL && w->app->fName!=NULL &&
        !strcmp(w->app->fName,fileName))
      return w;

  return NULL;
}

#endif /* RESOURCE_IDS_ONLY */

