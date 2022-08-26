#pragma once

#include "base/lai_memory.h"
#include "base/log.h"
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

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darray_create(type)                                                    \
  _darray_create<type>(DARRAY_DEFAULT_CAPACITY, sizeof(type))

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

template <typename T> T *_darray_create(u64 length, u64 stride) {
  u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 array_size = length * stride;
  u64 *new_array =
      (u64 *)lai_allocate(header_size + array_size, MEMORY_TAG_ARRAY);
  lai_set_memory(new_array, 0, header_size + array_size);
  new_array[DARRAY_CAPACITY] = length;
  new_array[DARRAY_LENGTH] = 0;
  new_array[DARRAY_STRIDE] = stride;
  return (T *)(new_array + DARRAY_FIELD_LENGTH);
}

template <typename T> void _darray_destroy(T *array) {
  u64 *header = (u64 *)array - DARRAY_FIELD_LENGTH;
  u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 total_size =
      header_size + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
  lai_free(header, total_size, MEMORY_TAG_DARRAY);
}

template <typename T> u64 _darray_field_get(T *array, u64 field) {
  u64 *header = (u64 *)array - DARRAY_FIELD_LENGTH;
  return header[field];
}

template <typename T> void _darray_field_set(T *array, u64 field, u64 value) {
  u64 *header = (u64 *)array - DARRAY_FIELD_LENGTH;
  header[field] = value;
}

template <typename T> T *_darray_resize(T *array) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  T *temp = _darray_create<T>((DARRAY_RESIZE_FACTOR * darray_capacity(array)),
                              stride);
  lai_copy_memory(temp, array, length * stride);
  _darray_field_set(temp, DARRAY_LENGTH, length);
  _darray_destroy(array);
  return temp;
}

template <typename T> void _darray_pop(T *array, T *dest) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  u64 addr = (u64)array;
  addr += ((length - 1) * stride);
  lai_copy_memory(dest, (T *)addr, stride);
  _darray_field_set(array, DARRAY_LENGTH, length - 1);
}

template <typename T>
T *_darray_insert_at(T *array, u64 index, const T *value_ptr) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (index >= length) {
    LAI_LOG_ERROR("Index out of bounds! length: %i, index: %i", length, index);
    return array;
  }
  if (length >= darray_capacity(array)) {
    array = _darray_resize(array);
  }

  u64 addr = (u64)array;

  if (index != length - 1) {
    lai_copy_memory((T *)(addr + ((index + 1) * stride)),
                    (T *)(addr + (index * stride)), stride * (length - index));
  }

  lai_copy_memory((T *)(addr + (index * stride)), value_ptr, stride);

  _darray_field_set(array, DARRAY_LENGTH, length + 1);
  return array;
}

template <typename T> T *_darray_pop_at(T *array, u64 index, T *dest) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (index >= length) {
    LAI_LOG_ERROR("Index out of bounds! length: %i, index: %i", length, index);
    return array;
  }

  u64 addr = (u64)array;
  lai_copy_memory(dest, (T *)(addr + (index * stride)), stride);

  if (index != length - 1) {
    lai_copy_memory((T *)(addr + (index * stride)),
                    (T *)(addr + ((index + 1) * stride)),
                    stride * (length - index));
  }

  _darray_field_set(array, DARRAY_LENGTH, length - 1);
  return array;
}

template <typename T> T *_darray_push(T *array, T *value_ptr) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (length >= darray_capacity(array)) {
    array = _darray_resize(array);
  }

  u64 addr = (u64)array;
  addr += (length * stride);
  lai_copy_memory((void *)addr, value_ptr, stride);
  _darray_field_set(array, DARRAY_LENGTH, length + 1);
  return array;
}