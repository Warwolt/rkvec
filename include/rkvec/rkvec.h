/*
------------------------------------------------------------------------------
Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

// rkvec.h
//
// This is a fork from the stb dynamic array implementation from:
// https://github.com/nothings/stb/blob/master/stb_ds.h
//
// The purpose of the fork is to change the API to be closer to a typical vector
// interface from other langauges.

// Vectors
//
//   Non-function interface:
//
//     Access the i'th item of a vector 'foo' of type T, vec_t(T) foo:
//       T val = foo[i]
//
//   Functions (actually macros)
//
//     vec_new:
//       vec_t(T) vec_new();
//         Declare an empty vector of type T.
//
//     vec_free:
//       void vec_free(vec_t(T) vec);
//         Frees the vector.
//
//     vec_from:
//       vec_t(T) vec_from(T, T[] array);
//         Creates a vector from an array literal.
//         e.g. `vec_t(int) vec = vec_from(int, { 1, 2, 3 });`
//
//     vec_len:
//       ptrdiff_t vec_len(vec_t(T) vec);
//         Returns the number of elements in the vector.
//
//     vec_lenu:
//       size_t vec_lenu(vec_t(T) vec);
//         Returns the number of elements in the vector as an unsigned type.
//
//     vec_pop:
//       T vec_pop(vec_t(T) vec)
//         Removes the final element of the vector and returns it.
//
//     vec_put:
//       T vec_put(vec_t(T) vec, T b);
//         Appends the item b to the end of vector a. Returns b.
//
//     vec_insert:
//       T vec_insert(vec_t(T) vec, int p, T b);
//         Inserts the item b into the middle of vector a, into a[p],
//         moving the rest of the vector over. Returns b.
//
//     vec_insertn:
//       void vec_insertn(vec_t(T) vec, int p, int n);
//         Inserts n uninitialized items into vector a starting at a[p],
//         moving the rest of the vector over.
//
//     vec_addn_ptr:
//       T* vec_addn_ptr(vec_t(T) vec, int n)
//         Appends n uninitialized items onto vector at the end.
//         Returns a pointer to the first uninitialized item added.
//
//     vec_addn_index:
//       size_t vec_addn_index(vec_t(T) vec, int n)
//         Appends n uninitialized items onto vector at the end.
//         Returns the index of the first uninitialized item added.
//
//     vec_delete:
//       void vec_delete(vec_t(T) vec, int p);
//         Deletes the element at a[p], moving the rest of the vector over.
//
//     vec_deleten:
//       void vec_deleten(vec_t(T) vec, int p, int n);
//         Deletes n elements starting at a[p], moving the rest of the vector over.
//
//     vec_delete_swap:
//       void vec_delete_swap(vec_t(T) vec, int p);
//         Deletes the element at a[p], replacing it with the element from
//         the end of the vector. O(1) performance.
//
//     vec_set_len:
//       void vec_set_len(vec_t(T) vec, int n);
//         Changes the length of the vector to n. Allocates uninitialized
//         slots at the end if necessary.
//
//     vec_set_cap:
//       size_t vec_set_cap(vec_t(T) vec, int n);
//         Sets the length of allocated storage to at least n. It will not
//         change the length of the vector.
//
//     vec_cap:
//       size_t vec_cap(vec_t(T) vec);
//         Returns the number of total elements the vector can contain without
//         needing to be reallocated.
//
// NOTES
//
//   * These data structures are realloc'd when they grow, and the macro
//     "functions" write to the provided pointer. This means: (a) the pointer
//     must be an lvalue, and (b) the pointer to the data structure is not
//     stable, and you must maintain it the same as you would a realloc'd
//     pointer. For example, if you pass a pointer to a dynamic vector to a
//     function which updates it, the function must return back the new
//     pointer to the caller. This is the price of trying to do this in C.
//
//   * You iterate over the contents of a dynamic vector using vec_len:
//
//       for (i=0; i < vec_len(foo); ++i)
//          ... foo[i] ...
//
//   * All operations except vec_insert/vec_delete are O(1) amortized, but individual
//     operations can be slow, so these data structures may not be suitable
//     for real time use. Vectors double in capacity as needed, so
//     elements are copied an average of once.
//
// CREDITS stb_ds.h
//
//   Sean Barrett -- library, idea for dynamic array API/implementation
//   Per Vognsen  -- idea for hash table API/implementation
//   Rafael Sachetto -- arrpop()
//   github:HeroicKatora -- arraddn() reworking
//
//   Bugfixes:
//     Andy Durdin
//     Shane Liesegang
//     Vinh Truong
//     Andreas Molzer
//     github:hashitaku
//     github:srdjanstipic
//     Macoy Madson
//     Andreas Vennstrom
//     Tobias Mansfield-Williams

#ifndef RKVEC_H
#define RKVEC_H

#include <stddef.h>
#include <string.h>

#ifndef RK_NO_SHORT_NAMES
#define vec_t rk_vec_t
#define vec_new rk_vec_new
#define vec_from rk_vec_from
#define vec_len rk_vec_len
#define vec_lenu rk_vec_lenu
#define vec_put rk_vec_push
#define vec_push rk_vec_push
#define vec_pop rk_vec_pop
#define vec_free rk_vec_free
#define vec_addn_ptr rk_vec_addn_ptr
#define vec_addn_index rk_vec_addn_index
#define vec_set_len rk_vec_set_len
#define vec_last rk_vec_last
#define vec_insert rk_vec_insert
#define vec_insertn rk_vec_insertn
#define vec_delete rk_vec_delete
#define vec_deleten rk_vec_deleten
#define vec_delete_swap rk_vec_delete_swap
#define vec_cap rk_vec_cap
#define vec_set_cap rk_vec_set_cap

#endif

#if defined(RK_REALLOC) && !defined(RK_FREE) || !defined(RK_REALLOC) && defined(RK_FREE)
#error "You must define both RK_REALLOC and RK_FREE, or neither."
#endif
#if !defined(RK_REALLOC) && !defined(RK_FREE)
#include <stdlib.h>
#define RK_REALLOC(c, p, s) realloc(p, s)
#define RK_FREE(c, p) free(p)
#endif

#ifdef _MSC_VER
#define RK_NOTUSED(v) (void)(v)
#else
#define RK_NOTUSED(v) (void)sizeof(v)
#endif

///////////////
//
// Everything below here is implementation details
//

extern void* rk_vec_growf(void* a, size_t elem_size, size_t addlen, size_t min_cap);
extern void rk_vec_freef(void* a);

#if defined(__GNUC__) || defined(__clang__)
#define RK_HAS_TYPEOF
#ifdef __cplusplus
//#define RK_HAS_LITERAL_ARRAY  // this is currently broken for clang
#endif
#endif

#if !defined(__cplusplus)
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define RK_HAS_LITERAL_ARRAY
#endif
#endif

// this macro takes the address of the argument, but on gcc/clang can accept rvalues
#if defined(RK_HAS_LITERAL_ARRAY) && defined(RK_HAS_TYPEOF)
#if __clang__
#define RK_ADDRESSOF(typevar, value) ((__typeof__(typevar)[1]) { value }) // literal array decays to pointer to value
#else
#define RK_ADDRESSOF(typevar, value) ((typeof(typevar)[1]) { value }) // literal array decays to pointer to value
#endif
#else
#define RK_ADDRESSOF(typevar, value) &(value)
#endif

#define RK_OFFSETOF(var, field) ((char*)&(var)->field - (char*)(var))

#define rk_header(t) ((rk_vector_header*)(t)-1)
#define rk_temp(t) rk_header(t)->temp
#define rk_temp_key(t) (*(char**)rk_header(t)->hash_table)

#define rk_vec_t(type) type*
#define rk_vec_new() NULL
#define rk_vec_from(type, ...) (rk_vec_t(type)) rk_vec_from_array((type[])__VA_ARGS__, sizeof((type[])__VA_ARGS__) / sizeof(type), sizeof(type))
#define rk_vec_set_cap(vec, n) (rk_vec_grow(vec, 0, n))
#define rk_vec_set_len(vec, n) ((rk_vec_cap(vec) < (size_t)(n) ? rk_vec_set_cap((vec), (size_t)(n)), 0 : 0), (vec) ? rk_header(vec)->length = (size_t)(n) : 0)
#define rk_vec_cap(vec) ((vec) ? rk_header(vec)->capacity : 0)
#define rk_vec_len(vec) ((vec) ? (ptrdiff_t)rk_header(vec)->length : 0)
#define rk_vec_lenu(vec) ((vec) ? rk_header(vec)->length : 0)
#define rk_vec_push(vec, ...) (rk_vec_maybegrow(vec, 1), (vec)[rk_header(vec)->length++] = (__VA_ARGS__))
#define rk_vec_pop(vec) (rk_header(vec)->length--, (vec)[rk_header(vec)->length])
#define rk_vec_addn(vec, n) ((void)(rk_vec_addn_index(vec, n))) // deprecated, use one of the following instead:
#define rk_vec_addn_ptr(vec, n) (rk_vec_maybegrow(vec, n), (n) ? (rk_header(vec)->length += (n), &(vec)[rk_header(vec)->length - (n)]) : (vec))
#define rk_vec_addn_index(vec, n) (rk_vec_maybegrow(vec, n), (n) ? (rk_header(vec)->length += (n), rk_header(vec)->length - (n)) : rk_vec_len(vec))
#define rk_vec_addnoff rk_vec_addn_index
#define rk_vec_last(vec) ((vec)[rk_header(vec)->length - 1])
#define rk_vec_free(vec) ((void)((vec) ? RK_FREE(NULL, rk_header(vec)) : (void)0), (vec) = NULL)
#define rk_vec_delete(vec, i) rk_vec_deleten(vec, i, 1)
#define rk_vec_deleten(vec, i, n) (memmove(&(vec)[i], &(vec)[(i) + (n)], sizeof *(vec) * (rk_header(vec)->length - (n) - (i))), rk_header(vec)->length -= (n))
#define rk_vec_delete_swap(vec, i) ((vec)[i] = rk_vec_last(vec), rk_header(vec)->length -= 1)
#define rk_vec_insertn(vec, i, n) (rk_vec_addn((vec), (n)), memmove(&(vec)[(i) + (n)], &(vec)[i], sizeof *(vec) * (rk_header(vec)->length - (n) - (i))))
#define rk_vec_insert(vec, i, ...) (rk_vec_insertn((vec), (i), 1), (vec)[i] = (__VA_ARGS__))

#define rk_vec_maybegrow(vec, n) ((!(vec) || rk_header(vec)->length + (n) > rk_header(vec)->capacity) ? (rk_vec_grow(vec, n, 0), 0) : 0)

#define rk_vec_grow(vec, b, c) ((vec) = rk_vec_growf((vec), sizeof *(vec), (b), (c)))

typedef struct
{
	size_t length;
	size_t capacity;
	void* hash_table;
	ptrdiff_t temp;
} rk_vector_header;

#endif // RKVEC_H

//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#ifdef RK_VECTOR_IMPLEMENTATION
#include <assert.h>
#include <string.h>

#ifndef RK_ASSERT
#define RK_ASSERT_WAS_UNDEFINED
#define RK_ASSERT(x) ((void)0)
#endif

#ifdef RK_STATISTICS
#define RK_STATS(x) x
size_t rk_vector_grow;
#else
#define RK_STATS(x)
#endif

//
// rk_vec implementation
//

//int *prev_allocs[65536];
//int num_prev;

void* rk_vec_growf(void* vec, size_t elem_size, size_t addlen, size_t min_cap) {
	rk_vector_header temp = { 0 }; // force debugging
	void* b;
	size_t min_len = rk_vec_len(vec) + addlen;
	(void)sizeof(temp);

	// compute the minimum capacity needed
	if (min_len > min_cap)
		min_cap = min_len;

	if (min_cap <= rk_vec_cap(vec))
		return vec;

	// increase needed capacity to guarantee O(1) amortized
	if (min_cap < 2 * rk_vec_cap(vec))
		min_cap = 2 * rk_vec_cap(vec);
	else if (min_cap < 4)
		min_cap = 4;

	//if (num_prev < 65536) if (vec) prev_allocs[num_prev++] = (int *) ((char *) vec+1);
	//if (num_prev == 2201)
	//  num_prev = num_prev;
	b = RK_REALLOC(NULL, (vec) ? rk_header(vec) : 0, elem_size * min_cap + sizeof(rk_vector_header));
	//if (num_prev < 65536) prev_allocs[num_prev++] = (int *) (char *) b;
	b = (char*)b + sizeof(rk_vector_header);
	if (vec == NULL) {
		rk_header(b)->length = 0;
		rk_header(b)->hash_table = 0;
		rk_header(b)->temp = 0;
	} else {
		RK_STATS(++rk_vector_grow);
	}
	rk_header(b)->capacity = min_cap;

	return b;
}

void* rk_vec_from_array(void* array, size_t array_len, size_t elem_size) {
	void* vec = vec_new();

	vec = rk_vec_growf(vec, elem_size, 0, array_len);
	memcpy(vec, array, array_len * elem_size);

	return vec;
}

void rk_vec_freef(void* vec) {
	RK_FREE(NULL, rk_header(vec));
}

#endif /* RK_VECTOR_IMPLEMENTATION */
