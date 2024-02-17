#include "utils/log.h"
#include "utils/miniz.h"
#include "utils/str.h"
#include "utils/data.h"

#include "image.h"
#include "model.h"

#include <optional>

namespace mygl {

void Model::loadMesh(const char *meshFileData, uint32_t meshFileSize) {

  utils::CharStream s(meshFileData, meshFileSize);

  uint32_t vCount = 0;
  uint32_t tCount = 0;

  for (uint32_t i = 0; i < s.lineCount(); i++) {
    std::string line = s.getLine(i);
    if (0 == strncmp(line.c_str(), "v ", 2))
      vCount++;
    if (0 == strncmp(line.c_str(), "f ", 2))
      tCount++;
  }
  auto vboName = name + "-mesh-vbo";
  auto iboName = name + "-mesh-ibo";

  std::vector<MyGL_VertexAttrib> attribs;
  attribs.push_back(MyGL_VertexAttrib { .type = MYGL_VERTEX_FLOAT, .components = MYGL_XYZ, .normalized = false });
  attribs.push_back(MyGL_VertexAttrib { .type = MYGL_VERTEX_FLOAT, .components = MYGL_XY, .normalized = false });
  meshVbo = std::make_shared<Vbo>(vCount, attribs);
  meshIbo = std::make_shared<Ibo>(nullptr, tCount * 3);
  namedVbos[vboName] = meshVbo;
  namedIbos[iboName] = meshIbo;

  if (MyGL_Debug_getChatty()) {
    utils::logout(" - created vbo '%s'(%u vertices)", vboName.data(), vCount);
    utils::logout(" - created ibo '%s'(%u triangles)", vboName.data(), tCount);
  }

  int vi = 0;
  int ti = 0;
  auto vboStream = MyGL_vboStream(vboName.c_str());
  auto iboStream = MyGL_iboStream(iboName.c_str());
  Vertex *verts = (Vertex*) vboStream.data;
  Triangle *tris = (Triangle*) iboStream.data;
  for (uint32_t i = 0; i < s.lineCount(); i++) {
    std::string line = s.getLine(i);
    if (0 == strncmp(line.c_str(), "v ", 2)) {
      strutils::Tokenizer<256, 4> tokenizer;
      tokenizer.tokenize(line.c_str(), " \n");
      sscanf(tokenizer[1].data(), "%f,%f,%f", &verts[vi].p.x, &verts[vi].p.y, &verts[vi].p.z);
      sscanf(tokenizer[3].data(), "%f,%f", &verts[vi].t.x, &verts[vi].t.y);
      vi++;
    }
    if (0 == strncmp(line.c_str(), "f ", 2)) {
      strutils::Tokenizer<128, 2> tokenizer;
      tokenizer.tokenize(line.c_str(), " ");
      sscanf(tokenizer[1].data(), "%d,%d,%d", &tris[ti].i, &tris[ti].j, &tris[ti].k);
      ti++;
    }
  }
  MyGL_vboPush(vboName.c_str());
  MyGL_iboPush(iboName.c_str());
}

std::map<std::string, std::shared_ptr<Model>> namedModels;

bool Model::loadZipped(void *zipContent, uint32_t size, std::string_view name_) {
  mz_zip_archive zip;

  memset(&zip, 0, sizeof(zip));
  if (!mz_zip_reader_init_mem(&zip, zipContent, size, 0)) {
    utils::logout(" - error: invalid archive '%s'", name.data());
    return false;
  }
  name = std::string(name_);

  uint32_t numFiles = mz_zip_reader_get_num_files(&zip);
  for (uint32_t i = 0; i < numFiles; i++) {
    mz_zip_archive_file_stat fileStat;
    if (mz_zip_reader_file_stat(&zip, i, &fileStat)) {
      std::string fileName(fileStat.m_filename);
      if (fileName.find("mesh.txt") != std::string::npos) {
        size_t actualSize = fileStat.m_uncomp_size;
        void *dataPtr = mz_zip_reader_extract_file_to_heap(&zip, fileStat.m_filename, &actualSize, 0);
        loadMesh((const char*) dataPtr, actualSize);
        free(dataPtr);
      }
      size_t loc = fileName.find(".bmp");
      if (loc != std::string::npos) {
        size_t actualSize = fileStat.m_uncomp_size;
        void *dataPtr = mz_zip_reader_extract_file_to_heap(&zip, fileStat.m_filename, &actualSize, 0);
        fileName = fileName.substr(0, loc);
        for (auto &c : fileName) {
          if (c == '/' || c == '\\')
            c = '_';
        }
        auto image = MyGL_imageFromBMPData(dataPtr, actualSize, fileName.c_str());
        MyGL_createTexture2D(fileName.c_str(), toRo(image), "rgb10a2", GL_TRUE, GL_TRUE, GL_TRUE);
        if (MyGL_Debug_getChatty())
          utils::logout(" - loading skin '%s'", fileName.c_str());
        free(dataPtr);
      }
    }
  }
  mz_zip_reader_end(&zip);
  return meshVbo && meshIbo;  // && textureNames.size();
}

}

GLboolean MyGL_loadModelArchive(const char *name, void *data, uint32_t size) {
  mygl::Model model;
  if (MyGL_Debug_getChatty())
    utils::logout("%s Loading Model '%s':", __func__, name);
  if (!model.loadZipped(data, size, name)) {
    utils::logout("failed", name);
    return GL_FALSE;
  }
  return GL_TRUE;
}

