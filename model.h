#pragma once

#include "bufferobjs.h"

namespace mygl {

struct Model{
  struct Vertex{
    MyGL_Vec3 p;
    MyGL_Vec3 n;
    MyGL_Vec2 t;
  };
  std::string name;
  std::shared_ptr< Vbo > meshVbo;
  std::shared_ptr< Tbo > framesTbo;
  std::shared_ptr< Ibo > trisIbo;
  std::vector< std::string > textures;

  bool loadZipped( void *zipContent, uint32_t size, std::string_view name );
};

}
