# MyGL
WIP - Humble beginnings of my OpenGL wrapper library written in C++ but with a C front-end. Requires GLEW. 

Shader Pass(es) Example
```
#if 0
Name "Vertex Position, Color, and Texture"

Passes "ZWrite, Back, Front"

#endif


ZWrite {

/**
Cull Off
Blend Off
ColorWrite Off
Depth LEqual
Stencil Always WriteMask.AllBits Value.3 ValueMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace
**/

#define TRANSFORM
#include "includes.glsl"

#ifdef __vert__

layout(location = 0) in vec4 vtx_p;

void main(){
  gl_Position = _PVW * vtx_p;
}

#endif

#ifdef __frag__

void main(){
  gl_FragData[0] = vec4( 0.0 );
}

#endif

}


Front {

/**
Cull Back
Depth Equal ReadOnly
ColorWrite Red, Blue
**/

#define TRANSFORM
#include "includes.glsl"

vary vec4 var_c;
vary vec4 var_t;

#ifdef __vert__

layout(location = 0) in vec4 vtx_p;
layout(location = 1) in vec4 vtx_c;
layout(location = 2) in vec4 vtx_t0;


void main(){
  gl_Position = _PVW * vtx_p;
  var_c = vtx_c;
  var_t = vtx_t0;
}

#endif

#ifdef __frag__

layout(binding = 0) uniform sampler2D tex;

void main(){
  vec3  c = texture2D( tex, var_t.xy ).rgb * var_c.rgb;
  gl_FragData[0] = vec4( c, 1.0 );
}

#endif

}


Back {

/**
Cull Front
Depth Equal ReadOnly
ColorWrite Green, Blue
**/

#define TRANSFORM
#include "includes.glsl"

vary vec4 var_c;
vary vec4 var_t;

#ifdef __vert__

 
layout(location = 0) in vec4 vtx_p;
layout(location = 1) in vec4 vtx_c;
layout(location = 2) in vec4 vtx_t0;


void main(){
  gl_Position = _PVW * vtx_p;
  var_c = vtx_c;
  var_t = vtx_t0;
}

#endif

#ifdef __frag__

layout(binding = 0) uniform sampler2D tex;

void main(){
  vec3  c = texture2D( tex, var_t.xy ).rgb * var_c.rgb;
  gl_FragData[0] = vec4( c, 1.0 );
}

#endif

}

```

Shader Header Example:
```
#ifdef TRANSFORM
uniform mat4 _P;
uniform mat4 _V;
uniform mat4 _W;
uniform mat4 _PVW;
#endif

#ifdef __vert__
#define vary out
#endif


#ifdef __frag__
#define vary in
#endif
```

Shader Pass Properties (aliases for some of these exist):
```
/**

// Cull: Back, Front, FrontAndBack/BackAndFront Off
Cull Front

//Blend: SrcAlpha OneMinusSrcAlpha Add, Off
Blend Off

// ColorWrite: Off, On, Red, Green, Blue
ColorWrite Red

// Depth/Stencil: Off Always, Never, NotEqual, Less, Greater, Equal, LEqual, GEqual, ReadOnly
Depth LEqual

// Stencil Test: Same as Depth (except for ReadOnly), Always (Default)
// Stencil Op:  Keep (Default), Zero, Replace, Incr, IncrWrap, Decr, DecrWrap, Invert
// stencil Write Mask = NoBits (all 0s), [number], AllBits (Default - all 1s)
Stencil Always WriteMask.AllBits Test.1 TestMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace

**/
```

