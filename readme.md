# RK Vec

An STB-style single header vector implementation for C.

The vector is stored on the heap with length and capacity followed by all items,
with the vector represented by a pointer to the items. This allows for indexing
into the vector just like an array.

## Usage

A vector variable is created with `vec_t(type)` with `type` as the type of the
items to store and a call to `vec_new()`. The vector must be freed with `vec_free()`.

Items are added to the vector with `vec_push`, that takes a pointer to the vector and the value to push.

The pushed items can then be accessed with `vec[index]`.

```C
vec_t(int) vec = vec_new();
vec_push(&vec, 123);
printf("%d\n", vec[0]);
vec_free(&vec);
```

The vector can also be created with an initializer list:

```C
vec_t(float) vec = vec_from(float, { 1.0, 2.0, 3.0 });
```

## See also

There are many existing useful vector libraries for C:

- https://github.com/nothings/stb/blob/master/stb_ds.h
- https://github.com/rxi/vec
