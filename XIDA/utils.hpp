#pragma once

static i32  arg_count;
static i8** arg_array;
static i8*  ida_directory;

namespace utils {
  inline i8* get_arg( const std::string& name ) {
    if ( arg_count < 1 || arg_array == nullptr )
      return nullptr;

    for ( auto i = 0; i < arg_count; i++ ) {
      auto* const str = arg_array[ i ];

      if ( strcmp( str, name.c_str( ) ) == 0 && i + 1 <= arg_count ) {
        printf( "[!] arg %s: \"%s\"\n", str, arg_array[ i + 1 ] );
        return arg_array[ i + 1 ];
      }
    }

    return nullptr;
  }

  inline std::string replace( std::string main_str, const std::string& search, const std::string& replace ) {
    u64 pos = 0;

    while ( ( pos = main_str.find( search, pos ) ) != std::string::npos ) {
      main_str.replace( pos, search.length( ), replace );
      pos += replace.length( );
    }

    return main_str;
  }

  inline bool file_exists( const std::string& file_name ) {
    auto* const handle = CreateFileA( file_name.c_str( ), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

    if ( handle == nullptr ) {
      CloseHandle( handle );
      return false;
    }

    const auto len = static_cast<i32>( GetFileSize( handle, nullptr ) );

    CloseHandle( handle );

    return len > 0;
  }

  inline u8* read_file( const std::string& file_name, u32 len, i32& file_len ) {

    auto* const handle = CreateFileA( file_name.c_str( ), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

    if ( handle == nullptr || len < 1 ) {
      CloseHandle( handle );
      return nullptr;
    }

    file_len = static_cast<i32>( GetFileSize( handle, nullptr ) );

    if ( file_len < 0 || len > file_len ) {
      CloseHandle( handle );
      return nullptr;
    }

    const auto buffer = static_cast<u8*>( malloc( len ) );
    memset( buffer, 0, len );

    u32 read_bytes;

    ReadFile( handle, buffer, len, reinterpret_cast<ul64*>( &read_bytes ), nullptr );

    CloseHandle( handle );

    return static_cast<u8*>( buffer );
  }

  inline bool write_file( const std::string& file_name, void* buffer, const u32 len ) {
    auto* const handle = CreateFileA( file_name.c_str( ), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );

    if ( handle == nullptr )
      return false;

    u32 wrote_bytes;
    WriteFile( handle, buffer, len, reinterpret_cast<ul64*>( &wrote_bytes ), nullptr );

    CloseHandle( handle );
    return true;
  }

  inline std::string format( const i8* format, ... ) {
    va_list list;
    va_start( list, format );

    i8 buffer[ 4096 ] = { 0 };
    vsnprintf( buffer, 4096, format, list );

    va_end( list );

    return std::string( buffer );
  }
}
