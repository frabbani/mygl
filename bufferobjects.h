#pragma once

#include "public/mygl.h"

#include <string.h>
#include <optional>

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

    Bo( GLenum target_, GLenum format_, size_t size_, bool clear=true ) :
      format(format_),
      target(target_)
    {
      size = 0 == size_ ? 1 : size_;
      glGenBuffers( 1, &bo );
      glBindBuffer( target, bo );
      glBufferData( target, size, nullptr, GL_DYNAMIC_DRAW );
    }

    void bind(){
      glBindBuffer( target, bo );
    }

    void push(){
      bind();
      void *p = glMapBuffer( bo, GL_WRITE_ONLY );
      memcpy( p, dataPtr.p, size );
      glUnmapBuffer( bo );

      //glBindBuffer   ( bo->target, bo->_o );
      //glBufferSubData( bo->target, bo->size, bo->data );
    }
  };

  struct Ibo : public Bo
  {
    size_t count;
    Ibo( uint32_t *indices, size_t count_ ) : Bo( GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, sizeof(uint32_t) * count_ > 0 ? count_ : 1 ){
      count = count_ > 0 ? count_ : 1;
      if( indices ){
        memcpy( dataPtr.uint32s, indices, sizeof(uint32_t) * count );
        push();
      }
    }
  };

  struct Vbo : public Bo{

    struct Attrib{
      GLenum type;
      uint32_t numComponents;
      uint32_t sizeInBytes;
      bool normalized;

      Attrib( MyGL_AttribType type_, MyGL_Components components, bool normalized_ ) :
        type(GLenum(type_)),
        numComponents( (uint32_t)components ),
        sizeInBytes( sizeOfAttrib(type_) ),
        normalized(normalized_){}

      Attrib( const Attrib& rhs ){
        type = rhs.type;
        numComponents = rhs.numComponents;
        sizeInBytes = rhs.sizeInBytes;
        normalized = rhs.normalized;
      }
    };

    struct Attribs{
      size_t count;
      std::optional<Attrib> attribs[MYGL_MAX_VERTEX_ATTRIBS];

      Attribs( const std::vector<Attrib>& list ){
        count = 0;
        for( const auto& a : list ){
          attribs[count++] = Attrib(a);
          if( count == MYGL_MAX_VERTEX_ATTRIBS )
            break;
        }
      }
      Attribs( const Attribs& rhs ){
        count = 0;
        for( size_t i = 0; i < rhs.count; i++ ){
          if( rhs.attribs[i].has_value() )
            attribs[count++] = Attrib(rhs.attribs[i].value());
        }
      }

      size_t stride() const {
        size_t totalBytes = 0;
        for( size_t i = 0; i < count; i++ ){
          if( attribs[i].has_value())
            totalBytes += attribs[i].value().sizeInBytes;
        }
        return totalBytes;
      }
    };

    size_t count;
    Attribs attribs;

    Vbo( size_t count_, const Attribs& attribs_, MyGL_Ptr data ) :
      Bo( GL_ARRAY_BUFFER, 0, attribs_.stride() * count_ ),
      count( count_ ),
      attribs( attribs_ ){
    }

  };
}


