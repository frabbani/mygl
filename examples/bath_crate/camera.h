#pragma once

#include "defs.h"
#include <mygl/public/vec.h>

typedef struct{
  float yaw, pitch;
  MyGL_Vec3 p, r, l, u;
  float  FOV, D_n, D_f;
}camera_t;


extern void camera_face( camera_t *cam, float yaw, float pitch );
extern void camera_move( camera_t *cam, float forward, float sideways, float upwards );
