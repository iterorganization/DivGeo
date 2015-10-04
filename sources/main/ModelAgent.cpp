#include "ModelAgent.h"

ModelAgent::ModelAgent(Console* _pConsole, StringsManager* _pSM ):
  QObject( null ),
  pConsole( _pConsole ),
  pSM( _pSM )
{
}

void ModelAgent::SetLoggingMode( LoggingMode _mode )
{
  pConsole->SetLoggingMode( _mode );
}

void ModelAgent::send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
           const std::string& _csText )
{
  pConsole->Send( _mt, _al, QString::fromStdString( _csSender ), QString::fromStdString( _csText ) );
}

ulong ModelAgent::send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
            ulong _msg_code, const std::StringList& _crArgs )
{
  QString sText = SM_MSG( _msg_code );
  if( sText.isEmpty() )
    sText = SM_DLG( _msg_code );
  if( _msg_code == DG3::FAIL )
    pConsole->SetFailFlag();
  FOREACH_CONST( std::StringList::const_iterator, it, _crArgs )
    sText = sText.arg( QString::fromStdString( *it ) );

  pConsole->Send( _mt, _al, QString::fromStdString( _csSender ), sText );
  return _msg_code;
}

void ModelAgent::Progress( ulong _pct )
{
  pConsole->Progress( _pct );
}

std::string ModelAgent::GetString( ulong _code, const char* _cpsDictionary ) const
{
  return pSM->GetString( _code, _cpsDictionary ).toStdString();
}

void ModelAgent::AddToUpdate( IComponentPtr _pObject, UpdateType _ut, bool _delayed )
{
  updateInfo.Append( _pObject, _ut );
  if( !_delayed )
    Update();
}

void ModelAgent::AddToUpdate( const IComponentList& _crList, UpdateType _ut, bool _delayed )
{
  updateInfo.Append( _crList, _ut );
  if( !_delayed )
    Update();
}

void ModelAgent::Update()
{
  emit UpdateViews( updateInfo );
  updateInfo.Clear();
}

void ModelAgent::Clear()
{
  updateInfo.Clear();
}
