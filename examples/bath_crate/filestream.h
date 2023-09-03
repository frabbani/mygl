#pragma once

#include "libs.h"

typedef enum {
  FS_UNOPENED = 0,
  FS_STREAMING,
  FS_EOF,
  FS_ERROR,
}file_stream_status_t;

typedef struct{
  file_stream_status_t status;
  FILE *fp;
  size_t size;
  size_t pos;
  size_t read;
  char buffer[1024];
  char name[256];
}file_stream_t;

void file_stream_init( file_stream_t *fs, const char *filename, int close ){
  if( close && fs->fp )
    fclose( fs->fp );

  strcpy( fs->name, filename );
  fs->status = FS_UNOPENED;
  fs->fp  = NULL;
  fs->size = fs->pos = fs->read = 0;
}

char file_stream_get_char( void *params ){
  file_stream_t *fs = params;
  if( fs->status == FS_ERROR || fs->status == FS_EOF ){
    return '\0';
  }

  if( FS_UNOPENED == fs->fp ){
    fs->fp = fopen( fs->name, "r" );
    if( !fs->fp ){
      printf( "%s - FAILED to open file '%s'\n", __FUNCTION__, fs->name );
      fs->status = FS_ERROR;
      return '\0';
    }
    fseek( fs->fp, 0, SEEK_END );
    fs->size = ftell( fs->fp );
    if( 0 == fs->size ){
      printf( "%s - INVALID FILE SIZE for file '%s'!\n", __FUNCTION__, fs->name );
      fs->status = FS_ERROR;
      fclose( fs->fp );
      fs->fp = NULL;
      return '\0';
    }
    fs->status = FS_STREAMING;
    fseek( fs->fp, 0, SEEK_SET );
    printf( "%s - file '%s' opened for streaming (size=%d)\n", __FUNCTION__, fs->name, (int)fs->size );
    fs->pos = fs->read = 0;
  }

  if( FS_STREAMING == fs->status ){
    if( fs->pos == fs->read ){
      fs->pos = 0;
      fs->read = fread( fs->buffer, 1, sizeof(fs->buffer), fs->fp );
      if( 0 == fs->read ){
        fs->status = feof( fs->fp ) ? FS_EOF : FS_ERROR;
        fclose( fs->fp );
        fs->fp = NULL;
        printf( "%s - file '%s' closed\n", __FUNCTION__, fs->name );
        return '\0';
      }
    }
    return fs->buffer[ fs->pos++ ];
  }

  return '\0';
}
