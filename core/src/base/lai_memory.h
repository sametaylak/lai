#pragma once

#include "defines.h"

enum memory_tag {
  MEMORY_TAG_UNKNOWN,
  MEMORY_TAG_ARRAY,
  MEMORY_TAG_DARRAY,
  MEMORY_TAG_DICT,
  MEMORY_TAG_RING_QUEUE,
  MEMORY_TAG_BST,
  MEMORY_TAG_STRING,
  MEMORY_TAG_APPLICATION,
  MEMORY_TAG_JOB,
  MEMORY_TAG_TEXTURE,
  MEMORY_TAG_MATERIAL_INSTANCE,
  MEMORY_TAG_RENDERER,
  MEMORY_TAG_GAME,
  MEMORY_TAG_TRANSFORM,
  MEMORY_TAG_ENTITY,
  MEMORY_TAG_ENTITIY_NODE,
  MEMORT_TAG_SCENE,
  MEMORY_TAG_MAX_TAGS,
};

void initialize_memory();
void shutdown_memory();

void *lai_allocate(u64 size, memory_tag tag);
void lai_free(void *block, u64 size, memory_tag tag);
void *lai_zero_memory(void *block, u64 size);
void *lai_copy_memory(void *destination, const void *source, u64 size);
void *lai_set_memory(void *destination, i32 value, u64 size);

char *get_memory_usage();