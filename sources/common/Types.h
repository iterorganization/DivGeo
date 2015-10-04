#ifndef _TYPES_H
#define _TYPES_H

#include "string.h"

typedef struct _PtrTable
{
  int id;
  void* p;
}* PtrTable;


struct _FlagsRec
{
  unsigned int mask;
  char c;

  _FlagsRec( unsigned int _mask, char _c ):
    mask( _mask),
    c( _c ) {} 

};

struct _NameRec
{
  int val;
  char* s;

  _NameRec( int _val, const char* _s ):
    val( _val )
  {
    if( _s != NULL ) {
      s = new char[ strlen( _s )+1 ];
      strcpy( s, _s );
    }
  }

};

#endif
