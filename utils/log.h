#pragma once

#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <functional>
#include <optional>
#include <variant>

#include "../public/mygl.h"

namespace utils{
void logfunc( MyGL_LogFunc );
void logout( const char *format, ... );
void logout2( const char *str, bool newline = true );
void logoutNoNewLine( const char *format, ... );

}
