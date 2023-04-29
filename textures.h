#pragma once

#include <string>
#include <memory>
#include <algorithm>
#include <map>

#include "public/mygl.h"
#include "colors.h"

/*
  Texture formats and Data formats are different!

*/
namespace mygl
{
  struct TextureData : public std::enable_shared_from_this<TextureData>{
    GLenum  format;
    GLenum  type;
    size_t  size;
    MyGL_Ptr pixels;
  private:
    TextureData( GLenum  format_, GLenum type_, size_t size_) : format(format_), type(type_){
      size = size_ < sizeof( MyGL_Vec4 ) ? sizeof( MyGL_Vec4 ) : size_;
      pixels.bytes = new GLubyte[ size ];
    }
  public:
    ~TextureData(){ size = 0; delete[] pixels.bytes; pixels.p = NULL; }

    std::shared_ptr<TextureData> getptr(){ return shared_from_this(); }

    [[nodiscard]] static std::shared_ptr<TextureData> createShared( GLenum format, GLenum type, size_t size ) {
        // Not using std::make_shared<Best> because the c'tor is private.
        return std::shared_ptr<TextureData>(new TextureData( format, type, size ));
    }
  };

  template<int N> struct Texture{

  static constexpr int usageUnit = GL_TEXTURE0 + 15;
  std::string name;
  size_t sizes[N];
  GLenum target;
  const MyGL_ColorFormat& format;
  GLuint tex;
  bool filtered;
  bool mipmapped;
  bool repeat;
  bool allocated = false;

  Texture( const char *name_, GLenum target_, const char *format_, bool filtered_, bool mipmapped_, bool repeat_ ) :
    target(target_),
    format( [=](){
      auto it = colorFormatByNames.find( format_ );
      return it != colorFormatByNames.end() ? it->second : colorFormats[11];  //rgb8 default
    }() ),
    filtered(filtered_),
    mipmapped(mipmapped_),
    repeat(repeat_){

    glActiveTexture( usageUnit );
    glGenTextures  ( 1, &tex );
    glBindTexture  ( target, tex );

    if( filtered ){
      glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
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
  virtual GLuint numMipLevels() = 0;
  virtual void apply( GLuint unit ) = 0;
  virtual ~Texture(){
    if( glIsTexture(tex))
      glDeleteTextures( 1, &tex );
    allocated = false;
  }
  size_t getSize(){
    size_t s = sizes[0];
    for( int i = 1; i < (int)N; i++ )
      s *= sizes[i];
    return s;
  }

  void pullData( GLuint level, std::shared_ptr<TextureData> data, GLuint unit = usageUnit ){
    if( level > numMipLevels() )
      return;

    size_t required = getSize();
    if( data->size < required ){
      data = TextureData::createShared( data->format,  data->type , required );
    }
    glActiveTexture( unit );
    glBindTexture( target, tex );
    glGetTexImage( target, level,  data->format, data->type, data->pixels.p );

  }
};

struct Texture2D : public Texture<2> {
  int numMips = 0;
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
    glGetTexLevelParameteriv( target, 0, GL_TEXTURE_INTERNAL_FORMAT, (int *)&format );
  }

  GLuint numMipLevels() override { return numMips; }

  void pushData( GLint level, GLint x, GLint y, GLsizei w, GLsizei h, std::shared_ptr<TextureData> data, GLuint unit = usageUnit ){
    if( level > numMips || data->size < (size_t)( w * h ) )
      return;
    glActiveTexture( unit );
    glBindTexture  ( target, tex );
    glTexSubImage2D( target, level,  x, y, w, h, data->format, data->type, data->pixels.p );
  }

  void pushData( GLint level, std::shared_ptr<TextureData> data, GLuint unit = usageUnit ){
    if( level > numMips || data->size < getSize() )
      return;

    glActiveTexture( unit );
    glBindTexture  ( target, tex );
    glTexSubImage2D( target, level,  0, 0, sizes[0], sizes[1], data->format, data->type, data->pixels.p );
  }
};

std::map< std::string, std::shared_ptr<Texture<1>>> named1DTextures;
std::map< std::string, std::shared_ptr<Texture<2>>> named2DTextures;

}
