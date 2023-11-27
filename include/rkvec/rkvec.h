#ifndef RK_VECTOR_H
#define RK_VECTOR_H

// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

// An STB-style header only vector implementation.
//
// In order to support the []-operator, we use a trick where a vector of
// some type T is represented as a pointer T*, where the data is stored on
// the heap with metadata in memory right before the elements.
//
// Given a variable `vec_t(T) vec`, we would have the following layout:
//
// +--------+----------+--------+--------+------   -+
// | length | capacity | item 0 | item 1 | item ... |
// +--------+----------+--------+--------+------   -+
//                        ^
//                       vec
//
// We can push items using `vec_push`, and access items with `vec[i]`:
//
//   void example(void) {
//       vec_t(int) vec = vec_new();
//       vec_push(&vec, 123);
//       printf("%d\n", vec[0]);
//   }

#include <stdint.h>

#ifndef RK_VECTOR_INITIAL_CAPACITY
#define RK_VECTOR_INITIAL_CAPACITY 10
#endif

/* Vector API --------------------------------------------------------------- */
typedef struct {
	size_t length;
	size_t capacity;
} _rk_header_t;

#define vec_t(TYPE) TYPE*
#define vec_new() _rkvec_new()
#define vec_from(type, ...) \
	(vec_t(type)) _rkvec_from_array(sizeof(type), (type[])__VA_ARGS__, sizeof((type[])__VA_ARGS__) / sizeof(type))
#define vec_free(vec) free(_rkvec_header(*(vec)))
#define vec_push(vec, ...)                                                                                \
	do {                                                                                                  \
		_rk_header_t* header = _rkvec_header(*(vec));                                                     \
		if (header->length == header->capacity) {                                                         \
			header->capacity = header->capacity == 0 ? RK_VECTOR_INITIAL_CAPACITY : 2 * header->capacity; \
			header = _rkvec_realloc(header, sizeof(_rk_header_t) + sizeof(*(vec)[0]) * header->capacity); \
			*(vec) = (void*)((uint8_t*)(header) + sizeof(_rk_header_t));                                  \
		}                                                                                                 \
		*(vec)[header->length] = __VA_ARGS__;                                                             \
		header->length += 1;                                                                              \
	} while (0)

typedef vec_t(char*) vec_str_t;
typedef vec_t(int) vec_int_t;
typedef vec_t(char) vec_char_t;
typedef vec_t(float) vec_float_t;
typedef vec_t(double) vec_double_t;

/* Vector implementation ---------------------------------------------------- */
static inline void* _rkvec_malloc(size_t num_bytes) {
	void* ptr = malloc(num_bytes);
	if (!ptr) {
		fprintf(stderr, "_rkvec_malloc: realloc failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

static inline void* _rkvec_realloc(void* ptr, size_t num_bytes) {
	ptr = realloc(ptr, num_bytes);
	if (!ptr) {
		fprintf(stderr, "_rkvec_realloc: realloc failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

static inline _rk_header_t* _rkvec_header(void* items) {
	return (_rk_header_t*)((uint8_t*)items - sizeof(_rk_header_t));
}

static inline void* _rkvec_new(void) {
	_rk_header_t* header = _rkvec_malloc(sizeof(_rk_header_t));
	if (header) {
		header->capacity = 0;
		header->length = 0;
	}

	return (void*)((uint8_t*)(header) + sizeof(_rk_header_t));
}

static inline void* _rkvec_from_array(size_t type_size, void* items, size_t num_items) {
	_rk_header_t* header = _rkvec_malloc(sizeof(_rk_header_t) + type_size * num_items);
	header->capacity = num_items * 2;
	header->length = num_items;

	void* vec = (void*)((uint8_t*)(header) + sizeof(_rk_header_t));
	memcpy(vec, items, type_size * num_items);

	return vec;
}

#endif /* RK_VECTOR_H */
