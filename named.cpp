#include "bufferobjs.h"
#include "textures.h"
#include "colors.h"
#include "model.h"
#include "framebuffer.h"

const MyGL_ColorFormat& mygl::colorFormatByName(const char *name) {
  auto it = mygl::colorFormatByNames.find(name);
  return it != mygl::colorFormatByNames.end() ? it->second : colorFormats[11];
}

std::map<std::string, std::shared_ptr<mygl::Vbo> > mygl::namedVbos;
std::map<std::string, std::shared_ptr<mygl::Ibo> > mygl::namedIbos;
std::map<std::string, std::shared_ptr<mygl::Tbo> > mygl::namedTbos;
std::map<std::string, std::shared_ptr<mygl::Texture<1> > > mygl::named1DTextures;
std::map<std::string, std::shared_ptr<mygl::Texture<2> > > mygl::named2DTextures;
std::map<std::string, std::shared_ptr<mygl::Texture<3> > > mygl::named3DTextures;
std::map<std::string, std::shared_ptr<mygl::Model>> mygl::namedModels;
std::map<std::string, std::shared_ptr<mygl::FrameBuffer>> mygl::namedFrameBuffers;
