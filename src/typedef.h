// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>
// Provides type aliases.

#ifndef CUTIL_TYPEDEF_H_INCLUDED
#define CUTIL_TYPEDEF_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intptr_t isize; // ssize_t is not available?

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

// UTF-8 code unit.
typedef unsigned char c8;
// UTF-16 code unit.
typedef uint16_t c16;
// UTF-32 code unit. (Unicode code point.)
typedef uint32_t c32;

typedef struct RawMemory RawMemory;
typedef struct Allocator Allocator;
typedef struct Span Span;
typedef struct Array Array;
typedef struct Vector Vector;
typedef struct Str Str;
typedef struct String String;

#endif
