#pragma once

#include <vector>
#include <string.h>
#include <string>
#include <set>

#include "public/mygl.h"

namespace mygl
{
uint32_t sizeOfAttrib( MyGL_AttribType t ){
  switch( t )
  {
    case(MYGL_CHAR):
    case(MYGL_UCHAR): return 1;
    case(MYGL_SHORT):
    case(MYGL_USHORT): return 2;
    case(MYGL_INT):
    case(MYGL_UINT):
    case(MYGL_FLOAT):
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

  struct Type{ MyGL_AttribType t; int n; };

  constexpr Type type(){
    if( std::is_same<T, MyGL_Vec2>::value ){
      return { MyGL_AttribType::MYGL_FLOAT, 2 };
    }
    else if( std::is_same<T, MyGL_Vec3>::value ){
      return { MyGL_AttribType::MYGL_FLOAT, 3 };
    }
    else if( std::is_same<T, MyGL_Vec4>::value ){
      return { MyGL_AttribType::MYGL_FLOAT, 4 };
    }
    else
      return { MyGL_AttribType::MYGL_FLOAT, 1 };
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
  bool drawPrimitive( const DrawCtx& drawCtx ){

    auto draw = [&]( int index ){
      for( size_t i = drawCtx.streams.size(); i > 0; i-- ){
        float x = drawCtx.streams[i-1]->values[index].x;
        float y = drawCtx.streams[i-1]->values[index].y;
        float z = drawCtx.streams[i-1]->values[index].z;
        float w = drawCtx.streams[i-1]->values[index].w;
        glVertexAttrib4f( i-1, x, y, z, w );
      }
    };


    switch( primitive ){
      case MYGL_QUADS:
        draw( index++ );
        draw( index++ );
        draw( index++ );
        draw( index++ );
        break;
      case MYGL_TRIANGLES:
        draw( index++ );
        draw( index++ );
        draw( index++ );
        break;
      case MYGL_LINES:
        draw( index++ );
        draw( index++ );
        break;
      case MYGL_POINTS:
        draw( index++ );
        break;
    }
    return next();
  }

};

}

