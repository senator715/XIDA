#pragma once

#include <Windows.h>
#include <cstdio>
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <filesystem>
#include <iostream>

typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long      ul64;
typedef unsigned long long u64;

#if defined(_M_AMD64)
typedef u64 uptr;
#else
typedef u32 uptr;
#endif

#define P(s, ...) SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15 ); printf(s, __VA_ARGS__)
#define PC(s, c, ...) SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), c ); printf(s, __VA_ARGS__); SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15 );

#define CPU_X86 0x014C
#define CPU_X64 0x8664

#include "utils.hpp"
