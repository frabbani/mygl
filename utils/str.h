#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace strutils{

bool equals( const char *lhs, const char* rhs, int n = 0 ){
  if( n <= 0 )
    return 0 == std::strcmp( lhs, rhs );
  return 0 == std::strncmp( lhs, rhs, n );
}

bool equals( const std::string_view& lhs, const char* rhs, int n = 0 ){
  return equals( lhs.data(), rhs );
}

bool equals( const std::string_view& lhs, const std::string_view& rhs, int n = 0 ){
  return equals( lhs.data(), rhs.data(), n );
}

bool toUnsigned( std::string_view s, uint64_t &v ){
  v = 0;
  uint64_t mul = 1;
  for( auto c : s ){
    if( c < '0' || c > '9' )
      return false;
    v = v * mul + uint64_t( (int)c - (int)'0' );
    mul = 10;
  }
  return true;
}

bool toSigned( std::string_view s, int64_t &v ){
  v = 0;
  int64_t mul = 1;
  auto it = s.begin();
  if( *it == '-' ){
    mul *= -1;
    it++;
  }
  for( ; it != s.end(); ++it ){
    auto c = *it;
    if( c < '0' || c > '9' )
      return false;
    v = v * mul + int64_t( (int)c - (int)'0' );
    mul = 10;
  }

  return true;
}


struct Lut{
  char chars[256];
  char& operator[](int i){ return chars[i]; }

  void delimit( const char *delims ){
    for( const char *c = delims; *c != '\0'; c++ )
      chars[(uint8_t)*c] = '\0';
  }
  void upper(){
    for( char c = 'a'; c <= 'z'; c++ ){
      int v = (int)c - (int)'a' + (int)'A';
      chars[(uint8_t)c ] = (char)v;
    }
  }
  void lower(){
    for( char c = 'A'; c <= 'Z'; c++ ){
      int v = (int)c - (int)'A' + (int)'a';
      chars[(uint8_t)c ] = (char)v;
    }
  }
  Lut(){
    for( uint32_t i = 0; i < 256; i++ )
      chars[i] = (char)i;
  }
  Lut( const Lut& v ){
    memcpy( chars, v.chars, 256 );
  }
  char operator()( char c ){
    return chars[ (uint8_t)c ];
  }
};

template<int N> class Buffer{
private:
  int len;
  char chars[N];
public:
  void init( std::string_view str, std::optional<Lut> lut = std::nullopt ){
    len = 0;
    for( auto c : str ){
      chars[len++] = lut.has_value() ? (*lut)(c) : c;
      if( len == N-1 )
        break;
    }
    chars[len] = '\0';
  }
  Buffer( std::string_view str, std::optional<Lut> lut = std::nullopt ){ init( str, lut ); }

  const char *skip( int n ) const {
    n = n < 0 ? 0 : n >= len ? len-1 : n;
    return &chars[n];
  }

  const char *skipOver( int n, char c ) const {
    n = n < 0 ? 0 : n > len-1 ? len-1 : n;
    while( n < len ){
      if( chars[n] != c )
        break;
      n++;
    }
    if( n == len )
      return nullptr;

    return &chars[n];
  }

  bool beginsWith( const char *beg ) const {
    for( int i = 0; beg[i] != '\0'; i++ ){
      if( i == len )
        return false;
      if( beg[i] != chars[i] )
        return false;
    }
    return true;
  }

  int indexOf( char c, int n = 0 ) const {
    for( int i = n; i < len; i++ ){
      if( chars[i] == c )
          return i;
    }
    return -1;
  }

  int length() const { return len;  }
  const char *kChars() const { return chars;  }
  const char& operator[](int i) const { return chars[ i % N ]; }

  bool equals( std::string_view s, int n = N-1 ) const {
    return 0 == strncmp( s.data(), chars, n );
  }

  void clip( const char *sub, int n = 0 ){
    int i = 0;
    if( n <= 0 ){
      for( i = 0; i < len; i++ ){
        if( 0 == strcmp( &chars[i], sub ) ){
          chars[i] = '\0';
          len = i;
          break;
        }
      }
    }
    else{
      for( i = 0; i < len; i++ ){
        if( 0 == strncmp( &chars[i], sub, n ) ){
          chars[i] = '\0';
          len = i;
          break;
        }
      }
    }
  }

};


template<int L, int N> struct Tokenizer{

  int  count = 0;
  char str[L] = {'\0'};
  int  pos[N] = {0};


  void clear(){
    memset( str, '\0', L );
    for( int i = 0; i < N; i++ )
      pos[i] = 0;
    count = 0;
  }

  void tokenize( const char *str_, const char *delims ){
    static const Lut lutBase;

    Lut lut(lutBase); //memcpy is faster than for loop?
    lut.delimit( delims );

    clear();

    int len = 0;
    for( len = 0; str_[len] != '\0'; len++ ){
      if( len == L-1 )
        break;
      str[len] = lut[ (uint8_t)str_[len] ];
    }
    str[len] = '\0';

    int i = 0;
    char *p = str;
    if( p[0] != '\0' ){
      pos[count++] = 0;
      for( i = 1; i < len; i++ ){
        if( p[i] == '\0' )
          break;
      }
    }

    for( int j = i+1; j < len; j++ ){
      if( p[j-1] == '\0' && p[j] != '\0' )
        pos[count++] = j;
    }
  }

  std::vector<std::string_view> toVec(){
    std::vector<std::string_view> v;
    for( int i = 0; i < count; i++ )
      v.push_back( &str[ pos[i] ] );
    return v;
  }

  std::string_view operator[](int i){
    if( i < 0 || i >= count )
      return str;
    return &str[ pos[i] ];
  }

  int contains( const char *tok ){
    for( int i = 0; i < count; i++ ){
      if( 0 == strncmp( &str[ pos[i] ], tok, L ) )
        return i;
    }
    return -1;
  }


};

template< int N> struct LineFeed{
  using LineBuffer = Buffer<N>;

  std::vector<std::shared_ptr<LineBuffer>> lines;
  struct LineInfo{
    int no = -1;
    std::shared_ptr<LineBuffer> ptr = nullptr;
  };

  struct P{
    size_t n = 0;
    const char *s = nullptr;
    P( const char *s_) : n(0), s(s_){}
  };
  struct Visitor{
    std::function<char(void *)> getChar = nullptr;
    Visitor(std::function<char(void *)> getChar_ = nullptr ) : getChar(getChar_){}

    char operator()( void *p ){
      if( getChar )
        return getChar(p);
      return '\0';
    }
    char operator()( P& p ){
      char c = p.s[ p.n ];
      if( '\0' != c )
        p.n++;
      return c;
    }
  };
  Visitor v;
  std::variant<void*, P> param;
  int lineCount = 0;
  std::optional<Lut> lut;

  LineFeed( std::function<char(void *)> getChar_, void *param_, std::optional<Lut> lut_ = std::nullopt ) :
    v(getChar_), param( param_), lut(lut_) {}

  LineFeed( const char* stream, std::optional<Lut> lut_ = std::nullopt ) :
    v(nullptr), param( std::move(P(stream)) ), lut(lut_) {}

  std::optional<LineInfo> getLine(){
    std::stringstream ss;
    for( int i = 0; i < N-1; i++ ){
      char c = std::visit( v, param );
      if( c == '\0' )
        break;
      if( c == '\n' ){
        ss << ' ';  // append space if in-case line is empty
        break;
      }
      ss << c;
    }
    std::string s = ss.str();
    if( 0 == s.size())
      return std::nullopt;
    auto buffer = std::make_shared<LineBuffer>(s, lut);
    lines.push_back(buffer);
    LineInfo l = { lineCount, buffer };
    lineCount++;
    return l;
  }

};


template<typename T>
struct KVParse{
public:

  using TokenFunc = std::function<bool(std::string_view)>;
  using ValueFunc = std::function<bool(std::string_view, T&) >;

  using TokenType = std::variant < std::string, TokenFunc >;
  using ValueType = std::variant < T, ValueFunc >;

  struct Token{
  public:
    TokenType value;

    Token( std::string_view s ){
      std::string str( s.data() );
      value = std::move( str );
    }
    Token( TokenFunc func ) : value(func) {}

    bool matches( std::string_view token, std::string other ) const {
      return equals( token, other );
    }

    bool matches( std::string_view token, TokenFunc other ) const {
      return other( token );
    }

    bool matches( std::string_view token )  const {
      auto match = [&]( auto&& arg ){
        return matches( token, arg );
      };
      return std::visit(  match, value );
    }
  };

  struct Value{
  public:
    Token name;
    ValueType actual;
    Value( std::string_view nameTag, T value ) : name( nameTag ), actual(value){}
    Value( std::string_view nameTag, ValueFunc valueFunc ) : name( nameTag ), actual(valueFunc){}
    Value( TokenFunc nameFunc, ValueFunc valueFunc ) : name( nameFunc ), actual( valueFunc ){}
    Value( TokenFunc nameFunc, T value ) : name( nameFunc ), actual( value ){}

    bool resolve( T& value, std::string_view token, T& from ) {
      value = std::move( from );
      return true;
    }
    bool resolve( T& value, std::string_view token, ValueFunc& from ) { return from( token, value ); }

    bool resolve( T& out, std::string_view token ) {
      auto visit = [&]( auto&& val ){
        return resolve( out, token, val );
      };
      return std::visit( visit, actual );
    }
  };


  std::vector<Token> aliases;
  std::vector<Value> values;
  T fallback;

  KVParse( const T& fallbackValue ) : fallback( fallbackValue ) {}

  void keyAlias( std::string_view key ){
    auto a = Token( key );
    aliases.push_back( std::move(a) );
  }

  void supportValue( std::string_view name, T value ){
    auto v = Value( name, value );
    values.push_back( std::move(v) );
  }


  void supportValue( std::string_view name, ValueFunc valueFunc ){
    auto v = Value( name, valueFunc );
    values.push_back( std::move(v) );
  }


  void supportValue( TokenFunc nameFunc, T value ){
    auto v = Value( nameFunc, value );
    values.push_back( std::move(v) );
  }


  void supportValue( TokenFunc nameFunc, ValueFunc valueFunc ){
    auto v = Value( nameFunc, valueFunc );
    values.push_back( std::move(v) );
  }

  std::pair<bool, bool> parseTokens( std::string_view keyToken, std::string_view valueToken, T& output ) {
    output = fallback;

    bool found = false;
    for( const auto& alias : aliases ){
      if( alias.matches( keyToken ) ){
        found = true;
        break;
      }
    }
    if( !found )
      return { false, false };

    for( auto& value : values ){
      if( value.name.matches( valueToken ) ){
        bool pass = value.resolve( output, valueToken );
        return { true, pass };
      }
    }
    return { true, false };
  }
};

}
