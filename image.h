#include "public/image.h"

#include <vector>

namespace mygl {

struct Mipmaps{
  size_t count = 0;
  MyGL_Image stack[32];

  void generate( MyGL_ROImage from ) {
    if( !from.pixels ){
      return;
    }
    stack[count++] = MyGL_imageDup( from );
    while( true ){
      stack[count] = MyGL_imageMip( MYGL_ROIMAGE( stack[count - 1] ) );
      if( !stack[count].pixels )
        break;
      count++;
    }
  }

  void free() {
    for( size_t i = 0; i < count; i++ ){
      MyGL_imageFree( &stack[i] );
    }
    count = 0;
  }

};

}

