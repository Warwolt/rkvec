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
// | size | capacity | item 0 | item 1 | item ...  |
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

/* Public API --------------------------------------------------------------- */
typedef struct {
	size_t size;
	size_t capacity;
} _rkvec_header_t;

#define vec_t(TYPE) TYPE*
#define vec_new(TYPE) (TYPE*)_rkvec_new()
#define vec_free(vec) free(_vec_header(*(vec)))
#define vec_push(vec, item)                                                                           \
	do {                                                                                              \
		_rkvec_header_t* header = _vec_header(*(vec));                                                \
		bool needs_reallocation = false;                                                              \
		if (header->capacity == 0) {                                                                  \
			needs_reallocation = true;                                                                \
			header->capacity = RK_VECTOR_INITIAL_CAPACITY;                                            \
		} else if (header->size >= header->capacity) {                                                \
			needs_reallocation = true;                                                                \
			header->capacity *= 2;                                                                    \
		}                                                                                             \
		if (needs_reallocation) {                                                                     \
			header = realloc(header, sizeof(_rkvec_header_t) + sizeof(*(vec)[0]) * header->capacity); \
			*(vec) = (void*)((uint8_t*)(header) + sizeof(_rkvec_header_t));                           \
		}                                                                                             \
		*(vec)[header->size] = item;                                                                  \
		header->size += 1;                                                                            \
	} while (0)

typedef vec_t(int) vec_int_t;

/* Implementation ---------------------------------------------------------- */
static inline _rkvec_header_t* _vec_header(void* items) {
	return (_rkvec_header_t*)((uint8_t*)items - sizeof(_rkvec_header_t));
}

static inline void* _rkvec_new(void) {
	void* data = malloc(sizeof(_rkvec_header_t));
	if (!data) {
		fprintf(stderr, "error: allocation failed in _rkvec_new, aborting.");
		abort();
	}

	_rkvec_header_t* meta = (_rkvec_header_t*)(data);
	if (meta) {
		meta->capacity = 0;
		meta->size = 0;
	}

	return (void*)((uint8_t*)(data) + sizeof(_rkvec_header_t));
}

#endif /* RK_VECTOR_H */
