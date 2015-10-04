#ifndef IACTION_H
#define IACTION_H

#include <assert.h>
#include <sstream>
#include <list>

#include "IObject.h"
#include "IComponent.h"
#include "IValueHolder.h"
//#include "Model.h"

class Model;

namespace Actions
{
  struct ACT {
  enum EActionType
  {
    UNKNOWN             = 0x000,
    ADD_ACTION          = 0x001,
    DEL_ACTION          = 0x002,
    CHANGE_ACTION       = 0x003,

    APP_UPDATE_LOCK     = 0x010,
    CHANGE_VALUE        = 0x013,
    CHANGE_OBJGROUP     = 0x014,
    UNDO_MARK           = 0x015,
    MARK_OBJECT         = 0x024,
    HIGHLIGHT_OBJECT    = 0x034,

    ADD_MESH            = 0x101,
    DEL_MESH            = 0x102,
    CHANGE_MESHPOINT    = 0x113,

    TECH_ACTION_INV     = 0x100, // Using: HasAnyFlag( ~flags, ACT::TECH_ACTION_INV )
    VAR_ACTION          = 0x200,

    ADD_VARSETDEF       = 0x211,
    DEL_VARSETDEF       = 0x212,
    CHANGE_VARSETDEF    = 0x213,

    ADD_VARDEF          = 0x221,
    DEL_VARDEF          = 0x222,
    CHANGE_VARDEF       = 0x223,

    ADD_VAR             = 0x231,
    DEL_VAR             = 0x232,

    ADD_VARSET          = 0x241,
    DEL_VARSET          = 0x242,

    ADD_SONNETDATA      = 0x301,
    DEL_SONNETDATA      = 0x302,

    ADD_NODE            = 0x311,
    DEL_NODE            = 0x312,
    CHANGE_NODE         = 0x313,

    ADD_ELEMENT         = 0x321,
    DEL_ELEMENT         = 0x322,
    CHANGE_ELEMENT      = 0x323,

    ADD_TEMPLATE        = 0x331,
    DEL_TEMPLATE        = 0x332,
    CHANGE_TEMPLATE     = 0x333,

    ADD_SEPARATOR       = 0x341,
    DEL_SEPARATOR       = 0x342,
    CHANGE_SEPARATOR    = 0x343,

    ADD_CHORD           = 0x411,
    DEL_CHORD           = 0x412,
    CHANGE_CHORD        = 0x413,

    ADD_SOURCE          = 0x421,
    DEL_SOURCE          = 0x422,
    CHANGE_SOURCE       = 0x423,

    ADD_COMMENT         = 0x431,
    DEL_COMMENT         = 0x432,

    ADD_EQUIL           = 0x501,
    DEL_EQUIL           = 0x502,

    ADD_GRIDPOINTEX     = 0x511,
    DEL_GRIDPOINTEX     = 0x512,
    CHANGE_GRIDPOINTEX  = 0x513,

    ADD_SURFACEEX       = 0x521,
    DEL_SURFACEEX       = 0x522,
    CHANGE_SURFACEEX    = 0x523,

    ADD_SURFACEZONE     = 0x531,
    DEL_SURFACEZONE     = 0x532,

    ADD_GRIDPOINTSEG    = 0x541,
    DEL_GRIDPOINTSEG    = 0x542,

    ADD_XPOINTSEG       = 0x551,
    DEL_XPOINTSEG       = 0x552,

    ADD_XPOINTTEST      = 0x561,
    DEL_XPOINTTEST      = 0x562,

    ADD_TOPOLOGY        = 0x571,
    DEL_TOPOLOGY        = 0x572,

    HIGH_LEVEL        = 0x10000,

    MASK_ACTION    = 0x0000000f,
    MASK_COMP      = 0x000000f0,
    MASK_GROUP     = 0x00000f00,

    GUI_STR        = 0x11100000
  };
  };

  typedef ACT::EActionType ActionType;

  class IAction
  {
  protected:
    ModelPtr pModel;
    IComponentPtr pObject;
    ActionType type;    
    std::string sender_name;

  public: 
    IAction( ModelPtr _pModel, IComponentPtr _pObject, ActionType _type = ACT::UNKNOWN ):
        pModel( _pModel ),
        pObject( _pObject == null ? null : _pObject->GetPtr() ),
        type( _type ),
        sender_name( "Action" ){}
    virtual ~IAction() { ReleaseObject( &pObject ); }

    ActionType Type() const { return type; }
    IComponentPtr Object() { return pObject; }

    virtual int Do() = 0;

    virtual bool IsEqualTo( const IAction* pAct ) const
    {
      return (pAct->type == type &&
              pAct->pObject == pObject);
    }
    virtual bool IsOppositeTo( const IAction* pAct ) const
    {
      UNUSED( pAct );
      return false;
    }

    void LogMessage( ulong _code, const std::StringList& _args = std::StringList() ) const;
  };

  typedef IAction* IActionPtr;
  typedef std::list<IAction*> ActionList;
  typedef ActionList::iterator ActionIter;
  typedef ActionList::const_iterator ActionIterConst;

} // Actions

#endif
