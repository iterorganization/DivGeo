#include "strlib.h"

#define SCNT 10
#define SLEN 256

// Diagnostics

void ValidatePtr(const void* p,const char* s) {
  UNUSED( s );
  assert(p!=NULL);
}

// Utilities

int zfprintf(FILE* f, const char* format,...) {
  va_list vl;
  int i=0;

  va_start(vl,format);
  if (f!=NULL) i=vfprintf(f,format,vl);
  va_end(vl);

  return i;
}

char* GetShortFName(char* fName) {
  int i;

  for (i=strlen(fName)-1;i>=0;i--)
    if (fName[i]=='/' || fName[i]=='\\' || fName[i]==':') return fName+i+1;
  return fName;
}

const char* GetShortFName(const char* fName) {
  int i;

  for (i=strlen(fName)-1;i>=0;i--)
    if (fName[i]=='/' || fName[i]=='\\' || fName[i]==':') return fName+i+1;
  return fName;
}

char* GetFileExt( char* fName ) {
  int i;

  fName=GetShortFName(fName);

  for (i=strlen(fName)-1;i>=0;i--)
    if (fName[i]=='.') return fName+i;

  return fName+strlen(fName);
}

char* GetFilePath( const char* _cpStr, char* _pPath )
{
  strcpy( _pPath, _cpStr );
  *GetShortFName( _pPath ) = 0;

  return _pPath;
}

int SplitPosition( const std::string& filePath )
{
  int splitPosition;
  for( splitPosition = filePath.length() - 1; splitPosition >= 0; splitPosition-- ) {
    if( filePath[splitPosition] == '/' || 
        filePath[splitPosition] == '\\' || 
        filePath[splitPosition] == ':' ) {
      break;
    }
  }
  return splitPosition + 1;
}

std::vector< std::string > SplitFilePath( const std::string& filePath )
{
  std::vector< std::string > result;
  int splitPosition = SplitPosition( filePath );
  result.push_back( filePath.substr( 0, splitPosition ) ); // file directory path
  result.push_back( filePath.substr( splitPosition ) ); // short file name
  return result;
}

std::string SplitFilePath( const std::string& filePath, int index )
{
  int splitPosition = SplitPosition( filePath );
  return (index == 0) ? filePath.substr( 0, splitPosition ) : // file directory path
                        filePath.substr( splitPosition ); // short file name
}

std::string CombinateFilePath( const std::string& srcPath, const std::string& srcName )
{
  return SplitFilePath( srcPath, 0 ) + SplitFilePath( srcName, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////


char* StripPrefix(char* str,char* prefix) {
  assert(strlen(prefix)==1);

  if (*str==*prefix) str++;

  return str;
}


//std::string ExpandFilename( const std::string& name )
//{
//  std::string result;

//  //static char s[DG_FNAME_LEN+1];
//  //if( name == s ) return name; /* Avoid interferences if already processed */

//  /* Check for an absolute name */
//  bool noExpand = false;
//  if( name[0] && name[1] == ':' )
//    result = name; /* DOS name, d:... */
//  else if( !noExpand && (name[0] == '\\' || name[0] == '/') )
//    result = name;
//  else {
//    char s[DG_FNAME_LEN + 1];
//    if( dg_getcwd( s, DG_FNAME_LEN + 1 ) == NULL )
//      result = name;
//    else if( s[0] == '\\' )
//      result = name;  /* Do not handle DOS names */
//    else if( name.length() + strlen( s ) > sizeof( s ) - 1 )
//      result = name;
//    else {
//      result = std::string( s );
//      if( !result.empty() && result[result.length() - 1] != '/' )
//        result += "/";
//      result += name;
//    }
//  }

//  /* Kill '.' references */
//  while( true ) {
//    size_t s = result.find( "/./" );
//    if( s == std::string::npos )
//      break;
    
//    result.erase( s, 2 );
//  }

//  /* Kill '..' references */
//  while( true ) {
//    size_t s = result.find( "/../" );
//    if( s == std::string::npos ) {
//      s = result.find( "\\..\\" );
//      if( s == std::string::npos )
//        break;
//    }

//    size_t s1 = s - 1;
//    while( result[s] != '/' || result[s] != '\\' || s != std::string::npos )
//      s1--;

//    result.erase( s1, s - s1 ); //TODO: check
//  }

//  return result;
//}

/* Reads one HPGL command, finished by a ';' or EOF */
/* Ignores blanks/line ends */

char* ReadHpglCommand( FILE* file )
{
  const int HPGL_COMMAND_MAX = 256;
  static char buf[HPGL_COMMAND_MAX];
  unsigned i = 0;

  while( true ) {
    int c = fgetc( file );
    if( c == EOF ) {
      if( i == 0 )
        return NULL;
      else
        break;
    }

    if( isspace( c ) )
      continue;

    if( c == (int)';' )
      break;

    buf[i++] = (char)tolower( c );

    if( i >= sizeof( buf ) - 1 )
      break;
  }

  buf[i] = 0;

  return buf;
}

int CheckForHpglFile( FILE* file )
{
  const int CHECK_MAXCOMMANDS = 100;
  for( int i = 0; i < CHECK_MAXCOMMANDS; i++ ) {
    char* s = ReadHpglCommand( file );
    if( s == NULL )
      break;
    double x = 0, y = 0;
    if( sscanf( s, "pd"SCANFLT","SCANFLT, &x, &y ) == 2 )
      return 0;
  }
  return -1;
}

int DetectFileType( const std::string& _crsFilename )
{
  char s[256];
  int i,ft;
  char c;
  double float1,float2,float3;

  // Correct slashes
  std::string sFilename( _crsFilename );
  std::replace( sFilename.begin(), sFilename.end(), '\\', '/' );

  FILE* f = fopen( sFilename.c_str(), "rb" );
  if (f==NULL) return FT_NOTFOUND;

  for (ft=i=0;!ft && i<10 && fgets(s,sizeof(s)-1,f)!=NULL;i++) {
    if( sscanf( s, SCANFLT""SCANFLT""SCANFLT, &float1, &float2, &float3 ) == 2 )
      ft=FT_2COLUMNS;
    if (sscanf(s,"   Element output%c",&c)==1 && i==1) ft=FT_SONNET;
    if (sscanf(s,"DivGeo temporary file>%c",&c)==1) ft=FT_DG_TMP;
    if (sscanf(s,"DivGeo data file>%c",&c)==1) ft=FT_DG_APP;
    if (sscanf(s,"DivGeo config file>%c",&c)==1) ft=FT_DG_CONFIG;
    if (sscanf(s,"DivGeo equilibrium file>%c",&c)==1) ft=FT_DG_EQUIL;
    if (sscanf(s,"DivGeo template file>%c",&c)==1) ft=FT_DG_TEMPLATE;
    if (i==1 && sscanf(s,
        "     km   :=  no. of grid points in vertical direction%c",&c)==1)
      ft=FT_1_EQUIL;
    if (sscanf(s,"P3/PATRAN Neutral File%c",&c)==1) ft=FT_OGR_TEMPLATE;
  }

  if( !ft )
  {
    fseek(f,0,SEEK_SET);
    if( !CheckForHpglFile( f ) )
      ft = FT_HPGL;
  }

  fclose(f);

  return ft ? ft : FT_UNKNOWN;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

int CompStrings(char* str1,char* str2) {
  if ((str1==NULL || !*str1) && (str2==NULL || !*str2)) return 0;
  if (str1!=NULL && str2==NULL) return -1;
  if (str1==NULL && str2!=NULL) return 1;
  return strcmp(str1,str2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////


char* Flags2Str(ulong flags,const _FlagsRec* fr) {
  static char buf[400];
  int i;

  for (i=0;fr[i].c;i++) {
    buf[2*i]=(char)((flags & fr[i].mask) ? '+' : '-');
    buf[2*i+1]=fr[i].c;
  }
  buf[2*i]=0;

  return buf;
}

void Str2Flags(char* s,ulong* flags, const _FlagsRec* fr) {
  for( int i = 0; s[i] != 0; i += 2 ) {
    int j;
    for( j = 0; fr[j].c != 0; j++ )
      if( fr[j].c == s[i+1] )
        break;
    assert( fr[j].c != 0 );
    (s[i] == '+') ? (*flags |= fr[j].mask) : (*flags &= ~fr[j].mask);
  }
}

char* Int2Name(int val, const _NameRec* nr) {
  int i;

  for (i=0;nr[i].s!=NULL;i++)
    if (val==nr[i].val) return nr[i].s;
  assert( false );
  return NULL;
}

int Name2Int(char* name, const _NameRec* nr ) {
  int i;
  if( name == null )
    return -1;

  for (i=0;nr[i].s!=NULL;i++)
    if (!strcmp(name,nr[i].s)) return nr[i].val;
  /*assert( false );*/
  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void WriteText(const std::string& _crsText, FILE* _pFile )
{
  unsigned stringsNumber = 0;
  for( unsigned i = 0; i < _crsText.length(); i++ )
    if( _crsText[i] == '\n' )
      stringsNumber++;

  bool finalized = true;
  unsigned text_len = _crsText.length();
  if( text_len != 0 && _crsText[text_len - 1] != '\0' && _crsText[text_len - 1] != '\n' ) {
    stringsNumber++;
    finalized = false;
  }

  fprintf( _pFile, "text101 %d\n", stringsNumber );
  fputs( _crsText.c_str(), _pFile );

  if( !finalized )
    fputc( '\n' , _pFile );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/*char* xstrtok(char* s1, const char* s2) {
  static char* stored_s=NULL;
  int i;

  if (s1!=NULL) stored_s=s1;
  if (stored_s==NULL) return NULL;

  s1=stored_s;

  for (;*stored_s;stored_s++) {
    for (i=0;s2[i];i++) if (s2[i]==*stored_s) {
      *stored_s++=0;
      return s1;
    }
  }

  stored_s=NULL;
  return s1;
}*/

//////////////////////////////////////////////////////////////////////////

char* RemoveLF(char* str)
{
  size_t l = strlen(str);

  if( l == 0 )
    return str;
  if( str[l-2] == '\r' && str[l-1] == '\n' )
    str[l-2] = 0;
  else if( str[l-1] == '\n' )
    str[l-1] = 0;

  return str;
}
