#ifdef TRANSFORM

varying vec3 _cam_p, _cam_bs[3]; 

#ifdef __vert__

uniform mat4 _PVW;  //<0>
uniform mat4 _P;    //<1>
uniform mat4 _V;    //<2>
uniform mat4 _W;    //<3>

#endif

#endif