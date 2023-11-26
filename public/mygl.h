#pragma once

#include <GL/glew.h>

#include "strn.h"
#include "vec.h"
#include "image.h"
#include "text.h"


typedef void (*MyGL_LogFunc)(const char*);

#define MYGL_MAX_SAMPLERS       16
#define MYGL_MAX_VERTEX_ATTRIBS 16
#define MYGL_TEXTURE_USAGE_UNIT (GL_TEXTURE0 + 8)

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


typedef enum MyGL_VertexAttribType_e {
  MYGL_VERTEX_CHAR    = GL_BYTE,
  MYGL_VERTEX_UCHAR   = GL_UNSIGNED_BYTE,
  MYGL_VERTEX_SHORT   = GL_SHORT,
  MYGL_VERTEX_USHORT  = GL_UNSIGNED_SHORT,
  MYGL_VERTEX_FLOAT   = GL_FLOAT,
  MYGL_VERTEX_INT     = GL_INT,
  MYGL_VERTEX_UINT    = GL_UNSIGNED_INT
} MyGL_VertexAttribType;

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



typedef struct MyGL_VertexAttrib_s{
  MyGL_VertexAttribType type;
  MyGL_Components components;
  GLboolean normalized;
}MyGL_VertexAttrib;

typedef struct MyGL_VertexAttributeStream_s{
  struct{
    MyGL_Str64 name;
    GLuint maxCount;  // #. of elements
    MyGL_VertexAttrib attrib;
  }info;
  MyGL_ArrPtr arr;
}MyGL_VertexAttributeStream;

typedef struct MyGL_VboStream_s{
  struct{
    MyGL_Str64 name;
    GLuint maxCount;  // #. of elements
    GLuint numAttribs;
    MyGL_VertexAttrib attribs[ MYGL_MAX_VERTEX_ATTRIBS ];
  }info;
  void *data;
}MyGL_VboStream;

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

typedef enum MyGL_StencilAction_e{
  MYGL_KEEP = GL_KEEP,
  MYGL_ZEROOUT = GL_ZERO,
  MYGL_REPLACE = GL_REPLACE,
  MYGL_INCR = GL_INCR,
  MYGL_INCR_WRAP = GL_INCR_WRAP,
  MYGL_DECR = GL_DECR,
  MYGL_DECR_WRAP = GL_DECR_WRAP,
  MYGL_INVERT = GL_INVERT,
}MyGL_StencilAction;

typedef MyGL_CullMode MyGL_StencilType;
typedef MyGL_DepthMode MyGL_StencilMode;

typedef struct MyGL_StencilOp_s{
  MyGL_StencilAction stencilFail;
  MyGL_StencilAction stencilPassDepthFail;
  MyGL_StencilAction stencilPassDepthPass;
}MyGL_StencilOp;

typedef struct MyGL_StencilTest_s{
  MyGL_StencilMode mode;
  GLint            ref;
  GLuint           mask;
}MyGL_StencilTest;

typedef struct MyGL_Stencil_s{
  GLboolean        on;
  // GLboolean        separate;
  GLuint           writeMask; // 8 bit
  MyGL_StencilTest stencilTest;
  MyGL_StencilOp   stencilOp;
  // MyGL_StencilTest stencilBackTest;
  // MyGL_StencilOp   stencilBackOp;

}MyGL_Stencil;

typedef struct MyGL_ColorMask_s{
  GLboolean red, green, blue, alpha;
}MyGL_ColorMask;

typedef struct MyGL_s{
  MyGL_Cull  cull;
  MyGL_Depth depth;
  MyGL_Blend blend;
  MyGL_Stencil stencil;
  MyGL_ColorMask colorMask;

  MyGL_Vec4 clearColor;
  float     clearDepth;
  GLint     clearStencil;

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


typedef enum MyGL_UniformType_e {
  MYGL_UNIFORM_FLOAT       = GL_FLOAT,
  MYGL_UNIFORM_FLOAT_VEC2  = GL_FLOAT_VEC2,
  MYGL_UNIFORM_FLOAT_VEC3  = GL_FLOAT_VEC3,
  MYGL_UNIFORM_FLOAT_VEC4  = GL_FLOAT_VEC4,
  MYGL_UNIFORM_FLOAT_MAT2  = GL_FLOAT_MAT2,
  MYGL_UNIFORM_FLOAT_MAT3  = GL_FLOAT_MAT3,
  MYGL_UNIFORM_FLOAT_MAT4  = GL_FLOAT_MAT4,
  MYGL_UNIFORM_INT         = GL_INT,
  MYGL_UNIFORM_INT_VEC2    = GL_INT_VEC2,
  MYGL_UNIFORM_INT_VEC3    = GL_INT_VEC3,
  MYGL_UNIFORM_INT_VEC4    = GL_INT_VEC4,
  MYGL_UNIFORM_UINT        = GL_UNSIGNED_INT,
  MYGL_UNIFORM_UINT_VEC2   = GL_UNSIGNED_INT_VEC2,
  MYGL_UNIFORM_UINT_VEC3   = GL_UNSIGNED_INT_VEC3,
  MYGL_UNIFORM_UINT_VEC4   = GL_UNSIGNED_INT_VEC4,
} MyGL_UniformType;


typedef union MyGL_UniformValue_u{
  GLint      int32;
  MyGL_IVec2 ivec2;
  MyGL_IVec3 ivec3;
  MyGL_IVec4 ivec4;

  GLuint     uint32;
  MyGL_UVec2 uvec2;
  MyGL_UVec3 uvec3;
  MyGL_UVec4 uvec4;

  GLfloat    floa;
  MyGL_Vec2  vec2;
  MyGL_Vec3  vec3;
  MyGL_Vec4  vec4;

  MyGL_Mat4  mat2;
  MyGL_Mat4  mat3;
  MyGL_Mat4  mat4;

}MyGL_UniformValue;

typedef struct{
  struct{
    MyGL_Str64         name;
    MyGL_UniformType   type;
  }info;
  MyGL_UniformValue  *value;
}MyGL_Uniform;


#ifdef __cplusplus
extern "C" {
#endif

#define DLLEXPORT __declspec( dllexport )

DLLEXPORT void  MyGL_terminate();
DLLEXPORT MyGL *MyGL_initialize( MyGL_LogFunc logger, int32_t initialize_glew, uint32_t stream_count );
DLLEXPORT MyGL_VertexAttributeStream MyGL_vertexAttributeStream( const char *name );
DLLEXPORT MyGL_ColorFormat MyGL_colorFormat( const char *name );
DLLEXPORT void MyGL_drawStreaming( const char *streams );
DLLEXPORT void MyGL_applyCull ();
DLLEXPORT void MyGL_applyDepth();
DLLEXPORT void MyGL_applyBlend();
DLLEXPORT void MyGL_applyStencil();
DLLEXPORT void MyGL_applyColorMask();
DLLEXPORT void MyGL_resetCull ();
DLLEXPORT void MyGL_resetDepth();
DLLEXPORT void MyGL_resetBlend();
DLLEXPORT void MyGL_resetStencil();
DLLEXPORT void MyGL_resetColorMask();

DLLEXPORT void MyGL_bindSamplers();

typedef char (*MyGl_GetCharFunc)( void * );

DLLEXPORT GLboolean MyGL_loadShaderLibrary( MyGl_GetCharFunc source_feed, void *source_param, const char *alias );
DLLEXPORT GLboolean MyGL_loadShaderLibraryStr( const char *source_str, const char *alias );
DLLEXPORT GLboolean MyGL_loadShader( MyGl_GetCharFunc source_feed, void *source_param, const char *alias );
DLLEXPORT GLboolean MyGL_loadShaderStr( const char *source_str, const char *alias );


DLLEXPORT GLboolean MyGL_createTexture2D( const char *name,
                                          MyGL_ROImage image, const char *format,
                                          GLboolean filtered, GLboolean mipmapped, GLboolean repeat  );

DLLEXPORT GLboolean MyGL_createTexture2DArray( const char *name,
                                               MyGL_ROImage image_atlas, uint32_t num_rows, uint32_t num_cols, const char *format,
                                               GLboolean filtered, GLboolean mipmapped, GLboolean repeat  );


DLLEXPORT void MyGL_clear( GLboolean color, GLboolean depth, GLboolean stencil );
DLLEXPORT void MyGL_drawVbo( const char *name, MyGL_Primitive primitive, GLint start_index, GLsizei index_count );
DLLEXPORT MyGL_VboStream MyGL_vboStream( const char *name );
DLLEXPORT void MyGL_vboPush( const char *name );
DLLEXPORT GLboolean MyGL_createVbo( const char *name, uint32_t size, const MyGL_VertexAttrib *attribs, uint32_t num_attribs );

DLLEXPORT GLboolean MyGL_Debug_getChatty();
DLLEXPORT void MyGL_Debug_setChatty( GLboolean chatty );

DLLEXPORT void MyGL_Trace_Stencil_set( char *output, uint32_t size );
DLLEXPORT void MyGL_Trace_Stencil_tag( const char *tag );

#ifdef __cplusplus
} /* extern "C" */

#endif
