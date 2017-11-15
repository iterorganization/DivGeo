#ifndef INOTIFICATOR_H
#define INOTIFICATOR_H

#include <string>
#include <vector>
#include "StdStringList.h"

typedef unsigned long ulong;

#define SENDER_NAME( str ) std::string dg__sender_name__( str )
#define FULL_SENDER sender_name + ": " + dg__sender_name__
#define SENDER dg__sender_name__
#define ARGS std::StringList
#define ARG( v ) std::StringList( ToStdString( v ) )
#define _ARG( v ) + ToStdString( v )

#define LOG_INFO      MT::LOG,    AL::INFO
#define LOG_ERROR     MT::LOG,    AL::ERROR
#define LOG_WARNING   MT::LOG,    AL::WARNING
#define LOG_RESULT    MT::LOG,    AL::RESULT
#define LOG_WIP_INFO  MT::LOG,    AL::WIP_INFO
#define LOG_WIP_WARN  MT::LOG,    AL::WIP_WARN

#define FATAL_ERROR   MT::WINDOW, AL::FATAL

#define WND_ERROR     MT::WINDOW, AL::ERROR
#define WND_PROCESS   MT::WINDOW, AL::PROCESS
#define WND_WIP_INFO  MT::WINDOW, AL::WIP_INFO
#define WND_WIP_WARN  MT::WINDOW, AL::WIP_WARN

#define STATUS_WARN   MT::STATUS, AL::WARNING
#define STATUS_INFO   MT::STATUS, AL::INFO

#define DYN_INFO      MT::DYNAMIC,AL::INFO

struct SMessageTypes
{
  enum EMessageTypes
  {
    LOG     = 0, // log
    WINDOW  = 1, // log + window
    STATUS  = 2, // log + status
    DYNAMIC = 3, // status
    ALL     = 4  // log + window + status
  };
};

typedef SMessageTypes::EMessageTypes MessageType;
typedef SMessageTypes MT;

struct SAlarmLevels
{
  enum EAlarmLevels
  {
    INFO    = 0, // Just informational message
    WARNING = 1, // Message about some troubles
    ERROR   = 2, // Reason why some action cann't be completed. Breaks processes.
    FATAL   = 3, // Crash: causes exit from application

    PROCESS   = 10, // waiting for RESULT
    WIP_INFO  = 11, // Informational message can be shown in IGNORE message mode
    WIP_WARN  = 12, // Warning message can be shown in IGNORE message mode
    RESULT    = 13 // Ending of PROCESS message

    //LIST_INFO = 21, // List of collected messages as information
    //LIST_ERR  = 22  // List of collected messages as error
  };
};

typedef SAlarmLevels::EAlarmLevels AlarmLevel;
typedef SAlarmLevels AL;

struct SLoggingModes
{
  enum ELoggingModes
  {
    USUAL,  // All messages output to log
    IGNORE, // Ignore messages except process and status information
    COLLECT // Collect messages to output or ignore together
  };
};

typedef SLoggingModes::ELoggingModes LoggingMode;
typedef SLoggingModes LM;

struct SUpdateTypes {
  enum EUpdateTypes { CREATE, REMOVE, UPDATE, USTYLE }; };
typedef SUpdateTypes::EUpdateTypes UpdateType;
typedef SUpdateTypes UT;

class IComponent;
typedef IComponent* IComponentPtr;
typedef std::list< IComponentPtr > IComponentList;

class IModelAgent
{
public:
  IModelAgent() {}

  /* Logging */
  virtual void SetLoggingMode( LoggingMode _mode ) = 0;
  virtual void send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
                     const std::string& _csText ) = 0;
  virtual ulong send( MessageType _mt, AlarmLevel _al, const std::string& _csSender,
                      ulong _msg_code, const std::StringList& _crArgs = std::StringList() ) = 0;
  virtual void Progress( ulong _pct ) = 0;

  /* Resources */
  virtual std::string GetString( ulong _code, const char* _cpsDictionary ) const = 0;

  /* Update */
  virtual void AddToUpdate( IComponentPtr _pObject, UpdateType _ut, bool _delayed = true ) = 0;
  virtual void AddToUpdate( const IComponentList& _crList, UpdateType _ut, bool _delayed = true ) = 0;
  virtual void Update( bool _force = true ) = 0; //1409 force
  virtual void Clear() = 0;
};

typedef IModelAgent* IModelAgentPtr;

#endif // IMESSENGER_H
