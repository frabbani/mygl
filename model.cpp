#include "model.h"
#include "utils/log.h"
#include "utils/miniz.h"

namespace mygl {

std::map< std::string, std::shared_ptr< Model > > namedModels;

bool Model::loadZipped( void *zipContent, uint32_t size, std::string_view name_ ) {
  mz_zip_archive zip;
  memset( &zip, 0, sizeof ( zip ) );
  if( !mz_zip_reader_init_mem( &zip, zipContent, size, 0 ) ){
    utils::logout( "error: invalid model '%s'", name.data() );
    return false;
  }

  name = std::string( name_ );
  uint32_t numFiles = mz_zip_reader_get_num_files( &zip );
  for( uint32_t i = 0; i < numFiles; i++ ){
    if( name.find( "mesh.txt" ) != std::string::npos ){

    }
  }
  return meshVbo && trisIbo && textures.size() > 0;

}

MyGL_Str64 MyGL_Model_loadFromMem( const char *name, void *data, uint32_t size ) {
  MyGL_Str64 s = MyGL_str64( "" );
  mygl::Model model;
  if( !model.loadZipped( data, size, name ) ){
    return s;
  }
  return MyGL_str64( model.name.c_str() );
}

}
