#ifndef _STRLIB_H
#define _STRLIB_H

//#include <qglobal.h> //TODO: the only link to qt from utils

/*#ifdef Q_OS_WIN
  #include <direct.h>
  #define dg_getcwd _getcwd
#else //Q_OS_LINUX
  #define dg_getcwd getcwd
#endif*/

#include "../common/Includes.h"

#define DG_FNAME_LEN 256
#define DG_LINE_LEN 512
#define DG_STR_LEN 256
#define SCANFLT "%lf"

// File types

#define FT_NOTFOUND 1
#define FT_UNKNOWN 2
#define FT_DG 3
#define FT_DG_OLD 4
#define FT_DG_EQUIL 5
#define FT_OGR_TEMPLATE 6
#define FT_1_EQUIL 7
#define FT_SNN_GRID 8
#define FT_DG_TEMPLATE 9
#define FT_DG_APP 10
#define FT_DG_CONFIG 11
#define FT_DG_TMP 12
#define FT_SONNET 13
#define FT_2COLUMNS 14
#define FT_HPGL 15

// memlib
int zfprintf(FILE* f, const char *format,...);
void ValidatePtr(const void* p,const char *s);

//

char* GetShortFName( char* fName );
const char* GetShortFName( const char* fName );
char* GetFileExt( char* fName );
char* GetFilePath( const char* _cpStr, char* _pPath );

// Splits full file path into { file directory path, short file name }
int SplitPosition( const std::string& filePath );
std::vector< std::string > SplitFilePath( const std::string& filePath );
std::string SplitFilePath( const std::string& filePath, int index );
std::string CombinateFilePath( const std::string& srcPath, const std::string& srcName );

char* StripPrefix(char* str,char* prefixes);
//std::string ExpandFilename( const std::string& str );
int DetectFileType( const std::string& _crsFilename );

int CheckForHpglFile( FILE* file ); 
char* ReadHpglCommand( FILE* f );

int CompStrings(char* str1,char* str2);

char* Flags2Str(ulong flags, const _FlagsRec* fr);
void Str2Flags(char* s,ulong* flags, const _FlagsRec* fr);
char* Int2Name(int val, const _NameRec* nr);
int Name2Int(char* name, const _NameRec* nr);

char* GetVersionStr(int versionInt);

/* Works exactly like strtok but also returns empty strings between
   two adjacent delimiters */
//char* xstrtok(char* s1,const char *s2);

char* RemoveLF(char* str);

void WriteText( const std::string& _crsText, FILE* _pFile );


#endif
