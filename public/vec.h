#pragma once


typedef struct MyGL_Vec2_s{
  union{
    struct{
      float x, y;
    };
    float f2[2];
  };
}MyGL_Vec2;


typedef struct MyGL_Vec3_s{
  union{
    struct{
      float x, y, z;
    };
    float f3[3];
  };
}MyGL_Vec3;


typedef struct MyGL_Vec4_s{
  union{
    struct{
      float x, y, z, w;
    };
    float f4[4];
  };
}MyGL_Vec4;


typedef struct MyGL_Mat4_s{
  union{
    struct{
      float e00, e01, e02, e03;
      float e10, e11, e12, e13;
      float e20, e21, e22, e23;
      float e30, e31, e32, e33;
    };
    float f16[4*4];
    float f4x4[4][4];
  };
}MyGL_Mat4;

