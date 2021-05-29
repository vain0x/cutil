// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>

#ifndef CUTIL_H_INCLUDED
#define CUTIL_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./types.h"

// -----------------------------------------------
// i32
// -----------------------------------------------

int i32_compare(int32_t l, int32_t r);

// -----------------------------------------------
// usize
// -----------------------------------------------

int usize_compare(size_t l, size_t r);

size_t usize_min(size_t l, size_t r);

// -----------------------------------------------
// struct RawMemory
// -----------------------------------------------

struct RawMemory raw_memory_new(size_t element_size);

// Resize a memory.
//
// If capacity is same, do nothing.
// Shrink if smaller. Extend if larger. Free if 0.
//
// While memory may move, contents are preserved.
// Extended part is zero-filled.
//
// The result capacity is guaranteed to be larger
// than or equal to `new_capacity`.
void raw_memory_resize(struct RawMemory *mem, size_t new_capacity,
                       struct Allocator *al);

// Grow a memory, at least exponentially.
//
// Capacity is already larger than or equal to `min_capacity`, do nothing.
// Extended part is zero-filled.
void raw_memory_grow(struct RawMemory *mem, size_t min_capacity,
                     struct Allocator *al);

// void raw_memory_free(struct RawMemory *mem, struct Allocator *al);

// -----------------------------------------------
// struct Str
// -----------------------------------------------

struct Str str_empty(void);

struct Str str_from_c_str(char const *c_str);

bool str_is_empty(struct Str s);

size_t str_len(struct Str s);

int str_compare(struct Str l, struct Str r);

bool str_equals(struct Str l, struct Str r);

struct Str str_skip(struct Str s, size_t start);

struct Str str_take(struct Str s, size_t len);

struct Str str_slice(struct Str s, size_t start, size_t end);

// -----------------------------------------------
// struct String
// -----------------------------------------------

struct RawMemory string_into_raw_memory(struct String s);

struct String string_empty(void);

void string_free(struct String *s, struct Allocator *al);

struct String string_from_str(struct Str s, struct Allocator *al);

struct String string_from_c_str(char const *c_str, struct Allocator *al);

struct Str string_as_str(struct String const *s);

size_t string_len(struct String const *s);

size_t string_capacity(struct String const *s);

void string_grow(struct String *string, size_t min_capacity,
                 struct Allocator *al);

void string_push_c8(struct String *s, unsigned char c, struct Allocator *al);

void string_push_str(struct String *string, struct Str str,
                     struct Allocator *al);

void string_append(struct String *string, struct String const *other,
                   struct Allocator *al);

#endif
