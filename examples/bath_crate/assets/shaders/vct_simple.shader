#if 0
Name "Vertex Position, Color, and Texture (Simple)"

Passes "Main"

#endif


Main {

/**
Cull Back
Blend SrcAlpha OneMinusSrcAlpha Add
Depth LEqual ReadOnly
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
  float a = texture( tex, var_t.xy ).r;
  gl_FragData[0] = vec4( var_c.rgb, a );
}

#endif

}

