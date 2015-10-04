#ifndef MODEL_H
#define MODEL_H

#include "CoreConstants.h"
#include "../../common/Includes.h"

#include "../mesh/MeshDecl.h"
#include "../sonnet/SonnetDecl.h"
#include "../diagnostics/DiagnosticsDecl.h"
#include "../flux/EquilDecl.h"
#include "../flux/Topology.h"

#include "ListContainer.h"
#include "IValueHolder.h"
#include "IManager.h"
#include "Actions.h"

#include "ActionStack.h"

#include "VarsManager.h"
class VarsManager;

#include "../flux/FluxModel.h"
class FluxModel;

class Model:
  public IValueHolder,
  public IManager
{
  DG_PROPERTIES {
    p_max_elem_id             = 0,
    p_output_flags            = 1,
    p_mesh_sliding_mode       = 2,
    p_mesh_sliding_threshold  = 3,
    p_double_mesh_border      = 4,
    p_topology_name           = 5,
    p_file_name               = 6,
    p_output_mode             = 7,
    p_creation_time           = 8,
    p_sz_restriction          = 9
  };

  DG_PROPERTIES_TABLE {
    AddProperty( "max_elem_id",             &maxElemId );
    AddProperty( "output_flags",            &outputFlags );
    AddProperty( "mesh_sliding_mode",       &meshSlidingMode );
    AddProperty( "mesh_sliding_threshold",  &meshSlidingThreshold );
    AddProperty( "double_mesh_border",      &doubleMeshBorder );
    AddProperty( "topology_name",           &topologyName );
    AddProperty( "file_name",               &fileName );
    AddProperty( "output_mode",             &outputMode );
    AddProperty( "creation_time",           &creationTime );
    AddProperty( "sz_restriction",          &szRestriction );
  }

  DG_HIERARCHY_TABLE {
    RegisterType( OT::NODE,       &nodes );
    RegisterType( OT::ELEMENT,    &elements );
    RegisterType( OT::SEPARATOR,  &separators );
    RegisterType( OT::SOURCE,     &sources );
    RegisterType( OT::CHORD,      &chords );
    RegisterType( OT::COMMENT,    &comments );

    RegisterType( OT::MESH,         pMesh );
    RegisterType( OT::MESHCELL,     pMesh );
    RegisterType( OT::MESHELEMENT,  pMesh );
    RegisterType( OT::MESHPOINT,    pMesh );

    RegisterType( OT::VARSETDEF,    pVars );
    RegisterType( OT::VARDEF,       pVars );
    RegisterType( OT::VARSET,       pVars );
    RegisterType( OT::VAR,          pVars );

    RegisterType( OT::EQUIL,        pFlux );
    RegisterType( OT::XPOINTTEST,   pFlux );
    RegisterType( OT::XPOINTSEG,    pFlux );
    RegisterType( OT::GRIDPOINTSEG, pFlux );
    RegisterType( OT::SURFACEZONE,  pFlux );
    RegisterType( OT::GRIDPOINTEX,  pFlux );
    RegisterType( OT::SURFACEEX,    pFlux );
  }

private:

  IModelAgentPtr pAgent;
  std::string sender_name;
  std::string title_name;

  ulong chFlags;

  int maxElemId;
  ulong outputMode;
  int updateLocks;

  int errorCode;
  bool bStrict;

  MeshPtr pMesh;
  TemplatePtr pTemplate;
  SonnetDataPtr pSonnetData;

  VarsManager* pVars;
  FluxModel* pFlux;

  std::string creationTime;
  std::string fileName;
  std::string topologyName;
  static const std::string topologyName_empty; // "*"

  CActionStack actStack;

  /* Groups */

  IComponentList markedList;
  IComponentList highlightedList;

  mutable IComponentList nodes_unused; /* for UnusedNodes() */
  IComponentList nodes;
  IComponentList elements;
  IComponentList separators;
  IComponentList sources;
  IComponentList chords;
  IComponentList comments;

  // Outer control vars
  ulong meshSlidingMode;
  double meshSlidingThreshold;
  bool doubleMeshBorder;
  ulong outputFlags;

  // Default view attributes
  Point min_position;
  Point max_position;
  double xyAngle;
  ulong showFlags;

  // Topology parameters
  ulong szRestriction;

  static const std::string scsShow, scsHide;

public:
  /* Constructors */
  Model(IModelAgentPtr _pAgent, const std::string& _crsName );
  ~Model();       // Release memory
  int Release();  // Carefully clear model data

  /* Messenger support */
  ulong SendMessage(MessageType _mt, AlarmLevel _al, const std::string& _sender,
        ulong msg_code, const std::StringList& args = std::StringList() ) const;
  void  SendMessage(MessageType _mt, AlarmLevel _al, const std::string& _sender,
                    const std::string& _message ) const;
  void SendProgress( ulong pct ) const { pAgent->Progress( pct ); }
  std::string GetStr( ulong code ) const { return pAgent->GetString( code, "msg" ); }

  /************************************************************
   * Loaders
   ************************************************************/
public:
  /* Detect type of file and prepare to data loading; calls ReadApp_File(), TODO: ReadOldDgFile() */
  int Load( const std::string& _fName, int* pErrFlags );
private:
  /* Load all model components from *.dg file */
  int ReadApp_File(FILE* f, const std::string &_fName, int* ef );
  /* IManager implementation */
  virtual int ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 );
  virtual int WriteObjects( Token _token, FILE* _pFile, int _mode = 0 ) const;
public:
  /* Load mesh from file; calls LoadSonnetMeshFile() */
  int LoadMesh( const std::string& _fName );
private:
  /* Load mesh components from file */
  int LoadSonnetMeshFile( const std::string& _fName,
                          MeshCellDataArray* pMCDs,
                          StdStringArray* pStrs /*CMT*/,
                          long* pFingerprint /*???*/ );
public:
  /* Parse dg file and return text string */
  static std::string ReadText( FILE* _pFile, ulong& _rLines );

  /************************************************************
   * Writers
   ************************************************************/
public:
  /* Prepare to write dg file, write header; calls WriteApp_File() */
  int Save(const std::string& fName, FileMode mode ) const;
private:
  /* Write model data to file *.dg v115 */
  void WriteApp_File( FILE* _pFile ) const;

public:
  void DelObject( IComponentPtr obj );
  IComponentPtr GetLockingObject( IComponentPtr pObject );

  /* Write data of target elements. Extracted from WriteTargetFile */
  void WriteTarget( FILE* _pFile, IVarItemListPtr _pTarget, GridPointSegPtr _pGPS ) const;
  /* Produce DG template file from elements */
  int  WriteElemsAsDgTemplate( const std::string& _fileName, bool _bMarked ) const;

  int  WriteOutputFile( const std::string& _fileName ) const;
  /* Writes a structure file */
  int  WriteStructureFile( const std::string& _fileName ) const;

  int  WriteTargetsFile( const std::string& _fileName ) const;

public:
  void Notify( unsigned int type, IComponentPtr pObject );

  /* Analize vars and returns flags - validity of the (possible) output data */
  ulong GetOutputValidationFlags() const;
  int SendValidationResult( ulong _flags, const std::string& _sender ) const;
  bool ValidateOutputData() const { return SendValidationResult( GetOutputValidationFlags(), sender_name ) == 0; }


  /************************************************************
   * Subobjects manipulations
   ************************************************************/

  void ExcludeMarked( IComponentPtr pObject ) { Remove( markedList, pObject ); ReleaseObject( pObject ); }
  void IncludeMarked( IComponentPtr pObject ) { markedList.push_back( pObject->GetPtr() ); }
  bool HasMarked( IComponentInPtr _inpObject ) const { return Contains( markedList, _inpObject ); }
  bool HasMarked() const { return !markedList.empty(); }
  ulong GetMarkedTypes() const;

  void MarkObject( IComponentPtr _pObject, bool _include = true );
  void MarkVarItem( IVarItemPtr _pObject, bool _include = true );
  void MarkGroup( const IComponentList& _crGroup, bool _include = true );
  void MarkGroup( const IVarItemList& _crGroup, bool _include = true );
  void MarkGroup( ObjectType groupType, bool include = true );
  void UnmarkAll() { MarkGroup( markedList, false ); }

  void ExcludeHighlighted( IComponentPtr pObject ) { Remove( highlightedList, pObject ); ReleaseObject( pObject ); }
  void IncludeHighlighted( IComponentPtr pObject ) { highlightedList.push_back( pObject->GetPtr() ); }
  bool HasHighlighted( IComponentInPtr _inpObject ) const { return Contains( highlightedList, _inpObject ); }
  bool HasHighlighted() const { return !highlightedList.empty(); }

  void Highlight( IComponentPtr pObject, bool include = true );
  void UnhighlightAll();

public:
  /* Emit error message describing why an object is locked */
  void ShowLockReasonOf( IComponentPtr _pObject ) const;

  IVarOriginPtr FindObject( int _id ) const;
  int GetObjectId(const IComponentPtr _pObj ) const;

  /* After action process changes in model */
  void ProcessChanges();

  /************************************************************
   * Actions
   ************************************************************/

  ulong         AddSonnetData( const std::string& _fName );
  int           DeleteSonnetData();
  ulong         AddTemplate( const std::string& _fName, Point _incr,
                             double angle, double scale );
  NodePtr       AddNode( Point _pnt, bool checkIfExists = false );
  ElementPtr    AddElem( NodePtr _pN1, NodePtr _pN2 );
  SeparatorPtr  AddSeparator( Point _position, NodePtr pN );
  SourcePtr     AddSource( Point _position );
  ChordPtr      AddChord( Point _p1, Point _p2, bool b3d = false );
  CommentPtr    AddComment(ObjectType _ot, const Point& _crPosEnd,
                           const Point& _crPos, const std::wstring& _crwsText,
                           int _parentScene);
  int           AddMesh( const char* _fName );

  void SetOutputFlags( ulong flags );
  void SetMeshSlidingMode( ulong mode );
  void SetMeshSlidingThreshold( double value );
  void SetDoubleMeshBorderFlag( bool flag );
  void SetTopologyName( const std::string& _crsName );
  void SetFileName( const std::string& _crsName );
  void SetCreationTime( const std::string& _crsCreaTime );
  void SetOutputMode( ulong newMode );
  void SetSZRestriction( ulong _restriction );

  /************************************************************
   * Sonnet, Template, Elements, Nodes
   ************************************************************/
  int AppendTemplate();
  /* It is the caller's responsibility to delete result array. */
  ComponentListContainerPtrArray* CreateCellsInfo( int* pErr, IComponentPtr* ppObj ) const;

  ElementPtr FindElementByID( int id ) const;
  int ConvertChordsToElems( const IComponentList& _chords, IComponentPtr *ppErrObj );
  int ConvertElemsToChords( const IComponentList &_elems, IComponentPtr *ppErrObj );
  int ConvertTemplateToChords();

  IComponentList GetMarkedElements();

  /* Create elements between closest nodes pairs */
  int GlueNodes(double _maxDist, bool _markedOnly, int* _prCount);
  /* Merge elements forming an almost straight line */
  int GlueElems( double _maxDist, double _maxLen,
                 bool _cutLonger, bool _markedOnly, int* _prCount );
  /* Force external normals of all adjacend elements into one dir */
  int GlueNormals( bool _markedOnly, int* _prCount );
  /* Renumber all elements and separators. Separators are renumbered first */
  void RenumberElements();

  /************************************************************
   * Getters
   ************************************************************/
  const IComponentList& MarkedObjects() const { return markedList; }
  const IComponentList& UnusedNodes() const;

  IModelAgent* Agent() const { return pAgent; }

  bool IsUnsaved()          const;
  bool IsLocked()           const { return !!errorCode; }
  bool IsStricted()         const { return bStrict; }
  bool CarreOutputMode()    const { return outputMode == OUTPUTMODE::CARRE; }
  ulong OutputMode()        const { return outputMode; }
  ulong ShowFlags()         const { return showFlags; }
  int UpdateLocks()         const { return updateLocks; }
  bool HasOutputFlag( OutputFlag of ) const { return HasAnyFlag( outputFlags, of ); }
  int SZRestriction()       const { return szRestriction; }

  Point MinPosition() const { return min_position; }
  Point MaxPosition() const { return max_position; }
  double XYAngle()    const { return xyAngle; }

  const std::string& Name()     const { return title_name; }
  const std::string& FileName() const { return fileName; }
  const std::string& CreationTime() const { return creationTime; }
  CActionStack& ActionStack() { return actStack; }

  ulong NodesNum()      const { return nodes.size(); }
  ulong ElementsNum()   const { return elements.size(); }
  ulong ChordsNum()     const { return chords.size(); }
  ulong SeparatorsNum() const { return separators.size(); }
  ulong SourcesNum()    const { return sources.size(); }

  const IComponentList& Nodes()       const { return nodes; }
  const IComponentList& Elements()    const { return elements; }
  const IComponentList& Separators()  const { return separators; }
  const IComponentList& Sources()     const { return sources; }
  const IComponentList& Chords()      const { return chords; }
  const IComponentList& Comments()    const { return comments; }

  NodePtr FindNode( const Point& pnt ) const;
  SourcePtr FindSource( const Point& pnt ) const;
  ChordPtr FindChord( const Point& p1, const Point& p2, bool checkOrder = true ) const;

  IComponentPtr LockedNode() const;

  bool HasMesh( MeshPtr _pMesh )  const { return pMesh == _pMesh; }
  bool HasMesh()                  const { return pMesh != null; }
  bool HasTemplate()              const { return pTemplate != null; }
  bool HasSonnetData()            const { return pSonnetData != null; }
  bool HasSeparators()            const { return !separators.empty(); }

  FluxModel* GetFluxModel()     const { return pFlux; }
  MeshPtr GetMesh()             const { return pMesh; }
  TemplatePtr GetTemplate()     const { return pTemplate; }
  SonnetDataPtr GetSonnetData() const { return pSonnetData; }
  VarsManager* Vars()           const { return pVars; }

  int GetNextElemId();
  int MaxElemId() const { return maxElemId; }

  bool HasTopology()                          const { return !topologyName.empty(); }
  const std::string& TopologyName()           const { return !topologyName.empty() ? topologyName : topologyName_empty; }
  double MeshSlidingThreshold()               const { return meshSlidingThreshold; }
  bool CheckMeshSlidingMode( unsigned mode )  const { return (meshSlidingMode & mode) == mode; }
  bool HasDoubleMeshBorder()                  const { return doubleMeshBorder; }

  /************************************************************
   * Setters
   ************************************************************/
  void SetChangeFlag      ( ChangeFlag _cf )      { chFlags |= _cf; }
  void SetUpdateLocks     ( int _updateLocks )    { updateLocks = _updateLocks; }

  void SetMesh            ( MeshPtr _pMesh )
  { ReleaseObject( (IComponentPtr*)&pMesh ); pMesh = DG_SHARE( _pMesh ); }

  void SetTemplate        ( TemplatePtr _pT )
  { ReleaseObject( (IComponentPtr*)&pTemplate ); pTemplate = DG_SHARE( _pT ); }

  void SetSonnetData      ( SonnetDataPtr _pSD )
  { ReleaseObject( (IComponentPtr*)&pSonnetData ); pSonnetData = DG_SHARE( _pSD ); }

  void SetMaxElemId       ( int id );
  void SetViewAttributes  ( Point _minPosition, Point _maxPosition, double _xyAngle, long _showFlags )
  { min_position = _minPosition; max_position = _maxPosition; xyAngle = _xyAngle; showFlags = _showFlags; }

  void SetTitleName       ( const std::string& _title )
  { title_name = _title; sender_name = "Model-"; sender_name += title_name.substr( 0, title_name.size() - 3 ); }
};


#endif

