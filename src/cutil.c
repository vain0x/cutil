// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>

#include "./cutil.h"
#include "./types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	MAX_CAPACITY = SIZE_MAX / 16,
	GROW_FACTOR = 2,
};

static void cutil_panic(char const *msg) {
	fprintf(stderr, "cutil_panic: %s\n", msg);
	exit(1);
}

static void dyn_assert(bool cond) {
	if (!cond) {
		cutil_panic("assertion violated");
	}
}

static void debug_assert(bool cond) {
	if (!cond) {
		cutil_panic("debug assertion violated");
	}
}

// -----------------------------------------------
// i32
// -----------------------------------------------

int i32_compare(int32_t l, int32_t r) {
	if (l == r) {
		return 0;
	}
	if (l < r) {
		return -1;
	}
	return 1;
}

// -----------------------------------------------
// usize
// -----------------------------------------------

int usize_compare(size_t l, size_t r) {
	if (l == r) {
		return 0;
	}
	if (l < r) {
		return -1;
	}
	return 1;
}

size_t usize_min(size_t l, size_t r) { return r < l ? r : l; }

// -----------------------------------------------
// Memory
// -----------------------------------------------

static int mem_compare(void const *l, void const *r, size_t len,
                       size_t element_size) {
	dyn_assert(l && r);
	return memcmp(l, r, len * element_size);
}

static void mem_copy_from(void *dest, void const *src, size_t len,
                          size_t element_size) {
	dyn_assert(dest && src);
	memmove(dest, src, len * element_size);
}

static void mem_clear(void *ptr, size_t start, size_t end,
                      size_t element_size) {
	dyn_assert(ptr);

	if (start <= end) {
		memset((unsigned char *)ptr + start * element_size, '\0', end - start);
	}
}

// -----------------------------------------------
// struct RawMemory
// -----------------------------------------------

struct RawMemory raw_memory_new(size_t element_size) {
	if (element_size == 0) {
		return (struct RawMemory){
		    .ptr = NULL,
		    .capacity = MAX_CAPACITY,
		    .element_size = 0,
		};
	}

	return (struct RawMemory){
	    .ptr = NULL,
	    .capacity = 0,
	    .element_size = element_size,
	};
}

void raw_memory_resize(struct RawMemory *mem, size_t new_capacity,
                       struct Allocator *al) {
	debug_assert(mem && al);

	size_t old_capacity = mem->capacity;
	size_t element_size = mem->element_size;

	if (old_capacity == new_capacity) {
		return;
	}

	if (new_capacity == 0) {
		if (element_size != 0) {
			al->free(*mem);
		}
		*mem = raw_memory_new(element_size);
		return;
	}

	if (old_capacity == 0) {
		*mem = al->allocate(new_capacity, element_size);
	} else {
		struct RawMemory other = al->reallocate(*mem, new_capacity);
		*mem = other;
	}

	// struct Allocator never returns null pointer.
	debug_assert(mem->ptr);

	// struct Allocator returns a memory that is larger than or equal to
	// requested capacity.
	dyn_assert(mem->capacity >= new_capacity);

	// Element size must not change.
	debug_assert(mem->element_size == element_size);

	// Clear newly allocated part.
	mem_clear(mem->ptr, old_capacity, mem->capacity, element_size);
}

void raw_memory_grow(struct RawMemory *mem, size_t min_capacity,
                     struct Allocator *al) {
	debug_assert(mem && al);

	if (mem->capacity >= min_capacity || mem->element_size == 0) {
		return;
	}

	size_t new_capacity = min_capacity;
	if (mem->capacity != 0) {
		if (new_capacity < mem->capacity * GROW_FACTOR) {
			new_capacity = mem->capacity * GROW_FACTOR;
		}
	}

	debug_assert(new_capacity > mem->capacity);
	raw_memory_resize(mem, new_capacity, al);
}

// static void raw_memory_free(struct RawMemory *mem, struct Allocator *al) {
// 	raw_memory_resize(mem, 0, al);
// }

// -----------------------------------------------
// struct Str
// -----------------------------------------------

struct Str str_empty(void) {
	return (struct Str){
	    .ptr = (unsigned char const *)"",
	    .len = 0,
	};
}

struct Str str_from_c_str(char const *c_str) {
	debug_assert(c_str);

	size_t len = strlen(c_str);
	if (len == 0) {
		return str_empty();
	}

	return (struct Str){
	    .ptr = (unsigned char const *)c_str,
	    .len = len,
	};
}

bool str_is_empty(struct Str s) { return *s.ptr == 0; }

size_t str_len(struct Str s) { return s.len; }

int str_compare(struct Str l, struct Str r) {
	size_t n = usize_min(l.len, r.len);
	int c = mem_compare(l.ptr, r.ptr, n, sizeof(unsigned char));
	if (c != 0) {
		return c < 0 ? -1 : 1;
	}

	return usize_compare(l.len, r.len);
}

bool str_equals(struct Str l, struct Str r) {
	return l.len == r.len &&
	       mem_compare(l.ptr, r.ptr, l.len, sizeof(unsigned char)) == 0;
}

struct Str str_skip(struct Str s, size_t start) {
	if (start >= s.len) {
		return str_empty();
	}

	return (struct Str){
	    .ptr = s.ptr + start,
	    .len = s.len - start,
	};
}

struct Str str_take(struct Str s, size_t len) {
	if (len >= s.len) {
		return str_empty();
	}

	return (struct Str){
	    .ptr = s.ptr,
	    .len = len,
	};
}

struct Str str_slice(struct Str s, size_t start, size_t end) {
	if (end > s.len) {
		end = s.len;
	}
	if (start >= end) {
		return str_empty();
	}
	return (struct Str){
	    .ptr = s.ptr + start,
	    .len = end - start,
	};
}

// -----------------------------------------------
// struct String
// -----------------------------------------------

// `struct String`s are null-terminated (redundantly)
// in case it's passed to a function that expects null-terminated string
// (unexpectedly).

struct RawMemory string_into_raw_memory(struct String s) {
	return (struct RawMemory){
	    .ptr = s.ptr,
	    .capacity = s.capacity,
	    .element_size = sizeof(unsigned char),
	};
}

struct String string_empty(void) {
	return (struct String){
	    .ptr = (unsigned char *)"",
	    .len = 0,
	    .capacity = 0,
	};
}

void string_free(struct String *s, struct Allocator *al) {
	debug_assert(s && al);

	if (s->capacity != 0) {
		al->free(string_into_raw_memory(*s));
		*s = string_empty();
	}
}

struct String string_from_str(struct Str s, struct Allocator *al) {
	debug_assert(al);

	if (s.len == 0) {
		return string_empty();
	}

	struct RawMemory mem = al->allocate(s.len + 1, sizeof(unsigned char));
	mem_copy_from(mem.ptr, s.ptr, s.len, sizeof(unsigned char));
	((unsigned char *)mem.ptr)[s.len] = 0;

	struct String string = {
	    .ptr = mem.ptr,
	    .len = s.len,
	    .capacity = mem.capacity,
	};
	return string;
}

struct String string_from_c_str(char const *c_str, struct Allocator *al) {
	return string_from_str(str_from_c_str(c_str), al);
}

struct Str string_as_str(struct String const *s) {
	return (struct Str){
	    .ptr = s->ptr,
	    .len = s->len,
	};
}

size_t string_len(struct String const *s) { return s->len; }

size_t string_capacity(struct String const *s) { return s->capacity; }

void string_grow(struct String *string, size_t min_capacity,
                 struct Allocator *al) {
	dyn_assert(string && al);

	if (min_capacity <= string->capacity) {
		return;
	}

	struct RawMemory mem = string_into_raw_memory(*string);
	raw_memory_grow(&mem, min_capacity, al);
	string->ptr = mem.ptr;
	string->capacity = mem.capacity;
	debug_assert(string->len < string->capacity);
}

void string_push_c8(struct String *s, unsigned char c, struct Allocator *al) {
	debug_assert(s && al);

	string_grow(s, s->len + 2, al);
	s->ptr[s->len] = c;
	s->len++;

	debug_assert(s->len < s->capacity);
	s->ptr[s->len] = '\0';
}

void string_push_str(struct String *string, struct Str str,
                     struct Allocator *al) {
	debug_assert(string && al);

	string_grow(string, string->len + str.len + 1, al);
	mem_copy_from(string->ptr + string->len, str.ptr, str.len,
	              sizeof(unsigned char));
	string->len += str.len;

	debug_assert(string->len < string->capacity);
	string->ptr[string->len] = '\0';
}

void string_append(struct String *string, struct String const *other,
                   struct Allocator *al) {
	string_push_str(string, string_as_str(other), al);
}

// -----------------------------------------------
// struct String functions
// -----------------------------------------------

// find, split, etc.

static bool str_occurs_at(struct Str str, size_t start, struct Str substr) {
	return str_equals(str_slice(str, start, start + substr.len), substr);
}

static bool str_starts_with(struct Str str, struct Str prefix) {
	return str_equals(str_take(str, prefix.len), prefix);
}

static bool str_ends_with(struct Str str, struct Str suffix) {
	return str.len >= suffix.len &&
	       str_equals(str_skip(str, str.len - suffix.len), suffix);
}

struct UsizeOption {
	bool ok;
	size_t value;
};

static struct UsizeOption str_rposition_c8(struct Str str, unsigned char c) {
	size_t i = str.len;
	while (i >= 1) {
		i--;
		if (str.ptr[i] == c) {
			return (struct UsizeOption){.ok = true, .value = i};
		}
	}
	return (struct UsizeOption){.ok = false};
}

// -----------------------------------------------
// struct PathStr
// -----------------------------------------------

struct PathStr path_str_new(struct Str str) {
	return (struct PathStr){.str = str};
}

// bool path_str_prev(struct PathStr path, struct PathStr *out) {
// 	struct Str str = path.str;

// 	struct StrDivision division;
// 	if (!str_divide_back_c8(str, '/', &division)) {
// 		return false;
// 	}

// 	str.ptr
// };

// struct PathSegments {
// 	bool is_absolute;
// 	struct Array segments;
// };

// struct Array path_str_split(struct PathStr str) {}

// Normalize a path string syntactically.
//
// Transformations:
//
// - Coalesce redundant separators: `foo//bar` -> `foo/bar`
// - Strip trailing separator: `foo/` -> `foo`
// - Drop `.` segments: `./foo` -> `foo`
// - Coalesce `..` segments: `foo/bar/..` -> `foo`
//
// Meaning of "syntactical normalization":
//
// - The result might not be normal in the sense of filesystem.
//    Path might be inaccessible or missing.
//    Different paths might map to the same file.
struct PathString path_str_norm(struct PathStr path) {}

struct PathSegment path_str_ancestors() {}
