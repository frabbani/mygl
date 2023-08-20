#pragma once

#include "public/mygl.h"

#include <string.h>
#include <map>

namespace mygl
{

  struct Bo{
    GLenum format,
           target;
    GLuint size;
    MyGL_ArrPtr dataPtr;
    GLuint bo;

    ~Bo(){
      if( glIsBuffer ){
        glDeleteBuffers( 1, &bo );
        bo = 0;
      }
      size = 0;
      if( dataPtr.bytes ){
        delete[] dataPtr.bytes;
        dataPtr.p = nullptr;
      }
      format = target = 0;
    }

    Bo( GLenum target_, GLenum format_, size_t size_ ) :
      format(format_),
      target(target_),
      size( size_ )
    {
      glGenBuffers( 1, &bo );
      glBindBuffer( target, bo );
      glBufferData( target, size, nullptr, GL_DYNAMIC_DRAW );
      dataPtr.bytes = new GLubyte[ size ];
    }

    void bind(){
      glBindBuffer( target, bo );
    }

    void push(){
      if( !dataPtr.p )
        return;

      bind();
      //void *p = glMapBuffer( bo, GL_WRITE_ONLY );
      //memcpy( p, dataPtr.p, size );
      //glUnmapBuffer( bo );
      glBindBuffer   ( target, bo );
      glBufferSubData( target, 0, size, dataPtr.p );
    }
  };

  struct Ibo : public Bo
  {
    size_t count;
    Ibo( uint32_t *indices, size_t count_ ) : Bo( GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, sizeof(uint32_t) * count ), count(count_) {
      if( indices ){
        memcpy( dataPtr.uint32s, indices, sizeof(uint32_t) * count );
        push();
      }
    }
  };

  struct Vbo : public Bo{

    struct Attrib : public MyGL_VertexAttrib {
    public:
      Attrib(){
        type = MYGL_FLOAT;
        components = MYGL_X;
        normalized = GL_FALSE;
      }
      uint32_t sizeInBytes() const {
        return sizeOfAttrib(type) * (uint32_t)components;
      }

      Attrib( const MyGL_VertexAttrib& rhs){
        *this = rhs;
      }

      Attrib( MyGL_AttribType type_, MyGL_Components components_, GLboolean normalized_ ){
        type = type_;
        components = components_;
        normalized = normalized_;
      }

      Attrib& operator = ( const MyGL_VertexAttrib& rhs ){
        type = rhs.type;
        components = rhs.components;
        normalized = rhs.normalized;
        return *this;
      }
    };

    struct Attribs{
      size_t count;
      Attrib attribs[ MYGL_MAX_VERTEX_ATTRIBS ];

      Attribs( const std::vector<MyGL_VertexAttrib>& list ){
        count = 0;
        for( auto& a : list ){
          attribs[count++] = a;
          if( count == MYGL_MAX_VERTEX_ATTRIBS )
            break;
        }
      }
      Attribs& operator = (const Attribs& rhs ){
        count = rhs.count;
        for( size_t i = 0; i < rhs.count; i++ )
          attribs[i] = rhs.attribs[i];
        return *this;
      }

      size_t stride() const {
        size_t totalBytes = 0;
        for( size_t i = 0; i < count; i++ ){
            totalBytes += attribs[i].sizeInBytes();
        }
        return totalBytes;
      }
    };

    size_t count;
    Attribs attribs;

    Vbo( size_t count_, const std::vector<MyGL_VertexAttrib>& attribs_ ) :
      Bo( GL_ARRAY_BUFFER, 0,
          [&](){
            size_t pitch = 0;
            for( size_t i = 0; i < attribs_.size(); i++ ){
              if( i == MYGL_MAX_VERTEX_ATTRIBS )
                break;
              pitch += Vbo::Attrib(attribs_[i]).sizeInBytes();
            }
            return pitch;
          }() * count_ ),
      count( count_ ),
      attribs( attribs_ ){
    }

    void bind(){
      Bo::bind();
      const void *ptr = 0;
      size_t stride = attribs.stride();
      for( size_t i = 0; i < attribs.count; i++ ){
        glEnableVertexAttribArray( i );
        auto& attrib = attribs.attribs[i];
        if( MYGL_FLOAT == attrib.type || attrib.normalized )
          glVertexAttribPointer( i, (GLint)attrib.components, (GLenum)attrib.type, attrib.normalized, stride, ptr );
        else
          glVertexAttribIPointer( i, (GLint)attrib.components, (GLenum)attrib.type, stride, ptr );
        ptr = (const void *)( (size_t)ptr + attrib.sizeInBytes() );
      }
    }
  };

  struct Tbo : public Bo{
    GLuint tex;
    MyGL_Components components;

    //size_t getSize( size_t count ){ return count * sizeof(float) * (size_t)components; }
    size_t getCount(){ return size / ( (size_t)components * sizeof(float) ); }
    float *getFloats(){ return dataPtr.floats; }

    static GLenum getFormat( MyGL_Components components ){
      switch( components ){
        case MYGL_X: return GL_R32F;
        case MYGL_XY: return GL_RG32F;
        case MYGL_XYZ: return GL_RGB32F;
        case MYGL_XYZW: return GL_RGBA32F;
        default : return GL_R32F;
      }
    }

    //GLenum target_, GLenum format_, size_t size_
    Tbo( MyGL_Components components_, size_t count ) :
      Bo( GL_TEXTURE_BUFFER, Tbo::getFormat(components_), count * components_ * sizeof(float) ) {
      glActiveTexture( MYGL_TEXTURE_USAGE_UNIT );
      glGenTextures( 1, &tex );
      glBindTexture( GL_TEXTURE_BUFFER, tex );
      glTexBuffer  ( GL_TEXTURE_BUFFER, format, bo );
    }
  };

  std::map<std::string, std::shared_ptr<Vbo>> namedVbos;

}

