#pragma once


#include <GL/glew.h>


#include "strn.h"
#include "vec.h"

typedef void (*MyGL_LogFunc)(const char*);

#define MYGL_MAX_SAMPLERS       16
#define MYGL_MAX_VERTEX_ATTRIBS 16

typedef enum MyGL_CullMode_e {
  MYGL_BACK           = GL_BACK,
  MYGL_FRONT          = GL_FRONT,
  MYGL_FRONT_AND_BACK = GL_FRONT_AND_BACK
} MyGL_CullMode;

typedef enum MyGL_DepthMode_e {
  MYGL_ALWAYS   = GL_ALWAYS,
  MYGL_NEVER    = GL_NEVER,
  MYGL_LESS     = GL_LESS,
  MYGL_EQUAL    = GL_EQUAL,
  MYGL_LEQUAL   = GL_LEQUAL,
  MYGL_GREATER  = GL_GREATER,
  MYGL_GEQUAL   = GL_GEQUAL,
  MYGL_NOTEQUAL = GL_NOTEQUAL
} MyGL_DepthMode;

typedef enum MyGL_BlendMode_e {
  MYGL_ZERO                 = GL_ZERO,
  MYGL_ONE                  = GL_ONE,
  MYGL_SRC_COLOR            = GL_SRC_COLOR,
  MYGL_ONE_MINUS_SRC_COLOR  = GL_ONE_MINUS_SRC_COLOR,
  MYGL_DST_COLOR            = GL_DST_COLOR,
  MYGL_ONE_MINUS_DST_COLOR  = GL_ONE_MINUS_DST_COLOR,
  MYGL_SRC_ALPHA            = GL_SRC_ALPHA,
  MYGL_ONE_MINUS_SRC_ALPHA  = GL_ONE_MINUS_SRC_ALPHA,
  MYGL_DST_ALPHA            = GL_DST_ALPHA,
  MYGL_ONE_MINUS_DST_ALPHA  = GL_ONE_MINUS_DST_ALPHA,
  // MYGL_CONSTANT_COLOR       = GL_CONSTANT_COLOR,
  // MYGL_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
  // MYGL_CONSTANT_ALPHA       = GL_CONSTANT_ALPHA,
  // MYGL_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
}MyGL_BlendMode;

typedef enum MyGL_BlendFunc_e {
  MYGL_FUNC_ADD     = GL_FUNC_ADD,
  MYGL_FUNC_SUB     = GL_FUNC_SUBTRACT,
  MYGL_FUNC_RSUB    = GL_FUNC_REVERSE_SUBTRACT,
  MYGL_FUNC_MIN     = GL_MIN,
  MYGL_FUNC_MAX     = GL_MAX,
}MyGL_BlendFunc;

typedef struct MyGL_BlendOp_s{
  MyGL_BlendMode src;
  MyGL_BlendMode dst;
  MyGL_BlendFunc func;
}MyGL_BlendOp;

typedef struct MyGL_Blend_s{
  GLboolean on;
  MyGL_BlendOp blendOp;
}MyGL_Blend;

typedef enum MyGL_Primitive_e {
  MYGL_POINTS       = GL_POINTS,
  MYGL_LINES        = GL_LINES,
  MYGL_QUADS        = GL_QUADS,
  MYGL_TRIANGLES    = GL_TRIANGLES,
  // MYGL_TRIANGLE_FAN = GL_TRIANGLE_FAN,
} MyGL_Primitive;

typedef enum MyGL_AttribType_e {
  MYGL_CHAR = GL_BYTE,
  MYGL_UCHAR = GL_UNSIGNED_BYTE,
  MYGL_SHORT = GL_SHORT,
  MYGL_USHORT = GL_UNSIGNED_SHORT,
  MYGL_FLOAT = GL_FLOAT,
  MYGL_INT  = GL_INT,
  MYGL_UINT = GL_UNSIGNED_INT
} MyGL_AttribType;

typedef enum MyGL_Components_e {
  MYGL_X    = 1,
  MYGL_XY   = 2,
  MYGL_XYZ  = 3,
  MYGL_XYZW = 4,
} MyGL_Components;


typedef union MyGL_Ptr_u{
  void      *p;
  GLubyte   *bytes;
  GLbyte    *int8;
  GLubyte   *uint8;
  GLshort   *int16;
  GLushort  *uint16;
  GLint     *int32;
  GLuint    *uint32;
  GLfloat   *floa;
  MyGL_Vec2 *vec2;
  MyGL_Vec3 *vec3;
  MyGL_Vec4 *vec4;
  MyGL_Mat4 *mat4;
}MyGL_Ptr;

typedef union MyGL_ArrPtr{
  void      *p;
  GLubyte   *bytes;
  GLbyte    *int8s;
  GLubyte   *uint8s;
  GLshort   *int16s;
  GLushort  *uint16s;
  GLint     *int32s;
  GLuint    *uint32s;
  GLfloat   *floats;
  MyGL_Vec2 *vec2s;
  MyGL_Vec3 *vec3s;
  MyGL_Vec4 *vec4s;
  MyGL_Mat4 *mat4s;
}MyGL_ArrPtr;

typedef struct MyGL_VertexAttributeStream_s{
  struct{
    MyGL_Str64 name;
    GLuint maxCount;  // #. of elements
    GLint size;     // 1,2,3 or 4
    MyGL_AttribType type;
    GLboolean normalized;
  }info;
  MyGL_ArrPtr arr;
}MyGL_VertexAttributeStream;


typedef struct MyGL_ColorFormat_s{
  MyGL_Str24 name;
  GLint sizedFormat;
  GLint baseFormat;
  GLubyte rgbaSize[4];
} MyGL_ColorFormat;


typedef struct MyGL_Cull_s{
  GLboolean     on;
  GLboolean     frontIsCCW;
  MyGL_CullMode cullMode;
}MyGL_Cull;

typedef struct MyGL_Depth_s{
  GLboolean on;
  GLboolean depthWrite;
  MyGL_DepthMode depthMode;
}MyGL_Depth;

typedef struct MyGL_s{
  MyGL_Cull  cull;
  MyGL_Depth depth;
  MyGL_Blend blend;

  MyGL_Vec4 clearColor;
  float     clearDepth;

  MyGL_Primitive primitive;
  GLuint numPrimitives;

  MyGL_Str64 positions;
  MyGL_Str64 normals;
  MyGL_Str64 colors;
  MyGL_Str64 tangents;
  MyGL_Str64 uvs[ MYGL_MAX_SAMPLERS ];

  MyGL_Str64 samplers[ MYGL_MAX_SAMPLERS ];

  MyGL_Mat4 W_matrix;
  MyGL_Mat4 V_matrix;
  MyGL_Mat4 P_matrix;

  MyGL_Str64 material;

}MyGL;


#ifdef __cplusplus
extern "C" {
#endif

#define DLLEXPORT __declspec( dllexport )

DLLEXPORT MyGL *MyGL_initialize( MyGL_LogFunc logger, int32_t initialize_glew, uint32_t stream_count );
DLLEXPORT MyGL_VertexAttributeStream MyGL_vertexAttributeStream( const char *name );
DLLEXPORT MyGL_ColorFormat MyGL_colorFormat( const char *name );
DLLEXPORT void MyGL_drawStream( const char *streams );
DLLEXPORT void MyGL_applyCull ();
DLLEXPORT void MyGL_applyDepth();
DLLEXPORT void MyGL_applyBlend();
DLLEXPORT void MyGL_resetCull ();
DLLEXPORT void MyGL_resetDepth();
DLLEXPORT void MyGL_resetBlend();

typedef char (*MyGl_GetCharFunc)( void * );

DLLEXPORT int32_t MyGL_loadSourceLibrary( MyGl_GetCharFunc source_feed, void *source_param, const char *alias );
DLLEXPORT int32_t MyGL_loadShader( MyGl_GetCharFunc source_feed, void *source_param, const char *alias );
DLLEXPORT int32_t MyGL_loadShaderStr( const char *source_str, void *source_param, const char *alias );
#ifdef __cplusplus
} /* extern "C" */

#endif
