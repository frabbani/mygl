#pragma once

#include <stdint.h>

typedef struct{
  float x, y;
}WaveFront_vec2_t;

typedef struct{
  float x, y, z;
}WaveFront_vec3_t;

typedef struct{
  int   vs[3];
  int   ts[3];
  int   ns[3];
}WaveFront_face_t;


typedef struct WAVE_obj_s{
  char name[256];

  uint32_t num_verts;
  WaveFront_vec3_t *verts;

  uint32_t num_uvs;
  WaveFront_vec2_t *uvs;

  uint32_t num_norms;
  WaveFront_vec3_t *norms;

  uint32_t num_faces;
  WaveFront_face_t *faces;

}WaveFront_obj_t;


extern void WaveFront_obj_term( WaveFront_obj_t *obj );
extern int  WaveFront_obj_load( WaveFront_obj_t *obj, const char objfile[], float scale, int term );

