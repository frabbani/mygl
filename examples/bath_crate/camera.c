#include "camera.h"

#include <mygl/public/vecdefs.h>
#include "math.h"


void camera_move( camera_t *cam, float forward, float sideways, float upwards )
{
  float mag = 1.0f;

  MyGL_Vec3 r = MyGL_vec3( cam->r.x, cam->r.y, 0.0f );
  MyGL_Vec3 l = MyGL_vec3( cam->l.x, cam->l.y, 0.0f );

  mag = sqrtf( MyGL_vec3Dot( r, r ) );
  mag = mag < 1e-6f ? 1.0f : mag;
  r = MyGL_vec3Scale( r, sideways / mag );

  mag = sqrtf( MyGL_vec3Dot( l, l ) );
  mag = mag < 1e-6f ? 1.0f : mag;
  l = MyGL_vec3Scale( l, forward / mag );

  cam->p = MyGL_vec3Add( cam->p, MyGL_vec3Add( r, l ) );
  cam->p.z += upwards;
}

void camera_face( camera_t *cam, float yaw, float pitch ){
  cam->l = MyGL_vec3L();
  cam->r = MyGL_vec3R();
  cam->u = MyGL_vec3U();

  cam->yaw += yaw;
  cam->pitch += pitch;
  CLAMP( cam->pitch, -89.0f, 89.0f );
  cam->yaw = fmodf( cam->yaw, 360.0f );
  if( cam->yaw < 0.0f )
    cam->yaw += 360.0f;

  float co, sn;
  co = cosf( cam->yaw * PI / 180.0f - PI / 2.0f );
  sn = sinf( cam->yaw * PI / 180.0f - PI / 2.0f );
  cam->r = MyGL_vec3Rotate( MyGL_vec3R(), MyGL_vec3U(), co, sn );
  cam->l = MyGL_vec3Rotate( MyGL_vec3L(), MyGL_vec3U(), co, sn );
  co = cosf( cam->pitch * PI / 180.0f );
  sn = sinf( cam->pitch * PI / 180.0f );
  cam->l = MyGL_vec3Rotate( cam->l, cam->r, co, sn );
  cam->u = MyGL_vec3Rotate( MyGL_vec3U(), cam->r, co, sn );
}
