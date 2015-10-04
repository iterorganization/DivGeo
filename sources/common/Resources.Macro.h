/* C++ 2003 and early standarts does not allow use enum name like:
 *        %enum%::%enum_member%
 * Using structs makes possible to do following without occupying too much memory:
 *        %struct%::%enum_member%
 * C++ 2011 standart does not have such problem. */

#define ENUM_STRUCT( name ) struct name { enum {
#define ENUM( name, base ) }; struct name { enum { TITLE = base,
#define END_ENUM };
#define END_ENUM_STRUCT }; };
