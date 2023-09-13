#pragma once

#include <float.h>
#include <stddef.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wunused-function"


#define PI  3.141592653589793
#define E   2.71828183
#define TOL (1e-5)


#define SWAP( a, b ){ \
  __typeof__(a) t;  \
  t=a;  \
  a=b;  \
  b=t;  \
}

#define SORT2( a, b )  \
{\
    if( (b)<(a) )\
        SWAP( a, b );\
}\


#define SORT3( a, b, c )  \
{\
    if( (c)<(b) )\
        SWAP(b,c);\
    if( (c)<(a) )\
        SWAP(a,c);\
    if( (b)<(a) )\
        SWAP(a,b);\
}\

#define BIT(n) ( 1 << (n) )
#define MIN(a,b,c){ if( (b)<=(c) ) (a)=(b); else (a)=(c); }
#define MAX(a,b,c){ if( (b)>=(c) ) (a)=(b); else (a)=(c); }
#define CLAMP(a,min,max){ if( (a) < (min) ) (a)=(min); else if( (a) > (max) ) (a)=(max); }
#define LERP(u,v,a) ( (u) + (a) * ( (v) - (u) ) )

#define SQUARE(a) ((a)*(a))

