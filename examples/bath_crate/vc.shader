#if 0
Name "Vertex Position, Color"

Passes "Main"

#endif


Main {

/**
Cull On
Blend SrcAlpha OneMinusSrcAlpha Add
Depth LEqual
Stencil Always WriteMask.AllBits Value.3 ValueMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace
**/

#define TRANSFORM
#include "includes.glsl"

vary vec4 var_c;

#ifdef __vert__

layout(location = 0) in vec4 vtx_p;
layout(location = 1) in vec4 vtx_c;

void main(){
  gl_Position = _PVW * vtx_p;
  var_c = vtx_c;
}

#endif

#ifdef __frag__

void main(){
  gl_FragData[0] = var_c;
}

#endif

}


