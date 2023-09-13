#if 0
Name "Vertex Position, Color, and Texture (Masked)"

Passes "Main"

#endif


Main {

/**
Cull Front
Blend SrcAlpha OneMinusSrcAlpha Add
Depth Off ReadOnly
Stencil Equal WriteMask.NoBits Value.3 ValueMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Keep
**/

#define TRANSFORM
#define VTX_P_C_T0
#include "assets/shaders/includes.glsl"

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
  vec3  c = texture2D( tex, var_t.xy ).rgb;
  gl_FragData[0] = vec4( c, 0.25 );
}

#endif

}
