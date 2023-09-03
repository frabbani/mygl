
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


#ifdef VTX_P_C_T0

#ifdef __vert__
layout(location = 0) in vec4 vtx_p;
layout(location = 1) in vec4 vtx_c;
layout(location = 2) in vec4 vtx_t0;
#endif

#endif