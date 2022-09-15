#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef bool b8;
typedef u32 b32;

#if defined(_WIN32)
#define SNOW_WINDOWS
#ifndef _WIN64
#error "Only 64 bit is supported"
#endif
#elif defined(__linux__)
#define SNOW_LINUX
#elif defined(__APPLE__)
#define SNOW_MACOS
#endif

static_assert(sizeof(u8) == 1, "u8 is not 1 byte");