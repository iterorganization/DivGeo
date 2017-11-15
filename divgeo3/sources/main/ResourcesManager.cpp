#include "ResourcesManager.h"
#include "Console.h"

ResourcesManager::ResourcesManager(Console* _pConsole , const QString& _crsFilename):
  pConsole( _pConsole ),
  sFilename( _crsFilename ),
  isLoaded( false ),
  codes( "/" ),
  plainTextMode( false )
{
  unknown.append( "< ??? >" );
}

const QStringList& ResourcesManager::GetStrings( ulong _code, const QString& _crsDictionary ) const
{
  DataValue* pValue = Value( _code, _crsDictionary );
  if( pValue != null )
    return pValue->strings;
  unknown.clear();
  unknown.append( QString( "%1:%2" ).arg(_crsDictionary ).arg( _code ) );
  return unknown;
}

const QString& ResourcesManager::GetString( ulong _code, const QString& _crsDictionary ) const
{
  return GetStrings( _code, _crsDictionary ).first();
}

DataValue* ResourcesManager::Value( ulong _code, const QString& _crsDictionary ) const
{
  DictionariesMap::const_iterator it1 = dictionaries.find( _crsDictionary );
  if( it1 == dictionaries.end() )
    return null;
  const Dictionary& crDict = it1.value();
  Dictionary::const_iterator it2 = crDict.find( _code );
  if( it2 == crDict.end() )
    return null;
  return it2.value();
}

void ResourcesManager::LoadStrings( bool _loadUnregistered )
{
  static const QString scsErr_NoFile = "Can not find resource file: %1";
  static const QString scsErr_AtLine = " has error at line %1. Line is skipped.";
  static const QString scsErr_WrongGroup = " has wrong group name at line %1";
  static const QString scsErr_NoGroup = "No group selected. Line %1 is skipped.";
  static const QString scsErr_NoSuchKey = "No such key \"%1\" found. Line %2 is skipped.";
  static const QString scsErr_NotLoaded = "Some strings couldn't be loaded, key names will be used instead: ";

  ulong noregStrings = 0;

  pConsole->Send( LOG_INFO, "", QString( "Loading resources from %1..." ).arg( sFilename ) );
  QDir dir( QApplication::applicationDirPath() );
  QFile file( dir.absolutePath() + "/" + sFilename );
  if( !file.exists() ) {
    dir.cdUp();
    file.setFileName( dir.absolutePath() + "/" + sFilename );
    if( !file.exists() )
      pConsole->Send( LOG_ERROR, "", scsErr_NoFile.arg( sFilename ) );
  }

  if( file.exists() ) {
    file.open( QFile::ReadOnly );

    Dictionary* pStrings = null;
    Dictionary* pStrings_local = null;
    TreeNode* pNode = null;
    ulong code = DataValue::EMPTY;
    QString sPath_global;
    QString sKey;
    QString sPlainText;
    QString text = file.readAll();
    QStringList lines = text.split( "\n" );
    ulong line_c = 0;
    foreach( const QString& line, lines ) {
      line_c++;
      if( line.trimmed().isEmpty() )
        continue;
      QString subline;
      long split_position = -1;
      for( long c = 0; c < line.size(); c++ )
        if( line.at(c) == '#' && (c == 0 || line.at(c-1) != '\\') ) {
          split_position = c;
          break;
        }

      if( split_position == -1 )
        subline = line.trimmed();
      else if( split_position == 0 )
        subline = "";
      else
        subline = line.left( split_position ).trimmed();

      // Ignore empty line
      if( subline.isEmpty() )
        continue;

      // PLAINTEXT mode
      if( plainTextMode ) {
        int tag = subline.indexOf( "</PLAINTEXT>" );
        if( tag >= 0 )
          subline = subline.left( tag );
        sPlainText.append( subline + '\n' );
        if( tag != -1 ) {
          plainTextMode = false;
          pNode->SetValue( sKey, QStringList( sPlainText ), true );
          sPlainText.clear();
        }
        continue;
      }

      // Append data for previous key
      if( subline[0] == '\"' ) {
        if( subline.length() < 2 || subline[subline.length()-1] != '\"' ) {
          pConsole->Send( LOG_ERROR, "", QString( sFilename + scsErr_AtLine ).arg( line_c ) );
          continue;
        }
        QStringList strs_new;
        if( !ParseTextString( subline, strs_new ) ) {
          pConsole->Send( LOG_ERROR, "", QString( sFilename + scsErr_AtLine).arg( line_c ) );
          continue;
        }

        pNode->SetValue( sKey, strs_new, true ); // Append
        continue;
      }
      // Select keys and destination dictionaries if possible
      else if( subline[0] == '/' ) {
        sPath_global = subline.mid(1);

        if( subline.length() == 1 )
          pNode = &codes;
        else
          pNode = codes.Node( subline.mid(1) );

        QString sTopLevel = subline.split( '/', QString::SkipEmptyParts )[0].trimmed();

        pStrings = &dictionaries[ sTopLevel ];

        if( !_loadUnregistered && (pStrings == null || pNode == null) )
          pConsole->Send( LOG_ERROR, "", QString( sFilename + scsErr_WrongGroup ).arg( line_c ) );
        continue;
      }
      else if( pNode == null && !_loadUnregistered) {
        pConsole->Send( LOG_ERROR, "", scsErr_NoGroup.arg( line_c ) );
        continue;
      }

      // Cut data line in 2 parts: key and data
      int eqv = subline.indexOf( QChar( '=' ) );
      if( eqv == -1 ) {
        pConsole->Send( LOG_ERROR, "", QString( sFilename + scsErr_AtLine ).arg( line_c ) );
        continue;
      }

      // Parse key and find value
      sKey = subline.left( eqv ).trimmed();
      QString str = subline.mid( eqv+1 ).trimmed();
      pStrings_local = null;
      code = (pNode != null) ? pNode->Code( sKey, true ) : DataValue::EMPTY;
      if( code == DataValue::EMPTY && !_loadUnregistered ) {
        pConsole->Send( LOG_ERROR, "", scsErr_NoSuchKey.arg( sKey ).arg( line_c ) );
        continue;
      }

      QStringList path = sKey.split( "." );
      if( pStrings == null ) {
        if( path.size() < 2 ) {
          pConsole->Send( LOG_ERROR, "", scsErr_NoSuchKey.arg( sKey ).arg( line_c ) );
          continue;
        }

        DictionariesMap::iterator it = dictionaries.find( path[0] );
        if( it == dictionaries.end() && !_loadUnregistered ) {
          pConsole->Send( LOG_ERROR, "", scsErr_NoSuchKey.arg( sKey ).arg( line_c ) );
          continue;
        }
        pStrings_local = &it.value();
      }
      else
        pStrings_local = pStrings;

      // Parse data part
      QStringList strs;
      if( !ParseTextString( str, strs ) ) {
        pConsole->Send( LOG_ERROR, "", QString( sFilename + scsErr_AtLine ).arg( line_c ) );
        continue;
      }

      for( int c = 0; c < strs.size(); c++ )
        strs[c].replace( "\\n", "\n" );

      if( code != DataValue::EMPTY ) {
        pNode->SetValue( sKey, strs );
        DataValue* pValue = pNode->Value( sKey );
        pStrings_local->insert( code, pValue );
      }
      else {
        codes.SetValue( sPath_global + "/" + sKey, strs );
        noregStrings++;
      }
    }
  }

  file.close();

  isLoaded = true;

  // Reports
  /*static const QString sReport = "%1 strings are loaded into %2 dictionary";
  for( DictionariesMap::const_iterator it = dictionaries.begin();
       it != dictionaries.end(); it++ )
    pConsole->Send( LOG_INFO, "", sReport.arg( it.value().size() ).arg( it.key() ) );
  pConsole->Send( LOG_INFO, "", QString( "%1 not registered strings loaded" ).arg( noregStrings ) );*/

  int totalStrings = 0;
  for( DictionariesMap::const_iterator it = dictionaries.begin();
       it != dictionaries.end(); it++ )
    totalStrings += it.value().size();
  pConsole->Send( LOG_INFO, "", QString( "%1 strings loaded from %2" )
                  .arg( totalStrings ).arg( sFilename ) );

  // Not loaded strings
  const QStringList& other_list = codes.GetUnmarked();
  if( !other_list.isEmpty() ) {
    QString other( scsErr_NotLoaded );
    foreach( const QString& other_str, other_list ) {
      other.append( other_str );
      other.append( ", " );
      QStringList path = other_str.split( "." );
      Dictionary* pStrings_local = null;
      DictionariesMap::iterator it = dictionaries.find( path[0] );
      if( it == dictionaries.end() )
        continue;
      pStrings_local = &it.value();
      path.removeFirst();
      DataValue* pValue = codes.Value( other_str );
      pValue->mark = true;
      //ulong code = codes.Code( other_str, true );
      if( path.last() == "menu" || path.last() == "title" )
        path.removeLast();
      pValue->strings = QStringList( path.last() );
      pStrings_local->insert( pValue->code, pValue );
    }
    other.truncate( other.length() - 2 );
    other.append( "." );
    pConsole->Send( LOG_WARNING, "", other );
  }
}


bool ResourcesManager::ParseTextString( const QString& _crStr, QStringList& _rlResult ) const
{
  int prev_quotes = 0;
  bool closedSubstring = true;
  int prev_comma = -1;
  bool commaExists = true;

  for( int c1 = 0; c1 < _crStr.size(); c1++ ) {
    if( _crStr.at(c1) == '\"' && ( c1 == 0 || _crStr.at(c1-1) != '\\' ) ) {
      closedSubstring = !closedSubstring;
      if( closedSubstring ) {
        QString sResult = _crStr.mid( prev_quotes+1, c1 - prev_quotes - 1 );
        for( int c2 = 0; c2 < sResult.size(); c2++ )
          if( sResult.at(c2) == '\\' && sResult.at(c2+1) != 'n' )
            sResult.remove( c2, 1 ); // For using '#' symbol in substring
        _rlResult.append( sResult );
      }
      else if( commaExists ){
        prev_quotes = c1;
        commaExists = false;
      }
      else
        return false;
      continue;
    }

    if( closedSubstring ) {
      if( !commaExists ) {
        if( _crStr[c1] == ',' ) {
          prev_comma = c1;
          commaExists = true;
        }
        else if( _crStr[c1] != ' ' && !_rlResult.empty() )
          return false;
      }
      else if( c1 == _crStr.length()-1 ) {
        QString sResult = _crStr.mid( prev_comma+1, c1 - prev_comma );
        if( sResult.trimmed() == "<PLAINTEXT>" ) {
          plainTextMode = true;
          _rlResult.append( "" );
        }
        else if( sResult.trimmed().isEmpty() )
          _rlResult.append( "" );
        else
          return false;
        return true;
      }
    }
  }
  return true;
}
