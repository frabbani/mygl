#pragma once

#include "public/mygl.h"

#include "utils/stateful.h"
#include "utils/union.h"

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


//MyGL_Cull& operator = ( MyGL_Cull& lhs, const Cull& rhs ){
//  lhs = static_cast<MyGL_Cull>(rhs);
//  return lhs;
//}

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
    return std::unique_ptr<utils::StatefulState<Cull> >( new CullState() );

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

//MyGL_Depth& operator = ( MyGL_Depth& lhs, const Depth& rhs ){
//  lhs = static_cast<MyGL_Depth>(rhs);
//  return lhs;
//}

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
    return std::unique_ptr<utils::StatefulState<Depth> >( new DepthState() );

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

//MyGL_Blend& operator = ( MyGL_Blend& lhs, const Blend& rhs ){
//  lhs = static_cast<MyGL_Blend>(rhs);
//  return lhs;
//}

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
    return std::unique_ptr<utils::StatefulState<Blend> >( new BlendState() );
    //auto p = std::make_unique<BlendState>();
    //return
    //    utils::unique_move< utils::StatefulState<Blend>, BlendState >( std::move(p) );
  }
};


bool operator != (const MyGL_StencilTest& lhs, const MyGL_StencilTest& rhs ){
  return lhs.mask != rhs.mask || lhs.mode != rhs.mode || lhs.ref != rhs.ref;
}

bool operator != (const MyGL_StencilOp& lhs, const MyGL_StencilOp& rhs ){
  return
      lhs.stencilFail != rhs.stencilFail ||
      lhs.stencilPassDepthFail != rhs.stencilPassDepthFail ||
      lhs.stencilPassDepthPass != rhs.stencilPassDepthPass;
}

bool operator != (const MyGL_Stencil& lhs, const MyGL_Stencil& rhs ){
  return
      lhs.on != rhs.on ||
      // lhs.separate != rhs.separate ||
      lhs.writeMask != rhs.writeMask ||
      lhs.stencilTest != rhs.stencilTest ||
      lhs.stencilOp != rhs.stencilOp; // ||
      // lhs.stencilBackTest != rhs.stencilBackTest ||
      // lhs.stencilBackOp != rhs.stencilBackOp;
}

struct Stencil : public MyGL_Stencil {

  Stencil(){
    on = GL_FALSE;
    // separate = GL_FALSE;
    writeMask = 0xffffffff;
    stencilOp.stencilFail = stencilOp.stencilPassDepthFail = stencilOp.stencilPassDepthPass = MYGL_KEEP;
    stencilTest.ref = 0;
    stencilTest.mask = 0xffffffff;
    stencilTest.mode = MYGL_ALWAYS;
    // stencilBackOp = stencilOp;
    // stencilBackTest = stencilTest;
  }

  Stencil& operator = ( const MyGL_Stencil& rhs ){
    *(static_cast<MyGL_Stencil *>(this)) = rhs;
    return *this;
  }

  void applyOn(){
    if( on )
      glEnable ( GL_STENCIL );
    else
      glDisable( GL_STENCIL );
  }


  void applyWriteMask(){
    glStencilMask( writeMask );
  }

  void applyTest(){
    /*
    if( separate ){
      glStencilFuncSeparate( GL_FRONT, stencilTest.mode, stencilTest.ref, stencilTest.mask );
      glStencilFuncSeparate( GL_BACK, stencilBackTest.mode, stencilBackTest.ref, stencilBackTest.mask );
    }
    else */
      glStencilFunc( stencilTest.mode, stencilTest.ref, stencilTest.mask );

  }

  void applyOp(){
    /*
    if( separate ){
      glStencilOpSeparate( GL_FRONT,
                           stencilOp.stencilFail,
                           stencilOp.stencilPassDepthFail,
                           stencilOp.stencilPassDepthPass );
      glStencilOpSeparate( GL_BACK,
                           stencilBackOp.stencilFail,
                           stencilBackOp.stencilPassDepthFail,
                           stencilBackOp.stencilPassDepthPass );
    }
    else
    */
    glStencilOp( stencilOp.stencilFail,
                 stencilOp.stencilPassDepthFail,
                 stencilOp.stencilPassDepthPass );
  }

  bool operator != ( const MyGL_Stencil& rhs ){
    return *(static_cast<MyGL_Stencil*>(this)) != rhs;
  }
};

//MyGL_Stencil& operator = ( MyGL_Stencil& lhs, const Stencil& rhs ){
//  lhs = static_cast<MyGL_Stencil>(rhs);
//  return lhs;
//}

struct StencilState : public utils::StatefulState<Stencil> {

  void forceCb() override {
    current.applyOn();
    current.applyWriteMask();
    current.applyTest();
    current.applyOp();
  }

  void applyCb( const Stencil& active ) override {
    /*
    if( active.separate != current.separate ){
      forceCb();
      return;
    }
    */

    if( active.on != current.on )
      current.applyOn();
    if( active.writeMask != current.writeMask )
      current.applyWriteMask();
    if( active.stencilTest != current.stencilTest )
      current.applyTest();
    if( active.stencilOp != current.stencilOp )
      current.applyOp();
    /*
    if( active.stencilTest != current.stencilTest ||
        active.stencilBackTest != current.stencilBackTest )
      current.applyTest();
    if( active.stencilOp != current.stencilOp ||
        active.stencilBackOp != current.stencilBackOp )
      current.applyOp();
    */
  }

  static std::unique_ptr< utils::StatefulState<Stencil> > makeUnique(){
    return std::unique_ptr<utils::StatefulState<Stencil> >( new StencilState() );
  }
};


struct ColorMask : MyGL_ColorMask {

  ColorMask(){
    red = green = blue = alpha = GL_TRUE;
  }

  ColorMask& operator = ( const MyGL_ColorMask& rhs ){
    *(static_cast<MyGL_ColorMask *>(this)) = rhs;
    return *this;
  }

  void apply(){
    glColorMask( red, green, blue, alpha );
  }

  bool operator != (const MyGL_ColorMask& rhs ) const {
    return red != rhs.red ||
        green != rhs.green ||
        blue != rhs.blue ||
        alpha != rhs.alpha;
  }

};

//MyGL_ColorMask& operator = ( MyGL_ColorMask& lhs, const ColorMask& rhs ){
//  lhs = static_cast<MyGL_ColorMask>(rhs);
//  return lhs;
//}

struct ColorMaskState : public utils::StatefulState<ColorMask> {

  void forceCb() override {
    current.apply();
  }

  void applyCb( const ColorMask& active ) override {
    if( active != current )
      current.apply();
  }

  static std::unique_ptr< utils::StatefulState<ColorMask> > makeUnique(){
    return std::unique_ptr< utils::StatefulState<ColorMask> >( new ColorMaskState() );
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
    std::visit( visitor, value );
  }

};

}
