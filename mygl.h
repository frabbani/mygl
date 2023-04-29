#pragma once

#include "public/mygl.h"

#include "utils/stateful.h"

namespace mygl
{

struct Cull : MyGL_Cull {
  Cull(){
    on = GL_FALSE;
    cullMode = MYGL_BACK;
    frontIsCCW = GL_TRUE;
  }

  Cull& operator = ( const MyGL_Cull& rhs ){
    *(static_cast<MyGL_Cull *>(this)) = rhs;
    return *this;
  }

  void applyOn(){
    if( on )
      glEnable ( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );
  }

  void applyCullMode(){ glCullFace( cullMode ); }

  void applyFrontFaceIsCCW(){
    if( frontIsCCW )
      glFrontFace( GL_CCW );
    else
      glFrontFace( GL_CW  );
  }

  bool operator != (const Cull& rhs ) const {
    return on != rhs.on || cullMode != rhs.cullMode || frontIsCCW != rhs.frontIsCCW;
  }
};

struct CullState : public utils::StatefulState<Cull> {

  void forceCb() override {
    current.applyOn();
    current.applyFrontFaceIsCCW();
    current.applyCullMode();
  }

  void applyCb( const Cull& active ) override {
    if( active.on != current.on )
      current.applyOn();
    if( active.frontIsCCW != current.frontIsCCW )
      current.applyFrontFaceIsCCW();
    if( active.cullMode != current.cullMode )
      current.applyCullMode();
  }

  static std::unique_ptr< utils::StatefulState<Cull> > makeUnique(){
    auto p = std::make_unique<CullState>();
    return
        utils::unique_move< utils::StatefulState<Cull>, CullState >( std::move(p) );
  }
};


struct Depth : MyGL_Depth {
  Depth(){
    on = GL_FALSE;
    depthWrite = GL_TRUE;
    depthMode = MYGL_LESS;
  }
  Depth& operator = ( const MyGL_Depth& rhs ){
    *(static_cast<MyGL_Depth *>(this)) = rhs;
    return *this;
  }

  void applyOn(){
    if( on )
      glEnable( GL_DEPTH_TEST );
    else
      glDisable( GL_DEPTH_TEST );
  }

  void applyDepthWrite(){
    if( depthWrite )
      glDepthMask( GL_TRUE );
    else
      glDepthMask( GL_FALSE );
  }

  void applyDepthMode(){
    glDepthFunc( depthMode );
  }

  bool operator != (const Depth& rhs ) const {
    return on != rhs.on || depthWrite != rhs.depthWrite || depthMode != rhs.depthMode;
  }
};

struct DepthState : public utils::StatefulState<Depth> {
  void forceCb() override {
    current.applyOn();
    current.applyDepthWrite();
    current.applyDepthMode();
  }

  void applyCb( const Depth& active ) override {
    if( active.on != current.on )
      current.applyOn();
    if( active.depthWrite != current.depthWrite )
      current.applyDepthWrite();
    if( active.depthMode != current.depthMode )
      current.applyDepthMode();
  }

  static std::unique_ptr< utils::StatefulState<Depth> > makeUnique(){
    auto p = std::make_unique<DepthState>();
    return
        utils::unique_move< utils::StatefulState<Depth>, DepthState >( std::move(p) );
  }
};


struct Blend : MyGL_Blend {

  Blend(){
    on = GL_FALSE;
    blendOp.src = MYGL_SRC_ALPHA;
    blendOp.dst = MYGL_ONE_MINUS_SRC_ALPHA;
    blendOp.func = MYGL_FUNC_ADD;
  }

  Blend& operator = ( const MyGL_Blend& rhs ){
    *(static_cast<MyGL_Blend *>(this)) = rhs;
    return *this;
  }

  void applyOn(){
    if( on )
      glEnable ( GL_BLEND );
    else
      glDisable( GL_BLEND );
  }

  void applyBlendOpSrcDst(){
    glBlendFunc( blendOp.src, blendOp.dst );
  }

  void applyBlendOpFunc(){
    glBlendEquation( blendOp.func );
  }

  bool operator != (const MyGL_Blend& rhs ) const {
    return on != rhs.on ||
        blendOp.src != rhs.blendOp.src || blendOp.dst != rhs.blendOp.dst ||
        blendOp.func != rhs.blendOp.func;
  }

};

struct BlendState : public utils::StatefulState<Blend> {
  void forceCb() override {
    current.applyOn();
    current.applyBlendOpSrcDst();
    current.applyBlendOpFunc();
  }

  void applyCb( const Blend& active ) override {
    if( active.on != current.on )
      current.applyOn();
    if( active.blendOp.src != current.blendOp.src ||
        active.blendOp.dst != current.blendOp.dst )
      current.applyBlendOpSrcDst();
    if( active.blendOp.func != current.blendOp.func )
      current.applyBlendOpFunc();
  }

  static std::unique_ptr< utils::StatefulState<Blend> > makeUnique(){
    auto p = std::make_unique<BlendState>();
    return
        utils::unique_move< utils::StatefulState<Blend>, BlendState >( std::move(p) );
  }
};


struct UniformSetter{
  using Variant =
      std::variant< const float*, const MyGL_Vec2*, const MyGL_Vec3*, const MyGL_Vec4*, const MyGL_Mat4*, std::function<MyGL_Mat4()> >;

  void set_( const float *v,     GLint loc ){
    glUniform1f( loc, *v );
  }
  void set_( const MyGL_Vec2 *v, GLint loc ){
    glUniform2fv( loc, 1, v->f2 );
  }
  void set_( const MyGL_Vec3* v, GLint loc ){
    glUniform3fv( loc, 1, v->f3 );
  }
  void set_( const MyGL_Vec4* v, GLint loc ){
    glUniform4fv( loc, 1, v->f4 );
  }
  void set_( const MyGL_Mat4* v, GLint loc ){
    glUniformMatrix4fv( loc, 1, GL_TRUE, v->f16 );
  }
  void set_( std::function<MyGL_Mat4()> v, GLint loc ){
    glUniformMatrix4fv( loc, 1, GL_TRUE, v().f16 );
  }

  Variant value;

  UniformSetter( const float *value_ ) : value(value_) {}
  UniformSetter( const MyGL_Vec2 *value_ ) : value( value_) {}
  UniformSetter( const MyGL_Vec3 *value_ ) : value( value_) {}
  UniformSetter( const MyGL_Vec4 *value_ ) : value( value_) {}
  UniformSetter( const MyGL_Mat4 *value_ ) : value( value_) {}
  UniformSetter( std::function<MyGL_Mat4()> value_ ) : value( value_) {}

  void set( GLint loc ){

    auto visitor = [&]( auto p ){
      set_( p, loc );
    };
    //std::string s(typeid(value.get()).name());
    //utils::logout( "FXR %s: type %s\n", __FUNCTION__, s.c_str() );
    std::visit( visitor, value );

  }
};

}
