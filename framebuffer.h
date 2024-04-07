#pragma once

#include "textures.h"
#include "optional"

namespace mygl {

struct FrameBuffer {
  GLuint buffer = 0;
  uint32_t w = 0, h = 0;
  std::vector<std::string> colorNames;
  std::vector<std::shared_ptr<Texture<2>>> colorTextures;
  std::optional<std::string> depthStencilName;
  std::optional<std::shared_ptr<Texture<2>>> depthStencilTexture;
  bool complete = false;
  FrameBuffer(uint32_t w_, uint32_t h_) {
    w = w_;
    h = h_;
    glGenFramebuffers(1, &buffer);
    complete = false;
  }
  ~FrameBuffer() {
    if (glIsFramebuffer(buffer))
      glDeleteFramebuffers(1, &buffer);
    buffer = 0;
    w = h = 0;
    complete = false;
  }

  /*
   std::pair<bool, std::string> addColor(std::shared_ptr<Texture<2>> texture);

   void setDepthStencil(std::shared_ptr<Texture<2>> texture) {
   complete = false;
   depthStencilTexture = texture;
   }
   */

  bool finalize();
};

extern std::map<std::string, std::shared_ptr<mygl::FrameBuffer> > namedFrameBuffers;
extern std::map<std::string, std::shared_ptr<mygl::Texture<2> > > named2DTextures;

}

