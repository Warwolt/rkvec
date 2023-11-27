#ifndef RK_VECTOR_H
#define RK_VECTOR_H

// An STB-style header only vector implementation.
//
// In order to support the []-operator, we use a trick where a vector of
// some type T is represented as a pointer T*, where the data is stored on
// the heap with metadata in memory right before the elements.
//
// Given a variable `vec_t(T) vec`, we would have the following layout:
//
// +------+----------+--------+--------+----- ... -+
// | length | capacity | item 0 | item 1 | item ...  |
// +------+----------+--------+--------+----- ... -+
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
#define vec_free(vec) free(_vec_header(*(vec)))
#define vec_push(vec, ...)                                                                                \
	do {                                                                                                  \
		_rk_header_t* header = _vec_header(*(vec));                                                       \
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

static inline _rk_header_t* _vec_header(void* items) {
	return (_rk_header_t*)((uint8_t*)items - sizeof(_rk_header_t));
}

static inline void* _rkvec_new(void) {
	void* data = _rkvec_malloc(sizeof(_rk_header_t));
	_rk_header_t* meta = (_rk_header_t*)(data);
	if (meta) {
		meta->capacity = 0;
		meta->length = 0;
	}

	return (void*)((uint8_t*)(data) + sizeof(_rk_header_t));
}

#endif /* RK_VECTOR_H */
