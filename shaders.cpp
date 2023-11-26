#include "shaders.h"

using namespace mygl;

extern GLboolean chatty;

MyGL_CullMode shaders::cullModeFromString( std::string_view s ){
  auto it = cullModes.find( s );
  if( it != cullModes.end() )
    return it->second;
  return MYGL_BACK;
}

MyGL_BlendMode shaders::blendModeFromString( std::string_view s ){
  auto it = blendModes.find( s );
  if( it != blendModes.end() )
    return it->second;
  return MYGL_ZERO;
}


MyGL_BlendFunc shaders::blendFuncFromString( std::string_view s ){
  auto it = blendFuncs.find( s );
  if( it != blendFuncs.end() )
    return it->second;
  return MYGL_FUNC_ADD;
}


MyGL_DepthMode shaders::depthModeFromString( std::string_view s ){
  auto it = depthModes.find( s );
  if( it != depthModes.end() )
    return it->second;
  return MYGL_ALWAYS;
}


MyGL_DepthMode shaders::stencilModeFromString( std::string_view s ){
  auto it = stencilModes.find( s );
  if( it != stencilModes.end() )
    return it->second;
  return MYGL_ALWAYS;
}

std::string_view mygl::stencilModeToString( MyGL_StencilMode mode ){
  for( auto [ k, v ] : shaders::stencilModes ){
    if( v == mode )
      return k;
  }
  return "???";
}

std::string_view mygl::stencilActionToString( MyGL_StencilAction action ){
  for( auto [ k, v ] : shaders::stencilActions ){
    if( v == action )
      return k;
  }
  return "???";
}


shaders::StencilParser::StencilParser(){
  stencilFail.keyAlias( "StencilFail" );
  stencilFail.keyAlias( "Fail" );
  stencilFail.supportValue( "Keep", MYGL_KEEP );
  stencilFail.supportValue( "Zero", MYGL_ZEROOUT );
  stencilFail.supportValue( "Replace", MYGL_REPLACE );
  stencilFail.supportValue( "Incr", MYGL_INCR );
  stencilFail.supportValue( "IncrWrap", MYGL_INCR_WRAP );
  stencilFail.supportValue( "Decr", MYGL_DECR );
  stencilFail.supportValue( "DecrWrap", MYGL_DECR_WRAP );
  stencilFail.supportValue( "Invert", MYGL_INVERT );

  stencilPassDepthFail.keyAlias( "StencilPassDepthFail" );
  stencilPassDepthFail.keyAlias( "DepthFail" );
  stencilPassDepthFail.values = stencilFail.values;

  stencilPassDepthPass.keyAlias( "StencilPassDepthPass" );
  stencilPassDepthPass.keyAlias( "Pass" );
  stencilPassDepthPass.values = stencilFail.values;


  auto isNumber = [=]( std::string_view str ){
    for( auto c : str )
      if( !std::isdigit(c) )
        return false;
    return true;
  };

  auto toNumber = [=]( std::string_view str, GLuint& v ) -> bool {
    uint64_t u;
    bool pass = strutils::toUnsigned( str,  u );
    if( pass )
      v = GLuint( u & 0xffffffff );
    return pass;
  };

  writeMask.keyAlias( "WriteMask" );
  writeMask.supportValue( "NoBits", 0 );
  writeMask.supportValue( "AllBits", 0xffffffff );
  writeMask.supportValue( isNumber, toNumber );

  valueMask.keyAlias( "TestMask" );
  valueMask.keyAlias( "ValueMask" );
  valueMask.values = writeMask.values;

  ref.keyAlias( "Test" );
  ref.keyAlias( "Value" );
  ref.keyAlias( "Ref" );

  auto toSignedNumber = [=]( std::string_view str, GLint& v ) -> bool {
    int64_t i;
    bool pass = strutils::toSigned( str,  i );
    if( pass )
      v = GLint( i );
    return pass;
  };

  ref.supportValue( isNumber, toSignedNumber );
}

bool shaders::StencilParser::parse( std::string_view token, Stencil& stencil, std::string& error ){
  std::stringstream ss;
  ss << "stencil error - ";
  if( strutils::equals( token.data(), "Off" ) ){
    stencil.on = GL_FALSE;
    return true;
  }
  auto it = stencilModes.find( token );
  if( it != stencilModes.end() ){
    stencil.stencilTest.mode = it->second;
    return true;
  }
  strutils::Tokenizer<32,2> tokenizer;
  tokenizer.tokenize( token.data(), "-.:_" );
  if( tokenizer.count != 2 ){
    ss << " invalid key-value pair: " << token;
    ss >> error;
    return false;
  }

  std::pair<bool, bool> r;
  r = writeMask.parseTokens(tokenizer[0], tokenizer[1], stencil.writeMask );
  if( r.first ){
    if( !r.second ){
      ss << "invalid write mask: " << std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }
    return true;
  }

  r = valueMask.parseTokens(tokenizer[0], tokenizer[1], stencil.stencilTest.mask );
  if( r.first ){
    if( !r.second ){
      ss << "invalid ref mask: " <<  std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }
    return true;
  }

  r = ref.parseTokens( tokenizer[0], tokenizer[1], stencil.stencilTest.ref );
  if( r.first ){
    if( !r.second ){
      ss << "invalid ref value: " << std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }

    return true;
  }

  r = stencilFail.parseTokens(tokenizer[0], tokenizer[1], stencil.stencilOp.stencilFail );
  if( r.first ){
    if( !r.second ){
      ss << "invalid stencil-fail value: " << std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }
    return true;
  }

  r = stencilPassDepthFail.parseTokens(tokenizer[0], tokenizer[1], stencil.stencilOp.stencilPassDepthFail );
  if( r.first ){
    if( !r.second ){
      ss << "invalid stencil-pass/depth-fail value: " << std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }
    return true;
  }

  r = stencilPassDepthPass.parseTokens(tokenizer[0], tokenizer[1], stencil.stencilOp.stencilPassDepthPass );
  if( r.first ){
    if( !r.second ){
      ss << "invalid stencil-pass/depth-pass value: " << std::string_view( tokenizer[1] );
      ss >> error;
      return false;
    }
    return true;
  }
  ss << "unknown token: " << token;
  ss >> error;
  return false;
}

shaders::StencilParser shaders::stencilParser;

void shaders::SourceCode::parse(){
  Tokenizer tokenizer;
  Buffer line( source.kChars(0) );

  if( !line.equals( "#if 0", 5 ) ){
    utils::logout( "error: shader source missing \"#if 0\" block" );
    return;
  }
  auto withinQuotes = []( const Buffer& line ) -> std::string {
    char token[ lineSize ];
    int a = line.indexOf( '\"' );
    if( -1 == a )
      return "";
    int b = line.indexOf( '\"', a+1 );
    if( -1 == b )
      return "";
    int n = 0;
    for( int i = a+1; i < b; i++ ){
      token[n++] = line[i];
    }
    token[n] = '\0';
    return std::string( token );
  };

  for( uint32_t i = 0; i < source.numLines(); i++ ){
    Buffer line( source.kChars(i) );
    if( line.equals("#endif", 6 ) )
      break;
    if( line.beginsWith( "Name" ) ){
      name = withinQuotes( line );
    }
    if( line.beginsWith( "Passes" ) ){
      Tokenizer tokenizer;
      tokenizer.tokenize( withinQuotes( line ).c_str(), " ,\t" );
      for( auto j = 0; j < tokenizer.count; j++ ){
        passes.push_back( tokenizer[j].data() );
      }
    }
  }
  utils::logout( "shader source name: '%s'", name.c_str() );
  for( auto s : passes ){
    utils::logout( " * pass '%s'", s.c_str() );
  }
}

std::optional<shaders::SourceCode::Lines> shaders::SourceCode::expand( const std::string& pass ){
  bool found = false;
  for( auto p : passes ){
    if( p == pass ){
      found = true;
      break;
    }
  }
  if( !found ){
    return std::nullopt;
  }

  auto withinQuotes = []( const Buffer& line ) -> std::string {
    char token[ lineSize ];
    int a = line.indexOf( '\"' );
    if( -1 == a )
      return "";
    int b = line.indexOf( '\"', a+1 );
    if( -1 == b )
      return "";
    int n = 0;
    for( int i = a+1; i < b; i++ ){
      token[n++] = line[i];
    }
    token[n] = '\0';
    return std::string( token );
  };

  std::vector<std::string> lines;
  std::vector<SourceCode::Lines::Meta> metas;
  Tokenizer tokenizer;

  int stack = 0;
  int numLines = 0;
  std::string also = pass + "{";
  for( size_t i = 0; i < source.lines.size(); i++ ){
    auto line = source.lines[i];
    auto meta = source.metas[i];
    if( 0 == stack ){
      tokenizer.tokenize( line.c_str(), " \t" );
      if( tokenizer.count >= 2 ){
        if( 0 == std::strcmp( tokenizer[0].data(), pass.c_str() ) &&
            0 == std::strcmp( tokenizer[1].data(), "{" ) ){
          stack++;
          continue;
        }
      }
      else if( tokenizer.count >= 1 ){
        if( 0 == std::strcmp( tokenizer[0].data(), also.c_str() ) ){
          stack++;
          continue;
        }
      }
    }

    if( stack > 0 ){
      Buffer l( line.c_str() );
      //l.clip( "//", 2 );
      if( std::strchr( l.kChars(), '{' ) )
        stack++;
      else if( std::strchr( l.kChars(), '}' ) )
        stack--;
      if( 0 == stack )
        break;

      if( l.beginsWith("#include") ){
        tokenizer.tokenize( l.kChars(), " \t" );
        std::string lib = tokenizer.count >= 2 ? withinQuotes( tokenizer[1] ) : "?????????";
        auto it = sharedSource.find( lib );
        if( it != sharedSource.end() ){
          it->second.appendTo( lines,  metas );
          numLines += it->second.numLines();
          continue;
        }
      }

      numLines++;
      lines.push_back( line );
      metas.push_back( meta );
    }
  }
  if( 0 == numLines )
    return std::nullopt;

  return Lines( source.source, lines, metas );
}

std::map<std::string, shaders::SourceCode::Lines> shaders::SourceCode::sharedSource;
std::map<std::string, UniformSetter> shaders::globalUniformSetters;

shaders::ShaderPass::GlobalUniforms::GlobalUniforms( GLint prog ){
  for( auto& [ k, v ] : globalUniformSetters ){
    auto loc = glGetUniformLocation( prog, k.data() );
    if( loc != -1 )
      uniformLocs[k] = loc;
  }
  utils::logout( " * Global uniforms:" );
  for( auto u : uniformLocs ){
    utils::logout( "    + uniform '%s' location %d", u.first.c_str(), u.second );
  }
}

void shaders::ShaderPass::Logger::log( GLuint shader, ShaderType type ){
  auto lineNo = [=]( const char *log ){
    // error messages have different formats based on the type of GPU
    // nvidia gpus use x(y), and amd gpus use ERRORD:x:y, either case,
    // find the second number (i think)
    int i, n;
    char num[32];
    n = 0;
    for( i = 0; i < 64; i++ ){
      if( std::isdigit(log[i]) )
        num[n++] = log[i];
      else if( n > 0 )
        break;
    }
    n = 0;  // discard, start again
    for( ; i < 64; i++ ){
      if( std::isdigit(log[i]) )
        num[n++] = log[i];
      else if( n > 0 )
        break;
    }
    num[n] = '\0';
    return (int)std::atol(num) - 1;
  };

  utils::logout( "%s Shader Log:", type == ShaderType::Vertex ? "Vertex" : "Fragment" );

  GLint len;
  glGetShaderiv( shader, GL_INFO_LOG_LENGTH , &len );
  if( len > 0 ){
    char *log = new char[ len + 1 ];
    glGetShaderInfoLog( shader, len, &len, (GLchar *)log );
    int no = lineNo(log);
    utils::logout2( log, false );
    int x = no - 5;
    x = x < 0 ? 0 : x;
    for( int i = x; i <= no; i++ )
      utils::logout( " * '%s'", lines[i]->kChars() );
    delete[] log;

  }
}

shaders::ShaderPass::ShaderPass( const std::string& pass_, const SourceCode::Lines& lines ) : pass( pass_){
  source = lines.sourceName();
  Tokenizer tokenizer;
  bool inside = false;
  GLint status;

  for( uint32_t i = 0; i < lines.numLines(); i++ ){
    Buffer b( lines.kChars(i) );
    //b.clip( "//",  2 );
    //utils::logout( "'%s'", b.kChars() );
    tokenizer.tokenize(b.kChars(), " ,\t" );
    if( strutils::equals( tokenizer[0].data(), "/**", 3 ) ){
      inside = true;
      continue;
    }
    if( !inside )
      continue;

    if( strutils::equals( tokenizer[0].data(), "**/", 3 ) )
      break;

    auto off = tokenizer.contains( "Off" ) != -1;

    if( strutils::equals( tokenizer[0].data(), "Cull" ) ){
      statefulCull = utils::Stateful<Cull>( CullState::makeUnique() );
      Cull& cull = statefulCull.value().current();
      cull.on = off ? GL_FALSE : GL_TRUE;
      if( tokenizer.count >= 2 )
        cull.cullMode = cullModeFromString( tokenizer[1] );
    }

    if( strutils::equals( tokenizer[0].data(), "Blend" ) ){
      statefulBlend = utils::Stateful<Blend>( BlendState::makeUnique() );
      Blend& blend = statefulBlend.value().current();
      blend.on = off ? GL_FALSE : GL_TRUE;
      if( tokenizer.count >= 4 ){
        blend.blendOp.src  = blendModeFromString( tokenizer[1] );
        blend.blendOp.dst  = blendModeFromString( tokenizer[2] );
        blend.blendOp.func = blendFuncFromString( tokenizer[3] );
      }
      continue;
    }

    if( strutils::equals( tokenizer[0].data(), "Depth" )  ){
      statefulDepth = utils::Stateful<Depth>( DepthState::makeUnique() );
      Depth& depth = statefulDepth.value().current();
      depth.on = off ? GL_FALSE : GL_TRUE;

      int readOnly = tokenizer.contains( "ReadOnly" );
      depth.depthWrite = readOnly < 0 ? GL_TRUE : GL_FALSE;

      for( int i = 1; i < tokenizer.count; i++ ){
        if( strutils::equals( tokenizer[i].data(), "ReadOnly" ) )
          continue;
        if( strutils::equals( tokenizer[i].data(), "Off" ) )
          continue;
        depth.depthMode = depthModeFromString( tokenizer[i] );
        break;
      }
      continue;
    }

    if( strutils::equals( tokenizer[0].data(), "Stencil" )  ){
      statefulStencil = utils::Stateful<Stencil>( StencilState::makeUnique() );
      Stencil& stencil = statefulStencil.value().current();
      stencil.on = off? GL_FALSE : GL_TRUE;
      std::string error;
      for( int i = 1; i < tokenizer.count; i++ ){
        if( strutils::equals( tokenizer[i], "///", 3) )
          break;
        if( !stencilParser.parse( tokenizer[i], stencil, error ) ){
          utils::logout( "Shader '%s' Pass '%s: %s", source.c_str(), pass.c_str(), error.c_str() );
        }
      }
    }

    if( strutils::equals( tokenizer[0].data(), "ColorWrite" )  ){
      statefulColorMask = utils::Stateful<ColorMask>( ColorMaskState::makeUnique() );
      auto& colorMask = statefulColorMask.value().current();
      colorMask = { GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE };
      if( !off )
        for( int i = 1; i < tokenizer.count; i++ ){
          if( strutils::equals( tokenizer[i].data(), "Red" ) )
            colorMask.red = GL_TRUE;
          if( strutils::equals( tokenizer[i].data(), "Green" ) )
            colorMask.green = GL_TRUE;
          if( strutils::equals( tokenizer[i].data(), "Blue" ) )
            colorMask.blue = GL_TRUE;
          if( strutils::equals( tokenizer[i].data(), "Alpha" ) )
            colorMask.alpha = GL_TRUE;
          if( strutils::equals( tokenizer[i].data(), "On" ) ){
            colorMask = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
            break;
          }
        }
    }

  }

  vert = glCreateShader( GL_VERTEX_SHADER );
  frag = glCreateShader( GL_FRAGMENT_SHADER );
  prog = glCreateProgram();
  utils::logout( "Shader '%s' Pass '%s' create status: vert=%u  frag=%u  prog=%u",
                 source.c_str(), pass.c_str(),
                 vert, frag, prog );

  std::string s = lines.formatted( "__vert__" );
  const char *p = s.c_str();
  if( chatty )
    utils::logout( "Source:\n%s", p );
  glShaderSource( vert, 1, &p, NULL );
  // glGetShaderiv ( vert, GL_SHADER_SOURCE_LENGTH, &status );
  // utils::logout( "vertex shader source length: %d", status );
  glCompileShader( vert );
  glGetShaderiv( vert, GL_COMPILE_STATUS, &status );
  utils::logout( "Vertex shader compile status: %d", status );
  if( GL_FALSE == status ){
    Logger logger(s);
    logger.log( vert, Logger::ShaderType::Vertex );
  }

  s = lines.formatted( "__frag__" );
  p = s.c_str();
  glShaderSource( frag, 1, &p, NULL );
  glCompileShader( frag );
  glGetShaderiv( frag, GL_COMPILE_STATUS, &status );
  utils::logout( "Fragment shader compile status: %d", status );
  if( GL_FALSE == status ){
    Logger logger(s);
    logger.log( frag, Logger::ShaderType::Fragment );
  }

  glAttachShader( prog, vert );
  glAttachShader( prog, frag );
  glLinkProgram ( prog );
  glGetProgramiv( prog, GL_LINK_STATUS , &status );
  if( GL_FALSE == status )
    programLog( prog );

  GLint numUniforms;
  glGetProgramiv( prog, GL_ACTIVE_UNIFORMS, &numUniforms );
  utils::logout( "%d Active uniforms", numUniforms );
  for( GLint i = 0; i < numUniforms; i++ ){
    Uniform uniform;
    char name[256];
    GLint size;
    GLenum type;
    glGetActiveUniform( prog, i, sizeof(name), nullptr, &size, &type, name );
    if( std::strncmp( "mygl.", name, 5 ) == 0 )
      continue;
    bool valid = false;
    switch( type ){
      case GL_FLOAT:
      case GL_FLOAT_VEC2:
      case GL_FLOAT_VEC3:
      case GL_FLOAT_VEC4:
      case GL_FLOAT_MAT2:
      case GL_FLOAT_MAT3:
      case GL_FLOAT_MAT4:
      case GL_INT:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_INT_VEC4:
      case GL_UNSIGNED_INT:
      case GL_UNSIGNED_INT_VEC2:
      case GL_UNSIGNED_INT_VEC3:
      case GL_UNSIGNED_INT_VEC4: valid = true; break;
      default: break;
    }
    if( !valid )
      continue;
    uniform.type = MyGL_UniformType( type );
    if( 1 == size ){
      uniform.name = name;
      uniform.loc  = glGetUniformLocation( prog, uniform.name.c_str() );
      utils::logout( " %d) '%s' size=%d type=%d", i, uniform.name.c_str(), size, type );
      uniforms[ uniform.name ] = std::make_shared<Uniform>(uniform);
    }
    else{
      *strchr(name, '[') = '\0';
      for( GLint j = 0; j < size; j++ ){
        std::stringstream ss;
        ss << name << '[' << j << ']';
        uniform.name = ss.str();
        uniform.loc  = glGetUniformLocation( prog, uniform.name.c_str() );
        utils::logout( " %d) '%s' size=%d type=%d", i, uniform.name.c_str(), size, type );
        uniforms[ uniform.name ] = std::make_shared<Uniform>(uniform);
      }
    }
  }
  GlobalUniforms g( prog );
  if( g.count() > 0 )
    globalUniforms = std::move( g );
}

void shaders::ShaderPass::apply(){
  if( (GLuint)-1 == prog )
    return;

  glUseProgram( prog );

  if( statefulCull.has_value() )
    statefulCull.value().apply();
  else
    MyGL_applyCull();

  if( statefulBlend.has_value() )
    statefulBlend.value().apply();
  else
    MyGL_applyBlend();

  if( statefulDepth.has_value() )
    statefulDepth.value().apply();
  else
    MyGL_applyDepth();

  if( statefulStencil.has_value() )
    statefulStencil.value().apply();
  else
    MyGL_applyStencil();

  if( statefulColorMask.has_value() )
    statefulColorMask.value().apply();
  else
    MyGL_applyColorMask();

  if( globalUniforms.has_value())
    globalUniforms.value().apply();
}


std::map<std::string, shaders::Material> shaders::Materials::materials;
