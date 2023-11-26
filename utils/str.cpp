#include "str.h"

bool strutils::equals( const char *lhs, const char* rhs, int n ){
  if( n <= 0 )
    return 0 == std::strcmp( lhs, rhs );
  return 0 == std::strncmp( lhs, rhs, n );
}

bool strutils::equals( const std::string_view& lhs, const char* rhs, int n ){
  return equals( lhs.data(), rhs );
}

bool strutils::equals( const std::string_view& lhs, const std::string_view& rhs, int n){
  return equals( lhs.data(), rhs.data(), n );
}

bool strutils::toUnsigned( std::string_view s, uint64_t &v ){
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

bool strutils::toSigned( std::string_view s, int64_t &v ){
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
