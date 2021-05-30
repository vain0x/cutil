// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>
// Provides nominal type definitions.

#ifndef CUTIL_TYPES_H_INCLUDED
#define CUTIL_TYPES_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

// Owned memory.
//
// - Unlike array, contents may be uninitialized.
struct RawMemory {
	void *ptr;
	size_t capacity;
	size_t element_size;
};

// Abstraction of memory allocator.
struct Allocator {
	// Allocate a memory for a number of items. (`calloc`)
	//
	// capacity: count of elements to allocate. Can be zero.
	// elment_size: size of an element in bytes. Typically `sizeof(T)`. Can be
	// zero.
	//
	// return: newly-allocated memory. Contents are zero-filled. Capacity is
	// larger than or equal to `capacity`.
	//
	// Result must be `free`-ed exactly once.
	struct RawMemory (*allocate)(size_t capacity, size_t element_size);

	// `realloc`
	struct RawMemory (*reallocate)(struct RawMemory mem, size_t new_capacity);

	// Deallocate a memory.
	//
	// mem: a memory that is returned by this allocator's `allocate`.
	void (*free)(struct RawMemory mem);
};

// Read-only span.
struct Span {
	void const *ptr;
	size_t len;
	size_t element_size;
};

// Read-write span.
struct Array {
	void *ptr;
	size_t len;
	size_t element_size;
};

// Growable owned array.
struct Vector {
	void *ptr;
	size_t len;
	size_t element_size;
	size_t capacity;
};

// Read-only span of UTF-8 string.
//
// Invariants:
//
// - Contents must be valid as UTF-8. (Not checked.)
// - Allocation unit (pointed by `ptr`) must contain at least one NULL byte
//    behind the span. (Not checked.) This invariant implies
//    accessing to `ptr[len]` is always okay.
//
// Remarks:
//
// - For arbitrary binary data, use Vector instead.
struct Str {
	unsigned char const *ptr;
	size_t len;
};

// Owned, growable array for UTF-8 string.
//
// Invariants:
//
// - Contents must be valid as UTF-8. (Not checked.)
// - Either `len == 0` (non-allocated) or `len < capacity && ptr[len] == '\0'`
// (allocated).
struct String {
	unsigned char *ptr;
	size_t len;
	size_t capacity;
};

// Read-only view of unix-style path string.
struct PathStr {
	struct Str str;
};

// Owned, growable unix-style UTF-8 path string.
struct PathString {
	struct String string;
};

#endif
