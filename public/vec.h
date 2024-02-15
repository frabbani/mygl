#pragma once

#include <cstdint>

typedef struct MyGL_IVec2_s{
  union {
    struct {
      int32_t x, y;
    };
    int32_t i2[2];
  };
} MyGL_IVec2;

typedef struct MyGL_IVec3_s{
  union {
    struct {
      int32_t x, y, z;
    };
    int32_t i3[3];
  };
} MyGL_IVec3;

typedef struct MyGL_IVec4_s{
  union {
    struct {
      int32_t x, y, z, w;
    };
    int32_t i4[4];
  };
} MyGL_IVec4;

typedef struct MyGL_UVec2_s{
  union {
    struct {
      uint32_t x, y;
    };
    uint32_t u2[2];
  };
} MyGL_UVec2;

typedef struct MyGL_UVec3_s{
  union {
    struct {
      uint32_t x, y, z;
    };
    uint32_t u3[3];
  };
} MyGL_UVec3;

typedef struct MyGL_UVec4_s{
  union {
    struct {
      uint32_t x, y, z, w;
    };
    uint32_t u4[4];
  };
} MyGL_UVec4;

typedef struct MyGL_Vec2_s{
  union {
    struct {
      float x, y;
    };
    float f2[2];
  };
} MyGL_Vec2;

typedef struct MyGL_Vec3_s{
  union {
    struct {
      float x, y, z;
    };
    float f3[3];
  };
} MyGL_Vec3;

typedef struct MyGL_Vec4_s{
  union {
    struct {
      float x, y, z, w;
    };
    float f4[4];
  };
} MyGL_Vec4;

typedef struct MyGL_Mat2_s{
  union {
    struct {
      float e00, e01;
      float e10, e11;
    };
    float f4[2 * 2];
    float f2x2[2][2];
  };
} MyGL_Mat2;

typedef struct MyGL_Mat3_s{
  union {
    struct {
      float e00, e01, e02;
      float e10, e11, e12;
      float e20, e21, e22;
    };
    float f9[3 * 3];
    float f3x3[3][3];
  };
} MyGL_Mat3;

typedef struct MyGL_Mat4_s{
  union {
    struct {
      float e00, e01, e02, e03;
      float e10, e11, e12, e13;
      float e20, e21, e22, e23;
      float e30, e31, e32, e33;
    };
    float f16[4 * 4];
    float f4x4[4][4];
  };
} MyGL_Mat4;

