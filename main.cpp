#include "utils/log.h"
#include "utils/str.h"
#include "utils/stateful.h"
#include "utils/thirdparty/miniz/miniz.h"

#include "colors.h"
#include "streams.h"
#include "textures.h"
#include "framebuffer.h"
#include "mygl.h"
#include "shaders.h"

#include <vector>
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "public/vecdefs.h"
#pragma GCC diagnostic pop

using namespace mygl;

MyGL myGL;
GLboolean chatty = false;

std::shared_ptr<utils::Stateful<Cull> > statefulCull = nullptr;
std::shared_ptr<utils::Stateful<Depth> > statefulDepth = nullptr;
std::shared_ptr<utils::Stateful<Blend> > statefulBlend = nullptr;
std::shared_ptr<utils::Stateful<Stencil> > statefulStencil = nullptr;
std::shared_ptr<utils::Stateful<ColorMask> > statefulColorMask = nullptr;

MyGL* MyGL_initialize(MyGL_LogFunc logger, int initialize_glew, uint32_t stream_count) {
  utils::logfunc(logger);
  if (initialize_glew) {
    auto err = glewInit();
    if ( GLEW_OK != err)
      utils::logout("GLEW Error: %s\n", glewGetErrorString(err));
    else
      utils::logout("GLEW initialized");
  }
  glEnable( GL_TEXTURE_2D);
  glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glDisable( GL_LIGHTING);

  statefulCull = std::make_shared<utils::Stateful<Cull> >(CullState::makeUnique());
  statefulDepth = std::make_shared<utils::Stateful<Depth> >(DepthState::makeUnique());
  statefulBlend = std::make_shared<utils::Stateful<Blend> >(BlendState::makeUnique());
  statefulStencil = std::make_shared<utils::Stateful<Stencil> >(StencilState::makeUnique());
  statefulColorMask = std::make_shared<utils::Stateful<ColorMask> >(ColorMaskState::makeUnique());

  statefulCull->force();
  statefulDepth->force();
  statefulBlend->force();
  statefulStencil->force();

  myGL.cull = static_cast<MyGL_Cull>(statefulCull->current());
  myGL.depth = static_cast<MyGL_Depth>(statefulDepth->current());
  myGL.blend = static_cast<MyGL_Blend>(statefulBlend->current());
  myGL.stencil = static_cast<MyGL_Stencil>(statefulStencil->current());
  myGL.colorMask = static_cast<MyGL_ColorMask>(statefulColorMask->current());

  for (int i = 0; i < MYGL_MAX_COLOR_ATTACHMENTS; i++)
    myGL.drawBufferOrder[i] = i;

  stream_count = stream_count < 65536 * 3 ? 65536 * 3 : stream_count;

  utils::logout(" * cull is %s (%s).", myGL.cull.on ? "enabled" : "disabled", myGL.cull.frontIsCCW ? "CCW front" : "CW front");
  utils::logout(" * depth test/write is %s/%s.", myGL.depth.on ? "true" : "false", myGL.depth.depthWrite ? "true" : "false");
  utils::logout(" * blend is %s.", myGL.blend.on ? "enabled" : "disabled");
  utils::logout(" * stencil is %s.", myGL.stencil.on ? "enabled" : "disabled");
  utils::logout(" * color write for RGBA is { %s, %s, %s, %s }.", myGL.colorMask.red ? "on" : "off", myGL.colorMask.green ? "on" : "off", myGL.colorMask.blue ? "on" : "off",
                myGL.colorMask.alpha ? "on" : "off");

  positionStream = std::make_shared<VertexAttributeStream4fv>("Position", stream_count);
  myGL.positions = positionStream->name;
  vertexAttributeStreams[positionStream->name.chars] = positionStream;

  mygl::normalStream = std::make_shared<VertexAttributeStream4fv>("Normal", stream_count);
  myGL.normals = normalStream->name;
  vertexAttributeStreams[normalStream->name.chars] = normalStream;

  mygl::colorStream = std::make_shared<VertexAttributeStream4fv>("Color", stream_count);
  myGL.colors = colorStream->name;
  vertexAttributeStreams[colorStream->name.chars] = colorStream;

  for (auto i = 0; i < MYGL_MAX_SAMPLERS; i++) {
    auto name = "UV" + std::to_string(i);
    uvsStream[i] = std::make_shared<VertexAttributeStream4fv>(name.c_str(), stream_count);
    myGL.uvs[i] = uvsStream[i]->name;
    vertexAttributeStreams[uvsStream[i]->name.chars] = uvsStream[i];
  }

  auto empty = MyGL_str64("");
  for (auto i = 0; i < MYGL_MAX_SAMPLERS; i++)
    myGL.samplers[i] = empty;

// myGL_last = myGL;
  myGL.primitive = MYGL_TRIANGLES;
  myGL.numPrimitives = 0;

  shaders::globalUniformSetters.emplace("mygl.matProj", &myGL.P_matrix);
  shaders::globalUniformSetters.emplace("mygl.matView", &myGL.V_matrix);
  shaders::globalUniformSetters.emplace("mygl.matWorld", &myGL.W_matrix);

  shaders::globalUniformSetters.emplace("mygl.matProjViewWorld", [&]() -> MyGL_Mat4 {
    MyGL_Mat4 m = MyGL_mat4Multiply(myGL.V_matrix, myGL.W_matrix);
    m = std::move(MyGL_mat4Multiply(myGL.P_matrix, std::move(m)));
    return m;
  });

  for (auto& [k, v] : shaders::globalUniformSetters) {
    utils::logout(" * global uniform: '%s'", k.c_str());
  }

  glMatrixMode( GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode( GL_PROJECTION);
  glLoadIdentity();
  glEnable( GL_TEXTURE_2D);

  utils::logout("done!");

  return &myGL;
}

void MyGL_terminate() {
  utils::logout("Farewell GL!");
}

MyGL_VertexAttributeStream MyGL_vertexAttributeStream(const char *name) {
  MyGL_VertexAttributeStream s;
  MyGL_VertexAttrib attrib;

  attrib.components = MYGL_X;
  attrib.normalized = GL_FALSE;
  attrib.type = MYGL_VERTEX_FLOAT;

  s.info.name = MyGL_str64("");
  s.info.maxCount = 0;
  s.info.attrib = attrib;

  auto matches = [&](MyGL_Str64 &stream_name) {
    return 0 == strcmp(name, stream_name.chars);
  };

  std::string nam = name;
  if (matches(positionStream->name)) {
    s.info.name = positionStream->name;
    s.info.attrib = positionStream->type();
    s.info.maxCount = positionStream->values.size();
    s.arr.p = (void*) positionStream->values.data();
    return s;
  }
  if (matches(normalStream->name)) {
    s.info.name = normalStream->name;
    s.info.attrib = normalStream->type();
    s.info.maxCount = normalStream->values.size();
    s.arr.p = (void*) normalStream->values.data();
    return s;
  }
  if (matches(colorStream->name)) {
    s.info.name = colorStream->name;
    s.info.attrib = colorStream->type();
    s.info.maxCount = colorStream->values.size();
    s.arr.p = (void*) colorStream->values.data();
    return s;
  }
  for (int i = 0; i < MYGL_MAX_SAMPLERS; i++) {
    if (matches(uvsStream[i]->name)) {
      s.info.name = uvsStream[i]->name;
      s.info.attrib = uvsStream[i]->type();
      s.info.maxCount = uvsStream[i]->values.size();
      s.arr.p = (void*) uvsStream[i]->values.data();
      return s;
    }
  }
  return s;
}

void MyGL_drawStreaming(const char *streams) {
  auto get = shaders::Materials::get(myGL.material.chars);
  if (!get.has_value())
    return;
  auto &material = get.value().get();

  size_t index = 0;
  strutils::Tokenizer<1024, MYGL_MAX_VERTEX_ATTRIBS> tokenizer;
  tokenizer.tokenize(streams, " ,\t\n");
  auto names = tokenizer.toVec();
  StreamPrimitiveDrawer drawer(myGL.primitive, index, myGL.numPrimitives);
  StreamPrimitiveDrawer::DrawCtx ctx(names);

  for (uint32_t i = 0; i < material.numPasses(); i++) {
    material.apply(i);
    glBegin(myGL.primitive);
    while (drawer.drawPrimitive(ctx)) {
    }
    glEnd();
    drawer.reset();
  }
}

void MyGL_applyCull() {
  statefulCull->current() = myGL.cull;
  statefulCull->apply();
}

void MyGL_applyDepth() {
  statefulDepth->current() = myGL.depth;
  statefulDepth->apply();
}

void MyGL_applyBlend() {
  statefulBlend->current() = myGL.blend;
  statefulBlend->apply();
}

void MyGL_applyStencil() {
  statefulStencil->current() = myGL.stencil;
  statefulStencil->apply();
}

void MyGL_applyColorMask() {
  statefulColorMask->current() = myGL.colorMask;
  statefulColorMask->apply();
}

void MyGL_resetCull() {
  statefulCull->current() = myGL.cull;
  statefulCull->force();
}

void MyGL_resetDepth() {
  statefulDepth->current() = myGL.depth;
  statefulDepth->force();
}

void MyGL_resetBlend() {
  statefulBlend->current() = myGL.blend;
  statefulBlend->force();
}

void MyGL_resetStencil() {
  statefulStencil->current() = myGL.stencil;
  statefulStencil->force();
}

void MyGL_resetColorMask() {
  statefulColorMask->current() = myGL.colorMask;
  statefulColorMask->force();
}

void MyGL_bindSamplers() {
  for (size_t i = 0; i < MYGL_MAX_SAMPLERS; i++) {
    if (myGL.samplers[i].chars[0] != '\0') {
      {
        auto f = named2DTextures.find(myGL.samplers[i].chars);
        if (f != named2DTextures.end()) {
          f->second->apply(i);
//utils::logout( "%s - binding '%s' to %d", __FUNCTION__, f->first.c_str(), (int)i );
        }
      }

      {
        auto f = named3DTextures.find(myGL.samplers[i].chars);
        if (f != named3DTextures.end()) {
          f->second->apply(i);
//utils::logout( "%s - binding '%s' to %d", __FUNCTION__, f->first.c_str(), (int)i );
        }
      }

      {
        auto f = namedTbos.find(myGL.samplers[i].chars);
        if (f != namedTbos.end()) {
          f->second->apply(i);
//utils::logout( "%s - binding '%s' to %d", __FUNCTION__, f->first.c_str(), (int)i );
        }
      }
    }
  }
}

void MyGL_bindFbo() {
  auto reset = [&]() {
    glViewport(myGL.viewPort.x, myGL.viewPort.y, myGL.viewPort.w, myGL.viewPort.h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };
  if (myGL.frameBuffer.chars[0] == '\0') {
    reset();
    return;
  }
  auto f = namedFrameBuffers.find(myGL.frameBuffer.chars);
  if (f == namedFrameBuffers.end()) {
    reset();
    return;
  }
  glViewport(myGL.viewPort.x, myGL.viewPort.y, myGL.viewPort.w, myGL.viewPort.h);
  glBindFramebuffer(GL_FRAMEBUFFER, f->second.get()->buffer);
  GLenum drawBuffers[MYGL_MAX_COLOR_ATTACHMENTS];
  int n = 0;
  for (; n < MYGL_MAX_COLOR_ATTACHMENTS; n++)
    if (myGL.drawBufferOrder[n] >= 0 && myGL.drawBufferOrder[n] < MYGL_MAX_COLOR_ATTACHMENTS)
      drawBuffers[n] = GL_COLOR_ATTACHMENT0 + myGL.drawBufferOrder[n];
    else
      break;
  glDrawBuffers(n, drawBuffers);

}

GLboolean MyGL_loadShaderLibrary(MyGl_GetCharFunc source_feed, void *source_param, const char *alias) {
  if (!alias) {
    utils::logout("%s - error: shader library has no alias", __func__);
    return GL_FALSE;
  }
  shaders::LineFeed feed(source_feed, source_param);
  shaders::SourceCode::sharedSource.try_emplace(alias, std::string(alias), feed);
  return GL_TRUE;
}

GLboolean MyGL_loadShaderLibraryStr(const char *source_str, const char *alias) {
  if (!alias) {
    utils::logout("%s error: shader library has no alias", __func__);
    return GL_FALSE;
  }
  shaders::LineFeed feed(source_str);
  shaders::SourceCode::sharedSource.try_emplace(alias, std::string(alias), feed);
  return GL_TRUE;
}

GLboolean MyGL_loadShader(MyGl_GetCharFunc source_feed, void *source_param, const char *alias) {
  if (!alias) {
    utils::logout("%s - error: shader has no alias", __func__);
    return GL_FALSE;
  }
  shaders::LineFeed feed(source_feed, source_param);
  shaders::SourceCode::Lines srcLines(alias, feed);
  shaders::SourceCode code(srcLines);
  shaders::Materials::add(code);

  return GL_TRUE;
}

GLboolean MyGL_loadShaderStr(const char *source_str, const char *alias) {
  if (!alias) {
    utils::logout("%s - error: shader has no alias", __func__);
    return GL_FALSE;
  }
  shaders::LineFeed feed(source_str);
  shaders::SourceCode::Lines srcLines(alias, feed);
  shaders::SourceCode code(srcLines);
  shaders::Materials::add(code);

  return GL_TRUE;
}

GLboolean MyGL_createTexture2D(const char *name, MyGL_ROImage image, const char *format, GLboolean filtered, GLboolean mipmapped, GLboolean repeat) {
  if (!name) {
    utils::logout("%s error: texture has no alias", __func__);
    return GL_FALSE;
  }

  if (!image.w || !image.h || !image.pixels) {
    utils::logout("%s error: texture image '%s' is invalid", __func__, name);
    return GL_FALSE;
  }

  auto tex = std::make_shared<Texture2D>(name, image, format, filtered, mipmapped, repeat);
  named2DTextures[name] = tex;
  if (MyGL_Debug_getChatty()) {
    utils::logout("%s 2D texture '%s' created:", __func__, name);
    tex->logInfo();
  }
  return GL_TRUE;
}

GLboolean MyGL_createEmptyTexture2D(const char *name, uint32_t w, uint32_t h, const char *format, GLboolean filtered, GLboolean repeat) {
  if (!name) {
    utils::logout("%s error: texture has no alias", __func__);
    return GL_FALSE;
  }

  if (!w || !h) {
    utils::logout("%s error: texture image '%s' is invalid", __func__, name);
    return GL_FALSE;
  }

  MyGL_ROImage image = { w, h, nullptr };  //for mip-mapped textures, pixels cannot be null
  auto tex = std::make_shared<Texture2D>(name, image, format, filtered, false, repeat);
  named2DTextures[name] = tex;
  if (MyGL_Debug_getChatty()) {
    utils::logout("%s 2D texture (empty) '%s' created:", __func__, name);
    tex->logInfo();
  }
  return GL_TRUE;
}

GLboolean MyGL_createTexture2DArray(const char *name, MyGL_ROImage image_atlas, uint32_t num_rows, uint32_t num_cols, const char *format, GLboolean filtered, GLboolean mipmapped, GLboolean repeat) {
  if (!name) {
    utils::logout("%s error: texture has no alias", __func__);
    return GL_FALSE;
  }

  if (!image_atlas.w || !image_atlas.h || !image_atlas.pixels) {
    utils::logout("%s error: texture '%s' image is invalid", __func__, name);
    return GL_FALSE;
  }

  if (!num_rows || !num_cols) {
    utils::logout("%s error: invalid texture '%s' array dimensions", __func__, name);
    return GL_FALSE;
  }

  if (!(image_atlas.w / num_cols) || (0 != (image_atlas.w % num_cols)) || !(image_atlas.h / num_rows) || (0 != (image_atlas.h % num_rows))) {
    utils::logout("%s - error: texture '%s' array/texture image mismatch", __func__, name);
    return GL_FALSE;
  }

  auto tex = std::make_shared<Texture2DArray>(name, image_atlas, num_rows, num_cols, format, filtered, mipmapped, repeat);
  named3DTextures[name] = tex;
  if (MyGL_Debug_getChatty()) {
    utils::logout("%s 2D texture Array '%s' created:", __func__, name);
    tex->logInfo();
  }

  return GL_TRUE;
}

void MyGL_clear(GLboolean color, GLboolean depth, GLboolean stencil) {
  GLuint flags = 0;
  if (color) {
    glClearColor(myGL.clearColor.x, myGL.clearColor.y, myGL.clearColor.z, myGL.clearColor.w);
    flags |= GL_COLOR_BUFFER_BIT;
  }
  if (depth) {
    glClearDepth(myGL.clearDepth);
    flags |= GL_DEPTH_BUFFER_BIT;
  }
  if (stencil) {
    glClearStencil(myGL.clearStencil);
    flags |= GL_STENCIL_BUFFER_BIT;
  }
  if (flags)
    glClear(flags);
}

GLboolean MyGL_createVbo(const char *name, uint32_t count, const MyGL_VertexAttrib *attribs, uint32_t num_attribs) {
  if (!name) {
    utils::logout("%s error: vbo has no name", __func__);
    return GL_FALSE;
  }
  if (!count) {
    utils::logout("%s error: vbo '%s' has no size", __func__, name);
    return GL_FALSE;
  }
  if (!attribs || !num_attribs) {
    utils::logout("%s error: vbo '%s' has no attributes specified", __func__, name);
    return GL_FALSE;
  }
  if (num_attribs > MYGL_MAX_VERTEX_ATTRIBS) {
    utils::logout("%s warning: no. attributes specified for vbo '%s' exceeds max no.", __func__, name);
    num_attribs = MYGL_MAX_VERTEX_ATTRIBS;
  }
  auto f = namedVbos.find(std::string(name));
  if (f != namedVbos.end())
    utils::logout("%s info: replacing vbo '%s'", __func__, name);

  std::vector<MyGL_VertexAttrib> list;
  for (uint32_t i = 0; i < num_attribs; i++) {
    list.push_back(attribs[i]);
  }
  namedVbos[name] = std::make_shared<mygl::Vbo>((size_t) count, list);
  if (MyGL_Debug_getChatty())
    utils::logout("%s vbo '%s' created", __func__, name);
  return GL_TRUE;
}

MyGL_VboStream MyGL_vboStream(const char *name) {
  MyGL_VboStream stream;
  stream.data = nullptr;
  stream.info.maxCount = 0;
  stream.info.numAttribs = 0;
  auto f = namedVbos.find(std::string(name));
  if (f == namedVbos.end())
    return stream;
  auto vbo = f->second;
  stream.data = vbo->dataPtr.p;
  stream.info.numAttribs = (GLuint) vbo->attribs.count;
  for (size_t i = 0; i < vbo->attribs.count; i++) {
    stream.info.attribs[i] = static_cast<MyGL_VertexAttrib>(vbo->attribs.attribs[i]);
  }
  stream.info.name = MyGL_str64(f->first.c_str());
  stream.info.maxCount = vbo->count;
  return stream;
}

void MyGL_vboPush(const char *name) {
  auto f = namedVbos.find(std::string(name));
  if (f != namedVbos.end())
    f->second->push();
}

GLboolean MyGL_createIbo(const char *name, uint32_t count) {
  if (!name) {
    utils::logout("%s error: ibo has no name", __func__);
    return GL_FALSE;
  }
  if (!count) {
    utils::logout("%s error: ibo '%s' has no size", __func__, name);
    return GL_FALSE;
  }
  auto f = namedIbos.find(std::string(name));
  if (f != namedIbos.end())
    utils::logout("%s info: replacing ibo '%s'", __func__, name);

  namedIbos[name] = std::make_shared<mygl::Ibo>(nullptr, (size_t) count);
  if (MyGL_Debug_getChatty())
    utils::logout("%s ibo '%s' created", __func__, name);
  return GL_TRUE;
}

MyGL_IboStream MyGL_iboStream(const char *name) {
  MyGL_IboStream stream;
  stream.data = nullptr;
  stream.info.maxCount = 0;
  auto f = namedIbos.find(std::string(name));
  if (f == namedIbos.end())
    return stream;
  auto ibo = f->second;
  stream.data = ibo->dataPtr.uint32s;
  stream.info.name = MyGL_str64(f->first.c_str());
  stream.info.maxCount = ibo->count;
  return stream;
}

void MyGL_iboPush(const char *name) {
  auto f = namedIbos.find(std::string(name));
  if (f != namedIbos.end())
    f->second->push();
}

GLboolean MyGL_createTbo(const char *name, uint32_t count, MyGL_Components components) {
  if (!name) {
    utils::logout("%s error: tbo has no name", __func__);
    return GL_FALSE;
  }
  if (!count) {
    utils::logout("%s error: tbo '%s' has no size", __func__, name);
    return GL_FALSE;
  }

  auto f = namedTbos.find(std::string(name));
  if (f != namedTbos.end())
    utils::logout("%s info: replacing tbo '%s'", __func__, name);

  namedTbos[name] = std::make_shared<mygl::Tbo>(components, (size_t) count);
  if (MyGL_Debug_getChatty())
    utils::logout("%s tbo '%s' created", __func__, name);

  return GL_TRUE;
}

MyGL_TboStream MyGL_tboStream(const char *name) {
  MyGL_TboStream stream;
  stream.data = nullptr;
  stream.info.maxCount = 0;
  auto f = namedTbos.find(std::string(name));
  if (f == namedTbos.end())
    return stream;
  auto tbo = f->second;
  stream.data = tbo->getFloats();
  stream.info.name = MyGL_str64(f->first.c_str());
  stream.info.maxCount = tbo->getCount();
  stream.info.components = tbo->components;
  return stream;
}

void MyGL_tboPush(const char *name) {
  auto f = namedTbos.find(std::string(name));
  if (f != namedTbos.end())
    f->second->push();
}

MyGL_Uniform MyGL_findUniform(const char *material_name, const char *pass_name, const char *uniform_name) {
  MyGL_Uniform unif = { { { 0 }, MYGL_UNIFORM_FLOAT }, nullptr };

  if (!material_name || !uniform_name)
    return unif;

  auto get = mygl::shaders::Materials::get(material_name);
  if (!get.has_value())
    return unif;
  std::optional<std::string> pass = std::nullopt;
  if (pass_name)
    pass = std::string(pass_name);

  auto find = get.value().get().findUniform(uniform_name, pass);
  if (find.has_value())
    return find.value();
  return unif;

}

void MyGL_drawVbo(const char *name, MyGL_Primitive primitive, GLint start_index, GLsizei index_count) {
  auto f = namedVbos.find(std::string(name));
  if (f == namedVbos.end())
    return;
  auto vbo = f->second;
  auto get = shaders::Materials::get(myGL.material.chars);
  if (!get.has_value())
    return;
  auto &material = get.value().get();

  vbo->bind();
  for (uint32_t i = 0; i < material.numPasses(); i++) {
    material.apply(i);
    glDrawArrays(primitive, start_index, index_count);
  }
}

void MyGL_drawIndexedVbo(const char *vbo_name, const char *ibo_name, MyGL_Primitive primitive, GLuint count) {
  auto fv = namedVbos.find(std::string(vbo_name));
  if (fv == namedVbos.end())
    return;
  auto vbo = fv->second;
  auto fi = namedIbos.find(std::string(ibo_name));
  if (fi == namedIbos.end())
    return;
  auto ibo = fi->second;

  auto get = shaders::Materials::get(myGL.material.chars);
  if (!get.has_value())
    return;
  auto &material = get.value().get();

  vbo->bind();
  ibo->bind();
  for (uint32_t i = 0; i < material.numPasses(); i++) {
    material.apply(i);
    glDrawElements(primitive, count, GL_UNSIGNED_INT, 0);
  }
}

GLboolean MyGL_Debug_getChatty() {
  return chatty;
}

void MyGL_Debug_setChatty(GLboolean chatty_) {
  chatty = chatty_;
}

void MyGL_Trace_Stencil_set(char *output, uint32_t size) {
  if (!size || !output) {
    trace::stencilOut = std::nullopt;
    return;
  }
  trace::stencilOut = trace::Output(output, "STENCIL", size);
}

void MyGL_Trace_Stencil_tag(const char *tag) {
  if (trace::stencilOut.has_value())
    trace::stencilOut.value().tag(tag);
}

void MyGL_readPixels(int x, int y, uint32_t w, uint32_t h, MyGL_ReadFormat format, MyGL_ReadType type, void *pixels) {
  if (pixels)
    glReadPixels(x, y, w, h, format, type, pixels);
}
