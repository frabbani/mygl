#pragma once

#include <stdint.h>

#pragma pack(push,1)

typedef struct MyGL_Color_24bit_s{
  union {
    struct {
      uint8_t r, g, b;
    };
    uint8_t rgb[3];
  };
} MyGL_Color_24bit;

#pragma pack(pop)

typedef struct MyGL_Color_32bit_s{
  union {
    struct {
      uint8_t r, g, b, a;
    };
    uint8_t rgba[4];
    uint32_t value;
  };
} MyGL_Color_32bit;

typedef MyGL_Color_32bit MyGL_Color;

typedef struct MyGL_Image_s{
  uint32_t w, h;
  MyGL_Color *pixels;
} MyGL_Image;

typedef struct MyGL_ROImage_s{
  uint32_t w, h;
  const MyGL_Color *pixels;
} MyGL_ROImage;

typedef struct MyGL_MipChain_s{
  size_t count;
  MyGL_Image levels[24];
} MyGL_MipChain;

#ifdef __cplusplus
extern "C"
{
#endif

#define DLLEXPORT __declspec( dllexport )

#define MYGL_ROIMAGE(i) (MyGL_ROImage){ .w=(i).w, .h=(i).h, .pixels=(i).pixels }

DLLEXPORT void MyGL_imageFree( MyGL_Image *image );
DLLEXPORT MyGL_Image MyGL_imageAlloc( uint32_t w, uint32_t h );
DLLEXPORT MyGL_Image MyGL_imageFromBMPData( const void *data, uint32_t size, const char *source );
DLLEXPORT MyGL_Image MyGL_imageDup( MyGL_ROImage image );
DLLEXPORT MyGL_Color MyGL_imageSampleBox( MyGL_ROImage image, uint32_t x, uint32_t y, uint32_t w, uint32_t h );
DLLEXPORT MyGL_Image MyGL_imageMip( MyGL_ROImage image );

DLLEXPORT void MyGL_mipChainFree( MyGL_MipChain *chain );
DLLEXPORT MyGL_MipChain MyGL_mipChainCreate( MyGL_ROImage image );

#ifdef __cplusplus
}/* extern "C" */
#endif
