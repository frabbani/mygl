#include "public/mygl.h"
#include <cstring>

MyGL_Color MyGL_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a ){
  MyGL_Color c;
  c.r = r;
  c.g = g;
  c.b = b;
  c.a = a;
  return c;
}

MyGL_ROImage MyGL_roImage( MyGL_Image image ){
  MyGL_ROImage out = { .w=image.w, .h=image.h, .pixels=image.pixels };
  return out;
}

void MyGL_imageFree( MyGL_Image *image ){
  if( image->w && image->h && image->pixels ){
    image->w = image->h = 0;
    delete[] image->pixels;
    image->pixels = nullptr;
  }
}

MyGL_Image MyGL_imageAlloc( uint32_t w, uint32_t h ){
  MyGL_Image image;
  image.w = w > 1 ? w : 1;
  image.h = h > 1 ? h : 1;
  image.pixels = new MyGL_Color[ image.w * image.h ];
  return image;
}

MyGL_Image MyGL_imageDup( MyGL_ROImage from ){
  MyGL_Image copy;
  copy.w = from.w;
  copy.h = from.h;
  copy.pixels = new MyGL_Color[ copy.w * copy.h ];
  for( uint32_t i = 0; i < copy.w * copy.h; i++ )
    copy.pixels[i] = from.pixels[i];
  return copy;
}


MyGL_Color MyGL_imageSampleBox( MyGL_ROImage image, uint32_t x, uint32_t y, uint32_t w, uint32_t h ){
  MyGL_Color c = MyGL_color( 0, 0, 0, 0 );

  float count = 0;
  float r, g, b, a;

  if( x >= image.w || y >= image.h )
    return c;

  uint32_t xmin = x;
  uint32_t ymin = y;
  uint32_t xmax = x + w - 1;
  uint32_t ymax = y + h - 1;
  if( xmax >= image.w ){ xmax = image.w - 1; }
  if( ymax >= image.h ){ ymax = image.h - 1; }

  r = g = b = a = 0.0f;
  for( y = ymin; y <= ymax; y++ )
    for( x = xmin; x <= xmax; x++ ){
      uint32_t index = y * image.w + x;
      r += (float)image.pixels[ index ].r;
      g += (float)image.pixels[ index ].g;
      b += (float)image.pixels[ index ].b;
      a += (float)image.pixels[ index ].a;
      count += 1.0f;
    }

  r /= count;
  g /= count;
  b /= count;
  a /= count;
  r = r > 255.0f ? 255.0f : r;
  g = g > 255.0f ? 255.0f : g;
  b = b > 255.0f ? 255.0f : b;
  a = a > 255.0f ? 255.0f : a;

  c.r = (uint8_t)r;
  c.g = (uint8_t)g;
  c.b = (uint8_t)b;
  c.a = (uint8_t)a;
  return c;
}

MyGL_Image MyGL_imageMip( MyGL_ROImage image ){
  MyGL_Image out = { 0, 0, nullptr };
  if( !image.w || !image.h || !image.pixels )
    return out;

  if( image.w == 1 && image.h == 1 )
    return out;

  /*
  if( 1 == image.w ){
    out = MyGL_imageAlloc( 1, image.h / 2 );
    bool oddh = ( image.h & 0x01 ) > 0;
    for( uint32_t y = 0; y < out.w; y++ ){
      bool endy = y == ( out.h - 1 );
      auto h    = oddh && endy ? 3 : 2;
      out.pixels[ y ] = MyGL_imageSampleBox( image, 0, y * 2, 1, h );
    }
    return out;
  }

  if( 1 == image.h ){
    out = MyGL_imageAlloc( image.w / 2, 1 );

    bool oddw = ( image.w & 0x01 ) > 0;
    for( uint32_t x = 0; x < out.w; x++ ){
      bool endx = x == ( out.w - 1 );
      auto w    = oddw && endx ? 3 : 2;
      out.pixels[ x ] = MyGL_imageSampleBox( image, x * 2, 0, w, 1 );
     }
    return out;
  }
  */

  out = MyGL_imageAlloc( image.w / 2, image.h / 2 );

  bool oddw = ( image.w & 0x01 ) > 0;
  bool oddh = ( image.h & 0x01 ) > 0;
  for( uint32_t y = 0; y < out.h; y++ )
    for( uint32_t x = 0; x < out.w; x++ ){
      bool endx = x == ( out.w - 1 );
      bool endy = y == ( out.h - 1 );
      auto w    = oddw && endx ? 3 : 2;
      auto h    = oddh && endy ? 3 : 2;
      out.pixels[ y * out.w + x ] = MyGL_imageSampleBox( image, x * 2, y * 2, w, h );
    }

  return out;
}

MyGL_MipChain MyGL_mipChainCreate( MyGL_ROImage image ){
  MyGL_MipChain chain;
  chain.count = 0;
  memset( (void *)&chain, 0, sizeof(MyGL_MipChain) );
  if( !image.pixels )
    return chain;

  chain.levels[ chain.count++ ] = MyGL_imageDup( image );
  while( true ){
    chain.levels[ chain.count ] = MyGL_imageMip( MyGL_roImage( chain.levels[ chain.count - 1 ] ) );
    if( !chain.levels[ chain.count ].pixels )
      break;
    chain.count++;
  }
  return chain;
}

void MyGL_mipChainFree( MyGL_MipChain *chain ){
  for( size_t i = 0; i < chain->count; i++ )
    MyGL_imageFree( &chain->levels[i] );
  memset( (void *)&chain, 0, sizeof(MyGL_MipChain) );

}

