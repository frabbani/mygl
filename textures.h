#pragma once

#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <algorithm>
#include <map>

#include "utils/log.h"
#include "public/mygl.h"
#include "bufferobjs.h"
#include "colors.h"

/*
  Texture formats and Data formats are different!

*/
namespace mygl {

extern const MyGL_ColorFormat& colorFormatByName( const char* );

struct TextureData: public std::enable_shared_from_this< TextureData >{
  GLenum format;
  GLenum type;
  size_t size;
  MyGL_ArrPtr pixels;
private:
  TextureData( GLenum format_, GLenum type_, size_t size_ ) :
      format( format_ ), type( type_ ) {
    size = size_ < sizeof(MyGL_Vec4) ? sizeof(MyGL_Vec4) : size_;
    pixels.bytes = new GLubyte[size];
  }
public:
  ~TextureData() {
    size = 0;
    delete[] pixels.bytes;
    pixels.p = NULL;
  }

  std::shared_ptr< TextureData > getptr() {
    return shared_from_this();
  }

  [[nodiscard]] static std::shared_ptr< TextureData > createShared( GLenum format, GLenum type, size_t size ) {
// Not using std::make_shared<Best> because the c'tor is private.
    return std::shared_ptr< TextureData >( new TextureData( format, type, size ) );
  }
};

template< int N >
  struct Texture{

    std::string name;
    size_t sizes[N];
    GLenum target;
    const MyGL_ColorFormat &format;
    GLuint tex;
    bool filtered;
    bool mipmapped;
    bool repeat;
    bool allocated = false;

    Texture( const char *name_, GLenum target_, const char *format_, bool filtered_, bool mipmapped_, bool repeat_ ) :
        target( target_ ), format( colorFormatByName( format_ ) ), filtered( filtered_ ), mipmapped( mipmapped_ ), repeat(
            repeat_ ) {

      static float aniso = -1.0f;
      if( aniso < 0.0f )
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso );

      glActiveTexture( MYGL_TEXTURE_USAGE_UNIT );
      glGenTextures( 1, &tex );
      glBindTexture( target, tex );

      if( !mipmapped ){
        glTexParameteri( target, GL_TEXTURE_BASE_LEVEL, 0 );
        glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, 0 );
      }

      if( filtered ){
        glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso );
        if( mipmapped )
          glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        else
          glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      }
      else{
        glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        if( mipmapped )
          glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
        else
          glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      }
      GLenum ws[] = { GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R };
      for( GLuint i = 0; i < N; i++ ){
        glTexParameteri( target, ws[i], repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE );
      }

    }
    virtual size_t numMipLevels() = 0;
    virtual void apply( GLuint unit ) = 0;

    virtual ~Texture() {
      if( glIsTexture( tex ) )
        glDeleteTextures( 1, &tex );
      allocated = false;
    }
    size_t getSize() {
      size_t s = sizes[0];
      for( int i = 1; i < (int) N; i++ )
        s *= sizes[i];
      return s;
    }

    void pullData( GLuint level, std::shared_ptr< TextureData > data, GLuint unit = MYGL_TEXTURE_USAGE_UNIT ) {
      if( level > numMipLevels() )
        return;

      size_t required = getSize();
      if( data->size < required ){
        data = TextureData::createShared( data->format, data->type, required );
      }
      glActiveTexture( unit );
      glBindTexture( target, tex );
      glGetTexImage( target, level, data->format, data->type, data->pixels.p );
    }

    virtual void logInfo() {
      int w, h, d, f, r, g, b, a, l, i;

      glActiveTexture( MYGL_TEXTURE_USAGE_UNIT );
      glBindTexture( target, tex );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_WIDTH, &w );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_HEIGHT, &h );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_INTERNAL_FORMAT, &f );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_RED_SIZE, &r );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_GREEN_SIZE, &g );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_BLUE_SIZE, &b );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_ALPHA_SIZE, &a );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_LUMINANCE_SIZE, &l );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_INTENSITY_SIZE, &i );
      glGetTexLevelParameteriv( target, 0, GL_TEXTURE_DEPTH_SIZE, &d );
      glBindTexture( target, 0 );

      utils::logout( " - width x height: %d x %d", w, h );
      std::stringstream ss;
      ss << " - # of bits (" << std::hex << f << std::dec << "H): ";
      if( r )
        ss << "r=" << r << " ";
      if( g )
        ss << "g=" << g << " ";
      if( b )
        ss << "b=" << b << " ";
      if( a )
        ss << "a=" << a << " ";
      if( l )
        ss << "l=" << l << " ";
      if( i )
        ss << "i=" << i << " ";
      if( d )
        ss << "d=" << d << " ";
      utils::logout( "%s", ss.str().c_str() );
    }
  };

struct Texture2D: public Texture< 2 > {
  size_t numMips = 0;
/*
  Texture2D( const char *name_, GLuint w, GLuint h, const char *format_, bool filtered_, bool mipmapped_, bool repeat_ ) :
    Texture( name_, GL_TEXTURE_2D, format_, filtered_, mipmapped_, repeat_ ){
    sizes[0] = w;
    sizes[1] = h;
    while( w >= 1 && h >= 1 ){
      glTexImage2D( GL_TEXTURE_2D, numMips++, format.sizedFormat, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
      if( !mipmapped )
        break;
      w >>= 1;
      h >>= 1;
    }
    //glGetTexLevelParameteriv( target, 0, GL_TEXTURE_INTERNAL_FORMAT, (int *)&format );
  }
  */

  Texture2D( const char *name_, MyGL_ROImage image, const char *format_, bool filtered_, bool mipmapped_, bool repeat_ ) :
      Texture( name_, GL_TEXTURE_2D, format_, filtered_, mipmapped_, repeat_ ) {
    sizes[0] = image.w;
    sizes[1] = image.h;
    if( !mipmapped ){
      numMips = 1;
      utils::logout( "creating texture" );
      glTexImage2D( GL_TEXTURE_2D, 0, format.sizedFormat, image.w, image.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.pixels );
    }
    else{
      utils::logout( "creating mip-mapped texture" );
      MyGL_MipChain chain = MyGL_mipChainCreate( image );
      for( size_t i = 0; i < chain.count; i++ ){
        glTexImage2D( GL_TEXTURE_2D, numMips++, format.sizedFormat, chain.levels[i].w, chain.levels[i].h, 0, GL_BGRA,
        GL_UNSIGNED_BYTE,
                      chain.levels[i].pixels );
      }
      MyGL_mipChainFree( &chain );
    }
  }

  size_t numMipLevels() override {
    return numMips;
  }
  void apply( GLuint unit ) override {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( target, tex );
  }

  void pushData( GLint level, GLint x, GLint y, GLsizei w, GLsizei h, std::shared_ptr< TextureData > data, GLuint unit =
  MYGL_TEXTURE_USAGE_UNIT ) {
    if( level > (GLint) numMips || data->size < (size_t) ( w * h ) )
      return;
    glActiveTexture( unit );
    glBindTexture( target, tex );
    glTexSubImage2D( target, level, x, y, w, h, data->format, data->type, data->pixels.p );
  }

  void pushData( GLint level, std::shared_ptr< TextureData > data, GLuint unit = MYGL_TEXTURE_USAGE_UNIT ) {
    if( level > (GLint) numMips || data->size < getSize() )
      return;

    glActiveTexture( unit );
    glBindTexture( target, tex );
    glTexSubImage2D( target, level, 0, 0, sizes[0], sizes[1], data->format, data->type, data->pixels.p );
  }

};

struct Texture2DArray: public Texture< 3 > {
  size_t numMips = 0;

  Texture2DArray( const char *name_, MyGL_ROImage atlas, uint32_t rows, uint32_t cols, const char *format_, bool filtered_,
                  bool mipmapped_, bool repeat_ ) :
      Texture( name_, GL_TEXTURE_2D_ARRAY, format_, filtered_, mipmapped_, repeat_ ) {
    sizes[0] = atlas.w / cols;
    sizes[1] = atlas.h / rows;
    sizes[2] = rows * cols;

    MyGL_Image cell = MyGL_imageAlloc( sizes[0], sizes[1] );
    MyGL_MipChain mips;

    auto getCell = [ & ]( uint32_t r, uint32_t c ) {
      uint32_t yoff = r * cell.h;
      uint32_t xoff = c * cell.w;
      for( uint32_t y = 0; y < cell.h; y++ ){
        for( uint32_t x = 0; x < cell.w; x++ ){
          cell.pixels[y * cell.w + x] = atlas.pixels[ ( yoff + y ) * atlas.w + ( xoff + x )];
        }
      }
    };

    if( !mipmapped ){
      numMips = 1;
      utils::logout( "creating texture array" );
      glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, format.sizedFormat, sizes[0], sizes[1], sizes[2], 0, GL_BGRA, GL_UNSIGNED_BYTE,
                    nullptr );
      uint32_t layer = 0;
      for( uint32_t r = 0; r < rows; r++ ){
        for( uint32_t c = 0; c < cols; c++ ){
          getCell( r, c );
          glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer++, cell.w, cell.h, 1, GL_BGRA, GL_UNSIGNED_BYTE, cell.pixels );
        }
      }
    }
    else{
      auto getMipMaps = [ & ]( bool freeMem = true ) {
        if( freeMem )
          MyGL_mipChainFree( &mips );
      mips = MyGL_mipChainCreate( MYGL_ROIMAGE(cell) );
    } ;

      utils::logout( "creating mip-mapped texture array" );
      getCell( 0, 0 );
      getMipMaps( false );
      numMips = mips.count;
      for( size_t i = 0; i < mips.count; i++ )
        glTexImage3D( GL_TEXTURE_2D_ARRAY, i, format.sizedFormat, mips.levels[i].w, mips.levels[i].h, sizes[2], 0, GL_BGRA,
        GL_UNSIGNED_BYTE,
                      nullptr );

      uint32_t layer = 0;
      for( uint32_t r = 0; r < rows; r++ ){
        for( uint32_t c = 0; c < cols; c++ ){
          getCell( r, c );
          getMipMaps();
          for( size_t i = 0; i < mips.count; i++ )
            glTexSubImage3D( GL_TEXTURE_2D_ARRAY, i, 0, 0, layer, mips.levels[i].w, mips.levels[i].h, 1, GL_BGRA,
            GL_UNSIGNED_BYTE,
                             mips.levels[i].pixels );
          layer++;
        }
      }
      MyGL_mipChainFree( &mips );
    }

    MyGL_imageFree( &cell );
  }

  size_t numMipLevels() override {
    return numMips;
  }
  void apply( GLuint unit ) override {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( target, tex );
  }

};

extern std::map< std::string, std::shared_ptr< Texture< 1 > > > named1DTextures;
extern std::map< std::string, std::shared_ptr< Texture< 2 > > > named2DTextures;
extern std::map< std::string, std::shared_ptr< Texture< 3 > > > named3DTextures;
extern std::map< std::string, std::shared_ptr< mygl::Tbo > > namedTbos;

}
