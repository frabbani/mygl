#pragma once

#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <functional>
#include <optional>
#include <variant>

#include "public/mygl.h"

namespace utils{

MyGL_LogFunc logFunc = NULL;

void logout( const char *format, ... ){
  if( !logFunc )
    return;
  char str[ 64 * 1024 ], out[ 64 * 1024 ];
  va_list args;
  __builtin_va_start( args,format );
  vsprintf( str, format, args );
  __builtin_va_end(args);
  sprintf( out, "[MYGL] %s\n", str );
  logFunc( out );
}

void logout2( const char *str, bool newline = true ){
  if( !logFunc )
    return;
  char out[ 64 * 1024 ];
  sprintf( out, "[MYGL] %s%c", str, newline ? '\n' : '\0' );
  logFunc( out );
}

void logoutNoNewLine( const char *format, ... ){
  if( !logFunc )
    return;
  char str[ 64 * 1024 ], out[ 64 * 1024 ];
  va_list args;
  __builtin_va_start( args,format );
  vsprintf( str, format, args );
  __builtin_va_end(args);
  sprintf( out, "[MYGL] %s", str );
  logFunc( out );
}

}

namespace strutils{

bool equals( const char *str, const char* str2, int n = 0 ){
  if( n <= 0 )
    return 0 == strcmp( str, str2 );

  return 0 == strncmp( str, str2, n );
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
    int n = 0;
    const char *s = nullptr;
    P( const char *s_) : n(0), s(s_){}
  };
  struct V{
    std::function<char(void *)> getChar = nullptr;
    V(std::function<char(void *)> getChar_ = nullptr ) : getChar(getChar_){}

    char operator()( void *p ){
      if( getChar )
        return getChar(p);
      return '\0';
    }
    char operator()( P& p ){
      char c = p.s[p.n];
      if( '\0' != c )
        p.n++;
      return c;
    }
  };
  V v;
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

}




