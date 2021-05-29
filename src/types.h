// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>
// Provides nominal type definitions.

#ifndef CUTIL_TYPES_H_INCLUDED
#define CUTIL_TYPES_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

// Owned memory.
//
// This looks similar to array
// but contents are not guaranteed to be initialized.
struct RawMemory {
	void *ptr;
	size_t capacity;
	size_t element_size;
};

// Abstraction of memory allocator.
struct Allocator {
	// Allocate a memory for a number of items. (`calloc`)
	//
	// len: count of elements to allocate. Can be zero.
	// size: size of an element in bytes. Typically `sizeof(T)`. Can be zero.
	//
	// return: newly-allocated memory. Contents are zeroed. Capacity is
	// *larger than or* equal to `count`.
	//
	// Result must be `free`-ed exactly once.
	struct RawMemory (*allocate)(size_t capacity, size_t element_size);

	// `realloc`
	struct RawMemory (*reallocate)(struct RawMemory mem, size_t new_capacity);

	// Deallocate a memory.
	//
	// mem: memory that is returned by this allocator's `allocate`.
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

// Borrowed (not owned), read-only span of UTF-8 string.
//
// Invariants:
//
// - Contents must be valid as UTF-8. (Not checked.)
// - Allocation unit (pointed by `ptr`) must contain at least one NULL byte
//    behind the span. (Not checked.) This invariant implies
//    dereferencing `ptr` is always okay.
//
// Remarks:
//
// - For arbitrary binary data, use Vector instead.
struct Str {
	unsigned char const *ptr;
	size_t len;
};

// Owned span for UTF-8 string.
//
// Invariants:
//
// - Same as str; and
// - either `len == 0` (non-allocated) or `len < capacity && ptr[len] == '\0'`
// (allocated).
struct String {
	unsigned char *ptr;
	size_t len;
	size_t capacity;
};

#endif
