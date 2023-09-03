
#define DISP_W  800
#define DISP_H  450
//#define CENTERED
#define NOFRAME

#define NO_SDL_GLEXT
#include "mysdl.h"
#include "bitmap.h"
#include "filestream.h"
#include "obj.h"
#include "camera.h"

#include <GL\glew.h>
#include <mygl/public/mygl.h>
#include <mygl/public/strn.h>
#include <mygl/public/vecdefs.h>


#include <math.h>
const float pi = 3.141592653589793238f;


MYGLSTRNFUNCS(64)

MyGL *mygl;
file_stream_t shader_stream;
WaveFront_obj_t floor_obj;
WaveFront_obj_t crate_obj;
camera_t cam;
float    crate_yaw;
float    crate_z;
float    water_z;

void myprint( const char *str ){
  static int first = 1;
  if( first ){
    printf( "********************************************************\n");
    printf( "********************************************************\n");
    printf( "********************************************************\n");
    first = 0;
  }
  printf( "%s", str );
}


void create_vbo_from_obj( WaveFront_obj_t *obj, const char *alias ){

   MyGL_VertexAttrib attribs[] = {
       { MYGL_FLOAT, MYGL_XYZW, GL_FALSE },
       { MYGL_FLOAT, MYGL_XYZW, GL_FALSE },
       { MYGL_FLOAT, MYGL_XYZW, GL_FALSE },
   };

   const char *name = alias ? alias : obj->name;

   MyGL_createVbo( name, obj->num_faces * 3, attribs, 3 );
   MyGL_VboStream s = MyGL_vboStream( name );
   struct { MyGL_Vec4 p, c, t; } *vs = s.data;

   int j = 0;
   for( uint32_t i = 0; i < obj->num_faces; i++ ){
     WaveFront_vec3_t *v;
     WaveFront_vec2_t *t;
     WaveFront_face_t *f = &obj->faces[i];

     for( int k = 0; k < 3; k++ ){
       v = &obj->verts[ f->vs[k] ];
       t = &obj->uvs[ f->ts[k] ];
       vs[j].p = MyGL_vec4( v->x, v->y, v->z, 1.0f );
       vs[j].c = MyGL_vec4( 1.0f, 1.0f, 1.0f, 1.0f );
       vs[j].t = MyGL_vec4( t->x, t->y, 0.0,  0.0f );
       j++;
     }
   }
   MyGL_vboPush( name );

}


SDL_bool MySDL_init(){
  glEnable ( GL_TEXTURE_2D );
  glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glDisable( GL_LIGHTING );

  mygl = MyGL_initialize( myprint, 1, 0 );
  mygl->cull.on = GL_TRUE;
  mygl->cull.cullMode = MYGL_BACK;
  mygl->cull.frontIsCCW = GL_TRUE;
  MyGL_resetCull();

  mygl->depth.on = GL_FALSE;
  mygl->depth.depthMode = MYGL_LESS;
  MyGL_resetDepth();

  mygl->blend.on = GL_FALSE;
  mygl->blend.blendOp.src = MYGL_SRC_ALPHA;
  mygl->blend.blendOp.dst = MYGL_ONE_MINUS_SRC_ALPHA;
  mygl->blend.blendOp.func = MYGL_FUNC_ADD;
  MyGL_resetBlend();

  mygl->stencil.on = GL_FALSE;
  mygl->stencil.writeMask = -1;
  mygl->stencil.stencilOp.stencilFail = MYGL_KEEP;
  mygl->stencil.stencilOp.stencilPassDepthFail = MYGL_KEEP;
  mygl->stencil.stencilOp.stencilPassDepthPass = MYGL_KEEP;
  mygl->stencil.stencilTest.mask = -1;
  mygl->stencil.stencilTest.mode = MYGL_NEVER;
  mygl->stencil.stencilTest.ref = 1;
  MyGL_resetStencil();

  mygl->clearColor = MyGL_vec4( 0.0f, 0.0f, 0.1f, 1.0f );
  mygl->clearDepth = 1.0f;
  mygl->clearStencil = 0;

  file_stream_t shader;
  file_stream_init( &shader, "includes.glsl", 0 );
  MyGL_loadShaderLibrary( file_stream_get_char, &shader, shader.name );

  file_stream_init( &shader, "vct.shader", 0 );
  MyGL_loadShader( file_stream_get_char, &shader, shader.name );

  file_stream_init( &shader, "vc.shader", 0 );
  MyGL_loadShader( file_stream_get_char, &shader, shader.name );

  file_stream_init( &shader, "vct_mask.shader", 0 );
  MyGL_loadShader( file_stream_get_char, &shader, shader.name );


  MyGL_Image tex_image = BMP_init_image( "models/crate.bmp" );
  MyGL_createTexture2D( "Crate Texture", MyGL_roImage( tex_image ), "rgb10a2", GL_TRUE, GL_TRUE, GL_TRUE );
  MyGL_imageFree( &tex_image );

  tex_image = BMP_init_image( "models/floor.bmp" );
  MyGL_createTexture2D( "Floor Texture", MyGL_roImage( tex_image ), "rgb10a2", GL_TRUE, GL_TRUE, GL_FALSE );
  MyGL_imageFree( &tex_image );

  WaveFront_obj_load( &floor_obj, "models/floor.obj", 2.0f, 0 );
  create_vbo_from_obj( &floor_obj, "floor" );

  WaveFront_obj_load( &crate_obj, "models/crate.obj", 1.0f, 0 );
  create_vbo_from_obj( &crate_obj, "crate" );


  MyGL_VertexAttributeStream vs  = MyGL_vertexAttributeStream( "Position" );
  MyGL_VertexAttributeStream cs  = MyGL_vertexAttributeStream( "Color" );

  vs.arr.vec4s[0] = MyGL_vec4( -2.0f, -2.0f, 0.0f, 1.0f );
  vs.arr.vec4s[1] = MyGL_vec4( +2.0f, -2.0f, 0.0f, 1.0f );
  vs.arr.vec4s[2] = MyGL_vec4( +2.0f, +2.0f, 0.0f, 1.0f );

  vs.arr.vec4s[3] = MyGL_vec4( -2.0f, -2.0f, 0.0f, 1.0f );
  vs.arr.vec4s[4] = MyGL_vec4( +2.0f, +2.0f, 0.0f, 1.0f );
  vs.arr.vec4s[5] = MyGL_vec4( -2.0f, +2.0f, 0.0f, 1.0f );

  MyGL_Vec4 c = MyGL_vec4Scale( MyGL_vec4( 101.0f, 139.0f, 188.0f, 128.0f ), 1.0f/255.0f );
  cs.arr.vec4s[0] = c;
  cs.arr.vec4s[1] = c;
  cs.arr.vec4s[2] = c;

  cs.arr.vec4s[3] = c;
  cs.arr.vec4s[4] = c;
  cs.arr.vec4s[5] = c;

  mygl->numPrimitives = 2;
  mygl->primitive = MYGL_TRIANGLES;

  cam.yaw = 90.0f;
  cam.pitch = 0.0f;
  cam.p = MyGL_vec3( 0.0f, -3.0f, 1.5f );
  cam.FOV = 75.0f;
  cam.D_n = 0.01f;
  cam.D_f = 1000.0f;


  crate_z = +0.5f;
  water_z = -0.2f;

  return SDL_TRUE;
}

void MySDL_step(){
  float yaw      = 0.0f;
  float pitch    = 0.0f;
  float forward  = 0.0f;
  float sideways = 0.0f;
  float upwards  = 0.0f;

  float delta = 2.5 * DT_SECS;
  float rot_delta = 360.0f / 3.0f * DT_SECS;

  yaw += MySDL_keyboard()->keys[ SDLK_LEFT ].down ? rot_delta : 0.0f;
  yaw -= MySDL_keyboard()->keys[ SDLK_RIGHT ].down ? rot_delta : 0.0f;
  pitch -= MySDL_keyboard()->keys[ SDLK_UP ].down ? rot_delta : 0.0f;
  pitch += MySDL_keyboard()->keys[ SDLK_DOWN ].down ? rot_delta : 0.0f;

  forward += MySDL_keyboard()->keys[ SDLK_w ].down ? delta : 0.0f;
  forward -= MySDL_keyboard()->keys[ SDLK_s ].down ? delta : 0.0f;
  sideways -= MySDL_keyboard()->keys[ SDLK_a ].down ? delta : 0.0f;
  sideways += MySDL_keyboard()->keys[ SDLK_d ].down ? delta : 0.0f;

  upwards += MySDL_keyboard()->keys[ SDLK_SPACE ].down ? delta : 0.0f;
  upwards -= MySDL_keyboard()->keys[ SDLK_c ].down ? delta : 0.0f;
  upwards *= 0.5f;

  camera_face( &cam, yaw, pitch );
  camera_move( &cam, forward, sideways, upwards );

  crate_yaw += 360.0f / 8.0f * DT_SECS;
  crate_yaw = fmodf( crate_yaw, 360.0f );
  if( crate_yaw < 0.0f )
    crate_yaw += 360.0f;
}


void reset(){
  MyGL_resetCull();
  MyGL_resetDepth();
  MyGL_resetBlend();
  MyGL_resetStencil();
  MyGL_resetColorMask();
}

void MySDL_draw(){

  reset();
  MyGL_clear( GL_TRUE, GL_TRUE, GL_TRUE );

  float rads = crate_yaw * PI / 180.0f;

  mygl->material = MyGL_str64("Vertex Position, Color, and Texture");

  mygl->V_matrix = MyGL_mat4View( cam.p, cam.r, cam.l, cam.u );
  mygl->P_matrix = MyGL_mat4Perspective( (float)DISP_W / (float)DISP_H, cam.FOV * PI / 180.0f, cam.D_n, cam.D_f );

  mygl->samplers[0] = MyGL_str64( "Floor Texture" );
  MyGL_bindSamplers();
  mygl->W_matrix = MyGL_mat4World( MyGL_vec3Zero(), MyGL_vec3R(), MyGL_vec3L() , MyGL_vec3U() );


  MyGL_drawVbo( "floor", MYGL_TRIANGLES, 0, floor_obj.num_faces * 3 );


  mygl->samplers[0] = MyGL_str64( "Crate Texture" );
  MyGL_bindSamplers();

  mygl->W_matrix = MyGL_mat4World( MyGL_vec3( 0.0f, 0.0f, crate_z ),
                                   MyGL_vec3Rotate( MyGL_vec3R(), MyGL_vec3U(), rads ),
                                   MyGL_vec3Rotate( MyGL_vec3L(), MyGL_vec3U(), rads ),
                                   MyGL_vec3U() );
  MyGL_drawVbo( "crate", MYGL_TRIANGLES, 0, crate_obj.num_faces * 3 );

  mygl->material = MyGL_str64( "Vertex Position, Color" );
  mygl->W_matrix = MyGL_mat4World( MyGL_vec3( 0.0f, 0.0f, water_z ),
                                   MyGL_vec3R(), MyGL_vec3L(), MyGL_vec3U() );

  MyGL_drawStreaming( "Position, Color" );


  mygl->material = MyGL_str64("Vertex Position, Color, and Texture (Masked)");
  mygl->samplers[0] = MyGL_str64( "Crate Texture" );
  MyGL_bindSamplers();
  mygl->W_matrix = MyGL_mat4World( MyGL_vec3( 0.0f, 0.0f, water_z - crate_z ),
                                   MyGL_vec3Rotate( MyGL_vec3R(), MyGL_vec3U(), rads ),
                                   MyGL_vec3Rotate( MyGL_vec3L(), MyGL_vec3U(), rads ),
                                   MyGL_vec3Scale( MyGL_vec3U(), -1.0f ) );
  MyGL_drawVbo( "crate", MYGL_TRIANGLES, 0, crate_obj.num_faces * 3 );
  SDL_GL_SwapBuffers();
}


void MySDL_term(){
  printf( "terminating...\n" );
  WaveFront_obj_term( &floor_obj );
  WaveFront_obj_term( &crate_obj );
  MyGL_terminate();
  printf( "done!\n" );
}


Sint32 main( int argc, char * args[] )
{
  setbuf( stdout, NULL );
  if( MySDL_gl_init( args[0], DISP_W, DISP_H, 1, 0, 0, 0 ) ){
    MySDL_run();
  }
  return 0;
}
