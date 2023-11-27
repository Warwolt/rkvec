#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "rkvec/rkvec.h"

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
	TEST(pushed_number_can_be_accessed) {
		vec_int_t vec = vec_new(int);
		vec_push(&vec, 123);
		EXPECT_EQ(vec[0], 123);
		vec_free(&vec);
	}

	printf("All tests passed\n");
	return 0;
}
