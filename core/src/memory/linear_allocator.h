#pragma once

#include "defines.h"

struct linear_allocator {
  u64 total_size;
  u64 allocated;

  void *memory;
  bool owns_memory;
};

void linear_allocator_create(u64 total_size, void *memory,
                             linear_allocator *out_allocator);
void linear_allocator_destroy(linear_allocator *out_allocator);

void *linear_allocator_allocate(linear_allocator *allocator, u64 size);
void linear_allocator_free_all(linear_allocator *allocator);