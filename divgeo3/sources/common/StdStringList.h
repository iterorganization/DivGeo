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

template< typename T >
std::string operator << ( const std::string& str1, const T& _value )
{
  std::stringstream ss;
  ss << str1 << _value;
  return ss.str();
}

/*template<>
std::string operator + ( const std::string& str1, const std::string& str2 )
{
  std::string result( str1 );
  result.append( str2 );
  return result;
}

template<>
std::string operator + ( const std::string& str1, const char* const & str2 )
{
  std::string result( str1 );
  result.append( str2 );
  return result;
}*/

typedef std::list< std::string > StdStringList;
typedef StdStringList* StdStringListPtr;
typedef StdStringList::iterator StdStringIter;
typedef std::vector< std::string > StdStringArray;

template< typename T >
std::string ToStdString( const T& _crv ) {
  std::stringstream ss;
  ss << _crv;
  return ss.str();
}

#endif // STDSTRINGLIST_H
