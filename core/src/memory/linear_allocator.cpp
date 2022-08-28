#include "memory/linear_allocator.h"
#include "base/lai_memory.h"
#include "base/log.h"

void linear_allocator_create(u64 total_size, void *memory,
                             linear_allocator *out_allocator) {
  if (out_allocator) {
    out_allocator->total_size = total_size;
    out_allocator->allocated = 0;
    out_allocator->owns_memory = memory == nullptr;

    if (memory) {
      out_allocator->memory = memory;
    } else {
      out_allocator->memory =
          lai_allocate(total_size, MEMORY_TAG_LINEAR_ALLOCATOR);
    }
  }
}

void linear_allocator_destroy(linear_allocator *out_allocator) {
  if (out_allocator) {
    if (out_allocator->owns_memory && out_allocator->memory) {
      lai_free(out_allocator->memory, out_allocator->total_size,
               MEMORY_TAG_LINEAR_ALLOCATOR);
    }
    out_allocator->memory = nullptr;
    out_allocator->total_size = 0;
    out_allocator->allocated = 0;
    out_allocator->owns_memory = false;
  }
}

void *linear_allocator_allocate(linear_allocator *allocator, u64 size) {
  if (allocator && allocator->memory) {
    if (allocator->allocated + size > allocator->total_size) {
      u64 remaining = allocator->total_size - allocator->allocated;
      LAI_LOG_ERROR("linear_allocator_allocate - Tried to allocate %lluB, only "
                    "%lluB remainig",
                    size, remaining);
      return nullptr;
    }

    void *block = (u8 *)allocator->memory + allocator->allocated;
    allocator->allocated += size;
    return block;
  }

  LAI_LOG_ERROR(
      "linear_allocate_allocate - Provided allocator not initialized");
  return nullptr;
}

void linear_allocator_free_all(linear_allocator *allocator) {
  if (allocator && allocator->memory) {
    allocator->allocated = 0;
    lai_zero_memory(allocator->memory, allocator->total_size);
  }
}