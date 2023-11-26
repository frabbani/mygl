#include "mygl.h"

std::optional<mygl::trace::Output> mygl::trace::stencilOut = std::nullopt;

mygl::Cull::Cull(){
  on = GL_FALSE;
  cullMode = MYGL_BACK;
  frontIsCCW = GL_TRUE;
}

mygl::Cull& mygl::Cull::operator = ( const MyGL_Cull& rhs ){
  *(static_cast<MyGL_Cull *>(this)) = rhs;
  return *this;
}

void mygl::Cull::applyOn(){
  if( on )
    glEnable ( GL_CULL_FACE );
  else
    glDisable( GL_CULL_FACE );
}

void mygl::Cull::applyCullMode(){ glCullFace( cullMode ); }

void mygl::Cull::applyFrontFaceIsCCW(){
  if( frontIsCCW )
    glFrontFace( GL_CCW );
  else
    glFrontFace( GL_CW  );
}

bool mygl::Cull::operator != (const Cull& rhs ) const {
  return on != rhs.on || cullMode != rhs.cullMode || frontIsCCW != rhs.frontIsCCW;
}

bool mygl::operator != (const MyGL_StencilTest& lhs, const MyGL_StencilTest& rhs ){
  return lhs.mask != rhs.mask || lhs.mode != rhs.mode || lhs.ref != rhs.ref;
}

bool mygl::operator != (const MyGL_StencilOp& lhs, const MyGL_StencilOp& rhs ){
  return
      lhs.stencilFail != rhs.stencilFail ||
      lhs.stencilPassDepthFail != rhs.stencilPassDepthFail ||
      lhs.stencilPassDepthPass != rhs.stencilPassDepthPass;
}

bool mygl::operator != (const MyGL_Stencil& lhs, const MyGL_Stencil& rhs ){
  return
      lhs.on != rhs.on ||
      // lhs.separate != rhs.separate ||
      lhs.writeMask != rhs.writeMask ||
      lhs.stencilTest != rhs.stencilTest ||
      lhs.stencilOp != rhs.stencilOp; // ||
      // lhs.stencilBackTest != rhs.stencilBackTest ||
      // lhs.stencilBackOp != rhs.stencilBackOp;
}


void mygl::UniformSetter::set( GLint loc, const int32_t    *v ){
  glUniform1i( loc, *v );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_IVec2 *v ){
  glUniform2iv( loc, 1, v->i2 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_IVec3* v ){
  glUniform3iv( loc, 1, v->i3 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_IVec4* v ){
  glUniform4iv( loc, 1, v->i4 );
}

void mygl::UniformSetter::set( GLint loc, const uint32_t   *v ){
  glUniform1ui( loc, *v );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_UVec2 *v ){
  glUniform2uiv( loc, 1, v->u2 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_UVec3* v ){
  glUniform3uiv( loc, 1, v->u3 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_UVec4* v ){
  glUniform4uiv( loc, 1, v->u4 );
}

void mygl::UniformSetter::set( GLint loc, const float     *v ){
  glUniform1f( loc, *v );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_Vec2 *v ){
  glUniform2fv( loc, 1, v->f2 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_Vec3* v ){
  glUniform3fv( loc, 1, v->f3 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_Vec4* v ){
  glUniform4fv( loc, 1, v->f4 );
}

void mygl::UniformSetter::set( GLint loc, const MyGL_Mat2* v ){
  glUniformMatrix2fv( loc, 1, GL_TRUE, v->f4 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_Mat3* v ){
  glUniformMatrix3fv( loc, 1, GL_TRUE, v->f9 );
}
void mygl::UniformSetter::set( GLint loc, const MyGL_Mat4* v ){
  glUniformMatrix4fv( loc, 1, GL_TRUE, v->f16 );
}

void mygl::UniformSetter::set( GLint loc, std::function<MyGL_Mat4()> v ){
  glUniformMatrix4fv( loc, 1, GL_TRUE, v().f16 );
}

void mygl::UniformSetter::set( GLint loc ){
  auto visitor = [&]( auto&& v ){
    set( loc, v );
  };
  std::visit( visitor, value );
}
