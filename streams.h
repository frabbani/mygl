#pragma once

#include <vector>
#include <string.h>
#include <string>
#include <set>

#include "public/mygl.h"

namespace mygl
{
uint32_t sizeOfAttrib( MyGL_VertexAttribType t ){
  switch( t )
  {
    case(MYGL_VERTEX_CHAR):
    case(MYGL_VERTEX_UCHAR): return 1;
    case(MYGL_VERTEX_SHORT):
    case(MYGL_VERTEX_USHORT): return 2;
    case(MYGL_VERTEX_INT):
    case(MYGL_VERTEX_UINT):
    case(MYGL_VERTEX_FLOAT):
    default: return 4;
  }
}

template<typename T>
struct VertexAttributeStream{

  static_assert(
      std::is_same<T, float>::value ||
      std::is_same<T, MyGL_Vec2>::value ||
      std::is_same<T, MyGL_Vec3>::value ||
      std::is_same<T, MyGL_Vec4>::value, "invalid type");


  constexpr MyGL_VertexAttrib type(){
    if( std::is_same<T, MyGL_Vec2>::value ){
      return { MyGL_VertexAttribType::MYGL_VERTEX_FLOAT, MYGL_XY, GL_FALSE };
    }
    else if( std::is_same<T, MyGL_Vec3>::value ){
      return { MyGL_VertexAttribType::MYGL_VERTEX_FLOAT, MYGL_XYZ, GL_FALSE };
    }
    else if( std::is_same<T, MyGL_Vec4>::value ){
      return { MyGL_VertexAttribType::MYGL_VERTEX_FLOAT, MYGL_XYZW, GL_FALSE };
    }
    else
      return { MyGL_VertexAttribType::MYGL_VERTEX_FLOAT, MYGL_X, GL_FALSE };
  }

  MyGL_Str64 name;
  std::vector<T> values;
  VertexAttributeStream( const char *name_, size_t attribNo_, int maxSize = 1024 ) {
    name = MyGL_str64( name_ );
    T v = {0};
    for( int i = 0; i < maxSize; i++)
      values.push_back(v);
  }

  T& operator[](int index ){ return values[ index ]; }
};

using VertexAttributeStream4fv = VertexAttributeStream<MyGL_Vec4>;

std::shared_ptr<VertexAttributeStream4fv> positionStream  = nullptr;
std::shared_ptr<VertexAttributeStream4fv> normalStream    = nullptr;
std::shared_ptr<VertexAttributeStream4fv> colorStream     = nullptr;
std::shared_ptr<VertexAttributeStream4fv> uvsStream[MYGL_MAX_SAMPLERS] = {nullptr};

std::map< std::string_view, std::shared_ptr<VertexAttributeStream4fv> > vertexAttributeStreams;

struct StreamPrimitiveDrawer{
  MyGL_Primitive primitive;
  size_t& index;
  size_t  count;
  size_t  total;
  StreamPrimitiveDrawer( MyGL_Primitive primitive_, size_t& index_, size_t total_ ) : primitive(primitive_),
      index(index_), count(0), total(total_){}

  bool next(){
    count++;
    return count < total;
  }

  void reset(){ index = 0; }

  struct DrawCtx{
    std::vector< std::shared_ptr<VertexAttributeStream4fv> > streams;
    DrawCtx( const std::vector<std::string_view>& names ){
      for( auto name : names ){
        auto find = vertexAttributeStreams.find( name );
        if( find != vertexAttributeStreams.end() )
          streams.push_back( std::ref(find->second) );
      }
    }
  };

  //separated avoid lambda overhead
  inline void draw( const DrawCtx& drawCtx, int index ){
    for( size_t i = drawCtx.streams.size(); i > 0; i-- ){
      float x = drawCtx.streams[i-1]->values[index].x;
      float y = drawCtx.streams[i-1]->values[index].y;
      float z = drawCtx.streams[i-1]->values[index].z;
      float w = drawCtx.streams[i-1]->values[index].w;
      glVertexAttrib4f( i-1, x, y, z, w );
    }
  };

  bool drawPrimitive( const DrawCtx& drawCtx ){

    switch( primitive ){
      case MYGL_QUADS:
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        break;
      case MYGL_TRIANGLES:
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        break;
      case MYGL_LINES:
        draw( drawCtx, index++ );
        draw( drawCtx, index++ );
        break;
      case MYGL_POINTS:
        draw( drawCtx, index++ );
        break;
    }
    return next();
  }

};

}

