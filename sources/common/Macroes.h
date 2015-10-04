#ifndef _MACRO_H
#define _MACRO_H

#include <math.h>

#define dg_swap(x,y) ((x)+=(y),(y)=(x)-(y),(x)-=(y))
#define dg_minmax(x,y) ((x)>(y) ? dg_swap((x),(y)) : (x))
#define dg_min(x,y) ((x)<(y) ? (x) : (y))
#define dg_max(x,y) ((x)>(y) ? (x) : (y))
#define dg_sqr(x) ((x)*(x))
#define dg_sign(a) ((a)<0 ? -1 : ((a)>0 ? 1 : 0))
#define dg_setmin(x,y) ((x)>(y) ? ((x)=(y)) : (x))
#define dg_setmax(x,y) ((x)<(y) ? ((x)=(y)) : (x))
#define dg_inrange(a,b1,b2) ((a)<=dg_max((b1),(b2)) && (a)>=dg_min((b1),(b2)))
#define dg_inrange_s(a,b1,b2) ((a)<dg_max((b1),(b2)) && (a)>dg_min((b1),(b2)))
#define dg_about(a,b,e) ((a)<=((b)+(e)))&&((a)>=((b)-(e)))


#define HasAnyFlag( flags, flag ) (((flags) & (flag)) != 0)
#define HasEachFlag( flags, flag ) (((flags) & (flag)) == flag)
#define SETFLAG( flags, flag, enable ) if( enable ) flags |= flag; else flags &= ~flag;

// Objective staff

#define ENDITER dg__it_end__
#define FOREACH( type, it, grp ) \
  for( type it = (grp).begin(); (it) != (grp).end(); ++(it) )
#define FOREACH_CONST( type, it, grp ) \
  for( type it = (grp).begin(), ENDITER = (grp).end(); (it) != ENDITER; ++(it) )

#define FOREACH_NODECL_CONST( it, grp ) \
  for( it = (grp).begin(); (it) != ENDITER; ++(it) )
#define FOREACH_NODECL( it, grp ) \
  for( it = (grp).begin(); (it) != (grp).end(); ++(it) )

#define ITERATOR dg__it__
#define ERASE( it, grp ) (it) = --((grp).erase(it))
#define ERASE_CURRENTPTR( grp ) ERASE( ITERATOR, grp )


#define REPEAT( n ) for( ulong __counter__ = 0; __counter__ < n; __counter__++ )

// To mark unused arguments
#define UNUSED(x) (void)(x)

// Common style declarations
#define DG_DEFINE_PTR(name) \
  typedef name*                       name##Ptr; \
  typedef const name*                 name##ConstPtr; \
  typedef const name* const           name##ConstPtrConst; \
  typedef const name* const           name##InPtr;
#define DG_DEFINE_LIST(name) \
  typedef std::list< name##Ptr >      name##List; \
  typedef name##List*                 name##ListPtr; \
  typedef name##List::iterator        name##Iter; \
  typedef name##List::const_iterator  name##IterConst;
#define DG_DEFINE_ARRAY(name) \
  typedef std::vector< name##Ptr >    name##Array; \
  typedef name##Array::iterator       name##Item; \
  typedef name##Array::const_iterator name##ItemConst;
#define DG_DEFINE_PLAINARRAY(name) \
  typedef std::list< name >         name##Array; \
  typedef name##Array::iterator       name##Item; \
  typedef name##Array::const_iterator name##ItemConst; // TODO: with std::vector type code will be ignored in unknown reason

#define DG_DEFINE_QUICK(name) \
  class name; \
  typedef name*                  name##Ptr; \
  typedef std::list< name##Ptr > name##List; \
  typedef const name* const      name##InPtr;

#endif
