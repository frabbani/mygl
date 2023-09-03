#pragma once

#include "defs.h"
#include <mygl/public/mygl.h>

#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3
#define BI_JPEG       4
#define BI_PNG        5


#pragma pack(push, 1)

typedef struct
{
  uint8_t num0, num1;
}BMP_file_magic_t;      //2 bytes

typedef struct
{
  uint32_t filesize;
  uint16_t creators[2];
  uint32_t dataoffset;
}BMP_file_header_t;     //12 bytes

typedef struct
{
  uint32_t headersize;
  int32_t  width, height;
  uint16_t numplanes, bitsperpixel;
  uint32_t compression;
  uint32_t datasize;
  int32_t  hpixelsper, vpixelsper;  //horizontal and vertical pixels-per-meter
  uint32_t numpalcolors, numimpcolors;

}BMP_dib_header_t;   //40 bytes, all windows versions since 3.0


typedef struct
{
  uint32_t headersize;
  int32_t  width, height;
  uint16_t numplanes, bitsperpixel;
  uint32_t compression;
  uint32_t datasize;
  int32_t  hpixelsper, vpixelsper;
  uint32_t numpalcolors, numimpcolors;
  uint32_t redmask, greenmask, bluemask;

}BMP_dib_header_v2_t;   //52 bytes, 40 + RGB double word masks (added by adobe)

typedef struct
{
  uint32_t headersize;
  int32_t  width, height;
  uint16_t numplanes, bitsperpixel;
  uint32_t compression;
  uint32_t datasize;
  int32_t  hpixelsper, vpixelsper;
  uint32_t numpalcolors, numimpcolors;
  uint32_t redmask, greenmask, bluemask, alphamask;

}BMP_dib_header_v3_t;   //56 bytes, 40 + RGBA double word masks (added by adobe)

#pragma pack(pop)

extern void BMP_write( const MyGL_Color *pixels, uint32_t w, uint32_t h, const char name[] );

extern MyGL_Image BMP_init_image( const char bmpfile[] );
extern void BMP_save_image( MyGL_ROImage image, const char bmpfile[] );
