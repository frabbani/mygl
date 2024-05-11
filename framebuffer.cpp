#include "framebuffer.h"
#include <utility>

namespace mygl {

bool FrameBuffer::finalize() {
  if (complete)
    return true;

  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLint fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

  glBindFramebuffer( GL_FRAMEBUFFER, buffer);
  for (size_t i = 0; i < colorTextures.size(); i++) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorTextures[i]->tex, 0);  //TODO: implement mip-level binding
  }
  if (depthStencilTexture.has_value())
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.value()->tex, 0);  //TODO: implement mip-level binding

  if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    complete = true;

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glPopAttrib();
  return complete;
}

std::pair<std::shared_ptr<FrameBuffer>, std::shared_ptr<Texture<2>>> getFboAndTex(const std::string &fboName, const std::string &texName) {
  std::shared_ptr<FrameBuffer> fbo = nullptr;
  std::shared_ptr<Texture<2>> tex = nullptr;
  auto it = namedFrameBuffers.find(fboName);
  if (it != namedFrameBuffers.end())
    fbo = it->second;
  auto it2 = named2DTextures.find(texName);
  if (it2 != named2DTextures.end())
    tex = it2->second;
  return {fbo, tex};
}

}  // namespace mygl

GLboolean MyGL_createFbo(const char *name, uint32_t w, uint32_t h) {
  using namespace mygl;
  auto fbo = std::make_shared<FrameBuffer>(w, h);
  namedFrameBuffers[name] = std::move(fbo);
  return GL_TRUE;
}

GLboolean MyGL_finalizeFbo(const char *name) {
  using namespace mygl;
  auto it = namedFrameBuffers.find(name);
  if (it == namedFrameBuffers.end()) {
    utils::logout("%s - error: FBO '%s' not found", __FUNCTION__, name);
    return GL_FALSE;
  }
  if (!it->second->finalize()) {
    utils::logout("%s - error: FBO '%s' is incomplete", __FUNCTION__, name);
    return GL_FALSE;
  }
  if (MyGL_Debug_getChatty())
    utils::logout("%s - FBO '%s' finalized", __FUNCTION__, name);

  return GL_TRUE;
}

GLboolean MyGL_fboAttachColor(const char *name, const char *texture_name) {
  using namespace mygl;

  auto pair = getFboAndTex(name, texture_name);
  if (!pair.first) {
    utils::logout("%s - error: FBO '%s' not found", __FUNCTION__, name);
    return GL_FALSE;
  }
  if (!pair.second) {
    utils::logout("%s - error: FBO '%s' failed to attach color '%s' (not found)", __FUNCTION__, name, texture_name);
    return GL_FALSE;
  }
  auto fbo = std::move(pair.first);
  auto tex = std::move(pair.second);

  if (tex->sizes[0] != fbo->w || tex->sizes[1] != fbo->h) {
    utils::logout("%s - error: FBO '%s' failed to attach color '%s' (size mismatch)", __FUNCTION__, name, texture_name);
    return GL_FALSE;
  }

  if (fbo->colorTextures.size() == MYGL_MAX_COLOR_ATTACHMENTS) {
    utils::logout("%s - warning: FBO '%s' failed to attach color '%s' (limit exceeded)", __FUNCTION__, name, texture_name);
    return GL_FALSE;
  }
  fbo->complete = false;
  fbo->colorNames.emplace_back(texture_name);
  fbo->colorTextures.push_back(tex);
  if (MyGL_Debug_getChatty())
    utils::logout("%s - FBO '%s' attached color texture '%s' to location %zu", __FUNCTION__, name, texture_name, fbo->colorTextures.size() - 1);

  return GL_TRUE;
}

GLboolean MyGL_fboAttachDepthStencil(const char *name, const char *texture_name) {
  using namespace mygl;

  auto pair = getFboAndTex(name, texture_name);
  if (!pair.first) {
    utils::logout("%s - error: FBO '%s' not found", __FUNCTION__, name);
    return GL_FALSE;
  }
  auto fbo = std::move(pair.first);
  if (!texture_name) {
    if (fbo->depthStencilName.has_value()) {
      fbo->complete = false;
      fbo->depthStencilName = std::nullopt;
      fbo->depthStencilTexture = std::nullopt;
      if (MyGL_Debug_getChatty())
        utils::logout("%s - FBO '%s' depth/stencil texture released", __FUNCTION__, name);
      return GL_TRUE;
    }
  }
  if (!pair.second) {
    utils::logout("%s - error: FBO '%s' failed to attach depth/stencil '%s' (not found)", __FUNCTION__, name, texture_name);
    return GL_FALSE;
  }
  auto tex = std::move(pair.second);

  if (tex->sizes[0] != fbo->w || tex->sizes[1] != fbo->h) {
    utils::logout("%s - error: FBO '%s' failed to attach depth/stencil '%s' (size mismatch)", __FUNCTION__, name, texture_name);
    return GL_FALSE;
  }

  fbo->complete = false;
  fbo->depthStencilName = texture_name;
  fbo->depthStencilTexture = tex;
  if (MyGL_Debug_getChatty())
    utils::logout("%s - FBO '%s' attached depth/stencil texture '%s'", __FUNCTION__, name, texture_name);
  return GL_TRUE;
}
