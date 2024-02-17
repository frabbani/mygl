#pragma once

#include "mygl.h"

#include "utils/str.h"

#include <string>
#include <string_view>
#include <map>
#include <optional>
#include <functional>

// Properties Definition

/**

 // Cull: Back, Front, FrontAndBack/BackAndFront Off
 Cull Front

 Blend SrcAlpha OneMinusSrcAlpha Add, Off

 // ColorWrite: Off, On, Red, Green, Blue
 ColorWrite Red

 // Depth/Stencil: Off Always, Never, NotEqual, Less, Greater, Equal, LEqual, GEqual, ReadOnly
 Depth LEqual

 // Stencil Test: Same as Depth (except for ReadOnly), Always (Default)
 // Stencil Op:  Keep (Default), Zero, Replace, Incr, IncrWrap, Decr, DecrWrap, Invert
 // stencil Write Mask = NoBits (all 0s), [number], AllBits (Default - all 1s)
 Stencil Always WriteMask.AllBits Test.1 TestMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace

 **/

namespace mygl {

namespace shaders {

static constexpr int lineSize = 256;
static constexpr int maxTokens = 16;
using Buffer = strutils::Buffer< lineSize >;
using LineFeed = strutils::LineFeed<lineSize>;
using Tokenizer = strutils::Tokenizer<lineSize, maxTokens>;

// @formatter:off
static const std::map<std::string_view, MyGL_CullMode> cullModes = {
  { "Back", MYGL_BACK },
  { "Front",  MYGL_FRONT },
  { "FrontAndBack", MYGL_FRONT_AND_BACK },
  { "BackAndFront", MYGL_FRONT_AND_BACK },
};

static const std::map<std::string_view, MyGL_BlendMode> blendModes = {
  { "Zero", MYGL_ZERO },
  { "One",  MYGL_ONE },
  { "SrcColor", MYGL_SRC_COLOR },
  { "OneMinusSrcColor", MYGL_ONE_MINUS_SRC_COLOR },
  { "DstColor", MYGL_DST_COLOR },
  { "OneMinusDstColor", MYGL_ONE_MINUS_DST_COLOR },
  { "SrcAlpha", MYGL_SRC_ALPHA },
  { "OneMinusSrcAlpha", MYGL_ONE_MINUS_SRC_ALPHA },
  { "DstAlpha", MYGL_DST_ALPHA },
  { "OneMinusDstAlphar", MYGL_ONE_MINUS_DST_ALPHA },
};

static const std::map<std::string_view, MyGL_BlendFunc> blendFuncs = {
  { "Add", MYGL_FUNC_ADD },
  { "Sub",  MYGL_FUNC_SUB },
  { "RSub", MYGL_FUNC_RSUB },
  { "Min", MYGL_FUNC_MIN },
  { "Max", MYGL_FUNC_MAX },
};
static const std::map<std::string_view, MyGL_DepthMode> depthModes = {
  { "Always", MYGL_ALWAYS },
  { "Never",  MYGL_NEVER },
  { "Equal",  MYGL_EQUAL },
  { "NotEqual",  MYGL_NOTEQUAL },
  { "Less", MYGL_LESS },
  { "LEqual", MYGL_LEQUAL },
  { "Greater", MYGL_GREATER },
  { "GEqual", MYGL_GEQUAL },
};

static const std::map<std::string_view, MyGL_StencilMode>& stencilModes = depthModes;

static const std::map<std::string_view, MyGL_StencilAction> stencilActions = {
  { "Zero", MYGL_ZEROOUT },
  { "Keep",  MYGL_KEEP },
  { "Replace", MYGL_REPLACE },
  { "Incr", MYGL_INCR },
  { "IncrWrap", MYGL_INCR_WRAP },
  { "Decr", MYGL_DECR },
  { "DecrWrap", MYGL_DECR_WRAP },
  { "Invert", MYGL_INVERT },
};
// @formatter:on

MyGL_CullMode cullModeFromString(std::string_view);
MyGL_BlendMode blendModeFromString(std::string_view);
MyGL_BlendFunc blendFuncFromString(std::string_view);
MyGL_DepthMode depthModeFromString(std::string_view);
MyGL_DepthMode stencilModeFromString(std::string_view);

struct StencilParser {
  strutils::KVParse<MyGL_StencilAction> stencilFail;
  strutils::KVParse<MyGL_StencilAction> stencilPassDepthFail;
  strutils::KVParse<MyGL_StencilAction> stencilPassDepthPass;
  strutils::KVParse<GLuint> writeMask;
  strutils::KVParse<GLuint> valueMask;
  strutils::KVParse<GLint> ref;

  StencilParser();
  bool parse(std::string_view token, Stencil &stencil, std::string &error);

};

extern StencilParser stencilParser;
struct SourceCode {
  struct Lines {
    friend class SourceCode;
    struct Meta {
      std::string source = "";
      uint32_t lineNo = 0;
    };

   protected:
    std::string source;
    std::vector<std::string> lines;
    std::vector<Meta> metas;

    Lines(const std::string &source_, const std::vector<std::string> &lines_, const std::vector<Meta> &metas_)
        :
        source(source_),
        lines(lines_),
        metas(metas_) {
    }

   public:
    Lines(const std::string &source_, LineFeed &feed)
        :
        source(source_) {
      while (true) {
        auto info = feed.getLine();
        if (!info.has_value())
          break;
        std::string l = info.value().ptr->kChars();
        for (auto i = l.size(); i < 64; i++)
          l += ' ';
        std::string pretty = '\'' + source + "\'(" + std::to_string(info.value().no + 1) + ')';
        l += "/// ";
        l += pretty;
        lines.push_back(l);
        metas.push_back( { source, (uint32_t) info.value().no + 1 });
      }
    }

    const std::string& sourceName() const {
      return source;
    }
    uint32_t numLines() const {
      return lines.size();
    }
    int length(uint32_t lineNo) {
      if (lineNo >= lines.size())
        return 0;
      return lines.size();
    }
    const char* kChars(uint32_t lineNo) const {
      if (lineNo >= lines.size())
        return nullptr;
      return lines[lineNo].c_str();
    }

    std::string formatted(std::string_view shader) const {
      std::stringstream ss;
      ss << "#version 420" << std::endl;
      ss << "#define " << shader << std::endl;
      for (auto l : lines) {
        ss << l << std::endl;
      }
      return ss.str();
    }

    std::string debugFormatted(std::string_view shader) const {
      std::stringstream ss;
      int n = 1;
      ss << "/*" + std::to_string(n++) + "*/ " + "#define " << shader << std::endl;
      for (auto l : lines) {
        ss << "/*" + std::to_string(n++) + "*/" + l << std::endl;
      }
      return ss.str();
    }

    const Meta& lineMeta(uint32_t lineNo) const {
      lineNo = lineNo >= lines.size() ? lines.size() - 1 : lineNo;
      return metas[lineNo];
    }

    void insert(uint32_t lineNo, const Lines &other) {
      if (lineNo >= lines.size())
        lineNo = lines.size() - 1;
      lines.insert(lines.begin() + lineNo, other.lines.begin(), other.lines.end());
      metas.insert(metas.begin() + lineNo, other.metas.begin(), other.metas.end());
    }

    void appendTo(std::vector<std::string> &lines_, std::vector<Meta> &metas_) {
      lines_.insert(lines_.end(), lines.begin(), lines.end());
      metas_.insert(metas_.end(), metas.begin(), metas.end());
    }
  };

  static std::map<std::string, SourceCode::Lines> sharedSource;

  Lines source;
  std::string name;
  std::vector<std::string> passes;

  void parse();
  SourceCode(Lines source_)
      :
      source(source_) {
    parse();
  }

  std::optional<Lines> expand(const std::string &pass);
};

extern std::map<std::string, UniformSetter> globalUniformSetters;

struct ShaderPass {
  struct GlobalUniforms {
    std::unordered_map<std::string, GLint> uniformLocs;
    GlobalUniforms(GLint prog = -1);
    size_t count() const {
      return uniformLocs.size();
    }
    void apply() {
      for (auto& [k, v] : uniformLocs) {
        auto f = globalUniformSetters.find(k);
        if (f != globalUniformSetters.end()) {
          f->second.set(v);
        }
      }
    }
  };

  struct Logger {
    enum ShaderType {
      Vertex,
      Fragment
    };
    std::string formatted;
    std::vector<std::shared_ptr<Buffer> > lines;
    Logger(std::string formatted_)
        :
        formatted(std::move(formatted_)) {
      LineFeed feed(formatted.c_str());
      while (true) {
        auto info = feed.getLine();
        if (!info.has_value())
          break;
        lines.push_back(info->ptr);
      }
    }

    void log(GLuint shader, ShaderType type);
  };

  std::string source;
  std::string pass;

  std::optional<utils::Stateful<Cull> > statefulCull = std::nullopt;
  std::optional<utils::Stateful<Depth> > statefulDepth = std::nullopt;
  std::optional<utils::Stateful<Blend> > statefulBlend = std::nullopt;
  std::optional<utils::Stateful<Stencil> > statefulStencil = std::nullopt;
  std::optional<utils::Stateful<ColorMask> > statefulColorMask = std::nullopt;
  std::optional<GlobalUniforms> globalUniforms = std::nullopt;
  std::map<std::string, std::shared_ptr<Uniform> > uniforms;

  GLuint vert = -1, frag = -1, prog = -1;

  std::string programLog(GLuint program) {
    GLint len;
    std::string s = "";
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      char *log = new char[len + 1];
      glGetShaderInfoLog(program, len, &len, (GLchar*) log);
      log[len] = '\0';
      utils::logout2("Program Log:");
      utils::logout2(log, false);
      delete[] log;
    }
    return s;
  }

  ~ShaderPass() {
    if ( glIsShader(vert))
      glDeleteShader(vert);
    if ( glIsShader(frag))
      glDeleteShader(frag);
    if ( glIsProgram(prog))
      glDeleteProgram(prog);
  }

  ShaderPass(const std::string &pass_, const SourceCode::Lines &lines);
  void apply();

  std::optional<MyGL_Uniform> findUniform(const std::string &name) {
    auto f = uniforms.find(name);
    if (f == uniforms.end())
      return std::nullopt;
    return f->second->forEdit();
  }
};

struct Material {
  friend class Materials;
  std::string name;
  std::vector<std::reference_wrapper<ShaderPass> > orderedPasses;
  std::map<std::string, ShaderPass> shaderPasses;
 private:
  Material(SourceCode &sourceCode) {
    name = sourceCode.name;
    for (auto pass : sourceCode.passes) {
      auto subLines = sourceCode.expand(pass);
      shaderPasses.try_emplace(pass, pass, subLines.value());
      auto it = shaderPasses.find(pass);
      if (it != shaderPasses.end())
        orderedPasses.push_back(std::ref(it->second));
    }
  }
 public:
  uint32_t numPasses() {
    return orderedPasses.size();
  }

  void apply(uint32_t passNo) {
    if (passNo >= orderedPasses.size())
      return;
    orderedPasses[passNo].get().apply();
  }

  std::optional<MyGL_Uniform> findUniform(const std::string &uniformName, std::optional<std::string> passName) {
    if (!passName.has_value()) {
      if (!orderedPasses.size())
        return std::nullopt;
      return orderedPasses[0].get().findUniform(passName.value());
    }
    auto f = shaderPasses.find(passName.value());
    if (f == shaderPasses.end())
      return std::nullopt;
    return f->second.findUniform(uniformName);
  }
};

struct Materials {
  static std::map<std::string, Material> materials;
  static void add(SourceCode &sourceCode) {
    Material m(sourceCode);
    materials.insert(std::make_pair(m.name, std::move(m)));
  }

  static std::optional<std::reference_wrapper<Material> > get(std::string_view name) {
    std::string s(name);
    auto f = materials.find(s);
    if (f != materials.end())
      return f->second;
    return std::nullopt;
  }
};

}

std::string_view stencilModeToString(MyGL_StencilMode);
std::string_view stencilActionToString(MyGL_StencilAction);

}
