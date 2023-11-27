#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define RK_VECTOR_IMPLEMENTATION
#include "rkvec/rkvec.h"

typedef struct {
	int x;
	int y;
} point_t;

#define TEST(name) \
	printf("[test] %s\n", #name);

#define EXPECT_EQ(lhs, rhs)                                                                                                   \
	{                                                                                                                         \
		const int lhs_val = lhs;                                                                                              \
		const int rhs_val = rhs;                                                                                              \
		bool condition = lhs_val == rhs_val;                                                                                  \
		if (!condition) {                                                                                                     \
			fprintf(stderr, "Assert failed: %s == %s\n    %s = %d\n    %s = %d\n", #lhs, #rhs, #lhs, lhs_val, #rhs, rhs_val); \
			exit(1);                                                                                                          \
		}                                                                                                                     \
	}

int main(void) {
	TEST(numbers_can_be_pushed) {
		vec_t(int) vec = vec_new();

		vec_push(vec, 123);

		EXPECT_EQ(vec[0], 123);

		vec_free(vec);
	}

	TEST(structs_can_be_pushed) {
		vec_t(point_t) vec = vec_new();

		vec_push(vec, (point_t) { .x = 1, .y = 2 });

		EXPECT_EQ(vec[0].x, 1);
		EXPECT_EQ(vec[0].y, 2);

		vec_free(vec);
	}

	TEST(vector_can_be_initialized_from_int_array) {
		vec_t(int) vec = vec_from(int, { 1, 2, 3 });

		EXPECT_EQ(vec[0], 1);
		EXPECT_EQ(vec[1], 2);
		EXPECT_EQ(vec[2], 3);

		vec_free(vec);
	}

	TEST(vector_can_be_initialized_from_struct_array) {
		vec_t(point_t) vec = vec_from(point_t, { { 1, 2 }, { 3, 4 }, { 5, 6 } });

		EXPECT_EQ(vec[0].x, 1);
		EXPECT_EQ(vec[1].x, 3);
		EXPECT_EQ(vec[2].x, 5);

		vec_free(vec);
	}

	printf("All tests passed\n");
	return 0;
}
