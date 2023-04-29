#if 0
Name "Vertex Position and Color"
// TODO maybe support builtin pass ClearDepth, ClearColor, ClearStencil?
Passes "Main"

#define TRANSFORM
#include "includes.shader"

#endif


#ifdef Main

/**
Cull Back
Blend SrcAlpha OneMinusSrcAlpha Add 

//Depth: Off Less Greater Equal LEqual, GEqual ReadOnly
Depth LEqual

//TODO: support stencil

**/

varying vec4 var_c;


#ifdef __vert__

attribute vec4 vtx_p; 
attribute vec4 vtx_c;

uniform mat4 _PVW;  //<0>
uniform mat4 _P;    //<1>
uniform mat4 _V;    //<2>
uniform mat4 _W;    //<3>


void main(){
  gl_Position = _PVW * vtx_p;
  var_c = vtx_c;
}

#endif

#ifdef __frag__

void main(){
  gl_FragData[0] = vtx_c;
}

#endif

#endif