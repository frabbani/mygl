#pragma once


// Properties Definition

/**

// Cull: Back, Front, FrontAndBack/BackAndFront Off
Cull Front

Blend SrcAlpha OneMinusSrcAlpha Add, Off

// ColorWrite: Off, On, Red, Green, Blue
ColorWrite Red

// Depth/Stencil: Off Always, Never, NotEqual, Less, Greater, Equal, LEqual, GEqual, ReadOnly
Depth LEqual

// Stencil Test: Same as Depth (except for ReadOnly), Always (Default)
// Stencil Op:  Keep (Default), Zero, Replace, Incr, IncrWrap, Decr, DecrWrap, Invert
// stencil Write Mask = NoBits (all 0s), [number], AllBits (Default - all 1s)
Stencil Always WriteMask.AllBits Test.1 TestMask.AllBits StencilFail.Keep StencilPassDepthFail.Keep StencilPassDepthPass.Replace

**/


extern MyGL myGL;

namespace mygl
{

namespace shaders
{
  static constexpr int lineSize = 256;
  static constexpr int maxTokens = 16;
  using Buffer = strutils::Buffer< lineSize >;
  using LineFeed = strutils::LineFeed<lineSize>;
  using Tokenizer = strutils::Tokenizer<lineSize, maxTokens>;

  static const std::map<std::string_view, MyGL_CullMode> cullModes={
      { "Back", MYGL_BACK },
      { "Front",  MYGL_FRONT },
      { "FrontAndBack", MYGL_FRONT_AND_BACK },
      { "BackAndFront", MYGL_FRONT_AND_BACK },
  };

  MyGL_CullMode cullModeFromString( std::string_view s ){
    auto it = cullModes.find( s );
    if( it != cullModes.end() )
      return it->second;
    return MYGL_BACK;
  }

  static const std::map<std::string_view, MyGL_BlendMode> blendModes={
      { "Zero", MYGL_ZERO },
      { "One",  MYGL_ONE },
      { "SrcColor", MYGL_SRC_COLOR },
      { "OneMinusSrcColor", MYGL_ONE_MINUS_SRC_COLOR },
      { "DstColor", MYGL_DST_COLOR },
      { "OneMinusDstColor", MYGL_ONE_MINUS_DST_COLOR },
      { "SrcAlpha", MYGL_SRC_ALPHA },
      { "OneMinusSrcAlpha", MYGL_ONE_MINUS_SRC_ALPHA },
      { "DstAlpha", MYGL_DST_ALPHA },
      { "OneMinusDstAlphar", MYGL_ONE_MINUS_DST_ALPHA },
  };

  MyGL_BlendMode blendModeFromString( std::string_view s ){
    auto it = blendModes.find( s );
    if( it != blendModes.end() )
      return it->second;
    return MYGL_ZERO;
  }

  static const std::map<std::string_view, MyGL_BlendFunc> blendFuncs={
      { "Add", MYGL_FUNC_ADD },
      { "Sub",  MYGL_FUNC_SUB },
      { "RSub", MYGL_FUNC_RSUB },
      { "Min", MYGL_FUNC_MIN },
      { "Max", MYGL_FUNC_MAX },
  };

  MyGL_BlendFunc blendFuncFromString( std::string_view s ){
    auto it = blendFuncs.find( s );
    if( it != blendFuncs.end() )
      return it->second;
    return MYGL_FUNC_ADD;
  }

  static const std::map<std::string_view, MyGL_DepthMode> depthModes={
      { "Always", MYGL_ALWAYS },
      { "Never",  MYGL_NEVER },
      { "Equal",  MYGL_EQUAL },
      { "NotEqual",  MYGL_NOTEQUAL },
      { "Less", MYGL_LESS },
      { "LEqual", MYGL_LEQUAL },
      { "Greater", MYGL_GREATER },
      { "GEqual", MYGL_GEQUAL },
  };

  static const std::map<std::string_view, MyGL_StencilMode>& stencilModes = depthModes;


  MyGL_DepthMode depthModeFromString( std::string_view s ){
    auto it = depthModes.find( s );
    if( it != depthModes.end() )
      return it->second;
    return MYGL_ALWAYS;
  }

  /*
  MYGL_KEEP = GL_KEEP,
  MYGL_ZEROOUT = GL_ZERO,
  MYGL_REPLACE = GL_REPLACE,
  MYGL_INCR = GL_INCR,
  MYGL_INCR_WRAP = GL_INCR_WRAP,
  MYGL_DECR = GL_DECR,
  MYGL_DECR_WRAP = GL_DECR_WRAP,
  MYGL_INVERT = GL_INVERT,
  */
  static const std::map<std::string_view, MyGL_StencilAction> stencilActions ={
      { "Zero", MYGL_ZEROOUT },
      { "Keep",  MYGL_KEEP },
      { "Replace", MYGL_REPLACE },
      { "Incr", MYGL_INCR },
      { "IncrWrap", MYGL_INCR_WRAP },
      { "Decr", MYGL_DECR },
      { "DecrWrap", MYGL_DECR_WRAP },
      { "Invert", MYGL_INVERT },
  };

  MyGL_DepthMode stencilModeFromString( std::string_view s ){
    auto it = stencilModes.find( s );
    if( it != stencilModes.end() )
      return it->second;
    return MYGL_ALWAYS;
  }


  struct StencilParser{

    strutils::KVParse<MyGL_StencilAction> stencilFail;
    strutils::KVParse<MyGL_StencilAction> stencilPassDepthFail;
    strutils::KVParse<MyGL_StencilAction> stencilPassDepthPass;
    strutils::KVParse<GLuint> writeMask;
    strutils::KVParse<GLuint> valueMask;
    strutils::KVParse<GLint> ref;

    StencilParser(){

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
          v = GLint( i & 0xffffffff );
        return pass;
      };

      ref.supportValue( isNumber, toSignedNumber );
    }

    bool parse( std::string_view token, Stencil& stencil, std::string& error ){
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

  };


  static std::map<std::string, UniformSetter> globalUniformSetters;
  static StencilParser stencilParser;

  struct SourceCode{

    struct Lines{
      friend class SourceCode;
      struct Meta{
        std::string source = "";
        uint32_t    lineNo = 0;
      };

    protected:
      std::string source;
      std::vector<std::string> lines;
      std::vector<Meta> metas;

      Lines( const std::string& source_, const std::vector<std::string>& lines_, const std::vector<Meta>& metas_ ) :
        source(source_), lines(lines_), metas(metas_) {}

    public:
      Lines( const std::string& source_, LineFeed& feed ) :
        source(source_){
        while( true ){
          auto info = feed.getLine();
          if( !info.has_value() )
            break;
          std::string l = info.value().ptr->kChars();
          for( auto i = l.size(); i < 64; i++ )
            l += ' ';
          std::string pretty =  '\'' + source + "\'(" + std::to_string( info.value().no + 1 ) + ')';
          l += "/// ";
          l += pretty;
          lines.push_back( l );
          metas.push_back( { source, (uint32_t)info.value().no + 1 } );
        }
      }

      const std::string& sourceName() const { return source; }
      uint32_t numLines() const { return lines.size(); }
      int length( uint32_t lineNo ){
        if( lineNo >= lines.size() )
          return 0;
        return lines.size();
      }
      const char *kChars( uint32_t lineNo ) const {
        if( lineNo >= lines.size() )
          return nullptr;
        return lines[lineNo].c_str();
      }

      std::string formatted( std::string_view shader ) const {
        std::stringstream ss;
        ss << "#version 420" << std::endl;
        ss << "#define " << shader << std::endl;
        for( auto l : lines ){
          ss << l << std::endl;
        }
        return ss.str();
      }

      std::string debugFormatted( std::string_view shader ) const {
        std::stringstream ss;
        int n = 1;
        ss << "/*" + std::to_string(n++) + "*/ " + "#define " << shader << std::endl;
         for( auto l : lines ){
          ss << "/*" + std::to_string(n++) + "*/" + l << std::endl;
        }
        return ss.str();
      }

      const Meta& lineMeta( uint32_t lineNo ) const {
        lineNo = lineNo >= lines.size() ? lines.size() - 1 : lineNo;
        return metas[lineNo];
      }

      void insert( uint32_t lineNo, const Lines& other ){
        if( lineNo >= lines.size() )
          lineNo = lines.size() - 1;
        lines.insert( lines.begin() + lineNo, other.lines.begin(), other.lines.end() );
        metas.insert( metas.begin() + lineNo, other.metas.begin(), other.metas.end() );
      }

      void appendTo( std::vector<std::string>& lines_, std::vector<Meta>& metas_ ){
        lines_.insert( lines_.end(), lines.begin(), lines.end() );
        metas_.insert( metas_.end(), metas.begin(), metas.end() );
      }
    };

    static std::map<std::string, SourceCode::Lines> sharedSource;

    Lines source;
    std::string name;
    std::vector<std::string> passes;

    void parse(){
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

    SourceCode( Lines source_ ) : source( source_ ){ parse(); }

    std::optional<Lines> expand( const std::string& pass ){
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
  };


  std::map<std::string, SourceCode::Lines> SourceCode::sharedSource;


  struct ShaderPass {
    struct GlobalUniforms{

      std::unordered_map<std::string, GLint> uniforms;
      GlobalUniforms( GLint prog = -1 ){
        for( auto& [ k, v ] : globalUniformSetters ){
          auto loc = glGetUniformLocation( prog, k.data() );
          if( loc != -1 )
            uniforms[k] = loc;
        }
        for( auto u : uniforms ){
          utils::logout( " * uniform '%s' location %d", u.first.c_str(), u.second );
        }
      }

      size_t count() const {
        return uniforms.size();
      }

      void apply(){
        for( auto& [ k, v ] : uniforms ){
          auto f = globalUniformSetters.find(k);
          if( f != globalUniformSetters.end() ){
            f->second.set(v);
          }
        }
      }
    };

    struct Logger{
      enum ShaderType{ Vertex, Fragment };
      std::string formatted;
      std::vector<std::shared_ptr<Buffer>> lines;
      Logger( std::string formatted_ ) :
        formatted(std::move(formatted_)){
        LineFeed feed(formatted.c_str() );
        while( true ){
          auto info = feed.getLine();
          if( !info.has_value() )
            break;
          lines.push_back( info->ptr);
        }
      }

      void log( GLuint shader, ShaderType type ){
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
    };

    std::string source;
    std::string pass;

    std::optional< utils::Stateful<Cull> > statefulCull = std::nullopt;
    std::optional< utils::Stateful<Depth> > statefulDepth = std::nullopt;
    std::optional< utils::Stateful<Blend> > statefulBlend = std::nullopt;
    std::optional< utils::Stateful<Stencil> > statefulStencil = std::nullopt;
    std::optional< utils::Stateful<ColorMask> > statefulColorMask = std::nullopt;

    std::optional<GlobalUniforms> globalUniforms = std::nullopt;
    std::map<std::string, GLint> unifs;

    GLuint vert = -1, frag = -1, prog = -1;

    std::string programLog( GLuint program ){
      GLint len;
      std::string s = "";
      glGetProgramiv( program, GL_INFO_LOG_LENGTH , &len );
      if( len > 0 ){
        char *log = new char[ len + 1 ];
        glGetShaderInfoLog( program, len, &len, (GLchar *)log );
        log[ len ] = '\0';
        utils::logout2( "Program Log:" );
        utils::logout2( log, false );
        delete[] log;
      }
      return s;
    }

    ~ShaderPass(){
      if( glIsShader(vert) )
        glDeleteShader( vert );
      if( glIsShader(frag) )
        glDeleteShader( frag );
      if( glIsProgram( prog ) )
        glDeleteProgram( prog );
    }

    ShaderPass( const std::string& pass_, const SourceCode::Lines& lines ) : pass( pass_){
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

          int writeOnly = tokenizer.contains( "WriteOnly" );
          depth.depthWrite = writeOnly != -1 ? GL_FALSE : GL_TRUE;

          for( int i = 1; i < tokenizer.count; i++ ){
            if( strutils::equals( tokenizer[i].data(), "WriteOnly" ) )
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
      //utils::logout( "Source:\n%s", p );
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
      //if( GL_FALSE == status ){
      //  programLog( prog );
      //}

      GlobalUniforms g( prog );
      if( g.count() > 0 )
        globalUniforms = std::move( g );
    }


    void apply(){
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
  };


  struct Material{
    friend class Materials;

    std::string name;
    std::vector<std::reference_wrapper<ShaderPass>> orderedPasses;
    std::map<std::string, ShaderPass > shaderPasses;
  private:
    Material( SourceCode& sourceCode ) {
      name = sourceCode.name;
      for( auto pass : sourceCode.passes ){
        auto subLines = sourceCode.expand( pass );
        shaderPasses.try_emplace( pass, pass, subLines.value() );
        auto it = shaderPasses.find( pass );
        if( it != shaderPasses.end() )
          orderedPasses.push_back( std::ref(it->second) );
      }
    }
  public:
    uint32_t numPasses(){ return orderedPasses.size(); }

    void apply( uint32_t passNo ){
      if( passNo >= orderedPasses.size() )
        return;
      orderedPasses[passNo].get().apply();
    }

  };

  struct Materials{
    static std::map<std::string, Material> materials;
    static void add( SourceCode& sourceCode ){
      Material m( sourceCode );
      materials.insert( std::make_pair( m.name, std::move(m) ) );
    }

    static std::optional<std::reference_wrapper<Material>> get( std::string_view name ){
      std::string s(name);
      auto f = materials.find( s );
      if( f != materials.end() )
        return f->second;
      return std::nullopt;
    }
  };

  std::map<std::string, Material> Materials::materials;
}

std::string_view stencilModeToString( MyGL_StencilMode mode ){
  for( auto [ k, v ] : shaders::stencilModes ){
    if( v == mode )
      return k;
  }
  return "???";
}

std::string_view stencilActionToString( MyGL_StencilAction action ){
  for( auto [ k, v ] : shaders::stencilActions ){
    if( v == action )
      return k;
  }
  return "???";
}

}

