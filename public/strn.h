#pragma once

/*
To use these macros, include the following header files
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
*/

#define MYGLSTRNDEF( len )    \
\
 typedef struct MyGL_Str##len##_s{  \
  char  chars[len]; \
} MyGL_Str##len;  \
\
MyGL_Str##len MyGL_str##len( const char *str ); \
MyGL_Str##len MyGL_str##len##Cat( MyGL_Str##len s, const char *str );  \
MyGL_Str##len MyGL_str##len##Fmt( const char* format, ... ); \



#define MYGLSTRNFUNCS( len )    \
\
MyGL_Str##len MyGL_str##len( const char *str ){ \
  MyGL_Str##len s;  \
  memset( &s, 0, sizeof(s) ); \
  strncpy( s.chars, str, sizeof(s.chars)-1 ); \
  return s;  \
} \
\
MyGL_Str##len MyGL_str##len##Cat( MyGL_Str##len s, const char *str ){ \
  strncat( s.chars, str, sizeof(s.chars)-1 ); \
  s.chars[ sizeof(s.chars) - 1 ] = '\0'; \
  return s;  \
} \
\
MyGL_Str##len MyGL_str##len##Fmt( const char* format, ... ){  \
  MyGL_Str##len s;  \
  memset( &s, 0, sizeof(s) ); \
  va_list args; \
  va_start( args, format ); \
  vsprintf( s.chars, format, args );  \
  va_end( args ); \
  return s; \
} \

MYGLSTRNDEF(24)
MYGLSTRNDEF(40)
MYGLSTRNDEF(64)
MYGLSTRNDEF(80)

