#include "public/vecdefs.h"
#include "utils/log.h"

#include <string>
#include <map>

namespace mygl {
struct CharSet {
  struct CharMap {
    float x, y, w, h;
    CharMap()
        :
        x(0.0f),
        y(0.0f),
        w(0.0f),
        h(0.0f) {
    }
  };
  MyGL_Str64 textureName;
  MyGL_Vec2 uvScale;
  MyGL_Vec2 maxSize;
  CharMap charMaps[ MYGL_NUM_ASCII_PRINTABLE_CHARS];

  MyGL_Vec4 getUVRect(const CharMap &charMap) const {
    MyGL_Vec4 v;
    v.x = (charMap.x + 0.5f - 1.0f) * uvScale.x;
    v.y = (charMap.y + 0.5f - 1.0f) * uvScale.y;
    v.z = (charMap.w + 1.0f) * uvScale.x;
    v.w = (charMap.h + 1.0f) * uvScale.y;
    return v;
  }

};

std::map<std::string, CharSet> textureCharSets;
}

int MyGL_loadAsciiCharSet(const MyGL_AsciiCharSet *char_set, int filtered, int mipmapped) {

  MyGL_ROImage image = { .w = char_set->imageAtlas.w, .h = char_set->imageAtlas.h, .pixels = char_set->imageAtlas.pixels };
  if (!MyGL_createTexture2D(char_set->name.chars, image, "r8", filtered, mipmapped, GL_FALSE)) {
    utils::logout("error: failed to load ascii character set '%s'", char_set->name.chars);
    return GL_FALSE;
  }

  mygl::CharSet charSet;
  charSet.textureName = char_set->name;

  charSet.uvScale.x = 1.0f / (float) image.w;
  charSet.uvScale.y = 1.0f / (float) image.h;
  charSet.maxSize.x = 0.0f;
  charSet.maxSize.y = 0.0f;
  for (uint32_t i = 0; i < char_set->numChars; i++) {
    if (char_set->chars[i].c >= MYGL_FIRST_ASCII_PRINTABLE_CHAR && char_set->chars[i].c <= MYGL_LAST_ASCII_PRINTABLE_CHAR) {
      auto &charMap = charSet.charMaps[(int) char_set->chars[i].c - (int) MYGL_FIRST_ASCII_PRINTABLE_CHAR];
      charMap.x = (float) char_set->chars[i].x;
      charMap.y = (float) char_set->chars[i].y;
      charMap.w = (float) char_set->chars[i].w;
      charMap.h = (float) char_set->chars[i].h;
      if (charMap.w > charSet.maxSize.x)
        charSet.maxSize.x = charMap.w;
      if (charMap.h > charSet.maxSize.y)
        charSet.maxSize.y = charMap.h;
    }
  }
  mygl::textureCharSets[charSet.textureName.chars] = charSet;
  return 1;
}

uint32_t MyGL_streamAsciiCharSet(const char *name, const char *text, MyGL_Color color, MyGL_Vec3 offset, MyGL_Vec2 scale, float spacing) {
  auto it = mygl::textureCharSets.find(name);
  if (it == mygl::textureCharSets.end())
    return 0;

  const auto &charSet = it->second;
  MyGL_VertexAttributeStream vs = MyGL_vertexAttributeStream("Position");
  MyGL_VertexAttributeStream cs = MyGL_vertexAttributeStream("Color");
  MyGL_VertexAttributeStream ts = MyGL_vertexAttributeStream("UV0");

  uint32_t numChars = 0;

  MyGL_Vec4 v;
  v.x = (float) color.r / 255.0f;
  v.y = (float) color.g / 255.0f;
  v.z = (float) color.b / 255.0f;
  v.w = (float) color.a / 255.0f;

  float x = offset.x;
  float y = offset.y;
  float z = offset.z;
  uint32_t numVerts = 0;
  for (const char *c = text; *c != '\0'; c++) {
    uint32_t index = (uint32_t) *c - (uint32_t) MYGL_FIRST_ASCII_PRINTABLE_CHAR;
    const mygl::CharSet::CharMap &charMap = charSet.charMaps[index];
    auto uvRect = charSet.getUVRect(charMap);
    float w = scale.x * charMap.w / charMap.h;
    float h = scale.y;

    vs.arr.vec4s[numVerts + 0] = MyGL_vec4(x, y, z, 1.0f);
    vs.arr.vec4s[numVerts + 1] = MyGL_vec4(x + w, y, z, 1.0f);
    vs.arr.vec4s[numVerts + 2] = MyGL_vec4(x + w, y, z + h, 1.0f);
    vs.arr.vec4s[numVerts + 3] = MyGL_vec4(x, y, z + h, 1.0f);

    ts.arr.vec4s[numVerts + 0] = MyGL_vec4(uvRect.x, uvRect.y, 0.0f, 0.0f);
    ts.arr.vec4s[numVerts + 1] = MyGL_vec4(uvRect.x + uvRect.z, uvRect.y, 0.0f, 0.0f);
    ts.arr.vec4s[numVerts + 2] = MyGL_vec4(uvRect.x + uvRect.z, uvRect.y + uvRect.w, 0.0f, 0.0f);
    ts.arr.vec4s[numVerts + 3] = MyGL_vec4(uvRect.x, uvRect.y + uvRect.w, 0.0f, 0.0f);

    cs.arr.vec4s[numVerts + 0] = v;
    cs.arr.vec4s[numVerts + 1] = v;
    cs.arr.vec4s[numVerts + 2] = v;
    cs.arr.vec4s[numVerts + 3] = v;

    x += (w + spacing);
    numVerts += 4;
    numChars++;
    if (numChars > 4096)
      break;
  }

  return numChars;
}

