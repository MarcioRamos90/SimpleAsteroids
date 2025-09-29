#if !define(PROGRAM_MAIN_H)
#define _USE_MATH_DEFINES
/*
    Kind program main but only for the definitions and includes
*/
#include <stdint.h>
#include <assert.h>
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <cwchar>
#include <math.h>

#define internal static;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32 bool32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef wchar_t wchar;

const wchar* FileText;

#if EDITOR_DEVELOPMENT
    #define Assert(Expression) assert(Expression)
#else
    #define Assert(Expression)
#endif

#define PROGRAM_MAIN_H
#endif
