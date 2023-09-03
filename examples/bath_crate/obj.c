#include "obj.h"

#include "libs.h"

/*
ARRAYFUNC( vec3, vec3_t );
ARRAYFUNC( vec2, vec2_t );

vec3_t vec3( const float3 f3 ){
  vec3_t v;
  v.x = f3[0];
  v.y = f3[1];
  v.z = f3[2];
  return v;
}

vec2_t vec2( const float2 f2 ){
  vec2_t v;
  v.x = f2[0];
  v.y = f2[1];
  return v;
}
*/

/*
#define WaveFront_VEC2 0
#define WaveFront_VEC3 1
#define WaveFront_FACE 2


void WaveFront_list_init( WaveFront_list_t *list, int size, int type ){
  size = size < 16 ? 16 : size;
  list->elems = malloc( size * sizeof(WaveFront_list_elem_t) );
  list->allocated = size * sizeof(WaveFront_list_elem_t);
  list->type = type;
}

void WaveFront_list_term( WaveFront_list_t* list ){
  if( list->elems ){
    free( list->elems );
    list->elems = NULL;
  }
  list->count = list->allocated = 0;
}


void WaveFront_list_new( WaveFront_list_t* list ){
  int pitch = 0;
  switch( list->type ){
    case( WaveFront_VEC2 ): pitch = sizeof(WaveFront_vec2_t); break;
    case( WaveFront_VEC3 ): pitch = sizeof(WaveFront_vec3_t); break;
    case( WaveFront_FACE ):
    default: pitch = sizeof(WaveFront_face_t); break;
  }

  int new_size = ( list->count + 1 ) * pitch;
  if( new_size >= list->allocated ){
    void *new_data = malloc( list->allocated * 2 );
    memcpy( new_data, list->elems, list->count * pitch );
    free( list->elems );
    list->elems = new_data;
    list->allocated *= 2;
  }
  list->count++;
}

void *WaveFront_list_get( WaveFront_list_t* list, int index ){
  if( index < 0 || index >= list->count )
    return NULL;

  switch( list->type ){
    case( WaveFront_VEC2 ):
      return &list->elems->v2s[index]; break;
    case( WaveFront_VEC3 ):
        return &list->elems->v3s[index]; break;
    case( WaveFront_FACE ):
      return &list->elems->fs[index]; break;
    default: break;
  }
  return NULL;
}

void *WaveFront_list_last( WaveFront_list_t* list){
  if( !list->count )
    return NULL;
  int index = list->count - 1;
  switch( list->type ){
    case( WaveFront_VEC2 ):
      return &list->elems->v2s[index]; break;
    case( WaveFront_VEC3 ):
        return &list->elems->v3s[index]; break;
    case( WaveFront_FACE ):
      return &list->elems->fs[index]; break;
    default: break;
  }
  return NULL;
}
*/

void WaveFront_obj_term( WaveFront_obj_t *obj ){
  if( obj->verts )
    free( obj->verts );
  if( obj->uvs )
    free( obj->uvs );
  if( obj->norms )
    free( obj->norms );
  if( obj->faces )
    free( obj->faces );
  memset( obj, 0, sizeof(WaveFront_obj_t) );
}

static void parse_indices( const char *tok, int *v, int *t, int *n ){
  char str[64];
  char *tptr = NULL;
  char *nptr = NULL;
  int i;

  *v = *t = *n = -1;

  strcpy( str, tok );
  for( i = 0; str[i] != '\0'; i++ ){
    if( '/' == str[i] ){
      str[i] = '\0';
      tptr = &str[i+1];
      break;
    }
  }
  for( ; str[i] != '\0'; i++ ){
    if( '/' == str[i] ){
      str[i] = '\0';
      nptr = &str[i+1];
      break;
    }
  }

  *v = atoi( str ) - 1;
  if( tptr && *tptr != '\0' )
    *t = atoi( tptr ) - 1;
  if( nptr && *nptr != '\0' )
    *n = atoi( nptr ) - 1;
}


int WaveFront_obj_load( WaveFront_obj_t *obj, const char objfile[], float scale, int term ){
  if( term )
    WaveFront_obj_term( obj );
  else
    memset( obj, 0, sizeof(WaveFront_obj_t) );

  if( scale <= 0.0f )
    scale = 1.0f;

  FILE *fp = NULL;
  char line[256];
  fp = fopen( objfile, "r" );
  if( !fp ){
    printf( "%s - invalid OBJ file '%s'\n", __FUNCTION__, objfile );
    return 0;
  }
  strcpy( obj->name, objfile );

  obj->num_verts = 0;
  obj->num_uvs = 0;
  obj->num_norms = 0;
  obj->num_faces = 0;
  while( 1 ){
    memset( line, 0, sizeof(line) );
    if( NULL == fgets( line, sizeof(line), fp ) )
      break;

    if( 'v' == line[0] &&
        ' ' == line[1] )
      obj->num_verts++;
    else
    if( 'v' == line[0] &&
        'n' == line[1] )
      obj->num_norms++;
    else
    if( 'v' == line[0] &&
        't' == line[1] )
      obj->num_uvs++;
    else
    if( 'f' == line[0] &&
        ' ' == line[1] )
      obj->num_faces++;
  }
  fseek( fp, 0, SEEK_SET );

  obj->verts = malloc( obj->num_verts * sizeof(WaveFront_vec3_t) );
  if( obj->num_uvs )
    obj->uvs = malloc( obj->num_uvs * sizeof(WaveFront_vec2_t) );
  if( obj->num_norms )
    obj->norms = malloc( obj->num_norms * sizeof(WaveFront_vec3_t) );
  obj->faces = malloc( obj->num_faces * sizeof(WaveFront_face_t) );

  int vi  = 0;
  int vni = 0;
  int vti = 0;
  int fi  = 0;
  while( 1 ){
    memset( line, 0, sizeof(line) );
    if( NULL == fgets( line, sizeof(line), fp ) )
      break;

    if( 'v' == line[0] &&
        ' ' == line[1] ){
      char *ptr = &line[2];
      while( *ptr == ' ' )
        ptr++;
      WaveFront_vec3_t *v = &obj->verts[vi++];
      sscanf( ptr, "%f %f %f", &v->x, &v->y, & v->z );
      v->x *= scale;
      v->y *= scale;
      v->z *= scale;
    }
    else
    if( 'v' == line[0] &&
        'n' == line[1] ){
      char *ptr = &line[3];
      while( *ptr == ' ' )
        ptr++;
      WaveFront_vec3_t *n = &obj->norms[vni++];
      sscanf( ptr, "%f %f %f", &n->x, &n->y, & n->z );
    }
    else
    if( 'v' == line[0] &&
        't' == line[1] ){
      char *ptr = &line[3];
      while( *ptr == ' ' )
        ptr++;
      WaveFront_vec2_t *t = &obj->uvs[vti++];
      sscanf( ptr, "%f %f", &t->x, &t->y );
    }
    else
    if( 'f' == line[0] &&
        ' ' == line[1] ){
      WaveFront_face_t *f = &obj->faces[fi++];
      char *ptr = &line[2];
      while( *ptr == ' ' )
        ptr++;
      char *tok = strtok( ptr, " \n" );
      parse_indices( tok, &f->vs[0], &f->ts[0], &f->ns[0] );
      tok = strtok( NULL, " \n" );
      parse_indices( tok, &f->vs[1], &f->ts[1], &f->ns[1] );
      tok = strtok( NULL, " \n" );
      parse_indices( tok, &f->vs[2], &f->ts[2], &f->ns[2] );
    }
  }

  fclose( fp );
  fp = NULL;

  printf( "%s - OBJ '%s' loaded: %u verts, %d normals, %u tex-coords, %u faces\n",
          __FUNCTION__,
          obj->name,
          obj->num_verts,
          obj->num_norms,
          obj->num_uvs,
          obj->num_faces );
  return 1;
}
