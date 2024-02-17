#include <cstdint>
#include <cstring>

#include "log.h"
#include "bitmap.h"

namespace utils {
namespace bmp {

enum Compression {
  RGB = 0,
  RLE8,
  RLE4,
  BITFIELDS,
  JPEG,
  PNG,
};

#pragma pack(push, 1)

//2 bytes
struct FileMagic {
  uint8_t num0, num1;
};

//12 bytes
struct FileHeader {
  uint32_t fileSize;
  uint16_t creators[2];
  uint32_t dataOffset;
};

//40 bytes, all windows versions since 3.0
struct DibHeader {
  uint32_t headerSize;
  int32_t width, height;
  uint16_t numPlanes, bitsPerPixel;
  uint32_t compression;
  uint32_t dataSize;
  int32_t hPixelsPer, vPixelsPer;  //horizontal and vertical pixels-per-meter
  uint32_t numPalColors, numImportantColors;
};

#pragma pack(pop)

MyGL_Image imageFromBitmapData(const std::vector<uint8_t> rawData, std::string_view source) {
  MyGL_Image image = { .w = 0, .h = 0, .pixels = nullptr };
  if (!rawData.size())
    return image;

  size_t streamPos = 0;

  const FileMagic *magic = (const FileMagic*) &rawData[streamPos];
  if (magic->num0 != 'B' && magic->num1 != 'M') {
    utils::logout("error: '%s' is not a valid bitmap image", source.data());
    return image;
  }
  streamPos += sizeof(FileMagic);

  const FileHeader *header = (const FileHeader*) &rawData[streamPos];
  streamPos += sizeof(FileHeader);

  const DibHeader *dib = (const DibHeader*) &rawData[streamPos];
  if (dib->compression != Compression::RGB || dib->bitsPerPixel != 24) {
    utils::logout("error: '%s' is not a 24 bit bitmap image", source.data());
    return image;
  }

  image.w = dib->width;
  image.h = dib->height;
  image.pixels = new MyGL_Color[image.w * image.h];

  int bypp = dib->bitsPerPixel / 8;
  int rem = 0;
  if ((image.w * bypp) & 0x03) {
    rem = 4 - ((image.w * bypp) & 0x03);
  }

  streamPos = header->dataOffset;
  for (uint32_t y = 0; y < image.h; y++) {
    for (uint32_t x = 0; x < image.w; x++) {
      image.pixels[y * image.w + x].r = rawData[streamPos++];
      image.pixels[y * image.w + x].g = rawData[streamPos++];
      image.pixels[y * image.w + x].b = rawData[streamPos++];
      image.pixels[y * image.w + x].a = 255;
    }
    for (int i = 0; i < rem; i++)
      streamPos++;
  }
  return image;
}

}
}

