#pragma once

#include "vec.h"

/** Math below uses right-handed coordinates, where right is +x, look is +y, and up is +z **/

MyGL_Vec2 MyGL_vec2(float x, float y);

MyGL_Vec2 MyGL_vec2Add(MyGL_Vec2 lhs, MyGL_Vec2 rhs);

MyGL_Vec2 MyGL_vec2Sub(MyGL_Vec2 lhs, MyGL_Vec2 rhs);

MyGL_Vec2 MyGL_vec2Scale(MyGL_Vec2 v, float s);

float MyGL_vec2Dot(MyGL_Vec2 lhs, MyGL_Vec2 rhs);

float MyGL_vec2Mag(MyGL_Vec2 v);

MyGL_Vec2 MyGL_vec2Norm(MyGL_Vec2 v);

extern const MyGL_Vec3 MyGL_vec3Zero;
extern const MyGL_Vec3 MyGL_vec3X;
extern const MyGL_Vec3 MyGL_vec3Y;
extern const MyGL_Vec3 MyGL_vec3Z;

MyGL_Vec3 MyGL_vec3(float x, float y, float z);

MyGL_Vec3 MyGL_vec3Add(MyGL_Vec3 lhs, MyGL_Vec3 rhs);

MyGL_Vec3 MyGL_vec3Sub(MyGL_Vec3 lhs, MyGL_Vec3 rhs);

MyGL_Vec3 MyGL_vec3Scale(MyGL_Vec3 v, float s);

float MyGL_vec3Dot(MyGL_Vec3 lhs, MyGL_Vec3 rhs);

float MyGL_vec3Mag(MyGL_Vec3 v);

MyGL_Vec3 MyGL_vec3Norm(MyGL_Vec3 v);

MyGL_Vec3 MyGL_vec3Cross(MyGL_Vec3 lhs, MyGL_Vec3 rhs);

MyGL_Vec3 MyGL_vec3Rotate(MyGL_Vec3 p, MyGL_Vec3 axis, float radians);

MyGL_Vec4 MyGL_vec4(float x, float y, float z, float w);

MyGL_Vec4 MyGL_vec4Scale(MyGL_Vec4 v, float s);

float MyGL_vec4Dot(MyGL_Vec4 lhs, MyGL_Vec4 rhs);

float MyGL_mat2Det(MyGL_Mat2 M);

MyGL_Mat2 MyGL_mat2Inv(MyGL_Mat2 M);

MyGL_Vec2 MyGL_vec2Transf(MyGL_Mat2 M, MyGL_Vec2 v);

MyGL_Vec4 MyGL_vec4Transf(MyGL_Mat4 M, MyGL_Vec4 v);

extern const MyGL_Mat4 MyGL_mat4Identity;

MyGL_Mat4 MyGL_mat4Multiply(MyGL_Mat4 lhs, MyGL_Mat4 rhs);

MyGL_Mat4 MyGL_mat4Perspective(float aspect, float fovInRadians, float min, float max);

MyGL_Mat4 MyGL_mat4Ortho(uint32_t w, uint32_t h, float min, float max);

MyGL_Mat4 MyGL_mat4World(MyGL_Vec3 p, MyGL_Vec3 r, MyGL_Vec3 l, MyGL_Vec3 u);

MyGL_Mat4 MyGL_mat4View(MyGL_Vec3 p, MyGL_Vec3 r, MyGL_Vec3 l, MyGL_Vec3 u);

MyGL_Mat4 MyGL_mat4Yaw(MyGL_Vec3 p, float angleInRadians);

MyGL_Mat4 MyGL_mat4RotateAxis(MyGL_Vec3 p, float angleInRadians, uint32_t axisType);

MyGL_Mat4 MyGL_mat4Scale(MyGL_Vec3 p, float xScale, float yScale, float zScale);
