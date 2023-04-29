#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <utility>

#include <GL/glew.h>
#include <GL/gl.h>
#include "public/mygl.h"
#include "public/defs.h"

#include "utils/stateful.h"

#include "utils.h"
#include "colors.h"
#include "vertices.h"
#include "textures.h"
#include "bufferobjects.h"
#include "mygl.h"
#include "shaders.h"


using namespace mygl;

MyGL myGL;

std::shared_ptr< utils::Stateful<Cull>  > statefulCull  = nullptr;
std::shared_ptr< utils::Stateful<Depth> > statefulDepth = nullptr;
std::shared_ptr< utils::Stateful<Blend> > statefulBlend = nullptr;

MyGL *MyGL_initialize( MyGL_LogFunc logger, int initialize_glew, uint32_t stream_count ){
  utils::logFunc = logger;
  if( initialize_glew ){
    glewInit();
    utils::logout( "GLEW initialized" );
  }

  statefulCull  = std::make_shared<utils::Stateful<Cull>> ( CullState::makeUnique()  );
  statefulDepth = std::make_shared<utils::Stateful<Depth>>( DepthState::makeUnique() );
  statefulBlend = std::make_shared<utils::Stateful<Blend>>( BlendState::makeUnique() );

  statefulCull->force();
  statefulDepth->force();
  statefulBlend->force();

  stream_count = stream_count < 65536 * 3 ? 65536 * 3 : stream_count;

  //myGL.cull = statefulCull.current();
  utils::logout( " * cull is  (CCW front).");

  //myGL.depth = statefulDepth.current();;
  utils::logout( " * depth test/write disabled/yes.");

  //myGL.blend = statefulBlend.current();
  utils::logout( " * blend disabled.");

  positionStream = std::make_shared<VertexAttributeStream4fv>( "Position", stream_count );
  myGL.positions = positionStream->name;
  vertexAttributeStreams[ positionStream->name.chars ] = positionStream;

  mygl::normalStream = std::make_shared<VertexAttributeStream4fv>( "Normal", stream_count );
  myGL.normals = normalStream->name;
  vertexAttributeStreams[ normalStream->name.chars ] = normalStream;

  mygl::colorStream = std::make_shared<VertexAttributeStream4fv>( "Color", stream_count );
  myGL.colors = colorStream->name;
  vertexAttributeStreams[ colorStream->name.chars ] = colorStream;

  for( auto i = 0; i < MYGL_MAX_SAMPLERS; i++ ){
    auto name = "UV" + std::to_string(i);
    uvsStream[i] = std::make_shared<VertexAttributeStream4fv>( name.c_str(), stream_count );
    myGL.uvs[i] = uvsStream[i]->name;
    vertexAttributeStreams[ uvsStream[i]->name.chars ] = uvsStream[i];
  }

  auto empty = MyGL_str64("");
  for( auto i = 0; i < MYGL_MAX_SAMPLERS; i++ )
    myGL.samplers[i] = empty;

  // myGL_last = myGL;
  myGL.primitive = MYGL_TRIANGLES;
  myGL.numPrimitives = 0;

  shaders::globalUniformSetters.emplace( "_P", &myGL.P_matrix );
  shaders::globalUniformSetters.emplace( "_V", &myGL.V_matrix );
  shaders::globalUniformSetters.emplace( "_W", &myGL.W_matrix );

  shaders::globalUniformSetters.emplace( "_PVW", [&]() -> MyGL_Mat4 {
    MyGL_Mat4 m = std::move( MyGL_mat4Multiply( myGL.V_matrix, myGL.W_matrix ) );
    m = std::move( MyGL_mat4Multiply( myGL.P_matrix, std::move(m) ) );
    return m;
  } );


  for( auto& [ k, v ] : shaders::globalUniformSetters ){
    utils::logout( "* global uniform: '%s'", k.c_str() );
  }

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  utils::logout( "done!" );

  return &myGL;
}

MyGL_VertexAttributeStream MyGL_vertexAttributeStream( const char *name ){
  MyGL_VertexAttributeStream s;

  s.info.name = MyGL_str64("");
  s.info.size = 0;
  s.info.maxCount = 0;
  s.info.type = MyGL_AttribType::MYGL_FLOAT;

  auto matches = [&]( MyGL_Str64& stream_name ){
    return 0 == strcmp( name, stream_name.chars );
  };

  std::string nam = name;
  if( matches( positionStream->name ) ){
    auto t = positionStream->type();
    s.info.name = positionStream->name;
    s.info.type = t.t;
    s.info.size = t.n;
    s.info.maxCount = positionStream->values.size();
    s.arr.p = (void *)positionStream->values.data();
    return s;
  }
  if( matches( normalStream->name ) ){
    auto t = normalStream->type();
    s.info.name = normalStream->name;
    s.info.type = t.t;
    s.info.size = t.n;
    s.info.maxCount = normalStream->values.size();
    s.arr.p = (void *)normalStream->values.data();
    return s;
  }
  if( matches( colorStream->name )  ){
    auto t = colorStream->type();
    s.info.name = colorStream->name;
    s.info.type = t.t;
    s.info.size = t.n;
    s.info.maxCount = colorStream->values.size();
    s.arr.p = (void *)colorStream->values.data();
    return s;
  }
  for( int i = 0; i < MYGL_MAX_SAMPLERS; i++ ){
    if( matches( uvsStream[i]->name ) ){
      auto t = uvsStream[i]->type();
      s.info.name = uvsStream[i]->name;
      s.info.type = t.t;
      s.info.size = t.n;
      s.info.maxCount = uvsStream[i]->values.size();
      s.arr.p = (void *)uvsStream[i]->values.data();
      return s;
    }
  }
  return s;
}

void MyGL_drawStream( const char *streams ){
  auto get = shaders::Materials::get( myGL.material.chars );
  if( !get.has_value())
    return;
  auto& material = get.value().get();

  size_t index = 0;
  strutils::Tokenizer<1024, MYGL_MAX_VERTEX_ATTRIBS> tokenizer;
  tokenizer.tokenize( streams, " ,\t\n" );
  auto names = tokenizer.toVec();
  StreamPrimitiveDrawer drawer( myGL.primitive, index, myGL.numPrimitives );
  StreamPrimitiveDrawer::DrawCtx ctx(names);


  for( uint32_t i = 0; i < material.numPasses(); i++ ){
    material.apply(i);
    glBegin( myGL.primitive );
    while( drawer.drawPrimitive( ctx ) ){
    }
    glEnd();
  }
}


void MyGL_applyCull(){
  statefulCull->current() = myGL.cull;
  statefulCull->apply();
}

void MyGL_applyDepth(){
  statefulDepth->current() = myGL.depth;
  statefulDepth->apply();
}


void MyGL_applyBlend(){
  statefulBlend->current() = myGL.blend;
  statefulBlend->apply();
}

void MyGL_resetCull(){
  statefulCull->current() = myGL.cull;
  statefulCull->force();
}

void MyGL_resetDepth(){
  statefulDepth->current() = myGL.depth;
  statefulDepth->force();
}

void MyGL_resetBlend(){
  statefulBlend->current() = myGL.blend;
  statefulBlend->force();
}

int32_t MyGL_loadSourceLibrary( MyGl_GetCharFunc source_feed, void *source_param, const char *alias ){
  if( !alias ){
    utils::logout( "error: shader library has no alias" );
    return 0;
  }
  shaders::LineFeed feed( source_feed, source_param );
  shaders::SourceCode::sharedSource.try_emplace( alias, std::string(alias), feed );
  return 1;
}

int32_t MyGL_loadShader( MyGl_GetCharFunc source_feed, void *source_param, const char *alias ){
  if( !alias ){
    utils::logout( "error: shader has no alias" );
    return 0;
  }
  shaders::LineFeed feed( source_feed, source_param );
  shaders::SourceCode::Lines srcLines( alias, feed );
  shaders::SourceCode code( srcLines );
  shaders::Materials::add(code);

  return 1;
}
