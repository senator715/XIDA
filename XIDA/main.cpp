#include "link.hpp"

bool has_ida() {
  return ida_directory != nullptr && utils::file_exists( utils::format( "%s\\ida.exe", ida_directory ) );
}

bool has_idaq64() {
  return ida_directory != nullptr && utils::file_exists( utils::format( "%s\\idaq64.exe", ida_directory ) );
}

bool has_ida64() {
  return ida_directory != nullptr && utils::file_exists( utils::format( "%s\\ida64.exe", ida_directory ) );
}

bool is_ida_present_in_ida_directory() {
  return ida_directory != nullptr && has_ida( ) && ( has_ida64( ) || has_idaq64( ) );
}

bool run_open() {
  auto* const open = utils::get_arg( "-open" );

  if ( open == nullptr )
    return false;

  if ( !is_ida_present_in_ida_directory( ) ) {
    P( "[-] " );
    PC( "-ida directory is invalid\n", 12 );
    system( "pause" );
    return false;
  }

  i32   file_size = 0;
  auto* file      = utils::read_file( open, 0x1000, file_size );

  u16 cpu_mode = 0;
  if ( file != nullptr && file_size >= 0x1000 ) {
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>( file );

    if ( dos != nullptr ) {
      if ( dos->e_magic == 0x5A4D ) {
        const auto e_lfanew = reinterpret_cast<IMAGE_NT_HEADERS64*>( file + dos->e_lfanew );

        if ( e_lfanew != nullptr ) {
          cpu_mode = e_lfanew->FileHeader.Machine;
        }
      }
    }

    free( file );
  }

  if ( cpu_mode == CPU_X86 || cpu_mode == CPU_X64 ) {
    P( "[!] " );
    PC( "Architecture: %s\n", 14, cpu_mode == CPU_X86 ? "x86" : "x64" );
  }
  else {
    P( "[-] " );
    PC( "Architecture cannot be determined\n", 12 );

    P( "[!] " );
    PC( "1 = x86\n", 15 );
    P( "[!] " );
    PC( "2 = x64\n", 15 );
    while ( true ) {
      P( "[?] " );
      PC( "Please select architecture: ", 14 );

      // This code is fucking ugly and retarded but I was unable to get an integer version working due to a weird loop bug possibly caused by compiler settings
      std::string input;
      if ( !( std::cin >> input ) ) {
        input.clear( );
        std::cin.clear( );
        continue;
      }

      if ( input == "1" || input == "2" ) {
        cpu_mode = input == "1" ? CPU_X86 : CPU_X64;
        break;
      }
    }
  }

  if ( cpu_mode == CPU_X86 )
    system( utils::format( R"(start "" "%s\ida.exe" "%s")", ida_directory, open ).c_str( ) );
  else {
    if ( has_ida64( ) )
      system( utils::format( R"(start "" "%s\ida64.exe" "%s")", ida_directory, open ).c_str( ) );
    else
      system( utils::format( R"(start "" "%s\idaq64.exe" "%s")", ida_directory, open ).c_str( ) );
  }

  return true;
}

bool run_uninstall() {
  auto* const uninstall = utils::get_arg( "-uninstall" );

  if ( uninstall == nullptr )
    return false;

  P( "[!] Running un-installation script\n" );

  std::string registry_file = R"(Windows Registry Editor Version 5.00
    [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA64]
    [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA]
    [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithXIDA]
  )";

  utils::write_file( "install.reg", ( void* )registry_file.c_str( ), registry_file.size( ) );

  P( "[!] Running compiled registry script\n" );

  system( R"(regedit /s "%cd%\install.reg")" );
  system( R"(del "%cd%\install.reg")" );

  return true;
}

bool run_install() {
  auto* const install = utils::get_arg( "-install" );

  if ( install == nullptr )
    return false;

  P( "[!] Running installation script\n" );

  Sleep( 1000 );

  if ( !is_ida_present_in_ida_directory( ) ) {
    P( "[-] " );
    PC( "-ida does not contain a valid IDA installation\n", 12 );
    system( "pause" );
    return false;
  }

  // perform installation
  {
    const auto safe_dir = utils::replace( ida_directory, "\\", "\\\\" );

    // install registry keys
    {
      P( "[!] Compiling registry script\n" );

      std::string registry_file = R"(Windows Registry Editor Version 5.00
        [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA64]
        [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA]
        [-HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithXIDA]
        
        ; IDA64
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA64]
        @="Open with IDA64"
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA64\command]
        @="\"!_IDA_DIR_!\\!_IDA_64_EXE_!\" \"%1\""
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA64]
        "Icon"="!_IDA_DIR_!\\!_IDA_64_EXE_!,0"
        
        ; IDA
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA]
        @="Open with IDA"
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA\command]
        @="\"!_IDA_DIR_!\\!_IDA_EXE_!\" \"%1\""
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithIDA]
        "Icon"="!_IDA_DIR_!\\!_IDA_EXE_!,0"
        
        ; XIDA
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithXIDA]
        @="Open with XIDA"
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithXIDA\command]
        @="\"!_IDA_DIR_!\\!_XIDA_EXE_!\" -ida \"!_IDA_DIR_!\" -open \"%1\""
        [HKEY_CURRENT_USER\Software\Classes\*\shell\OpenwithXIDA]
        "Icon"="!_IDA_DIR_!\\!_IDA_EXE_!,0"
      )";

      // !_IDA_DIR_! = IDA DIRECTORY
      // !_IDA_64_EXE_! = IDA64.EXE
      // !_IDA_EXE_! = IDA.EXE
      // !_XIDA_EXE_! = XIDA.EXE

      registry_file = utils::replace( registry_file, "!_IDA_DIR_!", safe_dir );
      registry_file = utils::replace( registry_file, "!_IDA_64_EXE_!", "ida64.exe" );
      registry_file = utils::replace( registry_file, "!_IDA_EXE_!", "ida.exe" );
      registry_file = utils::replace( registry_file, "!_XIDA_EXE_!", "XIDA.exe" );

      utils::write_file( "install.reg", ( void* )registry_file.c_str( ), registry_file.size( ) );

      P( "[!] Running compiled registry script\n" );

      system( R"(regedit /s "%cd%\install.reg")" );
      system( R"(del "%cd%\install.reg")" );
    }
  }

  system( "pause" );

  return true;
}

bool run_arguments() {
  P( "[!] Arguments:\n" );

  // -input
  {
    P( "[!] \t-open\t\t<Binary file>\t" );
    PC( "Opens a file determining the architecture automatically\n", 14 );
  }

  // -ida
  {
    P( "[!] \t-ida\t\t<IDA directory>\t" );
    PC( "Sets the IDA Directory\n", 14 );
  }

  // -install
  {
    P( "[!] \t-install\t1\t\t" );
    PC( "Installs the context menu options for the specific IDA Directory\n", 14 );
  }

  // -uninstall
  {
      P("[!] \t-uninstall\t1\t\t");
      PC("Uninstalls the context menu options\n", 14);
  }

  system( "pause" );

  return true;
}

i32 main( const i32 argc, i8* argv[ ] ) {
  arg_count = argc;
  arg_array = argv;

  system( "cls" );

  // get ida directory
  {
    auto ida = utils::get_arg( "-ida" );
    if ( ida != nullptr )
      ida_directory = ida;
  }

  P( "[!] XIDA - %s\n", utils::replace( __DATE__, "  ", " " ).c_str( ) );
  {
    if ( run_uninstall( ) )
      return 0;

    if ( run_install( ) )
      return 0;

    if ( run_open( ) )
      return 0;

    if ( run_arguments( ) )
      return 0;
  }
}
