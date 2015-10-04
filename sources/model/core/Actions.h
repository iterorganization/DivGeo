#ifndef ACTIONS_H
#define ACTIONS_H

#include "IAction.h"

#include "../vars/VarsDecl.h"
#include "../sonnet/SonnetDecl.h"
#include "../diagnostics/DiagnosticsDecl.h"
#include "../flux/EquilDecl.h"
#include "../mesh/MeshDecl.h"
#include "../material/MaterialDecl.h"
#include "../flux/Topology.h"

#define AT_ONCE_ARGS bool doAtOnce = false
#define AT_ONCE     if( doAtOnce ) Do();
#define DO_AT_ONCE true
#define REDRAW true
#define NO_REDRAW false
#define INCLUDE true
#define EXCLUDE false

class Model;
typedef Model* ModelPtr;

namespace Actions
{
  class ActHighLevel: public IAction
  {
    ActionList actions;
    bool isComplete;
    std::string sDescr;

  public:
    ActHighLevel( ModelPtr _pModel, const std::string& _crsDescr = "" ):
      IAction( _pModel, null, ACT::HIGH_LEVEL ),
      isComplete( false ),
      sDescr( _crsDescr ) {}

    virtual ~ActHighLevel();

    bool AddAction( IAction* _pAct );

    ActHighLevel& operator << ( IAction* _pAct )
    { AddAction( _pAct ); return *this; }

    virtual int Do();

    void SetComplete( const std::string& _crsDescr = "" )
    { if( isComplete ) return; isComplete = true; sDescr = _crsDescr; }
    bool IsComplete() const { return isComplete; }

    ulong Size() const { return actions.size(); }
    const std::string& Descr() const { return sDescr; }

    const ActionList& Actions() const { return actions; }
  };

  class ActAddMesh: public IAction
  {
  public:
    ActAddMesh( ModelPtr _pModel, MeshPtr _pMesh, AT_ONCE_ARGS ):
      IAction( _pModel, (IComponentPtr)_pMesh, ACT::ADD_MESH ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelMesh: public IAction
  {
  public:
    ActDelMesh( ModelPtr _pModel, MeshPtr _pMesh, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pMesh, ACT::DEL_MESH ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeValue: public IAction
  {
  protected:
    IValueHolderPtr pHolder;
    unsigned char id;
    UPtr value;
    bool bRedraw;
  public:
    ActChangeValue( ModelPtr _pModel, IComponentPtr _pObject, IValueHolderPtr _pHolder,
                    unsigned char _id, const UPtr& _crValue, bool _bRedraw, AT_ONCE_ARGS ):
        IAction( _pModel, _pObject, ACT::CHANGE_VALUE ),
        pHolder( _pHolder ),
        id( _id ),
        value( _crValue.Copy() ),
        bRedraw( _bRedraw )
    {
      if( (IValueHolderPtr)pModel == pHolder ) // Model can not be an IComponent
        pObject = null;
      else
        assert( pObject != null );

      AT_ONCE
    }
    virtual int Do();
    IValueHolderPtr Holder() const { return pHolder; }
    unsigned char ID() const { return id; }
    const UPtr& Value() const { return value; }

    virtual bool IsEqualTo( const IAction* _pAct ) const
    {
      const ActChangeValue* pAct = static_cast< const ActChangeValue* >( _pAct );
      return (pAct->type    == type &&
              pAct->pObject == pObject &&
              pAct->pHolder == pHolder &&
              pAct->id      == id &&
              //pAct->value.IsEqual( value ) &&
              pAct->bRedraw == bRedraw);
    }
  };


  class ActChangeMeshPoint: public IAction
  {
    Point position;

  public:
    ActChangeMeshPoint( ModelPtr _pModel, MeshPoint* _pMPoint, Point _position, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pMPoint, ACT::CHANGE_MESHPOINT ),
        position( _position ) { AT_ONCE }
    virtual int Do();
    const Point& Position() const { return position; }
    MeshPoint* MPoint() { return (MeshPoint*)pObject; }
  };


  class ActUndoMark: public IAction
  {
  public:
    ActUndoMark( ModelPtr _pModel, AT_ONCE_ARGS ):
        IAction( _pModel, null, ACT::UNDO_MARK ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddVarSetDef: public IAction
  {
  public:
    ActAddVarSetDef( ModelPtr _pModel, VarSetDefPtr _pVSD, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVSD, ACT::ADD_VARSETDEF ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelVarSetDef: public IAction
  {
  public:
    ActDelVarSetDef( ModelPtr _pModel, VarSetDefPtr _pVSD, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVSD, ACT::DEL_VARSETDEF ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeVarSetDef: public IAction
  {
    int minVarSets;
    int maxVarSets;
  public:
    ActChangeVarSetDef( ModelPtr _pModel, VarSetDefPtr _pVSD,
                        int _minVarSets, int _maxVarSets, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVSD, ACT::CHANGE_VARSETDEF ),
        minVarSets( _minVarSets ),
        maxVarSets( _maxVarSets ) { AT_ONCE }
    virtual int Do();
    int MinVarSets() const { return minVarSets; }
    int MaxVarSets() const { return maxVarSets; }
  };


  class ActAddVarDef: public IAction
  {
  public:
    ActAddVarDef( ModelPtr _pModel, VarDefPtr _pVD, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVD, ACT::ADD_VARDEF ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelVarDef: public IAction
  {
  public:
    ActDelVarDef( ModelPtr _pModel, VarDefPtr _pVD, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVD, ACT::DEL_VARDEF ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeVarDef: public IAction
  {
    VarSetDef* pVSD;
    VarDefData* pData;
  public:
    ActChangeVarDef( ModelPtr _pModel, VarDef* _pVD, VarSetDef* _pVSD, const VarDefData& _crVdData, AT_ONCE_ARGS );
    ~ActChangeVarDef();
    virtual int Do();
    VarSetDef* VSD() const { return pVSD; }
    VarDefData* Data() const { return pData; }
  };


  class ActAddVarSet: public IAction
  {
  public:
    ActAddVarSet( ModelPtr _pModel, VarSetPtr _pVS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVS, ACT::ADD_VARSET ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelVarSet: public IAction
  {
  public:
    ActDelVarSet( ModelPtr _pModel, VarSetPtr _pVS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pVS, ACT::DEL_VARSET ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddVar: public IAction
  {
  public:
    ActAddVar( ModelPtr _pModel, VarPtr _pV, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pV, ACT::ADD_VAR ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelVar: public IAction
  {
  public:
    ActDelVar( ModelPtr _pModel, VarPtr _pV, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pV, ACT::DEL_VAR ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeGroup: public IAction
  {
    //ModelPtr pModel; //my wtf?
    IVarItemPtr pItem;
    bool include;
    bool bRedraw;
  public:
    ActChangeGroup( ModelPtr _pModel, VarPtr _pVar, IVarItemPtr _pItem,
                    bool _include, bool _bRedraw, AT_ONCE_ARGS ):
      IAction( _pModel, (IComponentPtr)_pVar, ACT::CHANGE_OBJGROUP ),
        pItem( _pItem ),
        include( _include ),
        bRedraw( _bRedraw ) { AT_ONCE }
    virtual int Do();
    IVarItemPtr VarItem() const { return pItem; }
    bool Include() const { return include; }
    virtual bool IsOppositeTo( const IAction *_pAct ) const
    {
      const ActChangeGroup* pAct = static_cast< const ActChangeGroup* >( _pAct );
      return (pAct->type    == type &&
              pAct->pObject == pObject &&
              pAct->pItem   == pItem &&
              pAct->include != include &&
              pAct->bRedraw == bRedraw);
    }
  };

  
  class ActMarkObject: public IAction
  {
    bool include;
  public:
    ActMarkObject( ModelPtr _pModel, IComponentPtr _pObject, bool _include, AT_ONCE_ARGS ):
        IAction( _pModel, _pObject, ACT::MARK_OBJECT ),
        include( _include ) { AT_ONCE }
    virtual int Do();
    bool Include() const { return include; }
    virtual bool IsOppositeTo( const IAction *_pAct ) const
    {
      const ActMarkObject* pAct = static_cast< const ActMarkObject* >( _pAct );
      return (pAct->type    == type &&
              pAct->pObject == pObject &&
              pAct->include == include);
    }
  };


  class ActHighlightObject: public IAction
  {
    bool include;
  public:
    ActHighlightObject( ModelPtr _pModel, IComponentPtr _pObject, bool _include, AT_ONCE_ARGS ):
        IAction( _pModel, _pObject, ACT::HIGHLIGHT_OBJECT ),
        include( _include ) { AT_ONCE }
    virtual int Do();
    bool Include() const { return include; }
    virtual bool IsOppositeTo( const IAction *_pAct ) const
    {
      const ActHighlightObject* pAct = static_cast< const ActHighlightObject* >( _pAct );
      return (pAct->type    == type &&
              pAct->pObject == pObject &&
              pAct->include == !include);
    }
  };


  class ActAddNode: public IAction
  {
  public:
    ActAddNode( ModelPtr _pModel, NodePtr _pNode, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pNode, ACT::ADD_NODE ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelNode: public IAction
  {
  public:
    ActDelNode( ModelPtr _pModel, NodePtr _pNode, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pNode, ACT::DEL_NODE ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeNode: public IAction
  {
    Point position;
  public:
    ActChangeNode( ModelPtr _pModel, NodePtr _pNode, const Point& _crPosition, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pNode, ACT::CHANGE_NODE ),
        position( _crPosition ) { AT_ONCE }
    virtual int Do();
    const Point& Position() const { return position; }
  };


  class ActAddElem: public IAction
  {
  public:
    ActAddElem( ModelPtr _pModel, ElementPtr _pElem, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pElem, ACT::ADD_ELEMENT ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelElem: public IAction
  {
  public:
    ActDelElem( ModelPtr _pModel, ElementPtr _pElem, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pElem, ACT::DEL_ELEMENT ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeElem: public IAction
  {
    NodePtr n1, n2;
    int id;
  public:
    ActChangeElem( ModelPtr _pModel, ElementPtr _pElem, NodePtr _n1, NodePtr _n2, int _id, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pElem, ACT::CHANGE_ELEMENT ),
        n1( _n1 ), n2( _n2 ), id( _id ) { AT_ONCE }
    NodePtr Node1() const { return n1; }
    NodePtr Node2() const { return n2; }
    int ID() const { return id; }
    virtual int Do();
  };


  class ActAddTemplate: public IAction
  {
  public:
    ActAddTemplate( ModelPtr _pModel, TemplatePtr _pTemplate, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pTemplate, ACT::ADD_TEMPLATE ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeTemplate: public IAction
  {
    TemplateParams* pTP;
  public:
    ActChangeTemplate( ModelPtr _pModel, TemplatePtr _pTemplate, const TemplateParams &_tp, AT_ONCE_ARGS );
    ~ActChangeTemplate();
    virtual int Do();
    const TemplateParams& Params() const { return *pTP; }
  };
  
  
  class ActDelTemplate: public IAction
  {
  public:
    ActDelTemplate( ModelPtr _pModel, TemplatePtr _pTemplate, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pTemplate, ACT::DEL_TEMPLATE ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddSonnetData: public IAction
  {
  public:
    ActAddSonnetData( ModelPtr _pModel, SonnetDataPtr _pSonnetData, AT_ONCE_ARGS ):
        IAction( _pModel, _pSonnetData, ACT::ADD_SONNETDATA ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelSonnetData: public IAction
  {
  public:
    ActDelSonnetData( ModelPtr _pModel, SonnetDataPtr _pSonnetData, AT_ONCE_ARGS ):
        IAction( _pModel, _pSonnetData, ACT::DEL_SONNETDATA ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddSeparator: public IAction
  {
  public:
    ActAddSeparator( ModelPtr _pModel, SeparatorPtr _pSeparator, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSeparator, ACT::ADD_SEPARATOR ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelSeparator: public IAction
  {
  public:
    ActDelSeparator( ModelPtr _pModel, SeparatorPtr _pSeparator, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSeparator, ACT::DEL_SEPARATOR ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeSeparator: public IAction
  {
    Point position;
    NodePtr pNode;
    int id;
  public:
    ActChangeSeparator( ModelPtr _pModel, SeparatorPtr _pSeparator,
                        const Point& _crPosition, NodePtr _pNode, int _id, AT_ONCE_ARGS ):
      IAction( _pModel, (IComponentPtr)_pSeparator, ACT::CHANGE_SEPARATOR ),
      position( _crPosition ), pNode( _pNode ), id( _id ) { AT_ONCE }
    const Point& Position() const { return position; }
    NodePtr Node() const { return pNode; }
    int ID()  const { return id; }
    virtual int Do();
  };


  class ActAddSource: public IAction
  {
  public:
    ActAddSource( ModelPtr _pModel, SourcePtr _pSource, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSource, ACT::ADD_SOURCE ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelSource: public IAction
  {
  public:
    ActDelSource( ModelPtr _pModel, SourcePtr _pSource, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSource, ACT::DEL_SOURCE ) { AT_ONCE }
    virtual int Do();
  };


  class ActChangeSource: public IAction
  {
    Point position;
  public:
    ActChangeSource( ModelPtr _pModel, SourcePtr _pSource, Point _position, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSource, ACT::CHANGE_SOURCE ),
        position( _position ) { AT_ONCE }
    const Point& Position() const { return position; }
    virtual int Do();
  };


  class ActAddChord: public IAction
  {
  public:
    ActAddChord( ModelPtr _pModel, ChordPtr _pChord, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pChord, ACT::ADD_CHORD ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelChord: public IAction
  {
  public:
    ActDelChord( ModelPtr _pModel, ChordPtr _pChord, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pChord, ACT::DEL_CHORD ) { AT_ONCE }
    virtual int Do();
  };

  class ActChangeChord: public IAction
  {
    Point p1, p2;
  public:
    ActChangeChord( ModelPtr _pModel, ChordPtr _pChord,
                    const Point& _crP1, const Point& _crP2, AT_ONCE_ARGS ):
    IAction( _pModel, (IComponentPtr)_pChord, ACT::CHANGE_CHORD ),
      p1( _crP1 ), p2( _crP2 ) { AT_ONCE }
    virtual int Do();
    const Point& P1() const { return p1; }
    const Point& P2() const { return p2; }
  };

  
  class ActAddEquil: public IAction
  {
  public:
    ActAddEquil( ModelPtr _pModel, EquilPtr _pEquil, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pEquil, ACT::ADD_EQUIL ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelEquil: public IAction
  {
  public:
    ActDelEquil( ModelPtr _pModel, EquilPtr _pEquil, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pEquil, ACT::DEL_EQUIL ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddTopology: public IAction
  {
  public:
    ActAddTopology( ModelPtr _pModel, TopologyPtr _pTopology, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pTopology, ACT::ADD_TOPOLOGY ) { AT_ONCE }
    virtual int Do();
  };


  class ActDelTopology: public IAction
  {
  public:
    ActDelTopology( ModelPtr _pModel, TopologyPtr _pTopology, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pTopology, ACT::DEL_TOPOLOGY ) { AT_ONCE }
    virtual int Do();
  };


  class ActAddGridPointEx: public IAction
  {
  public:
    ActAddGridPointEx( ModelPtr _pModel, GridPointExPtr _pGPX, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pGPX, ACT::ADD_GRIDPOINTEX ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelGridPointEx: public IAction
  {
   public:
    ActDelGridPointEx( ModelPtr _pModel, GridPointExPtr _pGPX, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pGPX, ACT::DEL_GRIDPOINTEX ) { AT_ONCE }
    virtual int Do();
  };

  class ActChangeGridPointEx: public IAction
  {
    int zone;
    double value;
  public:
    ActChangeGridPointEx( ModelPtr _pModel, GridPointExPtr _pGPX, int _zone, double _value, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pGPX, ACT::CHANGE_GRIDPOINTEX ),
        zone( _zone ), value( _value ) { AT_ONCE }
    int Zone() const { return zone; }
    double Value() const { return value; }
    virtual int Do();
  };

  
  class ActAddSurfaceEx: public IAction
  {
  public:
    ActAddSurfaceEx( ModelPtr _pModel, SurfaceExPtr _pSX, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSX, ACT::ADD_SURFACEEX ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelSurfaceEx: public IAction
  {
   public:
    ActDelSurfaceEx( ModelPtr _pModel, SurfaceExPtr _pSX, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSX, ACT::DEL_SURFACEEX ) { AT_ONCE }
    virtual int Do();
  };

  class ActChangeSurfaceEx: public IAction
  {
    int zone;
    double level;
    Point xy;
  public:
    ActChangeSurfaceEx( ModelPtr _pModel, SurfaceExPtr _pSX, int _zone, double _level, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSX, ACT::CHANGE_SURFACEEX ),
        zone( _zone ), level( _level ), xy( 0. ) { AT_ONCE }
    ActChangeSurfaceEx( ModelPtr _pModel, SurfaceExPtr _pSX, Point _xy, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSX, ACT::CHANGE_SURFACEEX ),
        zone( SZN_XY ), level( 0 ), xy( _xy ) { AT_ONCE }
    virtual int Do();
    int Zone() const { return zone; }
    double Level() const { return level; }
    const Point& XY() const { return xy; }
  };



  class ActAddSurfaceZone: public IAction
  {
  public:
    ActAddSurfaceZone( ModelPtr _pModel, SurfaceZonePtr _pSZ, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSZ, ACT::ADD_SURFACEZONE ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelSurfaceZone: public IAction
  {
  public:
    ActDelSurfaceZone( ModelPtr _pModel, SurfaceZonePtr _pSZ, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pSZ, ACT::DEL_SURFACEZONE ) { AT_ONCE }
    virtual int Do();
  };



  class ActAddGridPointSeg: public IAction
  {
  public:
    ActAddGridPointSeg( ModelPtr _pModel, GridPointSegPtr _pGPS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pGPS, ACT::ADD_GRIDPOINTSEG ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelGridPointSeg: public IAction
  {
  public:
    ActDelGridPointSeg( ModelPtr _pModel, GridPointSegPtr _pGPS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pGPS, ACT::DEL_GRIDPOINTSEG ) { AT_ONCE }
    virtual int Do();
  };



  class ActAddXPointSeg: public IAction
  {
  public:
    ActAddXPointSeg( ModelPtr _pModel, XPointSegPtr _pXPS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pXPS, ACT::ADD_XPOINTSEG ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelXPointSeg: public IAction
  {
  public:
    ActDelXPointSeg( ModelPtr _pModel, XPointSegPtr _pXPS, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pXPS, ACT::DEL_XPOINTSEG ) { AT_ONCE }
    virtual int Do();
  };



  class ActAddXPointTest: public IAction
  {
  public:
    ActAddXPointTest( ModelPtr _pModel, XPointTestPtr _pXPT, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pXPT, ACT::ADD_XPOINTTEST ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelXPointTest: public IAction
  {
  public:
    ActDelXPointTest( ModelPtr _pModel, XPointTestPtr _pXPT, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pXPT, ACT::DEL_XPOINTTEST ) { AT_ONCE }
    virtual int Do();
  };



  class ActAddComment: public IAction
  {
  public:
    ActAddComment( ModelPtr _pModel, CommentPtr _pComment, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pComment, ACT::ADD_COMMENT ) { AT_ONCE }
    virtual int Do();
  };

  class ActDelComment: public IAction
  {
  public:
    ActDelComment( ModelPtr _pModel, CommentPtr _pComment, AT_ONCE_ARGS ):
        IAction( _pModel, (IComponentPtr)_pComment, ACT::DEL_COMMENT ) { AT_ONCE }
    virtual int Do();
  };


} // Actions

#endif
