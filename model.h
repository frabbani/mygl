#pragma once

#include "public/mygl.h"
#include "bufferobjs.h"
#include <string>
#include <vector>
#include <memory>

namespace mygl {

struct Model {
  struct Vertex {
    MyGL_Vec3 p;
    MyGL_Vec2 t;
  };
  struct Triangle {
    union {
      struct {
        uint32_t indices[3];
      };
      uint32_t i, j, k;
    };
  };

  std::string name;
  std::shared_ptr<Vbo> meshVbo;
  std::shared_ptr<Ibo> meshIbo;
  std::vector<std::string> textureNames;
  std::vector<std::shared_ptr<Tbo>> frameTbos;

  void loadMesh(const char *meshFileData, uint32_t meshFileSize);
  bool loadZipped(void *zipContent, uint32_t size, std::string_view name);

};

extern std::map<std::string, std::shared_ptr<Model>> namedModels;
}

