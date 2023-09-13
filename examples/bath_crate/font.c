#include "font.h"

#include <mygl/public/mygl.h>
#include <mygl/public/vecdefs.h>

#include <stdio.h>


void load_font( const char name[] ){
  MyGL_Image atlas = BMP_init_image(  MyGL_str64Fmt( "assets/%s/glyphs.bmp", name ).chars );
  if( !atlas.pixels ){
    printf( "failed to load font '%s' bitmap\n", name );
    return;
  }

  MyGL_AsciiCharSet char_set;

  char_set.name = MyGL_str64( name );
  char_set.imageAtlas =  atlas;

  FILE *fp = fopen( MyGL_str64Fmt( "assets/%s/glyphs.txt", name ).chars, "r" );
  if( !fp )
    return;
  char line[256];

  for( int i = 0; i < MYGL_NUM_ASCII_PRINTABLE_CHARS; i++ ){
    char_set.chars[i].c = 0;
    char_set.chars[i].x = char_set.chars[i].y = 0.0f;
    char_set.chars[i].w = char_set.chars[i].h = 0.0f;
  }

  char_set.numChars = 0;
  while( fgets( line, sizeof(line), fp ) ){
    if( line[0] >= ' ' && line[0] <= '~' ){
      int i = char_set.numChars;
      char_set.chars[i].c = line[0];
      sscanf( &line[2], "%u %u %u %u",
              &char_set.chars[i].w, &char_set.chars[i].h,
              &char_set.chars[i].x, &char_set.chars[i].y );
      //printf( "'%c' %u x %u < %u , %u >\n",
      //        char_set.chars[i].c,
      //        char_set.chars[i].w, char_set.chars[i].h, char_set.chars[i].x, char_set.chars[i].y );
      char_set.numChars++;
    }
  }
  fclose(fp);

  MyGL_loadAsciiCharSet( &char_set, GL_TRUE, GL_TRUE );

  MyGL_imageFree( &char_set.imageAtlas );
}

/*
typedef struct{
  char c;
  float x, y, w, h;
}glyph_info_t;

glyph_info_t infos[ NUM_PRINTABLE_CHARS ];

void load_font0( const char name[] ){
  MyGL_Image atlas = BMP_init_image(  MyGL_str64Fmt( "assets/glyphs.bmp", name ).chars );
  if( !atlas.pixels ){
    printf( "failed to load font '%s' bitmap\n", name );
    return;
  }


  MyGL_createTexture2DArray( name, MyGL_roImage(atlas), NUM_PRINTABLE_CHARS, 1, "r8",
                             GL_TRUE, GL_FALSE, GL_FALSE );
  MyGL_imageFree( &atlas );

  FILE *fp = fopen( MyGL_str64Fmt( "assets/glyphs.txt", name ).chars, "r" );
  if( !fp )
    return;
  char line[256];

  for( int i = 0; i < NUM_PRINTABLE_CHARS; i++ ){
    infos[i].c = (char)( i + (int)' ' );
    infos[i].x = infos[i].y = 0.0f;
    infos[i].w = infos[i].h = 1.0f;
  }

  while( fgets( line, sizeof(line), fp ) ){
    if( line[0] >= ' ' && line[0] <= '~' ){
      int i = (int)line[0] - (int)' ';
      infos[i].c = line[0];
      sscanf( &line[2], "%f %f %f %f",
              &infos[i].w, &infos[i].h, &infos[i].x, &infos[i].y );
      printf( "'%c' %f x %f < %f , %f >\n", infos[i].c, infos[i].w, infos[i].h, infos[i].x, infos[i].y );
    }
  }

  fclose(fp);

}


size_t font_format( const char *str, MyGL_Color color, MyGL_Vec3 offset, MyGL_Vec2 scale, float spacing ){
  MyGL_VertexAttributeStream vs  = MyGL_vertexAttributeStream( "Position" );
  MyGL_VertexAttributeStream cs  = MyGL_vertexAttributeStream( "Color" );
  MyGL_VertexAttributeStream ts  = MyGL_vertexAttributeStream( "UV0" );

  float w = scale.x > 0.0f ? scale.x : 1.0f;
  float h = scale.y > 0.0f ? scale.y : 1.0f;
  int num_chars = 0;

  float x = offset.x;
  float y = offset.y;

  MyGL_Vec4 v;
  v.x = (float)color.r / 255.0f;
  v.y = (float)color.g / 255.0f;
  v.z = (float)color.b / 255.0f;
  v.w = (float)color.a / 255.0f;

  int num_verts = 0;
  for( const char *c = str; *c != '\0'; c++ ){
    float l = NUM_PRINTABLE_CHARS - ((int)(*c) - (int)' ') - 1;


    glyph_info_t *info = &infos[ (int)*c - (int)' ' ];
    float sw = w * info->w;
    float sh = h * info->h;
    vs.arr.vec4s[ num_verts + 0 ] = MyGL_vec4( x,      0.0f, y,      1.0f );
    vs.arr.vec4s[ num_verts + 1 ] = MyGL_vec4( x + sw, 0.0f, y,      1.0f );
    vs.arr.vec4s[ num_verts + 2 ] = MyGL_vec4( x + sw, 0.0f, y + sh, 1.0f );
    vs.arr.vec4s[ num_verts + 3 ] = MyGL_vec4( x ,     0.0f, y + sh, 1.0f );

    ts.arr.vec4s[ num_verts + 0 ] = MyGL_vec4( info->x,      info->y,      l, 1.0f );
    ts.arr.vec4s[ num_verts + 1 ] = MyGL_vec4( 1.0f-info->x, info->y,      l, 1.0f );
    ts.arr.vec4s[ num_verts + 2 ] = MyGL_vec4( 1.0f-info->x, 1.0f-info->y, l, 1.0f );
    ts.arr.vec4s[ num_verts + 3 ] = MyGL_vec4( info->x,      1.0f-info->y, l, 1.0f );

    cs.arr.vec4s[ num_verts + 0 ] = v;
    cs.arr.vec4s[ num_verts + 1 ] = v;
    cs.arr.vec4s[ num_verts + 2 ] = v;
    cs.arr.vec4s[ num_verts + 3 ] = v;
    x += ( sw + spacing );

    num_verts += 4;

    num_chars++;
    if( num_chars > 8000 )
      break;
  }

  return num_chars;
}

size_t font_format_fixed( const char *str, MyGL_Color color, MyGL_Vec3 offset, MyGL_Vec2 scale, float spacing ){
  MyGL_VertexAttributeStream vs  = MyGL_vertexAttributeStream( "Position" );
  MyGL_VertexAttributeStream cs  = MyGL_vertexAttributeStream( "Color" );
  MyGL_VertexAttributeStream ts  = MyGL_vertexAttributeStream( "UV0" );

  float w = scale.x > 0.0f ? scale.x : 1.0f;
  float h = scale.y > 0.0f ? scale.y : 1.0f;
  float letter_spacing = w * 0.5f * ( spacing > 0.0f ? spacing : 1.0f );
  int num_chars = 0;

  float x = offset.x;
  float y = offset.y;

  MyGL_Vec4 v;
  v.x = (float)color.r / 255.0f;
  v.y = (float)color.g / 255.0f;
  v.z = (float)color.b / 255.0f;
  v.w = (float)color.a / 255.0f;

  int num_verts = 0;
  for( const char *c = str; *c != '\0'; c++ ){
    float l = NUM_PRINTABLE_CHARS - ((int)(*c) - (int)' ') - 1;

    vs.arr.vec4s[ num_verts + 0 ] = MyGL_vec4( x,   0.0f, y,   1.0f );
    vs.arr.vec4s[ num_verts + 1 ] = MyGL_vec4( x+w, 0.0f, y,   1.0f );
    vs.arr.vec4s[ num_verts + 2 ] = MyGL_vec4( x+w, 0.0f, y+h, 1.0f );
    vs.arr.vec4s[ num_verts + 3 ] = MyGL_vec4( x ,  0.0f, y+h, 1.0f );

    ts.arr.vec4s[ num_verts + 0 ] = MyGL_vec4( 0.0f, 0.0f, l, 1.0f );
    ts.arr.vec4s[ num_verts + 1 ] = MyGL_vec4( 1.0f, 0.0f, l, 1.0f );
    ts.arr.vec4s[ num_verts + 2 ] = MyGL_vec4( 1.0f, 1.0f, l, 1.0f );
    ts.arr.vec4s[ num_verts + 3 ] = MyGL_vec4( 0.0f, 1.0f, l, 1.0f );

    cs.arr.vec4s[ num_verts + 0 ] = v;
    cs.arr.vec4s[ num_verts + 1 ] = v;
    cs.arr.vec4s[ num_verts + 2 ] = v;
    cs.arr.vec4s[ num_verts + 3 ] = v;

    x += letter_spacing;
    num_verts += 4;
    num_chars++;
    if( num_chars > 8000 )
      break;
  }

  return num_chars;
}
*/
