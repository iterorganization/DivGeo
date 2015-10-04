#include "IAction.h"
#include "Model.h"

namespace Actions {

void IAction::LogMessage( ulong _code, const std::StringList& _args ) const
{
  std::string sObjInfo = (pObject != null) ? pObject->DetailedInfo() : "model";
  switch( _code ) {
  case DG3::ACT_ADDED:
  case DG3::ACT_DELETED:
    pModel->SendMessage( LOG_INFO, sender_name, _code, ARGS( sObjInfo ) );
    break;
  case DG3::ACT_CHANGED: {
    std::stringstream ss;
    for( std::StringList::const_iterator it = _args.begin(), itEnd = _args.end();
         it != itEnd; ++it )
      ss << "\n" << *it;
    if( _args.empty() )
      ss << "no changes";
    pModel->SendMessage( LOG_INFO, sender_name, _code, ARGS( sObjInfo ) + ss.str() );
    break;
  }
  case DG3::ACT_UNDOMARK:
    pModel->SendMessage( LOG_INFO, sender_name, _code );
    break;
  default:
    pModel->SendMessage( LOG_INFO, sender_name, _code, ARGS( sObjInfo ) + _args );
    break;
  }
}

} // Actions
