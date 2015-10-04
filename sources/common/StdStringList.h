#ifndef STDSTRINGLIST_H
#define STDSTRINGLIST_H

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>

typedef unsigned long ulong;
typedef unsigned int uint;

namespace std
{
  class StringList:
    public std::list< std::string >
  {
  public:
    StringList() { }
    StringList( const std::string& _crStr ) { push_back( _crStr ); }
    StringList& operator + ( const std::string& _crStr ) { push_back( _crStr ); return *this; }
    StringList& operator + ( const StringList& _crList )
    { insert( end(), _crList.begin(), _crList.end() ); return *this; }
  };
}

static std::string operator + ( const std::string& str1, const std::string& str2 )
{
  std::string result( str1 );
  result.append( str2 );
  return result;
}

static std::string operator + ( const std::string& str1, const char* str2 )
{
  std::string result( str1 );
  result.append( str2 );
  return result;
}

template< typename T >
static std::string operator + ( const std::string& str1, const T& _value )
{
  std::stringstream ss;
  ss << str1 << _value;
  return ss.str();
}

typedef std::list< std::string > StdStringList;
typedef StdStringList* StdStringListPtr;
typedef StdStringList::iterator StdStringIter;
typedef std::vector< std::string > StdStringArray;

static std::string ToStdString( int v )
{
  char str[16];
  int num = sprintf( str, "%d", v );
  return std::string( str, num );
}

static std::string ToStdString( ulong v )
{
  char str[16];
  int num = sprintf( str, "%lu", v );
  return std::string( str, num );
}

static std::string ToStdString( double v )
{
  char str[16];
  int num = sprintf( str, "%f", v );
  return std::string( str, num );
}


static std::string ToStdString( uint v )
{
  char str[16];
  int num = sprintf( str, "%u", v );
  return std::string( str, num );
}

#endif // STDSTRINGLIST_H
