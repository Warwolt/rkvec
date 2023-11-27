# RK Vec

A fork of the dynamic array in [stb_ds](https://github.com/nothings/stb/blob/master/stb_ds.h).

The vector is stored on the heap with length and capacity followed by all items,
with the vector represented by a pointer to the items. This allows for indexing
into the vector just like an array.

## Usage

Add the `rkvec.h` header to your project, and in exactly _one_ file define the
symbol `RK_VECTOR_IMPLEMENTATION` and include `rkvec.h`:

```C
#define RK_VECTOR_IMPLEMENTATION
#include "rkvec/rkvec.h"
```

A vector variable is created with `vec_t(type)` with `type` as the type of the
items to store and a call to `vec_new()`. The vector must be freed with
`vec_free()`.

Items are added to the vector with `vec_push`, that takes a pointer to the
vector and the value to push.

The pushed items can then be accessed with `vec[index]`.

```C
vec_t(int) vec = vec_new();
vec_push(vec, 123);
printf("%d\n", vec[0]);
vec_free(vec);
```

## See also

There are many existing useful vector libraries for C:

- https://github.com/nothings/stb/blob/master/stb_ds.h
- https://github.com/rxi/vec
