#ifdef MYGL_STATIC_INLINE_VEC

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable" 
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <stdint.h>

#include "vec.h"
#define _USE_MATH_DEFINES
#include <math.h>

/** Math below uses right-handed coordinates, where right is +x, look is +y, and up is +z **/

static inline MyGL_Vec2 MyGL_vec2(float x, float y) {
  MyGL_Vec2 v2;
  v2.x = x;
  v2.y = y;
  return v2;
}

static inline MyGL_Vec2 MyGL_vec2Add(MyGL_Vec2 lhs, MyGL_Vec2 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

static inline MyGL_Vec2 MyGL_vec2Sub(MyGL_Vec2 lhs, MyGL_Vec2 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static inline MyGL_Vec2 MyGL_vec2Scale(MyGL_Vec2 v, float s) {
  v.x *= s;
  v.y *= s;
  return v;
}

static inline float MyGL_vec2Dot(MyGL_Vec2 lhs, MyGL_Vec2 rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

static inline float MyGL_vec2Mag(MyGL_Vec2 v) {
  float s = v.x * v.x + v.y * v.y;
  if (s < 1e-9f)
    return 0.0f;
  return sqrtf(s);
}

static MyGL_Vec2 MyGL_vec2Norm(MyGL_Vec2 v) {
  float s = v.x * v.x + v.y * v.y;
  if (s < 1e-9f)
    return MyGL_vec2(0.0f, 0.0f);
  return MyGL_vec2Scale(v, 1.0f / s);
}

static const MyGL_Vec3 MyGL_vec3Zero = { { { 0.0f, 0.0f, 0.0f } } };
static const MyGL_Vec3 MyGL_vec3R = { { { 1.0f, 0.0f, 0.0f } } };
static const MyGL_Vec3 MyGL_vec3L = { { { 0.0f, 1.0f, 0.0f } } };
static const MyGL_Vec3 MyGL_vec3U = { { { 0.0f, 0.0f, 1.0f } } };

static const MyGL_Vec3 MyGL_vec3X = { { { 1.0f, 0.0f, 0.0f } } };
static const MyGL_Vec3 MyGL_vec3Y = { { { 0.0f, 1.0f, 0.0f } } };
static const MyGL_Vec3 MyGL_vec3Z = { { { 0.0f, 0.0f, 1.0f } } };

static inline MyGL_Vec3 MyGL_vec3(float x, float y, float z) {
  MyGL_Vec3 v3;
  v3.x = x;
  v3.y = y;
  v3.z = z;
  return v3;
}

static inline MyGL_Vec3 MyGL_vec3Add(MyGL_Vec3 lhs, MyGL_Vec3 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}

static inline MyGL_Vec3 MyGL_vec3Sub(MyGL_Vec3 lhs, MyGL_Vec3 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static inline MyGL_Vec3 MyGL_vec3Scale(MyGL_Vec3 v, float s) {
  v.x *= s;
  v.y *= s;
  v.z *= s;
  return v;
}

static inline float MyGL_vec3Dot(MyGL_Vec3 lhs, MyGL_Vec3 rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

static inline float MyGL_vec3Mag(MyGL_Vec3 v) {
  float s = v.x * v.x + v.y * v.y + v.z * v.z;
  if (s < 1e-9f)
    return 0.0f;
  return sqrtf(s);
}

static MyGL_Vec3 MyGL_vec3Norm(MyGL_Vec3 v) {
  float s = v.x * v.x + v.y * v.y + v.z * v.z;
  if (s < 1e-9f)
    return MyGL_vec3(0.0f, 0.0f, 0.0f);
  return MyGL_vec3Scale(v, 1.0f / s);
}

static inline MyGL_Vec3 MyGL_vec3Cross(MyGL_Vec3 lhs, MyGL_Vec3 rhs) {
  MyGL_Vec3 n;
  n.x = (lhs.y * rhs.z - lhs.z * rhs.y);
  n.y = -(lhs.x * rhs.z - lhs.z * rhs.x);
  n.z = (lhs.x * rhs.y - lhs.y * rhs.x);
  return n;
}

static MyGL_Vec3 MyGL_vec3Rotate(MyGL_Vec3 p, MyGL_Vec3 axis, float radians) {
  MyGL_Vec3 proj, perp, s, t;

  proj = MyGL_vec3Scale(axis, MyGL_vec3Dot(p, axis));
  perp = MyGL_vec3Sub(p, proj);

  float co = cosf(radians);
  float sn = sinf(radians);
  s = MyGL_vec3Scale(perp, co);
  t = MyGL_vec3Scale(MyGL_vec3Cross(axis, perp), sn);

  return MyGL_vec3Add(proj, MyGL_vec3Add(s, t));
}

static inline MyGL_Vec4 MyGL_vec4(float x, float y, float z, float w) {
  MyGL_Vec4 v;
  v.x = x;
  v.y = y;
  v.z = z;
  v.w = w;
  return v;
}

static inline MyGL_Vec4 MyGL_vec4Scale(MyGL_Vec4 v, float s) {
  v.x *= s;
  v.y *= s;
  v.z *= s;
  v.w *= s;
  return v;
}

static inline float MyGL_vec4Dot(MyGL_Vec4 lhs, MyGL_Vec4 rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

static inline float MyGL_mat2Det(MyGL_Mat2 M) {
  return M.e00 * M.e11 - M.e10 * M.e01;
}

static inline MyGL_Mat2 MyGL_mat2Inv(MyGL_Mat2 M) {
  float s = MyGL_mat2Det(M);
  if (0.0f == s) {
    M.e00 = M.e01 = M.e10 = M.e11 = 0.0f;
    return M;
  }
  s = 1.0f / s;
  MyGL_Mat2 M_inv;
  M_inv.e00 = +s * M.e11;
  M_inv.e01 = -s * M.e01;
  M_inv.e10 = -s * M.e10;
  M_inv.e11 = +s * M.e00;
  return M_inv;
}

static inline MyGL_Vec2 MyGL_vec2Transf(MyGL_Mat2 M, MyGL_Vec2 v) {
  MyGL_Vec2 r;
  r.x = MyGL_vec2Dot(MyGL_vec2(M.e00, M.e01), v);
  r.y = MyGL_vec2Dot(MyGL_vec2(M.e10, M.e11), v);
  return r;
}

static inline MyGL_Vec4 MyGL_vec4Transf(MyGL_Mat4 M, MyGL_Vec4 v) {
  MyGL_Vec4 r;
  r.x = MyGL_vec4Dot(MyGL_vec4(M.e00, M.e01, M.e02, M.e03), v);
  r.y = MyGL_vec4Dot(MyGL_vec4(M.e10, M.e11, M.e12, M.e13), v);
  r.z = MyGL_vec4Dot(MyGL_vec4(M.e20, M.e21, M.e22, M.e23), v);
  r.w = MyGL_vec4Dot(MyGL_vec4(M.e30, M.e31, M.e32, M.e33), v);
  return r;
}

static const MyGL_Mat4 MyGL_mat4Identity = { { { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, } } };

static inline MyGL_Mat4 MyGL_mat4Multiply(MyGL_Mat4 lhs, MyGL_Mat4 rhs) {
  MyGL_Mat4 mat4;
  for (size_t i = 0; i < 4; i++)
    for (size_t j = 0; j < 4; j++) {
      mat4.f4x4[i][j] = lhs.f4x4[i][0] * rhs.f4x4[0][j] + lhs.f4x4[i][1] * rhs.f4x4[1][j] + lhs.f4x4[i][2] * rhs.f4x4[2][j] + lhs.f4x4[i][3] * rhs.f4x4[3][j];
    }
  return mat4;
}

static MyGL_Mat4 MyGL_mat4Perspective(float aspect, float fovInRadians, float min, float max) {
  // given point <x,y,z,w>, and 4 by 4 matrix mat
  //    | ...,   0,   0,   0 |   | x |   | x' |
  //    |   0,   0, ...,   0 | * | y | = | y' |
  //    |   0,   a,   0,   b |   | z |   | z' |
  //    |   0,   1,   0,   0 |   | 1 |   | w' |
  //
  // x' = x / tan( fov/2 )
  // y' = aspect * z / tan( fov/2 )
  // z' = a * y + b
  // w' = y
  //
  // NOTES:
  // z-buffer := z'/w, or z'/y. for min/max distances, we want:
  //    i) ( a * min + b ) / min = -1
  //   ii) ( a * max + b ) / max = +1
  // solve for a & b (opengl z-buffer ranges from -1 to +1)
  //
  // screen coordinates are signed normalized, so the
  // perspective divide maps x & y values to the -1/1 range, meaning:
  //  -1 <= x'/w <= 1 &
  //  -1 <= y'/w <= 1
  // working in one dimension:
  // let x := sin( theta ), z:= cos( theta ), because look is adj to view angle, right is opp
  // x * D / z = 1, when theta is half of the fov,
  // solving for D, we get D = z/x, or cos( fov/2 ) / sin( fov/2 ), or cotan( fov/2 )

  // solution:
  //
  // 1. a * min + b = min * n,
  // 2. a * max + b = max * f,
  // subtract 2. from 1 and solve for a
  // substitute value of a in 2. & solve for b

  const float z_n = -1.0f;
  const float z_f = +1.0f;
  float a = (z_f * max - z_n * min) / (max - min);
  float b = z_f * max - a * max;

  MyGL_Mat4 mat4;
  for (size_t i = 0; i < 16; i++) {
    mat4.f16[i] = 0.0f;
  }

  float cotan = 1.0f / tanf(fovInRadians * 0.5f);
  mat4.f4x4[0][0] = cotan;
  mat4.f4x4[1][2] = aspect * cotan;
  mat4.f4x4[2][1] = a;
  mat4.f4x4[2][3] = b;
  mat4.f4x4[3][1] = 1.0f;
  return mat4;
}

static MyGL_Mat4 MyGL_mat4Ortho(uint32_t w, uint32_t h, float min, float max) {
  // given point <x,y,z,w>, and 4 by 4 matrix mat
  //    | ...,   0,   0,   0 |   | x |   | x' |
  //    |   0,   0, ...,   0 | * | y | = | y' |
  //    |   0,   a,   0,   b |   | z |   | z' |
  //    |   0,   1,   0,   0 |   | 1 |   | w' |
  //
  // x' = x / w
  // y' = z / h
  // z' = a * y + b
  // w' = 1

  float a = 1.0f / max;
  float b = -min / max;

  MyGL_Mat4 mat4;
  for (size_t i = 0; i < 16; i++)
    mat4.f16[i] = 0.0f;

  mat4.f4x4[0][0] = 2.0f / (float) w;
  mat4.f4x4[1][2] = 2.0f / (float) h;
  mat4.f4x4[2][1] = a;
  mat4.f4x4[2][3] = b;
  mat4.f4x4[3][3] = 1.0f;
  return mat4;
}

static MyGL_Mat4 MyGL_mat4World(MyGL_Vec3 p, MyGL_Vec3 r, MyGL_Vec3 l, MyGL_Vec3 u) {
  MyGL_Mat4 mat4;
  mat4.f4x4[0][0] = r.x;
  mat4.f4x4[1][0] = r.y;
  mat4.f4x4[2][0] = r.z;
  mat4.f4x4[3][0] = 0.0f;

  mat4.f4x4[0][1] = l.x;
  mat4.f4x4[1][1] = l.y;
  mat4.f4x4[2][1] = l.z;
  mat4.f4x4[3][1] = 0.0f;

  mat4.f4x4[0][2] = u.x;
  mat4.f4x4[1][2] = u.y;
  mat4.f4x4[2][2] = u.z;
  mat4.f4x4[3][2] = 0.0f;

  mat4.f4x4[0][3] = p.x;
  mat4.f4x4[1][3] = p.y;
  mat4.f4x4[2][3] = p.z;
  mat4.f4x4[3][3] = 1.0f;
  return mat4;
}

static MyGL_Mat4 MyGL_mat4View(MyGL_Vec3 p, MyGL_Vec3 r, MyGL_Vec3 l, MyGL_Vec3 u) {
  MyGL_Mat4 mat4;

  mat4.f4x4[0][0] = r.x;
  mat4.f4x4[0][1] = r.y;
  mat4.f4x4[0][2] = r.z;
  mat4.f4x4[0][3] = -MyGL_vec3Dot(p, r);

  mat4.f4x4[1][0] = l.x;
  mat4.f4x4[1][1] = l.y;
  mat4.f4x4[1][2] = l.z;
  mat4.f4x4[1][3] = -MyGL_vec3Dot(p, l);

  mat4.f4x4[2][0] = u.x;
  mat4.f4x4[2][1] = u.y;
  mat4.f4x4[2][2] = u.z;
  mat4.f4x4[2][3] = -MyGL_vec3Dot(p, u);

  mat4.f4x4[3][0] = 0.0f;
  mat4.f4x4[3][1] = 0.0f;
  mat4.f4x4[3][2] = 0.0f;
  mat4.f4x4[3][3] = 1.0f;
  return mat4;
}

static MyGL_Mat4 MyGL_mat4Yaw(MyGL_Vec3 p, float angleInRadians) {
  MyGL_Mat4 mat4 = MyGL_mat4Identity;
  float co = cosf(angleInRadians);
  float sn = sinf(angleInRadians);

  mat4.f4x4[0][0] = co;
  mat4.f4x4[0][1] = -sn;
  mat4.f4x4[1][0] = sn;
  mat4.f4x4[1][1] = co;

  mat4.f4x4[0][3] = p.x;
  mat4.f4x4[1][3] = p.y;
  mat4.f4x4[2][3] = p.z;
  mat4.f4x4[3][3] = 1.0f;
  return mat4;
}

static MyGL_Mat4 MyGL_mat4RotateAxis(MyGL_Vec3 p, float angleInRadians, uint32_t axisType) {
  if (1 == axisType)
    angleInRadians = -angleInRadians;

  float co = cosf(angleInRadians);
  float sn = sinf(angleInRadians);
  MyGL_Mat4 mat4 = MyGL_mat4Identity;
  mat4.f4x4[0][3] = p.x;
  mat4.f4x4[1][3] = p.y;
  mat4.f4x4[2][3] = p.z;
  mat4.f4x4[3][3] = 1.0f;

  axisType %= 3;
  if (0 == axisType) {
    mat4.f4x4[1][1] = co;
    mat4.f4x4[1][2] = -sn;
    mat4.f4x4[2][1] = sn;
    mat4.f4x4[2][2] = co;

  } else if (1 == axisType) {
    mat4.f4x4[0][0] = co;
    mat4.f4x4[0][2] = -sn;
    mat4.f4x4[2][0] = sn;
    mat4.f4x4[2][2] = co;

  } else if (2 == axisType) {
    mat4.f4x4[0][0] = co;
    mat4.f4x4[0][1] = -sn;
    mat4.f4x4[1][0] = sn;
    mat4.f4x4[1][1] = co;

  }
  return mat4;
}

static MyGL_Mat4 MyGL_mat4Scale(MyGL_Vec3 p, float xScale, float yScale, float zScale) {
  MyGL_Mat4 mat4 = MyGL_mat4Identity;
  mat4.f4x4[0][0] = xScale;
  mat4.f4x4[1][1] = yScale;
  mat4.f4x4[2][2] = zScale;
  mat4.f4x4[0][3] = p.x;
  mat4.f4x4[1][3] = p.y;
  mat4.f4x4[2][3] = p.z;
  mat4.f4x4[3][3] = 1.0f;

  return mat4;
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif