#pragma once

#include "defines.h"

/*
 *
 * Memory Layout
 * u64 capacity -> number of elements that can be held
 * u64 length   -> number of elements currently contained
 * u64 stride   -> size of each element in bytes
 * void* elements
 *
 * */

enum { DARRAY_CAPACITY, DARRAY_LENGTH, DARRAY_STRIDE, DARRAY_FIELD_LENGTH };

void *_darray_create(u64 length, u64 stride);
void _darray_destroy(void *array);

u64 _darray_field_get(void *array, u64 field);
void _darray_field_set(void *array, u64 field, u64 value);

void *_darray_resize(void *array);

void *_darray_push(void *array, const void *value_ptr);
void _darray_pop(void *array, void *dest);

void *_darray_insert_at(void *array, u64 index, const void *value_ptr);
void *_darray_pop_at(void *array, u64 index, void *dest);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darray_create(type)                                                    \
  _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define darray_reserve(type, capacity) _darray_create(capacity, sizeof(type))

#define darray_destroy(array) _darray_destroy(array)

#define darray_push(array, value)                                              \
  {                                                                            \
    auto temp = value;                                                         \
    array = _darray_push(array, &temp);                                        \
  }

#define darray_insert_at(array, index, value)                                  \
  {                                                                            \
    auto temp = value;                                                         \
    array = _darray_insert_at(array, index, &temp);                            \
  }

#define darray_pop(array, value_ptr) _darray_pop(array, value_ptr)
#define darray_pop_at(array, index, value_ptr)                                 \
  _darray_pop_at(array, index, value_ptr)

#define darray_clear(array) _darray_field_set(array, DARRAY_LENGTH, 0)
#define darray_length(array) _darray_field_get(array, DARRAY_LENGTH)
#define darray_capacity(array) _darray_field_get(array, DARRAY_CAPACITY)
#define darray_stride(array) _darray_field_get(array, DARRAY_STRIDE)
#define darray_length_set(array, value)                                        \
  _darray_field_set(array, DARRAY_LENGTH, value)
