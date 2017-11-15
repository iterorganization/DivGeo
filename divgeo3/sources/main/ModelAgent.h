#ifndef MODELAGENT_H
#define MODELAGENT_H

#include <QObject>
#include "../common/IModelAgent.h"

#include "../common/UpdateInfo.h"
#include "Console.h"
#include "StringsManager.h"

class ModelAgent:
    public QObject,
    public IModelAgent
{
  Q_OBJECT

  Console* pConsole;
  StringsManager* pSM;

  UpdateInfo updateInfo;

public:
  ModelAgent( Console* _pConsole, StringsManager* _pSM );

  /* IModelAgent implementation */
  virtual void SetLoggingMode( LoggingMode _mode );
  virtual void send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
                     const std::string& _csText );
  virtual ulong send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
                      ulong _msg_code, const std::StringList& _crArgs = std::StringList() );
  virtual void Progress( ulong _pct );
  virtual std::string GetString( ulong _code, const char* _cpsDictionary ) const;
  virtual void AddToUpdate( IComponentPtr _pObject, UpdateType _ut, bool _delayed = true );
  virtual void AddToUpdate( const IComponentList& _crList, UpdateType _ut, bool _delayed = true );
  virtual void Update( bool _force = true ); //1409 force
  virtual void Clear();

signals:
  void UpdateViews( const UpdateInfo& _crUI, bool _force ); //1409
};

#endif // MODELAGENT_H
