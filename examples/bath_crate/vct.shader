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
Stencil Always WriteMask.AllBits Value.1 ValueMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace
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
**/

#define TRANSFORM
#define VTX_P_C_T0
#include "includes.glsl"

vary vec4 var_c;
vary vec4 var_t;

#ifdef __vert__

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
**/

#define TRANSFORM
#define VTX_P_C_T0
#include "includes.glsl"

vary vec4 var_c;
vary vec4 var_t;

#ifdef __vert__

void main(){
  gl_Position = _PVW * vtx_p;
  var_c = vtx_c;
  var_t = vtx_t0;
}

#endif

#ifdef __frag__

layout(binding = 0) uniform sampler2D tex;

void main(){
  vec3  c = texture2D( tex, var_t.xy ).rgb * var_c.rgb * 0.33;
  gl_FragData[0] = vec4( c, 1.0 );
}

#endif

}


