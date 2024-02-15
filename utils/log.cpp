#include "log.h"

#include <cstdio>
#include <vadefs.h>

//#include <vector>
//#include <string>
//#include <string.h>
//#include <sstream>
//#include <functional>
//#include <optional>
//#include <variant>

MyGL_LogFunc logFunc = NULL;

void utils::logfunc( MyGL_LogFunc func ) {
  logFunc = func;
}

void utils::logout( const char *format, ... ) {
  if( !logFunc )
    return;
  char str[64 * 1024], out[64 * 1024];
  va_list args;
  __builtin_va_start( args, format );
  vsprintf( str, format, args );
  __builtin_va_end( args );
  sprintf( out, "[MYGL] %s\n", str );
  logFunc( out );
}

void utils::logout2( const char *str, bool newline ) {
  if( !logFunc )
    return;
  char out[64 * 1024];
  sprintf( out, "[MYGL] %s%c", str, newline ? '\n' : '\0' );
  logFunc( out );
}

void utils::logoutNoNewLine( const char *format, ... ) {
  if( !logFunc )
    return;
  char str[64 * 1024], out[64 * 1024];
  va_list args;
  __builtin_va_start( args, format );
  vsprintf( str, format, args );
  __builtin_va_end( args );
  sprintf( out, "[MYGL] %s", str );
  logFunc( out );
}
