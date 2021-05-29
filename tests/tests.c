// LICENSE: CC0-1.0 <https://github.com/vain0x/cutil/tree/main/LICENSE>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/cutil.h"
#include "../src/typedef.h"
#include "../src/types.h"

static RawMemory la_allocate(size_t capacity, size_t element_size) {
	if (capacity == 0 || element_size == 0) {
		return (RawMemory){
		    .ptr = NULL,
		    .capacity = capacity,
		    .element_size = element_size,
		};
	}

	void *ptr = calloc(capacity, element_size);
	if (!ptr) {
		fprintf(stderr, "error: Out of memory.\n");
		exit(1);
	}

	return (RawMemory){
	    .ptr = ptr,
	    .capacity = capacity,
	    .element_size = element_size,
	};
}

static void la_free(RawMemory mem) {
	if (mem.capacity != 0 && mem.element_size != 0) {
		free(mem.ptr);
	}
}

static RawMemory la_reallocate(RawMemory mem, size_t new_capacity) {
	size_t element_size = mem.element_size;

	if (new_capacity == 0) {
		la_free(mem);
		return (RawMemory){
		    .ptr = NULL,
		    .capacity = 0,
		    .element_size = element_size,
		};
	}

	if (mem.capacity == 0 || element_size == 0) {
		return la_allocate(new_capacity, element_size);
	}

	void *ptr = realloc(mem.ptr, new_capacity);
	return (RawMemory){
	    .ptr = ptr,
	    .capacity = new_capacity,
	    .element_size = mem.element_size,
	};
}

// Allocator backed by malloc/free.
static Allocator GLOBAL_ALLOCATOR = {
    .allocate = la_allocate,
    .reallocate = la_reallocate,
    .free = la_free,
};

static Allocator *const AL = &GLOBAL_ALLOCATOR;

// -----------------------------------------------
// Testing
// -----------------------------------------------

static size_t s_pass = 0;
static size_t s_fail = 0;

static void test_do_assert(bool cond, char const *file, size_t line) {
	if (cond) {
		s_pass++;
	} else {
		s_fail++;

		// yellow
		fprintf(stderr,
		        "[\x1B[33mERROR\x1B[0m] test: Assertion violated at %s:%d\n",
		        file, (int)line);
	}
}

static void test_do_assert_eq_i(isize actual, isize expected, char const *file,
                                size_t line) {
	if (actual == expected) {
		s_pass++;
	} else {
		s_fail++;

		// yellow
		fprintf(stderr,
		        "[\x1B[33mERROR\x1B[0m] test: Assertion violated at %s:%d\n  "
		        "actual = %zd\n  expected = %zd\n",
		        file, (int)line, actual, expected);
	}
}

static void test_do_assert_eq_u(usize actual, usize expected, char const *file,
                                size_t line) {
	if (actual == expected) {
		s_pass++;
	} else {
		s_fail++;

		// yellow
		fprintf(stderr,
		        "[\x1B[33mERROR\x1B[0m] test: Assertion violated at %s:%d.\n  "
		        "actual = %zu\n  expected = %zu\n",
		        file, (int)line, actual, expected);
	}
}

static void test_do_assert_eq_s(Str actual, Str expected, char const *file,
                                size_t line) {
	if (str_equals(actual, expected)) {
		s_pass++;
	} else {
		s_fail++;

		// yellow
		fprintf(stderr,
		        "[\x1B[33mERROR\x1B[0m] test: Assertion violated at %s:%d.\n  "
		        "actual = %*s\n  expected = %*s\n",
		        file, (int)line, (int)actual.len, actual.ptr, (int)expected.len,
		        expected.ptr);
	}
}

#define assert_that(COND) (test_do_assert((COND), __FILE__, __LINE__))
#define assert_eq_i(ACTUAL, EXPECTED)                                          \
	(test_do_assert_eq_i((ACTUAL), (EXPECTED), __FILE__, __LINE__))
#define assert_eq_u(ACTUAL, EXPECTED)                                          \
	(test_do_assert_eq_u((ACTUAL), (EXPECTED), __FILE__, __LINE__))
#define assert_eq_s(ACTUAL, EXPECTED)                                          \
	(test_do_assert_eq_s((ACTUAL), (EXPECTED), __FILE__, __LINE__))

// return exit code
static int test_finish(void) {
	size_t total_count = s_pass + s_fail;

	if (total_count == 0) {
		// yellow
		fprintf(stderr, "[\x1B[33mERROR\x1B[0m] test: No assertion.\n");
		return 1;
	}

	if (s_fail >= 1) {
		// yellow
		fprintf(stderr,
		        "[\x1B[33mERROR\x1B[0m] test: Failed.\n        "
		        "test: total %zu / pass %zu / fail %zu\n",
		        total_count, s_pass, s_fail);
		return 1;
	}

	// cyan
	fprintf(stderr, "[\x1B[36mINFO\x1B[0m] test: OK\n       test: total %zu\n",
	        total_count);
	return 0;
}

// -----------------------------------------------
// Str
// -----------------------------------------------

static Str C(char const *c_str) { return str_from_c_str(c_str); }

static void test_str(void) {
	Str empty = str_empty();
	assert_eq_u(str_len(empty), 0);
	assert_that(str_is_empty(empty));

	Str hello = str_from_c_str("hello");
	assert_eq_u(str_len(hello), 5);
	assert_that(!str_is_empty(hello));

	{
		Str lo = str_skip(hello, 3);
		assert_eq_u(lo.len, 2);
	}

	{
		assert_eq_i(str_compare(C("a"), C("aa")), -1);
		assert_eq_i(str_compare(C("aa"), C("ab")), -1);
		assert_eq_i(str_compare(C("ba"), C("a")), 1);
		assert_eq_i(str_compare(str_skip(C("hello"), 3), C("lo")), 0);
	}

	{
		assert_eq_i(str_compare(str_slice(hello, 0, 99), hello), 0);
		assert_eq_i(str_compare(str_slice(hello, 99, 66), empty), 0);
		assert_eq_i(str_compare(str_slice(hello, 1, 4), C("ell")), 0);
	}
}

// -----------------------------------------------
// String
// -----------------------------------------------

static Str as_str(String const *s) { return string_as_str(s); }

static void test_string(void) {
	String local_s = string_empty();
	String *s = &local_s;

	assert_eq_i(str_compare(as_str(s), C("")), 0);

	string_push_c8(s, 'a', AL);
	assert_eq_i(str_compare(as_str(s), C("a")), 0);

	string_push_c8(s, 'b', AL);
	assert_eq_i(str_compare(as_str(s), C("ab")), 0);

	string_push_str(s, C("cd"), AL);
	assert_eq_i(str_compare(as_str(s), C("abcd")), 0);
}

// ===============================================

int main(void) {
	test_str();
	test_string();
	return test_finish();
}
