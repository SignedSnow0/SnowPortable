#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

#ifndef NDEBUG
#define SNOW_DEBUG
#endif

static_assert(sizeof(u8) == 1, "u8 is not 1 byte");

typedef glm::vec2 vec2f;
typedef glm::vec3 vec3f;
typedef glm::vec4 vec4f;

typedef glm::ivec2 vec2i;
typedef glm::ivec3 vec3i;
typedef glm::ivec4 vec4i;

typedef glm::uvec2 vec2u;
typedef glm::uvec3 vec3u;
typedef glm::uvec4 vec4u;

typedef glm::mat2 mat2f;
typedef glm::mat3 mat3f;
typedef glm::mat4 mat4f;

typedef glm::quat quat;