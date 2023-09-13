#pragma once

#include "image.h"

#define MYGL_FIRST_ASCII_PRINTABLE_CHAR ' '
#define MYGL_LAST_ASCII_PRINTABLE_CHAR  '~'
#define MYGL_NUM_ASCII_PRINTABLE_CHARS  ( (int)MYGL_LAST_ASCII_PRINTABLE_CHAR - (int)MYGL_FIRST_ASCII_PRINTABLE_CHAR + 1 )


typedef struct{
  MyGL_Str64 name;
  MyGL_Image imageAtlas;
  uint32_t numChars;
  struct{
    char c;
    uint32_t x, y, w, h;
  } chars[ MYGL_NUM_ASCII_PRINTABLE_CHARS ];
}MyGL_AsciiCharSet;


#ifdef __cplusplus
extern "C" {
#endif

#define DLLEXPORT __declspec( dllexport )
DLLEXPORT GLboolean MyGL_loadAsciiCharSet( const MyGL_AsciiCharSet *char_set, GLboolean filtered, GLboolean mipmapped  );
DLLEXPORT uint32_t  MyGL_drawAsciiCharSet( const char *name, const char *text, MyGL_Color color, MyGL_Vec3 offset, MyGL_Vec2 scale, float spacing );

#ifdef __cplusplus
}
#endif
