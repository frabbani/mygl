#pragma once

#include "../public/image.h"
#include <vector>
#include <string_view>

namespace utils {
namespace bmp {

MyGL_Image imageFromBitmapData( std::vector< uint8_t > rawData, std::string_view source );

}
}
